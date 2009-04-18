/*
 * dxr3pesframe.h
 *
 * Copyright (C) 2002-2004 Kai Möller
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

#ifndef _DXR3PESFRAME_H_
#define _DXR3PESFRAME_H_

#include <assert.h>
#include <stdint.h>
#include "Uncopyable.h"

// ==================================
enum eVideoFrameType
{
    I_FRAME,
    P_FRAME,
    B_FRAME,
    UNKNOWN_FRAME
};

// ==================================
// pes - packetized elementary stream
class cDxr3PesFrame : private Uncopyable {
public:

    // ==================================
    enum ePesDataType
    {
	PES_AUDIO_DATA,
	PES_VIDEO_DATA,
	PES_PRIVATE_DATA,
	PES_UNKNOWN_DATA
    };

public:
    cDxr3PesFrame() :
	m_pesDataType(PES_UNKNOWN_DATA),
	m_pPesStart(0),
	m_payload(0),
	m_payloadLength(0),
	m_pts(0),
	m_videoFrameType(UNKNOWN_FRAME),
	m_aspectRatio(m_staticAspectRatio),
	m_horizontalSize(0),
	m_verticalSize(0),
	m_offset(0) {};

    virtual ~cDxr3PesFrame() {}

    bool parse(const uint8_t *pes, uint32_t length);

    ePesDataType GetPesDataType() const
    {
        return m_pesDataType;
    }

    const uint8_t* GetPesStart() const
    {
        return m_pPesStart;
    }

    const uint8_t* GetPayload() const
    {
        return m_payload;
    }

    uint32_t GetPayloadLength() const
    {
        return m_payloadLength;
    }

    uint32_t GetPts() const
    {
        return m_pts;
    }

    eVideoFrameType GetFrameType() const
    {
        assert(m_pesDataType == PES_VIDEO_DATA);
        return m_videoFrameType;
    }

    uint32_t GetAspectRatio() const
    {
        assert(m_pesDataType == PES_VIDEO_DATA);
        return m_aspectRatio;
    }

    uint32_t GetHorizontalSize() const
    {
        assert(m_pesDataType == PES_VIDEO_DATA);
        return m_horizontalSize;
    }

    uint32_t GetVerticalSize() const
    {
        assert(m_pesDataType == PES_VIDEO_DATA);
        return m_verticalSize;
    }

    int GetOffset() const
    {
        return m_offset;
    }

protected:
    void InitData()
    {
        m_pesDataType = PES_UNKNOWN_DATA;
        m_pPesStart = 0;
        m_payload = 0;
        m_payloadLength = 0;
        m_pts = 0;
        m_videoFrameType = UNKNOWN_FRAME;
        m_aspectRatio = m_staticAspectRatio;
        m_horizontalSize = 0;
        m_verticalSize = 0;
        m_offset = 0;
    }

    ePesDataType m_pesDataType;
    const uint8_t* m_pPesStart;
    const uint8_t* m_payload;
    uint32_t m_payloadLength;
    uint32_t m_pts;

    eVideoFrameType m_videoFrameType;
    uint32_t m_aspectRatio;
    uint32_t m_horizontalSize;
    uint32_t m_verticalSize;

    int m_offset;

    static uint32_t m_staticAspectRatio;

protected:
    static const uint32_t MAX_PES_HEADER_SIZE;
};

#endif /*_DXR3PESFRAME_H_*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
