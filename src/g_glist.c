
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
#include "s_system.h"
#include "g_canvas.h"

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

extern t_class      *canvas_class;
extern t_class      *array_define_class;
extern t_pd         *pd_newest;

extern t_symbol     *canvas_fileName;
extern t_symbol     *canvas_directory;
extern t_atom       *canvas_argv;

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

extern int          canvas_argc;
extern int          canvas_magic;

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

static t_glist      *canvas_lastCanvas;             /* Shared. */

static int          canvas_lastCanvasX;             /* Shared. */
static int          canvas_lastCanvasY;             /* Shared. */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

void canvas_setActiveFileNameAndDirectory (t_symbol *name, t_symbol *directory)
{
    canvas_fileName  = name;
    canvas_directory = directory;
}

void canvas_setActiveArguments (int argc, t_atom *argv)
{
    if (canvas_argv) { PD_MEMORY_FREE (canvas_argv); }
    
    canvas_argc = argc;
    canvas_argv = PD_MEMORY_GET_COPY (argv, argc * sizeof (t_atom));
}

void canvas_newPatch (void *dummy, t_symbol *name, t_symbol *directory)
{
    canvas_setActiveFileNameAndDirectory (name, directory);
    canvas_new (NULL, NULL, 0, NULL);
    canvas_pop (cast_glist (s__X.s_thing), 1);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

t_glist *canvas_castToGlistChecked (t_pd *x)
{
    if (pd_class (x) == canvas_class || pd_class (x) == array_define_class) {
        return cast_glist (x);
    } else {
        return NULL;
    }
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

t_glist *canvas_getCurrent (void)
{
    return (cast_glist (pd_findByClass (&s__X, canvas_class)));
}

t_environment *canvas_getEnvironment (t_glist *glist)
{
    PD_ASSERT (glist);
    
    while (!glist->gl_environment) { if (!(glist = glist->gl_parent)) { PD_BUG; } }
    
    return glist->gl_environment;
}

t_glist *canvas_getRoot (t_glist *glist)
{
    if (!glist->gl_parent || canvas_isAbstraction (glist)) { return glist; }
    else {
        return (canvas_getRoot (glist->gl_parent));
    }
}

t_glist *canvas_getTopmostParent (t_glist *glist)
{
    while (glist->gl_parent) { glist = glist->gl_parent; }
    
    return glist;
}

t_glist *canvas_getPatch (t_glist *glist)
{
    while (glist->gl_parent && !canvas_canHaveWindow (glist)) { glist = glist->gl_parent; }
    
    return glist;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

int canvas_isVisible (t_glist *glist)
{
    return (!glist->gl_isLoading && canvas_getPatch (glist)->gl_isMapped);
}

int canvas_isAbstraction (t_glist *glist)
{
    return (glist->gl_environment != NULL);
}

int canvas_canHaveWindow (t_glist *glist)
{
    return (glist->gl_haveWindow || !glist->gl_isGraphOnParent);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

t_glist *canvas_findDirty (t_glist *glist)
{
    if (glist->gl_environment && glist->gl_isDirty) { return glist; }
    else {
    //
    t_gobj *o = NULL;
    
    for (o = glist->gl_graphics; o; o = o->g_next) {
        t_glist *t = NULL;
        if (pd_class (o) == canvas_class && (t = canvas_findDirty (cast_glist (o)))) { return t; }
    }
    //
    }
    
    return NULL;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

t_glist *canvas_addGraph (t_glist *glist, t_symbol *name,
    t_float indexStart,
    t_float valueUp,
    t_float indexEnd,
    t_float valueDown,
    t_float topLeftX,
    t_float topLeftY,
    t_float bottomRightX,
    t_float bottomRightY)
{
    static int graphCount = 0;      /* Shared. */

    int createdFromMenu = 0;
    t_glist *x = (t_glist *)pd_new (canvas_class);
    
    int fontSize = (canvas_getCurrent() ? canvas_getCurrent()->gl_fontSize : font_getDefaultFontSize());
    
    PD_ASSERT (name);
    
    if (name == &s_) {
        char n[PD_STRING] = { 0 };
        string_sprintf (n, PD_STRING, "graph%d", ++graphCount);
        name = gensym (n);
        createdFromMenu = 1;
        
    } else {
        char *s = name->s_name;
        int n;
        if (!strncmp (s, "graph", 5) && (n = atoi (s + 5)) > graphCount) { graphCount = n; }
    }

    if (indexStart >= indexEnd || valueUp == valueDown) {
    //
    indexStart  = GLIST_DEFAULT_START;
    indexEnd    = GLIST_DEFAULT_END;
    valueUp     = GLIST_DEFAULT_UP;
    valueDown   = -valueUp;
    //
    }
    
    if (topLeftX >= bottomRightX || topLeftY >= bottomRightY) {
    //
    topLeftX     = GLIST_DEFAULT_X;
    topLeftY     = GLIST_DEFAULT_Y;
    bottomRightX = topLeftX + GLIST_DEFAULT_WIDTH;
    bottomRightY = topLeftY + GLIST_DEFAULT_HEIGHT;
    //
    }
    
    x->gl_obj.te_buffer         = buffer_new();
    x->gl_obj.te_xCoordinate    = topLeftX;
    x->gl_obj.te_yCoordinate    = topLeftY;
    x->gl_obj.te_type           = TYPE_OBJECT;
    x->gl_stub                  = gstub_new (x, NULL);
    x->gl_parent                = glist;
    x->gl_name                  = name;
    x->gl_magic                 = ++canvas_magic;
    x->gl_width                 = bottomRightX - topLeftX;
    x->gl_height                = bottomRightY - topLeftY;
    x->gl_indexStart            = indexStart;
    x->gl_indexEnd              = indexEnd;
    x->gl_valueUp               = valueUp;
    x->gl_valueDown             = valueDown;
    x->gl_windowTopLeftX        = 0;
    x->gl_windowTopLeftY        = CANVAS_WINDOW_HEADER_HEIGHT;
    x->gl_windowBottomRightX    = CANVAS_WINDOW_DEFAULT_WIDTH;
    x->gl_windowBottomRightY    = CANVAS_WINDOW_DEFAULT_HEIGHT + CANVAS_WINDOW_HEADER_HEIGHT;
    x->gl_fontSize              = fontSize;
    x->gl_isGraphOnParent       = 1;
    x->gl_hasRectangle          = 0;
    
    canvas_bind (x);
    buffer_vAppend (cast_object (x)->te_buffer, "s", gensym ("graph"));
    if (!createdFromMenu) { stack_push (cast_pd (x)); }
    glist_add (glist, cast_gobj (x));
    
    return x;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

int canvas_openFile (t_glist *glist,
    const char *name,
    const char *extension,
    char *directoryResult,
    char **nameResult,
    size_t size)
{
    const char *directory = glist ? canvas_getEnvironment (glist)->ce_directory->s_name : ".";
    
    int f = file_openConsideringSearchPath (directory, 
                name,
                extension,
                directoryResult,
                nameResult, 
                size);
        
    return f;
}

t_symbol *canvas_expandDollar (t_glist *glist, t_symbol *s)
{
    t_symbol *t = s;
    
    if (strchr (s->s_name, '$')) {
    //
    t_environment *environment = canvas_getEnvironment (glist);
    stack_push (cast_pd (glist));
    t = dollar_expandDollarSymbol (s, environment->ce_argc, environment->ce_argv);
    stack_pop (cast_pd (glist));
    //
    }

    return t;
}

t_error canvas_makeFilePath (t_glist *glist, char *name, char *dest, size_t size)
{
    t_error err = PD_ERROR_NONE;
    
    char *directory = canvas_getEnvironment (glist)->ce_directory->s_name;
    
    if (name[0] == '/' || (name[0] && name[1] == ':') || !(*directory)) { 
        err |= string_copy (dest, size, name);
        
    } else {
        err |= string_copy (dest, size, directory);
        err |= string_addSprintf (dest, size, "/%s", name);
    }
    
    return err;
}

void canvas_setName (t_glist *glist, t_symbol *name, t_symbol *directory)
{
    canvas_unbind (glist);
    glist->gl_name = name;
    canvas_bind (glist);
    
    if (glist->gl_haveWindow) { canvas_updateTitle (glist); }
    if (directory && directory != &s_) {
        canvas_getEnvironment (glist)->ce_directory = directory; 
    }
}

void canvas_updateTitle (t_glist *glist)
{
    sys_vGui ("::ui_patch::setTitle .x%lx {%s} {%s} %d\n",  // --
        glist,
        canvas_getEnvironment (glist)->ce_directory->s_name,
        glist->gl_name->s_name,
        glist->gl_isDirty);
}

t_symbol *canvas_makeBindSymbol (t_symbol *s)
{
    t_error err = PD_ERROR_NONE;
    char t[PD_STRING] = { 0 };
    PD_ASSERT (s);
    err = string_sprintf (t, PD_STRING, "pd-%s", s->s_name);
    PD_ASSERT (!err);
    return (gensym (t));
}

int canvas_hasGraphOnParentTitle (t_glist *glist)
{
    if (glist->gl_hideText) { return 0; }
    else {
    //
    int argc     = (cast_object (glist)->te_buffer ? buffer_size (cast_object (glist)->te_buffer)  : 0);
    t_atom *argv = (cast_object (glist)->te_buffer ? buffer_atoms (cast_object (glist)->te_buffer) : NULL);
    return !(argc && IS_SYMBOL (argv) && GET_SYMBOL (argv) == gensym ("graph"));
    //
    }
}

int canvas_getFontSize (t_glist *glist)
{
    while (!glist->gl_environment) { if (!(glist = glist->gl_parent)) { PD_BUG; } }
    
    return glist->gl_fontSize;
}

void canvas_setLastCoordinates (t_glist *glist, int a, int b)
{
    canvas_lastCanvas   = glist;
    canvas_lastCanvasX  = a;
    canvas_lastCanvasY  = b;
}

void canvas_getLastCoordinates (t_glist *glist, int *a, int *b)
{
    if (canvas_lastCanvas == glist) { *a = canvas_lastCanvasX; *b = canvas_lastCanvasY; } 
    else {
        *a = CANVAS_OBJECT_DEFAULT_X; *b = CANVAS_OBJECT_DEFAULT_Y;
    }
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

void canvas_bind (t_glist *glist)
{
    if (strcmp (glist->gl_name->s_name, PD_NAME_SHORT)) {
        pd_bind (cast_pd (glist), canvas_makeBindSymbol (glist->gl_name));
    }
}

void canvas_unbind (t_glist *glist)
{
    if (strcmp (glist->gl_name->s_name, PD_NAME_SHORT)) {
        pd_unbind (cast_pd (glist), canvas_makeBindSymbol (glist->gl_name));
    }
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

int canvas_getIndexOfObject (t_glist *glist, t_gobj *object)
{
    t_gobj *t = NULL;
    int n = 0;
    for (t = glist->gl_graphics; t && t != object; t = t->g_next) { n++; }
    return n;
}

t_gobj *canvas_getObjectAtIndex (t_glist *glist, int n)
{
    t_gobj *t = NULL;
    int i = 0;
    
    for (t = glist->gl_graphics; t; t = t->g_next) {
        if (i == n) { return t; }
        i++;
    }
    
    return NULL;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

void canvas_traverseLinesStart (t_linetraverser *t, t_glist *glist)
{
    t->tr_owner             = glist;
    t->tr_connectionCached  = NULL;
    t->tr_srcObject         = NULL;
    
    t->tr_srcIndexOfNextOutlet = t->tr_srcNumberOfOutlets = 0;
}

/* Get the lines outlet per outlet, object per object. */
/* Coordinates are set at the same time. */

t_outconnect *canvas_traverseLinesNext (t_linetraverser *t)
{
    t_outconnect *connection = t->tr_connectionCached;
    
    while (!connection) {
    //
    int n = t->tr_srcIndexOfNextOutlet;
    
    while (n == t->tr_srcNumberOfOutlets) {
    //
    t_gobj   *y = NULL;
    t_object *o = NULL;
    
    if (!t->tr_srcObject) { y = t->tr_owner->gl_graphics; }
    else {
        y = cast_gobj (t->tr_srcObject)->g_next;
    }
    
    for (; y; y = y->g_next) {
        if ((o = canvas_castToObjectIfBox (y))) { break; }     /* Only box objects are considered. */
    }
    
    if (!o) { return NULL; }
    
    t->tr_srcObject          = o;
    t->tr_srcNumberOfOutlets = object_numberOfOutlets (o);
    n = 0;
    
    if (canvas_isVisible (t->tr_owner)) {
    
        gobj_getRectangle (y, t->tr_owner,
            &t->tr_srcTopLeftX,
            &t->tr_srcTopLeftY,
            &t->tr_srcBottomRightX,
            &t->tr_srcBottomRightY);
            
    } else {
        t->tr_srcTopLeftX = t->tr_srcTopLeftY = t->tr_srcBottomRightX = t->tr_srcBottomRightY = 0;
    }
    //
    }
    
    t->tr_srcIndexOfOutlet     = n;
    t->tr_srcIndexOfNextOutlet = n + 1;
    connection = object_traverseOutletStart (t->tr_srcObject, &t->tr_srcOutlet, n);
    //
    }
    
    t->tr_connectionCached = object_traverseOutletNext (connection,
        &t->tr_destObject,
        &t->tr_destInlet,
        &t->tr_destIndexOfInlet);
                                                            
    t->tr_destNumberOfInlets = object_numberOfInlets (t->tr_destObject);
    
    PD_ASSERT (t->tr_destNumberOfInlets);
    
    if (canvas_isVisible (t->tr_owner)) {

        gobj_getRectangle (cast_gobj (t->tr_destObject), t->tr_owner,
            &t->tr_destTopLeftX,
            &t->tr_destTopLeftY,
            &t->tr_destBottomRightX,
            &t->tr_destBottomRightY);
        
        {
            int w = t->tr_srcBottomRightX - t->tr_srcTopLeftX;
            int i = t->tr_srcIndexOfOutlet;
            int j = t->tr_srcNumberOfOutlets;
        
            t->tr_lineStartX = t->tr_srcTopLeftX + INLETS_MIDDLE (w, i, j);
            t->tr_lineStartY = t->tr_srcBottomRightY;
        }
        {
            int w = t->tr_destBottomRightX - t->tr_destTopLeftX;
            int i = t->tr_destIndexOfInlet;
            int j = t->tr_destNumberOfInlets;
        
            t->tr_lineEndX = t->tr_destTopLeftX + INLETS_MIDDLE (w, i, j);
            t->tr_lineEndY = t->tr_destTopLeftY;
        }
        
    } else {
        t->tr_lineStartX   = t->tr_lineStartY   = t->tr_lineEndX         = t->tr_lineEndY         = 0;
        t->tr_destTopLeftX = t->tr_destTopLeftY = t->tr_destBottomRightX = t->tr_destBottomRightY = 0;
    }
    
    return connection;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
