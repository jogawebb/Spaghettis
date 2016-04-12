
/* 
    Copyright (c) 1997-2015 Miller Puckette and others.
*/

/* < https://opensource.org/licenses/BSD-3-Clause > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

/* Original "g_7_guis.h" written by Thomas Musil (c) IEM KUG Graz Austria 2000-2001. */

/* Thanks to Miller Puckette, Guenther Geiger and Krzystof Czaja. */

/* < http://iem.kug.ac.at/ > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

#include "m_pd.h"
#include "m_core.h"
#include "m_macros.h"
#include "g_canvas.h"
#include "g_iem.h"

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#define IEM_PANEL_DEFAULT_WIDTH     100
#define IEM_PANEL_DEFAULT_HEIGHT    60
#define IEM_PANEL_DEFAULT_COLORS    { -233017, -1, -1 }

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

#define IEM_PANEL_MINIMUM_SIZE      1

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

static t_widgetbehavior panel_widgetBehavior;

static t_class *panel_class;

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

void panel_draw_new(t_panel *x, t_glist *glist)
{
    int xpos=text_xpix(&x->x_gui.iem_obj, glist);
    int ypos=text_ypix(&x->x_gui.iem_obj, glist);
    t_glist *canvas=glist_getcanvas(glist);

    sys_vGui(".x%lx.c create rectangle %d %d %d %d -fill #%6.6x -outline #%6.6x -tags %lxRECT\n",
             canvas, xpos, ypos,
             xpos + x->x_panelWidth, ypos + x->x_panelHeight,
             x->x_gui.iem_colorBackground, x->x_gui.iem_colorBackground, x);
    sys_vGui(".x%lx.c create rectangle %d %d %d %d -outline #%6.6x -tags %lxBASE\n",
             canvas, xpos, ypos,
             xpos + x->x_gui.iem_width, ypos + x->x_gui.iem_height,
             x->x_gui.iem_colorBackground, x);
    sys_vGui(".x%lx.c create text %d %d -text {%s} -anchor w \
             -font [::getFont %d] -fill #%6.6x -tags %lxLABEL\n",
             canvas, xpos+x->x_gui.iem_labelX, ypos+x->x_gui.iem_labelY,
             strcmp(x->x_gui.iem_label->s_name, "empty")?x->x_gui.iem_label->s_name:"",
             x->x_gui.iem_fontSize,
             x->x_gui.iem_colorLabel, x);
}

void panel_draw_move(t_panel *x, t_glist *glist)
{
    int xpos=text_xpix(&x->x_gui.iem_obj, glist);
    int ypos=text_ypix(&x->x_gui.iem_obj, glist);
    t_glist *canvas=glist_getcanvas(glist);

    sys_vGui(".x%lx.c coords %lxRECT %d %d %d %d\n",
             canvas, x, xpos, ypos, xpos + x->x_panelWidth,
             ypos + x->x_panelHeight);
    sys_vGui(".x%lx.c coords %lxBASE %d %d %d %d\n",
             canvas, x, xpos, ypos,
             xpos + x->x_gui.iem_width, ypos + x->x_gui.iem_height);
    sys_vGui(".x%lx.c coords %lxLABEL %d %d\n",
             canvas, x, xpos+x->x_gui.iem_labelX,
             ypos+x->x_gui.iem_labelY);
}

void panel_draw_erase(t_panel* x, t_glist *glist)
{
    t_glist *canvas=glist_getcanvas(glist);

    sys_vGui(".x%lx.c delete %lxBASE\n", canvas, x);
    sys_vGui(".x%lx.c delete %lxRECT\n", canvas, x);
    sys_vGui(".x%lx.c delete %lxLABEL\n", canvas, x);
}

void panel_draw_config(t_panel* x, t_glist *glist)
{
    t_glist *canvas=glist_getcanvas(glist);

    sys_vGui(".x%lx.c itemconfigure %lxRECT -fill #%6.6x -outline #%6.6x\n", canvas, x,
             x->x_gui.iem_colorBackground, x->x_gui.iem_colorBackground);
    sys_vGui(".x%lx.c itemconfigure %lxBASE -outline #%6.6x\n", canvas, x,
             x->x_gui.iem_isSelected?IEM_COLOR_SELECTED:x->x_gui.iem_colorBackground);
    sys_vGui(".x%lx.c itemconfigure %lxLABEL -font [::getFont %d] -fill #%6.6x -text {%s} \n",
             canvas, x, x->x_gui.iem_fontSize,
             x->x_gui.iem_colorLabel,
             strcmp(x->x_gui.iem_label->s_name, "empty")?x->x_gui.iem_label->s_name:"");
}

void panel_draw_select(t_panel* x, t_glist *glist)
{
    t_glist *canvas=glist_getcanvas(glist);

    if(x->x_gui.iem_isSelected)
    {
        sys_vGui(".x%lx.c itemconfigure %lxBASE -outline #%6.6x\n", canvas, x, IEM_COLOR_SELECTED);
    }
    else
    {
        sys_vGui(".x%lx.c itemconfigure %lxBASE -outline #%6.6x\n", canvas, x, x->x_gui.iem_colorBackground);
    }
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

void panel_draw(t_panel *x, t_glist *glist, int mode)
{
    if(mode == IEM_DRAW_MOVE)
        panel_draw_move(x, glist);
    else if(mode == IEM_DRAW_NEW)
        panel_draw_new(x, glist);
    else if(mode == IEM_DRAW_SELECT)
        panel_draw_select(x, glist);
    else if(mode == IEM_DRAW_ERASE)
        panel_draw_erase(x, glist);
    else if(mode == IEM_DRAW_CONFIG)
        panel_draw_config(x, glist);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

static void panel_dialog(t_panel *x, t_symbol *s, int argc, t_atom *argv)
{
    int a = (int)(t_int)atom_getFloatAtIndex(0, argc, argv);
    int w = (int)(t_int)atom_getFloatAtIndex(2, argc, argv);
    int h = (int)(t_int)atom_getFloatAtIndex(3, argc, argv);
    iemgui_fromDialog(&x->x_gui, argc, argv);

    x->x_gui.iem_loadbang = 0;
    if(a < 1)
        a = 1;
    x->x_gui.iem_width = a;
    x->x_gui.iem_height = x->x_gui.iem_width;
    if(w < 1)
        w = 1;
    x->x_panelWidth = w;
    if(h < 1)
        h = 1;
    x->x_panelHeight = h;
    (*x->x_gui.iem_draw) (x, x->x_gui.iem_glist, IEM_DRAW_CONFIG);
    (*x->x_gui.iem_draw) (x, x->x_gui.iem_glist, IEM_DRAW_MOVE);
}

static void panel_size(t_panel *x, t_symbol *s, int ac, t_atom *av)
{
    int i = (int)(t_int)atom_getFloatAtIndex(0, ac, av);

    if(i < 1)
        i = 1;
    x->x_gui.iem_width = i;
    x->x_gui.iem_height = i;
    iemgui_boxChanged((void *)x, &x->x_gui);
}

static void panel_delta(t_panel *x, t_symbol *s, int ac, t_atom *av)
{iemgui_movePosition((void *)x, &x->x_gui, s, ac, av);}

static void panel_pos(t_panel *x, t_symbol *s, int ac, t_atom *av)
{iemgui_setPosition((void *)x, &x->x_gui, s, ac, av);}

static void panel_label_font(t_panel *x, t_symbol *s, int ac, t_atom *av)
{iemgui_setLabelFont((void *)x, &x->x_gui, s, ac, av);}

static void panel_label_pos(t_panel *x, t_symbol *s, int ac, t_atom *av)
{iemgui_setLabelPosition((void *)x, &x->x_gui, s, ac, av);}

static void panel_vis_size(t_panel *x, t_symbol *s, int ac, t_atom *av)
{
    int i;

    i = (int)(t_int)atom_getFloatAtIndex(0, ac, av);
    if(i < 1)
        i = 1;
    x->x_panelWidth = i;
    if(ac > 1)
    {
        i = (int)(t_int)atom_getFloatAtIndex(1, ac, av);
        if(i < 1)
            i = 1;
    }
    x->x_panelHeight = i;
    if(glist_isvisible(x->x_gui.iem_glist))
        (*x->x_gui.iem_draw) (x, x->x_gui.iem_glist, IEM_DRAW_MOVE);
}

static void panel_get_pos(t_panel *x)
{
    if(x->x_gui.iem_canSend && x->x_gui.iem_send->s_thing)
    {
        x->x_t[0].a_w.w_float = text_xpix(&x->x_gui.iem_obj, x->x_gui.iem_glist);
        x->x_t[1].a_w.w_float = text_ypix(&x->x_gui.iem_obj, x->x_gui.iem_glist);
        pd_list(x->x_gui.iem_send->s_thing, 2, x->x_t);
    }
}

static void panel_send(t_panel *x, t_symbol *s)
{iemgui_setSend(x, &x->x_gui, s);}

static void panel_receive(t_panel *x, t_symbol *s)
{iemgui_setReceive(x, &x->x_gui, s);}

static void panel_label(t_panel *x, t_symbol *s)
{iemgui_setLabel((void *)x, &x->x_gui, s);}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

static void panel_getrect(t_gobj *z, t_glist *glist, int *xp1, int *yp1, int *xp2, int *yp2)
{
    t_panel *x = (t_panel *)z;
    
    *xp1 = text_xpix(&x->x_gui.iem_obj, glist);
    *yp1 = text_ypix(&x->x_gui.iem_obj, glist);
    *xp2 = *xp1 + x->x_gui.iem_width;
    *yp2 = *yp1 + x->x_gui.iem_height;
}

static void panel_behaviorSave (t_gobj *z, t_buffer *b)
{
    t_panel *x = (t_panel *)z;
    t_error err = PD_ERROR_NONE;
    t_iemnames names;
    t_iemcolors colors;

    iemgui_serialize (&x->x_gui, &names, &colors);
    
    buffer_vAppend (b, "ssiisiiisssiiiiiii",
        gensym ("#X"),
        gensym ("obj"),
        (int)cast_object (z)->te_xCoordinate,
        (int)cast_object (z)->te_yCoordinate,
        gensym ("cnv"),
        x->x_gui.iem_width,                                                     // Grip width.
        x->x_panelWidth,                                                        // Panel width.
        x->x_panelHeight,                                                       // Panel height.
        names.n_unexpandedSend,                                                 // Send.
        names.n_unexpandedReceive,                                              // Receive.
        names.n_unexpandedLabel,                                                // Label.
        x->x_gui.iem_labelX,                                                    // Label X.
        x->x_gui.iem_labelY,                                                    // Label Y.
        iemgui_serializeFontStyle (&x->x_gui),                                  // Label font.
        x->x_gui.iem_fontSize,                                                  // Label font size.
        colors.c_colorBackground,                                               // Background color.
        colors.c_colorLabel);                                                   // Label color.
        
    buffer_vAppend (b, ";");
}

static void panel_properties(t_gobj *z, t_glist *owner)
{
    t_panel *x = (t_panel *)z;
    char buf[800];
    t_symbol *srl[3];

    iemgui_serializeNames(&x->x_gui, srl);
    sprintf(buf, "::ui_iem::create %%s Panel \
            %d %d {Grip Size} 0 0 empty \
            %d {Panel Width} %d {Panel Height} \
            -1 empty empty \
            -1 \
            -1 -1 empty \
            %s %s \
            %s %d %d \
            %d \
            %d %d %d \
            -1\n",
            x->x_gui.iem_width, IEM_PANEL_MINIMUM_SIZE,
            x->x_panelWidth, x->x_panelHeight,
            srl[0]->s_name, srl[1]->s_name,
            srl[2]->s_name, x->x_gui.iem_labelX, x->x_gui.iem_labelY,
            x->x_gui.iem_fontSize,
            0xffffff & x->x_gui.iem_colorBackground, -1/*no frontcolor*/, 0xffffff & x->x_gui.iem_colorLabel);
    gfxstub_new(&x->x_gui.iem_obj.te_g.g_pd, x, buf);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

