/*
 * dxr3interface.h
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

#include <assert.h>
#include <math.h>
#include <sys/soundcard.h>

#include "dxr3interface.h"
#include "dxr3syncbuffer.h"
#include "dxr3log.h"
#include "dxr3configdata.h"

// ==================================
const int LPCM_HEADER_LENGTH = 7;
const int ZEROBUFFER_SIZE = 4096;
uint8_t zerobuffer[ZEROBUFFER_SIZE] = {0};

// ==================================
//! helper function to generate name
static const char *Dxr3Name(const char *Name, int n)
{
	static char buffer[PATH_MAX];
	snprintf(buffer, sizeof(buffer), "/dev/em8300%s-%d", Name, n);
	return buffer;
}

// ==================================
//! helper function to open the card #n
static int Dxr3Open(const char *Name, int n, int Mode)
{
	const char *FileName = Dxr3Name(Name, n);
	int fd = open(FileName, Mode);
	
	if (fd < 0)
	{
		cLog::Instance() << "Unable to open " << FileName << "\n";
	}
	return fd;
}

// ==================================
//! constructor
cDxr3Interface::cDxr3Interface()
{
	// open control stream
	m_fdControl = Dxr3Open("", cDxr3ConfigData::Instance().GetDxr3Card(), O_WRONLY | O_SYNC);
	if (!m_fdControl)
	{
		cLog::Instance() << "Unable to open the control stream!\n";
		cLog::Instance() << "Please check if the dxr3 modules are loaded!\n";
	}

	// upload microcode to dxr3
	UploadMicroCode();

	///< open 'multimedia' streams
    m_fdVideo = Dxr3Open("_mv", cDxr3ConfigData::Instance().GetDxr3Card(), O_WRONLY | O_SYNC);
    m_fdAudio = Dxr3Open("_ma", cDxr3ConfigData::Instance().GetDxr3Card(), O_WRONLY | O_SYNC);
    m_fdSpu = Dxr3Open("_sp", cDxr3ConfigData::Instance().GetDxr3Card(), O_WRONLY | O_SYNC);

	// everything ok?
	if (!m_fdVideo || !m_fdAudio || !m_fdSpu)
	{
		cLog::Instance() << "Unable to open one of the 'mulitmedia' streams!\n";
		exit(1);
	}

	// create clock
	m_pClock = new cDxr3SysClock(m_fdControl, m_fdVideo, m_fdSpu);

	// everything ok?
	if (!m_pClock)
	{
		cLog::Instance() << "Unable to allocate memory for m_pClock in cDxr3Interface\n";
		exit(1);
	}

	// set default values
    m_AudioActive = false;
    m_VideoActive = false; 
	m_OverlayActive = false;
    m_ExternalReleased = false;
    m_volume = 255;
	m_horizontal = 720;
    m_audioChannelCount = UNKNOWN_CHANNEL_COUNT;
    m_audioDataRate = 0;
    m_audioSampleSize = 0;

	// default value 9 = unused value
    m_audioMode =  9;
	m_aspectRatio = UNKNOWN_ASPECT_RATIO;
	m_spuMode = EM8300_SPUMODE_OFF;

	// configure device based on settings
	ConfigureDevice();

	// get bcs values from driver
	ioctl(m_fdControl, EM8300_IOCTL_GETBCS, &m_bcs);

	if (cDxr3ConfigData::Instance().GetDebug())
	{
		cLog::Instance() << "DXR3: brightness: " << m_bcs.brightness << "\n"; 
		cLog::Instance() << "DXR3: contrast: " << m_bcs.contrast << "\n"; 
		cLog::Instance() << "DXR3: saturation: " << m_bcs.saturation << "\n"; 
	}

    PlayBlackFrame();
    SetChannelCount(1);
}

// ==================================
cDxr3Interface::~cDxr3Interface()
{
	// close filehandles
	if (m_fdControl)
	{
		close(m_fdControl);
	}
    if (m_fdVideo)
	{
		close(m_fdVideo);
	}
	if (m_fdSpu)
	{
		close(m_fdSpu);
	}
	if (m_fdAudio)
	{
		close(m_fdAudio);
	}

	// free some memory
	if (m_pClock)
	{
		delete m_pClock;
	}
}

// main
// ==================================
void cDxr3Interface::Start()
{
}

// ==================================
void cDxr3Interface::Stop()
{
}

// audio
// ==================================
//! set audio-output to analog
void cDxr3Interface::SetAudioAnalog()
{
    int ioval = 0;
    Lock();

    if (!m_ExternalReleased && m_audioMode != EM8300_AUDIOMODE_ANALOG) 
	{
        int prevMode = m_audioMode;           
        m_audioMode = ioval = EM8300_AUDIOMODE_ANALOG;
        if (ioctl(m_fdControl, EM8300_IOCTL_SET_AUDIOMODE, &ioval) < 0) 
		{
            cLog::Instance() << "cDxr3AbsDevice::SetAudioAnalog Unable to set audiomode!\n";
        }
        if (prevMode ==  EM8300_AUDIOMODE_DIGITALAC3) 
		{
			ReOpenAudio();
        }  
    }

    Unlock();
}

// ==================================
//! set audio-output to digital pcm
void cDxr3Interface::SetAudioDigitalPCM()
{
    int ioval = 0;
    Lock();

    if (!m_ExternalReleased && m_audioMode != EM8300_AUDIOMODE_DIGITALPCM) 
	{
        int prevMode = m_audioMode;
        m_audioMode = ioval = EM8300_AUDIOMODE_DIGITALPCM;
        
        if (ioctl(m_fdControl, EM8300_IOCTL_SET_AUDIOMODE, &ioval) < 0) 
		{
            cLog::Instance() << "cDxr3AbsDevice::SetAudioAnalog Unable to set audiomode!\n";
        }
        if (prevMode ==  EM8300_AUDIOMODE_DIGITALAC3) 
		{
			ReOpenAudio();
        }        
    }

    Unlock();
}

// ==================================
//! set audio-output to digital ac3
void cDxr3Interface::SetAudioDigitalAC3()
{
    if (m_audioMode != EM8300_AUDIOMODE_DIGITALAC3) 
	{
	    int ioval = 0;
		Lock();

		if (!m_ExternalReleased && m_audioMode != EM8300_AUDIOMODE_DIGITALAC3) 
		{
			m_audioMode = ioval = EM8300_AUDIOMODE_DIGITALAC3;
			if (ioctl(m_fdControl, EM8300_IOCTL_SET_AUDIOMODE, &ioval) < 0) 
			{
				cLog::Instance() << "cDxr3AbsDevice::SetAudioAnalog Unable to set audiomode!\n";
			}
			ReOpenAudio();
		}
		
		Unlock();
    }
}

// ==================================
//! set audiosepeed
void cDxr3Interface::SetAudioSpeed(uint32_t speed)
{
    if (m_audioDataRate != speed && speed != UNKNOWN_DATA_RATE) 
	{
        if (!m_ExternalReleased) 
		{
            if (m_audioMode != EM8300_AUDIOMODE_DIGITALAC3) 
			{
			    if (ioctl(m_fdAudio, SNDCTL_DSP_SPEED, &speed) < 0) 
				{
					cLog::Instance() << "cDxr3AbsDevice::SetAudioSpeed Unable to set dsp speed\n";
                } 
            }
        }
        m_audioDataRate = speed;
    }
}

// ==================================
//! set nummber of channels
void cDxr3Interface::SetChannelCount(uint32_t count)
{
    if (m_audioChannelCount != count && count != UNKNOWN_CHANNEL_COUNT) 
	{
        if (!m_ExternalReleased) 
		{
            if (m_audioMode != EM8300_AUDIOMODE_DIGITALAC3) 
			{
                if (ioctl(m_fdAudio, SNDCTL_DSP_STEREO, &count) < 0) 
				{
                    cLog::Instance() << "cDxr3AbsDevice::SetChannelCount Unable to set channel count\n";
                } 
            }
        }
        m_audioChannelCount = count;
    }
}

// ==================================
//! set audio sample size
void cDxr3Interface::SetAudioSampleSize(uint32_t sampleSize) 
{
    if (!m_ExternalReleased) 
	{
        if (ioctl(m_fdAudio, SNDCTL_DSP_SAMPLESIZE, sampleSize)) 
		{
            cLog::Instance() <<"cDxr3AbsDevice::SetAudioSampleSize Unable to set audio sample size\n";
        }
    }
    m_audioSampleSize = sampleSize;                              
}

// clock
// ==================================
void cDxr3Interface::SetSysClock(uint32_t scr) 
{
    if (!m_ExternalReleased) 
	{    
        m_pClock->SetSysClock(scr);
    }
}

// ==================================
uint32_t cDxr3Interface::GetSysClock() const 
{
    uint32_t ret = 0;
    if (!m_ExternalReleased) 
	{
        ret = m_pClock->GetSysClock();
    }    
    return ret;
}

// ==================================
void cDxr3Interface::SetPts(uint32_t pts) 
{
    if (!m_ExternalReleased) 
	{    
        m_pClock->SetPts(pts);
    }
}

// ==================================
void cDxr3Interface::SetSpuPts(uint32_t pts) 
{
    pts = pts >> 1;    
    if (!m_ExternalReleased) 
	{
        if (pts > m_pClock->GetSysClock() && pts - m_pClock->GetSysClock() < 100000) 
		{
            m_pClock->SetSpuPts(pts);
        }
    }
}

// state changes
// ==================================
//! enable subpicture proeccesing of the dxr3
void cDxr3Interface::EnableSPU()
{
    int ioval = 0;
    Lock();

    if (!m_ExternalReleased && m_spuMode != EM8300_SPUMODE_ON) 
	{    
        m_spuMode = ioval = EM8300_SPUMODE_ON;
        if (ioctl(m_fdControl, EM8300_IOCTL_SET_SPUMODE, &ioval) < 0) 
		{
            cLog::Instance() << "cDxr3AbsDevice::EnableSpu Unable to set subpicture mode!\n";
        }
    }

    Unlock();
}

// ==================================
//! disable subpicture proeccesing of the dxr3
void cDxr3Interface::DisableSPU()
{
    int ioval = 0;
    Lock();

    if (!m_ExternalReleased && m_spuMode != EM8300_SPUMODE_OFF) 
	{    
        m_spuMode = ioval = EM8300_SPUMODE_OFF;
        if (ioctl(m_fdControl, EM8300_IOCTL_SET_SPUMODE, &ioval) < 0) 
		{
            cLog::Instance() << "cDxr3AbsDevice::EnableSpu Unable to set subpicture mode!\n";
        }
    }

    Unlock();
}

// ==================================
//! disable audio output of dxr3
void cDxr3Interface::DisableAudio()
{ 
	m_AudioActive = false;

	// we wirte zero buffers to dxr3
    if (!m_ExternalReleased) 
	{   
        if (write(m_fdAudio, zerobuffer, ZEROBUFFER_SIZE) < 0) Resuscitation();
        if (write(m_fdAudio, zerobuffer, ZEROBUFFER_SIZE) < 0) Resuscitation();
        if (write(m_fdAudio, zerobuffer, ZEROBUFFER_SIZE) < 0) Resuscitation();
        if (write(m_fdAudio, zerobuffer, ZEROBUFFER_SIZE) < 0) Resuscitation();
    }
}

// ==================================
//! enable overlay mode of the dxr3
void cDxr3Interface::EnableOverlay()
{
	// first we check, if it is enable yet
	if (m_OverlayActive)
	{
		return;
	}

	/*
	#define EM8300_OVERLAY_SIGNAL_ONLY 1
	#define EM8300_OVERLAY_SIGNAL_WITH_VGA 2
	#define EM8300_OVERLAY_VGA_ONLY 3
	*/

	int ioval = EM8300_OVERLAY_SIGNAL_WITH_VGA;
	// set overlay signal mode
	if (ioctl(m_fdControl, EM8300_IOCTL_OVERLAY_SIGNALMODE, &ioval) < 0) 
	{
		//######
		cLog::Instance() << "Singnalmode failed\n";
		return;
	}

	// setup overlay screen
	em8300_overlay_screen_t scr;
	scr.xsize = 1024;
	scr.ysize = 768;

	if (ioctl(m_fdControl, EM8300_IOCTL_OVERLAY_SETSCREEN, &scr) < 0) 
	{
		//######
		cLog::Instance() << "seting up screen failed\n";
		return;
	}

	// setup overlay window
	em8300_overlay_window_t win;
	win.xpos = 0;
	win.ypos = 0;
	win.width = 1024;
	win.height = 768;

	if (ioctl(m_fdControl, EM8300_IOCTL_OVERLAY_SETWINDOW, &win) < 0) 
	{
		//######
		cLog::Instance() << "seting up window failed\n";
		return;
	}

	m_OverlayActive = true;
}

