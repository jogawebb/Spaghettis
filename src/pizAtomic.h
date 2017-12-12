
/* 
    Copyright (c) 2014, Nicolas Danet, < nicolas.danet@free.fr >. 
*/

/* < http://opensource.org/licenses/MIT > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

#ifndef PIZ_ATOMIC_H
#define PIZ_ATOMIC_H

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#include "pizDefine.h"

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

/* Assume than 32-bit integer load and store are atomic. */

/* < http://preshing.com/20130618/atomic-vs-non-atomic-operations/ > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#if PIZ_32BIT
    #define PIZ_LOAD_STORE_32_IS_ATOMIC     1
#endif

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#if PIZ_64BIT
    #define PIZ_LOAD_STORE_32_IS_ATOMIC     1
    #define PIZ_LOAD_STORE_64_IS_ATOMIC     1
#endif

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

/* Note that the new value is returned. */
/* Note that thoses macros correspond only to relaxed atomic operations. */

/* < http://preshing.com/20130618/atomic-vs-non-atomic-operations/ > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

/* Alignment may not work on stack (don't use local atomic variables). */

/* < http://gcc.gnu.org/bugzilla/show_bug.cgi?id=16660 > */
/* < http://stackoverflow.com/questions/841433/gcc-attribute-alignedx-explanation > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#if PIZ_ATOMIC_WITH_MAC
    #include "pizAtomicMac.h"
#endif

#if PIZ_ATOMIC_WITH_POSIX
    #include "pizAtomicPosix.h"
#endif

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#endif // PIZ_ATOMIC_H
