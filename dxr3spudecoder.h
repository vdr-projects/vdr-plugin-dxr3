/*
 * dxr3spudecoder.h
 *
 * Copyright (C) 2005 Christian Gmeiner
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

#ifndef _DXR3SPUDECODER_H_
#define _DXR3SPUDECODER_H_

#include "dxr3vdrincludes.h"
#include "dxr3interface.h"
#include <inttypes.h>

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
    virtual ~cDxr3SpuDecoder()  {}

    // lower level stuffer
    virtual int setTime(uint32_t pts);
    virtual eScaleMode getScaleMode()                { return m_ScaleMode; }
    virtual void setScaleMode(eScaleMode ScaleMode)  { m_ScaleMode = ScaleMode; }
    virtual void setPalette(uint32_t * pal);
    virtual void setHighlight(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t palette);
    virtual void clearHighlight();

    // higher level stuff
    virtual void Empty();
    virtual void Hide()         { m_Visible = false; }
    virtual void Draw()         { m_Visible = true; }
    virtual bool IsVisible()    { return m_Visible; }
#if VDRVERSNUM >= 10318
    virtual void processSPU(uint32_t pts, uint8_t * buf, bool AllowedShow);
#else
    virtual void processSPU(uint32_t pts, uint8_t * buf);
#endif

private:
    bool m_Visible;                 ///< is anything visible (nav, osd, subtitles)
    eScaleMode m_ScaleMode;
    cDxr3Interface& m_Interface;    ///< interface to dxr3 driver
};

#endif /*_DXR3SPUDECODER_H_*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
