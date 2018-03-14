
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

/* < http://www.eng.tau.ac.il/~yash/infosec-seminar/2007/Random_Bit_Generator.pdf > */
/* < http://www.cse.yorku.ca/~oz/hash.html > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

class Random {

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

/* Please note that this PRNG has NOT been intensively tested. */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

public:
    Random (const std::string& key)
    {
        std::ostringstream s;
    
        #if PRIM_WINDOWS
        s << _getpid();
        #else
        s << getpid();
        #endif 
        s << key;
        s << std::time (nullptr);
    
        std::string t = s.str();
    
        uint32 seed = 5381;
        for (int i = 0; i < (int)t.length(); ++i) { seed = ((seed << 5) + seed) + t[i]; }
        for (int i = 0; i < 5; ++i) { seed *= 29943829; seed -= 1; history_[i] = seed; }
        for (int i = 0; i < (static_cast < int > (std::clock()) % 101) + 12; ++i) { next(); }
    }

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#if PRIM_CPP11

public:
    Random (const Random&) = default;
    Random (Random&&) = default;
    Random& operator = (const Random&) = default;
    Random& operator = (Random&&) = default;

#endif

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

public:
    std::string nextID()
    {
        const char hex[] = "0123456789abcdef";
        const std::size_t n = 32;
    
        std::string s;
        
        while (s.length() < n) {
        //
        uint32 v = next();
        do {
        s += hex[static_cast < std::size_t > (v & 15)];
        v >>= 4;
        } while ((v != 0) && (s.length() < n));
        //
        }
        
        return s;
    }

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

public:
    double nextDouble()
    {
        return (next() * (1.0 / 4294967296.0));
    }
    
// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

private:
    uint32 next()
    {
        uint64 sum = 2111111111ULL * static_cast < uint64 > (history_[3]);
        sum += 1492ULL * static_cast < uint64 > (history_[2]);
        sum += 1776ULL * static_cast < uint64 > (history_[1]);
        sum += 5115ULL * static_cast < uint64 > (history_[0]);
        sum += static_cast < uint64 > (history_[4]);

        history_[3] = history_[2];
        history_[2] = history_[1];
        history_[1] = history_[0];
        history_[4] = static_cast < uint32 > (sum >> 32);
        history_[0] = static_cast < uint32 > (sum);

        return history_[0];
    }
    
private:
    uint32 history_[5];

private:
    PRIM_LEAK_DETECTOR (Random)
};

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

} // namespace prim

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
