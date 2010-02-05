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

#ifndef _AUDIO_OSS_H_
#define _AUDIO_OSS_H_

#include "dxr3audio.h"

class cAudioOss : public iAudio {
public:
    cAudioOss() : iAudio(), fd(-1) {}
    ~cAudioOss() {}

    virtual void openDevice();
    virtual void releaseDevice();
    virtual void setup(int channels, int samplerate);
    virtual void write(uchar* data, size_t size);
    virtual void poll(cPoller &poller);
    virtual void setDigitalAudio(bool on);

private:
    int fd;
};

#endif /*_AUDIO_OSS_H_*/
