
/* Copyright (c) 1997-2019 Miller Puckette and others. */

/* < https://opensource.org/licenses/BSD-3-Clause > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

#include "../m_spaghettis.h"
#include "../m_core.h"
#include "../s_system.h"

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

struct _clocks {
    pthread_rwlock_t    x_lock;
    t_int32Atomic       x_offset;
    t_int32Atomic       x_maximum;
    int                 x_size;
    t_clock             **x_cache;
    t_pointerAtomic     *x_clocks;
    t_buffer            *x_garbage;
    };

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

#define CLOCKS_SIZE     65535           /* Arbitrary. */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

static t_clock clocks_empty;            /* Static. */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

int atomic_pointerCompareAndSwap        (void **, void *, t_pointerAtomic *);
int atomic_int32CompareAndSwap          (int32_t *, int32_t, t_int32Atomic *);

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

void        scheduler_setLogicalTime    (t_systime);
t_systime   clock_getExecuteTime        (t_clock *);
void        clock_setExecuteTime        (t_clock *, t_systime);
void        clock_increment             (t_clock *);
void        clock_decrement             (t_clock *);
void        clock_inhibit               (t_clock *);
void        clock_execute               (t_clock *);
int         clock_isGood                (t_clock *);

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

static void clocks_setMaximum (t_clocks *x, int i)
{
    int t, n;
    
    do {
    //
    t = PD_ATOMIC_INT32_READ (&x->x_maximum);
    n = PD_MAX (t, i);
    //
    } while (!atomic_int32CompareAndSwap (&t, n, &x->x_maximum));
}

