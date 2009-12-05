/*
 * dxr3audiodecoder.c
 *
 * Copyright (C) 2002-2004 Kai Möller
 * Copyright (C) 2004-2009 Christian Gmeiner
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


/*
  ToDo:
  - cDxr3AudioDecoder::Init: Why are we always reinit the codec?
*/

#include <algorithm>
#include "dxr3audiodecoder.h"
#include "dxr3pesframe.h"

// ==================================
const int LPCM_HEADER_LENGTH = 7;

// ==================================
//! constructor
cDxr3AudioDecoder::cDxr3AudioDecoder() : rbuf(50000), ac3dtsDecoder(&rbuf)
{
    // setup ffmpeg
    avcodec_init();
    avcodec_register_all();

    // look for decoder
    audio = avcodec_find_decoder(CODEC_ID_MP3);

    if (!audio) {
        esyslog("[dxr3-decoder] no suitable audio codec found.");
        esyslog("[dxr3-decoder] check your ffmpeg installation.");
        exit(-1);
    }

    // create a new codec context
    contextAudio = avcodec_alloc_context();
    int ret = avcodec_open(contextAudio, audio);

    if (ret < 0) {
        esyslog("[dxr3-decoder] failed to open codec %s.", audio->name);
        exit(-1);
    }

    lastBitrate = 0xff; // init with an invalid value - see checkMpegAudioHdr;
}

// ==================================
//! deconst.
cDxr3AudioDecoder::~cDxr3AudioDecoder()
{
    // close codec, if it is open
    avcodec_close(contextAudio);
}

// ==================================
//! (re)init ffmpeg codec
void cDxr3AudioDecoder::Init()
{
    avcodec_close(contextAudio);

    // create a new codec context
    contextAudio = avcodec_alloc_context();
    int ret = avcodec_open(contextAudio, audio);

    if (ret < 0) {
        esyslog("[dxr3-decoder] failed to open codec %s.", audio->name);
        exit(-1);
    }
}

