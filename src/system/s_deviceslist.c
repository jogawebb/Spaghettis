
/* Copyright (c) 1997-2019 Miller Puckette and others. */

/* < https://opensource.org/licenses/BSD-3-Clause > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#include "../m_spaghettis.h"
#include "../m_core.h"
#include "../s_system.h"

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

void deviceslist_init (t_deviceslist *p)
{
    int i;

    p->d_sampleRate         = AUDIO_DEFAULT_SAMPLERATE;
    p->d_inSize             = 0;
    p->d_outSize            = 0;

    for (i = 0; i < DEVICES_MAXIMUM_IO; i++) {
    //
    p->d_inChannels[i]      = 0;
    p->d_outChannels[i]     = 0;
    p->d_inNames[i]         = NULL;
    p->d_outNames[i]        = NULL;
    //
    }
}

void deviceslist_copy (t_deviceslist *dest, t_deviceslist *src)
{
    int i;
    
    dest->d_sampleRate      = src->d_sampleRate;
    dest->d_inSize          = src->d_inSize;
    dest->d_outSize         = src->d_outSize;
    
    for (i = 0; i < DEVICES_MAXIMUM_IO; i++) {
    //
    dest->d_inChannels[i]   = src->d_inChannels[i];
    dest->d_outChannels[i]  = src->d_outChannels[i];
    dest->d_inNames[i]      = src->d_inNames[i];
    dest->d_outNames[i]     = src->d_outNames[i];
    //
    }
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

int deviceslist_containsIn (t_deviceslist *p, t_symbol *device)
{
    PD_ASSERT (device);
    
    int i; for (i = 0; i < DEVICES_MAXIMUM_IO; i++) { if (p->d_inNames[i]  == device) { return i; } }
    
    return -1;
}

int deviceslist_containsOut (t_deviceslist *p, t_symbol *device)
{
    PD_ASSERT (device);
    
    int i; for (i = 0; i < DEVICES_MAXIMUM_IO; i++) { if (p->d_outNames[i] == device) { return i; } }
    
    return -1;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

void deviceslist_setSampleRate (t_deviceslist *p, int n)
{
    p->d_sampleRate = n;
}

int deviceslist_getSampleRate (t_deviceslist *p)
{
    return p->d_sampleRate;
}

int deviceslist_getInSize (t_deviceslist *p)
{
    return p->d_inSize;
}

int deviceslist_getOutSize (t_deviceslist *p)
{
    return p->d_outSize;
}

int deviceslist_getInChannelsAtIndex (t_deviceslist *p, int i)
{
    PD_ASSERT (i >= 0);
    PD_ASSERT (i < DEVICES_MAXIMUM_IO);
    
    return p->d_inChannels[i];
}

int deviceslist_getOutChannelsAtIndex (t_deviceslist *p, int i)
{
    PD_ASSERT (i >= 0);
    PD_ASSERT (i < DEVICES_MAXIMUM_IO);
    
    return p->d_outChannels[i];
}

t_symbol *deviceslist_getInAtIndex (t_deviceslist *p, int i)
{
    PD_ASSERT (i >= 0);
    PD_ASSERT (i < DEVICES_MAXIMUM_IO);
    
    return p->d_inNames[i];
}

t_symbol *deviceslist_getOutAtIndex (t_deviceslist *p, int i)
{
    PD_ASSERT (i >= 0);
    PD_ASSERT (i < DEVICES_MAXIMUM_IO);
    
    return p->d_outNames[i];
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

int deviceslist_getTotalOfChannelsIn (t_deviceslist *p)
{
    int i, k = 0;
    
    for (i = 0; i < p->d_inSize; i++) {
        int n = p->d_inChannels[i]; if (n > 0) { k += n; }
    }
    
    return k;
}

int deviceslist_getTotalOfChannelsOut (t_deviceslist *p)
{
    int i, k = 0;
    
    for (i = 0; i < p->d_outSize; i++) {
        int n = p->d_outChannels[i]; if (n > 0) { k += n; }
    }
    
    return k;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

t_error deviceslist_appendMidiIn (t_deviceslist *p, t_symbol *device)
{
    if (p->d_inSize < DEVICES_MAXIMUM_IO) {
    //
    p->d_inNames[p->d_inSize] = device;
    p->d_inSize++;
    return PD_ERROR_NONE;
    //
    }
    
    return PD_ERROR;
}

t_error deviceslist_appendMidiOut (t_deviceslist *p, t_symbol *device)
{
    if (p->d_outSize < DEVICES_MAXIMUM_IO) {
    //
    p->d_outNames[p->d_outSize] = device;
    p->d_outSize++;
    return PD_ERROR_NONE;
    //
    }
    
    return PD_ERROR;
}

t_error deviceslist_appendAudioIn (t_deviceslist *p, t_symbol *device, int channels)
{
    if (p->d_inSize < DEVICES_MAXIMUM_IO) {
    //
    int t = PD_CLAMP (channels, -DEVICES_MAXIMUM_CHANNELS, DEVICES_MAXIMUM_CHANNELS);
    p->d_inNames[p->d_inSize]    = device;
    p->d_inChannels[p->d_inSize] = t;
    p->d_inSize++;
    return PD_ERROR_NONE;
    //
    }
    
    return PD_ERROR;
}

t_error deviceslist_appendAudioOut (t_deviceslist *p, t_symbol *device, int channels)
{
    if (p->d_outSize < DEVICES_MAXIMUM_IO) {
    //
    int t = PD_CLAMP (channels, -DEVICES_MAXIMUM_CHANNELS, DEVICES_MAXIMUM_CHANNELS);
    p->d_outNames[p->d_outSize]    = device;
    p->d_outChannels[p->d_outSize] = t;
    p->d_outSize++;
    return PD_ERROR_NONE;
    //
    }
    
    return PD_ERROR;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
