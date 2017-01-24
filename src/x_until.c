
/* 
    Copyright (c) 1997-2016 Miller Puckette and others.
*/

/* < https://opensource.org/licenses/BSD-3-Clause > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

#include "m_pd.h"
#include "m_macros.h"
#include "m_core.h"

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

static t_class *until_class;        /* Shared. */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

typedef struct _until {
    t_object    x_obj;              /* Must be the first. */
    int         x_run;
    int         x_count;
    t_outlet    *x_outlet;
    } t_until;

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

static void until_perform (t_until *x)
{
    while (x->x_run && x->x_count) { x->x_count--; outlet_bang (x->x_outlet); }
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

static void until_bang (t_until *x)
{
    x->x_run = 1; x->x_count = -1; until_perform (x);
}

static void until_float (t_until *x, t_float f)
{
    x->x_run = 1; x->x_count = PD_MAX (0.0, f); until_perform (x);
}

static void until_stop (t_until *x)
{
    x->x_run = 0;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

static void *until_new (void)
{
    t_until *x = (t_until *)pd_new (until_class);
    
    x->x_run    = 0;
    x->x_outlet = outlet_new (cast_object (x), &s_bang);
    
    inlet_new (cast_object (x), cast_pd (x), &s_bang, sym_inlet2);

    return x;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

void until_setup (void)
{
    t_class *c = NULL;
    
    c = class_new (sym_until,
            (t_newmethod)until_new,
            NULL,
            sizeof (t_until),
            CLASS_DEFAULT,
            A_NULL);
            
    class_addBang (c, (t_method)until_bang);
    class_addFloat (c, (t_method)until_float);
    
    class_addMethod (c, (t_method)until_stop, sym_inlet2, A_NULL);
    
    until_class = c;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