// ==================================
//! decode given buffer
void cDxr3AudioDecoder::Decode(cDxr3PesFrame *frame, uint32_t pts, cDxr3SyncBuffer &aBuf)
{
    int len;
    int out_size = AVCODEC_MAX_AUDIO_FRAME_SIZE;

    const uint8_t *buf = frame->GetPayload();
    int length = frame->GetPayloadLength();

    if (checkMpegAudioHdr(buf)) {

        // look if Bitrate has changed
        if ((buf[2] & 0xf0) != (lastBitrate & 0xf0)) {
            dsyslog("[dxr3-audiodecoder] found new audio header");

            // we need now to reinit the deocder and to store the new
            // part from the audio header
            Init();
            lastBitrate = buf[2];
        }
    }

#if LIBAVCODEC_VERSION_INT < AV_VERSION_INT(51, 29, 0)
    len = avcodec_decode_audio(
#else
    len = avcodec_decode_audio2(
#endif
    contextAudio, (short *)(&pcmbuf), &out_size, const_cast<uint8_t *>(buf), length);

    if (len < 0) {
        esyslog("[dxr3-decoder] failed to decode audio");
        return;
    }

    // can this happen?
    if ((length - len) > 0) {
        esyslog("[dxr3-decoder] TODO: more to decode");
    }

    if (out_size) {
        cFixedLengthFrame* pTempFrame = aBuf.Push(pcmbuf,
                              out_size, pts);
        if (pTempFrame) {
            // TODO: should we break out of the loop on push timeout?
            pTempFrame->SetChannelCount(contextAudio->channels);
            pTempFrame->SetSampleRate(contextAudio->sample_rate);
        }
    }
}

// ==================================
//! decode lpcm
void cDxr3AudioDecoder::DecodeLpcm(cDxr3PesFrame *frame, uint32_t pts, cDxr3SyncBuffer &aBuf)
{
    const uint8_t *buf = frame->GetPayload();
    int length = frame->GetPayloadLength();

    // all informations about the LPCM header can be found
    // here http://dvd.sourceforge.net/dvdinfo/lpcm.html

    if (length > (LPCM_HEADER_LENGTH + 2)) {
        // only even number of bytes are allowed
        if ((length - LPCM_HEADER_LENGTH) % 2 != 0) {
            esyslog("[dxr3-audiodecoder] skipping %d lpcm bytes", length);
            return;
        }

        // remove header and set pointer to first data byte
        const uint8_t *data = buf + LPCM_HEADER_LENGTH;
        length -= LPCM_HEADER_LENGTH;

        int codedSpeed = (buf[5] >> 4) & 0x03;
        int speed = 0;

        switch (codedSpeed) {
        case 1:
            speed = 96000;
            break;

        case 2:
            speed = 44100;
            break;

        case 3:
            speed = 32000;
            break;

        default:
            speed = 48000;
            break;
        }

        int channels = (buf[5] & 0x03);
        channels++;

        cFixedLengthFrame* pTempFrame = aBuf.Push(data, length, pts);
        if (pTempFrame) {
            pTempFrame->SetChannelCount(channels);
            pTempFrame->SetSampleRate(speed);
        }
    }
}

// ==================================
//! decode ac3
void cDxr3AudioDecoder::DecodeAc3Dts(const uint8_t* pPes, const uint8_t* buf,
				     int length, uint32_t pts,
				     cDxr3SyncBuffer &aBuf)
{
    int headerLength = (int) (buf - pPes);

    uint8_t* pBuf = (uint8_t*) pPes;
    ac3dtsDecoder.Check(pBuf + headerLength, length, pBuf);
    ac3dtsDecoder.Encapsulate(pBuf + headerLength, length);

    cFrame* pFrame = 0;
    while ((pFrame = rbuf.Get())) {
        if (pFrame && pFrame->Count()) {
            cDxr3PesFrame tempPes;
            tempPes.parse(pFrame->Data(), pFrame->Count());
            int pesHeaderLength = (int) (tempPes.GetPayload() - tempPes.GetPesStart());
            uint8_t* pData = pFrame->Data() + pesHeaderLength + LPCM_HEADER_LENGTH;

            for (int i = 0; i < pFrame->Count() - pesHeaderLength - LPCM_HEADER_LENGTH; i += 2) {
                std::swap(pData[i], pData[i + 1]);
            }

            aBuf.Push(pFrame->Data() + pesHeaderLength + LPCM_HEADER_LENGTH, pFrame->Count() - pesHeaderLength - 7, tempPes.GetPts());
            if (pFrame)
                rbuf.Drop(pFrame);
        }
    }
}

/**
 * \brief decoded payload of frame
 */
void cDxr3AudioDecoder::decode(cDxr3PesFrame *frame)
{
    int len;
    int out_size = AVCODEC_MAX_AUDIO_FRAME_SIZE;

    const uint8_t *buf = frame->GetPayload();
    int length = frame->GetPayloadLength();

#if LIBAVCODEC_VERSION_INT < ((51<<16)+(29<<8)+0)
    len = avcodec_decode_audio(
#else
    len = avcodec_decode_audio2(
#endif
    contextAudio, frame->decoded, &out_size, const_cast<uint8_t *>(buf), length);

    if (len < 0) {
        esyslog("[dxr3-decoder] failed to decode audio");
        frame->decodedSize = 0;
        return;
    }

    // can this happen?
    if ((length - len) > 0) {
        esyslog("[dxr3-decoder] TODO: more to decode");
    }

    frame->decodedSize = len;
    frame->ctx.channels = contextAudio->channels;
    frame->ctx.samplerate = contextAudio->sample_rate;
}

// ==================================
//! checking routine
bool cDxr3AudioDecoder::checkMpegAudioHdr(const uint8_t *head)
{
    // all informations about the mpeg audio header
    // can be found at http://www.datavoyage.com/mpgscript/mpeghdr.htm

    // the header conists of four 8bit elements, described as
    // AAAAAAAA AAABBCCD EEEEFFGH IIJJKLMM

    // all As must be set to 1
    if (head[0] != 0xff || (head[1] & 0xe0) != 0xe0) {
        return false;
    }

    // B is not allowed to be 01 (bits), as it is a reserved value
    if ((head[1] & 0x18) == 0x8) {
        return false;
    }

    // C is not allowed to be 00 (bits), as it is a reserved value
    if ((head[1] & 0x6) == 0x0) {
        return false;
    }

    // all Es are not allowed to be 1
    if ((head[2] & 0xf0) == 0xf0) {
        return false;
    }

    // all Fs are not allowed to be 1
    if ((head[2] & 0xc) == 0xc) {
        return false;
    }

    return true;
}

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// tab-width: 4;
// indent-tabs-mode: nil
// End:
