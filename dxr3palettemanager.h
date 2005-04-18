/*
 * dxr3palettemanager.h:
 *
 * See the main source file 'dxr3.c' for copyright information and
 * how to reach the author.
 *
 */
 
#ifndef _DXR3PALETTEMANAGER_H_
#define _DXR3PALETTEMANAGER_H_

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

// ==================================
class cDxr3PaletteManager 
{
public:
    cDxr3PaletteManager();
    ~cDxr3PaletteManager() {};

    void AddColor(int color);
    void RemoveColor(int color);
    int GetCount();
    int operator[](int index);
    int GetIndex(int color);
    bool HasChanged();
    uint32_t* GetPalette(); 

private:
    static const int MAX_COLORS = 16;
    int m_colors[MAX_COLORS];
    uint32_t m_pal[MAX_COLORS];
    int m_users[MAX_COLORS];
    bool m_changed;
};

#endif /*_DXR3PALETTEMANAGER_H_*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// indent-tabs-mode: t
// End:
