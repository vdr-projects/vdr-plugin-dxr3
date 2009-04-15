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
// XXX: Should we use here std:vector?
template <class T>
class cDxr3SafeArray
{
public:
    // ==================================
    enum eSafeArrayException
    {
	SAFE_ARRAY_INDEX_OUT_OF_BOUND
    };

    cDxr3SafeArray(T* pBuf, uint32_t length) :
	m_pBuf(pBuf), m_length(length) {};
    cDxr3SafeArray(const cDxr3SafeArray& from) :
	m_pBuf(from.m_pBuf), m_length(from.m_length) {};
    virtual ~cDxr3SafeArray() {};

    T& operator[](uint32_t index) throw (eSafeArrayException)
    {
	if (index >= m_length)
	    throw(SAFE_ARRAY_INDEX_OUT_OF_BOUND);
	return m_pBuf[index];
    };
    cDxr3SafeArray SubArray(uint32_t offset, uint32_t length)
    {
	if (offset + length > m_length)
	    throw(SAFE_ARRAY_INDEX_OUT_OF_BOUND);
	return cDxr3SafeArray(m_pBuf + offset, length);
    };

    uint32_t GetLength(void)
    {
	return m_length;
    };

protected:
    T* m_pBuf;
    uint32_t m_length;

private:
    cDxr3SafeArray(); // no standard constructor
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

    // ==================================
    enum ePesFrameError
    {
	PES_GENERAL_ERROR
    };

public:
    cDxr3PesFrame() :
	m_pesDataType(PES_UNKNOWN_DATA),
	m_bValid(false),
	m_pPesStart(0),
	m_payload(0),
	m_payloadLength(0),
	m_pts(0),
	m_videoFrameType(UNKNOWN_FRAME),
	m_aspectRatio(m_staticAspectRatio),
	m_horizontalSize(m_staticHorizontalSize),
	m_verticalSize(m_staticVerticalSize),
	m_pNextStart(0),
	m_remainingLength(0),
	m_offset(0) {};

    virtual ~cDxr3PesFrame() {}

    bool ExtractNextFrame(const uint8_t* pBuf, uint32_t length)
    throw (ePesFrameError);

    ePesDataType GetPesDataType() const
    {
        assert(m_bValid);
        return m_pesDataType;
    }

    const uint8_t* GetPesStart() const
    {
        assert(m_bValid);
        return m_pPesStart;
    }

    const uint8_t* GetPayload() const
    {
        assert(m_bValid);
        return m_payload;
    }

    uint32_t GetPayloadLength() const
    {
        assert(m_bValid);
        return m_payloadLength;
    }

    const uint8_t* GetNextStart() const
    {
        return m_pNextStart;
    }

    uint32_t GetRemainingLength() const
    {
        return m_remainingLength;
    }

    uint32_t GetPts() const
    {
        assert(m_bValid);
        return m_pts;
    }

    eVideoFrameType GetFrameType() const
    {
        assert(m_bValid);
        assert(m_pesDataType == PES_VIDEO_DATA);
        return m_videoFrameType;
    }

    uint32_t GetAspectRatio() const
    {
        assert(m_bValid);
        assert(m_pesDataType == PES_VIDEO_DATA);
        return m_aspectRatio;
    }

    uint32_t GetHorizontalSize() const
    {
        assert(m_bValid);
        assert(m_pesDataType == PES_VIDEO_DATA);
        return m_horizontalSize;
    }

    uint32_t GetVerticalSize() const
    {
        assert(m_bValid);
        assert(m_pesDataType == PES_VIDEO_DATA);
        return m_verticalSize;
    }

    int GetOffset() const
    {
        assert(m_bValid);
        return m_offset;
    }

    bool IsValid()
    {
        return m_bValid;
    }

protected:
    bool IsPesHeader(cDxr3SafeArray<uint8_t> header)
	throw (cDxr3SafeArray<uint8_t>::eSafeArrayException);
    void ExtractPts(cDxr3SafeArray<uint8_t> ptsData)
	throw (cDxr3SafeArray<uint8_t>::eSafeArrayException);
    int ExtractVideoData(cDxr3SafeArray<uint8_t> esFrame)
	throw (cDxr3SafeArray<uint8_t>::eSafeArrayException);

    void InitData()
    {
        m_pesDataType = PES_UNKNOWN_DATA;
        m_bValid = false;
        m_pPesStart = 0;
        m_payload = 0;
        m_payloadLength = 0;
        m_pts = 0;
        m_videoFrameType = UNKNOWN_FRAME;
        m_aspectRatio = m_staticAspectRatio;
        m_horizontalSize = m_staticHorizontalSize;
        m_verticalSize = m_staticVerticalSize;
        m_pNextStart = 0;
        m_remainingLength = 0;
        m_offset = 0;
    }

    ePesDataType m_pesDataType;
    bool m_bValid;
    const uint8_t* m_pPesStart;
    const uint8_t* m_payload;
    uint32_t m_payloadLength;
    uint32_t m_pts;

    eVideoFrameType m_videoFrameType;
    uint32_t m_aspectRatio;
    uint32_t m_horizontalSize;
    uint32_t m_verticalSize;

    const uint8_t* m_pNextStart;
    uint32_t m_remainingLength;
    int m_offset;

    static uint32_t m_staticAspectRatio;
    static uint32_t m_staticHorizontalSize;
    static uint32_t m_staticVerticalSize;

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
