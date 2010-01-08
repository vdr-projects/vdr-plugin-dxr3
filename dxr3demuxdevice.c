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
    aBuf(AUDIO_MAX_BUFFER_SIZE, AUIDO_MAX_FRAME_SIZE),
    vBuf(VIDEO_MAX_BUFFER_SIZE, VIDEO_MAX_FRAME_SIZE)
{
    synchState = DXR3_DEMUX_UNSYNCHED;
    demuxMode = DXR3_DEMUX_OFF_MODE;
    audioThread = new cDxr3AudioOutThread(aBuf);
    if (!audioThread) {
        esyslog("dxr3: fatal: unable to allocate memory for audio thread");
        exit(1);
    }

    videoThread = new cDxr3VideoOutThread(vBuf);
    if (!videoThread) {
        esyslog("dxr3: fatal: unable to allocate memory for video thread");
        exit(1);
    }
    videoThread->Start();
}

void cDxr3DemuxDevice::setAudio(iAudio *audio)
{
    audioThread->setAudio(audio);
    audioThread->Start();
}

// ==================================
// deconstr.
cDxr3DemuxDevice::~cDxr3DemuxDevice()
{
    delete videoThread;
    delete audioThread;
}

// ==================================
// stop demuxing process
void cDxr3DemuxDevice::Stop()
{
    cDxr3Interface::instance()->DisableVideo();
    audioThread->audio()->setEnabled(false);
    vBuf.Clear();
    aBuf.Clear();
    vBuf.WakeUp();
    aBuf.WakeUp();
    vBuf.WaitForReceiverStopped();
    aBuf.WaitForReceiverStopped();
    synchState = DXR3_DEMUX_UNSYNCHED;
    demuxMode = DXR3_DEMUX_OFF_MODE;

    cDxr3Interface::instance()->PlayBlackFrame();
}

// ==================================
void cDxr3DemuxDevice::Resync()
{
    cDxr3Interface::instance()->DisableVideo();
    audioThread->audio()->setEnabled(false);
    vBuf.Clear();
    aBuf.Clear();
    vBuf.WakeUp();
    aBuf.WakeUp();
    vBuf.WaitForReceiverStopped();
    aBuf.WaitForReceiverStopped();
    synchState = DXR3_DEMUX_UNSYNCHED;
    demuxMode = DXR3_DEMUX_OFF_MODE;
}

// ==================================
void cDxr3DemuxDevice::Clear()
{
    cDxr3Interface::instance()->DisableVideo();
    audioThread->audio()->setEnabled(false);
    vBuf.Clear();
    aBuf.Clear();
    vBuf.WakeUp();
    aBuf.WakeUp();
    vBuf.WaitForReceiverStopped();
    aBuf.WaitForReceiverStopped();
    synchState = DXR3_DEMUX_UNSYNCHED;
    demuxMode = DXR3_DEMUX_OFF_MODE;
}

// ==================================
void cDxr3DemuxDevice::SetReplayMode()
{
    if (demuxMode != DXR3_DEMUX_REPLAY_MODE)
    {
        if (demuxMode == DXR3_DEMUX_TRICK_MODE && trickState == DXR3_FREEZE) {
            cDxr3Interface::instance()->SetPlayMode();
            cDxr3Interface::instance()->SetSysClock(stopScr);
            cDxr3Interface::instance()->EnableVideo();
            audioThread->audio()->setEnabled(true);
            vBuf.Start();
            aBuf.Start();
            vBuf.WakeUp();
            aBuf.WakeUp();
        } else {
            synchState = DXR3_DEMUX_UNSYNCHED;
            aBuf.SetDemuxMode(DXR3_DEMUX_REPLAY_MODE);
            vBuf.SetDemuxMode(DXR3_DEMUX_REPLAY_MODE);
	}
    }
    demuxMode = DXR3_DEMUX_REPLAY_MODE;
}

// ==================================
void cDxr3DemuxDevice::SetTrickMode(eDxr3TrickState trickState, int Speed)
{
    demuxMode = DXR3_DEMUX_TRICK_MODE;
    trickState = trickState;
    audioThread->audio()->setEnabled(false);

    if (demuxMode == DXR3_DEMUX_TRICK_MODE && trickState == DXR3_FREEZE) {
        stopScr = cDxr3Interface::instance()->GetSysClock();
        // cDxr3Interface::instance()->Pause();
        vBuf.Stop();
        aBuf.Stop();
    } else {
        vBuf.Clear();
        aBuf.Clear();
    }
}

// ==================================
void cDxr3DemuxDevice::StillPicture()
{
    vBuf.Clear();
    aBuf.Clear();
    demuxMode = DXR3_DEMUX_TRICK_MODE;
    trickState = DXR3_FREEZE;
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

    scr = cDxr3Interface::instance()->GetSysClock();

    if (audioThread->NeedResync() || videoThread->NeedResync()) {
        Resync();
        if (demuxMode == DXR3_DEMUX_REPLAY_MODE) {
            SetReplayMode();
        }
        aBuf.Clear();
        vBuf.Clear();
        audioThread->ClearResyncRequest();
        videoThread->ClearResyncRequest();
        aDecoder.Reset();
        lastPts = 0;
        aPts = 0;
        vPts = 0;
        bPlaySuc = false;
    }

    if (demuxMode == DXR3_DEMUX_OFF_MODE) {
        demuxMode = DXR3_DEMUX_TV_MODE;
        synchState = DXR3_DEMUX_UNSYNCHED;

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

    if (demuxMode != DXR3_DEMUX_VIDEO_ONLY_MODE) {

        if (pesFrame->GetPesDataType() != cDxr3PesFrame::PES_VIDEO_DATA) {

            if (synchState == DXR3_DEMUX_AUDIO_SYNCHED || synchState == DXR3_DEMUX_SYNCHED) {
                if (pts && synchState != DXR3_DEMUX_SYNCHED) {
                    synchState = DXR3_DEMUX_SYNCHED;
                    pcr = aPts - PRE_BUFFER_LENGTH;
                    cDxr3Interface::instance()->SetSysClock(pcr);
                    cDxr3Interface::instance()->SetPlayMode();
                    cDxr3Interface::instance()->EnableVideo();
                    audioThread->audio()->setEnabled(true);
                    vBuf.Start();
                    aBuf.Start();
                }
                while(!Poll(100)) {}

                if (pesFrame->GetPesDataType() == cDxr3PesFrame::PES_PRIVATE_DATA) {
                    if (bAc3Dts) {
                        aDecoder.DecodeAc3Dts(pesFrame, pts, aBuf);
                    } else {
                        aDecoder.DecodeLpcm(pesFrame, pts, aBuf);
                    }
                }
                aDecoder.Decode(pesFrame, pts, aBuf);

            } else {
                if (pts) {
                    aPts = pts;

                    if (pesFrame->GetPesDataType() == cDxr3PesFrame::PES_PRIVATE_DATA) {
                        if (bAc3Dts) {
                            aDecoder.DecodeAc3Dts(pesFrame, pts, aBuf);
                        } else {
                            aDecoder.DecodeLpcm(pesFrame, pts, aBuf);
                        }
                    }
                    aDecoder.Decode(pesFrame, pts, aBuf);

                    if (synchState == DXR3_DEMUX_VIDEO_SYNCHED) {
                        synchState = DXR3_DEMUX_SYNCHED;
                    } else {
                        synchState = DXR3_DEMUX_AUDIO_SYNCHED;
                    }
                    if (synchState == DXR3_DEMUX_SYNCHED) {
                        if (!vPts) {
                            vPts = aPts;
                        }
                        if (aPts < vPts) {
                            pcr = aPts - PRE_BUFFER_LENGTH;
                        } else {
                            pcr = vPts - PRE_BUFFER_LENGTH;
                        }
                        cDxr3Interface::instance()->SetSysClock(pcr);
                        cDxr3Interface::instance()->SetPlayMode();
                        cDxr3Interface::instance()->EnableVideo();
                        audioThread->audio()->setEnabled(true);
                        vBuf.Start();
                        aBuf.Start();
                    }
                }
            }
        } else {

            if (demuxMode == DXR3_DEMUX_TRICK_MODE) {
                switch (pesFrame->GetFrameType()) {
                case I_FRAME:
                    dsyslog("dxr3: demux: I-frame");
                    bPlaySuc = true;
                    //if (bPlayedFrame) return length;
                    bPlayedFrame = true;
                    cDxr3Interface::instance()->setDimension(pesFrame->GetHorizontalSize(), pesFrame->GetVerticalSize());
                    cDxr3Interface::instance()->PlayVideoFrame(pesFrame);
                    break;

                case UNKNOWN_FRAME:
                    dsyslog("dxr3: demux: unknown frame");
                    if (bPlaySuc) {
                        cDxr3Interface::instance()->PlayVideoFrame(pesFrame);
                    }
                    break;

                default:
                    dsyslog("dxr3: demux: default frame");
                    if (bPlaySuc) {
                        cDxr3Interface::instance()->PlayVideoFrame(pesFrame);
                    }

                    bPlaySuc = false;
                    break;
                }

            } else if (synchState == DXR3_DEMUX_VIDEO_SYNCHED ||  synchState == DXR3_DEMUX_SYNCHED) {
                cDxr3Interface::instance()->setDimension(pesFrame->GetHorizontalSize(), pesFrame->GetVerticalSize());
                while (!Poll(100)) {}
                cFixedLengthFrame* pTempFrame = vBuf.Push(pesFrame->GetPayload(), (int) (pesFrame->GetPayloadLength()), pts, ftVideo);

                // TODO: rework me
                //if (!pTempFrame) /* Push Timeout */
                //    throw (cDxr3PesFrame::PES_GENERAL_ERROR);

                pTempFrame->SetAspectRatio(pesFrame->GetAspectRatio());

                aBuf.WakeUp();

                if (vBuf.GetFillLevel() > 5 && synchState != DXR3_DEMUX_SYNCHED) {
                    synchState = DXR3_DEMUX_SYNCHED;
                    pcr = vPts - PRE_BUFFER_LENGTH;
                    cDxr3Interface::instance()->SetSysClock(pcr);
                    cDxr3Interface::instance()->SetPlayMode();
                    cDxr3Interface::instance()->EnableVideo();
                    audioThread->audio()->setEnabled(true);
                    vBuf.Start();
                    aBuf.Start();
                }
            } else {
                if (pesFrame->GetFrameType() == I_FRAME) {
                    vPts = pts;

                    cDxr3Interface::instance()->setDimension(pesFrame->GetHorizontalSize(), pesFrame->GetVerticalSize());
                    cFixedLengthFrame* pTempFrame = vBuf.Push(pesFrame->GetPayload(), (int) (pesFrame->GetPayloadLength()), pts, ftVideo);
                    // TODO: rework me
                    //if (!pTempFrame) /* Push Timeout */
                    //    throw (cDxr3PesFrame::PES_GENERAL_ERROR);

                    pTempFrame->SetAspectRatio(pesFrame->GetAspectRatio());

                    if (synchState == DXR3_DEMUX_AUDIO_SYNCHED) {
                        synchState = DXR3_DEMUX_SYNCHED;
                    } else {
                        synchState = DXR3_DEMUX_VIDEO_SYNCHED;
                    }
                    if (synchState == DXR3_DEMUX_SYNCHED) {
                        if (!vPts) {
                            vPts = aPts;
                        }
                        if (aPts < vPts) {
                            pcr = aPts - PRE_BUFFER_LENGTH;
                        } else {
                            pcr = vPts - PRE_BUFFER_LENGTH;
                        }
                        cDxr3Interface::instance()->SetSysClock(pcr);
                        cDxr3Interface::instance()->SetPlayMode();
                        cDxr3Interface::instance()->EnableVideo();
                        audioThread->audio()->setEnabled(true);
                        vBuf.Start();
                        aBuf.Start();
                    }
                }
            }

        }
    } else if (demuxMode == DXR3_DEMUX_VIDEO_ONLY_MODE) {
        cDxr3Interface::instance()->PlayVideoFrame(pesFrame);
    }

    delete pesFrame;
    return length;
}

// ==================================
int cDxr3DemuxDevice::DemuxAudioPes(const uint8_t* buf, int length)
{
    static int syncCounter = 0;

    demuxMode = DXR3_DEMUX_AUDIO_ONLY_MODE;
    aBuf.SetDemuxMode(DXR3_DEMUX_REPLAY_MODE);
    vBuf.SetDemuxMode(DXR3_DEMUX_REPLAY_MODE);

    cDxr3PesFrame *pesFrame = new cDxr3PesFrame();

    if (!pesFrame || !pesFrame->parse(buf, length)) {
        return -1;
    }

    if (pesFrame->GetPesDataType() == cDxr3PesFrame::PES_PRIVATE_DATA) {
        if (synchState != DXR3_DEMUX_AUDIO_SYNCHED &&
            syncCounter > 2) {
            synchState = DXR3_DEMUX_AUDIO_SYNCHED;
            cDxr3Interface::instance()->SetPlayMode();
            cDxr3Interface::instance()->EnableVideo();
            audioThread->audio()->setEnabled(true);
            vBuf.Start();
            aBuf.Start();
        }
        if (synchState != DXR3_DEMUX_AUDIO_SYNCHED &&
            syncCounter <= 2) {
            syncCounter++;
        }
        while (!aBuf.Poll(100)) {}
        aDecoder.DecodeLpcm(pesFrame, 0, aBuf);
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
