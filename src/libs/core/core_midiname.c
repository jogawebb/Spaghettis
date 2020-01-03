
/* Copyright (c) 1997-2020 Miller Puckette and others. */

/* < https://opensource.org/licenses/BSD-3-Clause > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#include "core_midi.h"

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

/* < http://developer.apple.com/qa/qa2004/qa1374.html > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

t_symbol *midiname_get (MIDIEndpointRef endpoint)
{
    t_symbol *name = NULL;
    
    if (endpoint) {
    //
    CFStringRef string = NULL;
    
    t_error err = coremidi_error (MIDIObjectGetStringProperty (endpoint, kMIDIPropertyDisplayName, &string));

    if (!err) { name = core_CFStringToSymbol (string); }

    CFRelease (string);
    //
    }
    
    return name;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