// ==================================
//! disable overlay mode of the dxr3
void cDxr3Interface::DisanleOverlay()
{
	// is it allready disabled
	if (!m_OverlayActive)
	{
		return;
	}
}

// set/get functions
// ==================================
//! get aspect ratio
uint32_t cDxr3Interface::GetAspectRatio() const
{
    int ioval = 0;
    Lock();
    
    if (!m_ExternalReleased) 
	{
        if (ioctl(m_fdControl, EM8300_IOCTL_GET_ASPECTRATIO, &ioval) < 0) 
		{
			cLog::Instance() << "cDxr3AbsDevice::GetAspectRatio Unable to get aspect ratio\n";
        }
    }
    
    Unlock();
    return ioval;
}

// ==================================
//! set aspectratio
void cDxr3Interface::SetAspectRatio(uint32_t ratio) 
{
    static int requestCounter = 0;
    
    Lock();

    if (cDxr3ConfigData::Instance().GetForceLetterBox()) ratio = EM8300_ASPECTRATIO_16_9;
	if (Setup.VideoFormat) ratio = EM8300_ASPECTRATIO_4_3;

	if (!m_ExternalReleased && ratio != UNKNOWN_ASPECT_RATIO) 
	{      
		if (ratio != m_aspectRatio && requestCounter > 50) 
		{
            requestCounter = 0;
            if (ioctl(m_fdControl, EM8300_IOCTL_SET_ASPECTRATIO, &ratio) < 0) 
			{
				cLog::Instance() << "cDxr3AbsDevice::SetAspectRatio Unable to set aspect ratio\n";
            } 
			else 
			{
                m_aspectRatio = ratio;
            }            
        } 
		else 
		{
            if (ratio != m_aspectRatio) 
			{
                ++requestCounter;
            } 
			else 
			{
                requestCounter = 0;
            }
        }
    }
   
    Unlock();
}

// play functions
// ==================================
//! set playing mode and start sync engine
void cDxr3Interface::SetPlayMode()
{
    em8300_register_t reg;
    int ioval;

    Lock();

    if (!m_ExternalReleased) 
	{
        ioval = EM8300_SUBDEVICE_AUDIO;
        ioctl(m_fdControl, EM8300_IOCTL_FLUSH, &ioval);
        fsync(m_fdVideo);


        ioval = EM8300_PLAYMODE_PLAY;
        if (ioctl(m_fdControl, EM8300_IOCTL_SET_PLAYMODE, &ioval) < 0) 
		{
			cLog::Instance() << "cDxr3Device::SetPlayMode Unable to set playmode!\n";
        }
        reg.microcode_register = 1;
        reg.reg = 0;
        reg.val = MVCOMMAND_SYNC;

        if (ioctl(m_fdControl, EM8300_IOCTL_WRITEREG, &reg) < 0) 
		{
            cLog::Instance() << "cDxr3Device::SetPlayMode Unable to start em8300 sync engine\n";
        }
    }

    Unlock();
}

// ==================================
void cDxr3Interface::Pause()
{
    int ioval = EM8300_PLAYMODE_PAUSED;
    Lock();

    if (!m_ExternalReleased) 
	{    
        if (ioctl(m_fdControl, EM8300_IOCTL_SET_PLAYMODE, &ioval) < 0) 
		{
            cLog::Instance() << "cDxr3Device::Pause Unable to set playmode!\n";
        }
    }

    Unlock();
}
// ==================================
void cDxr3Interface::SingleStep()
{
    int ioval = EM8300_PLAYMODE_SINGLESTEP;
    Lock();

    if (!m_ExternalReleased) 
	{    
        if (ioctl(m_fdControl, EM8300_IOCTL_SET_PLAYMODE, &ioval) < 0) {
            cLog::Instance() << "cDxr3Device::Pause Unable to set playmode!\n";
        }
    }

    Unlock();
}

