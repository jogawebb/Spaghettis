
/* Copyright (c) 1997-2018 Miller Puckette and others. */

/* < https://opensource.org/licenses/BSD-3-Clause > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

#ifndef __m_symbols_h_
#define __m_symbols_h_

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

void symbols_initialize (void);

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

extern t_symbol s_pointer;
extern t_symbol s_float;
extern t_symbol s_symbol;
extern t_symbol s_bang;
extern t_symbol s_list;
extern t_symbol s_anything;
extern t_symbol s_signal;
extern t_symbol s__N;
extern t_symbol s__X;
extern t_symbol s__A;
extern t_symbol s_;

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

extern t_symbol *sym___TEMPLATE__float;
extern t_symbol *sym___TEMPLATE__float__dash__array;
extern t_symbol *sym___ampersand__;
extern t_symbol *sym___ampersand____ampersand__;
extern t_symbol *sym___arrobe__;
extern t_symbol *sym___arrobe__0;
extern t_symbol *sym___arrobe__1;
extern t_symbol *sym___arrobe__2;
extern t_symbol *sym___arrobe__3;
extern t_symbol *sym___arrobe__4;
extern t_symbol *sym___arrobe__5;
extern t_symbol *sym___arrobe__6;
extern t_symbol *sym___arrobe__7;
extern t_symbol *sym___arrobe__8;
extern t_symbol *sym___arrobe__9;
extern t_symbol *sym___arrobe__a;
extern t_symbol *sym___arrobe__b;
extern t_symbol *sym___arrobe__c;
extern t_symbol *sym___arrobe__d;
extern t_symbol *sym___arrobe__e;
extern t_symbol *sym___arrobe__f;
extern t_symbol *sym___asterisk__;
extern t_symbol *sym___asterisk____tilde__;
extern t_symbol *sym___bar__;
extern t_symbol *sym___bar____bar__;
extern t_symbol *sym___comma__;
extern t_symbol *sym___dash__;
extern t_symbol *sym___dash____dash__;
extern t_symbol *sym___dash__1;
extern t_symbol *sym___dash__aif;
extern t_symbol *sym___dash__aiff;
extern t_symbol *sym___dash__big;
extern t_symbol *sym___dash__binary;
extern t_symbol *sym___dash__bytes;
extern t_symbol *sym___dash__color;
extern t_symbol *sym___dash__curve;
extern t_symbol *sym___dash__discard;
extern t_symbol *sym___dash__elapsed;
extern t_symbol *sym___dash__field;
extern t_symbol *sym___dash__fieldw;
extern t_symbol *sym___dash__fieldx;
extern t_symbol *sym___dash__fieldy;
extern t_symbol *sym___dash__fill;
extern t_symbol *sym___dash__fillcolor;
extern t_symbol *sym___dash__format;
extern t_symbol *sym___dash__frames;
extern t_symbol *sym___dash__global;
extern t_symbol *sym___dash__increment;
extern t_symbol *sym___dash__keep;
extern t_symbol *sym___dash__label;
extern t_symbol *sym___dash__little;
extern t_symbol *sym___dash__next;
extern t_symbol *sym___dash__nextstep;
extern t_symbol *sym___dash__none;
extern t_symbol *sym___dash__normalize;
extern t_symbol *sym___dash__resize;
extern t_symbol *sym___dash__samplerate;
extern t_symbol *sym___dash__skip;
extern t_symbol *sym___dash__symbol;
extern t_symbol *sym___dash__template;
extern t_symbol *sym___dash__udp;
extern t_symbol *sym___dash__unit;
extern t_symbol *sym___dash__visible;
extern t_symbol *sym___dash__width;
extern t_symbol *sym___dash__wait;
extern t_symbol *sym___dash__wave;
extern t_symbol *sym___dash__x;
extern t_symbol *sym___dash__y;
extern t_symbol *sym___dot__;
extern t_symbol *sym___equals____equals__;
extern t_symbol *sym___exclamation____equals__;
extern t_symbol *sym___greater__;
extern t_symbol *sym___greater____tilde__;
extern t_symbol *sym___greater____equals__;
extern t_symbol *sym___greater____greater__;
extern t_symbol *sym___hash__A;
extern t_symbol *sym___hash__N;
extern t_symbol *sym___hash__X;
extern t_symbol *sym___less__;
extern t_symbol *sym___less____tilde__;
extern t_symbol *sym___less____equals__;
extern t_symbol *sym___less____less__;
extern t_symbol *sym___minus__;
extern t_symbol *sym___minus____tilde__;
extern t_symbol *sym___percent__;
extern t_symbol *sym___percent__s;
extern t_symbol *sym___plus__;
extern t_symbol *sym___plus____tilde__;
extern t_symbol *sym___point__aif;
extern t_symbol *sym___point__aiff;
extern t_symbol *sym___point__snd;
extern t_symbol *sym___point__wav;
extern t_symbol *sym___question__;
extern t_symbol *sym___semicolon__;
extern t_symbol *sym___slash__;
extern t_symbol *sym___slash____tilde__;
extern t_symbol *sym__addline;
extern t_symbol *sym__array;
extern t_symbol *sym__arraydialog;
extern t_symbol *sym__audiodialog;
extern t_symbol *sym__audioproperties;
extern t_symbol *sym__autorelease;
extern t_symbol *sym__back;
extern t_symbol *sym__bendin;
extern t_symbol *sym__callback;
extern t_symbol *sym__cancel;
extern t_symbol *sym__canvasdialog;
extern t_symbol *sym__copy;
extern t_symbol *sym__ctlin;
extern t_symbol *sym__cut;
extern t_symbol *sym__duplicate;
extern t_symbol *sym__dummy;
extern t_symbol *sym__dspstatus;
extern t_symbol *sym__floatarraytemplate;
extern t_symbol *sym__floattemplate;
extern t_symbol *sym__font;
extern t_symbol *sym__front;
extern t_symbol *sym__gatomdialog;
extern t_symbol *sym__graph;
extern t_symbol *sym__grid;
extern t_symbol *sym__help;
extern t_symbol *sym__iemdialog;
extern t_symbol *sym__inlet2;
extern t_symbol *sym__inlet3;
extern t_symbol *sym__key;
extern t_symbol *sym__keyup;
extern t_symbol *sym__keyname;
extern t_symbol *sym__map;
extern t_symbol *sym__mididialog;
extern t_symbol *sym__midiin;
extern t_symbol *sym__midiproperties;
extern t_symbol *sym__midirealtimein;
extern t_symbol *sym__modified;
extern t_symbol *sym__motion;
extern t_symbol *sym__mousedown;
extern t_symbol *sym__mouseup;
extern t_symbol *sym__notein;
extern t_symbol *sym__paste;
extern t_symbol *sym__path;
extern t_symbol *sym__pgmin;
extern t_symbol *sym__polling;
extern t_symbol *sym__polytouchin;
extern t_symbol *sym__popupdialog;
extern t_symbol *sym__properties;
extern t_symbol *sym__quit;
extern t_symbol *sym__savepreferences;
extern t_symbol *sym__savetofile;
extern t_symbol *sym__scalardialog;
extern t_symbol *sym__selectall;
extern t_symbol *sym__signoff;
extern t_symbol *sym__snap;
extern t_symbol *sym__sysexin;
extern t_symbol *sym__touchin;
extern t_symbol *sym__window;
extern t_symbol *sym_a;
extern t_symbol *sym_abs;
extern t_symbol *sym_abs__tilde__;
extern t_symbol *sym_acoustic;
extern t_symbol *sym_acoustic__tilde__;
extern t_symbol *sym_adc__tilde__;
extern t_symbol *sym_add;
extern t_symbol *sym_addcomma;
extern t_symbol *sym_adddollar;
extern t_symbol *sym_adddollarsymbol;
extern t_symbol *sym_addsemicolon;
extern t_symbol *sym_aiff;
extern t_symbol *sym_amptodb;
extern t_symbol *sym_amptodb__tilde__;
extern t_symbol *sym_append;
extern t_symbol *sym_apply;
extern t_symbol *sym_arguments;
extern t_symbol *sym_arithmetic;
extern t_symbol *sym_arithmetic__tilde__;
extern t_symbol *sym_array;
extern t_symbol *sym_array__space__get;
extern t_symbol *sym_array__space__max;
extern t_symbol *sym_array__space__min;
extern t_symbol *sym_array__space__quantile;
extern t_symbol *sym_array__space__random;
extern t_symbol *sym_array__space__set;
extern t_symbol *sym_array__space__size;
extern t_symbol *sym_array__space__sum;
extern t_symbol *sym_atan;
extern t_symbol *sym_atan2;
extern t_symbol *sym_atom;
extern t_symbol *sym_audio;
extern t_symbol *sym_automatic;
extern t_symbol *sym_b;
extern t_symbol *sym_backgroundcolor;
extern t_symbol *sym_bag;
extern t_symbol *sym_bang__tilde__;
extern t_symbol *sym_bendin;
extern t_symbol *sym_bendout;
extern t_symbol *sym_big;
extern t_symbol *sym_bindlist;
extern t_symbol *sym_biquad__tilde__;
extern t_symbol *sym_bitwise;
extern t_symbol *sym_block__tilde__;
extern t_symbol *sym_blockinfo__tilde__;
extern t_symbol *sym_blocksize;
extern t_symbol *sym_bng;
extern t_symbol *sym_bp__tilde__;
extern t_symbol *sym_bundle;
extern t_symbol *sym_buttonsnumber;
extern t_symbol *sym_bytes;
extern t_symbol *sym_cancel;
extern t_symbol *sym_canvas;
extern t_symbol *sym_canvasmaker;
extern t_symbol *sym_catch__tilde__;
extern t_symbol *sym_change;
extern t_symbol *sym_channels;
extern t_symbol *sym_circle;
extern t_symbol *sym_class;
extern t_symbol *sym_clear;
extern t_symbol *sym_click;
extern t_symbol *sym_clip;
extern t_symbol *sym_clip__tilde__;
extern t_symbol *sym_close;
extern t_symbol *sym_closebang;
extern t_symbol *sym_cnv;
extern t_symbol *sym_color;
extern t_symbol *sym_comment;
extern t_symbol *sym_concept;
extern t_symbol *sym_connect;
extern t_symbol *sym_console;
extern t_symbol *sym_constant;
extern t_symbol *sym_coords;
extern t_symbol *sym_cos;
extern t_symbol *sym_cos__tilde__;
extern t_symbol *sym_cosinesum;
extern t_symbol *sym_count;
extern t_symbol *sym_counter;
extern t_symbol *sym_cpole__tilde__;
extern t_symbol *sym_cputime;
extern t_symbol *sym_ctlin;
extern t_symbol *sym_ctlout;
extern t_symbol *sym_curve;
extern t_symbol *sym_cyclic;
extern t_symbol *sym_czero__tilde__;
extern t_symbol *sym_czero_rev__tilde__;
extern t_symbol *sym_d;
extern t_symbol *sym_dac__tilde__;
extern t_symbol *sym_data;
extern t_symbol *sym_dbtoamp;
extern t_symbol *sym_dbtoamp__tilde__;
extern t_symbol *sym_dbtopow;
extern t_symbol *sym_dbtopow__tilde__;
extern t_symbol *sym_dbtorms;
extern t_symbol *sym_dbtorms__tilde__;
extern t_symbol *sym_define;
extern t_symbol *sym_del;
extern t_symbol *sym_delay;
extern t_symbol *sym_delete;
extern t_symbol *sym_delread__tilde__;
extern t_symbol *sym_delread4__tilde__;
extern t_symbol *sym_delwrite__tilde__;
extern t_symbol *sym_denominator;
extern t_symbol *sym_deselect;
extern t_symbol *sym_dial;
extern t_symbol *sym_direction;
extern t_symbol *sym_dirty;
extern t_symbol *sym_disable;
extern t_symbol *sym_discard;
extern t_symbol *sym_disconnect;
extern t_symbol *sym_displace;
extern t_symbol *sym_distance;
extern t_symbol *sym_div;
extern t_symbol *sym_draw;
extern t_symbol *sym_drawcircle;
extern t_symbol *sym_drawcurve;
extern t_symbol *sym_drawnumber;
extern t_symbol *sym_drawpolygon;
extern t_symbol *sym_drawtext;
extern t_symbol *sym_drawsymbol;
extern t_symbol *sym_dsp;
extern t_symbol *sym_dspstatus;
extern t_symbol *sym_editmode;
extern t_symbol *sym_elapsed;
extern t_symbol *sym_element;
extern t_symbol *sym_empty;
extern t_symbol *sym_enable;
extern t_symbol *sym_endianness;
extern t_symbol *sym_env__tilde__;
extern t_symbol *sym_exp;
extern t_symbol *sym_exp__tilde__;
extern t_symbol *sym_expansion;
extern t_symbol *sym_expr;
extern t_symbol *sym_expression;
extern t_symbol *sym_f;
extern t_symbol *sym_false;
extern t_symbol *sym_fft__tilde__;
extern t_symbol *sym_field;
extern t_symbol *sym_filledcurve;
extern t_symbol *sym_filledpolygon;
extern t_symbol *sym_flashtime;
extern t_symbol *sym_float;
extern t_symbol *sym_floatatom;
extern t_symbol *sym_floatinlet;
extern t_symbol *sym_flush;
extern t_symbol *sym_foregroundcolor;
extern t_symbol *sym_format;
extern t_symbol *sym_frames;
extern t_symbol *sym_framp__tilde__;
extern t_symbol *sym_freeze;
extern t_symbol *sym_fromsymbol;
extern t_symbol *sym_fswap;
extern t_symbol *sym_ftom;
extern t_symbol *sym_ftom__tilde__;
extern t_symbol *sym_garray;
extern t_symbol *sym_gatom;
extern t_symbol *sym_get;
extern t_symbol *sym_getposition;
extern t_symbol *sym_getsize;
extern t_symbol *sym_graph;
extern t_symbol *sym_group;
extern t_symbol *sym_head;
extern t_symbol *sym_help;
extern t_symbol *sym_hip__tilde__;
extern t_symbol *sym_hold;
extern t_symbol *sym_hradio;
extern t_symbol *sym_hslider;
extern t_symbol *sym_i;
extern t_symbol *sym_ifft__tilde__;
extern t_symbol *sym_index;
extern t_symbol *sym_info;
extern t_symbol *sym_initialize;
extern t_symbol *sym_inlet;
extern t_symbol *sym_inlet__tilde__;
extern t_symbol *sym_insert;
extern t_symbol *sym_int;
extern t_symbol *sym_integer;
extern t_symbol *sym_interval;
extern t_symbol *sym_invalid;
extern t_symbol *sym_item;
extern t_symbol *sym_iterate;
extern t_symbol *sym_key;
extern t_symbol *sym_keyname;
extern t_symbol *sym_keyup;
extern t_symbol *sym_l;
extern t_symbol *sym_length;
extern t_symbol *sym_line;
extern t_symbol *sym_line__tilde__;
extern t_symbol *sym_linear;
extern t_symbol *sym_linewidth;
extern t_symbol *sym_list__space__append;
extern t_symbol *sym_list__space__fromsymbol;
extern t_symbol *sym_list__space__group;
extern t_symbol *sym_list__space__iterate;
extern t_symbol *sym_list__space__length;
extern t_symbol *sym_list__space__prepend;
extern t_symbol *sym_list__space__split;
extern t_symbol *sym_list__space__store;
extern t_symbol *sym_list__space__tosymbol;
extern t_symbol *sym_list__space__trim;
extern t_symbol *sym_listen;
extern t_symbol *sym_listinlet;
extern t_symbol *sym_little;
extern t_symbol *sym_loadbang;
extern t_symbol *sym_loader;
extern t_symbol *sym_localhost;
extern t_symbol *sym_log;
extern t_symbol *sym_log__tilde__;
extern t_symbol *sym_logarithmic;
extern t_symbol *sym_logged;
extern t_symbol *sym_logical;
extern t_symbol *sym_logical__tilde__;
extern t_symbol *sym_lop__tilde__;
extern t_symbol *sym_lrshift__tilde__;
extern t_symbol *sym_mag__tilde__;
extern t_symbol *sym_makefilename;
extern t_symbol *sym_makenote;
extern t_symbol *sym_map;
extern t_symbol *sym_math;
extern t_symbol *sym_math__tilde__;
extern t_symbol *sym_max;
extern t_symbol *sym_max__tilde__;
extern t_symbol *sym_message;
extern t_symbol *sym_messageresponder;
extern t_symbol *sym_metro;
extern t_symbol *sym_mica;
extern t_symbol *sym_mica__space__get;
extern t_symbol *sym_mica__space__index;
extern t_symbol *sym_mica__space__info;
extern t_symbol *sym_mica__space__interval;
extern t_symbol *sym_mica__space__item;
extern t_symbol *sym_mica__space__map;
extern t_symbol *sym_mica__space__sequence;
extern t_symbol *sym_mica__space__set;
extern t_symbol *sym_mica__space__spell;
extern t_symbol *sym_midiclkin;
extern t_symbol *sym_midiin;
extern t_symbol *sym_midiout;
extern t_symbol *sym_midirealtimein;
extern t_symbol *sym_millisecond;
extern t_symbol *sym_min;
extern t_symbol *sym_min__tilde__;
extern t_symbol *sym_minute;
extern t_symbol *sym_mod;
extern t_symbol *sym_mode;
extern t_symbol *sym_moses;
extern t_symbol *sym_move;
extern t_symbol *sym_msg;
extern t_symbol *sym_mtof;
extern t_symbol *sym_mtof__tilde__;
extern t_symbol *sym_multiple;
extern t_symbol *sym_n;
extern t_symbol *sym_name;
extern t_symbol *sym_namecanvas;
extern t_symbol *sym_nbx;
extern t_symbol *sym_near;
extern t_symbol *sym_netreceive;
extern t_symbol *sym_netsend;
extern t_symbol *sym_new;
extern t_symbol *sym_next;
extern t_symbol *sym_noise__tilde__;
extern t_symbol *sym_nonzero;
extern t_symbol *sym_normalize;
extern t_symbol *sym_notein;
extern t_symbol *sym_noteout;
extern t_symbol *sym_null;
extern t_symbol *sym_number;
extern t_symbol *sym_numerator;
extern t_symbol *sym_obj;
extern t_symbol *sym_object;
extern t_symbol *sym_objectmaker;
extern t_symbol *sym_octaves;
extern t_symbol *sym_open;
extern t_symbol *sym_openpanel;
extern t_symbol *sym_osc__tilde__;
extern t_symbol *sym_oscbundle;
extern t_symbol *sym_oscformat;
extern t_symbol *sym_oscparse;
extern t_symbol *sym_oscstream;
extern t_symbol *sym_outlet;
extern t_symbol *sym_outlet__tilde__;
extern t_symbol *sym_p;
extern t_symbol *sym_pak;
extern t_symbol *sym_pack;
extern t_symbol *sym_pad;
extern t_symbol *sym_path;
extern t_symbol *sym_pd;
extern t_symbol *sym_period;
extern t_symbol *sym_permillisecond;
extern t_symbol *sym_perminute;
extern t_symbol *sym_persecond;
extern t_symbol *sym_phasor__tilde__;
extern t_symbol *sym_pipe;
extern t_symbol *sym_pgmin;
extern t_symbol *sym_pgmout;
extern t_symbol *sym_plot;
extern t_symbol *sym_pointerinlet;
extern t_symbol *sym_poly;
extern t_symbol *sym_polygon;
extern t_symbol *sym_polytouchin;
extern t_symbol *sym_polytouchout;
extern t_symbol *sym_position;
extern t_symbol *sym_pow;
extern t_symbol *sym_pow__tilde__;
extern t_symbol *sym_powtodb;
extern t_symbol *sym_powtodb__tilde__;
extern t_symbol *sym_prepend;
extern t_symbol *sym_print;
extern t_symbol *sym_print__tilde__;
extern t_symbol *sym_proxy;
extern t_symbol *sym_qlist;
extern t_symbol *sym_quality;
extern t_symbol *sym_quantile;
extern t_symbol *sym_quit;
extern t_symbol *sym_r;
extern t_symbol *sym_r__tilde__;
extern t_symbol *sym_radio;
extern t_symbol *sym_random;
extern t_symbol *sym_range;
extern t_symbol *sym_read;
extern t_symbol *sym_readsf__tilde__;
extern t_symbol *sym_realtime;
extern t_symbol *sym_receive;
extern t_symbol *sym_receive__tilde__;
extern t_symbol *sym_recent;
extern t_symbol *sym_rename;
extern t_symbol *sym_resampling;
extern t_symbol *sym_reset;
extern t_symbol *sym_resize;
extern t_symbol *sym_restore;
extern t_symbol *sym_rewind;
extern t_symbol *sym_rfft__tilde__;
extern t_symbol *sym_rifft__tilde__;
extern t_symbol *sym_rmag__tilde__;
extern t_symbol *sym_rmstodb;
extern t_symbol *sym_rmstodb__tilde__;
extern t_symbol *sym_route;
extern t_symbol *sym_rpole__tilde__;
extern t_symbol *sym_rsqrt__tilde__;
extern t_symbol *sym_rzero__tilde__;
extern t_symbol *sym_rzero_rev__tilde__;
extern t_symbol *sym_s;
extern t_symbol *sym_s__tilde__;
extern t_symbol *sym_samphold__tilde__;
extern t_symbol *sym_sample;
extern t_symbol *sym_samplerate;
extern t_symbol *sym_samplerate__tilde__;
extern t_symbol *sym_save;
extern t_symbol *sym_saveas;
extern t_symbol *sym_savepanel;
extern t_symbol *sym_scalar;
extern t_symbol *sym_scalar__space__define;
extern t_symbol *sym_scale;
extern t_symbol *sym_scan;
extern t_symbol *sym_search;
extern t_symbol *sym_second;
extern t_symbol *sym_seed;
extern t_symbol *sym_sequence;
extern t_symbol *sym_sel;
extern t_symbol *sym_select;
extern t_symbol *sym_send;
extern t_symbol *sym_send__tilde__;
extern t_symbol *sym_serial;
extern t_symbol *sym_set;
extern t_symbol *sym_setsize;
extern t_symbol *sym_sfthread;
extern t_symbol *sym_sig__tilde__;
extern t_symbol *sym_signal;
extern t_symbol *sym_sin;
extern t_symbol *sym_sinesum;
extern t_symbol *sym_single;
extern t_symbol *sym_size;
extern t_symbol *sym_slider;
extern t_symbol *sym_snapshot__tilde__;
extern t_symbol *sym_soundfiler;
extern t_symbol *sym_soundinfo;
extern t_symbol *sym_spell;
extern t_symbol *sym_spigot;
extern t_symbol *sym_split;
extern t_symbol *sym_sqrt;
extern t_symbol *sym_sqrt__tilde__;
extern t_symbol *sym_stamp;
extern t_symbol *sym_start;
extern t_symbol *sym_steady;
extern t_symbol *sym_step;
extern t_symbol *sym_steps;
extern t_symbol *sym_stop;
extern t_symbol *sym_store;
extern t_symbol *sym_stripnote;
extern t_symbol *sym_struct;
extern t_symbol *sym_stub;
extern t_symbol *sym_style;
extern t_symbol *sym_substitution;
extern t_symbol *sym_sum;
extern t_symbol *sym_swap;
extern t_symbol *sym_switch__tilde__;
extern t_symbol *sym_symbolatom;
extern t_symbol *sym_symbolinlet;
extern t_symbol *sym_sysexin;
extern t_symbol *sym_t;
extern t_symbol *sym_tabosc4__tilde__;
extern t_symbol *sym_tabplay__tilde__;
extern t_symbol *sym_tabread;
extern t_symbol *sym_tabread__tilde__;
extern t_symbol *sym_tabread4;
extern t_symbol *sym_tabread4__tilde__;
extern t_symbol *sym_tabreceive;
extern t_symbol *sym_tabreceive__tilde__;
extern t_symbol *sym_tabsend__tilde__;
extern t_symbol *sym_tabwrite;
extern t_symbol *sym_tabwrite__tilde__;
extern t_symbol *sym_tan;
extern t_symbol *sym_template;
extern t_symbol *sym_text;
extern t_symbol *sym_text__space__define;
extern t_symbol *sym_text__space__delete;
extern t_symbol *sym_text__space__get;
extern t_symbol *sym_text__space__insert;
extern t_symbol *sym_text__space__search;
extern t_symbol *sym_text__space__sequence;
extern t_symbol *sym_text__space__set;
extern t_symbol *sym_text__space__size;
extern t_symbol *sym_textfile;
extern t_symbol *sym_tgl;
extern t_symbol *sym_threshold__tilde__;
extern t_symbol *sym_throw__tilde__;
extern t_symbol *sym_timer;
extern t_symbol *sym_timestamp;
extern t_symbol *sym_title;
extern t_symbol *sym_tosymbol;
extern t_symbol *sym_touchin;
extern t_symbol *sym_touchout;
extern t_symbol *sym_traverse;
extern t_symbol *sym_trigger;
extern t_symbol *sym_trim;
extern t_symbol *sym_true;
extern t_symbol *sym_type;
extern t_symbol *sym_undefined;
extern t_symbol *sym_unit;
extern t_symbol *sym_unpack;
extern t_symbol *sym_until;
extern t_symbol *sym_updated;
extern t_symbol *sym_uzi;
extern t_symbol *sym_v;
extern t_symbol *sym_value;
extern t_symbol *sym_vcf__tilde__;
extern t_symbol *sym_vd__tilde__;
extern t_symbol *sym_vexpr;
extern t_symbol *sym_vline__tilde__;
extern t_symbol *sym_vradio;
extern t_symbol *sym_vslider;
extern t_symbol *sym_vsnapshot__tilde__;
extern t_symbol *sym_vu;
extern t_symbol *sym_w;
extern t_symbol *sym_wave;
extern t_symbol *sym_window;
extern t_symbol *sym_wrap;
extern t_symbol *sym_wrap__tilde__;
extern t_symbol *sym_write;
extern t_symbol *sym_writesf__tilde__;
extern t_symbol *sym_x;
extern t_symbol *sym_y;
extern t_symbol *sym_z;
extern t_symbol *sym_Ascending;
extern t_symbol *sym_Array;
extern t_symbol *sym_BackSpace;
extern t_symbol *sym_Delete;
extern t_symbol *sym_Descending;
extern t_symbol *sym_Down;
extern t_symbol *sym_Enter;
extern t_symbol *sym_Escape;
extern t_symbol *sym_JACK;
extern t_symbol *sym_Left;
extern t_symbol *sym_Patch;
extern t_symbol *sym_Return;
extern t_symbol *sym_Right;
extern t_symbol *sym_Scalar;
extern t_symbol *sym_Space;
extern t_symbol *sym_Tab;
extern t_symbol *sym_Text;
extern t_symbol *sym_Up;

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#if PD_WITH_LEGACY

extern t_symbol *sym___dash__b;
extern t_symbol *sym___dash__c;
extern t_symbol *sym___dash__f;
extern t_symbol *sym___dash__g;
extern t_symbol *sym___dash__k;
extern t_symbol *sym___dash__maxsize;
extern t_symbol *sym___dash__n;
extern t_symbol *sym___dash__nframes;
extern t_symbol *sym___dash__r;
extern t_symbol *sym___dash__rate;
extern t_symbol *sym___dash__s;
extern t_symbol *sym___dash__t;
extern t_symbol *sym___dash__u;
extern t_symbol *sym___dash__v;
extern t_symbol *sym___dash__w;
extern t_symbol *sym_add2;
extern t_symbol *sym_adddollsym;
extern t_symbol *sym_addsemi;
extern t_symbol *sym_args;
extern t_symbol *sym_auto;
extern t_symbol *sym_const;
extern t_symbol *sym_delta;
extern t_symbol *sym_double_change;
extern t_symbol *sym_fromlist;
extern t_symbol *sym_get_pos;
extern t_symbol *sym_hsl;
extern t_symbol *sym_init;
extern t_symbol *sym_label;
extern t_symbol *sym_label_font;
extern t_symbol *sym_label_pos;
extern t_symbol *sym_lin;
extern t_symbol *sym_log_height;
extern t_symbol *sym_menu__dash__open;
extern t_symbol *sym_menuarray;
extern t_symbol *sym_menuclose;
extern t_symbol *sym_menusave;
extern t_symbol *sym_menusaveas;
extern t_symbol *sym_ms;
extern t_symbol *sym_msec;
extern t_symbol *sym_my_canvas;
extern t_symbol *sym_my_numbox;
extern t_symbol *sym_mycnv;
extern t_symbol *sym_numbox;
extern t_symbol *sym_page;
extern t_symbol *sym_permin;
extern t_symbol *sym_perms;
extern t_symbol *sym_permsec;
extern t_symbol *sym_persec;
extern t_symbol *sym_pos;
extern t_symbol *sym_q8_rsqrt__tilde__;
extern t_symbol *sym_q8_sqrt__tilde__;
extern t_symbol *sym_sam;
extern t_symbol *sym_samp;
extern t_symbol *sym_send__dash__window;
extern t_symbol *sym_setbounds;
extern t_symbol *sym_sec;
extern t_symbol *sym_single_change;
extern t_symbol *sym_tempo;
extern t_symbol *sym_text__space__fromlist;
extern t_symbol *sym_text__space__tolist;
extern t_symbol *sym_toggle;
extern t_symbol *sym_tolist;
extern t_symbol *sym_vis_size;
extern t_symbol *sym_vnext;
extern t_symbol *sym_vsl;
extern t_symbol *sym_vumeter;

#endif

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#endif // __m_symbols_h_
