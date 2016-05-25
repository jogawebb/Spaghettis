
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
#include "g_canvas.h"

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

extern t_pd     *pd_newest;
extern t_class  *canvas_class;
extern t_class  *vinlet_class;
extern t_class  *voutlet_class;

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

static int canvas_getIndexOfObjectAmong (t_glist *glist, t_gobj *y, int selected)
{
    t_gobj *t = NULL;
    int n = 0;

    for (t = glist->gl_graphics; t && t != y; t = t->g_next) {
        if (selected == canvas_isObjectSelected (glist, t)) { 
            n++; 
        }
    }
    
    return n;
}

static void canvas_deselectAllRecursive (t_gobj *g)
{
    if (pd_class (g) == canvas_class) { 
    //
    t_gobj *o = NULL;
    for (o = cast_glist (g)->gl_graphics; o; o = o->g_next) { canvas_deselectAllRecursive (o); }
    canvas_deselectAll (cast_glist (g));
    //
    }
}

static void canvas_deselectLine (t_glist *glist)
{
    glist->gl_editor->e_isSelectedline = 0;
    
    sys_vGui (".x%lx.c itemconfigure %lxLINE -fill black\n",
                glist,
                glist->gl_editor->e_selectedLineConnection);   
}

static void canvas_cacheLines (t_glist *glist)
{
    t_gobj *selectedHead = NULL;
    t_gobj *selectedTail = NULL;
    t_gobj *unselectedHead = NULL;
    t_gobj *unselectedTail = NULL;
    t_gobj *y1 = NULL;
    t_gobj *y2 = NULL;
    t_linetraverser t;
    t_outconnect *connection;
    
    /* Split selected object from uneselected ones and move it to the end. */
    
    for (y1 = glist->gl_graphics; y1; y1 = y2) {
    //
    y2 = y1->g_next;
    
    if (canvas_isObjectSelected (glist, y1)) {
        if (selectedTail) { selectedTail->g_next = y1; selectedTail = y1; y1->g_next = NULL; }
        else {
            selectedHead = selectedTail = y1; selectedTail->g_next = NULL;
        }
    } else {
        if (unselectedTail) { unselectedTail->g_next = y1; unselectedTail = y1; y1->g_next = NULL; }
        else {
            unselectedHead = unselectedTail = y1; unselectedTail->g_next = NULL;
        }
    }
    //
    }

    if (!unselectedHead) { glist->gl_graphics = selectedHead; }
    else {
        glist->gl_graphics = unselectedHead; unselectedTail->g_next = selectedHead; 
    }

    buffer_reset (glist->gl_editor->e_buffer);
    
    canvas_traverseLinesStart (&t, glist);
    
    while (connection = canvas_traverseLinesNext (&t)) {
    //
    int s1 = canvas_isObjectSelected (glist, cast_gobj (t.tr_srcObject));
    int s2 = canvas_isObjectSelected (glist, cast_gobj (t.tr_destObject));
    
    if (s1 != s2) {
    //
    buffer_vAppend (glist->gl_editor->e_buffer, "ssiiii;",
        sym___hash__X, 
        sym_connect,
        canvas_getIndexOfObject (glist, cast_gobj (t.tr_srcObject)),
        t.tr_srcIndexOfOutlet,
        canvas_getIndexOfObject (glist, cast_gobj (t.tr_destObject)),
        t.tr_destIndexOfInlet);
    //
    }
    //
    }
}

