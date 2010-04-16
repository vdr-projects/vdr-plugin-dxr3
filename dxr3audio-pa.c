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

#ifdef PULSEAUDIO

#include "dxr3audio-pa.h"

#include <pulse/error.h>

void cAudioPA::releaseDevice()
{
    if (s) {
        pa_simple_free(s);
        s = NULL;
    }
}

void cAudioPA::setup(int channels, int samplerate)
{
    bool reopen = false;

    // set sample rate
    if (curContext.samplerate != samplerate) {
        dsyslog("[dxr3-audio-pa] changing samplerate to %d (old %d) ", samplerate, curContext.samplerate);
        curContext.samplerate = samplerate;
        ss.rate = samplerate;
        reopen = true;
    }

    // set channels
    if (curContext.channels != channels) {
        dsyslog("[dxr3-audio-pa] changing num of channels to %d (old %d)", channels, curContext.channels);
        curContext.channels = channels;
        ss.channels = channels;
        reopen = true;
    }

    if (reopen) {
        ss.format = PA_SAMPLE_S16LE;

        if (s)
            pa_simple_free(s);

        // create a new playback stream
        s = pa_simple_new(NULL, "vdr-plugin-dxr3", PA_STREAM_PLAYBACK, NULL, "playback", &ss, NULL, NULL, &error);

        if (!s) {
            // error handling
        }
    }
}

void cAudioPA::write(uchar* data, size_t size)
{
    int ret = pa_simple_write(s, data, size, &error);

    if  (ret < 0) {
        esyslog("[dxr3-audio-pa] failed to write audio data: %s", pa_strerror(error));
    }
}

void cAudioPA::flush()
{
    int ret = pa_simple_flush(s, &error);

    if  (ret < 0) {
        esyslog("[dxr3-audio-pa] failed to write audio data: %s", pa_strerror(error));
    }
}
#endif
