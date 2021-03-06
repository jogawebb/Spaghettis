
/* < https://opensource.org/licenses/BSD-3-Clause > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

/* Compare Mersenne Twister outputs with valid known outputs. */

/* < http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/MT2002/CODES/mt19937ar.out > */
/* < http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/mt19937-64.out.txt > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

#if PD_32BIT

#include "t_randomMT32.h"

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#if 0
void test0__outputMT32() {
#endif

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

TTT_BEGIN (RandomOutMT32, 0, "Random - Output MT32")

    uint32_t init[4] = { 0x123, 0x234, 0x345, 0x456 };

    MTState32 *rnd = NULL;

    TTT_EXPECT ((rnd = genrand32_newByArray (4, init)));

    if (rnd) {

        int i;
        
        for (i = 0; i < 1000; i++) {
            uint32_t n = genrand32_int32 (rnd);
            TTT_EXPECT (n == test_genrand_int32[i]);
        }

        for (i = 0; i < 1000; i++) {
            double f = genrand32_real2 (rnd);
            double g = test_genrand_real2[i];
            TTT_EXPECT (PD_ABS (f - g) < 1E-6);
        }
    }

    genrand32_free (rnd);

TTT_END

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#if 0
}
#endif

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#endif // PD_32BIT

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#if PD_64BIT

#include "t_randomMT64.h"

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#if 0
void test0__outputMT64() {
#endif

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

TTT_BEGIN (RandomOutMT64, 0, "Random - Output MT64")

    uint64_t init[4] = { 0x12345ULL, 0x23456ULL, 0x34567ULL, 0x45678ULL };

    MTState64 *rnd = NULL;

    TTT_EXPECT ((rnd = genrand64_newByArray (4L, init)));

    if (rnd) {

        int i;
        
        for (i = 0; i < 1000; i++) {
            uint64_t n = genrand64_int64 (rnd);
            TTT_EXPECT (n == test_genrand64_int64[i]);
        }
        for (i = 0; i < 1000; i++) {
            double f = genrand64_real2 (rnd);
            double g = test_genrand64_real2[i];
            TTT_EXPECT (PD_ABS (f - g) < 1E-6);
        }
    }

    genrand64_free (rnd);

TTT_END

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#if 0
}
#endif

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#endif // PD_64BIT

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
