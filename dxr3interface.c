/*
 * dxr3interface.c
 *
 * Copyright (C) 2002-2004 Kai Möller
 * Copyright (C) 2004-2009 Christian Gmeiner
 * Copyright (C) 2005-2008 Ville Skyttä
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

#include "dxr3interface.h"
#include "dxr3syncbuffer.h"
#include "dxr3osd.h"

static const char *DEV_DXR3_OSD   = "_sp";
static const char *DEV_DXR3_VIDEO = "_mv";
static const char *DEV_DXR3_OSS   = "_ma";
static const char *DEV_DXR3_CONT  = "";

// ==================================
//! constructor
cDxr3Interface::cDxr3Interface() :
    m_fdControl(-1), m_fdVideo(-1), m_fdSpu(-1)
{
    ClaimDevices();
}

// ==================================
//! destructor
cDxr3Interface::~cDxr3Interface()
{
    ReleaseDevices();
}

// audio
bool cDxr3Interface::IsOssAudio()
{
    // try to open oss audio interface
    int handle = Dxr3Open(DEV_DXR3_OSS, O_RDWR | O_NONBLOCK, false);

    if (handle > -1) {
        close(handle);
        return true;
    }

    return false;
}

int cDxr3Interface::OssSetPlayMode(uint32_t mode)
{
    return ioctl(m_fdControl, EM8300_IOCTL_SET_AUDIOMODE, &mode);
}

// clock
// ==================================
void cDxr3Interface::SetSysClock(uint32_t scr)
{
	m_pClock->SetSysClock(scr);
}

// ==================================
uint32_t cDxr3Interface::GetSysClock() const
{
	return m_pClock->GetSysClock();
}

// ==================================
int64_t cDxr3Interface::GetPts()
{
    return m_lastSeenPts << 1;
}

// ==================================
void cDxr3Interface::SetPts(uint32_t pts)
{
	m_pClock->SetPts(pts);
}

// ==================================
void cDxr3Interface::SetSpuPts(uint32_t pts)
{
	pts = pts >> 1;

	if (pts > m_pClock->GetSysClock() &&
	    pts - m_pClock->GetSysClock() < 100000)
	{
	    m_pClock->SetSpuPts(pts);
	}
}

// state changes
// ==================================
//! enable subpicture processing of the dxr3
void cDxr3Interface::EnableSPU()
{
    uint32_t ioval = EM8300_SPUMODE_ON;

    Lock();

    if (m_spuMode != ioval)
    {
	if (ioctl(m_fdControl, EM8300_IOCTL_SET_SPUMODE, &ioval) == -1)
	{
	    esyslog("dxr3: unable to enable subpicture mode: %m");
	}
	else
	{
	    m_spuMode = ioval;
	}
    }

    Unlock();
}

// ==================================
//! disable subpicture proeccesing of the dxr3
void cDxr3Interface::DisableSPU()
{
    uint32_t ioval = EM8300_SPUMODE_OFF;

    Lock();

    if (m_spuMode != ioval)
    {
	if (ioctl(m_fdControl, EM8300_IOCTL_SET_SPUMODE, &ioval) == -1)
	{
	    esyslog("dxr3: unable to disable subpicture mode: %m");
	}
	else
	{
	    m_spuMode = ioval;
	}
    }

    Unlock();
}

// ==================================
//! disable audio output of dxr3
void cDxr3Interface::DisableAudio()
{
    Lock();
    m_AudioActive = false;
    Unlock();
}

// set/get functions
// ==================================
//! get aspect ratio
uint32_t cDxr3Interface::GetAspectRatio() const
{
    int ioval = 0;

    Lock();

	if (ioctl(m_fdControl, EM8300_IOCTL_GET_ASPECTRATIO, &ioval) == -1)
	{
	    esyslog("dxr3: unable to get aspect ratio: %m");
	}

    Unlock();

    return ioval;
}

// ==================================
//! set aspect ratio
void cDxr3Interface::SetAspectRatio(uint32_t ratio)
{
    static int requestCounter = 0;
    int aspect;

    Lock();

    if (cDxr3ConfigData::Instance().GetForceLetterBox())
	ratio = EM8300_ASPECTRATIO_16_9;

    if (ratio != UNKNOWN_ASPECT_RATIO)
    {
	if (ratio != m_aspectRatio && requestCounter > 50)
	{
	    if (Setup.VideoFormat)
	    {
		aspect = EM8300_ASPECTRATIO_4_3;
#ifdef EM8300_IOCTL_SET_WSS
		if (cDxr3ConfigData::Instance().GetUseWSS())
		{
		    int wssmode;
		    if (ratio == EM8300_ASPECTRATIO_16_9)
			wssmode = EM8300_WSS_16_9;
		    else
			wssmode = EM8300_WSS_OFF;
		    if (ioctl(m_fdControl, EM8300_IOCTL_SET_WSS, &wssmode) == -1)
		    {
			esyslog("dxr3: unable to set WSS: %m");
		    }
		}
#endif
	    }
	    else
	    {
		aspect = ratio;
	    }

	    requestCounter = 0;
	    if (ioctl(m_fdControl, EM8300_IOCTL_SET_ASPECTRATIO, &aspect) == -1)
	    {
		esyslog("dxr3: unable to set aspect ratio: %m");
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

	ioval = EM8300_SUBDEVICE_AUDIO;
	ioctl(m_fdControl, EM8300_IOCTL_FLUSH, &ioval);
	fsync(m_fdVideo);

	ioval = EM8300_PLAYMODE_PLAY;
	if (ioctl(m_fdControl, EM8300_IOCTL_SET_PLAYMODE, &ioval) == -1)
	{
	    esyslog("dxr3: unable to set play mode: %m");
	}
	reg.microcode_register = 1;
	reg.reg = 0;
	reg.val = MVCOMMAND_SYNC;

	if (ioctl(m_fdControl, EM8300_IOCTL_WRITEREG, &reg) == -1)
	{
	    esyslog("dxr3: unable to start em8300 sync engine: %m");
	}

    Unlock();
}

// ==================================
void cDxr3Interface::Pause()
{
    int ioval = EM8300_PLAYMODE_PAUSED;

    Lock();

	if (ioctl(m_fdControl, EM8300_IOCTL_SET_PLAYMODE, &ioval) == -1)
	{
	    esyslog("dxr3: unable to set pause mode: %m");
	}

    Unlock();
}
// ==================================
void cDxr3Interface::SingleStep()
{
    int ioval = EM8300_PLAYMODE_SINGLESTEP;

    Lock();

	if (ioctl(m_fdControl, EM8300_IOCTL_SET_PLAYMODE, &ioval) == -1)
	{
	    esyslog("dxr3: unable to set single-step mode: %m");
	}

    Unlock();
}

// ==================================
void cDxr3Interface::PlayVideoFrame(cFixedLengthFrame* pFrame, int times)
{
    if (!m_VideoActive) {
        return;
    }

    int written = 0;
    int count = 0;

	Lock();

	for (int i = 0; i < times; i++)
	{
	if (times > 1)
	{
		dsyslog("dxr3: playvideoframe: times=%d", times);
	}

	while (written < pFrame->GetCount() && count >= 0)
	{
		if ((count = write(m_fdVideo, pFrame->GetData() + written, pFrame->GetCount() - written)) == -1)
		{
		// an error occured
		Resuscitation();
		}
		written += count;
	}

	// reset
	written = 0;
	}

	Unlock();

	SetAspectRatio(pFrame->GetAspectRatio());
	uint32_t pts = pFrame->GetPts();
	if (pts > 0)
	    m_lastSeenPts = pts;
}

// ==================================
void cDxr3Interface::PlayVideoFrame(const uint8_t* pBuf, int length, int times)
{
    Lock();

	for (int i = 0; i < times; i++)
	{
	    if (write(m_fdVideo, pBuf, length) == -1)
		Resuscitation();
	}

    Unlock();
}

// ==================================
void cDxr3Interface::ClaimDevices()
{
    // devices already open
    if (m_fdControl > -1 && m_fdVideo > -1 && m_fdSpu > -1) {
        return;
    }

    // open control stream
    m_fdControl = Dxr3Open(DEV_DXR3_CONT, O_WRONLY | O_SYNC);
    if (m_fdControl == -1)
    {
	esyslog("dxr3: please verify that the em8300 modules are loaded");
	exit(1);
    }

    // upload microcode to dxr3
    UploadMicroCode();

    ///< open multimedia streams
    m_fdVideo = Dxr3Open(DEV_DXR3_VIDEO, O_WRONLY | O_SYNC);
    m_fdSpu = Dxr3Open(DEV_DXR3_OSD, O_WRONLY | O_SYNC);

    // everything ok?
    if (m_fdVideo == -1 || m_fdSpu == -1)
    {
	esyslog("dxr3: fatal: unable to open some em8300 devices");
	exit(1);
    }

    // create clock
    m_pClock = new cDxr3SysClock(m_fdControl, m_fdVideo, m_fdSpu);

    // everything ok?
    if (!m_pClock)
    {
	esyslog("dxr3: fatal: unable to allocate memory for em8300 clock");
	close(m_fdControl);
	close(m_fdVideo);
	close(m_fdSpu);
	exit(1);
    }

    // set default values
    m_AudioActive = false;
    m_VideoActive = false;
    m_horizontal = 720;
    m_vertical = 576;
    m_aspectRatio = UNKNOWN_ASPECT_RATIO;
    m_spuMode = EM8300_SPUMODE_OFF;

    // configure device based on settings
    ConfigureDevice();

    // get bcs values from driver
    if (ioctl(m_fdControl, EM8300_IOCTL_GETBCS, &m_bcs) == -1)
    {
	esyslog("dxr3: failed to get brightness/contrast/saturation: %m");
    }
    else
    {
	dsyslog("dxr3: intf: brightness=%d,contrast=%d,saturation=%d at init",
		m_bcs.brightness, m_bcs.contrast, m_bcs.saturation);
    }

    PlayBlackFrame();
}

// ==================================
void cDxr3Interface::ReleaseDevices()
{
    if (m_fdControl > -1) {
    	close(m_fdControl);
    	m_fdControl = -1;
    }

    if (m_fdVideo > -1) {
		close(m_fdVideo);
		m_fdVideo = -1;
    }

    if (m_fdSpu > -1) {
		close(m_fdSpu);
		m_fdSpu = -1;
    }

    m_aspectRatio = UNKNOWN_ASPECT_RATIO;
    delete m_pClock;
    m_pClock = NULL;
}

// tools
// ==================================
//! play black frame on tv
void cDxr3Interface::PlayBlackFrame()
{
    extern char blackframe[];
    extern int blackframeLength;

    Lock();

	for (int i = 0; i < 3; i++)
	{
	    if (write(m_fdVideo, blackframe, blackframeLength) == -1)
		Resuscitation();
	}

    m_horizontal = 720;
    m_vertical = 576;

    Unlock();
}

// ==================================
void cDxr3Interface::ReOpenAudio()
{
    Lock();
/*
    if (!m_ExternalReleased)
    {
	if (m_fdAudio != -1)
	{
	    int bufsize = 0;
	    ioctl(m_fdAudio, SNDCTL_DSP_GETODELAY, &bufsize);
	    cCondWait::SleepMs(bufsize / 192);

	    delete m_pClock;
	    close(m_fdAudio);

	    m_fdAudio = Dxr3Open(DEV_DXR3_OSS, O_WRONLY | O_SYNC);

	    m_pClock = new cDxr3SysClock(m_fdControl, m_fdVideo, m_fdSpu);
	}
    }
*/
    Unlock();
}

// ==================================
//! uploadroutine for microcode
void cDxr3Interface::UploadMicroCode()
{
    em8300_microcode_t em8300_microcode;
    struct stat s;

    // try to open it
    // MICROCODE comes from makefile
    int UCODE = open(MICROCODE, O_RDONLY);

    if (UCODE <0)
    {
	esyslog("dxr3: fatal: unable to open microcode file %s: %m",
		MICROCODE);
	exit(1);
    }

    if (fstat(UCODE, &s ) <0)
    {
	esyslog("dxr3: fatal: unable to fstat microcode file %s: %m",
		MICROCODE);
	exit(1);
    }

    // read microcode
    em8300_microcode.ucode = new char[s.st_size];
    if (em8300_microcode.ucode == NULL)
    {
	esyslog("dxr3: fatal: unable to malloc() space for microcode");
	exit(1);
    }

    if (read(UCODE,em8300_microcode.ucode,s.st_size) < 1)
    {
	esyslog("dxr3: fatal: unable to read microcode file %s: %m",
		MICROCODE);
	// free memory to avoid memory leak
	delete [] (char*) em8300_microcode.ucode;
	exit(1);
    }

    close(UCODE);

    em8300_microcode.ucode_size = s.st_size;

    // upload it
    if( ioctl(m_fdControl, EM8300_IOCTL_INIT, &em8300_microcode) == -1)
    {
	esyslog("dxr3: fatal: microcode upload failed: %m");
	// free memory to avoid memory leak
	delete [] (char*) em8300_microcode.ucode;
	exit(1);
    }

    // free memory to avoid memory leak
    delete [] (char*) em8300_microcode.ucode;
}

