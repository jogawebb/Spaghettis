
/* Copyright (c) 1997-2017 Miller Puckette and others. */

/* < https://opensource.org/licenses/BSD-3-Clause > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

#include "m_pd.h"
#include "m_core.h"
#include "s_system.h"
#include "g_graphics.h"

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#define METRO_DEFAULT_DELAY             1000.0

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

static t_class *metro_class;            /* Shared. */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

typedef struct _metro {
    t_object    x_obj;                  /* Must be the first. */
    double      x_delay;
    int         x_reentrantStart;
    int         x_reentrantStop;
    t_outlet    *x_outlet;
    t_clock     *x_clock;
    } t_metro;

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

static void metro_float (t_metro *, t_float);

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

static void metro_task (t_metro *x)
{
    x->x_reentrantStop  = 0;
    
    if (!x->x_reentrantStart) {
    //
    x->x_reentrantStart = 1;
    outlet_bang (x->x_outlet);
    //
    }
    
    if (!x->x_reentrantStop) { 
    //
    clock_delay (x->x_clock, x->x_delay);
    //
    }
    
    x->x_reentrantStart = 0;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

static void metro_bang (t_metro *x)
{
    metro_float (x, (t_float)1.0);
}

static void metro_float (t_metro *x, t_float f)
{
    if (f != 0.0) { metro_task (x); }
    else {
        clock_unset (x->x_clock);
    }
    
    x->x_reentrantStop = 1;
}

static void metro_floatDelay (t_metro *x, t_float f)
{
    if (f < 0.0) { error_invalid (sym_metro, sym_delay); }
    else {
        x->x_delay = (double)((f == 0.0) ? METRO_DEFAULT_DELAY : f);
    }
}

static void metro_stop (t_metro *x)
{
    metro_float (x, (t_float)0.0);
}

/* Note that float arguments are always passed at last. */

static void metro_unit (t_metro *x, t_symbol *unitName, t_float f)
{
    t_error err = clock_setUnitParsed (x->x_clock, f, unitName);
    
    if (err) {
        error_invalid (sym_metro, sym_unit); 
    }
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

static void *metro_new (t_symbol *s, int argc, t_atom *argv)
{
    t_float f          = atom_getFloatAtIndex (0, argc, argv);
    t_float unit       = atom_getFloatAtIndex (1, argc, argv);
    t_symbol *unitName = atom_getSymbolAtIndex (2, argc, argv);
        
    t_metro *x = (t_metro *)pd_new (metro_class);
    
    x->x_reentrantStart = 0;
    x->x_reentrantStop  = 0;
    
    x->x_clock  = clock_new ((void *)x, (t_method)metro_task);
    x->x_outlet = outlet_new (cast_object (x), &s_bang);
    
    inlet_new2 (x, &s_float);
    
    metro_floatDelay (x, f);
    
    if (unitName != &s_) { metro_unit (x, unitName, unit); }
    
    return x;
}

static void metro_free (t_metro *x)
{
    clock_free (x->x_clock);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

void metro_setup (void)
{
    t_class *c = NULL;
    
    c = class_new (sym_metro,
            (t_newmethod)metro_new,
            (t_method)metro_free,
            sizeof (t_metro),
            CLASS_DEFAULT,
            A_GIMME,
            A_NULL);
            
    class_addBang (c, (t_method)metro_bang);
    class_addFloat (c, (t_method)metro_float);
    
    class_addMethod (c, (t_method)metro_floatDelay, sym__inlet2,    A_FLOAT, A_NULL);
    class_addMethod (c, (t_method)metro_stop,       sym_stop,       A_NULL);
    class_addMethod (c, (t_method)metro_unit,       sym_unit,       A_FLOAT, A_SYMBOL, A_NULL);

    #if PD_WITH_LEGACY 
    
    class_addMethod (c, (t_method)metro_unit,       sym_tempo,      A_FLOAT, A_SYMBOL, A_NULL);
    
    #endif
        
    metro_class = c;
}

void metro_destroy (void)
{
    class_free (metro_class);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
