/***************************************************************************
                          dxr3colormanager.c -  description
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
/**
 * Background:
 * Each encoded pixel in the SPU could have one of the values 0,1,2,3.    *
 * These values

 	Pixelvalue	Maps
				to index
 		0		-->		4	-->


 * The SPU data definition allows highlighting of rectangular areas. They *
 * are defined by a starting and ending row. Whithin these rows one could *
 * define a starting column for using a new color palette mapping. This   *
 * mapping will be used til the end of the line or up to the next column  *
 * defintion.
 * Look at the picture below:
 *
	Row		   Col=5     Col=16
	n-2		...............................
	n-1		...............................
	n		.....-------------------------- <- Highligh region starts here
			.....| 6,3,1,2 | 0,2,8,9
			.....| 6,3,1,2 | 0,2,8,9
			.....| 6,3,1,2 | 0,2,8,9
	n+4		.....-------------------------- <- Highligh region ends here
	n+5		...............................
	n+6		...............................

	In the above example one region (from n to n+4) is defined with two    *
	highlight sections - one from column 5 up to 15 and the second from 16 *
	til the end of the line.
**/


#include <assert.h>

#include "dxr3colormanager.h"
#include "dxr3memcpy.h"
#include <stdio.h>
#include <string.h>

// ==================================
//! constructor
cColorManager::cColorManager()
{
	NrOfRegions = 0;
	isopen = false;
	for(int i = 0; i < MAX_NO_OF_REGIONS; i++)
		hlr[i] = NULL;
}

// ==================================
cColorManager::~cColorManager()
{
	for (int i = 0; i < NrOfRegions; i++)
	{
		if (hlr[i])
		{
			delete(hlr[i]);
		}
	}
}

// ==================================
// Opens a new highlight region
void cColorManager::OpenRegion(int y, int NrOfSecToCopy)
{
	hlr[NrOfRegions] = new yRegion();
	hlr[NrOfRegions]->Y1 = y;
	isopen = true;

    if (NrOfSecToCopy > 0)
    {
    	for (int i = 0; i < NrOfSecToCopy; i++)
        {
        	hlr[NrOfRegions]->Section[i] = hlr[NrOfRegions - 1]->Section[i];
        }
    }
}

// ==================================
// Closes the spu-highlight region
void cColorManager::CloseRegion(int y)
{

	hlr[NrOfRegions]->Y2 = y;
	isopen = false;

	if (hlr[NrOfRegions]->N != 0)		// skip this region if there is no section defined
	{
		if (NrOfRegions < MAX_NO_OF_SECTIONS -1)
        {
			NrOfRegions++;
   		}
	}

}

// ==================================    
void cColorManager::EncodeColors(int width, int height, unsigned char* map, unsigned char* dmap)
{
    unsigned char color = 0xFF, ccol = 0xFF;
    unsigned char ColorIndex = 0xFF;
    unsigned char buffer[1024] = {0};

    for (int y = 0; y < height; ++y) 
	{
        color = 0xFF;
        for(int x = 0; x < width; ++x) 
		{
            ccol = map[y * width + x];
            if (ccol != 0) MaxY = y;
            if (ccol != color)  
			{
                color = ccol; // save this color
                if (!AddColor(x,y,color, ColorIndex)) 
				{ 
					// add this color to highlight regions
                    color = 0xFF;
                    x = -1;
                } 
				else
				{ 
					// color successfully added
                    buffer[x] = ColorIndex;
                }
            } 
			else
			{
                buffer[x] = ColorIndex;//*(dmap+(y * width + x)) = ColorIndex;
            }
        }
        dxr3_memcpy(dmap+y*width, buffer,width);
    }
}

// ==================================
unsigned char cColorManager::AddColor(int x, int y, unsigned char color, unsigned char &ColorIndex) {
    static int yold = -1;
    xSection* Section = 0;
    int SectionIndex = 0;

    if (isopen) 
	{
		// there is an opened highlight-region
        Section = GetSection(x, SectionIndex);	
		
		// checks whether we have a section defined on the formerly line on this x-position
        if (Section != NULL) 
		{ 
			// there was a section
            if (!Section->HasColor(color, ColorIndex)) 
			{ 
				// this color is new for this section
                if (Section->AllColorsUsed()) 
				{ 
					// no more free colors
                    if (yold != y) 
					{
                        CloseRegion(y-1); 
						// terminate region
                        return(0);
                        yold = y;
						// open new region
                        OpenRegion(y,SectionIndex+1);
                    }
					// create new section
                    Section = NewSection(x);
                }
				// and add new color
                ColorIndex = Section->AddColor(color);
            }
        }
		else
		{ 
			// no section found (but region already open)

			// terminate region
            CloseRegion(y-1);
            yold = y;
			// open new region
            OpenRegion(y);
        	// create new section
			Section = NewSection(x);           
			// and add new color
			ColorIndex = Section->AddColor(color);
        }
    } 
	else
	{ 
		// currently no region open
        yold = y;

		// open new region
        OpenRegion(y);
		// create new section
        Section = NewSection(x);
		// and add new color
        ColorIndex = Section->AddColor(color);
    }
    return(1);
}

// ==================================
xSection *cColorManager::GetSection(int x, int &n)
{
	int i;
    n = 0;

	// for every section in the current region
    for (i = 0; i < hlr[NrOfRegions]->N; i++)	
    {
    	if ((x <= hlr[NrOfRegions]->Section[i]->X2) &&  (x >= hlr[NrOfRegions]->Section[i]->X1)) // x-pos is in section
        {
        	n = i;
        	return (hlr[NrOfRegions]->Section[i]);
        }
    }
    return(NULL);
}

/** Adds a new highlight region beginning from FIRST to LAST column */
/**
void cColorManager::AddRegion(int first, int last, unsigned int color, unsigned int opac)
{
	DIAG("AddRegion(%d %d %x %x)\n",first, last, color, opac);
	hlr[NrOfRegions]->AddSection(first, last, color, opac);
}
**/

// ==================================
// convert into SPU - hope is correct description
unsigned char* cColorManager::GetSpuData(int& len)
{
    if (isopen)		// there is an opened highlight-region
		CloseRegion(MaxY);

	if (NrOfRegions != 0)
    {
    int ptr = 0;
	spudata[ptr++] = 0x07;	// CHG_COLCON command
	spudata[ptr++] = 0x00;  // total size of parameter area
	spudata[ptr++] = 0x00;  // will be filled later


	for(int i = 0; i < NrOfRegions;i++)
	{
		spudata[ptr++] = (hlr[i]->Y1 >> 8) & 0x0f;
		spudata[ptr++] = (hlr[i]->Y1 & 0xff);
		spudata[ptr++] = ( ( (hlr[i]->N) & 0x0f) << 4)	| ((hlr[i]->Y2 >> 8) & 0x0f);
		spudata[ptr++] = (hlr[i]->Y2 & 0xff);

		for(int c = 0; c < hlr[i]->N; c++)
		{
			spudata[ptr++] = hlr[i]->Section[c]->X1 >> 8;
			spudata[ptr++] = hlr[i]->Section[c]->X1 & 0xff;
			spudata[ptr++] = (hlr[i]->Section[c]->Colors[3] << 4) | (hlr[i]->Section[c]->Colors[2] & 0x0F);
			spudata[ptr++] = (hlr[i]->Section[c]->Colors[1] << 4) | (hlr[i]->Section[c]->Colors[0] & 0x0F);

			spudata[ptr++] = (hlr[i]->Section[c]->Opac[3] << 4) | hlr[i]->Section[c]->Opac[2];
			spudata[ptr++] = (hlr[i]->Section[c]->Opac[1] << 4) | hlr[i]->Section[c]->Opac[0];
		}
	}
	spudata[ptr++] = 0x0f;	// termination of parameter block
	spudata[ptr++] = 0xff;
	spudata[ptr++] = 0xff;
	spudata[ptr++] = 0xff;
	int size = ptr - 1;
	spudata[1] = size >> 8;
	spudata[2] = size & 0xff;

	len = ptr;
    }
    else
    	len = 0;

#if OSD_SPU_CM_DUMP
	FILE *fp;
    fp = fopen("CM.dump","a+");
    fprintf(fp,"len:%03d ",len);
    for (int i = 0; i < len; i++)
    	fprintf(fp,"%02X",*(spudata+i));
    fprintf(fp,"\n");
    fclose(fp);
#endif

	return(spudata);
}

// ==================================
xSection *cColorManager::NewSection(int x)
{
	xSection* sec = new xSection(x);
    int N = hlr[NrOfRegions]->N;

	hlr[NrOfRegions]->Section[hlr[NrOfRegions]->N] = sec;
	if (N > 0)
    	hlr[NrOfRegions]->Section[hlr[NrOfRegions]->N - 1]->X2 = x-1;
	(hlr[NrOfRegions]->N)++;

    return(sec);
}

/**
// No descriptions */
/**
void HLRegion::AddSection(int first, int last, unsigned int color, unsigned int opac)
{
	DIAG("  AddSection %d,%d %X,%X\n",first, last, color, opac);
	Region[N] = new ColRegion(first, last, color, opac);
	assert(N<MAX_NO_OF_REGIONS -1);
	N++;
	DIAG("HL N:%d\n",N);
}
**/

// ==================================
xSection::xSection(int x)
{
    X1 = x;
    X2 = 0xFFF;
    NrOfColors = 0;
    for (int i = 0; i < 4; i++)
    {
		Opac[i] = 0xFF;
		Colors[i] = 0;
    }
}

// ==================================
unsigned char xSection::AddColor(unsigned int color)
{
	unsigned char ColorIndex = 0;

	if (NrOfColors <= 3)
    {
		Colors[NrOfColors] = color;
		Opac[NrOfColors] = color >> 4;
        ColorIndex = NrOfColors;
        NrOfColors++;
    }
    return(ColorIndex);
}

// ==================================
bool xSection::HasColor(unsigned int color, unsigned char &ColorIndex)
{
	for(int i = 0; i < NrOfColors; i++)
    {
    	if (Colors[i] == color)
        {
        	ColorIndex = i;
        	return (true);
        }
    }
    return(false);
}

/** No descriptions */
void cColorManager::SetBgColor(unsigned int bgColor)
{
//	cColorManager::BgColor = bgColor;
}

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
