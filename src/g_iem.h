
/* 
    Copyright (c) 1997-2015 Miller Puckette and others.
*/

/* < https://opensource.org/licenses/BSD-3-Clause > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

/* Original "g_7_guis.h" written by Thomas Musil (c) IEM KUG Graz Austria 2000-2001. */

/* < http://iem.kug.ac.at/ > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

#ifndef __g_iem_h_
#define __g_iem_h_

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

#define IEM_GUI_COLOR_NORMAL        0
#define IEM_GUI_COLOR_SELECTED      255

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

#define IEM_GUI_DRAW_UPDATE         0
#define IEM_GUI_DRAW_MOVE           1
#define IEM_GUI_DRAW_NEW            2
#define IEM_GUI_DRAW_SELECT         3
#define IEM_GUI_DRAW_ERASE          4
#define IEM_GUI_DRAW_CONFIG         5
#define IEM_GUI_DRAW_IO             6

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

#define IS_POINTER(atom, index)     ((atom + index)->a_type == A_POINTER)
#define IS_FLOAT(atom, index)       ((atom + index)->a_type == A_FLOAT)
#define IS_SYMBOL(atom, index)      ((atom + index)->a_type == A_SYMBOL)
#define IS_DOLLAR(atom, index)      ((atom + index)->a_type == A_DOLLAR)
#define IS_DOLLSYM(atom, index)     ((atom + index)->a_type == A_DOLLSYM)

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

#define IEM_NUMBER_BUFFER_LENGTH    32

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

typedef struct _iem_fstyle_flags
{
    unsigned int x_font_style:6;            /* Unused but kept for compatibility. */
    unsigned int x_rcv_able:1;
    unsigned int x_snd_able:1;
    unsigned int x_lab_is_unique:1;
    unsigned int x_rcv_is_unique:1;
    unsigned int x_snd_is_unique:1;
    unsigned int x_lab_arg_tail_len:6;
    unsigned int x_lab_is_arg_num:6;
    unsigned int x_shiftdown:1;
    unsigned int x_selected:1;
    unsigned int x_finemoved:1;
    unsigned int x_put_in2out:1;
    unsigned int x_change:1;
    unsigned int x_thick:1;
    unsigned int x_lin0_log1:1;
    unsigned int x_steady:1;
} t_iem_fstyle_flags;

typedef struct _iem_init_symargs
{
    unsigned int x_loadinit:1;
    unsigned int x_rcv_arg_tail_len:6;
    unsigned int x_snd_arg_tail_len:6;
    unsigned int x_rcv_is_arg_num:6;
    unsigned int x_snd_is_arg_num:6;
    unsigned int x_scale:1;
    unsigned int x_flashed:1;
    unsigned int x_locked:1;
} t_iem_init_symargs;

typedef void (*t_iemfunptr)(void *x, t_glist *glist, int mode);

typedef struct _iemgui
{
    t_object           x_obj;
    t_glist            *x_glist;
    t_iemfunptr        x_draw;
    int                x_h;
    int                x_w;
    int                x_ldx;
    int                x_ldy;
    t_iem_fstyle_flags x_fsf;
    int                x_fontsize;
    t_iem_init_symargs x_isa;
    int                x_fcol;
    int                x_bcol;
    int                x_lcol;
    t_symbol           *x_snd;              /* send symbol */
    t_symbol           *x_rcv;              /* receive */
    t_symbol           *x_lab;              /* label */
    t_symbol           *x_snd_unexpanded;   /* same 3, with '$' unexpanded */
    t_symbol           *x_rcv_unexpanded;
    t_symbol           *x_lab_unexpanded;
    int                x_binbufindex;       /* where in binbuf to find these */
    int                x_labelbindex;       /* where in binbuf to find label */
} t_iemgui;

typedef struct _bng
{
    t_iemgui x_gui;
    int      x_flashed;
    int      x_flashtime_break;
    int      x_flashtime_hold;
    t_clock  *x_clock_hld;
    t_clock  *x_clock_brk;
    t_clock  *x_clock_lck;
} t_bng;

typedef struct _hslider
{
    t_iemgui x_gui;
    int      x_pos;
    int      x_val;
    int      x_lin0_log1;
    int      x_steady;
    double   x_min;
    double   x_max;
    double   x_k;
    t_float  x_fval;
} t_hslider;

typedef struct _hdial
{
    t_iemgui x_gui;
    int      x_on;
    int      x_on_old;  /* LATER delete this; it's used for old version */
    int      x_change;
    int      x_number;
    int      x_drawn;
    t_float  x_fval;
    t_atom   x_at[2];
} t_hdial;

typedef struct _toggle
{
    t_iemgui x_gui;
    t_float    x_on;
    t_float    x_nonzero;
} t_toggle;

typedef struct _my_canvas
{
    t_iemgui x_gui;
    t_atom   x_at[3];
    int      x_vis_w;
    int      x_vis_h;
} t_my_canvas;

typedef struct _vslider
{
    t_iemgui x_gui;
    int      x_pos;
    int      x_val;
    int      x_lin0_log1;
    int      x_steady;
    double   x_min;
    double   x_max;
    double   x_k;
    t_float  x_fval;
} t_vslider;

