
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

const char *midi_nameNative (void)
{
    static const char *name = "Dummy"; return name;     /* Static. */
}

void midi_initializeNative (void)
{
}

void midi_releaseNative (void)
{
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

void midi_openNative (t_devices *p)
{
}

void midi_closeNative (void)
{
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

void midi_pushSysexNative (int port, int argc, t_atom *argv)
{
}

void midi_pushNative (int port, int status, int a, int b)
{
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

void midi_pollNative (void)
{
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

t_error midi_getListsNative (t_deviceslist *p)
{
    t_error err = PD_ERROR_NONE;
    
    err |= deviceslist_appendMidiIn (p, gensym ("Dummy Input"));
    err |= deviceslist_appendMidiOut (p, gensym ("Dummy Output"));
  
    return err;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
