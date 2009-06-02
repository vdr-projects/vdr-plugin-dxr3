/*
 *
 * Copyright (C) 2009 Christian Gmeiner
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

#include <sys/soundcard.h>
#include <unistd.h> // for close

#include "dxr3audio-oss.h"
#include "dxr3interface.h"

static const char *DEV_DXR3_OSS   = "_ma";

void cAudioOss::openDevice()
{
    if (open)
        return;

    fd = cDxr3Interface::Dxr3Open(DEV_DXR3_OSS, O_RDWR | O_NONBLOCK);

    if (!fd) {
        esyslog("[dxr3-audio-oss] failed to open dxr3 audio subdevice");
        exit(1);
    }

    if (cDxr3ConfigData::instance()->GetUseDigitalOut()) {
        dsyslog("[dxr3-audio-oss] audio mode: digital");
        setAudioMode(DigitalPcm);
    } else {
        dsyslog("[dxr3-audio-oss] audio mode: analog");
        setAudioMode(Analog);
    }

    open = true;
}

void cAudioOss::releaseDevice()
{
    if (!open)
        return;

    close(fd);

    open = false;
}

void cAudioOss::setup(const SampleContext& ctx)
{
    // set sample rate
    if (curContext.samplerate != ctx.samplerate) {
        dsyslog("[dxr3-audio-oss] changing samplerate to %d (old %d) ", ctx.samplerate, curContext.samplerate);
        curContext.samplerate = ctx.samplerate;
        CHECK( ioctl(fd, SNDCTL_DSP_SAMPLESIZE, &ctx.samplerate));
    }

    // set channels
    if (curContext.channels != ctx.channels) {
        dsyslog("[dxr3-audio-oss] changing num of channels to %d (old %d)", ctx.channels, curContext.channels);
        curContext.channels = ctx.channels;
        CHECK( ioctl(fd, SNDCTL_DSP_CHANNELS, &ctx.channels));
    }
}

void cAudioOss::write(uchar* data, size_t size)
{
    size_t ret = WriteAllOrNothing(fd, data, size, 1000, 10);

    if (ret != size) {
        dsyslog("[dxr3-audio-oss] writing audio failed");
    }
}

void cAudioOss::setAudioMode(AudioMode mode)
{
    uint32_t ioval = 0;

    switch (mode) {
    case Analog:
        ioval = EM8300_AUDIOMODE_ANALOG;
        break;

    case DigitalPcm:
        ioval = EM8300_AUDIOMODE_DIGITALPCM;
        break;

    case Ac3:
        ioval = EM8300_AUDIOMODE_DIGITALAC3;
        break;
    }

    // we need to do it this way, as we dont have access
    // to the file handle for the conrtol sub device.
    if (cDxr3Interface::instance()->OssSetPlayMode(ioval)) {
        this->mode = mode;
    }
}
