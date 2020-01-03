
/* Copyright (c) 1997-2020 Miller Puckette and others. */

/* < https://opensource.org/licenses/BSD-3-Clause > */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

#include "../m_spaghettis.h"
#include "../m_core.h"
#include "../s_system.h"

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

#define FONT_LIST_SIZE      12

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

#define FONT_DEFAULT_SIZE   12

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

typedef struct _fontinfo {
    int     fi_size;
    double  fi_width;
    double  fi_height;
    } t_fontinfo;

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

static t_fontinfo font_fontList[FONT_LIST_SIZE] =       /* Static. */
    {
        { 6,   3.5,  7.0 },
        { 8,   5.0,  9.0 },
        { 9,   5.5, 10.0 },
        { 10,  6.0, 11.0 },
        { 11,  6.5, 13.0 },
        { 12,  7.0, 14.0 },
        { 14,  8.5, 16.0 },
        { 16,  9.5, 19.0 },
        { 18, 11.0, 21.0 },
        { 20, 12.0, 24.0 },
        { 24, 14.5, 28.0 },
        { 36, 22.0, 41.0 }
    };

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------

static int font_defaultSize;                                   /* Static. */

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

static t_fontinfo *font_getNearest (int n)
{
    t_fontinfo *info = font_fontList;
    int i;
        
    for (i = 0; i < (FONT_LIST_SIZE - 1); i++, info++) {
        if (n < (info + 1)->fi_size) {
            return info; 
        }
    }
    
    return (font_fontList + (FONT_LIST_SIZE - 1));
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

void font_setDefaultSize (int n)
{
    font_defaultSize = font_getValidSize (n);
}

int font_getDefaultSize (void)
{
    if (!font_defaultSize) { font_setDefaultSize (FONT_DEFAULT_SIZE); } return font_defaultSize;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

int font_getValidSize (int n)
{
    return font_getNearest (n)->fi_size;
}

double font_getWidth (int n)
{
    return font_getNearest (n)->fi_width;
}

double font_getHeight (int n)
{
    return font_getNearest (n)->fi_height;
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
// MARK: -

void font_withHostMeasured (int argc, t_atom *argv)
{
    int i, n = argc / 3;
    
    PD_ASSERT (n > 0);
    PD_ASSERT (argc == n * 3);

    for (i = 0; i < n; i++) {
    //
    t_float size     = atom_getFloatAtIndex ((3 * i) + 0, argc, argv);
    t_float width    = atom_getFloatAtIndex ((3 * i) + 1, argc, argv);
    t_float height   = atom_getFloatAtIndex ((3 * i) + 2, argc, argv);
    
    t_fontinfo *info = font_getNearest ((int)size);
    
    PD_ASSERT (width  > 1.0);
    PD_ASSERT (height > 1.0);
    
    info->fi_width  = width;
    info->fi_height = height;
    //
    }
}

// -----------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------
