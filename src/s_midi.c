
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
#include "s_midi.h"

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

#define MIDI_QUEUE_SIZE                 1024
#define MIDI_UNDEFINED_OFFSET           (-(DBL_MAX))

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

typedef struct _midiqelem {
    double          q_time;
    int             q_portNumber;
    unsigned char   q_hasOneByte;
    unsigned char   q_byte1;
    unsigned char   q_byte2;
    unsigned char   q_byte3;
    } t_midiqelem;

typedef struct _midiparser {
    int mp_status;
    int mp_gotByte1;
    int mp_byte1;
    } t_midiparser;

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

static t_midiqelem  midi_outQueue[MIDI_QUEUE_SIZE];                         /* Shared. */
static int          midi_outHead;                                           /* Shared. */
static int          midi_outTail;                                           /* Shared. */

static t_midiqelem  midi_inQueue[MIDI_QUEUE_SIZE];                          /* Shared. */
static int          midi_inHead;                                            /* Shared. */
static int          midi_inTail;                                            /* Shared. */

static t_midiparser midi_parser[MAXIMUM_MIDI_IN];                           /* Shared. */
    
static double       midi_realTimeAtStart;                                   /* Shared. */
static double       midi_logicalTimeAtStart;                                /* Shared. */
static double       midi_dacOffset;                                         /* Shared. */
static double       midi_adcOffset;                                         /* Shared. */
static double       midi_dacNewOffset = MIDI_UNDEFINED_OFFSET;              /* Shared. */
static double       midi_adcNewOffset = MIDI_UNDEFINED_OFFSET;              /* Shared. */
static double       midi_needToUpdateTime;                                  /* Shared. */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

void midi_start (void)
{
    midi_realTimeAtStart    = sys_getRealTimeInSeconds();
    midi_logicalTimeAtStart = scheduler_getLogicalTime();
    midi_dacOffset          = 0.0;
    midi_adcOffset          = 0.0;
}