// ==================================
void cDxr3Interface::PlayVideoFrame(cFixedLengthFrame* pFrame, int times)
{
    int written = 0;
    int count = 0;

    if (m_VideoActive) 
	{
        Lock();

        if (!m_ExternalReleased) 
		{
			for (int i = 0; i < times; i++)
			{
				if (times > 1)
				{
					cLog::Instance() << "times: " << times << "\n";
				}

				while (written < pFrame->GetCount() && count >= 0) 
				{
					if ((count = write(m_fdVideo, pFrame->GetData() + written, pFrame->GetCount() - written)) < 0) 
					{
						// an error occured
						Resuscitation();
					}
					written += count;
				}

				// reset
				written = 0;
			}
        }

        Unlock();

        SetAspectRatio(pFrame->GetAspectRatio());
    }
}

// ==================================
void cDxr3Interface::PlayVideoFrame(const uint8_t* pBuf, int length, int times)
{
    Lock();
    
    if (!m_ExternalReleased) 
	{
		for (int i = 0; i < times; i++)
		{
			if (write(m_fdVideo, pBuf, length) < 0) Resuscitation();
		}
    }
    
    Unlock();
}

// ==================================
void cDxr3Interface::PlayAudioFrame(cFixedLengthFrame* pFrame)
{

	// XXX: Call this only with we are not in external mode?

    if (m_AudioActive) 
	{
        Lock();

        SetAudioSpeed(pFrame->GetDataRate());
        SetChannelCount(pFrame->GetChannelCount());

        if (!m_ExternalReleased) 
		{
			if (!cDxr3ConfigData::Instance().GetAc3OutPut()) ResampleVolume((short*)pFrame->GetData(), pFrame->GetCount());
            write(m_fdAudio, pFrame->GetData(), pFrame->GetCount());
        }

        Unlock();
    }
}

