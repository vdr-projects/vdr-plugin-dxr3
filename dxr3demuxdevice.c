/*
 * dxr3demuxdevice.c
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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include "dxr3demuxdevice.h"
#include <linux/em8300.h>
#include "dxr3log.h"
#include "dxr3pesframe.h"
#include "dxr3configdata.h"
#include "dxr3log.h"

// ==================================
//! constructor
cDxr3DemuxDevice::cDxr3DemuxDevice(cDxr3Interface& dxr3Device) :
m_dxr3Device(dxr3Device),
m_aBuf(AUDIO_MAX_BUFFER_SIZE, AUIDO_MAX_FRAME_SIZE, m_dxr3Device), 
m_vBuf(VIDEO_MAX_BUFFER_SIZE, VIDEO_MAX_FRAME_SIZE, m_dxr3Device) 
{
	m_ReUseFrame = 1;
    m_synchState = DXR3_DEMUX_UNSYNCHED;
    m_demuxMode = DXR3_DEMUX_OFF_MODE;
    m_pAudioThread = new cDxr3AudioOutThread(dxr3Device, m_aBuf);
	if (!m_pAudioThread)
	{
		cLog::Instance() << "cDxr3DemuxDevice::cDxr3DemuxDevice: failed to allocate memory\n";
		exit(1);
	}
    m_pAudioThread->Start();

    m_pVideoThread = new cDxr3VideoOutThread(dxr3Device, m_vBuf);
	if (!m_pVideoThread)
	{
		cLog::Instance() << "cDxr3DemuxDevice::cDxr3DemuxDevice: failed to allocate memory\n";
		exit(1);
	}
    m_pVideoThread->Start();
    m_aDecoder.Init();
}

// ==================================
cDxr3DemuxDevice::cDxr3DemuxDevice() : // dummy constructor
m_dxr3Device(cDxr3Interface::Instance()),
m_aBuf(AUDIO_MAX_BUFFER_SIZE, AUIDO_MAX_FRAME_SIZE, m_dxr3Device), 
m_vBuf(VIDEO_MAX_BUFFER_SIZE, VIDEO_MAX_FRAME_SIZE, m_dxr3Device) 
{
    m_synchState = DXR3_DEMUX_UNSYNCHED;
    m_demuxMode = DXR3_DEMUX_OFF_MODE;
}

// ==================================
// deconstr.
cDxr3DemuxDevice::~cDxr3DemuxDevice() 
{
	if (!m_pVideoThread)
	{
		delete m_pVideoThread;
	}

	if (!m_pVideoThread)
	{
		delete m_pVideoThread;
	}
}

// ==================================
// stop demuxing process
void cDxr3DemuxDevice::Stop() 
{
    m_dxr3Device.DisableVideo();
    m_dxr3Device.DisableAudio();
    m_vBuf.Clear();
    m_aBuf.Clear();
    m_vBuf.WakeUp();
    m_aBuf.WakeUp();
    m_aDecoder.Init();
    m_vBuf.WaitForReceiverStopped();
    m_aBuf.WaitForReceiverStopped();
    m_synchState = DXR3_DEMUX_UNSYNCHED;
    m_demuxMode = DXR3_DEMUX_OFF_MODE;

    m_dxr3Device.PlayBlackFrame();
    m_dxr3Device.ReOpenAudio();            
}

// ==================================
void cDxr3DemuxDevice::Resync() 
{
    m_dxr3Device.DisableVideo();
    m_dxr3Device.DisableAudio();
    m_vBuf.Clear();
    m_aBuf.Clear();
    m_vBuf.WakeUp();
    m_aBuf.WakeUp();
    m_aDecoder.Init();
    m_vBuf.WaitForReceiverStopped();
    m_aBuf.WaitForReceiverStopped();
    m_synchState = DXR3_DEMUX_UNSYNCHED;
    m_demuxMode = DXR3_DEMUX_OFF_MODE;
}

// ==================================
void cDxr3DemuxDevice::Clear() 
{
    m_dxr3Device.DisableVideo();
    m_dxr3Device.DisableAudio();
    m_vBuf.Clear();
    m_aBuf.Clear();
    m_vBuf.WakeUp();
    m_aBuf.WakeUp();
    m_aDecoder.Init();
    m_vBuf.WaitForReceiverStopped();
    m_aBuf.WaitForReceiverStopped();
    m_synchState = DXR3_DEMUX_UNSYNCHED;
    m_demuxMode = DXR3_DEMUX_OFF_MODE;
}

// ==================================
void cDxr3DemuxDevice::Init() 
{
    m_vBuf.Clear();
    m_aBuf.Clear();
    m_synchState = DXR3_DEMUX_UNSYNCHED;
    m_demuxMode = DXR3_DEMUX_OFF_MODE;
    m_aDecoder.Init();
}

// ==================================
void cDxr3DemuxDevice::SetTvMode() 
{
    m_synchState = DXR3_DEMUX_UNSYNCHED;
    m_demuxMode = DXR3_DEMUX_TV_MODE;
    m_aBuf.SetDemuxMode(DXR3_DEMUX_TV_MODE);
    m_vBuf.SetDemuxMode(DXR3_DEMUX_TV_MODE);
    m_aBuf.Start();
    m_vBuf.Start();
}

// ==================================
void cDxr3DemuxDevice::SetAudioOnlyMode() 
{
    m_synchState = DXR3_DEMUX_UNSYNCHED;
    m_demuxMode =  DXR3_DEMUX_AUDIO_ONLY_MODE;
    m_aBuf.SetDemuxMode(DXR3_DEMUX_REPLAY_MODE);
    m_vBuf.SetDemuxMode(DXR3_DEMUX_REPLAY_MODE);
    m_aBuf.Start();
    m_vBuf.Start();
}    

// ==================================
void cDxr3DemuxDevice::SetReplayMode() 
{
    if (m_demuxMode != DXR3_DEMUX_REPLAY_MODE) 
	{
        if (m_demuxMode == DXR3_DEMUX_TRICK_MODE && m_trickState == DXR3_FREEZE) 
		{
            m_dxr3Device.SetPlayMode();
            m_dxr3Device.SetSysClock(m_stopScr);
            m_dxr3Device.EnableVideo();
            m_dxr3Device.EnableAudio();
            m_vBuf.Start();
            m_aBuf.Start();
            m_vBuf.WakeUp();
            m_aBuf.WakeUp();
        }
		else 
		{
            m_synchState = DXR3_DEMUX_UNSYNCHED;
            m_aBuf.SetDemuxMode(DXR3_DEMUX_REPLAY_MODE);
            m_vBuf.SetDemuxMode(DXR3_DEMUX_REPLAY_MODE);
        }
    }
    m_demuxMode = DXR3_DEMUX_REPLAY_MODE;
}

// ==================================
void cDxr3DemuxDevice::SetTrickMode(eDxr3TrickState trickState, int Speed) 
{
    m_demuxMode = DXR3_DEMUX_TRICK_MODE;
    m_trickState = trickState;
    m_dxr3Device.DisableAudio();

    if (m_demuxMode == DXR3_DEMUX_TRICK_MODE && m_trickState == DXR3_FREEZE) 
	{
        m_stopScr = m_dxr3Device.GetSysClock();
        // m_dxr3Device.Pause();
        m_vBuf.Stop();
        m_aBuf.Stop();
    }
	else 
	{
        m_vBuf.Clear();
        m_aBuf.Clear();
    }

	m_ReUseFrame = 1;//Speed;
}

// ==================================
void cDxr3DemuxDevice::SetVideoOnlyMode() 
{
    m_demuxMode = DXR3_DEMUX_VIDEO_ONLY_MODE;
    m_dxr3Device.DisableAudio();
    
    if (m_demuxMode == DXR3_DEMUX_TRICK_MODE && m_trickState == DXR3_FREEZE) 
	{
        m_stopScr = m_dxr3Device.GetSysClock();
        // m_dxr3Device.Pause();
        m_vBuf.Stop();
        m_aBuf.Stop();
    } 
	else 
	{
        m_vBuf.Clear();
        m_aBuf.Clear();
    }
    m_dxr3Device.SetPlayMode();
    
}

#if VDRVERSNUM < 10307
// ==================================
cOsdBase* cDxr3DemuxDevice::NewOsd(int x, int y) 
{
    return m_dxr3Device.NewOsd(x, y);
}
#endif 

// ==================================
void cDxr3DemuxDevice::StillPicture(const uint8_t* buf, int length) 
{
    m_vBuf.Clear();
    m_aBuf.Clear();
    m_demuxMode = DXR3_DEMUX_TRICK_MODE;
    m_trickState = DXR3_FREEZE;
    m_dxr3Device.SingleStep();

	cLog::Instance() << "StillPicture: len = " << length << "\n";

    DemuxPes(buf, length);
    DemuxPes(buf, length);
    DemuxPes(buf, length);
}

// ==================================
int cDxr3DemuxDevice::DemuxPes(const uint8_t* buf, int length, bool bAc3Dts) 
{
    uint32_t pts = 0;
    static uint32_t aPts = 0;
    static uint32_t vPts = 0;
    static uint32_t lastPts = 0;
    static bool bPlaySuc = false;
    static bool bPlayedFrame = false;
    int origLength = length;
	
    int scr = 0;
    int pcr = 0;
	
    scr = m_dxr3Device.GetSysClock();
	
    // printf("vBuf size = %d\n", m_vBuf.Available());
    // printf("aBuf size = %d\n", m_aBuf.Available());
	/*
    if (cDxr3ConfigData::Instance().GetAc3OutPut()) {
	cDxr3AbsDevice::Instance().SetAudioDigitalAC3(); // !!! FIXME
    }
	*/
    
    if (m_pAudioThread->NeedResync() || m_pVideoThread->NeedResync()) 
	{
        Resync();
        if (m_demuxMode == DXR3_DEMUX_REPLAY_MODE) 
		{
            SetReplayMode();
        }
        m_aBuf.Clear();
        m_vBuf.Clear();
        m_pAudioThread->ClearResyncRequest();
        m_pVideoThread->ClearResyncRequest();
        m_aDecoder.Reset();
        lastPts = 0;
        aPts = 0;
        vPts = 0;
        bPlaySuc = false;       
	}
	
    if (m_demuxMode == DXR3_DEMUX_OFF_MODE) 
	{
        m_demuxMode = DXR3_DEMUX_TV_MODE;
        m_synchState = DXR3_DEMUX_UNSYNCHED;        
		
        lastPts = 0;
        aPts = 0;
        vPts = 0;
        bPlaySuc = false;
		/*        
        if (cDxr3ConfigData::Instance().GetAc3OutPut()) {
		cDxr3AbsDevice::Instance().SetAudioDigitalAC3(); // !!! FIXME
        }
		*/
    }

    // find start code
    try 
	{
        cDxr3PesFrame pesFrame;
		
        pesFrame.ExtractNextFrame(buf, length);
		
        while (pesFrame.IsValid()) 
		{
			if (pesFrame.GetEsLength() > (uint32_t) VIDEO_MAX_FRAME_SIZE) { throw (cDxr3PesFrame::PES_GENERAL_ERROR);};
			if (pesFrame.GetPts() != lastPts) 
			{
				pts = lastPts = pesFrame.GetPts();
			} 
			else 
			{
				pts = 0;
			}
			
			if (pesFrame.GetPesDataType() == cDxr3PesFrame::PES_VIDEO_DATA) 
			{
				//  m_dxr3Device.PlayVideoFrame(pesFrame.GetEsStart(), (int) (pesFrame.GetEsLength()));
				
				if (m_demuxMode == DXR3_DEMUX_TRICK_MODE) 
				{                
					switch (pesFrame.GetFrameType()) 
					{
                    case I_FRAME:
						cLog::Instance() << "i - frame\n";
                        m_dxr3Device.SingleStep();
                        bPlaySuc = true;
                        // if (bPlayedFrame) return length;
                        bPlayedFrame = true;
                        // usleep(30000); // otherwise there is problem with audio (driver bug?)
                        m_dxr3Device.SetHorizontalSize(pesFrame.GetHorizontalSize());
                        m_dxr3Device.PlayVideoFrame(pesFrame.GetEsStart(), (int) (pesFrame.GetEsLength()), m_ReUseFrame);
                        break;
						
                    case UNKNOWN_FRAME:
						cLog::Instance() << "frame unknown\n";
                        if (bPlaySuc) 
						{
                            m_dxr3Device.PlayVideoFrame(pesFrame.GetEsStart(), (int) (pesFrame.GetEsLength()), m_ReUseFrame);
                        }
                        break;
						
					default:
						cLog::Instance() << "default frame\n";
                        if (bPlaySuc) 
						{
                            m_dxr3Device.PlayVideoFrame(pesFrame.GetEsStart(), (int) (pesFrame.GetOffset()), m_ReUseFrame);
                        }
                        
                        bPlaySuc = false;                        
                        break;
					}
					
				} 
				else if (m_demuxMode == DXR3_DEMUX_VIDEO_ONLY_MODE) 
				{
					m_dxr3Device.PlayVideoFrame(pesFrame.GetEsStart(), (int) (pesFrame.GetEsLength()));
				} 
				else if (m_synchState == DXR3_DEMUX_VIDEO_SYNCHED || m_synchState == DXR3_DEMUX_SYNCHED) 
				{
					m_dxr3Device.SetHorizontalSize(pesFrame.GetHorizontalSize());
					while(!Poll(100));
					cFixedLengthFrame* pTempFrame = m_vBuf.Push(pesFrame.GetEsStart(), (int) (pesFrame.GetEsLength()), pts, ftVideo);
					pTempFrame->SetAspectRatio(pesFrame.GetAspectRatio());
					
					m_aBuf.WakeUp();
					
					if (m_vBuf.GetFillLevel() > 5 && m_synchState != DXR3_DEMUX_SYNCHED) 
					{
						m_synchState = DXR3_DEMUX_SYNCHED;
						pcr = vPts - PRE_BUFFER_LENGTH;
						m_dxr3Device.SetSysClock(pcr);
						m_dxr3Device.SetPlayMode();
						m_dxr3Device.EnableVideo();
						m_dxr3Device.EnableAudio();
						m_vBuf.Start();
						m_aBuf.Start();
					}
				} 
				else 
				{
					if (pesFrame.GetFrameType() == I_FRAME) 
					{
						vPts = pts;
						
						m_dxr3Device.SetHorizontalSize(pesFrame.GetHorizontalSize());
						cFixedLengthFrame* pTempFrame = m_vBuf.Push(pesFrame.GetEsStart(), (int) (pesFrame.GetEsLength()), pts, ftVideo);
						pTempFrame->SetAspectRatio(pesFrame.GetAspectRatio());
						
						if (m_synchState == DXR3_DEMUX_AUDIO_SYNCHED) 
						{
							m_synchState = DXR3_DEMUX_SYNCHED;
						} 
						else 
						{
							m_synchState = DXR3_DEMUX_VIDEO_SYNCHED;
						}
						if (m_synchState == DXR3_DEMUX_SYNCHED) 
						{
							if (!vPts) vPts = aPts;
							if (aPts < vPts) 
							{
								pcr = aPts - PRE_BUFFER_LENGTH;
							} 
							else 
							{
								pcr = vPts - PRE_BUFFER_LENGTH;
							}
							m_dxr3Device.SetSysClock(pcr);
							m_dxr3Device.SetPlayMode();
							m_dxr3Device.EnableVideo();
							m_dxr3Device.EnableAudio();
							m_vBuf.Start();
							m_aBuf.Start();
						}
					}
				}
				
			} 
			else if (pesFrame.GetPesDataType() == cDxr3PesFrame::PES_AUDIO_DATA
				&& m_demuxMode != DXR3_DEMUX_VIDEO_ONLY_MODE
				&& !cDxr3ConfigData::Instance().GetAc3OutPut()) 
			{
				if (m_synchState == DXR3_DEMUX_AUDIO_SYNCHED || m_synchState == DXR3_DEMUX_SYNCHED) 
				{
					if (pts && m_synchState != DXR3_DEMUX_SYNCHED) 
					{
						m_synchState = DXR3_DEMUX_SYNCHED;
						pcr = aPts - PRE_BUFFER_LENGTH;
						m_dxr3Device.SetSysClock(pcr);
						m_dxr3Device.SetPlayMode();
						m_dxr3Device.EnableVideo();
						m_dxr3Device.EnableAudio();
						m_vBuf.Start();
						m_aBuf.Start();
					}
					while(!Poll(100));               
					m_aDecoder.Decode(pesFrame.GetEsStart(), (int) (pesFrame.GetEsLength()), pts, m_aBuf);
					
				} 
				else 
				{
					if (pts) 
					{
						aPts = pts;
						
						m_aDecoder.Decode(pesFrame.GetEsStart(), (int) (pesFrame.GetEsLength()), pts, m_aBuf);
						
						if (m_synchState == DXR3_DEMUX_VIDEO_SYNCHED) 
						{
							m_synchState = DXR3_DEMUX_SYNCHED;
						} 
						else 
						{
							m_synchState = DXR3_DEMUX_AUDIO_SYNCHED;
						}
						if (m_synchState == DXR3_DEMUX_SYNCHED) 
						{
							if (!vPts) vPts = aPts;
							if (aPts < vPts) 
							{
								pcr = aPts - PRE_BUFFER_LENGTH;
							} 
							else 
							{
								pcr = vPts - PRE_BUFFER_LENGTH;
							}
							m_dxr3Device.SetSysClock(pcr);
							m_dxr3Device.SetPlayMode();
							m_dxr3Device.EnableVideo();
							m_dxr3Device.EnableAudio();
							m_vBuf.Start();
							m_aBuf.Start();
						}
					}
				}
			} 
			else if (pesFrame.GetPesDataType() == cDxr3PesFrame::PES_PRIVATE_DATA
				&& m_demuxMode != DXR3_DEMUX_VIDEO_ONLY_MODE
				&& !cDxr3ConfigData::Instance().GetAc3OutPut()
				&& !bAc3Dts) 
			{
				if (m_synchState == DXR3_DEMUX_AUDIO_SYNCHED || m_synchState == DXR3_DEMUX_SYNCHED) 
				{
					m_aDecoder.DecodeLpcm(pesFrame.GetEsStart(), pesFrame.GetEsLength(), pts, m_aBuf);
				}
				else 
				{                
					if (pts) 
					{                 
						aPts = pts;
						m_aDecoder.DecodeLpcm(pesFrame.GetEsStart(), pesFrame.GetEsLength(), pts, m_aBuf);
						
						if (m_synchState == DXR3_DEMUX_VIDEO_SYNCHED) 
						{
							m_synchState = DXR3_DEMUX_SYNCHED;
						} 
						else 
						{
							m_synchState = DXR3_DEMUX_AUDIO_SYNCHED;
						}
						if (m_synchState == DXR3_DEMUX_SYNCHED) 
						{
							if (!vPts) vPts = aPts;
							if (aPts < vPts) 
							{
								pcr = aPts - PRE_BUFFER_LENGTH;
							} 
							else 
							{
								pcr = vPts - PRE_BUFFER_LENGTH;
							}
							m_dxr3Device.SetSysClock(pcr);
							m_dxr3Device.SetPlayMode();
							m_dxr3Device.EnableVideo();
							m_dxr3Device.EnableAudio();
							m_vBuf.Start();
							m_aBuf.Start();
						}
					}
				}
			} 
			else if (pesFrame.GetPesDataType() == cDxr3PesFrame::PES_PRIVATE_DATA
				&& m_demuxMode != DXR3_DEMUX_VIDEO_ONLY_MODE
				&& cDxr3ConfigData::Instance().GetAc3OutPut()
				&& bAc3Dts) 
			{
				if (m_synchState == DXR3_DEMUX_AUDIO_SYNCHED || m_synchState == DXR3_DEMUX_SYNCHED) 
				{
					m_aDecoder.DecodeAc3Dts(pesFrame.GetPesStart(), pesFrame.GetEsStart(), pesFrame.GetEsLength(), pts, m_aBuf);
				} 
				else 
				{
					if (pts) 
					{
						aPts = pts;
						m_aDecoder.DecodeAc3Dts(pesFrame.GetPesStart(), pesFrame.GetEsStart(), pesFrame.GetEsLength(), pts, m_aBuf);
						
						if (m_synchState == DXR3_DEMUX_VIDEO_SYNCHED) 
						{
							m_synchState = DXR3_DEMUX_SYNCHED;
						} 
						else 
						{
							m_synchState = DXR3_DEMUX_AUDIO_SYNCHED;
						}
						if (m_synchState == DXR3_DEMUX_SYNCHED) 
						{
							if (!vPts) vPts = aPts;
							if (aPts < vPts) 
							{
								pcr = aPts - PRE_BUFFER_LENGTH;
							} 
							else 
							{
								pcr = vPts - PRE_BUFFER_LENGTH;
							}
							m_dxr3Device.SetSysClock(pcr);
							m_dxr3Device.SetPlayMode();
							m_dxr3Device.EnableVideo();
							m_dxr3Device.EnableAudio();
							m_vBuf.Start();
							m_aBuf.Start();
						}
					}
				}
				
			} 
			
			if (pesFrame.IsValid()) 
			{
				pesFrame.ExtractNextFrame(pesFrame.GetNextStart(), pesFrame.GetRemainingLength());
			}
			
       }
	   
	   length -= pesFrame.GetRemainingLength();
	   
	   // if (m_demuxMode == DXR3_DEMUX_TRICK_MODE) return origLength;
	   return length;    
    } 
	catch (cDxr3PesFrame::ePesFrameError err) 
	{
        dsyslog("cDxr3DemuxDevice::DemuxPes() ePesFrameError skipping data and resync");
        Resync();
        return origLength;
    } 
	catch (cDxr3SyncBuffer::eSyncBufferException err) 
	{
        Stop();
        return origLength;
    }
}

// ==================================
int cDxr3DemuxDevice::DemuxAudioPes(const uint8_t* buf, int length) 
{
    static int syncCounter = 0;
    int origLength = length;

    m_demuxMode = DXR3_DEMUX_AUDIO_ONLY_MODE;
    m_aBuf.SetDemuxMode(DXR3_DEMUX_REPLAY_MODE);
    m_vBuf.SetDemuxMode(DXR3_DEMUX_REPLAY_MODE);

    try 
	{
        cDxr3PesFrame pesFrame;

        pesFrame.ExtractNextFrame(buf, length);

        while (pesFrame.IsValid()) 
		{
            if (pesFrame.GetPesDataType() == cDxr3PesFrame::PES_PRIVATE_DATA) 
			{
                if (m_synchState != DXR3_DEMUX_AUDIO_SYNCHED && syncCounter > 2) 
				{
                    m_synchState = DXR3_DEMUX_AUDIO_SYNCHED;
                    m_dxr3Device.SetPlayMode();
                    m_dxr3Device.EnableVideo();
                    m_dxr3Device.EnableAudio();
                    m_vBuf.Start();
                    m_aBuf.Start();
                }
                if (m_synchState != DXR3_DEMUX_AUDIO_SYNCHED && syncCounter <= 2) 
				{
                    syncCounter++;
                }
                while (!m_aBuf.Poll(100));
                m_aDecoder.DecodeLpcm(pesFrame.GetEsStart(), pesFrame.GetEsLength(), 0, m_aBuf);

            } 

            if (pesFrame.IsValid()) 
			{
                pesFrame.ExtractNextFrame(pesFrame.GetNextStart(), pesFrame.GetRemainingLength());
            }
        }

        length -= pesFrame.GetRemainingLength();

        return length;
    } 
	catch (cDxr3PesFrame::ePesFrameError err) 
	{
        dsyslog("cDxr3DemuxDevice::DemuxAudioPes() ePesFrameError skipping data and resync");
        Stop();
        return origLength;
    } 
}
