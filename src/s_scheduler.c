
/* 
    Copyright (c) 1997-2015 Miller Puckette and others.
*/

/* < https://opensource.org/licenses/BSD-3-Clause > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#include "m_pd.h"
#include "m_core.h"
#include "m_macros.h"
#include "s_system.h"

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

#define SCHEDULER_RUN       0
#define SCHEDULER_QUIT      1
#define SCHEDULER_RESTART   2
#define SCHEDULER_ERROR     3

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

extern int      sys_schedadvance;
extern t_float  sys_dacsr;

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

extern t_pdinstance *pd_this;

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

static volatile sig_atomic_t scheduler_quit;                                /* Shared. */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

static int      scheduler_sleepGrain;                                       /* Shared. */
static int      scheduler_blockSize     = AUDIO_DEFAULT_BLOCK;              /* Shared. */
static int      scheduler_audioMode     = SCHEDULER_AUDIO_NONE;             /* Shared. */

static double   scheduler_realTime;                                         /* Shared. */
static double   scheduler_logicalTime;                                      /* Shared. */
static double   scheduler_systimePerDSPTick;                                /* Shared. */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#if PD_WATCHDOG 
#if PD_WITH_NOGUI

static int      scheduler_didDSP;                                           /* Shared. */
static int      scheduler_nextPing;                                         /* Shared. */

#endif
#endif

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

double scheduler_getLogicalTime (void)
{
    return pd_this->pd_systime;
}

double scheduler_getLogicalTimeAfter (double ms)
{
    return (pd_this->pd_systime + (SYSTIME_CLOCKS_PER_MILLISECOND * ms));
}

double scheduler_getUnitsSince (double systime, double unit, int isSamples)
{
    double d, elapsed = pd_this->pd_systime - systime;
    
    PD_ASSERT (elapsed >= 0.0);
    
    if (isSamples) { d = SYSTIME_CLOCKS_PER_SECOND / sys_dacsr; } 
    else { 
        d = SYSTIME_CLOCKS_PER_MILLISECOND;
    }
    
    return (elapsed / (d * unit));
}

double scheduler_getMillisecondsSince (double systime)
{
    double elapsed = pd_this->pd_systime - systime;
    
    PD_ASSERT (elapsed >= 0.0);
    
    return (elapsed / SYSTIME_CLOCKS_PER_MILLISECOND);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

void scheduler_setAudioMode (int flag)
{
    PD_ASSERT (flag != SCHEDULER_AUDIO_CALLBACK);           /* Not fully tested yet. */
    PD_ABORT  (flag == SCHEDULER_AUDIO_CALLBACK);

    scheduler_audioMode = flag;
}

void scheduler_needToRestart (void)
{
    scheduler_quit = SCHEDULER_RESTART;
}

void scheduler_needToExit (void)
{
    scheduler_quit = SCHEDULER_QUIT;
}

void scheduler_needToExitWithError (void)
{
    scheduler_quit = SCHEDULER_ERROR;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

static double scheduler_getSystimePerDSPTick (void)
{
    return (SYSTIME_CLOCKS_PER_SECOND * ((double)scheduler_blockSize / sys_dacsr));
}

static void scheduler_pollWatchdog (void)
{
    #if PD_WATCHDOG
    #if PD_WITH_NOGUI
        
    if ((scheduler_didDSP - scheduler_nextPing) > 0) {
        interface_watchdog (NULL);
        scheduler_nextPing = scheduler_didDSP + (2 * (int)(sys_dacsr / (double)scheduler_blockSize));
    }
    
    #endif
    #endif
}

static void scheduler_pollStuck (int init)
{
    static double idleTime;
    
    if (init) { idleTime = sys_getRealTimeInSeconds(); }
    else {
    //
    if (sys_getRealTimeInSeconds() - idleTime > 1.0) {
        sys_close_audio();
        scheduler_setAudioMode (SCHEDULER_AUDIO_NONE);
        scheduler_quit = SCHEDULER_RESTART;
        post_error (PD_TRANSLATE ("audio: I/O stuck"));     // --
    }
    //
    }
}

static void scheduler_tick (void)
{
    double nextSystime = pd_this->pd_systime + scheduler_systimePerDSPTick;
    
    while (pd_this->pd_clocks && pd_this->pd_clocks->c_systime < nextSystime) {
    //
    t_clock *c = pd_this->pd_clocks;
    pd_this->pd_systime = c->c_systime;
    clock_unset (c);
    (*c->c_fn)(c->c_owner);
    if (scheduler_quit) { return; }
    //
    }
    
    pd_this->pd_systime = nextSystime;
    
    dsp_tick();
    
    #if PD_WATCHDOG
    #if PD_WITH_NOGUI
    
    scheduler_didDSP++;
        
    #endif
    #endif
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

static void scheduler_withLoop (void)
{
    int idleCount = 0;
        
    double realTimeAtStart = sys_getRealTimeInSeconds();
    double logicalTimeAtStart  = scheduler_getLogicalTime();
    
    scheduler_sleepGrain = PD_CLAMP (sys_schedadvance / 4, 100, 5000);
    scheduler_systimePerDSPTick = scheduler_getSystimePerDSPTick();

    midi_initialize();
    
    while (!scheduler_quit) {
    //
    int timeForward, didSomething = 0;

    if (scheduler_audioMode != SCHEDULER_AUDIO_NONE) {

        timeForward = sys_send_dacs();

        if (timeForward) { idleCount = 0; }
        else if (!(++idleCount % 31)) { scheduler_pollStuck (idleCount == 32); }
        
    } else {
    
        double realLapse = SECONDS_TO_MILLISECONDS (sys_getRealTimeInSeconds() - realTimeAtStart);
        double logicalLapse = scheduler_getMillisecondsSince (logicalTimeAtStart);

        if (realLapse > logicalLapse) { timeForward = DACS_YES; }
        else {
            timeForward = DACS_NO;
        }
    }
    
    if (!scheduler_quit) {
    //
    midi_setOffsets();
    
    if (timeForward != DACS_NO)  { scheduler_tick(); }
    if (timeForward == DACS_YES) { didSomething = 1; }

    sys_pollmidiqueue();
    
    if (!scheduler_quit && interface_pollOrFlushGui()) { didSomething = 1; }

    if (!scheduler_quit && !didSomething) {
    //
    scheduler_pollWatchdog();

    if (timeForward != DACS_SLEPT) { interface_monitorBlocking (scheduler_sleepGrain); }
    //
    }
    //
    }
    //
    }
}

static void scheduler_withCallback (void)
{
    midi_initialize();
    
    while (!scheduler_quit) {
    //
    #if PD_WINDOWS
        Sleep (1000);
    #else
        sleep (1);
    #endif
    //
    }
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

void scheduler_audioCallback (void)
{
    midi_setOffsets();
    scheduler_tick();
    sys_pollmidiqueue();
    interface_pollOrFlushGui();
    scheduler_pollWatchdog();
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

t_error scheduler_main (void)
{
    while (!scheduler_quit) {
    //
    if (scheduler_audioMode == SCHEDULER_AUDIO_CALLBACK) { scheduler_withCallback(); }
    else {
        scheduler_withLoop();
    }
    
    if (scheduler_quit == SCHEDULER_RESTART) {
        if (audio_isopen()) { sys_close_audio(); sys_reopen_audio(); } scheduler_quit = SCHEDULER_RUN;
    } 
    
    if (scheduler_quit == SCHEDULER_ERROR) {
        sys_close_audio();
        sys_close_midi();
    }
    //
    }
    
    return (scheduler_quit == SCHEDULER_ERROR);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
