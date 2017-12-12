
/* 
    Copyright (c) 2014, Nicolas Danet, < nicolas.danet@free.fr >. 
*/

/* < http://opensource.org/licenses/MIT > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

#ifndef PIZ_LCG_H
#define PIZ_LCG_H

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#include "pizDefine.h"

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#include "pizTime.h"

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

/* Rand48. */

/* < http://en.wikipedia.org/wiki/Linear_congruential_generator > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

// PIZUInt64

#define PIZ_RAND48_INIT(s)      ((s) = pizSeedMake() & 0xffffffffffffULL)          
#define PIZ_RAND48_NEXT(s)      ((s) = (((s) * 0x5deece66dULL + 0xbULL) & 0xffffffffffffULL))
#define PIZ_RAND48_UINT32(s)    (PIZUInt32)(PIZ_RAND48_NEXT (s) >> 16)
#define PIZ_RAND48_DOUBLE(s)    (PIZ_RAND48_UINT32 (s) * (1.0 / 4294967296.0))

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

/* Infamously bad RANDU. */

/* < http://en.wikipedia.org/wiki/RANDU > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

// PIZUInt32

#define PIZ_RANDU_INIT(s)       ((s) = ((PIZUInt32)pizSeedMake() | 1) & 0x7fffffff)    /* Odd number. */
#define PIZ_RANDU_UINT32(s)     ((s) = (65539 * (s)) & 0x7fffffff)
#define PIZ_RANDU_DOUBLE(s)     (PIZ_RANDU_UINT32 (s) * (1.0 / 2147483648.0))

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#endif // PIZ_LCG_H
