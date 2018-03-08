
/* 
    Copyright (c) 2007-2013 William Andrew Burnson.
    Copyright (c) 2013-2018 Nicolas Danet.
    
*/

/* < http://opensource.org/licenses/BSD-2-Clause > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

#if defined ( _WIN32 ) || defined ( _WIN64 )
    #define PRIM_WINDOWS            1
#endif

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

#if defined ( __LP64__ ) || defined ( _LP64 )
    #define PRIM_64BIT              1
    #define PRIM_LP64               1
#elif defined ( _WIN64 )
    #define PRIM_64BIT              1
    #define PRIM_LLP64              1
#else 
    #define PRIM_32BIT              1
#endif

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

#ifdef PRIM_64BIT
#else
#ifdef PRIM_32BIT
#else
    #error "Unknown architecture!"
#endif
#endif

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#if defined ( _BIG_ENDIAN ) || defined ( __BIG_ENDIAN__ )
    #define PRIM_BIG_ENDIAN         1
#else
#if defined ( PRIM_WINDOWS ) || defined ( __LITTLE_ENDIAN__ )
    #define PRIM_LITTLE_ENDIAN      1
#else
    #include <endian.h>
    #if ( BYTE_ORDER == LITTLE_ENDIAN )
    #define PRIM_LITTLE_ENDIAN      1
    #else
    #define PRIM_BIG_ENDIAN         1
    #endif
#endif
#endif

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#ifdef PRIM_LITTLE_ENDIAN
#else
#ifdef PRIM_BIG_ENDIAN
#else
    #error "Unknown endianness!"
#endif
#endif

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

#if ( __cplusplus < 201103L )
    #define PRIM_CPP11              0
#else
    #define PRIM_CPP11              1
#endif

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

#if ! ( PRIM_CPP11 )

    #ifndef nullptr
        #define nullptr (0)         
    #endif

    #ifndef override
        #define override
    #endif

    #ifndef noexcept
        #define noexcept
    #endif

#endif

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
