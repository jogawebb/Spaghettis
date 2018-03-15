
/* Copyright (c) 1997-2018 Miller Puckette and others. */

/* < https://opensource.org/licenses/BSD-3-Clause > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

#include "m_spaghettis.h"
#include "m_core.h"
#include "x_mica.h"

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#if PD_WITH_BELLE

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#include "Belle.hpp"

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

t_class *concept_class;             /* Shared. */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

typedef struct _concept {
    t_pd        x_pd;               /* Must be the first. */
    t_symbol    *x_name;
    mica::UUID  x_uuid;
    } t_concept;

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

static t_concept *concept_new (t_symbol *s, mica::UUID uuid)
{
    t_concept *x = (t_concept *)pd_new (concept_class);
    
    x->x_name = s;
    x->x_uuid = uuid;
    
    pd_bind (cast_pd (x), x->x_name);
    
    return x;
}

static void concept_free (t_concept *x)
{
    post_log ("FREE %s", mica::Concept (x->x_uuid).toString().c_str());
    
    pd_unbind (cast_pd (x), x->x_name);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

void concept_setup (void)
{
    t_class *c = NULL;
    
    c = class_new (sym_concept,
            NULL,
            (t_method)concept_free,
            sizeof (t_concept),
            CLASS_NOBOX,
            A_NULL);
    
    concept_class = c;
}

void concept_destroy (void)
{
    class_free (concept_class);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

static void *mica_makeObject (t_symbol *s, int argc, t_atom *argv)
{
    t_pd *newest = NULL;
    
    instance_setNewestObject (NULL);
    
    if (!argc || !IS_SYMBOL (argv)) { newest = (t_pd *)micaset_new (s, argc, argv); }
    else {
    //
    t_symbol *t = atom_getSymbolAtIndex (0, argc, argv);
    
    if (t == sym_set)               { newest = (t_pd *)micaset_new (s, argc - 1, argv + 1); }
    else if (t == sym_get)          { }
    else if (t == sym_map)          { }
    else if (t == sym_index)        { }
    else if (t == sym_item)         { }
    else if (t == sym_info)         { }
    else if (t == sym_interval)     { }
    else if (t == sym_spell)        { }
    else {
        error_unexpected (sym_mica, t);
    }
    //
    }
    
    instance_setNewestObject (newest);
    
    return newest;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#endif // PD_WITH_BELLE

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

void mica_setup (void)
{
    #if PD_WITH_BELLE
    
    class_addCreator ((t_newmethod)mica_makeObject, sym_mica, A_GIMME, A_NULL);
    
    #endif
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
