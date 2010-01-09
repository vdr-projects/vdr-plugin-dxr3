/*
 * dxr3syncbuffer.c
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

/*
  ToDo:
  - cDxr3SyncBuffer::Push: XXX This is only a workaround until a
  sufficient control algorithm is implemented
*/

#include <sys/time.h>
#include "dxr3syncbuffer.h"

const int DXR3_MAX_VIDEO_FRAME_LENGTH = 4096;
const int DXR3_MAX_AUDIO_FRAME_LENGTH = 4096;

// ==================================
cFixedLengthFrame::~cFixedLengthFrame()
{
    if (m_pData) {
        delete[] m_pData;
    }
}

// ==================================
// ! setup our frame
void cFixedLengthFrame::Init(uint32_t lenght)
{
    m_length = lenght;
    m_pData = new uint8_t[lenght];

    // allocation ok?
    if (!m_pData) {
        esyslog("dxr3: fatal: unable to allocate memory for new frame");
        exit(1);
    }
}

// ==================================
void cFixedLengthFrame::CopyFrame(const uint8_t* pStart, int length, uint32_t pts)
{
    if (length > m_length) {
        delete[] m_pData;
        m_pData = new uint8_t[length];
        m_length = length;
    }
    m_count = length;
    m_pts = pts;
    memcpy((void*) m_pData, (void*) pStart, length);
}

// ==================================
uint8_t* cFixedLengthFrame::GetData(void)
{
    return m_pData;
}

// ==================================
int cFixedLengthFrame::GetCount(void)
{
    return m_count;
}

// ==================================
uint32_t cFixedLengthFrame::GetPts(void)
{
    return m_pts;
}

// ==================================
void cFixedLengthFrame::SetPts(uint32_t pts)
{
    m_pts = pts;
}

// ==================================
//! constructor
cDxr3SyncBuffer::cDxr3SyncBuffer(int frameCount, int frameLength) :
    cRingBuffer(frameCount, true)
{
    m_dxr3Device = cDxr3Interface::instance();
    m_pBuffer = new cFixedLengthFrame[frameCount];

    // got we a valid m_pBuffer?
    if (!m_pBuffer) {
        esyslog("dxr3: fatal: unable to allocate memory for new frame");
        exit(1);
    }

    // init our new m_pBuffer;
    for (int i = 0; i < frameCount; i++) {
        m_pBuffer[i].Init(frameLength);
    }

    // set some default values
    m_count = 0;
    m_nextFree = 0;
    m_next = 0;
    m_bPutBlock = false;
    m_bGetBlock = false;
    m_bStartReceiver = false;
    m_bStopped = false;
    m_demuxMode = DXR3_DEMUX_TV_MODE;
    m_bPollSync = false;
    SetTimeouts(1000, 10);
}

// ==================================
cDxr3SyncBuffer::~cDxr3SyncBuffer()
{
    if (m_pBuffer) {
        delete[] m_pBuffer;
    }
}

// ==================================
int cDxr3SyncBuffer::Available(void)
{
    return m_count;
}

// ==================================
const int BUFFER_LIMIT = 5;
const int BUFFER_LIMIT_2 = 10;

// ==================================
bool cDxr3SyncBuffer::Poll(int TimeoutMs)
{
    bool retVal = true;
    uint32_t currTime = m_dxr3Device->GetSysClock();
    struct timeval tv_start, tv;
    m_bPollSync = true;
    gettimeofday(&tv_start, NULL);
    if (m_demuxMode == DXR3_DEMUX_REPLAY_MODE) {
        if (Available() >= Size() - (Size()*BUFFER_LIMIT/100)) {
            m_bPollSync = true;
            while ((Available() >= Size() - (Size()*BUFFER_LIMIT_2)/100) &&
               ((m_dxr3Device->GetSysClock() - currTime) <
                ((uint32_t)TimeoutMs * (uint32_t)45)))
            {
                int d_s, d_us, ms;
                m_bPutBlock = true;
                EnableGet();
                WaitForPut();
                gettimeofday(&tv, NULL);
                d_s  = tv.tv_sec  - tv_start.tv_sec;
                d_us = tv.tv_usec - tv_start.tv_usec;
                ms = d_s * 1000 + d_us / 1000;
                if (ms > TimeoutMs * 2) {
                    esyslog("dxr3: sync: secondary timeout");
                    break;
                }
            }
            if (Available() >= Size() - (Size()*BUFFER_LIMIT_2)/100) {
                retVal = false;
            }
        }
    }

    return retVal;
}

