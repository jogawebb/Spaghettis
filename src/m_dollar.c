
/* 
    Copyright (c) 1997-2016 Miller Puckette and others.
*/

/* < https://opensource.org/licenses/BSD-3-Clause > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#include "m_pd.h"
#include "m_core.h"
#include "g_graphics.h"

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

static int dollar_getDollarZero (t_glist *glist)
{
    t_environment *environment = NULL;
    
    glist       = (glist == NULL) ? instance_contextGetCurrent() : glist;
    environment = (glist != NULL) ? glist_getEnvironment (glist) : NULL;
    
    if (environment) { return environment_getDollarZero (environment); }
    else {
        return 0;
    }
}

static int dollar_expand (char *s, char *buffer, int size, int argc, t_atom *argv, t_glist *glist)
{
    int n = (int)atol (s);      /* Note that atol return zero for an invalid number. */
    char *ptr = s;
    char c = 0;
    int length = 0;
    t_error err = PD_ERROR_NONE;
    
    *buffer = 0;
    
    c = *ptr;
    
    while (c && (c >= '0') && (c <= '9')) { c = *(++ptr); length++; }

    /* Dollar number argument is out of bound. */
    /* Dollar expansion invalid (like "$bla"). */
    /* Dollar zero expansion. */
    /* Dollar number expansion. */
    
    if (n < 0 || n > argc) { return 0; }

    if (ptr == s) {                                       
        err = string_sprintf (buffer, size, "$");       /* Unsubstituted dollars are preserved. */
        return 0;

    } else if (n == 0) {                                    
        t_atom a;
        SET_FLOAT (&a, dollar_getDollarZero (glist));
        err = atom_toString (&a, buffer, size);
        PD_ASSERT (length == 1);
        
    } else {                                                
        err = atom_toString (argv + (n - 1), buffer, size);
    }
    
    return (err ? -1 : length);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

/* Dollar symbol expansion (e.g. '$1-foo' to 'bar-foo'). */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

t_symbol *dollar_expandDollarSymbolByEnvironment (t_symbol *s, t_glist *glist)
{
    if (strchr (s->s_name, '$') == NULL) { return s; }
    else {
    //
    t_environment *e = NULL;
    
    if (glist) { e = glist_getEnvironment (glist); }

    if (!e) { return dollar_expandDollarSymbol (s, glist, 0, NULL); }
    else {
    //
    return dollar_expandDollarSymbol (s,
        glist,
        environment_getNumberOfArguments (e),
        environment_getArguments (e));
    //
    }
    //
    }
}

t_symbol *dollar_expandDollarSymbol (t_symbol *s, t_glist *glist, int argc, t_atom *argv)
{
    char t[PD_STRING] = { 0 };
    char result[PD_STRING] = { 0 };
    char *str = s->s_name;
    char *substr = NULL;
    int next = 0;
    t_error err = PD_ERROR_NONE;
    
    substr = strchr (str, '$');
    
    if (!substr) { return s; }
    else {
        err |= string_append (result, PD_STRING, str, (int)(substr - str));
        str = substr + 1;
    }

    while (!err && ((next = dollar_expand (str, t, PD_STRING, argc, argv, glist)) >= 0)) {
    //
    if ((next == 0) && (*t == 0)) { return NULL; }      /* Dollar's number argument is out of bound. */

    err |= string_add (result, PD_STRING, t);
    str += next;
    
    substr = strchr (str, '$');
    
    if (substr) { err |= string_append (result, PD_STRING, str, (int)(substr - str)); str = substr + 1; }
    else {
        err |= string_add (result, PD_STRING, str);
        break;
    }
    //
    }
    
    if (err) { error_invalid (&s_, sym_expansion); return NULL; }
    else {
        return gensym (result);
    }
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

/* Dollar number expansion (e.g. '$1' to 'foo'). */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

void dollar_expandDollarNumberByEnvironment (t_atom *dollar, t_atom *a, t_glist *glist)
{
    t_environment *e = NULL;
    
    if (glist) { e = glist_getEnvironment (glist); }

    if (!e) { dollar_expandDollarNumber (dollar, a, glist, 0, NULL); }
    else {
    //
    dollar_expandDollarNumber (dollar,
        a,
        glist,
        environment_getNumberOfArguments (e),
        environment_getArguments (e));
    //
    }
}

void dollar_expandDollarNumber (t_atom *dollar, t_atom *a, t_glist *glist, int argc, t_atom *argv)
{
    int n = GET_DOLLAR (dollar);
        
    PD_ASSERT (IS_DOLLAR (dollar));
    
    if (n > 0 && n <= argc) { *a = *(argv + n - 1); }
    else if (n == 0)        { SET_FLOAT (a, dollar_getDollarZero (glist)); }
    else {
        error_invalid (&s_, sym_expansion); SET_FLOAT (a, (t_float)0.0);
    }
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

/* Force to expand to symbol if possible. */

t_symbol *dollar_getSymbolExpandIfNeeded (t_atom *a, t_glist *glist)
{
    t_symbol *s = &s_;
    
    if (a) {
        if (IS_SYMBOL (a) || IS_DOLLARSYMBOL (a)) { 
            s = dollar_expandDollarSymbolByEnvironment (GET_SYMBOL (a), glist);
            
        } else if (IS_DOLLAR (a)) {
            t_atom t; 
            dollar_expandDollarNumberByEnvironment (a, &t, glist);
            if (IS_SYMBOL (&t)) { s = GET_SYMBOL (&t); }
        }
    }
    
    return s;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

void dollar_copyExpandAtoms (t_atom *src, int m, t_atom *dest, int n, int argc, t_atom *argv, t_glist *glist)
{
    int i;
    int size = PD_MIN (m, n);
    
    for (i = 0; i < size; i++) {

        t_atom *a = src + i;
        t_atom *b = dest + i;
        
        if (IS_SYMBOL_OR_FLOAT (a))     { *b = *a; }
        else if (IS_DOLLAR (a))         { dollar_expandDollarNumber (a, b, glist, argc, argv); }
        else if (IS_DOLLARSYMBOL (a))   {
            t_symbol *s = dollar_expandDollarSymbol (GET_SYMBOL (a), glist, argc, argv);
            if (s) { SET_SYMBOL (b, s); } else { SET_SYMBOL (b, GET_SYMBOL (a)); }
        } else { 
            PD_BUG; 
        }
    }
}

void dollar_copyExpandAtomsByEnvironment (t_atom *src, int m, t_atom *dest, int n, t_glist *glist)
{
    t_environment *e = NULL;
    
    if (glist) { e = glist_getEnvironment (glist); }

    if (!e) { dollar_copyExpandAtoms (src, m, dest, n, 0, NULL, glist); }
    else {
    //
    dollar_copyExpandAtoms (src,
        m,
        dest,
        n,
        environment_getNumberOfArguments (e),
        environment_getArguments (e),
        glist);
    //
    }
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