// ==================================
//! config and setup device via ioctl calls
void cDxr3Interface::ConfigureDevice()
{
    uint32_t videomode = cDxr3ConfigData::Instance().GetVideoMode();

    switch (videomode) {
    case PAL:
        dsyslog("dxr3: configure: video mode: PAL");
        break;

    case PAL60:
        dsyslog("dxr3: configure: video mode: PAL60");
        break;

    case NTSC:
        dsyslog("dxr3: configure: video mode: NTSC");
        break;
    }

    if (ioctl(m_fdControl, EM8300_IOCTL_SET_VIDEOMODE, &videomode) == -1) {
        esyslog("dxr3: unable to set video mode: %m");
    }

    // set brightness/contrast/saturation
    m_bcs.brightness = cDxr3ConfigData::Instance().GetBrightness();
    m_bcs.contrast = cDxr3ConfigData::Instance().GetContrast();
    m_bcs.saturation = cDxr3ConfigData::Instance().GetSaturation();
    dsyslog("dxr3: configure: brightness=%d,contrast=%d,saturation=%d",
	    m_bcs.brightness, m_bcs.contrast, m_bcs.saturation);
    if (ioctl(m_fdControl, EM8300_IOCTL_SETBCS, &m_bcs) == -1) {
        esyslog("dxr3: unable to set brightness/contrast/saturation: %m");
    }
}

