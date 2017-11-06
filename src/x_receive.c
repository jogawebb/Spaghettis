
/* Copyright (c) 1997-2017 Miller Puckette and others. */

/* < https://opensource.org/licenses/BSD-3-Clause > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

#include "m_pd.h"
#include "m_core.h"

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

static t_class *receive_class;      /* Shared. */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

typedef struct _receive {
    t_object    x_obj;              /* Must be the first. */
    t_symbol    *x_name;
    t_outlet    *x_outlet;
    } t_receive;

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

static void receive_bang (t_receive *x)
{
    outlet_bang (x->x_outlet);
}

static void receive_float (t_receive *x, t_float f)
{
    outlet_float (x->x_outlet, f);
}

static void receive_symbol (t_receive *x, t_symbol *s)
{
    outlet_symbol (x->x_outlet, s);
}

static void receive_pointer (t_receive *x, t_gpointer *gp)
{
    outlet_pointer (x->x_outlet, gp);
}

static void receive_list (t_receive *x, t_symbol *s, int argc, t_atom *argv)
{
    outlet_list (x->x_outlet, argc, argv);
}

static void receive_anything (t_receive *x, t_symbol *s, int argc, t_atom *argv)
{
    outlet_anything (x->x_outlet, s, argc, argv);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

static void receive_bind (t_receive *x, t_symbol *s)
{
    if (x->x_name != &s_) { pd_unbind (cast_pd (x), x->x_name); }
    
    x->x_name = s;
    
    if (x->x_name != &s_) { pd_bind (cast_pd (x), x->x_name); }
}

static void receive_set (t_receive *x, t_symbol *s, int argc, t_atom *argv)
{
    if (argc && IS_SYMBOL (argv)) { receive_bind (x, GET_SYMBOL (argv)); }
    else {
        error_unexpected (sym_receive, sym_value);
    }
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

static void *receive_new (t_symbol *s)
{
    t_receive *x = (t_receive *)pd_new (receive_class);
    
    x->x_name   = &s_;
    x->x_outlet = outlet_new (cast_object (x), &s_anything);
    
    receive_bind (x, s);
    
    if (x->x_name == &s_) { inlet_new2 (x, &s_symbol); }

    return x;
}

static void receive_free (t_receive *x)
{
    receive_bind (x, &s_);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

void receive_setup (void)
{
    t_class *c = NULL;
    
    c = class_new (sym_receive,
            (t_newmethod)receive_new, 
            (t_method)receive_free,
            sizeof (t_receive),
            CLASS_DEFAULT | CLASS_NOINLET,
            A_DEFSYMBOL,
            A_NULL);
            
    class_addCreator ((t_newmethod)receive_new, sym_r, A_DEFSYMBOL, A_NULL);
    
    class_addBang (c, (t_method)receive_bang);
    class_addFloat (c, (t_method)receive_float);
    class_addSymbol (c, (t_method)receive_symbol);
    class_addPointer (c, (t_method)receive_pointer);
    class_addList (c, (t_method)receive_list);
    class_addAnything (c, (t_method)receive_anything);
    
    /* Use an A_GIMME signature (instead of A_SYMBOL) to avoid warnings with list. */
    
    class_addMethod (c, (t_method)receive_set, sym__inlet2, A_GIMME, A_NULL);
    
    receive_class = c;
}

void receive_destroy (void)
{
    class_free (receive_class);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