// ==================================
cFixedLengthFrame* cDxr3SyncBuffer::Push(const uint8_t* pStart, int length, uint32_t pts)  throw (eSyncBufferException)
{
    int lastIndex = 0;
    struct timeval tv_start, tv;
    gettimeofday(&tv_start, NULL);

    while ((Available() >= Size() - (Size()*10)/100)) {
        int d_s, d_us, ms;
        m_bPutBlock = true;
        EnableGet();
        WaitForPut();
        gettimeofday(&tv, NULL);
        d_s  = tv.tv_sec  - tv_start.tv_sec;
        d_us = tv.tv_usec - tv_start.tv_usec;
        ms = d_s * 1000 + d_us / 1000;
        if (ms > 2000) {
            esyslog("dxr3: sync: push timeout");
            return NULL;
        }
    }

    lastIndex = m_nextFree;
    m_pBuffer[m_nextFree].CopyFrame(pStart, length, pts);
    m_pBuffer[m_nextFree].channels(UNKNOWN_CHANNEL_COUNT);
    m_pBuffer[m_nextFree].samplerate(UNKNOWN_DATA_RATE);
    m_pBuffer[m_nextFree].aspectratio(UNKNOWN_ASPECT_RATIO);
    m_nextFree++;
    m_count++;
    m_nextFree %= Size();

    if (m_nextFree == m_next) {
        esyslog("dxr3: sync: push buffer overrun");
        Clear(); // XXX This is only a workaround until a sufficient control algorithm is implemented
        throw(SYNC_BUFFER_OVERRUN);
    }
    if (m_bStartReceiver) {
        EnableGet();
    }

    return &m_pBuffer[lastIndex];
}

// ==================================
void cDxr3SyncBuffer::Pop(void)
{
    if (m_count) {
        m_next++;
        m_count--;
        m_next %= Size();
        if (m_next == m_nextFree) {
            m_next = m_nextFree = m_count = 0;
        }
    }
    EnablePut();
}

// ==================================
cFixedLengthFrame* cDxr3SyncBuffer::Get(void)
{
    cFixedLengthFrame* pRet = 0;

    if (!m_bStopped) {
        while ((!Available() || !m_bStartReceiver) && !m_bStopped) {
            m_bGetBlock = true;
            ReceiverStopped();
            WaitForGet();
        }

        if (m_nextFree != m_next) {
            pRet = &m_pBuffer[m_next];
        }
    } else {
        WaitForGet();
    }

    return pRet;
}

// ==================================
void cDxr3SyncBuffer::Clear(void)
{
    m_next = 0;
    m_nextFree = 0;
    m_count = 0;
    m_bStartReceiver = false;
    m_bPollSync = false;
    if (m_bPutBlock) {
        EnablePut();
        m_bPutBlock = false;
    }
}

// ==================================
void cDxr3SyncBuffer::Start(void)
{
    m_bStartReceiver = true;
    m_bStopped = false;
    if (Available()) {
        EnableGet();
    }
}

// ==================================
void cDxr3SyncBuffer::WakeUp(void)
{
    if (m_bStartReceiver == true) {
        EnableGet();
    }
}

// ==================================
void cDxr3SyncBuffer::WaitForReceiverStopped(void)
{
    if (!m_bGetBlock) {
        receiverStoppedMutex.Lock();
        receiverStopped.Wait(receiverStoppedMutex);
        receiverStoppedMutex.Unlock();
    }
}

// ==================================
void cDxr3SyncBuffer::ReceiverStopped(void)
{
    receiverStopped.Broadcast();
}

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// tab-width: 4;
// indent-tabs-mode: nil
// End:
