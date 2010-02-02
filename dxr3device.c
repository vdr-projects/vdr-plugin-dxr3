/*
 * dxr3device.c
 *
 * Copyright (C) 2002-2004 Kai Möller
 * Copyright (C) 2004 Christian Gmeiner
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

#include "dxr3device.h"
#include "dxr3interface.h"
#include "dxr3tools.h"
#include "dxr3osd.h"
#include "dxr3audio-oss.h"
#include "dxr3audio-alsa.h"
#include "dxr3pesframe.h"

// ==================================
//! constructor
cDxr3Device::cDxr3Device() : pluginOn(true)
{
    m_spuDecoder = NULL;

    // TODO: this will be later the place,
    //       where we will decide what kind of
    //       audio output system we will use.
    audioOut = new cAudioOss();
    //audioOut = new cAudioAlsa();
    audioOut->openDevice();
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
#if VDRVERSNUM >= 10711
    cDevice::MakePrimaryDevice(On);
#endif

    if (On) {
        new cDxr3OsdProvider();
    }
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
    dsyslog("[dxr3-device] setting playmode %d", PlayMode);

    switch (PlayMode) {
    case pmNone:
        audioOut->setEnabled(false);
        break;

    case pmAudioVideo:
        audioOut->setEnabled(true);
        break;

    default:
        dsyslog("[dxr3-device] playmode not supported");
        return false;
    }

    return true;
}

// ==================================
int64_t cDxr3Device::GetSTC()
{
    return cDxr3Interface::instance()->GetPts();
}

// ==================================
void cDxr3Device::TrickSpeed(int Speed)
{
    dsyslog("dxr3: device: tricspeed: %d", Speed);
}

// ==================================
//! clear our demux buffer
void cDxr3Device::Clear()
{
    cDevice::Clear();
}

// ==================================
//! play a recording
void cDxr3Device::Play()
{
}

// ==================================
//! puts the device into "freeze frame" mode
void cDxr3Device::Freeze()
{
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
    // clear used buffers of output threads

    // we need to check if Data points to a pes
    // frame or to non-pes data. This could be the
    // case for the radio-Plugin, which points to an
    // elementary stream.
    cDxr3PesFrame frame;

    if (frame.parse(Data, Length)) {
        // TODO
    } else {
        // TODO
    }
}

// ==================================
bool cDxr3Device::Poll(cPoller &Poller, int TimeoutMs)
{
    /*
    if ((m_DemuxDevice.GetDemuxMode() == DXR3_DEMUX_TRICK_MODE &&
	 m_DemuxDevice.GetTrickState() == DXR3_FREEZE)) {
        cCondWait::SleepMs(TimeoutMs);
        return false;
    }
    return m_DemuxDevice.Poll(TimeoutMs); // Poller.Poll(TimeoutMs);
    */
    return true;
}

// ==================================
//! actually plays the given data block as video
int cDxr3Device::PlayVideo(const uchar *Data, int Length)
{
    return Length;
}

// ==================================
// plays additional audio streams, like Dolby Digital
int cDxr3Device::PlayAudio(const uchar *Data, int Length, uchar Id)
{
    return Length;
}

#if VDRVERSNUM >= 10710
void cDxr3Device::GetVideoSize(int &Width, int &Height, double &VideoAspect)
{
    cDxr3Interface::instance()->dimension((uint32_t&)Width, (uint32_t&)Height);
    uint32_t aspect = cDxr3Interface::instance()->GetAspectRatio();

    if (aspect == EM8300_ASPECTRATIO_4_3) {
        VideoAspect = 4.0 / 3.0;
    } else {
        VideoAspect = 16.0 / 9.0;
    }
}
#endif

// additional functions

// ==================================
void cDxr3Device::SetVideoFormat(bool VideoFormat16_9)
{
    cDxr3Interface::instance()->SetAspectRatio(
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

void cDxr3Device::SetDigitalAudioDevice(bool on)
{
    audioOut->setDigitalAudio(on);
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

void cDxr3Device::turnPlugin(bool on)
{
    // we have support for a very unique thing.
    // it is possible to release the current
    // used dxr3 device and make it so usable
    // for other programs.
    // and the unique thing is that VDR can run
    // without interrupting it.


    if (on) {

        // check if we already at 'on' state
        if (pluginOn) {
            return;
        }

        // get full control over device
        cDxr3Interface::instance()->ClaimDevices();
        audioOut->openDevice();

        // enable pes packet processing
        pluginOn = true;

    } else {

        // check if we already at 'off' state
        if (!pluginOn) {
            return;
        }

        // clear buffer

        // release device and give control to somebody else
        Tools::WriteInfoToOsd(tr("DXR3: releasing devices"));
        cDxr3Interface::instance()->ReleaseDevices();
        audioOut->releaseDevice();

        // disable pes packet processing
        pluginOn = false;
    }
}

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// tab-width: 4;
// indent-tabs-mode: nil
// End:
