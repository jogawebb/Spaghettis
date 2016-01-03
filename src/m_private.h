
/* 
    Copyright (c) 1997-2015 Miller Puckette and others.
*/

/* < https://opensource.org/licenses/BSD-3-Clause > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

#ifndef __m_private_h_
#define __m_private_h_

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

#define TYPE_TEXT               0
#define TYPE_OBJECT             1
#define TYPE_MESSAGE            2
#define TYPE_ATOM               3

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

#define DATA_FLOAT              0
#define DATA_SYMBOL             1
#define DATA_TEXT               2
#define DATA_ARRAY              3

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

#define POINTER_NONE            0
#define POINTER_GLIST           1
#define POINTER_ARRAY           2

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

PD_STRUCT _widgetbehavior;

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

typedef void (*t_savefn)(t_gobj *x, t_binbuf *b);
typedef void (*t_propertiesfn)(t_gobj *x, t_glist *glist);
typedef void (*t_callbackfn)(t_gobj *x, t_glist *glist);

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

typedef struct _entry {
    t_symbol    *me_name;
    t_method    me_method;
    t_atomtype  me_arguments[PD_ARGUMENTS + 1];
    } t_entry;

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

typedef void (*t_bangmethod)(t_pd *x);
typedef void (*t_pointermethod)(t_pd *x, t_gpointer *gp);
typedef void (*t_floatmethod)(t_pd *x, t_float f);
typedef void (*t_symbolmethod)(t_pd *x, t_symbol *s);
typedef void (*t_listmethod)(t_pd *x, t_symbol *s, int argc, t_atom *argv);
typedef void (*t_anymethod)(t_pd *x, t_symbol *s, int argc, t_atom *argv);

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

struct _class {
    t_symbol                *c_name;
    t_symbol                *c_helpName;
    t_symbol                *c_externalDirectory;
    size_t                  c_size;
    t_entry                 *c_methods;
    int                     c_methodsSize;
    t_method                c_methodFree;
    t_bangmethod            c_methodBang;
    t_pointermethod         c_methodPointer;
    t_floatmethod           c_methodFloat;
    t_symbolmethod          c_methodSymbol;
    t_listmethod            c_methodList;
    t_anymethod             c_methodAny;
    t_widgetbehavior        *c_behavior;
    t_parentwidgetbehavior  *c_behaviorParent;
    t_savefn                c_fnSave;
    t_propertiesfn          c_fnProperties;
    int                     c_floatSignalIn;
    char                    c_isGraphic;
    char                    c_isBox;
    char                    c_hasFirstInlet;
    char                    c_hasDrawCommand;
    };

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

struct _pdinstance {
    double      pd_systime;
    t_clock     *pd_clocks;
    t_int       *pd_dspChain;
    int         pd_dspChainSize;
    int         pd_dspState;
    t_signal    *pd_signals;
    t_canvas    *pd_canvases;
    //
    t_symbol    *sym_midiin;
    t_symbol    *sym_sysexin;
    t_symbol    *sym_notein;
    t_symbol    *sym_ctlin;
    t_symbol    *sym_pgmin;
    t_symbol    *sym_bendin;
    t_symbol    *sym_touchin;
    t_symbol    *sym_polytouchin;
    t_symbol    *sym_midiclkin;
    t_symbol    *sym_midirealtimein;
    };
    
// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

void    pd_push                     (t_pd *x);
void    pd_pop                      (t_pd *x);
void    pd_empty                    (t_pd *x);
void    pd_vMessage                 (t_pd *x, t_symbol *s, char *fmt, ...);
void    pd_performLoadbang          (void);
int     pd_setLoadingAbstraction    (t_symbol *s);

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

void    text_save                   (t_gobj *z, t_binbuf *b);

void    canvas_popabstraction       (t_canvas *x);

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

void        class_setExternalDirectory  (t_symbol *s);
t_method    class_getMethod             (t_class *c, t_symbol *s);
int         class_hasMethod             (t_class *c, t_symbol *s);
int         class_hasBang               (t_class *c);
int         class_hasDrawCommand        (t_class *c);
int         class_hasPropertiesFunction (t_class *c); 
void        class_setWidget             (t_class *c, t_widgetbehavior *w);
void        class_setParentWidget       (t_class *c, t_parentwidgetbehavior *w);
void        class_setDrawCommand        (t_class *c);
void        class_setHelpName           (t_class *c, t_symbol *s);
void        class_setPropertiesFunction (t_class *c, t_propertiesfn f);
void        class_setSaveFunction       (t_class *c, t_savefn f);

char        *class_getName              (t_class *c);
char        *class_getHelpName          (t_class *c);
char        *class_getHelpDirectory     (t_class *c);

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

t_parentwidgetbehavior  *class_getParentWidget          (t_class *c);
t_propertiesfn           class_getPropertiesFunction     (t_class *c);

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

void            object_list                 (t_object *x, t_symbol *s, int argc, t_atom *argv);

t_outconnect    *obj_starttraverseoutlet    (t_object *x, t_outlet **op, int nout);
t_outconnect    *obj_nexttraverseoutlet     (t_outconnect *lastconnect, 
                                                t_object **destp, 
                                                t_inlet **inletp, 
                                                int *whichp);

t_outconnect    *object_connect             (t_object *src, int m, t_object *dest, int n);
void            object_disconnect           (t_object *src, int m, t_object *dest, int n);

int obj_noutlets                            (t_object *x);
int obj_ninlets                             (t_object *x);
int obj_issignalinlet                       (t_object *x, int m);
int obj_issignaloutlet                      (t_object *x, int m);
int obj_nsiginlets                          (t_object *x);
int obj_nsigoutlets                         (t_object *x);
int obj_siginletindex                       (t_object *x, int m);
int obj_sigoutletindex                      (t_object *x, int m);

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

int outlet_isSignal                         (t_outlet *x);

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

t_pd *glob_evalfile     (t_pd *ignore, t_symbol *name, t_symbol *dir);

void glob_initfromgui   (void *dummy, t_symbol *s, int argc, t_atom *argv);
void glob_quit          (void *dummy);
void open_via_helppath  (const char *name, const char *dir);

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

void message_initialize (void);
void object_initialize  (void);
void conf_init          (void);
void glob_init          (void);
void garray_init        (void);

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

void sys_vgui               (char *fmt, ...);
void sys_gui                (char *s);
void sys_pretendguibytes    (int n);
void sys_queuegui           (void *client, t_glist *glist, t_callbackfn f);
void sys_unqueuegui         (void *client);
void gfxstub_new            (t_pd *owner, void *key, const char *cmd);
void gfxstub_deleteforkey   (void *key);

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#endif // __m_private_h_
