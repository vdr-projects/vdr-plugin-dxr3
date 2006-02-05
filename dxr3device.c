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

#include "dxr3device.h"
#include "dxr3configdata.h"
#include "dxr3interface.h"
#include "dxr3tools.h"
#include "dxr3osd.h"

// ==================================
//! constructor
cDxr3Device::cDxr3Device() : m_DemuxDevice(cDxr3Interface::Instance())
{
    m_Offset = 0;
    m_strBuf.erase(m_strBuf.begin(), m_strBuf.end());
    m_spuDecoder = NULL;
    m_AC3Present = false;
    m_CalledBySet = false;
}

// ==================================
cDxr3Device::~cDxr3Device()
{
    if (m_spuDecoder)
    {
	delete m_spuDecoder;
    }
}

// ==================================
void cDxr3Device::MakePrimaryDevice(bool On)
{
    new cDxr3OsdProvider();
}

// replaying
// ==================================
//! does we have an mpeg2 devocer?
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
    if (PlayMode == pmExtern_THIS_SHOULD_BE_AVOIDED)
    {
	Tools::WriteInfoToOsd(tr("DXR3: releasing devices"));
	cDxr3Interface::Instance().ExternalReleaseDevices();
    }
    else
    {
	cDxr3Interface::Instance().ExternalReopenDevices();
    }

    // should this relay be here?
    m_Offset = 0;
    m_AC3Present = false;
    m_strBuf.erase(m_strBuf.begin(), m_strBuf.end());

    if (PlayMode == pmAudioOnlyBlack)
    {
	m_PlayMode = pmAudioOnly;
    }
    else
    {
	m_PlayMode = PlayMode;
    }

    if (m_PlayMode == pmAudioVideo)
    {
	m_DemuxDevice.SetReplayMode();
    }

    if (m_PlayMode == pmNone)
    {
	m_DemuxDevice.Stop();
    }

    if (cDxr3ConfigData::Instance().GetUseDigitalOut())
    {
	if (cDxr3ConfigData::Instance().GetAc3OutPut() && m_CalledBySet)
	{
	    isyslog("dxr3: Setting AC3 audio mode");
	    cDxr3Interface::Instance().SetAudioDigitalAC3(); // !!! FIXME
	}
	else
	{
	    isyslog("dxr3: Setting digital PCM audio mode");
	    cDxr3Interface::Instance().SetAudioDigitalPCM();
	    cDxr3ConfigData::Instance().SetAc3OutPut(0);
	}
    }
    else
    {
	isyslog("dxr3: Setting analog audio mode");
	cDxr3Interface::Instance().SetAudioAnalog();
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

    /*
	6 ... 1x vowärts
	3 ... 2x vowärts
	1 ... 2x vowärts

	6 ... 1x rückwärts
	3 ... 2x rückwärts
	1 ... 3x rückwärts

	8 ... 1x vorwörts, wenn Pause gedrückt
    */

    /*
#define EM8300_PLAYMODE_PAUSED          1
#define EM8300_PLAYMODE_SLOWFORWARDS    2
#define EM8300_PLAYMODE_SLOWBACKWARDS   3
#define EM8300_PLAYMODE_SINGLESTEP      4
    */
    /*
    if (Speed == 8)
    {
	cDxr3Interface::Instance().SingleStep();
    }
    else
    {
	m_DemuxDevice.SetTrickMode(DXR3_FAST);
    }
    */
}

// ==================================
//! clear our demux buffer
void cDxr3Device::Clear()
{
    m_DemuxDevice.Clear();
    m_Offset = 0;
    m_strBuf.erase(m_strBuf.begin(), m_strBuf.end());
    cDevice::Clear();
}

// ==================================
//! play a recording
void cDxr3Device::Play()
{
    m_DemuxDevice.SetReplayMode();
    m_Offset = 0;
    ///< free buffer
    m_strBuf.erase(m_strBuf.begin(), m_strBuf.end());
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
    m_DemuxDevice.SetTrickMode(DXR3_FAST);
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
	 m_DemuxDevice.GetTrickState() == DXR3_FREEZE) ||
	cDxr3Interface::Instance().IsExternalReleased())
    {
#if VDRVERSNUM >= 10314
	cCondWait::SleepMs(TimeoutMs);
#else
	usleep(TimeoutMs * 1000);
#endif
	return false;
    }
    return m_DemuxDevice.Poll(TimeoutMs); // Poller.Poll(TimeoutMs);
}

