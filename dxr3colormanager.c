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
#include "dxr3log.h"
#include "dxr3memcpy.h"
#include <stdio.h>
#include <string.h>

// ==================================
//! constructor
cColorManager::cColorManager()
{
	NrOfRegions = -1;
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
void cColorManager::OpenRegion(int y)
{
	// Calling method (cColorManager::EncodeColors)
	// already checks MAX_NO_OF_REGIONS
	curRegion = new yRegion();
	curRegion->Y1 = y;
	NrOfRegions++;
	hlr[NrOfRegions] = curRegion;
	NewSection(0); //there's always at least a section
}

// ==================================
// Closes the spu-highlight region
void cColorManager::CloseRegion(int y)
{
	curRegion->Y2 = y;
}

// ==================================    
void cColorManager::EncodeColors(int width, int height, unsigned char* map, unsigned char* dmap)
{
    unsigned char color;
    unsigned char oldcolor=0xFF;
    unsigned char ColorIndex;
    int mapoffset=0;

    OpenRegion(0);
    for (int y = 0; y < height; ++y) {
        oldcolor=0xFF;
        FirstSection();
        for(int x = 0; x < width; ++x) {
            if(x>curSection->X2) {
               oldcolor=0xFF;
               NextSection();
            }
            color = map[mapoffset + x];
            if (color==oldcolor) dmap[mapoffset + x]=ColorIndex; else {
              //try to map the color in the current region
              if (AddColor(x,y,color, ColorIndex)) { 
   	         // store as the highlight region index
	         dmap[mapoffset + x]=ColorIndex;
	      } else {   
                CloseRegion(y-1); 
	        if(NrOfRegions<=MAX_NO_OF_REGIONS-1) {
	          //retry with another region
	          OpenRegion(y);    
                  x = -1;
                  oldcolor=0xFF;
                } else
                {
                  //give up
                  cLog::Instance() << "dxr3colormanager: too many regions!\n";
                  return;
                }  
              }
            }   
        }
        mapoffset+=width;
    }
    //close the last highligt region
    CloseRegion(height);

//#define colordebug  
#ifdef colordebug    
     {
    FILE *fp;
    fp = fopen("OSD.dump","w+");
    u_char *pippo=dmap;
    u_char *pippo2=map;
    int curregion=0;
    int cursection=0;
    
    
    for (int dumpy=0; dumpy<height; dumpy++) 
      {
      if(curregion<NrOfRegions) {
        if(hlr[curregion]->Y1==dumpy) {
          fprintf(fp,"%i",hlr[curregion]->N);
          for(int sec=0; sec<hlr[curregion]->N; sec++) fprintf(fp,",%i",hlr[curregion]->Section[sec]->X1);
          for(int dumpx=0; dumpx<width; dumpx++) fprintf(fp,"=");
          fprintf(fp,"\n");
          curregion++;
        }
      }  
      
      cursection=0;
      for(int dumpx=0; dumpx<width; dumpx++) {
       if(curregion<NrOfRegions) {
         if(cursection<hlr[curregion]->N) {
           if(hlr[curregion]->Section[cursection]->X1==dumpx) { 
             fprintf(fp,"|"); 
             cursection++;
             }
           }
         }    
       fprintf(fp,"%01X",*pippo2 & 0xF);
       pippo2++;
       }
       fprintf(fp,"\n");
      
      
      cursection=0;
      for(int dumpx=0; dumpx<width; dumpx++) {
       if(curregion<NrOfRegions) {
         if(cursection<hlr[curregion]->N) {
           if(hlr[curregion]->Section[cursection]->X1==dumpx) { 
             fprintf(fp,"|"); 
             cursection++;
             }
           }
         }    
       fprintf(fp,"%01X",*pippo & 0xF);
       pippo++;
       }
       fprintf(fp,"\n");
      }
      fclose(fp);
      printf("**** dumped\n");
    }
#endif
   
}

// ==================================
unsigned char cColorManager::AddColor(int x, int y, unsigned char color, unsigned char &ColorIndex) {

    if (!curSection->HasColor(color, ColorIndex)) 
    { 
      // this color is new for this section
      if (curSection->AllColorsUsed()) 
      { 
        // no more free colors
        if (y != curRegion->Y1) 
        {
          // terminate region
          return(0);
        }
        NewSection(x);
      }		
      // and add new color
      ColorIndex = curSection->AddColor(color);
    }
    return(1);
}

// ==================================
void cColorManager::FirstSection(void)
{
    curSectionIndex=0;
    curSection=curRegion->Section[0];
}

// ==================================
void cColorManager::NextSection(void)
{
    curSectionIndex++;
    if(curSectionIndex<curRegion->N) curSection=curRegion->Section[curSectionIndex];
    //it shouldn't happen
    else cLog::Instance() <<
      "dxr3colormanager: no more sections in NextSection!\n";
}

// ==================================
// convert into SPU - hope is correct description
unsigned char* cColorManager::GetSpuData(int& len)
{
	if (NrOfRegions >= 0)
    {
    int ptr = 0;
	spudata[ptr++] = 0x07;	// CHG_COLCON command
	spudata[ptr++] = 0x00;  // total size of parameter area
	spudata[ptr++] = 0x00;  // will be filled later


	for(int i = 0; i <= NrOfRegions;i++)
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
void cColorManager::NewSection(int x)
{
    int N = curRegion->N;
    if (N>=MAX_NO_OF_SECTIONS-1) {
      cLog::Instance() << "dxr3colormanager: bummer, too many sections\n";
      return; //reuse last section, not optimal but there's no other way out
    }  
    curSection  = new xSection(x);
    curRegion->Section[N] = curSection;
    if (N > 0) curRegion->Section[N-1]->X2 = x-1;
    (curRegion->N)++;
    curSectionIndex=N;
}

// ==================================
xSection::xSection(int x)
{
    X1 = x;
    X2 = 32767;
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

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
