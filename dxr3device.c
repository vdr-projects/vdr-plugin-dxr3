/*
 * dxr3device.c
 *
 * Copyright (C) 2002-2004 Kai Möller
 * Copyright (C) 2004 Christian Gmeiner
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

#include <linux/em8300.h>

#include "dxr3device.h"
#include "dxr3configdata.h"
#include "dxr3interface.h"
#include "dxr3tools.h"
#include "dxr3osd.h"
#include "dxr3audio-oss.h"
#include "dxr3audio-alsa.h"

// ==================================
//! constructor
cDxr3Device::cDxr3Device() : m_DemuxDevice(cDxr3Interface::Instance())
{
    m_spuDecoder = NULL;

    // TODO: this will be later the place,
    //       where we will decide what kind of
    //       audio output system we will use.
    audioOut = new cAudioOss();
    //audioOut = new cAudioAlsa();
    audioOut->openDevice();

    m_DemuxDevice.setAudio(audioOut);
}

// ==================================
cDxr3Device::~cDxr3Device()
{
    audioOut->releaseDevice();
    delete audioOut;

    if (m_spuDecoder)
        delete m_spuDecoder;
}

// ==================================
void cDxr3Device::MakePrimaryDevice(bool On)
{
    new cDxr3OsdProvider();
}

// replaying
// ==================================
//! do we have an mpeg2 decoder?
bool cDxr3Device::HasDecoder() const
{
    // sure we have one ;)
    return true;
}

// ==================================
//! can we replay vdr recordings?
bool cDxr3Device::CanReplay() const
{
    // also sure...
    return true;
}

// ==================================
bool cDxr3Device::SetPlayMode(ePlayMode PlayMode)
{
    if (PlayMode != pmExtern_THIS_SHOULD_BE_AVOIDED) {
        cDxr3Interface::Instance().ClaimDevices();
        audioOut->openDevice();
    }

    dsyslog("setting playmode %d", PlayMode);

    switch (PlayMode) {
    case pmExtern_THIS_SHOULD_BE_AVOIDED:
        Tools::WriteInfoToOsd(tr("DXR3: releasing devices"));
        cDxr3Interface::Instance().ReleaseDevices();
        audioOut->releaseDevice();
        break;

    case pmNone:
        //m_DemuxDevice.Stop();
        break;

    case pmAudioVideo:
    case pmAudioOnly:
    case pmAudioOnlyBlack:
    case pmVideoOnly:

        m_PlayMode = PlayMode;
    }

    return true;
}

// ==================================
int64_t cDxr3Device::GetSTC()
{
    return cDxr3Interface::Instance().GetPts();
}

// ==================================
void cDxr3Device::TrickSpeed(int Speed)
{
    dsyslog("dxr3: device: tricspeed: %d", Speed);

    m_DemuxDevice.SetTrickMode(DXR3_FAST, Speed);
}

// ==================================
//! clear our demux buffer
void cDxr3Device::Clear()
{
    m_DemuxDevice.Clear();
    cDevice::Clear();
}

// ==================================
//! play a recording
void cDxr3Device::Play()
{
    m_DemuxDevice.SetReplayMode();
}

// ==================================
//! puts the device into "freeze frame" mode
void cDxr3Device::Freeze()
{
    m_DemuxDevice.SetTrickMode(DXR3_FREEZE);
}

// ==================================
void cDxr3Device::Mute()
{
    audioOut->mute();
    cDevice::Mute();
}

// ==================================
//! displays the given I-frame as a still picture.
void cDxr3Device::StillPicture(const uchar *Data, int Length)
{
    m_DemuxDevice.StillPicture(Data, Length);
}

// ==================================
bool cDxr3Device::Poll(cPoller &Poller, int TimeoutMs)
{
    if ((m_DemuxDevice.GetDemuxMode() == DXR3_DEMUX_TRICK_MODE &&
	 m_DemuxDevice.GetTrickState() == DXR3_FREEZE)) {
        cCondWait::SleepMs(TimeoutMs);
        return false;
    }
    return m_DemuxDevice.Poll(TimeoutMs); // Poller.Poll(TimeoutMs);
}

// ==================================
//! actually plays the given data block as video
int cDxr3Device::PlayVideo(const uchar *Data, int Length)
{
    return m_DemuxDevice.DemuxPes(Data, Length);
}

// ==================================
// plays additional audio streams, like Dolby Digital
int cDxr3Device::PlayAudio(const uchar *Data, int Length, uchar Id)
{
    bool isAc3 = ((Id & 0xF0) == 0x80) || Id == 0xbd;

    if (isAc3 && !audioOut->isAudioModeAC3())
        audioOut->setAudioMode(iAudio::Ac3);

    if (m_PlayMode == pmAudioOnly) {
        return m_DemuxDevice.DemuxAudioPes(Data, Length);
    } else {
        return m_DemuxDevice.DemuxPes(Data, Length);
    }
}

// additional functions

// ==================================
void cDxr3Device::SetVideoFormat(bool VideoFormat16_9)
{
    cDxr3Interface::Instance().SetAspectRatio(
	VideoFormat16_9 ? EM8300_ASPECTRATIO_16_9 : EM8300_ASPECTRATIO_4_3);
}

// ==================================
//! sets volume for audio output
void cDxr3Device::SetVolumeDevice(int Volume)
{
    audioOut->setVolume(Volume);
}

// ==================================
//! sets audio channel for audio output (stereo, mono left, mono right)
void cDxr3Device::SetAudioChannelDevice(int AudioChannel)
{
    audioOut->setAudioChannel(AudioChannel);
}

int cDxr3Device::GetAudioChannelDevice()
{
    return audioOut->getAudioChannel();
}

// ==================================
// get spudecoder
cSpuDecoder *cDxr3Device::GetSpuDecoder()
{
    if (!m_spuDecoder && IsPrimaryDevice()) {
        m_spuDecoder = new cDxr3SpuDecoder();
    }
    return m_spuDecoder;
}

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
