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
#include "settings.h"

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

void cAudioOss::setup(int channels, int samplerate)
{
    if (!open)
        return;

    // set sample rate
    if (curContext.samplerate != samplerate) {
        dsyslog("[dxr3-audio-oss] changing samplerate to %d (old %d) ", samplerate, curContext.samplerate);
        curContext.samplerate = samplerate;
        CHECK( ioctl(fd, SNDCTL_DSP_SAMPLESIZE, &samplerate));
    }

    // set channels
    if (curContext.channels != channels) {
        dsyslog("[dxr3-audio-oss] changing num of channels to %d (old %d)", channels, curContext.channels);
        curContext.channels = channels;
        CHECK( ioctl(fd, SNDCTL_DSP_CHANNELS, &channels));
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
    if (digitalAudio == on)
        return;

    reconfigure();

    digitalAudio = on;
}

void cAudioOss::reconfigure()
{
	uint32_t ioval = 0;
	dsyslog("[dxr3-oss] reconfigure");

    // this is quite an imporant part, as we need to
    // decide how we set the audiomode of oss device.
    if (digitalAudio) {
        ioval = EM8300_AUDIOMODE_DIGITALAC3;
    } else {

    	// digital or analog pcm?
    	if (cSettings::instance()->useDigitalOut()) {
    		ioval = EM8300_AUDIOMODE_DIGITALPCM;
    	} else {
    	    ioval = EM8300_AUDIOMODE_ANALOG;
    	}
    }

    // we need to do it this way, as we dont have access
    // to the file handle for the conrtol sub device.
    cDxr3Device::instance()->ossSetPlayMode(ioval);
}
