
/* Copyright (c) 1997-2018 Miller Puckette and others. */

/* < https://opensource.org/licenses/BSD-3-Clause > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

#include "../../m_spaghettis.h"
#include "../../m_core.h"
#include "../../g_graphics.h"

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#include "x_text.h"

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

t_error textclient_init (t_textclient *x, int *ac, t_atom **av)
{
    int argc = *ac;
    t_atom *argv = *av;
    
    x->tc_name = NULL;
    
    if (argc) {
    //
    /* Dollar expansion is zero in abstraction opened as patch. */
    
    if (IS_FLOAT (argv) && (GET_FLOAT (argv) == 0.0)) { x->tc_name = &s_; argc--; argv++; }
    else {
        if (!IS_SYMBOL (argv)) { return PD_ERROR; }
        else {
            x->tc_name = GET_SYMBOL (argv); argc--; argv++;
        }
    }
    //
    }

    *ac = argc;
    *av = argv;
    
    return PD_ERROR_NONE;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

t_buffer *textclient_fetchBuffer (t_textclient *x)
{
    if (x->tc_name) {
    //
    t_textbuffer *y = (t_textbuffer *)symbol_getThingByClass (x->tc_name, textdefine_class);

    if (y) { return textbuffer_getBuffer (y); } else { error_canNotFind (sym_text, x->tc_name); }
    //
    }
    
    return NULL;
}

t_glist *textclient_fetchView (t_textclient *x)
{
    if (x->tc_name) {
    //
    t_textbuffer *y = (t_textbuffer *)symbol_getThingByClass (x->tc_name, textdefine_class);

    if (y) { return textbuffer_getView (y); } else { error_canNotFind (sym_text, x->tc_name); }
    //
    }
    
    return NULL;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

void textclient_update (t_textclient *x)
{
    if (x->tc_name) {
    //
    t_textbuffer *y = (t_textbuffer *)symbol_getThingByClass (x->tc_name, textdefine_class);
    
    if (y) { textbuffer_update (y); } else { error_canNotFind (sym_text, x->tc_name); }
    //
    }
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
