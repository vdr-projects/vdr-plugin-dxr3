/*
 * dxr3spudecoder.h
 *
 * Copyright (C) 2004 Christian Gmeiner
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#ifndef _DXR3SPUDECODER_H_
#define _DXR3SPUDECODER_H_
 
#include "dxr3vdrincludes.h"
#include "dxr3interface.h"

// ==================================
//! spu decoder
/*!
	cDxr3SpuDecoder is used to show DVD
	navigation and subtitles.
	We make here use of the DVD-Functions
	of the dxr3 driver/card.
*/
class cDxr3SpuDecoder : public cSpuDecoder 
{
public: 
	cDxr3SpuDecoder(); 
	~cDxr3SpuDecoder()	{}
	
	int setTime(uint32_t pts); 
	
	cSpuDecoder::eScaleMode getScaleMode(void);
	void setScaleMode(cSpuDecoder::eScaleMode ScaleMode); 
	void setPalette(uint32_t * pal);
	void setHighlight(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t palette); 
	void clearHighlight(); 
	void Empty(); 
	void Hide(); 
	void Draw(); 
	bool IsVisible()	{ return m_visible; } 
#if VDRVERSNUM >= 10318
	void processSPU(uint32_t pts, uint8_t * buf, bool AllowedShow);
#else
	void processSPU(uint32_t pts, uint8_t * buf);
#endif

private:
	cDxr3Interface&	m_Interface;	///< interface to dxr3 driver
	bool			m_visible;		///< is anything visible (nav, osd, subtilte)
}; 
 
#endif /*_DXR3SPUDECODER_H_*/