// ==================================
void cDxr3Interface::PlayAudioFrame(uint8_t* pBuf, int length)
{
    int written = 0;
    Lock();

    if (!m_ExternalReleased) 
	{
        if (!cDxr3ConfigData::Instance().GetAc3OutPut()) ResampleVolume((short*)pBuf, length);

		if ((written = write(m_fdAudio, pBuf, length) < 0)) Resuscitation();
        if (written != length) 
		{
			cLog::Instance() << "cDxr3Interface::PlayAudioFrame(uint8_t* pBuf, int length): Not written = " << length - written << "\n";
		}
    }
    Unlock();
}

// ==================================
void cDxr3Interface::PlayAudioLpcmFrame(uint8_t* pBuf, int length)
{
    if (length > (LPCM_HEADER_LENGTH + 2)) 
	{
        uint8_t* pFrame = new uint8_t[length - LPCM_HEADER_LENGTH];
        assert(!((length - LPCM_HEADER_LENGTH) % 2)); // only even number of bytes are allowed
        
		for (int i = LPCM_HEADER_LENGTH; i < length; i += 2) 
		{
            pFrame[i - LPCM_HEADER_LENGTH] = pBuf[i + 1];
            pFrame[i - LPCM_HEADER_LENGTH + 1] = pBuf[i];
        }

        int codedSpeed = (pBuf[5] >> 4) & 0x03;
        int speed = 0;

        switch (codedSpeed) 
		{
            case 1:
                speed = 96000;
			    break;
            
			case 2:
                speed = 44100;
				break;
            
			case 3:
                speed = 32000;
				break;
            
			default:
                speed = 48000;
				break;
        }

        SetAudioSpeed(speed);
        PlayAudioFrame(pFrame, length - LPCM_HEADER_LENGTH);
        delete[] pFrame;
    }
}

// external device access
// ==================================
//! release devices, so mplayer-plugin, for instance,
//! can access the dxr3
void cDxr3Interface::ExternalReleaseDevices()
{
    Lock();

    if (!m_ExternalReleased) 
	{
        if (m_fdControl > 0) close(m_fdControl);
        if (m_fdVideo > 0) close(m_fdVideo);
        if (m_fdSpu > 0) close(m_fdSpu);
        if (m_fdAudio > 0) close(m_fdAudio);
        m_fdControl = m_fdVideo = m_fdSpu = m_fdAudio = -1;

        m_ExternalReleased = true;
        
		delete m_pClock;
        m_pClock = 0;
    }
    
    Unlock();
}

// ==================================
//! reopen devices for using in the dxr3 plugin
void cDxr3Interface::ExternalReopenDevices()
{
    Lock();

    if (m_ExternalReleased) 
	{
		// open control stream
		m_fdControl = Dxr3Open("", cDxr3ConfigData::Instance().GetDxr3Card(), O_WRONLY | O_SYNC);

		// open 'multimedia' streams
	    m_fdVideo = Dxr3Open("_mv", cDxr3ConfigData::Instance().GetDxr3Card(), O_WRONLY | O_SYNC);
	    m_fdAudio = Dxr3Open("_ma", cDxr3ConfigData::Instance().GetDxr3Card(), O_WRONLY | O_SYNC);
	    m_fdSpu = Dxr3Open("_sp", cDxr3ConfigData::Instance().GetDxr3Card(), O_WRONLY | O_SYNC);

        if (m_fdControl < 0 || m_fdVideo < 0 || m_fdAudio < 0 || m_fdSpu <0) 
		{
            ExternalReleaseDevices();
        } 
		else 
		{
            m_pClock = new cDxr3SysClock(m_fdControl, m_fdVideo, m_fdSpu);
			if (!m_pClock)
			{
				cLog::Instance() << "Unable to allocate memory for m_pClock in cDxr3Interface\n";
				exit(1);
			}

            SetChannelCount(1);    
            m_ExternalReleased = false;
        }

        Resuscitation();
    }
    
    Unlock();
}


