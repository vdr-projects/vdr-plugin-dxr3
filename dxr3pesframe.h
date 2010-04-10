/*
 * dxr3pesframe.h
 *
 * Copyright (C) 2002-2004 Kai Möller
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#ifndef _DXR3PESFRAME_H_
#define _DXR3PESFRAME_H_

#include <stdint.h>
#include "uncopyable.h"
#include "accessors.h"

// ==================================
// pes - packetized elementary stream
class cDxr3PesFrame : private Uncopyable {
public:

    // ==================================
    enum ePesDataType {
    	PES_AUDIO_DATA,
    	PES_VIDEO_DATA,
    	PES_PRIVATE_DATA,
    	PES_UNKNOWN_DATA
    };

public:
    cDxr3PesFrame() : pesDataType(PES_UNKNOWN_DATA), pesStart(0), payload(0), payloadSize(0),
					  pts(0), aspectRatio(m_staticAspectRatio), horizontalSize(0), verticalSize(0) {}

    bool parse(const uint8_t *pes, uint32_t length);

    Accessors<ePesDataType, ro> pesDataType;

    Accessors<const uint8_t *, ro> pesStart;
    Accessors<const uint8_t *, ro> payload;

    Accessors<uint32_t, ro> payloadSize;
    Accessors<uint32_t, ro> pts;

    Accessors<uint32_t, ro> aspectRatio;
    Accessors<uint32_t, ro> horizontalSize;
    Accessors<uint32_t, ro> verticalSize;

private:
    static uint32_t m_staticAspectRatio;
};

#endif /*_DXR3PESFRAME_H_*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// tab-width: 4;
// indent-tabs-mode: nil
// End:
