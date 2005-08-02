/*
 * dxr3spudecoder.c
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "dxr3spudecoder.h"

// ==================================
// ! constructor
cDxr3SpuDecoder::cDxr3SpuDecoder() : m_Interface(cDxr3Interface::Instance()), m_visible(false) 
{
    m_ScaleMode = eSpuNormal;
}

// ==================================
// ! send spu data to dxr3
#if VDRVERSNUM >= 10318
void cDxr3SpuDecoder::processSPU(uint32_t pts, uint8_t * buf, bool AllowedShow)
#else
void cDxr3SpuDecoder::processSPU(uint32_t pts, uint8_t * buf)
#endif
{
	// size are the first two bits of buf
	// size = (buf[0] << 8) + buf[1]
	
	m_Interface.WriteSpu(buf, (buf[0] << 8) + buf[1]);
	m_Interface.SetSpuPts(pts);
} 

// ==================================
// ! get scalemode
cSpuDecoder::eScaleMode cDxr3SpuDecoder::getScaleMode(void)
{
	return m_ScaleMode;
}

// ==================================
// ! set scalemode - not needed
void cDxr3SpuDecoder::setScaleMode(cSpuDecoder::eScaleMode ScaleMode) 
{
	m_ScaleMode = ScaleMode;
} 

// ==================================
// ! send palette to dxr3
void cDxr3SpuDecoder::setPalette(uint32_t * pal) 
{
	m_Interface.SetPalette(pal);
} 

// ==================================
// ! send highlight to dxr3
void cDxr3SpuDecoder::setHighlight(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t palette) 
{
	m_Interface.SetButton(sx, sy, ex, ey, palette);
} 

// ==================================
// ! clear highlight
void cDxr3SpuDecoder::clearHighlight(void) 
{
	m_Interface.ClearButton();
} 

// ==================================
// ! draw nav, subtitles, ...
void cDxr3SpuDecoder::Draw(void) 
{
	m_visible = true;
} 

// ==================================
// ! hide nav, subtitles, ...
void cDxr3SpuDecoder::Hide(void) 
{
	m_visible = false;
} 

// ==================================
// ! clear highlight and osd
void cDxr3SpuDecoder::Empty(void) 
{
	m_Interface.ClearOsd();
} 

// ==================================
// ! set pts
int cDxr3SpuDecoder::setTime(uint32_t pts) 
{ 
    m_Interface.SetSpuPts(pts);
    return (pts == 0) ? 0 : 1; 
}

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
