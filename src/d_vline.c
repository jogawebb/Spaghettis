
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
#include "d_dsp.h"

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

/* -------------------------- vline~ ------------------------------ */
static t_class *vline_tilde_class;
#include "s_system.h"    /* for AUDIO_DEFAULT_BLOCKSIZE; this should be in m_pd.h */
typedef struct _vseg
{
    double s_targettime;
    double s_starttime;
    t_sample s_target;
    struct _vseg *s_next;
} t_vseg;

typedef struct _vline
{
    t_object x_obj;
    double x_value;
    double x_inc;
    double x_referencetime;
    double x_lastlogicaltime;
    double x_nextblocktime;
    double x_samppermsec;
    double x_msecpersamp;
    double x_targettime;
    t_sample x_target;
    t_float x_inlet1;
    t_float x_inlet2;
    t_vseg *x_list;
} t_vline;

static t_int *vline_tilde_perform(t_int *w)
{
    t_vline *x = (t_vline *)(w[1]);
    t_float *out = (t_float *)(w[2]);
    int n = (int)(w[3]), i;
    double f = x->x_value;
    double inc = x->x_inc;
    double msecpersamp = x->x_msecpersamp;
    double samppermsec = x->x_samppermsec;
    double timenow, logicaltimenow = scheduler_getMillisecondsSince(x->x_referencetime);
    t_vseg *s = x->x_list;
    if (logicaltimenow != x->x_lastlogicaltime)
    {
        int sampstotime = (n > AUDIO_DEFAULT_BLOCKSIZE ? n : AUDIO_DEFAULT_BLOCKSIZE);
        x->x_lastlogicaltime = logicaltimenow;
        x->x_nextblocktime = logicaltimenow - sampstotime * msecpersamp;
    }
    timenow = x->x_nextblocktime;
    x->x_nextblocktime = timenow + n * msecpersamp;
    for (i = 0; i < n; i++)
    {
        double timenext = timenow + msecpersamp;
    checknext:
        if (s)
        {
            /* has starttime elapsed?  If so update value and increment */
            if (s->s_starttime < timenext)
            {
                if (x->x_targettime <= timenext)
                    f = x->x_target, inc = 0;
                    /* if zero-length segment bash output value */
                if (s->s_targettime <= s->s_starttime)
                {
                    f = s->s_target;
                    inc = 0;
                }
                else
                {
                    double incpermsec = (s->s_target - f)/
                        (s->s_targettime - s->s_starttime);
                    f = f + incpermsec * (timenext - s->s_starttime);
                    inc = incpermsec * msecpersamp;
                }
                x->x_inc = inc;
                x->x_target = s->s_target;
                x->x_targettime = s->s_targettime;
                x->x_list = s->s_next;
                PD_MEMORY_FREE(s);
                s = x->x_list;
                goto checknext;
            }
        }
        if (x->x_targettime <= timenext)
            f = x->x_target, inc = x->x_inc = 0, x->x_targettime = 1e20;
        *out++ = f;
        f = f + inc;
        timenow = timenext;
    }
    x->x_value = f;
    return (w+4);
}

static void vline_tilde_stop(t_vline *x)
{
    t_vseg *s1, *s2;
    for (s1 = x->x_list; s1; s1 = s2)
        s2 = s1->s_next, PD_MEMORY_FREE(s1);
    x->x_list = 0;
    x->x_inc = 0;
    x->x_inlet1 = x->x_inlet2 = 0;
    x->x_target = x->x_value;
    x->x_targettime = 1e20;
}

static void vline_tilde_float(t_vline *x, t_float f)
{
    double timenow = scheduler_getMillisecondsSince(x->x_referencetime);
    t_float inlet1 = (x->x_inlet1 < 0 ? 0 : x->x_inlet1);
    t_float inlet2 = x->x_inlet2;
    double starttime = timenow + inlet2;
    t_vseg *s1, *s2, *deletefrom = 0, *snew;
    if (PD_BIG_OR_SMALL(f))
        f = 0;

        /* negative delay input means stop and jump immediately to new value */
    if (inlet2 < 0)
    {
        x->x_value = f;
        vline_tilde_stop(x);
        return;
    }
    snew = (t_vseg *)PD_MEMORY_GET(sizeof(*snew));
        /* check if we supplant the first item in the list.  We supplant
        an item by having an earlier starttime, or an equal starttime unless
        the equal one was instantaneous and the new one isn't (in which case
        we'll do a jump-and-slide starting at that time.) */
    if (!x->x_list || x->x_list->s_starttime > starttime ||
        (x->x_list->s_starttime == starttime &&
            (x->x_list->s_targettime > x->x_list->s_starttime || inlet1 <= 0)))
    {
        deletefrom = x->x_list;
        x->x_list = snew;
    }
    else
    {
        for (s1 = x->x_list; s2 = s1->s_next; s1 = s2)
        {
            if (s2->s_starttime > starttime ||
                (s2->s_starttime == starttime &&
                    (s2->s_targettime > s2->s_starttime || inlet1 <= 0)))
            {
                deletefrom = s2;
                s1->s_next = snew;
                goto didit;
            }
        }
        s1->s_next = snew;
        deletefrom = 0;
    didit: ;
    }
    while (deletefrom)
    {
        s1 = deletefrom->s_next;
        PD_MEMORY_FREE(deletefrom);
        deletefrom = s1;
    }
    snew->s_next = 0;
    snew->s_target = f;
    snew->s_starttime = starttime;
    snew->s_targettime = starttime + inlet1;
    x->x_inlet1 = x->x_inlet2 = 0;
}

static void vline_tilde_dsp(t_vline *x, t_signal **sp)
{
    dsp_add(vline_tilde_perform, 3, x, sp[0]->s_vector, sp[0]->s_vectorSize);
    x->x_samppermsec = ((double)(sp[0]->s_sampleRate)) / 1000;
    x->x_msecpersamp = ((double)1000) / sp[0]->s_sampleRate;
}

static void *vline_tilde_new(void)
{
    t_vline *x = (t_vline *)pd_new(vline_tilde_class);
    outlet_new(&x->x_obj, &s_signal);
    inlet_newFloat(&x->x_obj, &x->x_inlet1);
    inlet_newFloat(&x->x_obj, &x->x_inlet2);
    x->x_inlet1 = x->x_inlet2 = 0;
    x->x_value = x->x_inc = 0;
    x->x_referencetime = x->x_lastlogicaltime = x->x_nextblocktime =
        scheduler_getLogicalTime();
    x->x_list = 0;
    x->x_samppermsec = 0;
    x->x_targettime = 1e20;
    return x;
}

void vline_tilde_setup(void)
{
    vline_tilde_class = class_new(sym_vline__tilde__, vline_tilde_new, 
        (t_method)vline_tilde_stop, sizeof(t_vline), 0, 0);
    class_addFloat(vline_tilde_class, (t_method)vline_tilde_float);
    class_addMethod(vline_tilde_class, (t_method)vline_tilde_dsp,
        sym_dsp, A_CANT, 0);
    class_addMethod(vline_tilde_class, (t_method)vline_tilde_stop,
        sym_stop, 0);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
