
/* 
    Copyright (c) 2007-2013 William Andrew Burnson.
    Copyright (c) 2013-2020 Nicolas Danet.
    
*/

/* < http://opensource.org/licenses/BSD-2-Clause > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

#ifndef __MICA_LIBRARY__
#define __MICA_LIBRARY__

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

namespace mica {

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

using prim::int64;
using prim::uint64;

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

/* < https://en.wikipedia.org/wiki/Universally_unique_identifier#Version_3_.28MD5_hash_.26_namespace.29 > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

/* The UUID class is for internal usage only. */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

struct UUID {

friend class Concept;

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

public:
    bool operator < (const UUID& uuid) const
    {
        return ((high_ < uuid.high_) || (high_ == uuid.high_ && low_ < uuid.low_));
    }
  
    bool operator > (const UUID& uuid) const
    {
        return ((high_ > uuid.high_) || (high_ == uuid.high_ && low_ > uuid.low_));
    }
  
    bool operator == (const UUID& uuid) const
    {
        return (high_ == uuid.high_ && low_ == uuid.low_);
    }
  
    bool operator != (const UUID& uuid) const
    {
        return (high_ != uuid.high_ || low_ != uuid.low_);
    }

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

public:
    static UUID withHex (const std::string& s)
    {
        UUID uuid = Undefined;
    
        size_t size = s.length();
    
        if (size == 32) {
            uuid.high_ = prim::Utils::withHex (s.substr (0,  16));
            uuid.low_  = prim::Utils::withHex (s.substr (16, 16));
        }
    
        return uuid;
    }

public:
    static UUID merge (UUID a, UUID b)
    {
        UUID c = { merge (a.high_, b.high_), merge (a.low_, b.low_) };
        return c;
    }

/* < http://www.codeproject.com/Articles/25172/Simple-Random-Number-Generation > */

private:
    static uint64 merge (uint64 a, uint64 b)
    {
        const uint64 low16 = 0xffffULL;
        const uint64 low32 = 0xffffffffULL;
        
        uint64 zH = 36969 * ((a >> 32) & low16) + (a >> 48);
        uint64 wH = 18000 * ((b >> 32) & low16) + (b >> 48);
        uint64 zL = 36969 * (a & low16) + ((a & low32) >> 16);
        uint64 wL = 18000 * (b & low16) + ((b & low32) >> 16);
        
        return (((zL << 16) + wL) & low32) + (((zH << 16) + wH) << 32);
    }

public:
    static const UUID Undefined;

public:
    uint64 high_;   /* Public for convenience. */
    uint64 low_;
};

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

class Concept : public UUID {

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

public:
    Concept()
    {
        high_ = low_ = 0ULL;            /* Equal to UUID::Undefined and mica::Undefined. */
    }
    
    Concept (const UUID& uuid)
    {
        high_ = uuid.high_;
        low_  = uuid.low_;
    }
    
    Concept (prim::Ratio rational)      /* Rational is always in reduced form with a postive denominator. */
    {
        high_ = static_cast < uint64 > (rational.getNumerator());       
        low_  = static_cast < uint64 > (rational.getDenominator());
    }
    
    Concept (int64 n)
    {
        high_ = static_cast < uint64 > (n);
        low_  = 1ULL;
    }

    explicit Concept (const std::string& key);

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#if PRIM_CPP11

public:
    Concept (const Concept&) = default;
    Concept (Concept&&) = default;
    Concept& operator = (const Concept&) = default;
    Concept& operator = (Concept&&) = default;

#endif

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

public:
    std::size_t hash() const
    {
        return static_cast < std::size_t > (UUID::merge (high_, low_));
    }
    
// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

public:
    int getNumerator() const 
    {
        return static_cast < int > (numerator());
    }

    int getDenominator() const 
    {
        return static_cast < int > (denominator());
    }
    
// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

public:
    prim::Ratio toRatio() const
    {
        if (denominator() != 0) { return prim::Ratio (numerator(), denominator()); }
        else {
            return prim::Ratio::undefined();
        }
    }

    std::string toString() const;

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

public:
    bool isValid() const
    {
        return (toString() != std::string ("(Wrong)"));
    }
    
    bool isUndefined() const
    {
        return (high_ == 0ULL && low_ == 0ULL);
    }
    
    bool isInteger() const
    {
        return (isNumber() && low_ == 1ULL);
    }
    
    /* Always in reduced form with a postive denominator. */
        
    bool isNumber() const
    {
        if (static_cast < int64 > (low_) > 0) {     /* MSB is always turned on in case of concept. */
        //
        int64 n = static_cast < int64 > (high_);
        int64 d = static_cast < int64 > (low_);
        prim::Ratio t (n, d);
        return ((t.getNumerator() == n) && (t.getDenominator() == d));
        //
        }
        
        return false;      
    }
  
    bool isSequence() const;
    bool isCyclic() const;

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

public:
    int length() const;
    
// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

public:
    int64 numerator() const
    {
        return (isNumber() ? static_cast < int64 > (high_) : 0);
    }

    int64 denominator() const
    {
        return (isNumber() ? static_cast < int64 > (low_) : 0);
    }

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

public:
    static UUID asUUID (Concept concept)
    {
        UUID uuid = { concept.high_, concept.low_ }; return uuid;
    }
    
    static std::string asHex (Concept concept)
    {
        std::string a = prim::Utils::paddedLeft (prim::Utils::asHex (concept.high_), 16, '0');
        std::string b = prim::Utils::paddedLeft (prim::Utils::asHex (concept.low_),  16, '0');
    
        return a + b;
    }
    
// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

private:
    PRIM_LEAK_DETECTOR (Concept)
};

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

Concept map (Concept a, Concept b);
Concept map (Concept a, Concept b, Concept c);

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

Concept index (Concept sequence, Concept element);
Concept index (Concept sequence, Concept origin, Concept element);

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

Concept item (Concept sequence, int64 index);
Concept item (Concept sequence, Concept origin, int64 index);

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

} // namespace mica

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

#include "Data/MicaDatabase.hpp"

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

namespace mica {

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

/* < https://en.wikipedia.org/wiki/Music_information_retrieval > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#include "MIR/Utils.hpp"
#include "MIR/Interval.hpp"
#include "MIR/Tuple.hpp"
#include "MIR/Spell.hpp"

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

} // namespace mica

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#endif // __MICA_LIBRARY__