typedef struct _vu
{
    t_iemgui x_gui;
    int      x_led_size;
    int      x_peak;
    int      x_rms;
    t_float  x_fp;
    t_float  x_fr;
    int      x_scale;
    void     *x_out_rms;
    void     *x_out_peak;
    unsigned int x_updaterms:1;
    unsigned int x_updatepeak:1;
} t_vu;

typedef struct _my_numbox
{
    t_iemgui x_gui;
    t_clock  *x_clock_reset;
    t_clock  *x_clock_wait;
    double   x_val;
    double   x_min;
    double   x_max;
    double   x_k;
    int      x_lin0_log1;
    char     x_buf[IEM_NUMBER_BUFFER_LENGTH];
    int      x_numwidth;
    int      x_log_height;
} t_my_numbox;

typedef struct _vdial
{
    t_iemgui x_gui;
    int      x_on;
    int      x_on_old;
    int      x_change;
    int      x_number;
    int      x_drawn;
    t_float  x_fval;
    t_atom   x_at[2];
} t_vdial;

#define t_vradio t_vdial
#define t_hradio t_hdial

extern int sys_noloadbang;
extern int iemgui_color_hex[];
extern int iemgui_vu_db2i[];
extern int iemgui_vu_col[];
extern char *iemgui_vu_scale_str[];

int iemgui_clip_size(int size);
int iemgui_clip_font(int size);
int iemgui_modulo_color(int col);
t_symbol *iemgui_unique2dollarzero(t_symbol *s, int unique_num, int and_unique_flag);
t_symbol *iemgui_sym2dollararg(t_symbol *s, int nth_arg, int tail_len);
t_symbol *iemgui_dollarzero2unique(t_symbol *s, int unique_num);
t_symbol *iemgui_dollararg2sym(t_symbol *s, int nth_arg, int tail_len, int pargc, t_atom *pargv);
int iemgui_is_dollarzero(t_symbol *s);
int iemgui_is_dollararg(t_symbol *s, int *tail_len);
void iemgui_fetch_unique(t_iemgui *iemgui);
void iemgui_fetch_parent_args(t_iemgui *iemgui, int *pargc, t_atom **pargv);
void iemgui_verify_snd_ne_rcv(t_iemgui *iemgui);
void iemgui_all_unique2dollarzero(t_iemgui *iemgui, t_symbol **srlsym);
void iemgui_all_sym2dollararg(t_iemgui *iemgui, t_symbol **srlsym);
void iemgui_all_dollarzero2unique(t_iemgui *iemgui, t_symbol **srlsym);
t_symbol *iemgui_new_dogetname(t_iemgui *iemgui, int indx, t_atom *argv);
void iemgui_new_getnames(t_iemgui *iemgui, int indx, t_atom *argv);
void iemgui_all_dollararg2sym(t_iemgui *iemgui, t_symbol **srlsym);
void iemgui_all_col2save(t_iemgui *iemgui, int *bflcol);
void iemgui_all_colfromload(t_iemgui *iemgui, int *bflcol);
int iemgui_compatible_col(int i);
void iemgui_all_dollar2raute(t_symbol **srlsym);
void iemgui_all_raute2dollar(t_symbol **srlsym);
void iemgui_send(void *x, t_iemgui *iemgui, t_symbol *s);
void iemgui_receive(void *x, t_iemgui *iemgui, t_symbol *s);
void iemgui_label(void *x, t_iemgui *iemgui, t_symbol *s);
void iemgui_label_pos(void *x, t_iemgui *iemgui, t_symbol *s, int ac, t_atom *av);
void iemgui_label_font(void *x, t_iemgui *iemgui, t_symbol *s, int ac, t_atom *av);
void iemgui_size(void *x, t_iemgui *iemgui);
void iemgui_delta(void *x, t_iemgui *iemgui, t_symbol *s, int ac, t_atom *av);
void iemgui_pos(void *x, t_iemgui *iemgui, t_symbol *s, int ac, t_atom *av);
void iemgui_color(void *x, t_iemgui *iemgui, t_symbol *s, int ac, t_atom *av);
int iemgui_list(void *x, t_iemgui *iemgui, t_symbol *s, int ac, t_atom *av);
void iemgui_displace(t_gobj *z, t_glist *glist, int dx, int dy);
void iemgui_select(t_gobj *z, t_glist *glist, int selected);
void iemgui_delete(t_gobj *z, t_glist *glist);
void iemgui_vis(t_gobj *z, t_glist *glist, int vis);
void iemgui_save(t_iemgui *iemgui, t_symbol **srl, int *bflcol);
void iemgui_properties(t_iemgui *iemgui, t_symbol **srl);
void iemgui_dialog(t_iemgui *iemgui, t_symbol **srl, int argc, t_atom *argv);

int canvas_getdollarzero(void);
void canvas_getargs(int *argcp, t_atom **argvp);

void iem_inttosymargs(t_iem_init_symargs *symargp, int n);
int iem_symargstoint(t_iem_init_symargs *symargp);
void iem_inttofstyle(t_iem_fstyle_flags *fstylep, int n);
int iem_fstyletoint(t_iem_fstyle_flags *fstylep);

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#endif // __g_iem_h_
