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

#include <string>
using namespace std;

#include "dxr3audio-alsa.h"
#include "settings.h"

void cAudioAlsa::openDevice()
{
    if (open)
        return;

    // generate alsa card name
    int card = cSettings::instance()->card();
    string cardname = "EM8300";

    if (card > 0) {
        cardname.append("_" + card);
    }
    string device = "default:CARD=" + cardname;

    dsyslog("[dxr3-audio-alsa] opening device %s", device.c_str());
    int err = snd_pcm_open(&handle, device.c_str(), SND_PCM_STREAM_PLAYBACK, SND_PCM_NONBLOCK);

    if (err < 0) {
        esyslog("[dxr3-audio-alsa] Playback open error: %s", snd_strerror(err));
        exit(1);
    }

    open = true;
}

void cAudioAlsa::releaseDevice()
{
    if (!open)
        return;

    if (handle) {
        snd_pcm_drop(handle);
        snd_pcm_close(handle);
        handle = NULL;
    }

    open = false;
}

void cAudioAlsa::setup(int channels, int samplerate)
{
    if (!open)
        return;

    // look if ctx is different
    if (curContext.channels == channels && curContext.samplerate == samplerate) {
        return;
    }

    dsyslog("[dxr3-audio-alsa] changing samplerate to %d (old %d) ", samplerate, curContext.samplerate);
    dsyslog("[dxr3-audio-alsa] changing num of channels to %d (old %d)", channels, curContext.channels);

    snd_pcm_hw_params_t* alsa_hwparams;
    snd_pcm_sw_params_t* alsa_swparams;

    snd_pcm_hw_params_alloca(&alsa_hwparams);
    snd_pcm_sw_params_alloca(&alsa_swparams);

    //
    // set hardware settings

    int err = snd_pcm_hw_params_any(handle, alsa_hwparams);
    if (err < 0) {
        esyslog("[dxr3-audio-alsa] Broken config for this PCM: no configurations available");
        exit(1);
    }

    // set access type
    pcm_write_func = &snd_pcm_mmap_writei;
    err = snd_pcm_hw_params_set_access(handle, alsa_hwparams, SND_PCM_ACCESS_MMAP_INTERLEAVED);
    if (err < 0) {
        esyslog("[dxr3-audio-alsa] mmap not available, attempting to fall back to slow writes");

        pcm_write_func = &snd_pcm_writei;
        err = snd_pcm_hw_params_set_access(handle, alsa_hwparams, SND_PCM_ACCESS_RW_INTERLEAVED);
        if (err < 0) {
            esyslog("[dxr3-audio-alsa] Unable to set access type: %s", snd_strerror(err));
            exit(-2);
        }
    }

    // set format
    err = snd_pcm_hw_params_set_format(handle, alsa_hwparams, SND_PCM_FORMAT_S16_LE);
    if (err < 0) {
        esyslog("[dxr3-audio-alsa] Unable to set format: %s", snd_strerror(err));
    }

    // set channels
    err = snd_pcm_hw_params_set_channels(handle, alsa_hwparams, channels);
    if (err < 0) {
        esyslog("[dxr3-audio-alsa] Unable to set channels %d: %s", channels, snd_strerror(err));
    }

    unsigned int sr = samplerate;

    // set samplerate
    err = snd_pcm_hw_params_set_rate_near(handle, alsa_hwparams, &sr, NULL);
    if (err < 0) {
        esyslog("[dxr3-audio-alsa] Unable to set samplerate %d: %s", samplerate, snd_strerror(err));
    }

    static unsigned int buffer_time = 500000;               // ring buffer length in us
    static unsigned int period_time = 100000;               // period time in us
    snd_pcm_uframes_t size;

    // set the buffer time
    err = snd_pcm_hw_params_set_buffer_time_near(handle, alsa_hwparams, &buffer_time, NULL);
    if (err < 0) {
        esyslog("[dxr3-audio-alsa] Unable to set buffer time %i for playback: %s\n", buffer_time, snd_strerror(err));
    }
    err = snd_pcm_hw_params_get_buffer_size(alsa_hwparams, &size);
    if (err < 0) {
        esyslog("[dxr3-audio-alsa] Unable to get buffer size for playback: %s\n", snd_strerror(err));
    }
    snd_pcm_sframes_t buffer_size = size;

    // set the period time
    err = snd_pcm_hw_params_set_period_time_near(handle, alsa_hwparams, &period_time, NULL);
    if (err < 0) {
        esyslog("[dxr3-audio-alsa] Unable to set period time %i for playback: %s\n", period_time, snd_strerror(err));
    }
    err = snd_pcm_hw_params_get_period_size(alsa_hwparams, &size, NULL);
    if (err < 0) {
        esyslog("[dxr3-audio-alsa] Unable to get period size for playback: %s\n", snd_strerror(err));
    }
    snd_pcm_sframes_t period_size = size;

    // set hardware pararmeters
    err = snd_pcm_hw_params(handle, alsa_hwparams);
    if (err < 0) {
        esyslog("[dxr3-audio-alsa] Unable to set hw pararmeters: %s", snd_strerror(err));
    }

    // prepare for playback
    err = snd_pcm_prepare(handle);
    if (err < 0) {
        esyslog("[dxr3-audio-alsa] Cannot prepare audio interface for use: %s", snd_strerror(err));
    }

    //
    // set software settings
    err = snd_pcm_sw_params_current(handle, alsa_swparams);
    if (err < 0) {
        esyslog("[dxr3-audio-alsa] Cannot get current sw params: %s", snd_strerror(err));
    }

    // start the transfer when the buffer is almost full: */
    // (buffer_size / avail_min) * avail_min */
    err = snd_pcm_sw_params_set_start_threshold(handle, alsa_swparams, (buffer_size / period_size) * period_size);
    if (err < 0) {
        esyslog("[dxr3-audio-alsa] Unable to set start threshold mode for playback: %s\n", snd_strerror(err));
    }

    // allow the transfer when at least period_size samples can be processed */
    // or disable this mechanism when period event is enabled (aka interrupt like style processing) */
    err = snd_pcm_sw_params_set_avail_min(handle, alsa_swparams, buffer_size);
    if (err < 0) {
        esyslog("[dxr3-audio-alsa] Unable to set avail min for playback: %s\n", snd_strerror(err));
    }

    snd_pcm_uframes_t boundary;
#if SND_LIB_VERSION >= 0x000901
    err = snd_pcm_sw_params_get_boundary(alsa_swparams, &boundary);
    if (err < 0) {
        esyslog("[dxr3-audio-alsa] Failed to get boundary: %s", snd_strerror(err));
    }
#else
    boundary = 0x7fffffff;
#endif

    // disable underrun reporting
    err = snd_pcm_sw_params_set_stop_threshold(handle, alsa_swparams, boundary);
    if (err < 0) {
        esyslog("[dxr3-audio-alsa] Failed to disable underrun reporting: %s", snd_strerror(err));
    }

#if SND_LIB_VERSION >= 0x000901
    // play silence when there is an underrun
    err = snd_pcm_sw_params_set_silence_size(handle, alsa_swparams, boundary);
    if (err < 0) {
        esyslog("[dxr3-audio-alsa] Failed to enable silence mode: %s", snd_strerror(err));
    }
#endif

    err = snd_pcm_sw_params(handle, alsa_swparams);
    if (err < 0) {
        esyslog("[dxr3-audio-alsa] Failed to set sw params: %s", snd_strerror(err));
    }

    curContext.channels = channels;
    curContext.samplerate = samplerate;

    bytesFrame = snd_pcm_format_physical_width(SND_PCM_FORMAT_S16_LE) / 8;
    bytesFrame *= curContext.channels;
}

void cAudioAlsa::write(uchar* data, size_t size)
{
    if (!open || !enabled)
        return;

    snd_pcm_uframes_t frames = size / bytesFrame;

    if (frames == 0) {
        dsyslog("[dxr3-audio-alsa] no frames");
        return;
    }

    uchar *output_samples = data;
    snd_pcm_sframes_t res = 0;

    while (frames > 0) {
        res = pcm_write_func(handle, output_samples, frames);

        if (res == -EAGAIN) {
            snd_pcm_wait(handle, 10);
        } else  if (res == -EINTR) {
            // nothing to do
            res = 0;
        } else  if (res == -EPIPE) {
            Xrun();
        } else if (res == -ESTRPIPE) {
            // suspend
            dsyslog("[dxr3-audio-alsa] pcm in suspend");
            while ((res = snd_pcm_resume(handle)) == -EAGAIN) {
                sleep(1);
            }
        }

        if (res > 0) {
            output_samples += res * bytesFrame;
            frames -= res;
        }
    }
}

void cAudioAlsa::flush()
{
    snd_pcm_nonblock(handle, 0);
    int err = snd_pcm_drain(handle);
    if (err < 0) {
        esyslog("[dxr3-audio-alsa] failed to pcm_drop: %s", snd_strerror(err));
    }
    snd_pcm_nonblock(handle, 1);

    err = snd_pcm_prepare(handle);
    if (err < 0) {
        esyslog("[dxr3-audio-alsa] failed to pcm_prepare: %s", snd_strerror(err));
    }
}

void cAudioAlsa::setDigitalAudio(bool on)
{
    if (digitalAudio == on) {
        return;
    }

    // TODO stub
}

void cAudioAlsa::Xrun()
{
    int res;
    snd_pcm_status_alloca(&status);

    dsyslog("[dxr3-audio-alsa] Xrun");

    res = snd_pcm_status(handle, status);
    if (res < 0) {
        esyslog("[dxr3-audio-alsa]: Xrun status error: %s FATAL exiting", snd_strerror(res));
        exit(EXIT_FAILURE);
    }

    if (snd_pcm_status_get_state(status) == SND_PCM_STATE_XRUN) {

        res = snd_pcm_prepare(handle);
        if (res < 0) {
            esyslog("[dxr3-audio-alsa]: Xrun prepare error: %s FATAL exiting", snd_strerror(res));
            exit(EXIT_FAILURE);
        }
        return;         // ok, data should be accepted again
    }

    esyslog("[dxr3-audio-alsa]: read/write error FATAL exiting");
    exit(-1);
}