// tools
// ==================================
//! play blackframe on tv
void cDxr3Interface::PlayBlackFrame()
{
    extern char blackframe[];
    extern int blackframeLength;
    
    Lock();
    
    if (!m_ExternalReleased) 
	{   
        if (write(m_fdVideo, blackframe, blackframeLength) < 0) Resuscitation();
        if (write(m_fdVideo, blackframe, blackframeLength) < 0) Resuscitation();
        if (write(m_fdVideo, blackframe, blackframeLength) < 0) Resuscitation();        
    }
    m_horizontal = 720;
    
    Unlock();
}

// ==================================
void cDxr3Interface::ReOpenAudio()
{
    Lock();
 
	if (!m_ExternalReleased) 
	{
        if (m_fdAudio > 0) 
		{
            int bufsize = 0;
            ioctl(m_fdAudio, SNDCTL_DSP_GETODELAY, &bufsize);
            usleep(bufsize / 192 * 1000);

            delete m_pClock;
            close(m_fdAudio);
            
			m_fdAudio = open("/dev/em8300_ma-0", O_WRONLY | O_SYNC);

                        
            uint32_t tmpAudioDataRate = m_audioDataRate;
            uint32_t tmpAudioChannelCount = m_audioChannelCount;
            m_audioDataRate = m_audioChannelCount = 0;
            m_pClock = new cDxr3SysClock(m_fdControl, m_fdVideo, m_fdSpu);
            SetAudioSpeed(tmpAudioDataRate);
            SetChannelCount(tmpAudioChannelCount);
        }
	}

    Unlock();
}

#if VDRVERSNUM < 10307
// ==================================
cOsdBase* cDxr3Interface::NewOsd(int x, int y)
{
	return new cDxr3Osd(x, y);
}
#endif

// ==================================
//! uploadroutine for microcode
void cDxr3Interface::UploadMicroCode()
{
	if (cDxr3ConfigData::Instance().GetDebug())
	{
		cLog::Instance() << "cDxr3Interface::UploadMicroCode: uploading...";
	}

    em8300_microcode_t em8300_microcode;    
    struct stat s;

	// try to open it
	// MICROCODE comes from makefile
    int UCODE = open(MICROCODE, O_RDONLY);
    
    if (UCODE <0) 
	{
		cLog::Instance() << "Unable to open microcode file " << MICROCODE << " for reading\n";
        exit(1);
    }

    if (fstat(UCODE, &s ) <0) 
	{
		cLog::Instance() << "Unable to fstat ucode file\n";
        exit(1);
    }

	// read microcode
    em8300_microcode.ucode = new char[s.st_size];
    if (em8300_microcode.ucode == NULL) 
	{
		cLog::Instance() << "Unable to malloc() space for ucode\n";
        exit(1);
    }
    
    if (read(UCODE,em8300_microcode.ucode,s.st_size) < 1) 
	{
		cLog::Instance() << "Unable to read data from microcode file\n";
		// free memory to avoid memory leak
		delete [] (char*) em8300_microcode.ucode;
		exit(1);
    }

    close(UCODE);

    em8300_microcode.ucode_size = s.st_size;

	// upload it
    if( ioctl(m_fdControl, EM8300_IOCTL_INIT, &em8300_microcode) == -1) 
	{
		cLog::Instance() << "Microcode upload to failed!! \n";
		// free memory to avoid memory leak
		delete [] (char*) em8300_microcode.ucode;
        exit(1);
    }

	// free memory to avoid memory leak
    delete [] (char*) em8300_microcode.ucode;

	if (cDxr3ConfigData::Instance().GetDebug())
	{
		cLog::Instance() << "...done\n";
	}
}

