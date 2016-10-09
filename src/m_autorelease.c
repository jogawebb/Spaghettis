
/* 
    Copyright (c) 1997-2016 Miller Puckette and others.
*/

/* < https://opensource.org/licenses/BSD-3-Clause > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#include "m_pd.h"
#include "m_core.h"
#include "m_macros.h"

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

extern t_pdinstance *pd_this;

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

#define AUTORELEASE_PERIOD  SECONDS_TO_MILLISECONDS (5.0)

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

static void autorelease_task (void *dummy)
{
    autorelease_drain();
    clock_delay (pd_this->pd_autorelease, AUTORELEASE_PERIOD);
}

static void autorelease_reschedule (void)
{
    clock_unset (pd_this->pd_autorelease); 
    clock_delay (pd_this->pd_autorelease, AUTORELEASE_PERIOD);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

void autorelease_run (void)
{
    pd_this->pd_autorelease = clock_new ((void *)NULL, (t_method)autorelease_task);
    clock_delay (pd_this->pd_autorelease, AUTORELEASE_PERIOD);
}

void autorelease_stop (void)
{
    autorelease_drain();
    clock_free (pd_this->pd_autorelease);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

void autorelease_drain (void)
{
    if (pd_isThingQuiet (sym__autoreleasepool)) {
    //
    pd_message (pd_getThing (sym__autoreleasepool), sym__autorelease, 0, NULL);
    //
    }   
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

void autorelease_add (t_pd *x)
{
    autorelease_reschedule(); pd_bind (x, sym__autoreleasepool);
}

void autorelease_perform (t_pd *x)
{
    pd_unbind (x, sym__autoreleasepool); pd_free (x);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
