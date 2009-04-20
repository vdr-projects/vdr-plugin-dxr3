/*
 * dxr3syncbuffer.h
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

#ifndef _DXR3SYNCBUFFER_H_
#define _DXR3SYNCBUFFER_H_

#include <stdint.h>

#include <vdr/ringbuffer.h>
#include "dxr3interface.h"
#include "dxr3generaldefines.h"
#include "Uncopyable.h"

// ==================================
const uint32_t UNKNOWN_CHANNEL_COUNT = 0xFFFFFFFF;
const uint32_t UNKNOWN_DATA_RATE = 0xFFFFFFFF;
const uint32_t UNKNOWN_ASPECT_RATIO = 0xFFFFFFFF;

// ==================================
class cFixedLengthFrame : private Uncopyable {
public:
    cFixedLengthFrame();
    ~cFixedLengthFrame();

    void Init(uint32_t lenght);

    void CopyFrame(const uint8_t* pStart, int length, uint32_t pts,
		   eFrameType type);
    uint8_t* GetData(void);
    int GetCount(void);
    uint32_t GetPts(void);
    void SetPts(uint32_t pts);
    void SetChannelCount(uint32_t channelCount)
    {
	    m_audioChannelCount = channelCount;
    }
    void SetSampleRate(uint32_t sampleRate)
    {
	    m_audioSampleRate = sampleRate;
    }
    void SetAspectRatio(uint32_t aspectRatio)
    {
	m_videoAspectRatio = aspectRatio;
    };
    uint32_t GetChannelCount()  { return m_audioChannelCount; }
    uint32_t GetSampleRate()    { return m_audioSampleRate; }
    uint32_t GetAspectRatio()   { return m_videoAspectRatio; }
    eFrameType GetFrameType()   { return m_type; }

private:
    uint8_t* m_pData;
    int m_count;
    int m_length;
    uint32_t m_pts;
    eFrameType m_type;

    uint32_t m_audioChannelCount;
    uint32_t m_audioSampleRate;
    uint32_t m_videoAspectRatio;
};

// ==================================
class cDxr3SyncBuffer : public cRingBuffer, private Uncopyable {
public:
    enum eSyncBufferException
    {
	SYNC_BUFFER_OVERRUN
    };
public:
    cDxr3SyncBuffer(int frameCount, int frameLength,
		    cDxr3Interface& dxr3Device);
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
    void WaitForSysClock(uint32_t pts, uint32_t delta);
    void WaitForNextPut(void);
    void WakeUp(void);
    void WaitForReceiverStopped(void);
    void SetDemuxMode(eDxr3DemuxMode demuxMode)
    {
	m_demuxMode = demuxMode;
    };
    eDxr3DemuxMode GetDemuxMode(void)
    {
	return m_demuxMode;
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
    bool m_bWaitPts;
    bool m_bPutBlock;
    bool m_bGetBlock;
    bool m_bStartReceiver;
    bool m_bStopped;
    uint32_t m_waitPts;
    uint32_t m_waitDelta;
    uint32_t m_lastPts;
    eDxr3DemuxMode m_demuxMode;

    cCondVar receiverStopped;
    cMutex receiverStoppedMutex;
    cDxr3Interface& m_dxr3Device;
    bool m_bPollSync;

    cDxr3SyncBuffer(); // you are not allowed to use this constructor
};

#endif /*_DXR3SYNCBUFFER_H_*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
