/*
 *
 * Copyright (C) 2010 Christian Gmeiner
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

#ifndef _AUDIO_PA_H_
#define _AUDIO_PA_H_

#ifdef PULSEAUDIO

#include "dxr3audio.h"

#include <pulse/simple.h>

class cAudioPA : public iAudio {
public:
    cAudioPA() : iAudio(), s(NULL) {}

    virtual void openDevice()       {}
    virtual void releaseDevice();
    virtual void setup(int channels, int samplerate);
    virtual void write(uchar* data, size_t size);
    virtual void flush();

    virtual void poll(cPoller &poller) {}

private:
    pa_sample_spec ss;
    pa_simple *s;
    int error;
};

#endif

#endif /*_AUDIO_PA_H_*/
