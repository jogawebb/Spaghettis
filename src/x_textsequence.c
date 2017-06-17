
/* Copyright (c) 1997-2017 Miller Puckette and others. */

/* < https://opensource.org/licenses/BSD-3-Clause > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

#include "m_pd.h"
#include "m_core.h"
#include "s_system.h"
#include "x_control.h"

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

void atom_copyAtomsExpandedWithArguments (t_atom *, int, t_atom *, int, t_glist *, int, t_atom *);

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

static t_class *textsequence_class;                 /* Shared. */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

typedef struct _textsequence {
    t_textclient        x_textclient;
    t_float             x_delay;
    int                 x_indexOfMessage;
    int                 x_waitNumberOfLeading;
    int                 x_waitCount;
    int                 x_isAutomatic;
    int                 x_isLooping;
    int                 x_argc;
    t_atom              *x_argv;
    t_symbol            *x_sendTo;
    t_symbol            *x_waitSymbol;
    t_outlet            *x_outletMain;
    t_outlet            *x_outletWait; 
    t_outlet            *x_outletEnd;
    t_clock             *x_clock;
    } t_textsequence;

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

static void textsequence_stop       (t_textsequence *);
static void textsequence_message    (t_textsequence *, t_float);

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

static int textsequence_proceedNeedToWait (t_textsequence *x, t_buffer *b, int start, int end) 
{
    if (end - start == 0)    { return 0; }
    else if (x->x_waitCount) { return 0; }
    else if (x->x_sendTo)    { return 0; }
    else {
        t_atom *first    = buffer_atomAtIndex (b, start);
        int waitOnNumber = IS_FLOAT (first)  && x->x_waitNumberOfLeading;
        int waitOnSymbol = IS_SYMBOL (first) && (GET_SYMBOL (first) == x->x_waitSymbol);
        
        return (waitOnNumber || waitOnSymbol);
    }
}

static int textsequence_proceedSetWaitRange (t_textsequence *x, t_buffer *b, int start, int end)
{
    if (IS_FLOAT (buffer_atomAtIndex (b, start))) {
        int n = start + x->x_waitNumberOfLeading;
        int i = start;
        n = (n < 0 ? PD_INT_MAX : n);
        n = PD_MIN (n, end);
        while (i < n && IS_FLOAT (buffer_atomAtIndex (b, i))) {
            i++; x->x_waitCount++;
        }
            
    } else {
        x->x_waitCount = end - start;
    }
    
    return (x->x_waitCount == (end - start));       /* Advance if nothing remains to output. */
}

static void textsequence_proceedOutWaitForSymbol (t_textsequence *x, t_buffer *b, int start, int end)
{
    if (x->x_isAutomatic && x->x_waitCount == 2 && IS_FLOAT (buffer_atomAtIndex (b, start + 1))) { 
        x->x_delay = GET_FLOAT (buffer_atomAtIndex (b, start + 1)); 
        
    } else {
        PD_ASSERT (x->x_outletWait);
        PD_ASSERT (x->x_waitCount);
        x->x_isAutomatic = 0;
        outlet_list (x->x_outletWait, x->x_waitCount - 1, buffer_atomAtIndex (b, start + 1));
    }
}

static void textsequence_proceedOutWaitForFloat (t_textsequence *x, t_buffer *b, int start, int end)
{
    if (x->x_isAutomatic && x->x_waitCount == 1) { 
        x->x_delay = GET_FLOAT (buffer_atomAtIndex (b, start)); 
        
    } else {
        PD_ASSERT (x->x_outletWait);
        PD_ASSERT (x->x_waitCount);
        x->x_isAutomatic = 0;
        outlet_list (x->x_outletWait, x->x_waitCount, buffer_atomAtIndex (b, start));
    }
}

static void textsequence_proceedOutWait (t_textsequence *x, t_buffer *b, int start, int end)
{
    int k = IS_FLOAT (buffer_atomAtIndex (b, start));
    
    x->x_sendTo    = NULL;
    x->x_isLooping = 0;

    if (k) { textsequence_proceedOutWaitForFloat (x, b, start, end);  } 
    else   { textsequence_proceedOutWaitForSymbol (x, b, start, end); }
}

static void textsequence_proceedOutContentMain (t_textsequence *x, t_atom *t, int count, t_atomtype type)
{
    /* Ensure that comma separated messages are well prepended. */
    /* For instance "toto a b c, 1 2;" results in "toto a b c" and "toto 1 2". */
        
    if (x->x_sendTo) {
        if (count) { memmove (t + 1, t, count * sizeof (t_atom)); }
        SET_SYMBOL (t, x->x_sendTo);
        count++;
    }
        
    if (type != A_COMMA) { x->x_sendTo = NULL; }
    else {
        if (!x->x_sendTo && count && IS_SYMBOL (t)) { x->x_sendTo = GET_SYMBOL (t); }
    }
        
    outlet_list (x->x_outletMain, count, t);
}

static void textsequence_proceedOutContentGlobal (t_textsequence *x, t_atom *t, int count, t_atomtype type)
{
    int shitfRight = 0;
    t_symbol *symSend = x->x_sendTo;
    
    PD_ASSERT (count > 0);
    
    if (!symSend) { if (IS_SYMBOL (t)) { symSend = GET_SYMBOL (t); } else { PD_BUG; } shitfRight = 1; }
    
    if (pd_hasThing (symSend)) {
    //    
    int n = count - shitfRight;
    t_atom *v = t + shitfRight;
    
    if (n > 0 && IS_SYMBOL (v)) { pd_message (pd_getThing (symSend), GET_SYMBOL (v), n - 1, v + 1); }
    else {
        pd_list (pd_getThing (symSend), n, v);
    }
    //
    }
    
    x->x_sendTo = (type != A_COMMA ? NULL : symSend);
}

static void textsequence_proceedOutContent (t_textsequence *x, 
    t_buffer *b, 
    int start, 
    int end, 
    t_atomtype type,
    int argc,
    t_atom *argv)
{
    t_glist *view = textclient_fetchView (&x->x_textclient);
        
    int count = end - start - x->x_waitCount;
    t_atom *a = buffer_atomAtIndex (b, start + x->x_waitCount);
    int size  = count + 1;
    t_atom *t = NULL;
    
    PD_ATOMS_ALLOCA (t, size);      /* Extra size reserved for possible labelling (see above). */
    
    if (argc) { atom_copyAtomsExpandedWithArguments (a, count, t, count, view, argc, argv);  }
    else {
        atom_copyAtomsExpanded (a, count, t, count, view);
    }
    
    if (x->x_outletMain) { textsequence_proceedOutContentMain (x, t, count, type); }
    else if (count > 0)  { textsequence_proceedOutContentGlobal (x, t, count, type); }
    
    PD_ATOMS_FREEA (t, size);
}

