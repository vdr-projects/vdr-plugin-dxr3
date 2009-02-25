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

#include <string>
using namespace std;

#include "dxr3audio-alsa.h"
#include "dxr3configdata.h"

void cAudioAlsa::openDevice()
{
    // generate alsa card name
    int card = cDxr3ConfigData::Instance().GetDxr3Card();
    string cardname = "EM8300";

    if (card > 0) {
        cardname.append("_" + card);
    }
    string device = "default:CARD=" + cardname;


    releaseDevice();

    dsyslog("[dxr3-audio-alsa] opening device %s", device.c_str());
    int err = snd_pcm_open(&handle, device.c_str(), SND_PCM_STREAM_PLAYBACK, 0);

    if (err < 0) {
        esyslog("[dxr3-audio-alsa] Playback open error: %s", snd_strerror(err));
        exit(1);
    }
}

void cAudioAlsa::releaseDevice()
{
    if (handle) {
        snd_pcm_drain(handle);
        snd_pcm_close(handle);
        handle = NULL;
    }
}

void cAudioAlsa::setup(SampleContext ctx)
{
    // look if ctx is different
    if (curContext.channels == ctx.channels && curContext.samplerate == ctx.samplerate) {
        return;
    }

    dsyslog("[dxr3-audio-alsa] changing samplerate to %d (old %d) ", ctx.samplerate, curContext.samplerate);
    dsyslog("[dxr3-audio-alsa] changing num of channels to %d (old %d)", ctx.channels, curContext.channels);

    snd_pcm_hw_params_t* alsa_hwparams;
    snd_pcm_hw_params_alloca(&alsa_hwparams);

    int err = snd_pcm_hw_params_any(handle, alsa_hwparams);
    if (err < 0) {
        esyslog("[dxr3-audio-alsa] Broken config for this PCM: no configurations available");
        exit(1);
    }

    // set access type
    err = snd_pcm_hw_params_set_access(handle, alsa_hwparams, SND_PCM_ACCESS_RW_INTERLEAVED);
    if (err < 0) {
        esyslog("[dxr3-audio-alsa] Unable to set access type: %s", snd_strerror(err));
    }

    // set format
    err = snd_pcm_hw_params_set_format(handle, alsa_hwparams, SND_PCM_FORMAT_S16_LE);
    if (err < 0) {
        esyslog("[dxr3-audio-alsa] Unable to set format: %s", snd_strerror(err));
    }

    // set channels
    err = snd_pcm_hw_params_set_channels(handle, alsa_hwparams, ctx.channels);
    if (err < 0) {
        esyslog("[dxr3-audio-alsa] Unable to set channels %d: %s", ctx.channels, snd_strerror(err));
    }

    // set samplerate
    err = snd_pcm_hw_params_set_rate_near(handle, alsa_hwparams, &ctx.samplerate, NULL);
    if (err < 0) {
        esyslog("[dxr3-audio-alsa] Unable to set samplerate %d: %s", ctx.samplerate, snd_strerror(err));
    }

    if (snd_pcm_state(handle) == SND_PCM_STATE_RUNNING) {
        if ((err = snd_pcm_drain(handle)) < 0) {
            esyslog("[dxr3-audio-alsa] Cannot drain (%s); will try to set parameters anyway\n", snd_strerror(err));
        }
    }

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

    curContext.channels = ctx.channels;
    curContext.samplerate = ctx.samplerate;

    bytesFrame = snd_pcm_format_physical_width(SND_PCM_FORMAT_S16_LE) / 8;
    bytesFrame *= curContext.channels;
}

void cAudioAlsa::write(uchar* data, size_t size)
{
    snd_pcm_uframes_t frames = size / bytesFrame;

    if (frames == 0) {
        dsyslog("[dxr3-audio-alsa] no frames");
        return;
    }

    uchar *output_samples = data;
    snd_pcm_sframes_t res = 0;

    while (frames > 0) {
        res = snd_pcm_writei(handle, output_samples, frames);

        if (res == -EAGAIN) {
            snd_pcm_wait(handle, 500);
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

void cAudioAlsa::setAudioMode(AudioMode mode)
{
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

