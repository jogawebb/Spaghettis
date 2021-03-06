
/* < https://opensource.org/licenses/BSD-3-Clause > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#if 0
void test40__soundfile() {
#endif

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

TTT_BEGIN (AtomicSoundfile, 40, "Soundfile - Define")

    TTT_EXPECT (SOUNDFILE_HELPER_SIZE > 16);
    TTT_EXPECT (SOUNDFILE_HELPER_SIZE > SOUNDFILE_HEADER_WAVE);
    TTT_EXPECT (SOUNDFILE_HELPER_SIZE > SOUNDFILE_HEADER_AIFF);
    TTT_EXPECT (SOUNDFILE_HELPER_SIZE > SOUNDFILE_HEADER_NEXT);
    
    TTT_EXPECT (sizeof (t_word) == sizeof (t_float));               /* Required for encoding and decoding. */
    TTT_EXPECT (sizeof (t_word) == sizeof (double));                /* Ditto. */
    TTT_EXPECT (sizeof (t_word) == sizeof (t_float64Atomic));       /* Ditto. */
    TTT_EXPECT (sizeof (t_word) == 8);                              /* Ditto. */
    
TTT_END

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#if 0
}
#endif

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
