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

#ifndef _AUDIO_H_
#define _AUDIO_H_

#include <string.h>
#include <vdr/tools.h>  // for uchar

struct SampleContext {
   unsigned int channels;
   unsigned int samplerate;
};

class iAudio {
public:
    enum AudioMode {
        Analog,
        DigitalPcm,
        Ac3,
    };

    iAudio() : vol(0), audioChannel(0)  { memset(&curContext, 0, sizeof(SampleContext)); }
    virtual ~iAudio() {}

    virtual void openDevice() = 0;
    virtual void releaseDevice() = 0;
    virtual void setup(SampleContext ctx) = 0;
    virtual void write(uchar* data, size_t size) = 0;

    void setVolume(int v)   { vol = v; }
    void mute()             { setVolume(0); }

    void changeVolume(short* pcmbuf, size_t size);
    void setAudioChannel(int channel)   { audioChannel = channel; }
    int getAudioChannel()               { return audioChannel; }

    virtual void setAudioMode(AudioMode m) = 0;
    AudioMode getAudioMode()            { return mode; }

    bool isAudioModeAC3()   { return mode == Ac3; }


protected:
    int vol;
    int audioChannel;
    SampleContext curContext;
    AudioMode mode;
};

#endif /*_AUDIO_H_*/
