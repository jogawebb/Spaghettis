
/* 
    Copyright 2007-2013 William Andrew Burnson. All rights reserved.

    File modified by Nicolas Danet.
    
*/

/* < http://opensource.org/licenses/BSD-2-Clause > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

#ifndef BELLE_MODERN_MUSIC_PART_HPP
#define BELLE_MODERN_MUSIC_PART_HPP

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

/* To determine spacing between parts. */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

namespace belle {

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

class Part {

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

public:
    Part() : position_ (0.0), extra_ (0.0)
    {
    }

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

#if PRIM_CPP11

public:
    Part (const Part&) = delete;
    Part& operator = (const Part&) = delete;
    
#else

private:
    Part (const Part&);
    Part& operator = (const Part&);
    
#endif

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

public:
    void addSpacing (const Path& path)
    {
        /* Do NOT include the symbols over multiple staves to compute the spacing. */
        /* Use the bounded box for efficiency. */
        
        if (path.getProperty (mica::Large) == mica::Undefined) { path_.addRectangle (path.getBounds()); }
    }

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

public:
    double getPosition() const
    {
        return position_;
    }

    double getExtra() const
    {
        return extra_;
    }
    
    const Path& getSpacing() const
    {
        return path_;
    }
    
// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

public:
    void setPosition (double position)
    {
        position_ = position;
    }
    
    void setExtra (double extra)
    {
        extra_ = extra;
    }

    void setSpacing (const Path& path)
    {
        path_ = path;
    }
    
private:
    Path path_;
    double position_;
    double extra_;

private:
    PRIM_LEAK_DETECTOR (Part)
};

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

} // namespace belle

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
#endif // BELLE_MODERN_MUSIC_PART_HPP