void midi_synchronise (void)
{
    double realLapse    = sys_getRealTimeInSeconds() - midi_realTimeAtStart;
    double logicalLapse = MILLISECONDS_TO_SECONDS (scheduler_getMillisecondsSince (midi_logicalTimeAtStart));
    
    double dacOffset = logicalLapse - realLapse - MICROSECONDS_TO_SECONDS (audio_getAdvanceInMicroseconds());
    double adcOffset = logicalLapse - realLapse;
    
    if (dacOffset > midi_dacNewOffset) { midi_dacNewOffset = dacOffset; }
    if (adcOffset > midi_adcNewOffset) { midi_adcNewOffset = adcOffset; }
        
    if (realLapse > midi_needToUpdateTime) {
    //
    midi_dacOffset    = midi_dacNewOffset;
    midi_adcOffset    = midi_adcNewOffset;
    midi_dacNewOffset = MIDI_UNDEFINED_OFFSET;
    midi_adcNewOffset = MIDI_UNDEFINED_OFFSET;
    
    midi_needToUpdateTime = realLapse + 1.0;
    //
    }
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

static double midi_getTimeIn (void)
{
    return (sys_getRealTimeInSeconds() + midi_adcOffset);
}

static double midi_getTimeOut (void)
{
    return (sys_getRealTimeInSeconds() + midi_dacOffset);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

static void midi_pushNextByte (int port, int a)
{
    midi_pushNextByteNative (port, a);
}

static void midi_pushNextMessage (int port, int a, int b, int c)
{
    midi_pushNextMessageNative (port, a, b, c);
}

static void midi_pushNext (void)
{
    t_midiqelem *e = midi_outQueue + midi_outTail;
        
    if (e->q_hasOneByte) { midi_pushNextByte (e->q_portNumber, e->q_byte1); }
    else {
        midi_pushNextMessage (e->q_portNumber, e->q_byte1, e->q_byte2, e->q_byte3);
    }   
    
    midi_outTail = (midi_outTail + 1 == MIDI_QUEUE_SIZE ? 0 : midi_outTail + 1);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

static void midi_dispatchNext (void)
{
    int port = midi_inQueue[midi_inTail].q_portNumber;
    int byte = midi_inQueue[midi_inTail].q_byte1;
    t_midiparser *p = midi_parser + port;
        
    PD_ASSERT (midi_inQueue[midi_inTail].q_hasOneByte);
    PD_ASSERT (port >= 0 || port < MAXIMUM_MIDI_IN);
    
    if (byte >= MIDI_CLOCK) { inmidi_realTimeIn (port, byte); }
    else {
    //
    inmidi_byte (port, byte);
    
    if (byte & 0x80) {
    
        if (byte == MIDI_TUNEREQUEST || byte == MIDI_RESERVED1 || byte == MIDI_RESERVED2) {
            p->mp_status = 0;
        } else if (byte == MIDI_STARTSYSEX) {
            inmidi_sysex (port, byte); p->mp_status = byte;
        } else if (byte == MIDI_ENDSYSEX) {
            inmidi_sysex (port, byte); p->mp_status = 0;
        } else {
            p->mp_status = byte;
        }
        
        p->mp_gotByte1 = 0;
        
    } else {
    
        int command  = (p->mp_status >= MIDI_STARTSYSEX ? p->mp_status : (p->mp_status & 0xf0));
        int channel  = (p->mp_status & 0xf);
        int byte1    = p->mp_byte1;
        
        switch (command) {
        //
        case MIDI_NOTEOFF           :   if (p->mp_gotByte1) { 
                                            inmidi_noteOn (port, channel, byte1, 0);
                                            p->mp_gotByte1 = 0; 
                                        } else {
                                            p->mp_byte1 = byte;
                                            p->mp_gotByte1 = 1;
                                        }
                                        break;
            
        case MIDI_NOTEON            :   if (p->mp_gotByte1) { 
                                            inmidi_noteOn (port, channel, byte1, byte);
                                            p->mp_gotByte1 = 0; 
                                        } else {
                                            p->mp_byte1 = byte;
                                            p->mp_gotByte1 = 1;
                                        }
                                        break;
            
        case MIDI_POLYPRESSURE      :   if (p->mp_gotByte1) {
                                            inmidi_polyPressure (port, channel, byte1, byte);
                                            p->mp_gotByte1 = 0;
                                        } else {
                                            p->mp_byte1 = byte;
                                            p->mp_gotByte1 = 1;
                                        }
                                        break;
                                    
        case MIDI_CONTROLCHANGE     :   if (p->mp_gotByte1) {
                                            inmidi_controlChange (port, channel, byte1, byte);
                                            p->mp_gotByte1 = 0;
                                        } else {
                                            p->mp_byte1 = byte;
                                            p->mp_gotByte1 = 1;
                                        }
                                        break;
                                    
        case MIDI_PROGRAMCHANGE     :   inmidi_programChange (port, channel, byte);
                                        break;
                                    
        case MIDI_AFTERTOUCH        :   inmidi_afterTouch (port, channel, byte);
                                        break;
                                    
        case MIDI_PITCHBEND         :   if (p->mp_gotByte1) {
                                            inmidi_pitchBend (port, channel, ((byte << 7) + byte1)); 
                                            p->mp_gotByte1 = 0;
                                        } else {
                                            p->mp_byte1 = byte;
                                            p->mp_gotByte1 = 1;
                                        }
                                        break;
                                    
        case MIDI_STARTSYSEX        :   inmidi_sysex (port, byte);
                                        break;
            
        default                     :   break;
        //
        }
    }
    //
    }
    
    midi_inTail = (midi_inTail + 1 == MIDI_QUEUE_SIZE ? 0 : midi_inTail + 1);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

static void midi_pollIn (void)
{
    double t = MILLISECONDS_TO_SECONDS (scheduler_getMillisecondsSince (midi_logicalTimeAtStart));

    while (midi_inHead != midi_inTail) {
    //
    if (midi_inQueue[midi_inTail].q_time <= t) { midi_dispatchNext(); }
    else { 
        break;
    }
    //
    }
}

static void midi_pollOut (void)
{
    double t = midi_getTimeOut();

    while (midi_outHead != midi_outTail) {
        if (midi_outQueue[midi_outTail].q_time <= t) { midi_pushNext(); }
        else { 
            break;
        }
    }
}

void midi_poll (void)
{
    midi_pollNative();
    midi_pollOut();
    midi_pollIn();
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

/* If FIFO is full dispatch or flush an element to make room. */

void midi_receive (int port, int byte)
{
    t_midiqelem *e = NULL;
    int newHead = (midi_inHead + 1 == MIDI_QUEUE_SIZE ? 0 : midi_inHead + 1);
    if (newHead == midi_inTail) { midi_dispatchNext(); }
    
    e = midi_inQueue + midi_inHead;
    
    e->q_portNumber = port;
    e->q_hasOneByte = 1;
    e->q_byte1      = byte;
    e->q_time       = midi_getTimeIn();
    
    midi_inHead = newHead;
    
    midi_pollIn();
}

void midi_broadcast (int port, int hasOneByte, int a, int b, int c)
{
    t_midiqelem *e = NULL;
    int newHead = (midi_outHead + 1 == MIDI_QUEUE_SIZE ? 0 : midi_outHead + 1);
    if (newHead == midi_outTail) { midi_pushNext(); }
    
    e = midi_outQueue + midi_outHead;
    
    e->q_portNumber = port;
    e->q_hasOneByte = hasOneByte;
    e->q_byte1      = a;
    e->q_byte2      = b;
    e->q_byte3      = c;
    e->q_time       = MILLISECONDS_TO_SECONDS (scheduler_getMillisecondsSince (midi_logicalTimeAtStart));
        
    midi_outHead = newHead;
    
    midi_pollOut();
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
