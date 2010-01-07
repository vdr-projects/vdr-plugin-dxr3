/*
 * dxr3demuxdevice.c
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

#include "dxr3demuxdevice.h"
#include "dxr3pesframe.h"

// ==================================
//! constructor
cDxr3DemuxDevice::cDxr3DemuxDevice() :
    m_dxr3Device(cDxr3Interface::instance()),
    m_aBuf(AUDIO_MAX_BUFFER_SIZE, AUIDO_MAX_FRAME_SIZE),
    m_vBuf(VIDEO_MAX_BUFFER_SIZE, VIDEO_MAX_FRAME_SIZE)
{
    m_synchState = DXR3_DEMUX_UNSYNCHED;
    m_demuxMode = DXR3_DEMUX_OFF_MODE;
    m_pAudioThread = new cDxr3AudioOutThread(m_aBuf);
    if (!m_pAudioThread) {
        esyslog("dxr3: fatal: unable to allocate memory for audio thread");
        exit(1);
    }

    m_pVideoThread = new cDxr3VideoOutThread(m_vBuf);
    if (!m_pVideoThread) {
        esyslog("dxr3: fatal: unable to allocate memory for video thread");
        exit(1);
    }
    m_pVideoThread->Start();
}

void cDxr3DemuxDevice::setAudio(iAudio *audio)
{
    m_pAudioThread->setAudio(audio);
    m_pAudioThread->Start();
}

// ==================================
// deconstr.
cDxr3DemuxDevice::~cDxr3DemuxDevice()
{
	delete m_pVideoThread;
	delete m_pAudioThread;
}

// ==================================
// stop demuxing process
void cDxr3DemuxDevice::Stop()
{
    m_dxr3Device->DisableVideo();
    m_pAudioThread->audio()->setEnabled(false);
    m_vBuf.Clear();
    m_aBuf.Clear();
    m_vBuf.WakeUp();
    m_aBuf.WakeUp();
    m_vBuf.WaitForReceiverStopped();
    m_aBuf.WaitForReceiverStopped();
    m_synchState = DXR3_DEMUX_UNSYNCHED;
    m_demuxMode = DXR3_DEMUX_OFF_MODE;

    m_dxr3Device->PlayBlackFrame();
}

// ==================================
void cDxr3DemuxDevice::Resync()
{
    m_dxr3Device->DisableVideo();
    m_pAudioThread->audio()->setEnabled(false);
    m_vBuf.Clear();
    m_aBuf.Clear();
    m_vBuf.WakeUp();
    m_aBuf.WakeUp();
    m_vBuf.WaitForReceiverStopped();
    m_aBuf.WaitForReceiverStopped();
    m_synchState = DXR3_DEMUX_UNSYNCHED;
    m_demuxMode = DXR3_DEMUX_OFF_MODE;
}

// ==================================
void cDxr3DemuxDevice::Clear()
{
    m_dxr3Device->DisableVideo();
    m_pAudioThread->audio()->setEnabled(false);
    m_vBuf.Clear();
    m_aBuf.Clear();
    m_vBuf.WakeUp();
    m_aBuf.WakeUp();
    m_vBuf.WaitForReceiverStopped();
    m_aBuf.WaitForReceiverStopped();
    m_synchState = DXR3_DEMUX_UNSYNCHED;
    m_demuxMode = DXR3_DEMUX_OFF_MODE;
}

// ==================================
void cDxr3DemuxDevice::SetReplayMode()
{
    if (m_demuxMode != DXR3_DEMUX_REPLAY_MODE)
    {
	if (m_demuxMode == DXR3_DEMUX_TRICK_MODE &&
	    m_trickState == DXR3_FREEZE)
	{
	    m_dxr3Device->SetPlayMode();
	    m_dxr3Device->SetSysClock(m_stopScr);
	    m_dxr3Device->EnableVideo();
            m_pAudioThread->audio()->setEnabled(true);
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
    m_pAudioThread->audio()->setEnabled(false);

    if (m_demuxMode == DXR3_DEMUX_TRICK_MODE &&
	m_trickState == DXR3_FREEZE)
    {
	m_stopScr = m_dxr3Device->GetSysClock();
	// m_dxr3Device->Pause();
	m_vBuf.Stop();
	m_aBuf.Stop();
    }
    else
    {
	m_vBuf.Clear();
	m_aBuf.Clear();
    }
}

// ==================================
void cDxr3DemuxDevice::StillPicture()
{
    m_vBuf.Clear();
    m_aBuf.Clear();
    m_demuxMode = DXR3_DEMUX_TRICK_MODE;
    m_trickState = DXR3_FREEZE;
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

    int scr = 0;
    int pcr = 0;

    scr = m_dxr3Device->GetSysClock();

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
    }

    cDxr3PesFrame *pesFrame = new cDxr3PesFrame();

    if (!pesFrame || !pesFrame->parse(buf, length)) {
        return -1;
    }

    if (pesFrame->GetPts() != lastPts) {
        pts = lastPts = pesFrame->GetPts();
    } else {
        pts = 0;
    }

    if (m_demuxMode != DXR3_DEMUX_VIDEO_ONLY_MODE) {

        if (pesFrame->GetPesDataType() != cDxr3PesFrame::PES_VIDEO_DATA) {

            if (m_synchState == DXR3_DEMUX_AUDIO_SYNCHED || m_synchState == DXR3_DEMUX_SYNCHED) {
                if (pts && m_synchState != DXR3_DEMUX_SYNCHED) {
                    m_synchState = DXR3_DEMUX_SYNCHED;
                    pcr = aPts - PRE_BUFFER_LENGTH;
                    m_dxr3Device->SetSysClock(pcr);
                    m_dxr3Device->SetPlayMode();
                    m_dxr3Device->EnableVideo();
                    m_pAudioThread->audio()->setEnabled(true);
                    m_vBuf.Start();
                    m_aBuf.Start();
                }
                while(!Poll(100)) {}

                if (pesFrame->GetPesDataType() == cDxr3PesFrame::PES_PRIVATE_DATA) {
                    if (bAc3Dts) {
                        m_aDecoder.DecodeAc3Dts(pesFrame, pts, m_aBuf);
                    } else {
                        m_aDecoder.DecodeLpcm(pesFrame, pts, m_aBuf);
                    }
                }
                m_aDecoder.Decode(pesFrame, pts, m_aBuf);

            } else {
                if (pts) {
                    aPts = pts;

                    if (pesFrame->GetPesDataType() == cDxr3PesFrame::PES_PRIVATE_DATA) {
                        if (bAc3Dts) {
                            m_aDecoder.DecodeAc3Dts(pesFrame, pts, m_aBuf);
                        } else {
                            m_aDecoder.DecodeLpcm(pesFrame, pts, m_aBuf);
                        }
                    }
                    m_aDecoder.Decode(pesFrame, pts, m_aBuf);

                    if (m_synchState == DXR3_DEMUX_VIDEO_SYNCHED) {
                        m_synchState = DXR3_DEMUX_SYNCHED;
                    } else {
                        m_synchState = DXR3_DEMUX_AUDIO_SYNCHED;
                    }
                    if (m_synchState == DXR3_DEMUX_SYNCHED) {
                        if (!vPts) {
                            vPts = aPts;
                        }
                        if (aPts < vPts) {
                            pcr = aPts - PRE_BUFFER_LENGTH;
                        } else {
                            pcr = vPts - PRE_BUFFER_LENGTH;
                        }
                        m_dxr3Device->SetSysClock(pcr);
                        m_dxr3Device->SetPlayMode();
                        m_dxr3Device->EnableVideo();
                        m_pAudioThread->audio()->setEnabled(true);
                        m_vBuf.Start();
                        m_aBuf.Start();
                    }
                }
            }
        } else {

            if (m_demuxMode == DXR3_DEMUX_TRICK_MODE) {
                switch (pesFrame->GetFrameType()) {
                case I_FRAME:
                    dsyslog("dxr3: demux: I-frame");
                    bPlaySuc = true;
                    //if (bPlayedFrame) return length;
                    bPlayedFrame = true;
                    m_dxr3Device->setDimension(pesFrame->GetHorizontalSize(), pesFrame->GetVerticalSize());
                    m_dxr3Device->PlayVideoFrame(pesFrame);
                    break;

                case UNKNOWN_FRAME:
                    dsyslog("dxr3: demux: unknown frame");
                    if (bPlaySuc) {
                        m_dxr3Device->PlayVideoFrame(pesFrame);
                    }
                    break;

                default:
                    dsyslog("dxr3: demux: default frame");
                    if (bPlaySuc) {
                        m_dxr3Device->PlayVideoFrame(pesFrame);
                    }

                    bPlaySuc = false;
                    break;
                }

            } else if (m_synchState == DXR3_DEMUX_VIDEO_SYNCHED ||  m_synchState == DXR3_DEMUX_SYNCHED) {
                m_dxr3Device->setDimension(pesFrame->GetHorizontalSize(), pesFrame->GetVerticalSize());
                while (!Poll(100)) {}
                cFixedLengthFrame* pTempFrame = m_vBuf.Push(pesFrame->GetPayload(), (int) (pesFrame->GetPayloadLength()), pts, ftVideo);

                // TODO: rework me
                //if (!pTempFrame) /* Push Timeout */
                //    throw (cDxr3PesFrame::PES_GENERAL_ERROR);

                pTempFrame->SetAspectRatio(pesFrame->GetAspectRatio());

                m_aBuf.WakeUp();

                if (m_vBuf.GetFillLevel() > 5 && m_synchState != DXR3_DEMUX_SYNCHED) {
                    m_synchState = DXR3_DEMUX_SYNCHED;
                    pcr = vPts - PRE_BUFFER_LENGTH;
                    m_dxr3Device->SetSysClock(pcr);
                    m_dxr3Device->SetPlayMode();
                    m_dxr3Device->EnableVideo();
                    m_pAudioThread->audio()->setEnabled(true);
                    m_vBuf.Start();
                    m_aBuf.Start();
                }
            } else {
                if (pesFrame->GetFrameType() == I_FRAME) {
                    vPts = pts;

                    m_dxr3Device->setDimension(pesFrame->GetHorizontalSize(), pesFrame->GetVerticalSize());
                    cFixedLengthFrame* pTempFrame = m_vBuf.Push(pesFrame->GetPayload(), (int) (pesFrame->GetPayloadLength()), pts, ftVideo);
                    // TODO: rework me
                    //if (!pTempFrame) /* Push Timeout */
                    //    throw (cDxr3PesFrame::PES_GENERAL_ERROR);

                    pTempFrame->SetAspectRatio(pesFrame->GetAspectRatio());

                    if (m_synchState == DXR3_DEMUX_AUDIO_SYNCHED) {
                        m_synchState = DXR3_DEMUX_SYNCHED;
                    } else {
                        m_synchState = DXR3_DEMUX_VIDEO_SYNCHED;
                    }
                    if (m_synchState == DXR3_DEMUX_SYNCHED) {
                        if (!vPts) {
                            vPts = aPts;
                        }
                        if (aPts < vPts) {
                            pcr = aPts - PRE_BUFFER_LENGTH;
                        } else {
                            pcr = vPts - PRE_BUFFER_LENGTH;
                        }
                        m_dxr3Device->SetSysClock(pcr);
                        m_dxr3Device->SetPlayMode();
                        m_dxr3Device->EnableVideo();
                        m_pAudioThread->audio()->setEnabled(true);
                        m_vBuf.Start();
                        m_aBuf.Start();
                    }
                }
            }

        }
    } else if (m_demuxMode == DXR3_DEMUX_VIDEO_ONLY_MODE) {
        m_dxr3Device->PlayVideoFrame(pesFrame);
    }

    delete pesFrame;
    return length;
}

