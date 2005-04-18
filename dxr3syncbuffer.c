/*
 * dxr3syncbuffer.c
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

/*
  ToDo:
  - cDxr3SyncBuffer::Push: XXX This is only a workaround until a sufficient control algorithm is implemented
*/

#include <unistd.h>
#include "dxr3syncbuffer.h"
#include "dxr3memcpy.h"

const int DXR3_MAX_VIDEO_FRAME_LENGTH = 4096;
const int DXR3_MAX_AUDIO_FRAME_LENGTH = 4096;

// ==================================
//! constructor
cFixedLengthFrame::cFixedLengthFrame() : 
  m_count(0), m_length(0), m_pts(0), m_type(ftUnknown) {

  m_audioChannelCount = UNKNOWN_CHANNEL_COUNT;
  m_audioDataRate = UNKNOWN_DATA_RATE;
  m_videoAspectRatio = UNKNOWN_ASPECT_RATIO;
}

// ==================================
cFixedLengthFrame::~cFixedLengthFrame()
{
  if (m_pData)
    {
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
  if (!m_pData)
    {
      cLog::Instance() << "Failed to allocate memory in cFixedLengthFrame (m_pData) - will stop now";
      exit(1);		
    }
}

// ==================================
void cFixedLengthFrame::CopyFrame(const uint8_t* pStart, int length, uint32_t pts, eFrameType type) 
{
  if (length > m_length) 
    {
      delete[] m_pData;
      m_pData = new uint8_t[length];
      m_length = length;
    }
  m_type = type;
  m_count = length;
  m_pts = pts;
  dxr3_memcpy((void*) m_pData,(void*) pStart, length); 
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
uint32_t cFixedLengthFrame::m_staticAudioChannelCount = 0;
uint32_t cFixedLengthFrame::m_staticAudioDataRate = 0;


// ==================================
//! constructor
cDxr3SyncBuffer::cDxr3SyncBuffer(int frameCount, int frameLength, cDxr3Interface& dxr3Device) : cRingBuffer(frameCount, true), m_dxr3Device(dxr3Device) 
{
  m_pBuffer = new cFixedLengthFrame[frameCount];

  // got we a valid m_pBuffer?
  if (!m_pBuffer)
    {
      cLog::Instance() << "Failed to allocate memory in cDxr3SyncBuffer (m_pBuffer) - will stop now";
      exit(1);
    }

  // init our new m_pBuffer;
  for (int i = 0; i < frameCount; i++)
    {
      m_pBuffer[i].Init(frameLength);
    }

  // set some default values
  m_count = 0;
  m_nextFree = 0;
  m_next = 0;
  m_bWaitPts = false;
  m_waitPts = 0;
  m_waitDelta = 0;
  m_lastPts = 0;
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
  if (m_pBuffer)
    {
      delete[] m_pBuffer;
    }
}

// ==================================
int cDxr3SyncBuffer::Available(void) 
{
  int ret = 0;
#if VDRVERSNUM < 10313
  Lock();
#endif
  ret = m_count;
#if VDRVERSNUM < 10313
  Unlock();
#endif
  return ret;
}

// ==================================
const int BUFFER_LIMIT = 5;
const int BUFFER_LIMIT_2 = 10;

// ==================================
bool cDxr3SyncBuffer::Poll(int TimeoutMs) 
{
  bool retVal = true;
  uint32_t currTime = m_dxr3Device.GetSysClock();
  m_bPollSync = true;
  if (m_demuxMode == DXR3_DEMUX_REPLAY_MODE) 
    {
      if (Available() >= Size() - (Size()*BUFFER_LIMIT/100)) 
	{
	  m_bPollSync = true;    
	  while ((Available() >= Size() - (Size()*BUFFER_LIMIT_2)/100) && ((m_dxr3Device.GetSysClock() - currTime) < ((uint32_t)TimeoutMs * (uint32_t)45))) 
	    {
	      m_bPutBlock = true;
	      EnableGet();
	      m_bWaitPts = false;
	      WaitForPut();               
            }                    
	  if (Available() >= Size() - (Size()*BUFFER_LIMIT_2)/100) 
	    {
	      retVal = false;
            }
        }        
    }

  return retVal;
}

// ==================================
cFixedLengthFrame* cDxr3SyncBuffer::Push(const uint8_t* pStart, int length, uint32_t pts, eFrameType type)  throw (eSyncBufferException) 
{
  int lastIndex = 0;

  switch (m_demuxMode) 
    {
    case DXR3_DEMUX_TRICK_MODE:
      break;

    case DXR3_DEMUX_TV_MODE:
    case DXR3_DEMUX_REPLAY_MODE:
    default:
        
      while ((Available() >= Size() - (Size()*10)/100)) 
	{
	  m_bPutBlock = true;
	  EnableGet();
	  m_bWaitPts = false;
	  WaitForPut();
	}
        
#if VDRVERSNUM < 10313
      Lock();
#endif
      if (pts == m_lastPts) 
	{
	  pts = 0;
	} 
      else 
	{
	  m_lastPts = pts;
	}
      lastIndex = m_nextFree;
      m_pBuffer[m_nextFree].CopyFrame(pStart, length, pts, type);
      m_pBuffer[m_nextFree].SetChannelCount(UNKNOWN_CHANNEL_COUNT);
      m_pBuffer[m_nextFree].SetDataRate(UNKNOWN_DATA_RATE);
      m_pBuffer[m_nextFree].SetAspectRatio(UNKNOWN_ASPECT_RATIO);
      m_nextFree++;
      m_count++;
      m_nextFree %= Size();

      if (m_nextFree == m_next) 
	{
	  cLog::Instance() << "Buffer overrun\n"; 
	  //                cLog::Instance() << "cDxr3SyncBuffer::Push m_demuxMode: " << (int)m_demuxMode << endl;
	  //                cLog::Instance() << "cDxr3SyncBuffer::Push Available(): " << Available() << endl;
	  //                cLog::Instance() << "cDxr3SyncBuffer::Push Size(): " << Size() << endl;

	  Clear(); // XXX This is only a workaround until a sufficient control algorithm is implemented
	  throw(SYNC_BUFFER_OVERRUN);
	}
      if (!m_bWaitPts) 
	{
	  if (m_bStartReceiver) 
	    {
	      EnableGet();
	    }
	} 
      else
	{
	  if (m_waitPts < m_dxr3Device.GetSysClock() || 
	      m_waitPts - m_dxr3Device.GetSysClock() < m_waitDelta) 
	    {
	      EnableGet();
	      m_bWaitPts = false;
	    }
	}
#if VDRVERSNUM < 10313
      Unlock();
#endif
      break;
    }

  return &m_pBuffer[lastIndex];
}

// ==================================
void cDxr3SyncBuffer::Pop(void) 
{
#if VDRVERSNUM < 10313
  Lock();
#endif
  if (m_count) 
    {
      uint32_t nextPts = 0;
      uint32_t tmpBuffer = m_next;
      for (int i = 0; i < m_count && nextPts == 0; ++i) 
	{
	  if (tmpBuffer) tmpBuffer = --tmpBuffer ? tmpBuffer : (Size() - 1);
	  nextPts = m_pBuffer[tmpBuffer].GetPts();
        }
      if (nextPts != 30) 
	{
	  cDxr3NextPts::Instance().SetNextPts(nextPts);
        }
        
      m_next++;
      m_count--;
      m_next %= Size();
      if (m_next == m_nextFree) 
	{
	  m_next = m_nextFree = m_count = 0;
        } 
    }
  EnablePut();
#if VDRVERSNUM < 10313
  Unlock();
#endif
}

// ==================================
cFixedLengthFrame* cDxr3SyncBuffer::Get(void) 
{
  cFixedLengthFrame* pRet = 0;
       
  if (!m_bStopped) 
    {
      while (!Available() || !m_bStartReceiver) 
	{
	  m_bGetBlock = true;
	  ReceiverStopped();
	  WaitForGet();
        }
        
#if VDRVERSNUM < 10313
      Lock();
#endif
      if (m_nextFree != m_next) 
	{
	  pRet = &m_pBuffer[m_next]; 
        }
#if VDRVERSNUM < 10313
      Unlock();
#endif
    } 
  else 
    {
      WaitForGet();
    }

  return pRet;
}

// ==================================
void cDxr3SyncBuffer::Clear(void) 
{
#if VDRVERSNUM < 10313
  Lock();
#endif
  m_next = 0;
  m_nextFree = 0;
  m_count = 0;
  m_lastPts = 0;
  m_bWaitPts = false;
  m_bStartReceiver = false;
  m_bPollSync = false;
  if (m_bPutBlock) 
    {
      EnablePut();
      m_bPutBlock = false;
    }
  cFixedLengthFrame::Clear();
  cDxr3NextPts::Instance().Clear();
#if VDRVERSNUM < 10313
  Unlock();
#endif
}

// ==================================
void cDxr3SyncBuffer::WaitForSysClock(uint32_t pts, uint32_t delta) 
{
  m_waitPts = pts;
  m_waitDelta = delta;
  if (!m_bPutBlock) 
    {
#if VDRVERSNUM < 10313
      Lock();
#endif
      m_bWaitPts = true;
#if VDRVERSNUM < 10313
      Unlock();
#endif
      m_bGetBlock = true;
      ReceiverStopped();
      WaitForGet();
    }
  else
    {
      usleep(1); //* (pts - pSysClock->GetSysClock()));
    }
}

// ==================================
void cDxr3SyncBuffer::WaitForNextPut(void) 
{
  if (!m_bPutBlock) 
    {
      m_bGetBlock = true;
      ReceiverStopped();
      WaitForGet();
    }
  else
    {
      usleep(1);
    }
}

// ==================================
void cDxr3SyncBuffer::Start(void) 
{
  m_bStartReceiver = true;
  m_bStopped = false;
  if (Available()) 
    {
      EnableGet();
    }
}

// ==================================
void cDxr3SyncBuffer::WakeUp(void) 
{
#if VDRVERSNUM < 10313
  Lock();
#endif
  if (m_bStartReceiver == true) 
    {
      if (!m_bWaitPts) 
	{
	  EnableGet();
        }
      else
	{
	  if (m_waitPts < m_dxr3Device.GetSysClock() || 
	      m_waitPts - m_dxr3Device.GetSysClock() < m_waitDelta) 
	    {
	      EnableGet();
	      m_bWaitPts = false;
            }
        }
    }
#if VDRVERSNUM < 10313
  Unlock();
#endif
}

// ==================================
void cDxr3SyncBuffer::WaitForReceiverStopped(void) 
{
  if (!m_bGetBlock) 
    {
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
