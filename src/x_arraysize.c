
/* 
    Copyright (c) 1997-2016 Miller Puckette and others.
*/

/* < https://opensource.org/licenses/BSD-3-Clause > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

#include "m_pd.h"
#include "m_core.h"
#include "m_macros.h"
#include "g_graphics.h"
#include "x_control.h"

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

extern t_class *garray_class;

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

static t_class *arraysize_class;                /* Shared. */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

typedef struct _arraysize {
    t_arrayclient   x_arrayclient;              /* Must be the first. */
    t_outlet        *x_outlet;
    } t_arraysize;

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

static void arraysize_bang (t_arraysize *x)
{
    t_array *a = arrayclient_fetchArray (&x->x_arrayclient);
    
    if (a) { outlet_float (x->x_outlet, array_getSize (a)); }
    else { 
        error_undefined (sym_array__space__size, sym_array); 
    }
}

static void arraysize_float (t_arraysize *x, t_float f)
{
    t_array *a = arrayclient_fetchArray (&x->x_arrayclient);
    
    if (a) {
    //
    int n = PD_MAX (1, (int)f);
    
    if (ARRAYCLIENT_ASPOINTER (&x->x_arrayclient)) {
        array_resizeAndRedraw (a, arrayclient_fetchView (&x->x_arrayclient), n);
    } else {
        garray_resizeWithInteger (arrayclient_fetchOwnerIfName (&x->x_arrayclient), n);
    }
    //
    } else { error_undefined (sym_array__space__size, sym_array); }
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

void *arraysize_new (t_symbol *s, int argc, t_atom *argv)
{
    t_arraysize *x = (t_arraysize *)pd_new (arraysize_class);
    
    t_error err = arrayclient_init (&x->x_arrayclient, &argc, &argv);      /* It may consume arguments. */
    
    if (!err) {
    
        error__options (sym_array__space__size, argc, argv);
        
        if (argc) { warning_unusedArguments (sym_array__space__size, argc, argv); }
        
        if (ARRAYCLIENT_ASPOINTER (&x->x_arrayclient)) {
            inlet_newPointer (cast_object (x), ARRAYCLIENT_GETPOINTER (&x->x_arrayclient));
        } else {
            inlet_newSymbol (cast_object (x),  ARRAYCLIENT_GETNAME    (&x->x_arrayclient));
        }
     
        x->x_outlet = outlet_new (cast_object (x), &s_float);
        
    } else {
    
        error_invalidArguments (sym_array__space__size, argc, argv);
        pd_free (x); x = NULL; 
    }
    
    return x;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

void arraysize_setup (void)
{
    t_class *c = NULL;
    
    c = class_new (sym_array__space__size,
            (t_newmethod)arraysize_new,
            (t_method)arrayclient_free,
            sizeof (t_arraysize),
            CLASS_DEFAULT,
            A_GIMME,
            A_NULL);
            
    class_addBang (c, arraysize_bang);
    class_addFloat (c, arraysize_float);
    
    class_setHelpName (c, sym_array);
    
    arraysize_class = c;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