// ==================================
//! config and setup device via ioctl calls
void cDxr3Interface::ConfigureDevice()
{
	// get videomode from driver
	uint32_t videomode_from_driver = 0;

	if (ioctl(m_fdControl, EM8300_IOCTL_GET_VIDEOMODE, &videomode_from_driver) == -1)
	{
		cLog::Instance() << "Unable to get videomode\n";
        exit(1);
	}

	// set video mode
	uint32_t videomode = 0;
	if (cDxr3ConfigData::Instance().GetVideoMode() == PAL)
	{
		videomode = EM8300_VIDEOMODE_PAL;
		if (cDxr3ConfigData::Instance().GetDebug())
		{
			cLog::Instance() << "cDxr3Interface::ConfigureDevice: Videomode = PAL\n";
		}
	}
	else if (cDxr3ConfigData::Instance().GetVideoMode() == PAL60)
	{
		videomode = EM8300_VIDEOMODE_PAL60;
		if (cDxr3ConfigData::Instance().GetDebug())
		{
			cLog::Instance() << "cDxr3Interface::ConfigureDevice: Videomode = PAL60\n";
		}
	}
	else
	{
		videomode = EM8300_VIDEOMODE_NTSC;
		if (cDxr3ConfigData::Instance().GetDebug())
		{
			cLog::Instance() << "cDxr3Interface::ConfigureDevice: Videomode = NTSC\n";
		}
	}

	// are the two videmodes different?
	if (videomode_from_driver != videomode)
	{
		// make ioctl
		if (ioctl(m_fdControl, EM8300_IOCTL_SET_VIDEOMODE, &videomode) == -1) 
		{
			cLog::Instance() << "Unable to set videomode\n";
			exit(1);
		}
	}

	// set audio mode
	if (!cDxr3ConfigData::Instance().GetUseDigitalOut())
	{
		SetAudioAnalog();
		if (cDxr3ConfigData::Instance().GetDebug())
		{
			cLog::Instance() << "cDxr3Interface::ConfigureDevice: Audiomode = Analog\n";
		}
	}
}

// ==================================
//! reset whole hardware
void cDxr3Interface::Resuscitation() 
{
    time_t startt = time(&startt);
    time_t endt = 0;
    m_ExternalReleased = true;
    dsyslog("cDxr3Interface::Resuscitation Device failure detected");
    
    UploadMicroCode();
    dsyslog("cDxr3Interface::Resuscitation Micro code upload successfully");
    
//    NonBlockingCloseOpen();
    m_ExternalReleased = false;
    
    endt = time(&endt);
    
    dsyslog("cDxr3Interface::Resuscitation Reopening devices took %d", (int)(endt - startt));
    
    if (endt - startt > 4) 
	{
        exit(1);
    }

    ConfigureDevice();
}

// ==================================
//! pcm resampling funtcion
void cDxr3Interface::ResampleVolume(short* pcmbuf, int size)
{
    if (m_volume == 0) 
	{
        memset(pcmbuf, 0, size);  
    }
	else if (m_volume != 255) 
	{
		int factor = (int)pow (2.0, (double)m_volume/32 + 8.0) - 1;
		//int factor = (int)pow (2.0, (double)m_volume/16) - 1;
        for (int i = 0; i < size / (int)sizeof(short); i++) 
		{
			pcmbuf[i] = (((int)pcmbuf[i]) * factor) / 65536;
        }
    }
}

// ==================================
void cDxr3Interface::ClearOsd()
{
    encodedata ed;
    int controlstart= 0;
    int x1 = 0;
    int& i = ed.count = 0;

    // display duration... 
    ed.data[i++]= 0x00;
    ed.data[i++]= 0x00; //durration before turn on command occurs
                         //in 90000/1024 units
    // x1
    ed.data[i++]= x1 >> 8;  //since this is the last command block, this
    ed.data[i++]= x1 & 0xff; //points back to itself


    // 0x01: start displaying
    ed.data[i++]= 0x02;

    // 0xFF: end sequence
    ed.data[i++]= 0xFF;
    if (!i&1) 
	{
        ed.data[i++]= 0xff;
    }

    // x0
    ed.data[2]= (controlstart) >> 8;
    ed.data[3]= (controlstart) & 0xff;

    // packet size
    ed.data[0]= i >> 8;
    ed.data[1]= i & 0xff;

    if (!m_ExternalReleased) 
	{
        WriteSpu((const uint8_t*) &ed, (int) ed.count);
        ClearButton();
    }
}

// ==================================
void cDxr3Interface::WriteSpu(const uint8_t* pBuf, int length)
{
    Lock();
        
    if (!m_ExternalReleased) 
	{     
        if (write(m_fdSpu, pBuf, length) < 0) Resuscitation();
    }
        
    Unlock();
}

// ==================================
void cDxr3Interface::SetButton(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t palette)
{
    em8300_button_t button;

    button.color = palette >> 16;
    button.contrast = palette & 0xFFFF;
    button.top = sy;
    button.bottom = ey;
    button.left = sx;
    button.right = ex;

    ioctl(m_fdSpu, EM8300_IOCTL_SPU_BUTTON, &button);
}

