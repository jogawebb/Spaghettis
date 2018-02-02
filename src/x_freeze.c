
/* Copyright (c) 1997-2018 Miller Puckette and others. */

/* < https://opensource.org/licenses/BSD-3-Clause > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

#include "m_spaghettis.h"
#include "m_core.h"
#include "g_graphics.h"

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

/* Avoid to accidentally overwrite native patches (help or examples). */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

static t_class *freeze_class;       /* Shared. */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

typedef struct _freeze {
    t_object    x_obj;
    } t_freeze;

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

/* Notice that it is usable only with a debug build. */

static void *freeze_new (t_symbol *s, int argc, t_atom *argv)
{
    t_freeze *x = (t_freeze *)pd_new (freeze_class);
    
    /* To by-pass the mechanism manually append a dummy argument in the patch file with a text editor. */
    
    if (!argc) {
    
        #if ! ( PD_WITH_DEBUG )
        
        glist_setFrozen (instance_contextGetCurrent(), 1);
        
        #endif
    }
    
    return x;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

void freeze_setup (void)
{
    t_class *c = NULL;
    
    c = class_new (sym_freeze,
            (t_newmethod)freeze_new,
            NULL,
            sizeof (t_freeze),
            CLASS_DEFAULT | CLASS_NOINLET,
            A_GIMME,
            A_NULL);
    
    freeze_class = c;
}

void freeze_destroy (void)
{
    class_free (freeze_class);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
