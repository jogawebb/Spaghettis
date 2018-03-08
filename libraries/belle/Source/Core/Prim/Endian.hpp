
/* 
    Copyright (c) 2007-2013 William Andrew Burnson.
    Copyright (c) 2013-2018 Nicolas Danet.
    
*/

/* < http://opensource.org/licenses/BSD-2-Clause > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

namespace prim {

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

struct Endian {

public:
    template < class T > static void swapIfBigEndian (T& data)
    {
        (void)data;
        
        #if PRIM_BIG_ENDIAN
        swapByteOrder (data);
        #endif
    }

    template < class T > static void swapIfLittleEndian (T& data)
    {
        (void)data;
        
        #if PRIM_LITTLE_ENDIAN
        swapByteOrder (data);
        #endif
    }

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

private:
    template < class T > static void swapByteOrder (T& data)
    {
        T v = 0;
        
        byte* oldPointer = reinterpret_cast < byte* > (&data);
        byte* newPointer = reinterpret_cast < byte* > (&v);
      
        for (size_t i = 0; i < sizeof (T); ++i) { newPointer[i] = oldPointer[sizeof (T) - i - 1]; }
        
        data = v;
    }
};

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

} // namespace prim

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
