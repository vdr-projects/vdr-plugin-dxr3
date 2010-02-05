/*
 *
 * Copyright (C) 2009 Christian Gmeiner
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

#include <sys/soundcard.h>
#include <unistd.h> // for close
#include <sys/ioctl.h>

#include "dxr3audio-oss.h"
#include "dxr3device.h"

static const char *DEV_DXR3_OSS   = "_ma";

void cAudioOss::openDevice()
{
    if (open)
        return;

    fd = cDxr3Device::Dxr3Open(DEV_DXR3_OSS, O_RDWR | O_NONBLOCK);

    if (!fd) {
        esyslog("[dxr3-audio-oss] failed to open dxr3 audio subdevice");
        exit(1);
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
    if (!open)
        return;

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
    if (!open || !enabled)
        return;

    size_t ret = WriteAllOrNothing(fd, data, size, 1000, 10);

    if (ret != size) {
        dsyslog("[dxr3-audio-oss] writing audio failed");
    }
}

void cAudioOss::poll(cPoller &poller)
{
    poller.Add(fd, true);
}

void cAudioOss::setDigitalAudio(bool on)
{
    if (digitalAudio == on) {
        return;
    }

    uint32_t ioval = 0;

    if (on) {
        ioval = EM8300_AUDIOMODE_DIGITALPCM;
    } else {
        ioval = EM8300_AUDIOMODE_ANALOG;
    }

    // we need to do it this way, as we dont have access
    // to the file handle for the conrtol sub device.
    cDxr3Device::instance()->ossSetPlayMode(ioval);

    digitalAudio = on;
}