static void textsequence_proceed (t_textsequence *x, int argc, t_atom *argv)
{
    t_buffer *b = textclient_fetchBuffer (&x->x_textclient);
    int start, end;
    t_atomtype type;
        
    if (b && buffer_getMessageAtWithTypeOfEnd (b, x->x_indexOfMessage, &start, &end, &type)) { 
        
        int advance, needToWait = textsequence_proceedNeedToWait (x, b, start, end);
        
        if (needToWait) {
            advance = textsequence_proceedSetWaitRange (x, b, start, end); 
            textsequence_proceedOutWait (x, b, start, end);
        } else {
            advance = 1;
            textsequence_proceedOutContent (x, b, start, end, type, argc, argv);
        }
        
        if (advance) { x->x_waitCount = 0; x->x_indexOfMessage += 1; }
        
    } else { 
    
        if (!b) { error_undefined (sym_text__space__search, sym_text); }
        x->x_indexOfMessage = PD_INT_MAX;
        x->x_waitCount      = 0;
        x->x_isLooping      = 0;
        x->x_isAutomatic    = 0;
        x->x_sendTo         = NULL;
        outlet_bang (x->x_outletEnd);
    }
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

static void textsequence_task (t_textsequence *x)
{
    x->x_sendTo = NULL;
    x->x_delay  = (t_float)0.0;
    
    while (x->x_isAutomatic) {
        x->x_isLooping = 1; while (x->x_isLooping) { textsequence_proceed (x, x->x_argc, x->x_argv); }
        if (x->x_delay > 0.0) { break; }
    }
    
    if (x->x_isAutomatic) { clock_delay (x->x_clock, x->x_delay); }
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

static void textsequence_bang (t_textsequence *x)
{
    x->x_isLooping = 1; while (x->x_isLooping) { textsequence_proceed (x, x->x_argc, x->x_argv); }
}

static void textsequence_list (t_textsequence *x, t_symbol *s, int argc, t_atom *argv)
{
    x->x_isLooping = 1;
    
    while (x->x_isLooping) {
    //
    if (argc) { textsequence_proceed (x, argc, argv); }
    else {
        textsequence_proceed (x, x->x_argc, x->x_argv);
    }
    //
    }
}

static void textsequence_stop (t_textsequence *x)
{
    x->x_isLooping = 0; if (x->x_isAutomatic) { clock_unset (x->x_clock); x->x_isAutomatic = 0; }
}

static void textsequence_step (t_textsequence *x)
{
    textsequence_stop (x); textsequence_proceed (x, x->x_argc, x->x_argv);
}

static void textsequence_automatic (t_textsequence *x)
{
    x->x_sendTo = NULL;
    
    if (x->x_isAutomatic) { clock_unset (x->x_clock); } else { x->x_isAutomatic = 1; }
    
    textsequence_task (x);
}

static void textsequence_rewind (t_textsequence *x)
{
    textsequence_message (x, (t_float)0.0);
}

static void textsequence_message (t_textsequence *x, t_float f)
{
    t_buffer *b = textclient_fetchBuffer (&x->x_textclient);

    if (b) {
    
        x->x_indexOfMessage = (int)PD_ABS (f);
        x->x_waitCount = 0;
        x->x_sendTo = NULL;

    } else { error_undefined (sym_text__space__sequence, sym_text); }
}

static void textsequence_arguments (t_textsequence *x, t_symbol *s, int argc, t_atom *argv)
{
    int i;
    size_t oldSize = x->x_argc * sizeof (t_atom);
    size_t newSize = argc * sizeof (t_atom);
    
    x->x_argc = argc;
    x->x_argv = PD_MEMORY_RESIZE (x->x_argv, oldSize, newSize);
    
    for (i = 0; i < argc; i++) { x->x_argv[i] = argv[i]; }
}

/* Note that float arguments are always passed at last. */

static void textsequence_unit (t_textsequence *x, t_symbol *unitName, t_float f)
{
    t_error err = clock_setUnitParsed (x->x_clock, f, unitName);
    
    if (err) { 
        error_invalid (sym_text__space__sequence, sym_unit);
    }
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

void *textsequence_new (t_symbol *s, int argc, t_atom *argv)
{
    t_textsequence *x = (t_textsequence *)pd_new (textsequence_class);
        
    t_error err = textclient_init (&x->x_textclient, &argc, &argv);         /* It may consume arguments. */
    
    if (!err) {
    
        int hasWait = 0;
        int useGlobal = 0;
        
        while (argc > 0) {
        
            t_symbol *t = atom_getSymbolAtIndex (0, argc, argv);
        
            if (t == sym___dash__g || t == sym___dash__global) { 
                useGlobal = 1; argc--; argv++; 
                
            } else if (argc >= 2 && (t == sym___dash__w || t == sym___dash__wait)) {
                if (!x->x_waitSymbol && !x->x_waitNumberOfLeading) {
                    if (IS_SYMBOL (argv + 1)) { x->x_waitSymbol = atom_getSymbol (argv + 1); }
                    else {
                        x->x_waitNumberOfLeading = PD_MAX (0, (int)atom_getFloat (argv + 1));
                    }
                    argc -= 2; argv += 2;
                }
                
            } else if (argc >= 3 && (t == sym___dash__u || t == sym___dash__unit)) {
                textsequence_unit (x, atom_getSymbol (argv + 2), atom_getFloat (argv + 1));
                argc -= 3; argv += 3;
                
            } else {
                break;
            }
        }
        
        error__options (s, argc, argv);

        if (argc) { warning_unusedArguments (s, argc, argv); }
        
        hasWait = (useGlobal || x->x_waitSymbol || x->x_waitNumberOfLeading);
        
        x->x_indexOfMessage = 0;
        
        x->x_argc   = 0;
        x->x_argv   = (t_atom *)PD_MEMORY_GET (0);
        x->x_clock  = clock_new ((void *)x, (t_method)textsequence_task);
        
        if (useGlobal)  { x->x_waitNumberOfLeading = PD_INT_MAX; }
        if (!useGlobal) { x->x_outletMain = outlet_new (cast_object (x), &s_list); }
        if (hasWait)    { x->x_outletWait = outlet_new (cast_object (x), &s_list); }
        
        x->x_outletEnd = outlet_new (cast_object (x), &s_bang);
        
        if (TEXTCLIENT_ASPOINTER (&x->x_textclient)) {
            inlet_newPointer (cast_object (x), TEXTCLIENT_GETPOINTER (&x->x_textclient));
        } else {
            inlet_newSymbol (cast_object (x),  TEXTCLIENT_GETNAME    (&x->x_textclient));
        }
        
    } else {
        error_invalidArguments (sym_text__space__search, argc, argv);
        pd_free (cast_pd (x)); x = NULL;
    }
    
    return x;
}

static void textsequence_free (t_textsequence *x)
{
    if (x->x_argv)  { PD_MEMORY_FREE (x->x_argv); }
    if (x->x_clock) { clock_free (x->x_clock); }
    
    textclient_free (&x->x_textclient);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

void textsequence_setup (void)
{
    t_class *c = NULL;
    
    c = class_new (sym_text__space__sequence,
            (t_newmethod)textsequence_new,
            (t_method)textsequence_free,
            sizeof (t_textsequence),
            CLASS_DEFAULT,
            A_GIMME,
            A_NULL);
    
    class_addBang (c, (t_method)textsequence_bang);
    class_addList (c, (t_method)textsequence_list);
    
    class_addMethod (c, (t_method)textsequence_stop,        sym_stop,       A_NULL);
    class_addMethod (c, (t_method)textsequence_step,        sym_step,       A_NULL);
    class_addMethod (c, (t_method)textsequence_automatic,   sym_automatic,  A_NULL);
    class_addMethod (c, (t_method)textsequence_rewind,      sym_rewind,     A_NULL);
    class_addMethod (c, (t_method)textsequence_message,     sym_message,    A_FLOAT, A_NULL);
    class_addMethod (c, (t_method)textsequence_arguments,   sym_arguments,  A_GIMME, A_NULL);
    class_addMethod (c, (t_method)textsequence_unit,        sym_unit,       A_FLOAT, A_SYMBOL, A_NULL);
    
    #if PD_WITH_LEGACY
    
    class_addMethod (c, (t_method)textsequence_automatic,   sym_auto,       A_NULL);
    class_addMethod (c, (t_method)textsequence_arguments,   sym_args,       A_GIMME, A_NULL);
    class_addMethod (c, (t_method)textsequence_message,     sym_line,       A_FLOAT, A_NULL);
    class_addMethod (c, (t_method)textsequence_unit,        sym_tempo,      A_FLOAT, A_SYMBOL, A_NULL);
        
    #endif 

    class_setHelpName (c, sym_text);
    
    textsequence_class = c;
}

void textsequence_destroy (void)
{
    CLASS_FREE (textsequence_class);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
