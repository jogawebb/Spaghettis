
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

static t_class *textsearch_class;       /* Shared. */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#define TEXTSEARCH_EQUAL                0
#define TEXTSEARCH_GREATER              1
#define TEXTSEARCH_GREATER_EQUAL        2
#define TEXTSEARCH_LESS                 3
#define TEXTSEARCH_LESS_EQUAL           4
#define TEXTSEARCH_NEAR                 5

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

typedef struct _key {
    int k_field;
    int k_type;
    } t_key;

typedef struct _textsearch {
    t_textclient    x_textclient;
    int             x_numberOfKeys;
    t_key           *x_keys;
    t_outlet        *x_outlet;
    } t_textsearch;

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

static int textsearch_listIsMatch (t_textsearch *x, t_buffer *b, int start, int end, int argc, t_atom *argv)
{
    int field = x->x_keys[0].k_field;
    int type  = x->x_keys[0].k_type;
    int count = end - start;
    int j;
    
    for (j = 0; j < argc;) {
    //
    if (field < count && atom_typesAreEqual (buffer_atomAtIndex (b, start + field), argv + j)) {
    //
    if (IS_FLOAT (argv + j)) {
        t_float f1 = GET_FLOAT (buffer_atomAtIndex (b, start + field));
        t_float f2 = GET_FLOAT (argv + j);
        switch (type) {
            case TEXTSEARCH_EQUAL           : if (f1 != f2) { return 0; } break;
            case TEXTSEARCH_GREATER         : if (f1 <= f2) { return 0; } break;
            case TEXTSEARCH_GREATER_EQUAL   : if (f1 < f2)  { return 0; } break;
            case TEXTSEARCH_LESS            : if (f1 >= f2) { return 0; } break;
            case TEXTSEARCH_LESS_EQUAL      : if (f1 > f2)  { return 0; } break;
            case TEXTSEARCH_NEAR            : break;
        }
        
    } else {
        if (type != TEXTSEARCH_EQUAL) { return 0; }
        if (atom_getSymbol (buffer_atomAtIndex (b, start + field)) != atom_getSymbol (argv + j)) {
            return 0; 
        }
    }
    
    if (++j < x->x_numberOfKeys) { field = x->x_keys[j].k_field; type = x->x_keys[j].k_type; }
    else {
        field++;
    }
    //
    } else {
        return 0;
    }
    //
    }

    return 1;
}

static int textsearch_listIsBetter (t_textsearch *x,
    t_buffer *b,
    int bestLineStart,
    int start,
    int argc,
    t_atom *argv)
{
    int field = x->x_keys[0].k_field;
    int type  = x->x_keys[0].k_type;
    int j;
        
    for (j = 0; j < argc;) {
    //
    if (IS_FLOAT (argv + j)) {
    //
    t_float f            = GET_FLOAT (argv + j);
    t_float bestValue    = GET_FLOAT (buffer_atomAtIndex (b, bestLineStart + field));
    t_float thisValue    = GET_FLOAT (buffer_atomAtIndex (b, start + field)); 
    t_float bestDistance = math_euclideanDistance (f, 0.0, bestValue, 0.0);
    t_float thisDistance = math_euclideanDistance (f, 0.0, thisValue, 0.0);
    
    if (type != TEXTSEARCH_EQUAL) { 
        int cmp = math_compareFloat (thisDistance, bestDistance);
        if (cmp == -1)     { return 1; }
        else if (cmp == 1) { return 0; }
    }
    //
    }
    
    if (++j < x->x_numberOfKeys) { field = x->x_keys[j].k_field; type = x->x_keys[j].k_type; }
    else {
        field++;
    }
    //
    }
    
    return 0;
}

static void textsearch_list (t_textsearch *x, t_symbol *s, int argc, t_atom *argv)
{
    t_buffer *b = textclient_fetchBuffer (&x->x_textclient);
    
    if (b) {
    //
    int numberOfLines = buffer_getNumberOfMessages (b);
    int bestLine = -1;
    int bestLineStart;
    int i, start, end;
        
    for (i = 0; i < numberOfLines; i++) {
        buffer_getMessageAt (b, i, &start, &end);
        if (textsearch_listIsMatch (x, b, start, end, argc, argv)) {
            if (bestLine < 0 || textsearch_listIsBetter (x, b, bestLineStart, start, argc, argv)) {
                bestLine = i; bestLineStart = start;
            }
        }
    }
    
    outlet_float (x->x_outlet, bestLine);
    //
    } else { error_undefined (sym_text__space__search, sym_text); }
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

void *textsearch_new (t_symbol *s, int argc, t_atom *argv)
{
    t_textsearch *x = (t_textsearch *)pd_new (textsearch_class);
    
    int i, numberOfKeys = 0;
    
    /* Note that it may consume arguments. */
    
    t_error err = textclient_init (&x->x_textclient, &argc, &argv);
    
    if (!err) {
    
        for (i = 0; i < argc; i++) { if (IS_FLOAT (argv + i)) { numberOfKeys++; } }
        
        x->x_numberOfKeys = PD_MAX (1, numberOfKeys);
        x->x_keys         = (t_key *)PD_MEMORY_GET (x->x_numberOfKeys * sizeof (t_key));
        x->x_outlet       = outlet_new (cast_object (x), &s_list);
         
        if (!numberOfKeys) { x->x_keys[0].k_field = 0; x->x_keys[0].k_type = TEXTSEARCH_EQUAL; }
        else {
        //
        int key = 0;
        int op = -1;
        
        for (i = 0; i < argc; i++) {
        //
        if (IS_FLOAT (argv + i)) {
            x->x_keys[key].k_field = PD_MAX (0.0, (int)GET_FLOAT (argv + i));
            x->x_keys[key].k_type  = PD_MAX (TEXTSEARCH_EQUAL, op);
            op = -1;
            key++;
            continue;
            
        } else {
            t_symbol *t = atom_getSymbolAtIndex (i, argc, argv);
            
            if ((key < x->x_numberOfKeys) && (op < 0))   {
                if (t == sym___equals____equals__)       { op = TEXTSEARCH_EQUAL;         }
                else if (t == sym___greater__)           { op = TEXTSEARCH_GREATER;       }
                else if (t == sym___greater____equals__) { op = TEXTSEARCH_GREATER_EQUAL; }
                else if (t == sym___less__)              { op = TEXTSEARCH_LESS;          }
                else if (t == sym___less____equals__)    { op = TEXTSEARCH_LESS_EQUAL;    }
                else if (t == sym_near)                  { op = TEXTSEARCH_NEAR;          }
                if (op >= 0) { continue; }
            }
        }
        
        warning_unusedArguments (sym_text__space__search, 1, argv + i);
        //
        }
        //
        }
        
        if (TEXTCLIENT_ASPOINTER (&x->x_textclient)) { 
            inlet_newPointer (cast_object (x), TEXTCLIENT_GETPOINTER (&x->x_textclient));
        } else {
            inlet_newSymbol (cast_object (x), TEXTCLIENT_GETNAME (&x->x_textclient));
        }
    
    } else {
    
        error_invalidArguments (sym_text__space__search, argc, argv);
        pd_free (x); x = NULL;
    }
    
    return x;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

void textsearch_setup (void)
{
    t_class *c = NULL;
    
    c = class_new (sym_text__space__search,
            (t_newmethod)textsearch_new,
            (t_method)textclient_free,
            sizeof (t_textsearch),
            CLASS_DEFAULT,
            A_GIMME,
            A_NULL);
            
    class_addList (c, textsearch_list);
    class_setHelpName (c, sym_text);
    
    textsearch_class = c;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