// ==================================
void cDxr3Interface::ClearButton()
{
    em8300_button_t button;

    button.color = 0;
    button.contrast = 0;
    button.top = 1;
    button.bottom = 2;
    button.left = 1;
    button.right = 2;

    ioctl(m_fdSpu, EM8300_IOCTL_SPU_BUTTON, &button);
}

// ==================================
void cDxr3Interface::SetPalette(unsigned int *pal)
{
	ioctl(m_fdSpu, EM8300_IOCTL_SPU_SETPALETTE, (uint8_t*)pal);
}

// helper functions for dxr3 main osd screen
// ==================================
//! reset dxr3 card
void cDxr3Interface::ResetHardware()
{
	Lock();
	cLog::Instance() << "cDxr3Interface: Resting DXR3 hardware\n";
    Resuscitation();
    Unlock();
}

// set brightness/contrast/saturation
// ==================================
//! set brightness
void cDxr3Interface::SetBrightness(int value)
{
	m_bcs.brightness = value;

	if (ioctl(m_fdControl, EM8300_IOCTL_SETBCS, &m_bcs) < 0) 
	{
		cLog::Instance() << "cDxr3Interface::SetBrightness: Unable to set brightness to " << value << "\n";
	} 
}

// ==================================
//! set contrast
void cDxr3Interface::SetContrast(int value)
{
	m_bcs.contrast = value;

	if (ioctl(m_fdControl, EM8300_IOCTL_SETBCS, &m_bcs) < 0) 
	{
		cLog::Instance() << "cDxr3Interface::SetContrast: Unable to set contrast to " << value << "\n";
	} 
}

// ==================================
//! set saturation
void cDxr3Interface::SetSaturation(int value)
{
	m_bcs.saturation = value;

	if (ioctl(m_fdControl, EM8300_IOCTL_SETBCS, &m_bcs) < 0) 
	{
		cLog::Instance() << "cDxr3Interface::SetSaturation: Unable to set saturation to " << value << "\n";
	} 
}

// access registers
// ==================================
//! read a register
long cDxr3Interface::ReadRegister(int registernum)
{
	em8300_register_t reg;
	
	reg.microcode_register = 0;
	reg.reg = registernum;
	reg.val = 0;
	
	ioctl(m_fdControl, EM8300_IOCTL_READREG, &reg);
	return (reg.val);	
}

// ==================================
//! write a register
void cDxr3Interface::WriteRegister(int registernum, int val)
{
	em8300_register_t reg;
	
	reg.microcode_register = 0;
	reg.reg = registernum;
	reg.val = val;
	ioctl(m_fdControl, EM8300_IOCTL_WRITEREG, &reg);
}

// ==================================
//! grabs the current tv screen
void cDxr3Interface::GrabScreen(int w, int h, char** buf)
{
	
}

// maybe we should copy this routine into em8300 driver
// ==================================
//! It appears that the follow routine copies length bytes to memory pointed to
//! by dst. 
//! Most likely, pos contains some kind of buffer offset.
char cDxr3Interface::Dxr3CopyYUVData(int pos, int *dst, int length)
{
	int l1;

	for (l1 = 0x1000; (l1) ;--l1) 
	{
		if (ReadRegister(0x1c1a) == 0)
			break;
	}

	if (l1 == 0) 
	{
		//printf("Borked!\n");
		exit(0);
	}

	WriteRegister(0x1c50, 8);
	WriteRegister(0x1c51, pos & 0xffff);
	WriteRegister(0x1c52, pos >>16);
	WriteRegister(0x1c53, length);
	WriteRegister(0x1c54, length);
	WriteRegister(0x1c55, 0);
	WriteRegister(0x1c56, 1);
	WriteRegister(0x1c57, 1);
	WriteRegister(0x1c58, pos & 0xffff);
	WriteRegister(0x1c59, 0);
	WriteRegister(0x1c5a, 1);
	
	int l2 = 0;
	for (l2=0; l2 < (length>>2) ; ++l2) 
	{
		*dst++ = ReadRegister(0x11800);
	}

	switch (length % 4) 
	{
	    case 3:
		*dst++ = ReadRegister(0x11000);
			break;

	    case 2:
		*dst++ = ReadRegister(0x10800);
			break;

	    case 1:
		*dst++ = ReadRegister(0x10000);
			break;
	}
	
	for (l1 = 0x1000; (l1) ; --l1) 
	{
		if (ReadRegister(0x1c1a) == 0)
			return 1;
	}
	
	return 0;	
}

// ==================================
cMutex* cDxr3Interface::m_pMutex = new cMutex;