void canvas_restoreCachedLines (t_glist *glist)
{
    t_pd *t = s__X.s_thing;
    
    s__X.s_thing = cast_pd (glist);
    buffer_eval (glist->gl_editor->e_buffer, NULL, 0, NULL);
    
    s__X.s_thing = t;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

void canvas_removeSelectedObjects (t_glist *glist)
{
    t_gobj *y1 = NULL;
    t_gobj *y2 = NULL;
        
    int state = dsp_suspend();
    
    /* If box text is selected, deselecting it might recreate the object. */ 
    /* Workaround by deselecting it first and looking for a "new" object next. */
        
    if (glist->gl_editor->e_selectedText) {
        pd_newest = NULL;
        canvas_deselectAll (glist);
        if (pd_newest) {
            for (y1 = glist->gl_graphics; y1; y1 = y1->g_next) {
                if (cast_pd (y1) == pd_newest) { canvas_selectObject (glist, y1); }
            }
        }
    }
    
    for (y1 = glist->gl_graphics; y1; y1 = y2) {
        y2 = y1->g_next;
        if (canvas_isObjectSelected (glist, y1)) { glist_delete (glist, y1); }
    }

    dsp_resume (state);
    
    canvas_dirty (glist, 1);
}

void canvas_removeSelectedLine (t_glist *glist)
{
    if (glist->gl_editor->e_isSelectedline) {
    //
    canvas_disconnect (glist, 
        glist->gl_editor->e_selectedLineIndexOfObjectOut,
        glist->gl_editor->e_selectedLineIndexOfOutlet,
        glist->gl_editor->e_selectedLineIndexOfObjectIn,
        glist->gl_editor->e_selectedLineIndexOfInlet);
             
    canvas_dirty (glist, 1);
    //
    }
}

void canvas_displaceSelectedObjects (t_glist *glist, int deltaX, int deltaY)
{
    t_selection *y = NULL;
    
    int needToResortInlets  = 0;
    int needToResortOutlets = 0;
    int isDirty = 0;
    
    for (y = glist->gl_editor->e_selectedObjects; y; y = y->sel_next) {
        gobj_displace (y->sel_what, glist, deltaX, deltaY);
        if (pd_class (y->sel_what) == vinlet_class)  { needToResortInlets  = 1; }
        if (pd_class (y->sel_what) == voutlet_class) { needToResortOutlets = 1; }
        isDirty = 1;
    }
    
    if (needToResortInlets)  { canvas_resortinlets (glist); }
    if (needToResortOutlets) { canvas_resortoutlets(glist); }
    
    sys_vGui ("::ui_patch::updateScrollRegion .x%lx.c\n", glist);
    
    if (isDirty) { canvas_dirty (glist, 1); }
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

int canvas_isObjectSelected (t_glist *glist, t_gobj *y)
{
    if (glist->gl_editor) {
    //
    t_selection *selection = NULL;
    for (selection = glist->gl_editor->e_selectedObjects; selection; selection = selection->sel_next) {
        if (selection->sel_what == y) { return 1; }
    }
    //
    }
    
    return 0;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

static void canvas_selectingByLasso (t_glist *glist, int positionX, int positionY, int close)
{
    if (close) {
    
        int a = PD_MIN (glist->gl_editor->e_previousX, positionX);
        int c = PD_MAX (glist->gl_editor->e_previousX, positionX);
        int b = PD_MIN (glist->gl_editor->e_previousY, positionY);
        int d = PD_MAX (glist->gl_editor->e_previousY, positionY);

        canvas_selectObjectsInRectangle (glist, a, b, c, d);
        glist->gl_editor->e_action = ACTION_NONE;
        
        sys_vGui (".x%lx.c delete TEMPORARY\n", glist);
        
    } else {
        sys_vGui (".x%lx.c coords TEMPORARY %d %d %d %d\n",
                        glist,
                        glist->gl_editor->e_previousX,
                        glist->gl_editor->e_previousY,
                        positionX,
                        positionY);
    }
}

void canvas_selectingByLassoStart (t_glist *glist, int positionX, int positionY)
{
    canvas_selectingByLasso (glist, positionX, positionY, 0);
}

void canvas_selectingByLassoEnd (t_glist *glist, int positionX, int positionY)
{
    canvas_selectingByLasso (glist, positionX, positionY, 1);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

void canvas_selectObjectsInRectangle (t_glist *glist, int a, int b, int c, int d)
{
    t_gobj *y;
    
    for (y = glist->gl_graphics; y; y = y->g_next) {
    //
    int x1, y1, x2, y2;
    
    gobj_getRectangle (y, glist, &x1, &y1, &x2, &y2);
    
    if (c >= x1 && a <= x2 && d >= y1 && b <= y2 && !canvas_isObjectSelected (glist, y)) {
        canvas_selectObject (glist, y);
    }
    //
    }
}

void canvas_selectObject (t_glist *glist, t_gobj *y)
{
    t_selection *selection = (t_selection *)PD_MEMORY_GET (sizeof (t_selection));
    
    if (glist->gl_editor->e_isSelectedline) { canvas_deselectLine (glist); }

    PD_ASSERT (!canvas_isObjectSelected (glist, y));    /* Must NOT be already selected. */
    
    selection->sel_next = glist->gl_editor->e_selectedObjects;
    selection->sel_what = y;
    
    glist->gl_editor->e_selectedObjects = selection;
    
    gobj_select (y, glist, 1);
}

void canvas_selectObjectIfNotSelected (t_glist *glist, t_gobj *y)
{
    if (!canvas_isObjectSelected (glist, y)) {
        canvas_deselectAll (glist);
        canvas_selectObject (glist, y);
    }
}

void canvas_selectLine (t_glist *glist,
    t_outconnect *connection,
    int indexOfObjectOut,
    int indexOfOutlet,
    int indexOfObjectIn,
    int indexOfInlet)
{
    canvas_deselectAll (glist);
        
    glist->gl_editor->e_isSelectedline                  = 1;
    glist->gl_editor->e_selectedLineIndexOfObjectOut    = indexOfObjectOut;
    glist->gl_editor->e_selectedLineIndexOfOutlet       = indexOfOutlet;
    glist->gl_editor->e_selectedLineIndexOfObjectIn     = indexOfObjectIn;
    glist->gl_editor->e_selectedLineIndexOfInlet        = indexOfInlet;
    glist->gl_editor->e_selectedLineConnection          = connection;
    
    sys_vGui (".x%lx.c itemconfigure %lxLINE -fill blue\n",
                glist,
                glist->gl_editor->e_selectedLineConnection);  
}

void canvas_deselectObject (t_glist *glist, t_gobj *y)
{
    int dspSuspended = 0;
    
    t_boxtext *z = NULL;
    
    t_selection *selection1 = NULL;
    t_selection *selection2 = NULL;

    PD_ASSERT (canvas_isObjectSelected (glist, y));     /* Must be already selected. */
    
    if (glist->gl_editor->e_selectedText) {
    
        t_boxtext *text = glist_findrtext (glist, cast_object (y));
        
        if (glist->gl_editor->e_selectedText == text) {
            if (glist->gl_editor->e_isTextDirty) {
                z = text;
                canvas_cacheLines (canvas_getView (glist));
                canvas_deselectAllRecursive (y);
            }
            gobj_activate (y, glist, 0);
        }
        
        if (class_hasMethod (pd_class (y), sym_dsp)) { dspSuspended = dsp_suspend(); }
    }
    
    selection1 = glist->gl_editor->e_selectedObjects;
    
    if (selection1->sel_what == y) {
        glist->gl_editor->e_selectedObjects = glist->gl_editor->e_selectedObjects->sel_next;
        gobj_select (y, glist, 0);
        PD_MEMORY_FREE (selection1);
        
    } else {
        for (; selection2 = selection1->sel_next; selection1 = selection2) {
            if (selection2->sel_what == y) {
                selection1->sel_next = selection2->sel_next;
                gobj_select (y, glist, 0);
                PD_MEMORY_FREE (selection2);
                break;
            }
        }
    }
    
    if (z) {
        char *t = NULL;
        int size;
        rtext_gettext (z, &t, &size);
        text_setto (cast_object (y), glist, t, size);
        canvas_updateLinesByObject (glist, cast_object (y));
        glist->gl_editor->e_selectedText = 0;
    }
    
    if (dspSuspended) { dsp_resume (dspSuspended); }
}

void canvas_deselectAll (t_glist *glist)
{
    while (glist->gl_editor->e_selectedObjects) {
        canvas_deselectObject (glist, glist->gl_editor->e_selectedObjects->sel_what);
    }

    if (glist->gl_editor->e_isSelectedline) { canvas_deselectLine (glist); }
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

int canvas_getNumberOfSelectedObjects (t_glist *glist)
{
    return canvas_getIndexOfObjectAmongSelected (glist, NULL);
}

int canvas_getNumberOfUnselectedObjects (t_glist *glist)
{
    return canvas_getIndexOfObjectAmongUnselected (glist, NULL);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

int canvas_getIndexOfObjectAmongSelected (t_glist *glist, t_gobj *y)
{
    return canvas_getIndexOfObjectAmong (glist, y, 1);
}

int canvas_getIndexOfObjectAmongUnselected (t_glist *glist, t_gobj *y)
{
    return canvas_getIndexOfObjectAmong (glist, y, 0);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
