
/* 
    Copyright (c) 1997-2016 Miller Puckette and others.
*/

/* < https://opensource.org/licenses/BSD-3-Clause > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

/* Changes by Thomas Musil IEM KUG Graz Austria 2001. */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

#include "m_pd.h"
#include "m_core.h"
#include "m_macros.h"
#include "s_system.h"
#include "s_utf8.h"
#include "g_canvas.h"

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

extern t_class  *canvas_class;
extern t_class  *gatom_class;
extern t_pd     *pd_newest;

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

t_class *text_class;                                /* Shared. */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

t_widgetbehavior text_widgetBehavior =              /* Shared. */
    {
        text_behaviorGetRectangle,
        text_behaviorDisplaced,
        text_behaviorSelected,
        text_behaviorActivated,
        text_behaviorDeleted,
        text_behaviorVisibilityChanged,
        text_behaviorClicked
    };

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

void text_behaviorGetRectangle (t_gobj *z, t_glist *glist, int *a, int *b, int *c, int *d)
{
    t_object *x = cast_object (z);
    t_boxtext *text = boxtext_fetch (glist, x);
    
    int w = boxtext_getWidth (text);
    int h = boxtext_getHeight (text);
    
    *a = text_xpix (x, glist);
    *b = text_ypix (x, glist);
    *c = *a + w;
    *d = *b + h;
}

void text_behaviorDisplaced (t_gobj *z, t_glist *glist, int deltaX, int deltaY)
{
    t_object *x = cast_object (z);
    
    x->te_xCoordinate += deltaX;
    x->te_yCoordinate += deltaY;
    
    if (canvas_isMapped (glist)) {
    //
    t_boxtext *text = boxtext_fetch (glist, x);
    boxtext_displace (text, deltaX, deltaY);
    text_drawborder (x, glist, boxtext_getTag (text), 0);
    canvas_updateLinesByObject (glist, x);
    //
    }
}

void text_behaviorSelected (t_gobj *z, t_glist *glist, int isSelected)
{
    t_object *x = cast_object (z);

    if (canvas_isMapped (glist)) {
    //
    t_boxtext *text = boxtext_fetch (glist, x);
    
    boxtext_select (text, isSelected);
    
    if (gobj_isVisible (z, glist)) {
    //
    sys_vGui (".x%lx.c itemconfigure %sBORDER -fill #%06x\n",
                    glist, 
                    boxtext_getTag (text),
                    (isSelected ? COLOR_SELECTED : COLOR_NORMAL));
    //
    }
    //
    }
}

void text_behaviorActivated (t_gobj *z, t_glist *glist, int isActivated)
{
    boxtext_activate (boxtext_fetch (glist, cast_object (z)), isActivated);
}

void text_behaviorDeleted (t_gobj *z, t_glist *glist)
{
    canvas_deleteLinesByObject (glist, cast_object (z));
}

void text_behaviorVisibilityChanged (t_gobj *z, t_glist *glist, int isVisible)
{
    t_object *x = cast_object (z);
    
    if (gobj_isVisible (z, glist)) {
    //
    t_boxtext *text = boxtext_fetch (glist, x);
    
    if (isVisible) {
        boxtext_draw (text);
        text_drawborder (x, glist, boxtext_getTag (text), 1);

    } else {
        text_eraseborder (x, glist, boxtext_getTag (text));
        boxtext_erase (text);
    }
    //
    }
}

