
/* Copyright (c) 1997-2019 Miller Puckette and others. */

/* < https://opensource.org/licenses/BSD-3-Clause > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

#include "../../m_spaghettis.h"
#include "../../m_core.h"
#include "../../s_system.h"
#include "../../d_dsp.h"

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

static int dsp_status;          /* Static. */
static int dsp_suspended;       /* Static. */
static int dsp_count;           /* Static. */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

static void dsp_report (void)
{
    if (symbol_hasThingQuiet (sym__dspstatus)) {
    //
    pd_float (symbol_getThing (sym__dspstatus), (t_float)dsp_status);
    //
    }
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

void dsp_setState (int n)
{
    n = (n != 0);
    
    if (n != dsp_status) {
    //
    instance_deselectAllObjects();
    
    if (n) { if (audio_start() == PD_ERROR_NONE) { instance_dspStart(); dsp_status = 1; } }
    else {
        instance_dspStop(); dsp_status = 0; audio_stop();
    }
    
    gui_vAdd ("set ::var(isDsp) %d\n", dsp_status);                                 // --
    
    if (dsp_status) { post ("dsp: start"); } else { post ("dsp: stop"); }           // --
    
    dsp_report();
    //
    }
}

int dsp_getState (void)
{
    return dsp_status;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

void dsp_update (void)
{
    dsp_resume (dsp_suspend());
}

/* Note that DSP chain is NOT suspended while the new chain is builded. */

int dsp_suspend (void)
{
    dsp_count++;    /* Check that each suspend and resume calls are balanced. */
    
    if (dsp_status && !dsp_suspended) { dsp_suspended = 1; return 1; }
    
    return 0;
}

void dsp_resume (int n)
{
    dsp_count--; PD_ASSERT (dsp_count >= 0);
    
    if (n) { dsp_suspended = 0; if (dsp_status) { instance_dspStart(); } }
}

void dsp_close (void)
{
    PD_ASSERT (dsp_count == 0); instance_dspStop(); dsp_status = 0; instance_dspFree();
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
