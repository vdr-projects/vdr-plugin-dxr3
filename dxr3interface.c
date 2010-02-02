/*
 * dxr3interface.c
 *
 * Copyright (C) 2002-2004 Kai Möller
 * Copyright (C) 2004-2009 Christian Gmeiner
 * Copyright (C) 2005-2008 Ville Skyttä
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

#include "dxr3interface.h"
#include "dxr3osd.h"
#include "dxr3pesframe.h"

static const char *DEV_DXR3_OSD   = "_sp";
static const char *DEV_DXR3_VIDEO = "_mv";
static const char *DEV_DXR3_CONT  = "";

static const int UNKNOWN_ASPECT_RATIO = 0xdeadbeef;

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
int cDxr3Interface::OssSetPlayMode(uint32_t mode)
{
    return ioctl(m_fdControl, EM8300_IOCTL_SET_AUDIOMODE, &mode);
}

// clock
// ==================================
void cDxr3Interface::SetSysClock(uint32_t scr)
{
    Lock();
    CHECK(ioctl(m_fdControl, EM8300_IOCTL_SCR_SET, &scr));
    Unlock();
}

// ==================================
uint32_t cDxr3Interface::GetSysClock()
{
    uint32_t retval;

    Lock();
    CHECK(ioctl(m_fdControl, EM8300_IOCTL_SCR_GET, &retval));
    Unlock();

    return retval;
}

// ==================================
void cDxr3Interface::SetPts(uint32_t pts)
{

    Lock();
    CHECK(ioctl(m_fdVideo, EM8300_IOCTL_VIDEO_SETPTS, &pts));
    Unlock();
}

// ==================================
void cDxr3Interface::SetSpuPts(uint32_t pts)
{
    uint32_t newPts = 0;

    Lock();
    newPts = pts << 1;  // fix for DVD subtitles
    CHECK(ioctl(m_fdSpu, EM8300_IOCTL_SPU_SETPTS, &newPts));
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

    if (cSettings::instance()->forceLetterBox())
	ratio = EM8300_ASPECTRATIO_16_9;

    if (ratio != UNKNOWN_ASPECT_RATIO)
    {
	if (ratio != m_aspectRatio && requestCounter > 50)
	{
	    if (Setup.VideoFormat)
	    {
		aspect = EM8300_ASPECTRATIO_4_3;
#ifdef EM8300_IOCTL_SET_WSS
                if (cSettings::Instance().GetUseWSS())
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


void cDxr3Interface::setDimension(uint32_t horizontal, uint32_t vertical)
{
    if (horizontal > 0) {
        m_horizontal = horizontal;
    }

    if (vertical > 0) {
        m_vertical = vertical;
    }
}

void cDxr3Interface::dimension(uint32_t &horizontal, uint32_t &vertical)
{
    horizontal = m_horizontal;
    vertical = m_vertical;
}

// play functions
// ==================================
//! set playing mode and start sync engine
void cDxr3Interface::SetPlayMode()
{
    // this is the case, when SVDRP command DOF was used and
    // should be ignored.
    if (m_fdControl == -1) {
        return;
    }

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
void cDxr3Interface::PlayVideoFrame(cDxr3PesFrame *frame, uint32_t pts)
{
    //if (!m_VideoActive) {
    //    return;
    //}

    if (pts > 0) {
        pts += 45000;
        dsyslog("setting pts %d", pts);
        this->SetPts(pts);
    }

    Lock();

    const uint8_t *data = frame->GetPayload();
    uint32_t len = frame->GetPayloadLength();

    while (len > 0) {

        int ret = write(m_fdVideo, data, len);

        if (ret > 0) {
            len -= ret;
            data += ret;
        }
    }

    Unlock();

    //SetAspectRatio(frame->GetAspectRatio());
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

    // set default values
    m_VideoActive = false;
    m_horizontal = 720;
    m_vertical = 576;
    m_aspectRatio = UNKNOWN_ASPECT_RATIO;

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
//! uploadroutine for microcode
void cDxr3Interface::UploadMicroCode()
{
    if (!cSettings::instance()->loadFirmware()) {
        return;
    }

    dsyslog("[dxr3-interface] loading firmware");
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
    uint32_t videomode = cSettings::instance()->videoMode();

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
    m_bcs.brightness = cSettings::instance()->brightness();
    m_bcs.contrast = cSettings::instance()->contrast();
    m_bcs.saturation = cSettings::instance()->saturation();
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

    CHECK(ioctl(m_fdSpu, EM8300_IOCTL_SPU_BUTTON, &button));
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

    CHECK(ioctl(m_fdSpu, EM8300_IOCTL_SPU_BUTTON, &button));
}

// ==================================
void cDxr3Interface::SetPalette(unsigned int *pal)
{
    CHECK(ioctl(m_fdSpu, EM8300_IOCTL_SPU_SETPALETTE, (uint8_t*)pal));
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

void cDxr3Interface::updateBcsValues()
{
    // update m_bcs with values from settings
    m_bcs.brightness = cSettings::instance()->brightness();
    m_bcs.contrast = cSettings::instance()->contrast();
    m_bcs.saturation = cSettings::instance()->saturation();

    // update bcs values in hardware
    CHECK(ioctl(m_fdControl, EM8300_IOCTL_SETBCS, &m_bcs));
}

// ==================================
//! get brightness
int cDxr3Interface::GetBrightness()
{
    return m_bcs.brightness;
}

// ==================================
//! get contrast
int cDxr3Interface::GetContrast()
{
    return m_bcs.contrast;
}

// ==================================
//! get saturation
int cDxr3Interface::GetSaturation()
{
    return m_bcs.saturation;
}

// ==================================
cMutex* cDxr3Interface::m_pMutex = new cMutex;

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// tab-width: 4;
// indent-tabs-mode: nil
// End:
