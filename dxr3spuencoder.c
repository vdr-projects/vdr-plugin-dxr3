/*
 *     dxr3spuencoder.c - encodes an OSD bitmap as subpicture
 *
 *	Assimilated and adapted by
 *  Stefan Schluenss <dxr3_osd@schluenss.de>
 *  Nov. 2002
 *
 * Based on the subpicture encoding routines from MPlayer and
 * the information given by
 *   Samuel Hocevar
 *   Michel Lespinasse
 *   and http://members.aol.com/mpucoder/DVD/spu.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "dxr3spuencoder.h"

// ==================================
// dec.
cSpuData::~cSpuData()
{
	Clear();
}

// ==================================
// free buffer and set it to 0
void cSpuData::Clear()
{
	if (data)
	{
		free(data);
		count = malloc_size = 0;
	}
}

// ==================================
// wirte a byte to spu buffer
void cSpuData::WriteByte(uint8_t byte)
{
	if (count >= malloc_size)
	{
		data = (u_char*)realloc(data, malloc_size += 2048);
	}
	
	data[count++] = byte;
}

// ==================================
void cSpuData::WriteNibble(int *higher_nibble, uint8_t nibble)
{
}

// ==================================
void cSpuData::WriteRle(int *higher_nibble, int length, int color)
{
}


// ==================================
cSPUEncoder::cSPUEncoder()
{
	m_lastwindow = 0;

	// clear osd
	memset(m_OSD, 0x00 ,OSDWIDTH * OSDHEIGHT);
}

// ==================================
// main function for the osd
// makes life nicer :)
int cSPUEncoder::Cmd(OSD_Command cmd, int color, int x0, int y0, int x1, int y1, const void *data)
{
	switch (cmd)
	{
	case OSD_SetWindow:
		//  (x0) set window with number 0<x0<8 as current
		if (x0 < 8 && x0 > 0)
		{
			m_lastwindow = x0;
			return 0;
		}

		return -1;
		break;

	case OSD_Open:
		// (x0,y0,x1,y1,BitPerPixel[2/4/8](color&0x0F),mix[0..15](color&0xF0))
		// Opens OSD with this size and bit depth
		// returns 0 on success, -1 on DRAM allocation error, -2 on "already open"
		m_windows[m_lastwindow].x0 = x0;
		m_windows[m_lastwindow].y0 = y0;
		m_windows[m_lastwindow].x1 = x1;
		m_windows[m_lastwindow].y1 = y1;

		// calculate new active osd area
		CalculateActiveOsdArea();

		return 0;
		break;

	case OSD_SetPalette:
		// Spu->Cmd(OSD_SetPalette, 0, NumColors - 1, 0, 0, 0, Colors);
		// (firstcolor{color},lastcolor{x0},data)
		// Set a number of entries in the palette
		// sets the entries "firstcolor" through "lastcolor" from the array "data"
		// data has 4 byte for each color:
		// R,G,B, and a opacity value: 0->transparent, 1..254->mix, 255->pixel

		return 0;
		break;

	case OSD_SetBlock:
		// (x0,y0,x1,y1,increment{color},data)
		// fills pixels x0,y0 through  x1,y1 with the content of data[]
		// inc contains the width of one line in the data block,
		// inc<=0 uses blockwidth as linewidth
		// returns 0 on success, -1 on clipping all pixel

		return 0;
		break;

	case OSD_Close:
		// clear colors from plattemanager

		// set windows position to 0
		m_windows[m_lastwindow].x0 = 0;
		m_windows[m_lastwindow].y0 = 0;
		m_windows[m_lastwindow].x1 = 0;
		m_windows[m_lastwindow].y1 = 0;

		// calculate new active osd area
		CalculateActiveOsdArea();

		return 0;
		break;

	case OSD_Clear:
		// Sets all pixel to color 0
		// returns 0 on success

		// This should be done in cSPUEncoder::cSPUEncoder

		return 0;
		break;

	// not needed - at the moment
	case OSD_Show:
	case OSD_Hide:
	case OSD_Fill:
	case OSD_SetColor:
	case OSD_SetTrans:
	case OSD_SetPixel:
	case OSD_GetPixel:
	case OSD_SetRow:
	case OSD_FillRow:
	case OSD_FillBlock:
	case OSD_Line:
	case OSD_Query:
	case OSD_Test:
	case OSD_Text:
	case OSD_MoveWindow:
		break;
	};

	return -1;
}

// ==================================
// stamps window content into full osd bitmap
void cSPUEncoder::CopyBlockIntoOSD(int linewidth, int x0, int y0, int x1, int y1, u_char *data)
{
	int i;
	int w;
	u_char *cp;
	u_char *sp = data;


	// linewidth contains the width of one line in the data block,
	// linewidth<=0 uses blockwidth as linewidth
	if (linewidth <= 0)
	{
		w = m_windows[m_lastwindow].x1 - m_windows[m_lastwindow].x0;
	}
	else
	{
		w = linewidth;
	}
	
    for (i = y0; i <= y1; ++i) 
	{
        cp = &m_OSD[i*OSDWIDTH + x0];
        if ((cp+x1-x0+1) < &m_OSD[OSDWIDTH * OSDHEIGHT-1]) 
		{
            for (int xx=0; xx <= (x1-x0); xx++) 
			{
                *(cp+xx) = m_windows[m_lastwindow].colors[*(sp+xx) & 0x0f];
            }
        } 
		else 
		{
            continue;
        }
        sp += w;
	}
}

// ==================================
// we _only_ write usefull data
void cSPUEncoder::CalculateActiveOsdArea()
{
	// reset
	m_active_osd.Reset();

	// calculate
	for (int i = 0; i < 8; i++)
	{
		m_active_osd.x0 = std::max(m_active_osd.x0, m_windows[i].x0);
		m_active_osd.x1 = std::max(m_active_osd.x1, m_windows[i].y0);
		m_active_osd.y0 = std::max(m_active_osd.y0, m_windows[i].x1);
		m_active_osd.y1 = std::max(m_active_osd.y1, m_windows[i].y1);
	}

	cLog::Instance() << "OSD x0: " << m_active_osd.x0 << "\n";
	cLog::Instance() << "OSD y0: " << m_active_osd.y0 << "\n";
	cLog::Instance() << "OSD x1: " << m_active_osd.x1 << "\n";
	cLog::Instance() << "OSD y1: " << m_active_osd.y1 << "\n";
}