// ==================================
//! reset whole hardware
void cDxr3Interface::Resuscitation()
{
    time_t startt = time(&startt);
    time_t endt = 0;

    dsyslog("dxr3: resuscitation: device failure or user initiated reset");

    ReleaseDevices();
    Unlock();
    ClaimDevices();
    Lock();

    endt = time(&endt);
    if (endt - startt > 4)
    {
	esyslog("dxr3: fatal: reopening devices took too long");
	exit(1);
    }
    dsyslog("dxr3: resuscitation: reopening devices took %ld seconds",
	    endt - startt);
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

	WriteSpu((const uint8_t*) &ed, (int) ed.count);
	ClearButton();
}

// ==================================
void cDxr3Interface::WriteSpu(const uint8_t* pBuf, int length)
{
    Lock();

	if (write(m_fdSpu, pBuf, length) == -1)
	    Resuscitation();

    Unlock();
}

// ==================================
void cDxr3Interface::SetButton(uint16_t sx, uint16_t sy, uint16_t ex,
			       uint16_t ey, uint32_t palette)
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

    isyslog("dxr3: hardware reset requested");
    Resuscitation();

    Unlock();
}

// ==================================
//! get brightness
int cDxr3Interface::GetBrightness()
{
    return m_bcs.brightness;
}

// ==================================
//! set brightness
void cDxr3Interface::SetBrightness(int value)
{
    int oldval = m_bcs.brightness;
    m_bcs.brightness = value;

    if (ioctl(m_fdControl, EM8300_IOCTL_SETBCS, &m_bcs) == -1)
    {
	esyslog("dxr3: unable to set brightness to %d: %m", value);
	m_bcs.brightness = oldval;
    }
}

// ==================================
//! get contrast
int cDxr3Interface::GetContrast()
{
    return m_bcs.contrast;
}

// ==================================
//! set contrast
void cDxr3Interface::SetContrast(int value)
{
    int oldval = m_bcs.contrast;
    m_bcs.contrast = value;

    if (ioctl(m_fdControl, EM8300_IOCTL_SETBCS, &m_bcs) == -1)
    {
	esyslog("dxr3: unable to set contrast to %d: %m", value);
	m_bcs.contrast = oldval;
    }
}

// ==================================
//! get saturation
int cDxr3Interface::GetSaturation()
{
    return m_bcs.saturation;
}

// ==================================
//! set saturation
void cDxr3Interface::SetSaturation(int value)
{
    int oldval = m_bcs.saturation;
    m_bcs.saturation = value;

    if (ioctl(m_fdControl, EM8300_IOCTL_SETBCS, &m_bcs) == -1)
    {
	esyslog("dxr3: unable to set saturation to %d: %m", value);
	m_bcs.saturation = oldval;
    }
}

// ==================================
cMutex* cDxr3Interface::m_pMutex = new cMutex;

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