// ==================================
int cDxr3DemuxDevice::DemuxAudioPes(const uint8_t* buf, int length)
{
    static int syncCounter = 0;

    m_demuxMode = DXR3_DEMUX_AUDIO_ONLY_MODE;
    m_aBuf.SetDemuxMode(DXR3_DEMUX_REPLAY_MODE);
    m_vBuf.SetDemuxMode(DXR3_DEMUX_REPLAY_MODE);

    cDxr3PesFrame *pesFrame = new cDxr3PesFrame();

    if (!pesFrame || !pesFrame->parse(buf, length)) {
        return -1;
    }

    if (pesFrame->GetPesDataType() == cDxr3PesFrame::PES_PRIVATE_DATA) {
        if (m_synchState != DXR3_DEMUX_AUDIO_SYNCHED &&
            syncCounter > 2) {
            m_synchState = DXR3_DEMUX_AUDIO_SYNCHED;
            m_dxr3Device->SetPlayMode();
            m_dxr3Device->EnableVideo();
            m_pAudioThread->audio()->setEnabled(true);
            m_vBuf.Start();
            m_aBuf.Start();
        }
        if (m_synchState != DXR3_DEMUX_AUDIO_SYNCHED &&
            syncCounter <= 2) {
            syncCounter++;
        }
        while (!m_aBuf.Poll(100)) {}
        m_aDecoder.DecodeLpcm(pesFrame, 0, m_aBuf);
    }

    delete pesFrame;

    return length;
}

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// tab-width: 4;
// indent-tabs-mode: nil
// End:
