
/* < https://opensource.org/licenses/BSD-3-Clause > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

#if 0
void test20__seed() {   /* PRNG seeding with time. */
#endif

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

TTT_BEGIN (TimeSeed, 20, "Time - Seed")

    t_seed seed1, seed2;
    
    int i;
    int failed = 0;

    for (i = 0; i < 100000; i++) {
    //
    seed1 = time_makeRandomSeed();
    seed2 = time_makeRandomSeed();

    failed += (seed1 == seed2);     /* Expect consecutive seeds NOT equals. */
    //
    }

    TTT_EXPECT (failed == 0);

TTT_END

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#if 0
}
#endif

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#if 0
void test21__time() {    /* Relative Time. */
#endif

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

TTT_BEGIN (Time, 21, "Time - Time")

    t_nano ns, elapsed;
    t_time time1, time2;

    ns = 1000000000ULL;
    
    time_set (&time1);
    
    time2 = time1;
    
    TTT_EXPECT (time_elapsedNanoseconds (&time1, &time2, &elapsed) != PD_ERROR_NONE);
    TTT_EXPECT (elapsed == 0);

    time_addNanoseconds (&time2, ns);

    TTT_EXPECT (time_elapsedNanoseconds (&time2, &time1, &elapsed) != PD_ERROR_NONE);
    TTT_EXPECT (time_elapsedNanoseconds (&time1, &time2, &elapsed) == PD_ERROR_NONE);
    TTT_EXPECT (elapsed == ns);

    time_set (&time1);
    nano_sleep (ns);
    time_set (&time2);

    TTT_EXPECT (time_elapsedNanoseconds (&time1, &time2, &elapsed) == PD_ERROR_NONE);
    
    TTT_EXPECT (PD_NANOSECONDS_TO_SECONDS (elapsed) > 0.99);
    TTT_EXPECT (PD_NANOSECONDS_TO_SECONDS (elapsed) < 1.01);

TTT_END

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#if 0
}
#endif

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#if 0
void test22__NTP() {    /* NTP Timestamps. */
#endif

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

TTT_BEGIN (TimeStamps, 22, "Time - NTP")

    t_nano ns, elapsed;
    t_stamp stamp1, stamp2;

    ns = 1000000000ULL;
    
    stamp_set (&stamp1);

    stamp2 = stamp1;

    TTT_EXPECT (stamp_elapsedNanoseconds (&stamp1, &stamp2, &elapsed) != PD_ERROR_NONE);
    TTT_EXPECT (elapsed == 0);

    stamp_addNanoseconds (&stamp2, ns);

    TTT_EXPECT (stamp_elapsedNanoseconds (&stamp2, &stamp1, &elapsed) != PD_ERROR_NONE);
    TTT_EXPECT (stamp_elapsedNanoseconds (&stamp1, &stamp2, &elapsed) == PD_ERROR_NONE);
    
    TTT_EXPECT (elapsed == (ns - 1));       /* Rounding error! */

    stamp_set (&stamp1);
    nano_sleep (ns);
    stamp_set (&stamp2);

    TTT_EXPECT (stamp_elapsedNanoseconds (&stamp1, &stamp2, &elapsed) == PD_ERROR_NONE);
    
    TTT_EXPECT (PD_NANOSECONDS_TO_SECONDS (elapsed) > 0.99);
    TTT_EXPECT (PD_NANOSECONDS_TO_SECONDS (elapsed) < 1.01);
    
TTT_END

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#if 0
}
#endif

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#if 0
void test23__size() {    /* Size of. */
#endif

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

TTT_BEGIN (TimeSize, 23, "Time - Size")

    TTT_EXPECT (sizeof (t_stamp) == sizeof (t_id));    /* Stamp conversion is used for identifiers. */

TTT_END

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#if 0
}
#endif

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
