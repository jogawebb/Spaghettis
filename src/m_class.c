
/* 
    Copyright (c) 1997-2015 Miller Puckette and others.
*/

/* < https://opensource.org/licenses/BSD-3-Clause > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#include "m_pd.h"
#include "m_private.h"
#include "m_macros.h"
#include "s_system.h"

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#if PD_WINDOWS
    #include <io.h>
#else
    #include <unistd.h>
#endif

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

#ifdef PD_MSVC
    #define snprintf sprintf_s
#endif

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

extern t_widgetbehavior text_widgetBehavior;

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

t_pd pd_objectMaker;                                /* Shared. */
t_pd pd_canvasMaker;                                /* Shared. */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

static t_symbol *class_externalDirectory = &s_;     /* Shared. */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

static void class_defaultFloat      (t_pd *x, t_float f);
static void class_defaultList       (t_pd *x, t_symbol *s, int argc, t_atom *argv);
static void class_defaultAnything   (t_pd *x, t_symbol *s, int argc, t_atom *argv);

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

static void class_floatForSignal (t_pd *x, t_float f)
{
    int offset = (*x)->c_floatSignalIn;
    PD_ASSERT (offset > 0);
    *(t_float *)(((char *)x) + offset) = f;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

static void class_defaultSave (t_gobj *z, t_binbuf *b)
{
    PD_BUG;
}

static void class_defaultProperties (t_gobj *z, t_glist *glist)
{
    PD_BUG;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

static void class_defaultBang (t_pd *x)
{
    if ((*(*x)->c_methodList) != class_defaultList) { (*(*x)->c_methodList) (x, NULL, 0, NULL); }
    else { 
        (*(*x)->c_methodAny) (x, &s_bang, 0, NULL);
    }
}

static void class_defaultPointer (t_pd *x, t_gpointer *gp)
{
    t_atom a;
    SET_POINTER (&a, gp);
        
    if ((*(*x)->c_methodList) != class_defaultList) { (*(*x)->c_methodList) (x, NULL, 1, &a); }
    else {
        (*(*x)->c_methodAny) (x, &s_pointer, 1, &a);
    }
}

static void class_defaultFloat (t_pd *x, t_float f)
{
    t_atom a;
    SET_FLOAT (&a, f);
        
    if ((*(*x)->c_methodList) != class_defaultList) { (*(*x)->c_methodList) (x, NULL, 1, &a); }
    else {
        (*(*x)->c_methodAny) (x, &s_float, 1, &a);
    }
}

static void class_defaultSymbol (t_pd *x, t_symbol *s)
{
    t_atom a;
    SET_SYMBOL (&a, s);
        
    if ((*(*x)->c_methodList) != class_defaultList) { (*(*x)->c_methodList) (x, NULL, 1, &a); }
    else {
        (*(*x)->c_methodAny) (x, &s_symbol, 1, &a);
    }
}

static void class_defaultList (t_pd *x, t_symbol *s, int argc, t_atom *argv)
{
    if (argc == 0) {
        if ((*(*x)->c_methodBang) != class_defaultBang) { (*(*x)->c_methodBang) (x); return; }
    }
    
    if (argc == 1) {
    
        if (IS_FLOAT (argv)) {
            if ((*(*x)->c_methodFloat) != class_defaultFloat) {
                (*(*x)->c_methodFloat) (x, GET_FLOAT (argv));
                return;
            }
        }
            
        if (IS_SYMBOL (argv)) { 
            if ((*(*x)->c_methodSymbol) != class_defaultSymbol) {
                (*(*x)->c_methodSymbol) (x, GET_SYMBOL (argv));
                return;
            }
        }
            
        if (IS_POINTER (argv)) {
            if ((*(*x)->c_methodPointer) != class_defaultPointer) {
                (*(*x)->c_methodPointer) (x, GET_POINTER (argv)); 
                return;
            }
        }
    }

    if ((*(*x)->c_methodAny) != class_defaultAnything) { (*(*x)->c_methodAny) (x, &s_list, argc, argv); }
    else if ((*x)->c_isBox) { obj_list ((t_object *)x, s, argc, argv); }
    else { 
        class_defaultAnything (x, &s_list, argc, argv); 
    }
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

static void class_defaultAnything (t_pd *x, t_symbol *s, int argc, t_atom *argv)
{
    post_error ("%s: unknown method \"%s\"", (*x)->c_name->s_name, s->s_name);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

void class_setExternalDirectory (t_symbol *s)
{
    class_externalDirectory = s;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

t_class *class_new (t_symbol *s, 
    t_newmethod newMethod, 
    t_method freeMethod, 
    size_t size, 
    int flags, 
    t_atomtype type1, ...)
{
    va_list ap;
    t_atomtype arg[PD_ARGUMENTS + 1] = { 0 };
    t_atomtype *vp = arg;
    int count = 0;
    t_class *c = NULL;
    int typeflag = flags & (CLASS_PURE | CLASS_GRAPHIC | CLASS_BOX);
    if (!typeflag) { typeflag = CLASS_BOX; }
    
    *vp = type1;

    va_start (ap, type1);
    
    while (*vp) {
        if (count == PD_ARGUMENTS) { PD_BUG; break; }
        vp++; count++;
        *vp = va_arg (ap, t_atomtype);
    }
    
    va_end (ap);
    
    if (pd_objectMaker && newMethod) {
    //
    class_addMethod (pd_objectMaker, (t_method)newMethod, s, arg[0], arg[1], arg[2], arg[3], arg[4], arg[5]);
    //
    }
    
    c = (t_class *)getbytes (sizeof (t_class));
    c->c_name               = s;
    c->c_helpName           = s;
    c->c_externalDirectory  = class_externalDirectory;
    c->c_size               = size;
    c->c_methods            = getbytes (0);                 /* Allocate 1 byte of memory. */
    c->c_methodsSize        = 0;
    c->c_methodFree         = freeMethod;
    c->c_methodBang         = class_defaultBang;
    c->c_methodPointer      = class_defaultPointer;
    c->c_methodFloat        = class_defaultFloat;
    c->c_methodSymbol       = class_defaultSymbol;
    c->c_methodList         = class_defaultList;
    c->c_methodAny          = class_defaultAnything;
    c->c_behavior           = (typeflag == CLASS_BOX ? &text_widgetBehavior : NULL);
    c->c_behaviorParent     = NULL;
    c->c_fnSave             = (typeflag == CLASS_BOX ? text_save : class_defaultSave);
    c->c_fnProperties       = class_defaultProperties;
    c->c_floatSignalIn      = 0;
    c->c_isGraphic          = (typeflag >= CLASS_GRAPHIC);
    c->c_isBox              = (typeflag == CLASS_BOX);
    c->c_hasFirstInlet      = ((flags & CLASS_NOINLET) == 0);
    c->c_hasDrawCommand     = 0;

    return c;
}

void class_addSignal (t_class *c, int offset)
{
    if (offset <= 0) { PD_BUG; }
    else {
        c->c_floatSignalIn = offset;
        c->c_methodFloat = class_floatForSignal;
    }
}

void class_addCreator (t_newmethod newmethod, t_symbol *s, t_atomtype type1, ...)
{
    va_list ap;
    t_atomtype arg[PD_ARGUMENTS + 1] = { 0 };
    t_atomtype *vp = arg;
    int count = 0;
    *vp = type1;

    va_start (ap, type1);
    
    while (*vp) {
        if (count == PD_ARGUMENTS) { PD_BUG; break; }
        vp++; count++;
        *vp = va_arg (ap, t_atomtype);
    } 
    
    va_end (ap);
    
    class_addMethod (pd_objectMaker, (t_method)newmethod, s, arg[0], arg[1], arg[2], arg[3], arg[4], arg[5]);
}

void class_addMethod (t_class *c, t_method fn, t_symbol *s, t_atomtype type1, ...)
{
    va_list ap;
    t_atomtype argtype = type1;
    t_methodentry *m = NULL;
    size_t oldSize, newSize;
    int i, n = 0;
        
    va_start (ap, type1);
    
    if (s == &s_signal) { PD_OBSOLETE; return; }
        
    if (s == &s_bang) {
        if (argtype) { PD_BUG; return; }
        class_addBang (c, fn);
        
    } else if (s == &s_float) {
        if (argtype != A_FLOAT || va_arg (ap, t_atomtype))  { PD_BUG; return; }
        class_addFloat (c, fn);
        
    } else if (s == &s_symbol) {
        if (argtype != A_SYMBOL || va_arg (ap, t_atomtype)) { PD_BUG; return; }
        class_addSymbol (c, fn);
        
    } else if (s == &s_list) {
        if (argtype != A_GIMME) { PD_BUG; return; }
        class_addList (c, fn);
        
    } else if (s == &s_anything) {
        if (argtype != A_GIMME) { PD_BUG; return; }
        class_addAnything (c, fn);
        
    } else {
    //
    
    for (i = 0; i < c->c_methodsSize; i++) {
        if (c->c_methods[i].me_name == s) { PD_OBSOLETE; return; }
    }
    
    oldSize = c->c_methodsSize * sizeof (t_methodentry);
    newSize = (c->c_methodsSize + 1) * sizeof (t_methodentry);
    
    c->c_methods = resizebytes (c->c_methods, oldSize, newSize);
    
    m = c->c_methods + c->c_methodsSize;
    c->c_methodsSize++;
    m->me_name = s;
    m->me_function = (t_gotfn)fn;

    while (argtype != A_NULL && n < PD_ARGUMENTS) {
        m->me_arguments[n] = argtype;
        n++;
        argtype = va_arg (ap, t_atomtype);
    }
    
    PD_ASSERT (argtype == A_NULL);

    va_end (ap);
    
    m->me_arguments[n] = A_NULL;
    //
    }
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

void class_addBang (t_class *c, t_method fn)
{
    c->c_methodBang = (t_bangmethod)fn;
}

void class_addPointer (t_class *c, t_method fn)
{
    c->c_methodPointer = (t_pointermethod)fn;
}

void class_addFloat (t_class *c, t_method fn)
{
    c->c_methodFloat = (t_floatmethod)fn;
}

void class_addSymbol (t_class *c, t_method fn)
{
    c->c_methodSymbol = (t_symbolmethod)fn;
}

void class_addList (t_class *c, t_method fn)
{
    c->c_methodList = (t_listmethod)fn;
}

void class_addAnything (t_class *c, t_method fn)
{
    c->c_methodAny = (t_anymethod)fn;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

int class_hasDrawCommand (t_class *c)
{
    return c->c_hasDrawCommand;
}

int class_hasPropertiesFunction (t_class *c)
{
    return (c->c_fnProperties != class_defaultProperties);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

void class_setHelpName (t_class *c, t_symbol *s)
{
    c->c_helpName = s;
}

void class_setWidget (t_class *c, t_widgetbehavior *w)
{
    c->c_behavior = w;
}

void class_setParentWidget (t_class *c, t_parentwidgetbehavior *pw)
{
    c->c_behaviorParent = pw;
}

void class_setDrawCommand (t_class *c)
{
    c->c_hasDrawCommand = 1;
}

void class_setSaveFunction (t_class *c, t_savefn f)
{
    c->c_fnSave = f;
}

void class_setPropertiesFunction (t_class *c, t_propertiesfn f)
{
    c->c_fnProperties = f;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

char *class_getName (t_class *c)
{
    return c->c_name->s_name;
}

char *class_getHelpName (t_class *c)
{
    return c->c_helpName->s_name;
}

char *class_getHelpDirectory(t_class *c)
{
    return (c->c_externalDirectory->s_name);
}

t_parentwidgetbehavior *class_getParentWidget (t_class *c)
{
    return c->c_behaviorParent;
}

t_propertiesfn class_getPropertiesFunction (t_class *c)
{
    return c->c_fnProperties;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

/* ---------------- the symbol table ------------------------ */

#define HASHSIZE 1024

static t_symbol *symhash[HASHSIZE];

t_symbol *dogensym(const char *s, t_symbol *oldsym)
{
    t_symbol **sym1, *sym2;
    unsigned int hash = 5381;
    int length = 0;
    const char *s2 = s;
    while (*s2) /* djb2 hash algo */
    {
        hash = ((hash << 5) + hash) + *s2;
        length++;
        s2++;
    }
    sym1 = symhash + (hash & (HASHSIZE-1));
    while (sym2 = *sym1)
    {
        if (!strcmp(sym2->s_name, s)) return(sym2);
        sym1 = &sym2->s_next;
    }
    if (oldsym) sym2 = oldsym;
    else
    {
        sym2 = (t_symbol *)getbytes(sizeof(*sym2));
        sym2->s_name = getbytes(length+1);
        sym2->s_next = 0;
        sym2->s_thing = 0;
        strcpy(sym2->s_name, s);
    }
    *sym1 = sym2;
    return (sym2);
}

t_symbol *gensym(const char *s)
{
    return(dogensym(s, 0));
}

static t_symbol *addfileextent(t_symbol *s)
{
    char namebuf[PD_STRING], *str = s->s_name;
    int ln = strlen(str);
    if (!strcmp(str + ln - 3, ".pd")) return (s);
    strcpy(namebuf, str);
    strcpy(namebuf+ln, ".pd");
    return (gensym(namebuf));
}

#define MAXOBJDEPTH 1000
static int tryingalready;

void canvas_popabstraction(t_canvas *x);
extern t_pd *newest;

t_symbol* pathsearch(t_symbol *s,char* ext);
int pd_setLoadingAbstraction (t_symbol *sym);

    /* this routine is called when a new "object" is requested whose class Pd
    doesn't know.  Pd tries to load it as an extern, then as an abstraction. */
void new_anything(void *dummy, t_symbol *s, int argc, t_atom *argv)
{
    int fd;
    char dirbuf[PD_STRING], classslashclass[PD_STRING], *nameptr;
    if (tryingalready>MAXOBJDEPTH){
      post_error ("maximum object loading depth %d reached", MAXOBJDEPTH);
      return;
    }
    newest = 0;
    if (sys_load_lib(canvas_getcurrent(), s->s_name))
    {
        tryingalready++;
        pd_typedmess(dummy, s, argc, argv);
        tryingalready--;
        return;
    }
    /* for class/class.pd support, to match class/class.pd_linux  */
    snprintf(classslashclass, PD_STRING, "%s/%s", s->s_name, s->s_name);
    if ((fd = canvas_open(canvas_getcurrent(), s->s_name, ".pd",
        dirbuf, &nameptr, PD_STRING, 0)) >= 0 ||
            (fd = canvas_open(canvas_getcurrent(), s->s_name, ".pat",
                dirbuf, &nameptr, PD_STRING, 0)) >= 0 ||
               (fd = canvas_open(canvas_getcurrent(), classslashclass, ".pd",
                    dirbuf, &nameptr, PD_STRING, 0)) >= 0)
    {
        close (fd);
        if (!pd_setLoadingAbstraction (s))
        {
            t_pd *was = s__X.s_thing;
            canvas_setargs(argc, argv);
            binbuf_evalfile(gensym(nameptr), gensym(dirbuf));
            if (s__X.s_thing && was != s__X.s_thing)
                canvas_popabstraction((t_canvas *)(s__X.s_thing));
            else s__X.s_thing = was;
            canvas_setargs(0, 0);
        }
        else post_error ("%s: can't load abstraction within itself\n", s->s_name);
    }
    else newest = 0;
}

/* Shared. */

t_symbol  s_pointer =   {"pointer", 0, 0};
t_symbol  s_float =     {"float", 0, 0};
t_symbol  s_symbol =    {"symbol", 0, 0};
t_symbol  s_bang =      {"bang", 0, 0};
t_symbol  s_list =      {"list", 0, 0};
t_symbol  s_anything =  {"anything", 0, 0};
t_symbol  s_signal =    {"signal", 0, 0};
t_symbol  s__N =        {"#N", 0, 0};
t_symbol  s__X =        {"#X", 0, 0};
t_symbol  s_x =         {"x", 0, 0};
t_symbol  s_y =         {"y", 0, 0};
t_symbol  s_ =          {"", 0, 0};

static t_symbol *symlist[] = { &s_pointer, &s_float, &s_symbol, &s_bang,
    &s_list, &s_anything, &s_signal, &s__N, &s__X, &s_x, &s_y, &s_};

void mess_init(void)
{
    t_symbol **sp;
    int i;

    if (pd_objectMaker) return;    
    for (i = sizeof(symlist)/sizeof(*symlist), sp = symlist; i--; sp++)
        (void) dogensym((*sp)->s_name, *sp);
    pd_objectMaker = class_new(gensym("objectmaker"), 0, 0, sizeof(t_pd),
        CLASS_DEFAULT, A_NULL);
    pd_canvasMaker = class_new(gensym("classmaker"), 0, 0, sizeof(t_pd),
        CLASS_DEFAULT, A_NULL);
    class_addAnything(pd_objectMaker, (t_method)new_anything);
}

t_pd *newest;

    /* horribly, we need prototypes for each of the artificial function
    calls in pd_typedmess(), to keep the compiler quiet. */
typedef t_pd *(*t_newgimme)(t_symbol *s, int argc, t_atom *argv);
typedef void(*t_messgimme)(t_pd *x, t_symbol *s, int argc, t_atom *argv);

typedef t_pd *(*t_fun0)(
    t_floatarg d1, t_floatarg d2, t_floatarg d3, t_floatarg d4, t_floatarg d5);
typedef t_pd *(*t_fun1)(t_int i1,
    t_floatarg d1, t_floatarg d2, t_floatarg d3, t_floatarg d4, t_floatarg d5);
typedef t_pd *(*t_fun2)(t_int i1, t_int i2,
    t_floatarg d1, t_floatarg d2, t_floatarg d3, t_floatarg d4, t_floatarg d5);
typedef t_pd *(*t_fun3)(t_int i1, t_int i2, t_int i3,
    t_floatarg d1, t_floatarg d2, t_floatarg d3, t_floatarg d4, t_floatarg d5);
typedef t_pd *(*t_fun4)(t_int i1, t_int i2, t_int i3, t_int i4,
    t_floatarg d1, t_floatarg d2, t_floatarg d3, t_floatarg d4, t_floatarg d5);
typedef t_pd *(*t_fun5)(t_int i1, t_int i2, t_int i3, t_int i4, t_int i5,
    t_floatarg d1, t_floatarg d2, t_floatarg d3, t_floatarg d4, t_floatarg d5);
typedef t_pd *(*t_fun6)(t_int i1, t_int i2, t_int i3, t_int i4, t_int i5, t_int i6,
    t_floatarg d1, t_floatarg d2, t_floatarg d3, t_floatarg d4, t_floatarg d5);

void pd_typedmess(t_pd *x, t_symbol *s, int argc, t_atom *argv)
{
    t_method *f;
    t_class *c = *x;
    t_methodentry *m;
    t_atomtype *wp, wanttype;
    int i;
    t_int ai[PD_ARGUMENTS+1], *ap = ai;
    t_floatarg ad[PD_ARGUMENTS+1], *dp = ad;
    int narg = 0;
    t_pd *bonzo;
    
        /* check for messages that are handled by fixed slots in the class
        structure.  We don't catch "pointer" though so that sending "pointer"
        to pd_objectMaker doesn't require that we supply a pointer value. */
    if (s == &s_float)
    {
        if (!argc) (*c->c_methodFloat)(x, 0.);
        else if (argv->a_type == A_FLOAT)
            (*c->c_methodFloat)(x, argv->a_w.w_float);
        else goto badarg;
        return;
    }
    if (s == &s_bang)
    {
        (*c->c_methodBang)(x);
        return;
    }
    if (s == &s_list)
    {
        (*c->c_methodList)(x, s, argc, argv);
        return;
    }
    if (s == &s_symbol)
    {
        if (argc && argv->a_type == A_SYMBOL)
            (*c->c_methodSymbol)(x, argv->a_w.w_symbol);
        else
            (*c->c_methodSymbol)(x, &s_);
        return;
    }
    for (i = c->c_methodsSize, m = c->c_methods; i--; m++)
        if (m->me_name == s)
    {
        wp = m->me_arguments;
        if (*wp == A_GIMME)
        {
            if (x == &pd_objectMaker)
                newest = (*((t_newgimme)(m->me_function)))(s, argc, argv);
            else (*((t_messgimme)(m->me_function)))(x, s, argc, argv);
            return;
        }
        if (argc > PD_ARGUMENTS) argc = PD_ARGUMENTS;
        if (x != &pd_objectMaker) *(ap++) = (t_int)x, narg++;
        while (wanttype = *wp++)
        {
            switch (wanttype)
            {
            case A_POINTER:
                if (!argc) goto badarg;
                else
                {
                    if (argv->a_type == A_POINTER)
                        *ap = (t_int)(argv->a_w.w_gpointer);
                    else goto badarg;
                    argc--;
                    argv++;
                }
                narg++;
                ap++;
                break;
            case A_FLOAT:
                if (!argc) goto badarg;
            case A_DEFFLOAT:
                if (!argc) *dp = 0;
                else
                {
                    if (argv->a_type == A_FLOAT)
                        *dp = argv->a_w.w_float;
                    else goto badarg;
                    argc--;
                    argv++;
                }
                dp++;
                break;
            case A_SYMBOL:
                if (!argc) goto badarg;
            case A_DEFSYMBOL:
                if (!argc) *ap = (t_int)(&s_);
                else
                {
                    if (argv->a_type == A_SYMBOL)
                        *ap = (t_int)(argv->a_w.w_symbol);
                            /* if it's an unfilled "dollar" argument it appears
                            as zero here; cheat and bash it to the null
                            symbol.  Unfortunately, this lets real zeros
                            pass as symbols too, which seems wrong... */
                    else if (x == &pd_objectMaker && argv->a_type == A_FLOAT
                        && argv->a_w.w_float == 0)
                        *ap = (t_int)(&s_);
                    else goto badarg;
                    argc--;
                    argv++;
                }
                narg++;
                ap++;
                break;
            default:
                goto badarg;
            }
        }
        switch (narg)
        {
        case 0 : bonzo = (*(t_fun0)(m->me_function))
            (ad[0], ad[1], ad[2], ad[3], ad[4]); break;
        case 1 : bonzo = (*(t_fun1)(m->me_function))
            (ai[0], ad[0], ad[1], ad[2], ad[3], ad[4]); break;
        case 2 : bonzo = (*(t_fun2)(m->me_function))
            (ai[0], ai[1], ad[0], ad[1], ad[2], ad[3], ad[4]); break;
        case 3 : bonzo = (*(t_fun3)(m->me_function))
            (ai[0], ai[1], ai[2], ad[0], ad[1], ad[2], ad[3], ad[4]); break;
        case 4 : bonzo = (*(t_fun4)(m->me_function))
            (ai[0], ai[1], ai[2], ai[3],
                ad[0], ad[1], ad[2], ad[3], ad[4]); break;
        case 5 : bonzo = (*(t_fun5)(m->me_function))
            (ai[0], ai[1], ai[2], ai[3], ai[4],
                ad[0], ad[1], ad[2], ad[3], ad[4]); break;
        case 6 : bonzo = (*(t_fun6)(m->me_function))
            (ai[0], ai[1], ai[2], ai[3], ai[4], ai[5],
                ad[0], ad[1], ad[2], ad[3], ad[4]); break;
        default: bonzo = 0;
        }
        if (x == &pd_objectMaker)
            newest = bonzo;
        return;
    }
    (*c->c_methodAny)(x, s, argc, argv);
    return;
badarg:
    post_error ("Bad arguments for message '%s' to object '%s'",
        s->s_name, c->c_name->s_name);
}

    /* convenience routine giving a stdarg interface to pd_typedmess().  Only
    ten args supported; it seems unlikely anyone will need more since
    longer messages are likely to be programmatically generated anyway. */
void pd_vmess(t_pd *x, t_symbol *sel, char *fmt, ...)
{
    va_list ap;
    t_atom arg[10], *at = arg;
    int nargs = 0;
    char *fp = fmt;

    va_start(ap, fmt);
    while (1)
    {
        if (nargs >= 10)
        {
            post_error ("pd_vmess: only 10 allowed");
            break;
        }
        switch(*fp++)
        {
        case 'f': SET_FLOAT(at, va_arg(ap, double)); break;
        case 's': SET_SYMBOL(at, va_arg(ap, t_symbol *)); break;
        case 'i': SET_FLOAT(at, va_arg(ap, t_int)); break;       
        case 'p': SET_POINTER(at, va_arg(ap, t_gpointer *)); break;
        default: goto done;
        }
        at++;
        nargs++;
    }
done:
    va_end(ap);
    pd_typedmess(x, sel, nargs, arg);
}

void pd_forwardmess(t_pd *x, int argc, t_atom *argv)
{
    if (argc)
    {
        t_atomtype t = argv->a_type;
        if (t == A_SYMBOL) pd_typedmess(x, argv->a_w.w_symbol, argc-1, argv+1);
        else if (t == A_POINTER)
        {
            if (argc == 1) pd_pointer(x, argv->a_w.w_gpointer);
            else pd_list(x, &s_list, argc, argv);
        }
        else if (t == A_FLOAT)
        {
            if (argc == 1) pd_float(x, argv->a_w.w_float);
            else pd_list(x, &s_list, argc, argv);
        }
        else { PD_BUG; }
    }

}

    /* am empty list calls the 'bang' method unless it's the default
    bang method -- that might turn around and call our 'list' method
    which could be an infinite recorsion.  Fall through to calling our
    'anything' method.  That had better not turn around and call us with
    an empty list.  */
    
void pd_emptylist(t_pd *x)
{
    if (*(*x)->c_methodBang != class_defaultBang)
        (*(*x)->c_methodBang)(x);
    else (*(*x)->c_methodAny)(x, &s_bang, 0, 0);
}

void nullfn (void) {}

t_gotfn getfn(t_pd *x, t_symbol *s)
{
    t_class *c = *x;
    t_methodentry *m;
    int i;

    for (i = c->c_methodsSize, m = c->c_methods; i--; m++)
        if (m->me_name == s) return(m->me_function);
    post_error ("%s: no method for message '%s'", c->c_name->s_name, s->s_name);
    return((t_gotfn)nullfn);
}

t_gotfn zgetfn(t_pd *x, t_symbol *s)
{
    t_class *c = *x;
    t_methodentry *m;
    int i;

    for (i = c->c_methodsSize, m = c->c_methods; i--; m++)
        if (m->me_name == s) return(m->me_function);
    return(0);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
