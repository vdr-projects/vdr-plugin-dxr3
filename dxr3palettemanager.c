/*
 * dxr3palettemanager.c:
 *
 * See the main source file 'dxr3.c' for copyright information and
 * how to reach the author.
 *
 */ 

/*
	ToDo:
		- cDxr3PaletteManager: Should we use here std::vector?
*/

#include <string.h> 
#include "dxr3palettemanager.h"
#include "dxr3tools.h"

// ================================== 
//! constructor
cDxr3PaletteManager::cDxr3PaletteManager() 
{
    memset(m_colors, 0, sizeof(int) * MAX_COLORS);
    memset(m_users, 0, sizeof(int) * MAX_COLORS);
    memset(m_pal, 0, sizeof(uint32_t) * MAX_COLORS);
    m_changed = false;
};

// ==================================
void cDxr3PaletteManager::AddColor(int color) 
{
    int freeIndex = MAX_COLORS;
    bool found = false;

    for (int i = 0; i < MAX_COLORS && !found; ++i) 
	{
        if (color == m_colors[i]) 
		{
            if (m_users[i] == 0) m_changed = true;
            ++m_users[i];
            found = true;
        }
        if (m_users[i] == 0 && freeIndex >= MAX_COLORS) 
		{
            freeIndex = i;
        }
    }
    if (!found && freeIndex < MAX_COLORS) 
	{
        m_colors[freeIndex] = color;
        m_users[freeIndex] = 1;
        m_changed = true;
    }
}

// ==================================
void cDxr3PaletteManager::RemoveColor(int color) 
{
    bool found = false;
    for (int i = 0; i < MAX_COLORS && !found; ++i) 
	{
        if (color == m_colors[i]) 
		{
            if (m_users[i] > 0) --m_users[i];
            found = true;
        }
    }
}

// ==================================
int cDxr3PaletteManager::GetIndex(int color) 
{
    bool found = false;
    int index = 0;
    for (int i = 0; i < MAX_COLORS && !found; ++i) 
	{
        if (color == m_colors[i]) 
		{
            index = i;
            found = true;
        }
    }
    return index; 
}

// ==================================
int cDxr3PaletteManager::GetCount() 
{
    return MAX_COLORS;
}

// ==================================
int cDxr3PaletteManager::operator[](int index) 
{
    assert(index < MAX_COLORS && index > 0);
    return m_colors[index];
}

// ==================================
bool cDxr3PaletteManager::HasChanged() 
{
    bool retval = m_changed;
    m_changed = false;
    return retval;
}

// ==================================
uint32_t* cDxr3PaletteManager::GetPalette() 
{   
    for (int i = 0; i < MAX_COLORS; ++i) 
	{
        m_pal[i] = Tools::Rgb2YCrCb(m_colors[i]);
    }
    
    return m_pal;
} 
