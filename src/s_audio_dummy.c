
/* 
    Copyright (c) 2010 Peter Brinkmann.
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

t_error dummy_open() 
{
    return PD_ERROR_NONE;
}

void dummy_close() 
{
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

int dummy_pollDSP() 
{
    static int t = 0; t = !t; return (t ? DACS_YES : DACS_NO);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#pragma mark -

t_error dummy_getLists (char *devicesIn, 
    int *numberOfDevicesIn,
    char *devicesOut,
    int *numberOfDevicesOut,
    int *canMultiple,
    int *canCallback) 
{
    t_error err = PD_ERROR_NONE;
    
    err |= string_copy (devicesIn,  MAXIMUM_DEVICES * MAXIMUM_DESCRIPTION, "Dummy");
    err |= string_copy (devicesOut, MAXIMUM_DEVICES * MAXIMUM_DESCRIPTION, "Dummy");
    
    *numberOfDevicesIn  = 1;
    *numberOfDevicesOut = 1;
    
    *canMultiple = 0;
    *canCallback = 0;
  
    return err;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

