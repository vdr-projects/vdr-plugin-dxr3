/*
 * dxr3devide.c
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include "dxr3device.h"
#include "dxr3configdata.h"
#include "dxr3interface.h"
#include "dxr3tools.h"
#include "dxr3log.h"
#include "dxr3osd.h"

extern "C" 
{
	#include <jpeglib.h>
}

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
#if VDRVERSNUM >= 10307
    new cDxr3OsdProvider();
#endif
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
	if (cDxr3ConfigData::Instance().GetDebug())
	{
		switch (PlayMode) 
		{
		case pmNone: 
			cLog::Instance() << "cDxr3Device::SetPlayMode audio/video from decoder\n"; 
			break;
			
		case pmAudioVideo: 
			cLog::Instance() << "cDxr3Device::SetPlayMode audio/video from player\n"; 
			break;
			
		case pmAudioOnly: 
			cLog::Instance() << "cDxr3Device::SetPlayMode audio only from player, video from decoder\n"; 
			break;
			
		case pmAudioOnlyBlack: 
			cLog::Instance() << "cDxr3Device::SetPlayMode audio only from player, no video (black screen)\n"; 
			break;
			
		case pmExtern_THIS_SHOULD_BE_AVOIDED: 
			cLog::Instance() << "cDxr3Device::SetPlayMode this should be avoided\n"; 
			break;

		#if VDRVERSNUM >= 10307
		case pmVideoOnly:
			cLog::Instance() << "cDxr3Device::SetPlayMode video only from player, audio from decoder\n"; 
			break;
		#endif
		}
	}

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
	
	cLog::Instance() << "Setting audio mode...";

    if (cDxr3ConfigData::Instance().GetUseDigitalOut()) 
	{
        if (cDxr3ConfigData::Instance().GetAc3OutPut() && m_CalledBySet) 
		{
            cDxr3Interface::Instance().SetAudioDigitalAC3(); // !!! FIXME
			cLog::Instance() << "ac3\n";
        } 
		else 
		{    
			cDxr3Interface::Instance().SetAudioDigitalPCM();
			cDxr3ConfigData::Instance().SetAc3OutPut(0);
			cLog::Instance() << "digital pcm\n";
        }
    } 
	else 
	{
		cDxr3Interface::Instance().SetAudioAnalog();
		cLog::Instance() << "analog\n";
    }

    return true;
}

// ==================================
int64_t cDxr3Device::GetSTC()
{
	return cDxr3Interface::Instance().GetSysClock();
}

// ==================================
void cDxr3Device::TrickSpeed(int Speed)
{
	if (cDxr3ConfigData::Instance().GetDebug())
	{
		cLog::Instance() << "cDxr3Device::TrickSpeed(int Speed): " << Speed << "\n";
	}


	m_DemuxDevice.SetTrickMode(DXR3_FAST, Speed);


/*
	switch (Speed)
	{
	case 6:
		cLog::Instance() << "Trickspeed: 1x vorwärts\n";
		break;

	case 3:
		cLog::Instance() << "Trickspeed: 2x vorwärts\n";
		break;

	case 1:
		cLog::Instance() << "Trickspeed: 3x vorwärts\n";
		break;
	};
*/
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
	}*/
}

// ==================================
//! clear our demux buffer
void cDxr3Device::Clear()
{
    m_DemuxDevice.Clear();
    m_Offset = 0;
    m_strBuf.erase(m_strBuf.begin(), m_strBuf.end());
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
	return m_DemuxDevice.Poll(TimeoutMs); // Poller.Poll(TimeoutMs);
}