// ==================================
//! actually plays the given data block as video
int cDxr3Device::PlayVideo(const uchar *Data, int Length)
{
    int retLength = 0;
    int origLength = Length;

    if ((m_DemuxDevice.GetDemuxMode() == DXR3_DEMUX_TRICK_MODE &&
	 m_DemuxDevice.GetTrickState() == DXR3_FREEZE) ||
	cDxr3Interface::Instance().IsExternalReleased())
    {
	// Why is here so a huge time waster?
	//usleep(1000000);
	return -1;
    }

    if (m_strBuf.length())
    {
	m_strBuf.append((const char*)Data, Length);

	if (m_PlayMode == pmAudioOnly)
	{
	    retLength = m_DemuxDevice.DemuxAudioPes((const uint8_t*)m_strBuf.data(), m_strBuf.length());
	}
	else
	{
	    retLength = m_DemuxDevice.DemuxPes((const uint8_t*)m_strBuf.data(), m_strBuf.length());
	}
    }
    else
    {
	if (m_PlayMode == pmAudioOnly)
	{
	    retLength = m_DemuxDevice.DemuxAudioPes((const uint8_t*)Data, Length);
	}
	else
	{
	    retLength = m_DemuxDevice.DemuxPes((const uint8_t*)Data, Length);
	}
    }

    Length -= retLength;

    if (m_strBuf.length())
    {
	m_strBuf.erase(m_strBuf.length() - retLength, retLength);
    }
    else
    {
	if (Length)
	{
	    m_strBuf.append((const char*)(Data + retLength), Length);
	}
    }

    return origLength;
}

// ==================================
// plays additional audio streams, like Dolby Digital
#if VDRVERSNUM >= 10318
#if VDRVERSNUM >= 10342
int cDxr3Device::PlayAudio(const uchar *Data, int Length, uchar Id)
#else
int cDxr3Device::PlayAudio(const uchar *Data, int Length)
#endif
#else
void cDxr3Device::PlayAudio(const uchar *Data, int Length)
#endif
{
    int retLength = 0;
#if VDRVERSNUM >= 10318
    int origLength = Length;
#endif

    m_AC3Present = true;

    if ((m_DemuxDevice.GetDemuxMode() == DXR3_DEMUX_TRICK_MODE &&
	 m_DemuxDevice.GetTrickState() == DXR3_FREEZE) ||
	cDxr3Interface::Instance().IsExternalReleased())
    {
	//usleep(1000000);
#if VDRVERSNUM >= 10318
	return 0;
#else
	return;
#endif
    }

    if (m_strBuf.length())
    {
	m_strBuf.append((const char*)Data, Length);
	if (m_PlayMode == pmAudioOnly)
	{
	    retLength = m_DemuxDevice.DemuxAudioPes((const uint8_t*)m_strBuf.data(), m_strBuf.length());
	} else {
	    retLength = m_DemuxDevice.DemuxPes((const uint8_t*)m_strBuf.data(), m_strBuf.length(), true);
	}
    }
    else
    {
	if (m_PlayMode == pmAudioOnly)
	{
	    retLength = m_DemuxDevice.DemuxAudioPes((const uint8_t*) Data, Length);
	} else {
	    retLength = m_DemuxDevice.DemuxPes((const uint8_t*)Data, Length, true);
	}
    }

    Length -= retLength;

    if (m_strBuf.length())
    {
	m_strBuf.erase(m_strBuf.length() - retLength, retLength);
    }
    else
    {
	if (Length)
	{
	    m_strBuf.append((const char*)(Data + retLength), Length);
	}
    }

#if VDRVERSNUM >= 10318
    return origLength;
#endif
}

// addition functions

// ==================================
void cDxr3Device::SetVideoFormat(bool VideoFormat16_9)
{
    // Do we need this function?
}

// ==================================
//! sets volume for audio output
void cDxr3Device::SetVolumeDevice(int Volume)
{
    cDxr3Interface::Instance().SetVolume(Volume);
}

// ==================================
//! sets audio channel for audio output (stero, mono left, mono right)
void cDxr3Device::SetAudioChannelDevice(int AudioChannel)
{
    cDxr3Interface::Instance().SetAudioChannel(AudioChannel);
}
int cDxr3Device::GetAudioChannelDevice(void)
{
    return cDxr3Interface::Instance().GetAudioChannel();
}

// ==================================
// get spudecoder
cSpuDecoder *cDxr3Device::GetSpuDecoder(void)
{
    if (!m_spuDecoder && IsPrimaryDevice())
    {
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
