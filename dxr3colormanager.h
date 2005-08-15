/***************************************************************************
                          dxr3colormanager.h  -  description
                             -------------------
    begin                : Tue Oct 22 2002
    copyright            : (C) 2002 by Stefan Schluenss
    email                : vdr@schluenss.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _DXR3COLORMANAGER_H_
#define _DXR3COLORMANAGER_H_
/*
// ==================================
struct rectangular_area
{
    rectangular_area() :
	m_startrow(0), m_endrow(0), m_startcol(0), m_endcol(0)	{}

private:
    size_t m_startrow;
    size_t m_endrow;
    size_t m_startcol;
    size_t m_endcol;

    unsigned int Colors[4];
    unsigned int Opac[4];
}
*/

/**SPU-ColorManager
 *@author Stefan Schluenss
 */

#include <stdio.h>

#define OSD_SPU_CM_DUMP 0

#define MAX_NO_OF_SECTIONS 15
#define MAX_NO_OF_REGIONS 100


// ==================================
class xSection
{
public:
    xSection(int x);
    bool HasColor(unsigned int color, unsigned char &ColorIndex);
    unsigned char AddColor(unsigned int color);
    bool AllColorsUsed(bool FirstLine) {
	//DIAG("AllColorsUsed: %d\n",NrOfColors);
	return(NrOfColors >= (FirstLine ? 3 : 4));
    };
    int X1;
    int X2;
    int NrOfColors;
    unsigned int Colors[4];
    unsigned int Opac[4];
};

// ==================================
class yRegion
{
public:
    yRegion() : Y1(0), Y2(0), N(0) {}

    /** No descriptions */
    void AddSection(int first, int last, unsigned int color,
		    unsigned int opac);
    int Y1;
    int Y2;
    int N;

    xSection* Section[MAX_NO_OF_SECTIONS];
};

// ==================================
class cColorManager
{
public:
    cColorManager();
    ~cColorManager();

    void EncodeColors(int width, int height, unsigned char* smap,
		      unsigned char* dmap);

    /** Sets a new color on the OSD */
    unsigned char AddColor(int x, int y, unsigned char color,
			   unsigned char &ColorIndex);

    /** Encodes the color information as highlight spu data */
    unsigned char* GetSpuData(int &len);

private:
    yRegion *hlr[MAX_NO_OF_REGIONS];
    yRegion *curRegion;
    int NrOfRegions;
    unsigned char spudata[(4+6*MAX_NO_OF_SECTIONS)*MAX_NO_OF_REGIONS+7];
    xSection *curSection;
    int curSectionIndex;

    /** Opens a new highlight region */
    void OpenRegion(int y);
    /** Closes the spu-highlight region */
    void CloseRegion(int y);

    void NewSection(int x);
    void FirstSection(void);
    void NextSection(void);
};

#endif /*_DXR3COLORMANAGER_H_*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
