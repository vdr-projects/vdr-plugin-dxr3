/*
 * dxr3syncbuffer.h
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

#ifndef _DXR3SYNCBUFFER_H_
#define _DXR3SYNCBUFFER_H_


#include <vdr/ringbuffer.h>
#include "dxr3interface.h"
#include "dxr3generaldefines.h"
#include "uncopyable.h"
#include "accessors.h"

// ==================================
const uint32_t UNKNOWN_CHANNEL_COUNT = 0xFFFFFFFF;
const uint32_t UNKNOWN_DATA_RATE = 0xFFFFFFFF;
const uint32_t UNKNOWN_ASPECT_RATIO = 0xFFFFFFFF;

// ==================================
class cFixedLengthFrame : private Uncopyable {
public:
    cFixedLengthFrame() : samplerate(UNKNOWN_DATA_RATE), channels(UNKNOWN_CHANNEL_COUNT), aspectratio(UNKNOWN_ASPECT_RATIO),
                          m_count(0), m_length(0), m_pts(0), m_type(ftUnknown)
    {}

    ~cFixedLengthFrame();

    void Init(uint32_t lenght);

    void CopyFrame(const uint8_t* pStart, int length, uint32_t pts,
		   eFrameType type);
    uint8_t* GetData(void);
    int GetCount(void);
    uint32_t GetPts(void);
    void SetPts(uint32_t pts);

    eFrameType GetFrameType()   { return m_type; }

    Accessors<uint32_t> samplerate;
    Accessors<uint32_t> channels;
    Accessors<uint32_t> aspectratio;


private:
    uint8_t* m_pData;
    int m_count;
    int m_length;
    uint32_t m_pts;
    eFrameType m_type;
};

// ==================================
class cDxr3SyncBuffer : public cRingBuffer, private Uncopyable {
public:
    enum eSyncBufferException
    {
	SYNC_BUFFER_OVERRUN
    };
public:
    cDxr3SyncBuffer(int frameCount, int frameLength);
    ~cDxr3SyncBuffer();

    virtual int Available(void);
    cFixedLengthFrame* Push(const uint8_t* pStart, int length, uint32_t pts,
			    eFrameType type = ftUnknown)
	throw (eSyncBufferException);
    void Pop(void);
    cFixedLengthFrame* Get(void);
    void Clear(void);
    void Stop(void)
    {
	m_bStopped = true;
    };
    void Start(void);
    void WakeUp(void);
    void WaitForReceiverStopped(void);
    void SetDemuxMode(eDxr3DemuxMode demuxMode)
    {
	m_demuxMode = demuxMode;
    };
    bool Poll(int TimeoutMs);
    bool IsPolled(void)
    {
	return m_bPollSync;
    };
    uint32_t GetFillLevel(void)
    {
	return Available() * 100 / Size();
    };

private:
    void ReceiverStopped(void);

    cFixedLengthFrame* m_pBuffer;
    int m_count;
    int m_nextFree;
    int m_next;
    bool m_bPutBlock;
    bool m_bGetBlock;
    bool m_bStartReceiver;
    bool m_bStopped;
    eDxr3DemuxMode m_demuxMode;

    cCondVar receiverStopped;
    cMutex receiverStoppedMutex;
    cDxr3Interface *m_dxr3Device;
    bool m_bPollSync;

    cDxr3SyncBuffer(); // you are not allowed to use this constructor
};

#endif /*_DXR3SYNCBUFFER_H_*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// tab-width: 4;
// indent-tabs-mode: nil
// End:
