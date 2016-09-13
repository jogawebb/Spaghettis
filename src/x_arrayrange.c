
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

void *arrayrange_new (t_class *class, int argc, t_atom *argv, int onset, int size)
{
    t_arrayrange *x = (t_arrayrange *)pd_new (class);

    t_error err = arrayclient_init (&x->ar_arrayclient, &argc, &argv);      /* It may consume arguments. */
    
    if (!err) {

        x->ar_first     = 0;
        x->ar_size      = -1;
        x->ar_fieldName = sym_y; 
            
        while (argc && IS_SYMBOL (argv)) {
        //
        t_symbol *t = GET_SYMBOL (argv);

        if (t == sym___dash__f || t == sym___dash__field) {
            if (argc >= 3 && IS_SYMBOL (argv + 1) && IS_SYMBOL (argv + 2)) {
                x->ar_fieldName = GET_SYMBOL (argv + 2);
                argc -= 3; argv += 3;
            } else {
                err = PD_ERROR;
            }
        }

        break;
        //
        }
    }
    
    error__options (class_getName (class), argc, argv);
    
    if (!err) { 
    
        if (onset) { inlet_newFloat (cast_object (x), &x->ar_first); }
        if (size)  { inlet_newFloat (cast_object (x), &x->ar_size); }
        
        if (argc && IS_FLOAT (argv)) { x->ar_first = GET_FLOAT (argv); argc--; argv++; }
        if (argc && IS_FLOAT (argv)) { x->ar_size  = GET_FLOAT (argv); argc--; argv++; }

        if (argc) { warning_unusedArguments (class_getName (class), argc, argv); }
        
        if (ARRAYCLIENT_ASPOINTER (&x->ar_arrayclient)) {
            inlet_newPointer (cast_object (x), ARRAYCLIENT_GETPOINTER (&x->ar_arrayclient));
        } else {
            inlet_newSymbol (cast_object (x),  ARRAYCLIENT_GETNAME    (&x->ar_arrayclient));
        }
     
        return x;
    }
    
    return NULL;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

int arrayrange_setFirst (t_arrayrange *x, t_float f)
{
    x->ar_first = f;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

int arrayrange_isValid (t_arrayrange *x)
{
    t_array *a = arrayclient_fetchArray (&x->ar_arrayclient);
    
    if (a) { return template_fieldIsFloat (array_getTemplate (a), x->ar_fieldName); }
    
    return 0;
}

t_array *arrayrange_getRange (t_arrayrange *x, int *i, int *n)
{
    t_array *a = arrayclient_fetchArray (&x->ar_arrayclient);
    
    PD_ASSERT (a);
    
    int size = array_getSize (a);
    int count, first = PD_CLAMP (x->ar_first, 0, size);
    
    if (x->ar_size < 0) { count = size - first; }
    else {
        count = x->ar_size; if (first + count > size) { count = size - first; }
    }
    
    *i = first;
    *n = count;
    
    return a;
}

t_symbol *arrayrange_getFieldName (t_arrayrange *x)
{
    return x->ar_fieldName;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

int array_rangeop_getrange (t_arrayrange *x, char **firstitemp, int *nitemp, int *stridep, int *arrayonsetp)
{
    //t_glist *glist;
    t_array *a = arrayclient_fetchArray (&x->ar_arrayclient);
    char *elemp;
    int stride, fieldonset, arrayonset, nitem, i, type;
    t_symbol *arraytype;
    double sum;
    t_template *template;
    if (!a)
        return (0);
    template = array_getTemplate (a);
    if (!template_findField(template, x->ar_fieldName, &fieldonset, /* Remove template_findField ASAP !!! */
        &type, &arraytype) || type != DATA_FLOAT)
    {
        return (0);
    }
    stride = a->a_stride;   /* Encapsulate ASAP. */
    arrayonset = x->ar_first;
    if (arrayonset < 0)
        arrayonset = 0;
    else if (arrayonset > array_getSize (a))
        arrayonset = array_getSize (a);
    if (x->ar_size < 0)
        nitem = array_getSize (a) - arrayonset;
    else
    {
        nitem = x->ar_size;
        if (nitem + arrayonset > array_getSize (a))
            nitem = array_getSize (a) - arrayonset;
    }
    
    *firstitemp = a->a_vector+(fieldonset+arrayonset*stride);   /* Encapsulate ASAP. */
    *nitemp = nitem;
    *stridep = stride;
    *arrayonsetp = arrayonset;
    return (1);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