static int clocks_fetchMaximum (t_clocks *x)
{
    int t = PD_ATOMIC_INT32_READ (&x->x_maximum);
    
    while (!atomic_int32CompareAndSwap (&t, 0, &x->x_maximum)) { }
    
    return t;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

void clocks_add (t_clocks *x, t_clock *c)
{
    t_error err = PD_ERROR;
    
    pthread_rwlock_rdlock (&x->x_lock);
    
        int i;

        for (i = 0; i < CLOCKS_SIZE; i++) {
        //
        void *t = PD_ATOMIC_POINTER_READ (x->x_clocks + i);
        
        if (t == NULL || (t == (void *)&clocks_empty && (i >= PD_ATOMIC_INT32_READ (&x->x_offset)))) {
            if (atomic_pointerCompareAndSwap (&t, (void *)c, x->x_clocks + i)) {
                clock_increment (c); clocks_setMaximum (x, i); err = PD_ERROR_NONE; break;
            }
        }
        //
        }
    
    pthread_rwlock_unlock (&x->x_lock);
    
    if (err) { PD_BUG; PD_ABORT (1); }
}

void clocks_remove (t_clocks *x, t_clock *c)
{
    if (clock_isSet (c)) {
    //
    pthread_rwlock_rdlock (&x->x_lock);
    
        int i;

        for (i = 0; i < CLOCKS_SIZE; i++) {
        //
        void *t = PD_ATOMIC_POINTER_READ (x->x_clocks + i);
        
        if (t == (void *)c) {
            if (atomic_pointerCompareAndSwap (&t, (void *)&clocks_empty, x->x_clocks + i)) {
                clock_decrement (c);
            }
            break;

        } else if (t == NULL) { break; }
        //
        }
    
    pthread_rwlock_unlock (&x->x_lock);
    //
    }
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

/* Clocks are temporary stored before to be freed. */
/* Unset a clock doesn't remove it from the clocks cached while executed. */
/* Theoritically a clock could trigger deletion of another one already in cached. */
/* Wait that all clocks have fired before to remove them. */

void clocks_destroy (t_clocks *x, t_clock *c)
{
    t_atom a;
    SET_CLOCK (&a, c);
    clocks_remove (x, c);
    clock_inhibit (c);
    PD_ASSERT (clock_isGood (c));
    buffer_appendAtom (x->x_garbage, &a);
}

void clocks_purge (t_clocks *x)
{
    int i, n = buffer_getSize (x->x_garbage);
    for (i = 0; i < n; i++) { t_clock *c = buffer_getClockAt (x->x_garbage, i); PD_MEMORY_FREE (c); }
    buffer_clear (x->x_garbage);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

static void clocks_tickCheck (t_clocks *x, t_systime systime, int i)
{
    void *t = PD_ATOMIC_POINTER_READ (x->x_clocks + i);
    
    if (t && t != (void *)&clocks_empty) {
    //
    t_clock *c     = (t_clock *)t;
    t_systime time = clock_getLogicalTime (c);
    
    if (time <= systime) {
        if (atomic_pointerCompareAndSwap (&t, (void *)&clocks_empty, x->x_clocks + i)) {
            clock_decrement (c);
            clock_setExecuteTime (c, time);
            x->x_cache[x->x_size++] = c;
            return;
        }
    }
    
    clocks_setMaximum (x, i);
    //
    }
}

static int clocks_tickCompare (const void *p1, const void *p2)
{
    t_float t1 = clock_getExecuteTime (*(t_clock **)p1);
    t_float t2 = clock_getExecuteTime (*(t_clock **)p2);
    
    if (t1 > t2) { return 1; } else if (t1 < t2) { return -1; }
    
    return 0;
}

static void clocks_tickExecute (t_clocks *x)
{
    if (x->x_size) {
    //
    int i;
    
    qsort (x->x_cache, x->x_size, sizeof (t_clock *), clocks_tickCompare);
    
    for (i = 0; i < x->x_size; i++) {
    //
    t_clock *c = x->x_cache[i]; x->x_cache[i] = NULL;
    scheduler_setLogicalTime (clock_getExecuteTime (c));
    clock_execute (c);
    //
    }
    //
    }
}

void clocks_tick (t_clocks *x, t_systime systime)
{
    pthread_rwlock_rdlock (&x->x_lock);
    
        int i = clocks_fetchMaximum (x);
    
        x->x_size = 0;
    
        /* To avoid ABA problem, don't allow to write over read area. */
    
        PD_ATOMIC_INT32_WRITE (i + 1, &x->x_offset);    PD_MEMORY_BARRIER;
    
            while (i >= 0) { clocks_tickCheck (x, systime, i); i--; }
    
        PD_ATOMIC_INT32_WRITE (0, &x->x_offset);        PD_MEMORY_BARRIER;
    
    pthread_rwlock_unlock (&x->x_lock);                 PD_MEMORY_BARRIER;
    
    clocks_tickExecute (x); clocks_purge (x);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

int clocks_clean (t_clocks *x)
{
    if (pthread_rwlock_trywrlock (&x->x_lock) == 0) {
    //
    int i, j = 0;
    
    for (i = 0; i < CLOCKS_SIZE; i++) {
    //
    void *t = *(x->x_clocks + i);
    
    if (t == NULL) { break; }
    else if (t && t != (void *)&clocks_empty) {
        if (i != j) { x->x_clocks[j] = t; }
        j++;
    }
    //
    }
    
    for (; j < CLOCKS_SIZE; j++) { x->x_clocks[j] = NULL; }
    
    pthread_rwlock_unlock (&x->x_lock);
    
    return 1;
    //
    }
    
    return 0;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

#if PD_WITH_DEADCODE

static char clocks_string[CLOCKS_SIZE + 1];     /* Static. */

char *clocks_debug (t_clocks *x, int n)
{
    pthread_rwlock_rdlock (&x->x_lock);
    
        int i;
    
        for (i = 0; i < CLOCKS_SIZE; i++) {
        //
        void *t = PD_ATOMIC_POINTER_READ (x->x_clocks + i);
        
        char c = '#'; if (t == (void *)&clocks_empty) { c = '.'; } else if (t) { c = 'X'; }
        
        clocks_string[i] = c;
        //
        }
    
        if (n) { clocks_string[PD_CLAMP (n, 0, CLOCKS_SIZE)] = 0; }
    
    pthread_rwlock_unlock (&x->x_lock);
    
    return clocks_string;
}

#endif // PD_WITH_DEADCODE

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

t_clocks *clocks_new (void)
{
    t_clocks *x = (t_clocks *)PD_MEMORY_GET (sizeof (t_clocks));
    
    pthread_rwlock_init (&x->x_lock, NULL);
    
    x->x_cache   = (t_clock **)PD_MEMORY_GET (sizeof (t_clock *) * CLOCKS_SIZE);
    x->x_clocks  = (t_pointerAtomic *)PD_MEMORY_GET (sizeof (t_pointerAtomic) * (CLOCKS_SIZE + 1));
    x->x_garbage = buffer_new();
    
    return x;
}

void clocks_free (t_clocks *x)
{
    clocks_purge (x);
    
    buffer_free (x->x_garbage);
    
    PD_MEMORY_FREE (x->x_clocks);
    PD_MEMORY_FREE (x->x_cache);
    
    pthread_rwlock_destroy (&x->x_lock);
    
    PD_MEMORY_FREE (x);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
