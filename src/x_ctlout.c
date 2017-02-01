
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
#include "s_system.h"

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

static t_class *ctlout_class;           /* Shared. */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

typedef struct _ctlout {
    t_object    x_obj;                  /* Must be the first. */
    t_float     x_control;
    t_float     x_channel;
    } t_ctlout;

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

static void ctlout_float (t_ctlout *x, t_float f)
{
    outmidi_controlChange (x->x_channel, (int)x->x_control, (int)f);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

static void *ctlout_new (t_float control, t_float channel)
{
    t_ctlout *x = (t_ctlout *)pd_new (ctlout_class);
    
    x->x_control = control;
    x->x_channel = channel;
    
    inlet_newFloat (cast_object (x), &x->x_control);
    inlet_newFloat (cast_object (x), &x->x_channel);
    
    return x;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

void ctlout_setup (void)
{
    t_class *c = NULL;
    
    c = class_new (sym_ctlout,
            (t_newmethod)ctlout_new,
            NULL,
            sizeof (t_ctlout),
            CLASS_DEFAULT,
            A_DEFFLOAT,
            A_DEFFLOAT,
            A_NULL);
            
    class_addFloat (c, (t_method)ctlout_float);
    
    class_setHelpName (c, sym_midiout);
    
    ctlout_class = c;
}

void ctlout_destroy (void)
{
    CLASS_FREE (ctlout_class);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
