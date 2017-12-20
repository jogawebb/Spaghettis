
/* 
    Copyright 2007-2013 William Andrew Burnson. All rights reserved.

    File modified by Nicolas Danet.
    
*/

/* < http://opensource.org/licenses/BSD-2-Clause > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#include "Mica.hpp"

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#include "MicaInternal.hpp"

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

namespace mica {

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

using namespace internal;

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

const UUID UUID::Undefined = { 0ULL, 0ULL };

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

namespace {

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

template < int N > Concept mapping (Concept (&array)[N])
{
    /* Sort concepts. */
    
    for (int i = 1; i < N; ++i) {
    //
    for (int j = 0; j < i; ++j) {
    //
    if (array[j] > array[i]) { std::swap (array[j], array[i]); }
    //
    }
    //
    }
    
    /* Iterative hash. */
    
    Concept hashed = array[0];
    
    for (int i = 1; i < N; ++i) { hashed = UUID::merge (hashed, array[i]); }
    
    /* Database lookup. */
    
    return MapToConcept::find (hashed);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

} // namespace

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

Concept::Concept (const prim::String& key) 
{
    prim::String s ("en:");
    s << key;
    
    *this = KeyToConcept::find (s.toCString());
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

int Concept::length() const
{
    int n = static_cast < int > (ConceptToLength::find (*this));
    
    return prim::Math::abs (n);
}

bool Concept::isSequence() const
{
    return (ConceptToLength::find (*this) != 0);
}
  
bool Concept::isCyclic() const
{
    return (ConceptToLength::find (*this) < 0);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

prim::String Concept::toString() const
{
    if (isUndefined())    { return prim::String ("(Undefined)"); } 
    else if (isInteger()) { return prim::String ("(Integer)"); }
    else if (isNumber())  { return prim::String ("(Ratio)"); }
    else {
    //
    const char* s = ConceptLanguageToKey::find (*this, "en");
    
    if (*s != 0) {
        while (*s) { if (*s++ == ':') { break; } }
        return prim::String (s);
    }
    //
    }
    
    return prim::String ("(Wrong)");
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

Concept map (Concept a, Concept b)
{
    Concept temp[] = { a, b };     return mapping (temp);
}

Concept map (Concept a, Concept b, Concept c)
{
    Concept temp[] = { a, b, c };  return mapping (temp);
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

Concept index (Concept sequence, Concept element)
{
    return SequenceConceptToIndex::find (sequence, element);
}
  
Concept index (Concept sequence, Concept origin, Concept element)
{
    Concept o = index (sequence, origin);
    Concept e = index (sequence, element);
    
    if (!o.isInteger() || !e.isInteger()) { return Concept(); }
    else {
        UUID uuid = { static_cast < uint64 > (e.numerator() - o.numerator()), 1ULL };
        return uuid;
    }
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

Concept item (Concept sequence, int64 i)
{
    int64 n = ConceptToLength::find (sequence);
    
    if (n < 0) {    /* Cyclic? */
        n = -n;
        if (i >= 0) { i = i % n; }
        else { 
            i = (n - ((-i) % n)) % n; 
        }
    }
    
    return SequenceIndexToConcept::find (sequence, i);
}

Concept item (Concept sequence, Concept origin, int64 i)
{
    Concept o = index (sequence, origin);
    
    if (o.isInteger()) { return item (sequence, o.numerator() + i); }
    else {
        return Concept();
    }
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

} // namespace mica

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

prim::String& operator << (prim::String& s, mica::Concept concept)
{
    return s << concept.toString().toCString();
}

std::ostream& operator << (std::ostream& stream, mica::Concept concept)
{
    return stream << concept.toString().toCString();
}

std::ostream& operator << (std::ostream& stream, prim::String& s)
{
    return stream << s.toCString();
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------