static void panel_dummy (t_panel *x, t_symbol *s, int argc, t_atom *argv)
{
    /* Dummy. */
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

static void *panel_new (t_symbol *s, int argc, t_atom *argv)
{
    t_panel *x = (t_panel *)pd_new (panel_class);
    
    int gripSize        = IEM_DEFAULT_SIZE;
    int panelWidth      = IEM_PANEL_DEFAULT_WIDTH;
    int panelHeight     = IEM_PANEL_DEFAULT_HEIGHT;
    int labelX          = IEM_DEFAULT_LABELX_TOP;
    int labelY          = IEM_DEFAULT_LABELY_TOP;
    int labelFontSize   = IEM_DEFAULT_FONTSIZE;
    t_iemcolors colors  = IEM_PANEL_DEFAULT_COLORS;
        
    if (argc >= 12                                                              // --
            && IS_FLOAT (argv + 0)                                              // Grip width.
            && IS_FLOAT (argv + 1)                                              // Panel width.
            && IS_FLOAT (argv + 2)                                              // Panel Height.
            && (IS_SYMBOL (argv + 3) || IS_FLOAT (argv + 3))                    // Send.
            && (IS_SYMBOL (argv + 4) || IS_FLOAT (argv + 4))                    // Receive.
            && (IS_SYMBOL (argv + 5) || IS_FLOAT (argv + 5))                    // Label.
            && IS_FLOAT (argv + 6)                                              // Label X.
            && IS_FLOAT (argv + 7)                                              // Label Y.
            && IS_FLOAT (argv + 8)                                              // Label font.
            && IS_FLOAT (argv + 9)                                              // Label font size.
            && IS_FLOAT (argv + 10)                                             // Background color.
            && IS_FLOAT (argv + 11))                                            // Label color.
    {
        gripSize                    = (int)atom_getFloatAtIndex (0,  argc, argv);
        panelWidth                  = (int)atom_getFloatAtIndex (1,  argc, argv);
        panelHeight                 = (int)atom_getFloatAtIndex (2,  argc, argv);
        labelX                      = (int)atom_getFloatAtIndex (6,  argc, argv);
        labelY                      = (int)atom_getFloatAtIndex (7,  argc, argv);
        labelFontSize               = (int)atom_getFloatAtIndex (9,  argc, argv);
        colors.c_colorBackground    = (int)atom_getFloatAtIndex (10, argc, argv);
        colors.c_colorLabel         = (int)atom_getFloatAtIndex (11, argc, argv);
        
        iemgui_deserializeNamesByIndex (&x->x_gui, 3, argv);
        iemgui_deserializeFontStyle (&x->x_gui, (int)atom_getFloatAtIndex (8, argc, argv));
                
    } else {
        iemgui_deserializeNamesByIndex (&x->x_gui, 3, NULL);
    }

    x->x_gui.iem_glist      = (t_glist *)canvas_getcurrent();
    x->x_gui.iem_draw       = (t_iemfn)panel_draw;
    x->x_gui.iem_canSend    = (x->x_gui.iem_send == iemgui_empty()) ? 0 : 1;
    x->x_gui.iem_canReceive = (x->x_gui.iem_receive == iemgui_empty()) ? 0 : 1;

    x->x_gui.iem_width      = PD_MAX (gripSize, IEM_PANEL_MINIMUM_SIZE);
    x->x_gui.iem_height     = PD_MAX (gripSize, IEM_PANEL_MINIMUM_SIZE);
    x->x_gui.iem_labelX     = labelX;
    x->x_gui.iem_labelY     = labelY;
    x->x_gui.iem_fontSize   = PD_MAX (labelFontSize, IEM_MINIMUM_FONTSIZE);
    
    iemgui_checkSendReceiveLoop (&x->x_gui);
    iemgui_deserializeColors (&x->x_gui, &colors);
    
    if (x->x_gui.iem_canReceive) { pd_bind (cast_pd (x), x->x_gui.iem_receive); }
    
    x->x_panelWidth  = PD_MAX (panelWidth,  IEM_PANEL_MINIMUM_SIZE);
    x->x_panelHeight = PD_MAX (panelHeight, IEM_PANEL_MINIMUM_SIZE);

    SET_FLOAT (&x->x_t[0], 0.0);
    SET_FLOAT (&x->x_t[1], 0.0);
    
    return x;
}

static void panel_free (t_panel *x)
{
    if (x->x_gui.iem_canReceive) { pd_unbind (cast_object (x), x->x_gui.iem_receive); }
    
    gfxstub_deleteforkey ((void *)x);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

void panel_setup (void)
{
    t_class *c = NULL;
    
    c = class_new (gensym ("cnv"), 
        (t_newmethod)panel_new,
        (t_method)panel_free,
        sizeof (t_panel), 
        CLASS_DEFAULT | CLASS_NOINLET,
        A_GIMME,
        A_NULL);
        
    class_addMethod (c, (t_method)panel_dialog,             gensym ("dialog"),          A_GIMME, A_NULL);
    class_addMethod (c, (t_method)panel_size,               gensym ("size"),            A_GIMME, A_NULL);
    class_addMethod (c, (t_method)panel_delta,              gensym ("move"),            A_GIMME, A_NULL);
    class_addMethod (c, (t_method)panel_pos,                gensym ("position"),        A_GIMME, A_NULL);
    class_addMethod (c, (t_method)panel_label_pos,          gensym ("labelposition"),   A_GIMME, A_NULL);
    class_addMethod (c, (t_method)panel_label_font,         gensym ("labelfont"),       A_GIMME, A_NULL);
    class_addMethod (c, (t_method)panel_vis_size,           gensym ("gripsize"),        A_GIMME, A_NULL);
    class_addMethod (c, (t_method)panel_get_pos,            gensym ("getposition"),     A_NULL);
    class_addMethod (c, (t_method)panel_send,               gensym ("send"),            A_DEFSYMBOL, A_NULL);
    class_addMethod (c, (t_method)panel_receive,            gensym ("receive"),         A_DEFSYMBOL, A_NULL);
    class_addMethod (c, (t_method)panel_label,              gensym ("label"),           A_DEFSYMBOL, A_NULL);

    #if PD_WITH_LEGACY
    
    class_addMethod (c, (t_method)panel_delta,              gensym ("delta"),           A_GIMME, A_NULL);
    class_addMethod (c, (t_method)panel_pos,                gensym ("pos"),             A_GIMME, A_NULL);
    class_addMethod (c, (t_method)panel_dummy,              gensym ("color"),           A_GIMME, A_NULL);
    class_addMethod (c, (t_method)panel_label_pos,          gensym ("label_pos"),       A_GIMME, A_NULL);
    class_addMethod (c, (t_method)panel_label_font,         gensym ("label_font"),      A_GIMME, A_NULL);
    class_addMethod (c, (t_method)panel_vis_size,           gensym ("vis_size"),        A_GIMME, A_NULL);
    class_addMethod (c, (t_method)panel_get_pos,            gensym ("get_pos"),         A_NULL);
        
    class_addCreator ((t_newmethod)panel_new, gensym ("my_canvas"), A_GIMME, A_NULL);
        
    #endif
    
    panel_widgetBehavior.w_getrectfn    = panel_getrect;
    panel_widgetBehavior.w_displacefn   = iemgui_behaviorDisplace;
    panel_widgetBehavior.w_selectfn     = iemgui_behaviorSelected;
    panel_widgetBehavior.w_activatefn   = NULL;
    panel_widgetBehavior.w_deletefn     = iemgui_behaviorDeleted;
    panel_widgetBehavior.w_visfn        = iemgui_behaviorVisible;
    panel_widgetBehavior.w_clickfn      = NULL;
    
    class_setWidgetBehavior (c, &panel_widgetBehavior);
    class_setHelpName (c, gensym ("cnv"));
    class_setSaveFunction (c, panel_behaviorSave);
    class_setPropertiesFunction (c, panel_properties);
    
    panel_class = c;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