// ==================================
//! actually plays the given data block as video
int cDxr3Device::PlayVideo(const uchar *Data, int Length)
{
    int retLength = 0;
    int origLength = Length;
	
    if ((m_DemuxDevice.GetDemuxMode() == DXR3_DEMUX_TRICK_MODE &&
		m_DemuxDevice.GetTrickState() == DXR3_FREEZE) || cDxr3Interface::Instance().IsExternalReleased()) 
	{
		// Why is here so a huge time waster?
        //usleep(1000000);
        return 0;
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
	int cDxr3Device::PlayAudio(const uchar *Data, int Length)
#else
	void cDxr3Device::PlayAudio(const uchar *Data, int Length)
#endif
{
    int retLength = 0;
#if VDRVERSNUM >= 10318
		int origLength = Length;
#endif    

#if VDRVERSNUM < 10307
    if (!m_AC3Present) 
	{
        Interface->Write(Interface->Width() / 2, 0, "AC3", clrRed);
    }	
#endif

    m_AC3Present = true;
	
    if ((m_DemuxDevice.GetDemuxMode() == DXR3_DEMUX_TRICK_MODE &&
		m_DemuxDevice.GetTrickState() == DXR3_FREEZE) || cDxr3Interface::Instance().IsExternalReleased()) 
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
        retLength = m_DemuxDevice.DemuxPes((const uint8_t*)m_strBuf.data(), m_strBuf.length(), true);
    } 
	else 
	{
        retLength = m_DemuxDevice.DemuxPes((const uint8_t*)Data, Length, true);
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
//! capture a single frame as an image
bool cDxr3Device::GrabImage(const char *FileName, bool Jpeg, int Quality, int SizeX, int SizeY)
{
	int w = SizeX;
	int h = SizeY;
	unsigned char *Data = new unsigned char[w*h*3];
	memset(Data, 0, w*h*3);

	// we could get a I-Frame and save it
	//m_DemuxDevice.StillPicture(Data, 100*1024);

	isyslog("grabbing to %s (%s %d %d %d)", FileName, Jpeg ? "JPEG" : "PNM", Quality, w, h);
	FILE *f = fopen(FileName, "wb");
	if (f) 
	{
		if (Jpeg) 
		{
			///< write JPEG file:
			struct jpeg_compress_struct cinfo;
			struct jpeg_error_mgr jerr;
			cinfo.err = jpeg_std_error(&jerr);
			jpeg_create_compress(&cinfo);
			jpeg_stdio_dest(&cinfo, f);
			cinfo.image_width = w;
			cinfo.image_height = h;
			cinfo.input_components = 3;
			cinfo.in_color_space = JCS_RGB;
						
			jpeg_set_defaults(&cinfo);
			jpeg_set_quality(&cinfo, Quality, true);
			jpeg_start_compress(&cinfo, true);
						
			int rs = w * 3;
			JSAMPROW rp[h];
			for (int k = 0; k < h; k++)
			{
				rp[k] = &Data[rs * k];
			}
			jpeg_write_scanlines(&cinfo, rp, h);
			jpeg_finish_compress(&cinfo);
			jpeg_destroy_compress(&cinfo);

		}
		else 
		{
			///< write PNM file:
			if (fprintf(f, "P6\n%d\n%d\n255\n", w, h) < 0 ||	fwrite(Data, w * h * 3, 1, f) < 0) 
			{
				LOG_ERROR_STR(FileName);
			}
		}
		fclose(f);
	}
	else
	{
		return false;
	}

	delete Data;
	return true;
}

// ==================================
void cDxr3Device::SetVideoFormat(bool VideoFormat16_9)
{
	if (cDxr3ConfigData::Instance().GetDebug())
	{
		cLog::Instance() << "cDxr3Device::SetPlayMode(ePlayMode PlayMode)() done\n";
	}
	// Do we need this function?
}

// ==================================
//! sets volume for audio output
void cDxr3Device::SetVolumeDevice(int Volume)
{
	cDxr3Interface::Instance().SetVolume(Volume);
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

#if VDRVERSNUM < 10307
// ==================================
// return osd
cOsdBase *cDxr3Device::NewOsd(int x, int y) 
{
    return m_DemuxDevice.NewOsd(x, y);
}
#endif

