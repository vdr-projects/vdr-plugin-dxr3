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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#ifndef _DXR3SYNCBUFFER_H_
#define _DXR3SYNCBUFFER_H_

#include <stdint.h>

#include "dxr3vdrincludes.h"
#include "dxr3interface.h"
#include "dxr3generaldefines.h"
#include "dxr3nextpts.h"

// ==================================
const uint32_t UNKNOWN_CHANNEL_COUNT = 0xFFFFFFFF;
const uint32_t UNKNOWN_DATA_RATE = 0xFFFFFFFF;
const uint32_t UNKNOWN_ASPECT_RATIO = 0xFFFFFFFF;

// ==================================
class cFixedLengthFrame
{
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
	if (channelCount != UNKNOWN_CHANNEL_COUNT)
	    m_staticAudioChannelCount = m_audioChannelCount = channelCount;
	else
	    m_audioChannelCount = m_staticAudioChannelCount;
    };
    void SetDataRate(uint32_t dataRate)
    {
	if (m_audioDataRate != UNKNOWN_DATA_RATE)
	    m_staticAudioDataRate = m_audioDataRate = dataRate;
	else
	    m_audioDataRate = m_staticAudioDataRate;
    };
    void SetAspectRatio(uint32_t aspectRatio)
    {
	m_videoAspectRatio = aspectRatio;
    };
    uint32_t GetChannelCount(void)
    {
	return ((m_audioChannelCount == m_staticAudioChannelCount ||
		 !m_staticAudioChannelCount)?
		m_audioChannelCount : m_staticAudioChannelCount);
    };
    uint32_t GetDataRate(void)
    {
	return ((m_audioDataRate == m_staticAudioDataRate ||
		 !m_staticAudioDataRate) ?
		m_audioDataRate : m_staticAudioDataRate);
    };
    uint32_t GetAspectRatio(void)
    {
	return m_videoAspectRatio;
    };
    eFrameType GetFrameType(void)
    {
	return m_type;
    }

    static void Clear(void)
    {
	m_staticAudioDataRate = 0;
	m_staticAudioChannelCount = 0;
    };

protected:
    uint8_t* m_pData;
    int m_count;
    int m_length;
    uint32_t m_pts;
    eFrameType m_type;

    uint32_t m_audioChannelCount;
    uint32_t m_audioDataRate;
    uint32_t m_videoAspectRatio;

    static uint32_t m_staticAudioChannelCount;
    static uint32_t m_staticAudioDataRate;

private:
    //cFixedLengthFrame(); // you are not allowed to use this constructor
    cFixedLengthFrame(cFixedLengthFrame&); // no copy constructor

};

// ==================================
class cDxr3SyncBuffer : public cRingBuffer
{
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

protected:
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

private:
    cDxr3SyncBuffer(); // you are not allowed to use this constructor
    cDxr3SyncBuffer(cDxr3SyncBuffer&); // no constructor
};

#endif /*_DXR3SYNCBUFFER_H_*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
