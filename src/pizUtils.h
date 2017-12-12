
/* 
    Copyright (c) 2014, Nicolas Danet, < nicolas.danet@free.fr >. 
*/

/* < http://opensource.org/licenses/MIT > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

#ifndef PIZ_UTILS_H
#define PIZ_UTILS_H

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#include "pizDefine.h"

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#include "pizTypes.h"

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

PIZUInt8    pizUInt8Reversed                (PIZUInt8 v);
PIZUInt16   pizUInt16Reversed               (PIZUInt16 v);
PIZUInt32   pizUInt32Reversed               (PIZUInt32 v);
PIZUInt64   pizUInt64Reversed               (PIZUInt64 v);

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

PIZBool     pizUInt32IsPower2               (PIZUInt32 v);  /* Zero return true. */
PIZBool     pizUInt64IsPower2               (PIZUInt64 v);

long        pizUInt32LogBase2Index          (PIZUInt32 v);  /* Position of the MSB. */
long        pizUInt32NextPower2Index        (PIZUInt32 v);  /* Exponent of the smallest next >= power of 2. */
long        pizUInt64LogBase2Index          (PIZUInt64 v);
long        pizUInt64NextPower2Index        (PIZUInt64 v);

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#endif // PIZ_UTILS_H