int text_behaviorClicked (t_gobj *z,
    t_glist *glist,
    int a,
    int b,
    int shift,
    int ctrl,
    int alt,
    int dbl,
    int clicked)
{
    t_object *x = cast_object (z);
    
    t_float f1 = (t_float)a;
    t_float f2 = (t_float)b;
    t_float f3 = (t_float)shift;
    t_float f4 = (t_float)ctrl;
    t_float f5 = (t_float)alt;
    
    if (x->te_type == TYPE_OBJECT) {
        if (class_hasMethod (pd_class (x), sym_click)) {
        //
        if (clicked) { pd_vMessage (cast_pd (x), sym_click, "fffff", f1, f2, f3, f4, f5); }
        return 1;
        //
        }
        
    } else if (x->te_type == TYPE_ATOM) {
        if (clicked) { gatom_click (cast_gatom (x), f1, f2, f3, f4, f5); }
        return 1;
        
    } else if (x->te_type == TYPE_MESSAGE) {
        if (clicked) { message_click ((t_message *)x, f1, f2, f3, f4, f5); }
        return 1;
    }
    
    return 0;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

static int text_isSerializable (t_object *x)
{
    int k = class_hasMethod (pd_class (x), sym__serialize);
    
    /* Is it an abstraction or a table object? */
    
    if (k && (pd_class (x) == canvas_class)) {          
        if (canvas_isAbstraction (cast_glist (x))) { return 0; }
        else {                                                             
            if (utils_getFirstAtomOfBufferAsSymbol (x) == sym_table) { return 0; }
        }
    }
        
    return k;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

void text_save (t_gobj *z, t_buffer *b)
{
    t_object *x = cast_object (z);
    
    if (x->te_type == TYPE_OBJECT) {
    
        if (text_isSerializable (x)) {  
            mess1((t_pd *)x, sym__serialize, b);
            buffer_vAppend(b, "ssii", sym___hash__X, sym_restore,
                (int)x->te_xCoordinate, (int)x->te_yCoordinate);
        }
        else    /* otherwise just save the text */
        {
            buffer_vAppend(b, "ssii", sym___hash__X, sym_obj,
                (int)x->te_xCoordinate, (int)x->te_yCoordinate);
        }
        buffer_serialize(b, x->te_buffer);
        
    } else if (x->te_type == TYPE_MESSAGE) {
        buffer_vAppend(b, "ssii", sym___hash__X, sym_msg,
            (int)x->te_xCoordinate, (int)x->te_yCoordinate);
        buffer_serialize(b, x->te_buffer);
    }
    else if (x->te_type == TYPE_ATOM)
    {
        t_atomtype t = ((t_gatom *)x)->a_atom.a_type;
        t_symbol *sel = (t == A_SYMBOL ? sym_symbolatom :
            (t == A_FLOAT ? sym_floatatom : sym_intatom));
        
        t_symbol *label = dollar_toHash (utils_substituteIfEmpty (cast_gatom (x)->a_unexpandedLabel, 1));
        t_symbol *symfrom = dollar_toHash (utils_substituteIfEmpty (cast_gatom (x)->a_unexpandedReceive, 1));
        t_symbol *symto = dollar_toHash (utils_substituteIfEmpty (cast_gatom (x)->a_unexpandedSend, 1));
        buffer_vAppend(b, "ssiiifffsss", sym___hash__X, sel,
            (int)x->te_xCoordinate, (int)x->te_yCoordinate, (int)x->te_width,
            (double)((t_gatom *)x)->a_lowRange,
            (double)((t_gatom *)x)->a_highRange,
            (double)((t_gatom *)x)->a_position,
            label, symfrom, symto);
    }           
    else        
    {
        buffer_vAppend(b, "ssii", sym___hash__X, sym_text,
            (int)x->te_xCoordinate, (int)x->te_yCoordinate);
        buffer_serialize(b, x->te_buffer);
    }
    if (x->te_width)
        buffer_vAppend(b, ",si", sym_f, (int)x->te_width);
    buffer_vAppend(b, ";");
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

    /* draw inlets and outlets for a text object or for a graph. */
void glist_drawio(t_glist *glist, t_object *ob, int firsttime,
    char *tag, int x1, int y1, int x2, int y2)
{
    int n = object_numberOfOutlets(ob), nplus = (n == 1 ? 1 : n-1), i;
    int width = x2 - x1;
    for (i = 0; i < n; i++)
    {
        int onset = x1 + (width - INLET_WIDTH) * i / nplus;
        if (firsttime)
            sys_vGui(".x%lx.c create rectangle %d %d %d %d \
-tags [list %so%d outlet]\n",
                canvas_getView(glist),
                onset, y2 - 1,
                onset + INLET_WIDTH, y2,
                tag, i);
        else
            sys_vGui(".x%lx.c coords %so%d %d %d %d %d\n",
                canvas_getView(glist), tag, i,
                onset, y2 - 1,
                onset + INLET_WIDTH, y2);
    }
    n = object_numberOfInlets(ob);
    nplus = (n == 1 ? 1 : n-1);
    for (i = 0; i < n; i++)
    {
        int onset = x1 + (width - INLET_WIDTH) * i / nplus;
        if (firsttime)
            sys_vGui(".x%lx.c create rectangle %d %d %d %d \
-tags [list %si%d inlet]\n",
                canvas_getView(glist),
                onset, y1,
                onset + INLET_WIDTH, y1 + INLET_HEIGHT,
                tag, i);
        else
            sys_vGui(".x%lx.c coords %si%d %d %d %d %d\n",
                canvas_getView(glist), tag, i,
                onset, y1,
                onset + INLET_WIDTH, y1 + INLET_HEIGHT);
    }
}

void text_drawborder(t_object *x, t_glist *glist,
    char *tag, int firsttime)
{
    t_object *ob;
    int x1, y1, x2, y2, width, height;
    text_behaviorGetRectangle(&x->te_g, glist, &x1, &y1, &x2, &y2);
    width = x2 - x1;
    height = y2 - y1;
    
    if (x->te_type == TYPE_OBJECT)
    {
        char *pattern = ((pd_class((t_pd *)x) == text_class) ? "-" : "\"\"");
        if (firsttime)
            sys_vGui(".x%lx.c create line\
 %d %d %d %d %d %d %d %d %d %d -dash %s -tags [list %sBORDER obj]\n",
                canvas_getView(glist),
                    x1, y1,  x2, y1,  x2, y2,  x1, y2,  x1, y1,  pattern, tag);
        else
        {
            sys_vGui(".x%lx.c coords %sBORDER\
 %d %d %d %d %d %d %d %d %d %d\n",
                canvas_getView(glist), tag,
                    x1, y1,  x2, y1,  x2, y2,  x1, y2,  x1, y1);
            sys_vGui(".x%lx.c itemconfigure %sBORDER -dash %s\n",
                canvas_getView(glist), tag, pattern);
        }
    }
    else if (x->te_type == TYPE_MESSAGE)
    {
        if (firsttime)
            sys_vGui(".x%lx.c create line\
 %d %d %d %d %d %d %d %d %d %d %d %d %d %d -tags [list %sBORDER msg]\n",
                canvas_getView(glist),
                x1, y1,  x2+4, y1,  x2, y1+4,  x2, y2-4,  x2+4, y2,
                x1, y2,  x1, y1,
                    tag);
        else
            sys_vGui(".x%lx.c coords %sBORDER\
 %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
                canvas_getView(glist), tag,
                x1, y1,  x2+4, y1,  x2, y1+4,  x2, y2-4,  x2+4, y2,
                x1, y2,  x1, y1);
    }
    else if (x->te_type == TYPE_ATOM)
    {
        if (firsttime)
            sys_vGui(".x%lx.c create line\
 %d %d %d %d %d %d %d %d %d %d %d %d -tags [list %sBORDER atom]\n",
                canvas_getView(glist),
                x1, y1,  x2-4, y1,  x2, y1+4,  x2, y2,  x1, y2,  x1, y1,
                    tag);
        else
            sys_vGui(".x%lx.c coords %sBORDER\
 %d %d %d %d %d %d %d %d %d %d %d %d\n",
                canvas_getView(glist), tag,
                x1, y1,  x2-4, y1,  x2, y1+4,  x2, y2,  x1, y2,  x1, y1);
    }
        /* for comments, just draw a bar on RHS if unlocked; when a visible
        canvas is unlocked we have to call this anew on all comments, and when
        locked we erase them all via the annoying "COMMENTBAR" tag. */
    else if (x->te_type == TYPE_COMMENT && glist->gl_isEditMode)
    {
        if (firsttime)
            sys_vGui(".x%lx.c create line\
 %d %d %d %d -tags [list %sBORDER COMMENTBAR]\n",
                canvas_getView(glist),
                x2, y1,  x2, y2, tag);
        else
            sys_vGui(".x%lx.c coords %sBORDER %d %d %d %d\n",
                canvas_getView(glist), tag, x2, y1,  x2, y2);
    }
        /* draw inlets/outlets */
    
    if (ob = canvas_castToObjectIfPatchable((t_pd *)x))
        glist_drawio(glist, ob, firsttime, tag, x1, y1, x2, y2);
}

void glist_eraseio(t_glist *glist, t_object *ob, char *tag)
{
    int i, n;
    n = object_numberOfOutlets(ob);
    for (i = 0; i < n; i++)
        sys_vGui(".x%lx.c delete %so%d\n",
            canvas_getView(glist), tag, i);
    n = object_numberOfInlets(ob);
    for (i = 0; i < n; i++)
        sys_vGui(".x%lx.c delete %si%d\n",
            canvas_getView(glist), tag, i);
}

void text_eraseborder(t_object *x, t_glist *glist, char *tag)
{
    if (x->te_type == TYPE_COMMENT && !glist->gl_isEditMode) return;
    sys_vGui(".x%lx.c delete %sBORDER\n",
        canvas_getView(glist), tag);
    glist_eraseio(glist, x, tag);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

    /* change text; if TYPE_OBJECT, remake it.  */
void text_setto(t_object *x, t_glist *glist, char *buf, int bufsize)
{
    if (x->te_type == TYPE_OBJECT)
    {
        t_buffer *b = buffer_new();
        int natom1, natom2, widthwas = x->te_width;
        t_atom *vec1, *vec2;
        buffer_withStringUnzeroed(b, buf, bufsize);
        natom1 = buffer_size(x->te_buffer);
        vec1 = buffer_atoms(x->te_buffer);
        natom2 = buffer_size(b);
        vec2 = buffer_atoms(b);
            /* special case: if  pd args change just pass the message on. */
        if (natom1 >= 1 && natom2 >= 1 && vec1[0].a_type == A_SYMBOL
            && !strcmp(vec1[0].a_w.w_symbol->s_name, "pd") &&
             vec2[0].a_type == A_SYMBOL
            && !strcmp(vec2[0].a_w.w_symbol->s_name, "pd"))
        {
            pd_message((t_pd *)x, sym_rename, natom2-1, vec2+1);
            buffer_free(x->te_buffer);
            x->te_buffer = b;
        }
        else  /* normally, just destroy the old one and make a new one. */
        {
            int xwas = x->te_xCoordinate, ywas = x->te_yCoordinate;
            glist_delete(glist, &x->te_g);
            canvas_makeTextObject(glist, xwas, ywas, widthwas, 0, b);
            canvas_restoreCachedLines (canvas_getView(glist));
                /* if it's an abstraction loadbang it here */
            if (pd_newest && pd_class(pd_newest) == canvas_class)
                canvas_loadbang((t_glist *)pd_newest);
        }
            /* if we made a new "pd" or changed a window name,
                update window list */
        /*if (natom2 >= 1  && vec2[0].a_type == A_SYMBOL
            && !strcmp(vec2[0].a_w.w_symbol->s_name, "pd"))
                canvas_updatewindowlist();*/
    }
    else buffer_withStringUnzeroed(x->te_buffer, buf, bufsize);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

static void text_anything (t_object *x, t_symbol *s, int argc, t_atom *argv)
{
}

void text_setup (void)
{
    t_class *c = NULL;
    
    c = class_new (sym_text, 
            NULL,
            NULL, 
            sizeof (t_object),
            CLASS_DEFAULT | CLASS_NOINLET,
            A_NULL);
        
    class_addAnything (c, text_anything);
    
    text_class = c;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
