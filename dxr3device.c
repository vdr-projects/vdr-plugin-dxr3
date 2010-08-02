/*
 *           _                 _             _                 _          _____
 * __   ____| |_ __      _ __ | |_   _  __ _(_)_ __         __| |_  ___ _|___ /
 * \ \ / / _` | '__|____| '_ \| | | | |/ _` | | '_ \ _____ / _` \ \/ / '__||_ \
 *  \ V / (_| | | |_____| |_) | | |_| | (_| | | | | |_____| (_| |>  <| |  ___) |
 *   \_/ \__,_|_|       | .__/|_|\__,_|\__, |_|_| |_|      \__,_/_/\_\_| |____/
 *                      |_|            |___/
 *
 * Copyright (C) 2002-2004 Kai Möller
 * Copyright (C) 2004-2010 Christian Gmeiner
 *
 * This file is part of vdr-plugin-dxr3.
 *
 * vdr-plugin-dxr3 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation version 2.
 *
 * vdr-plugin-dxr3 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with dxr3-plugin.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <sys/ioctl.h>

#include "dxr3device.h"
#include "dxr3tools.h"
#include "dxr3osd.h"
#include "dxr3audio-alsa.h"
#include "dxr3audio-pa.h"
#include "dxr3pesframe.h"
#include "settings.h"

static const char *DEV_DXR3_OSD   = "_sp";
static const char *DEV_DXR3_VIDEO = "_mv";
static const char *DEV_DXR3_CONT  = "";

static const int TIMESTAMPS_PREBUFFER = 4500;    // 50ms

cDxr3Device::cDxr3Device() : spuDecoder(NULL), pluginOn(true), vPts(0), scrSet(false), aspectRatio(EM8300_ASPECTRATIO_4_3)
{
    claimDevices();

    switch (cSettings::instance()->audioDriver()) {
    case ALSA:
        isyslog("[dxr3-device] using alsa audio driver");
        audio = new cAudioAlsa();
        break;
#ifdef PULSEAUDIO
    case PA:
        isyslog("[dxr3-device] using PulseAudio audio driver");
        audio = new cAudioPA();
        break;
#endif
    }

    audio->openDevice();
    decoder = new cDecoder();

    // register observer
    cSettings::instance()->registerObserver(this);
}

cDxr3Device::~cDxr3Device()
{
    audio->releaseDevice();
    delete audio;
    delete decoder;

    releaseDevices();

    if (spuDecoder)
        delete spuDecoder;
}

cDxr3Device *cDxr3Device::instance()
{
    if (!inst) {
        inst = new cDxr3Device();
    }

    return inst;
}

// init static inst with NULL
cDxr3Device *cDxr3Device::inst = NULL;

int cDxr3Device::Dxr3Open(const char *name, int mode, bool report_error)
{
    const char *filename = *cDxr3Name(name, cSettings::instance()->card());
    int fd = open(filename, mode);

    if (report_error && (fd < 0)) {
        LOG_ERROR_STR(filename);
    }
    return fd;
}

void cDxr3Device::MakePrimaryDevice(bool On)
{
#if VDRVERSNUM >= 10711
    cDevice::MakePrimaryDevice(On);
#endif

    if (On) {
        new cDxr3OsdProvider();
    }
}

bool cDxr3Device::HasDecoder() const
{
    return true;
}

bool cDxr3Device::CanReplay() const
{
    return true;
}

bool cDxr3Device::SetPlayMode(ePlayMode PlayMode)
{
    cMutexLock l(&mutex);

    dsyslog("[dxr3-device] setting playmode %d", PlayMode);
    uint32_t val;

    switch (PlayMode) {
    case pmNone:
        audio->setEnabled(false);
        audio->flush();
        scrSet = false;

        // reset pts values
        vPts = 0;
        aPts = 0;

        // here we use some magic
        // set the scr into future so that the firmware/hardware
        // clears the buffers.
        CHECK(ioctl(fdControl, EM8300_IOCTL_SCR_GET, &val));
        val += TIMESTAMPS_PREBUFFER * 4;
        CHECK(ioctl(fdControl, EM8300_IOCTL_SCR_SET, &val));

        playBlackFrame(val + TIMESTAMPS_PREBUFFER);
        break;

    case pmAudioVideo:
        audio->setEnabled(true);
        break;

    default:
        dsyslog("[dxr3-device] playmode not supported");
        return false;
    }

    return true;
}

int64_t cDxr3Device::GetSTC()
{
    return (vPts << 1);
}

void cDxr3Device::TrickSpeed(int Speed)
{
    dsyslog("dxr3: device: tricspeed: %d", Speed);
}

void cDxr3Device::Clear()
{
    dsyslog("[dxr3-device] clear");
    uint32_t val = 0;

    // here we use some magic
    // set the scr into future so that the firmware/hardware
    // clears the buffers.
    CHECK(ioctl(fdControl, EM8300_IOCTL_SCR_GET, &val));
    val += TIMESTAMPS_PREBUFFER * 4;
    CHECK(ioctl(fdControl, EM8300_IOCTL_SCR_SET, &val));

    audio->flush();
    cDevice::Clear();
}

void cDxr3Device::Play()
{
    dsyslog("[dxr3-device] play");
}

void cDxr3Device::Freeze()
{
    dsyslog("[dxr3-device] freeze");
}

void cDxr3Device::Mute()
{
    audio->mute();
    cDevice::Mute();
}

void cDxr3Device::StillPicture(const uchar *Data, int Length)
{
    dsyslog("[dxr3-device] stillpciture");
    cDxr3PesFrame frame;

    // we need to check if Data points to a pes
    // frame or to non-pes data. This could be the
    // case for the radio-Plugin, which points to an
    // elementary stream.
    if (!Data || Length < TS_SIZE)
       return;

    if (Data[0] == 0x47) {
       // TS data
       cDevice::StillPicture(Data, Length);
    } else if (frame.parse(Data, Length)) {
        // PES data
    } else {
        // unknown data
        dsyslog("[dxr3-device] StillPicture: TODO %08x %08x %08x", Data[0], Data[1], Data[2]);
    }
}

bool cDxr3Device::Poll(cPoller &Poller, int TimeoutMs)
{
    Poller.Add(fdVideo, true);
    audio->poll(Poller);
    return Poller.Poll(TimeoutMs);
}

int cDxr3Device::PlayVideo(const uchar *Data, int Length)
{
    cMutexLock l(&mutex);

    cDxr3PesFrame frame;
    if (!frame.parse(Data, Length)) {
    	// if parsing failed, ignore frame
    	return Length;
    }

    uint32_t pts = frame.pts();

    if (pts == 0) {
        pts = vPts;
    } else {
        vPts = pts;
    }

    if (scrSet) {
        playVideoFrame(&frame, vPts);
    }

    return Length;
}

int cDxr3Device::PlayAudio(const uchar *Data, int Length, uchar Id)
{
    cMutexLock l(&mutex);

    cDxr3PesFrame frame;
    if (!frame.parse(Data, Length)) {
    	// if parsing failed, ignore frame
    	return Length;
    }

    uint32_t pts = frame.pts();

    if (pts == 0) {
        pts = aPts;
    } else {
        aPts = pts;
    }

    if (!scrSet && aPts != 0) {
        setScr(aPts);
        scrSet = true;
    }

    bool isAc3 = ((Id & 0xF0) == 0x80) || Id == 0xbd;

    if (!isAc3) {
        decoder->decode(&frame, audio);
    } else {
        decoder->ac3dts(&frame, audio);
    }

    return Length;
}

#if VDRVERSNUM >= 10710
void cDxr3Device::GetVideoSize(int &Width, int &Height, double &VideoAspect)
{
    uint32_t aspect;
    CHECK(ioctl(fdControl, EM8300_IOCTL_GET_ASPECTRATIO, &aspect));

    if (aspect == EM8300_ASPECTRATIO_4_3) {
        VideoAspect = 4.0 / 3.0;
    } else {
        VideoAspect = 16.0 / 9.0;
    }

    Width = horizontal;
    Height = vertical;
}
#endif

void cDxr3Device::SetVideoFormat(bool VideoFormat16_9)
{
    int ratio = EM8300_ASPECTRATIO_4_3;
    if (VideoFormat16_9) {
        ratio = EM8300_ASPECTRATIO_16_9;
    }

    int aspect;

    if (cSettings::instance()->forceLetterBox()) {
        ratio = EM8300_ASPECTRATIO_16_9;
    }

    if (Setup.VideoFormat) {
        aspect = EM8300_ASPECTRATIO_4_3;
#ifdef EM8300_IOCTL_SET_WSS
        if (cSettings::Instance().GetUseWSS()) {
            int wssmode;
            if (ratio == EM8300_ASPECTRATIO_16_9) {
                wssmode = EM8300_WSS_16_9;
            } else {
                wssmode = EM8300_WSS_OFF;
            }

            CHECK(ioctl(fdControl, EM8300_IOCTL_SET_WSS, &wssmode));
        }
#endif
    } else {
        aspect = ratio;
    }

    CHECK(ioctl(fdControl, EM8300_IOCTL_SET_ASPECTRATIO, &aspect));

    SetVideoDisplayFormat(eVideoDisplayFormat(Setup.VideoDisplayFormat));
}

void cDxr3Device::SetVolumeDevice(int Volume)
{
    audio->setVolume(Volume);
}

void cDxr3Device::SetAudioChannelDevice(int AudioChannel)
{
    audio->setAudioChannel(AudioChannel);
}

int cDxr3Device::GetAudioChannelDevice()
{
    return audio->getAudioChannel();
}

void cDxr3Device::SetDigitalAudioDevice(bool on)
{
    audio->setDigitalAudio(on);
}

cSpuDecoder *cDxr3Device::GetSpuDecoder()
{
    if (!spuDecoder && IsPrimaryDevice()) {
        spuDecoder = new cDxr3SpuDecoder();
    }
    return spuDecoder;
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
        claimDevices();
        audio->openDevice();

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
        releaseDevices();
        audio->releaseDevice();

        // disable pes packet processing
        pluginOn = false;
    }
}

void cDxr3Device::dimension(uint32_t &hor, uint32_t &ver)
{
    hor = horizontal;
    ver = vertical;
}

void cDxr3Device::setPalette(uint8_t *pal)
{
    CHECK(ioctl(fdSpu, EM8300_IOCTL_SPU_SETPALETTE, pal));
}

void cDxr3Device::writeSpu(const uint8_t* data, int length)
{
    WriteAllOrNothing(fdSpu, data, length, 1000, 10);
}

void cDxr3Device::setButton(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t palette)
{
    em8300_button_t button;

    button.color = palette >> 16;
    button.contrast = palette & 0xFFFF;
    button.top = sy;
    button.bottom = ey;
    button.left = sx;
    button.right = ex;

    CHECK(ioctl(fdSpu, EM8300_IOCTL_SPU_BUTTON, &button));
}

void cDxr3Device::clearButton()
{
    em8300_button_t button;

    // todo: why are there 1s and 2s - memset 0
    button.color = 0;
    button.contrast = 0;
    button.top = 1;
    button.bottom = 2;
    button.left = 1;
    button.right = 2;

    CHECK(ioctl(fdSpu, EM8300_IOCTL_SPU_BUTTON, &button))
}

void cDxr3Device::settingsChange(SettingsChange change)
{
    if (change == BCS) {
        // update bcs value
        bcs.brightness = cSettings::instance()->brightness();
        bcs.contrast = cSettings::instance()->contrast();
        bcs.saturation = cSettings::instance()->saturation();

        CHECK(ioctl(fdControl, EM8300_IOCTL_SETBCS, &bcs));
    }
}

void cDxr3Device::claimDevices()
{
    // open control stream
    fdControl = Dxr3Open(DEV_DXR3_CONT, O_WRONLY | O_SYNC);
    if (fdControl < 0) {
        esyslog("dxr3: please verify that the em8300 modules are loaded");
        exit(1);
    }

    uploadFirmware();

    ///< open multimedia streams
    fdVideo = Dxr3Open(DEV_DXR3_VIDEO, O_WRONLY | O_SYNC);
    fdSpu = Dxr3Open(DEV_DXR3_OSD, O_WRONLY | O_SYNC);

    // everything ok?
    if (fdVideo < 0 || fdSpu < 0) {
        esyslog("dxr3: fatal: unable to open some em8300 devices");
        exit(1);
    }

    // get bcs values from driver
    CHECK(ioctl(fdControl, EM8300_IOCTL_GETBCS, &bcs));

    setPlayMode();
    playBlackFrame(TIMESTAMPS_PREBUFFER);
}

void cDxr3Device::releaseDevices()
{
    close(fdControl);
    close(fdVideo);
    close(fdSpu);

    fdControl = fdVideo = fdSpu = -1;
}

void cDxr3Device::uploadFirmware()
{
    if (!cSettings::instance()->loadFirmware()) {
        return;
    }

    dsyslog("[dxr3-interface] loading firmware");

    // try to open it
    // MICROCODE comes from makefile
    int UCODE = open(MICROCODE, O_RDONLY);

    if (UCODE < 0) {
        esyslog("dxr3: fatal: unable to open microcode file %s", MICROCODE);
        exit(1);
    }

    struct stat s;
    if (fstat(UCODE, &s ) <0) {
        esyslog("dxr3: fatal: unable to fstat microcode file %s", MICROCODE);
        exit(1);
    }

    // read microcode
    em8300_microcode_t em8300_microcode;
    em8300_microcode.ucode = new char[s.st_size];
    if (em8300_microcode.ucode == NULL) {
        esyslog("dxr3: fatal: unable to malloc() space for microcode");
        exit(1);
    }

    if (read(UCODE,em8300_microcode.ucode,s.st_size) < 1) {
        esyslog("dxr3: fatal: unable to read microcode file %s", MICROCODE);
        // free memory to avoid memory leak
        delete[] (char*) em8300_microcode.ucode;
        exit(1);
    }

    close(UCODE);
    em8300_microcode.ucode_size = s.st_size;

    // upload it
    if( ioctl(fdControl, EM8300_IOCTL_INIT, &em8300_microcode) == -1) {
        esyslog("dxr3: fatal: microcode upload failed: %m");
        // free memory to avoid memory leak
        delete [] (char*) em8300_microcode.ucode;
        exit(1);
    }

    // free memory to avoid memory leak
    delete [] (char*) em8300_microcode.ucode;
}

void cDxr3Device::setPlayMode()
{
    uint32_t val = 0;
    CHECK(ioctl(fdControl, EM8300_IOCTL_SCR_SET, &val));

    int ioval = EM8300_PLAYMODE_PLAY;
    CHECK(ioctl(fdControl, EM8300_IOCTL_SET_PLAYMODE, &ioval));
    writeRegister(0, MVCOMMAND_SYNC);
}

void cDxr3Device::playVideoFrame(cDxr3PesFrame *frame, uint32_t pts)
{
    if (pts > 0) {
        uint32_t val = pts - offset;
        CHECK(ioctl(fdVideo, EM8300_IOCTL_VIDEO_SETPTS, &val));
    }

    // apply aspect ratio
    uint32_t ratio = frame->aspectRatio();

    if (aspectRatio != ratio) {
    	if (ratio == EM8300_ASPECTRATIO_16_9)
    		SetVideoFormat(true);
    	else
    		SetVideoFormat(false);

    	aspectRatio = ratio;
    }

    // write video data
    const uint8_t *data = frame->payload();
    uint32_t len = frame->payloadSize();

    WriteAllOrNothing(fdVideo, data, len, 1000, 10);
}

void cDxr3Device::playBlackFrame(uint32_t pts)
{
    extern char blackframe[];
    extern int blackframeLength;

    CHECK(ioctl(fdVideo, EM8300_IOCTL_VIDEO_SETPTS, &pts));

    for (int i = 0; i < 3; i++) {
        WriteAllOrNothing(fdVideo, (const uchar*)blackframe, blackframeLength, 1000, 10);
    }

    horizontal = 720;
    vertical = 576;
}

void cDxr3Device::writeRegister(int reg, int value)
{
    em8300_register_t regs;

    regs.microcode_register = 1;
    regs.reg = reg;
    regs.val = value;

    CHECK(ioctl(fdControl, EM8300_IOCTL_WRITEREG, &regs));
}

void cDxr3Device::setScr(uint32_t val)
{
    uint32_t scr;
    CHECK(ioctl(fdControl, EM8300_IOCTL_SCR_GET, &scr));
    offset = val - scr - TIMESTAMPS_PREBUFFER;
}

uint32_t cDxr3Device::getScr()
{
    uint32_t scr;
    CHECK(ioctl(fdControl, EM8300_IOCTL_SCR_GET, &scr));
    scr += offset;

    return scr;
}

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// tab-width: 4;
// indent-tabs-mode: nil
// End:
