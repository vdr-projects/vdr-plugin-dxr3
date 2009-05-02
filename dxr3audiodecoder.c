/*
 * dxr3audiodecoder.c
 *
 * Copyright (C) 2002-2004 Kai Möller
 * Copyright (C) 2004 Christian Gmeiner
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

    audioSynched = false;

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

    lastHeader[0] = 0xFF;
    lastHeader[1] = lastHeader[2] = lastHeader[3] = 0;
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

    foundHeader = false;
	decodeAudio = true;

	//lastHeader[0] = 0xFF;
	//lastHeader[1] = lastHeader[2] = lastHeader[3] = 0;
}

// ==================================
//! decode given buffer
void cDxr3AudioDecoder::Decode(cDxr3PesFrame *frame, uint32_t pts, cDxr3SyncBuffer &aBuf)
{
    int len;
    int out_size = AVCODEC_MAX_AUDIO_FRAME_SIZE;

    enum audioException
    {
	WRONG_LENGTH,
    };

    const uint8_t *buf = frame->GetPayload();
    int length = frame->GetPayloadLength();

    int i = 0;
    for (i = 0; i < length-4 && !foundHeader; i++)
    {
	unsigned int tempHead = *((unsigned int*)(buf+i));
	if (HeadCheck(tempHead))
	{
	    if ((buf[i+2] & 0xFC) != (lastHeader[2] & 0xFC))
	    {
		dsyslog("dxr3: audiodecoder: found different audio header"
			" (new: %#x, old: %#x), (re)initializing",
			*((uint32_t*) lastHeader), *((uint32_t*) (buf+i)));

		Init();
		lastHeader[0] = buf[i];
		lastHeader[1] = buf[i+1];
		lastHeader[2] = buf[i+2];
		lastHeader[3] = buf[i+3];
	    }
	    foundHeader = true;
	}
    }

    if (audioSynched)
    {
	// no header found
	decodeAudio = true;
    }
    else
    {
	if (foundHeader && pts)
	{
	    decodeAudio = true;
	    audioSynched = true;
	}
    }

    try
    {
	while (length > 0 && decodeAudio)
	{
#if LIBAVCODEC_VERSION_INT < ((51<<16)+(29<<8)+0)
	    len = avcodec_decode_audio(
#else
	    len = avcodec_decode_audio2(
#endif
		contextAudio, (short *)(&pcmbuf), &out_size,
		const_cast<uint8_t *>(buf), length);
	    if (len < 0 || out_size < 0)
		throw WRONG_LENGTH;

	    if (out_size)
	    {
		cFixedLengthFrame* pTempFrame = aBuf.Push(pcmbuf,
							  out_size, pts);
		if (pTempFrame)
		{
		    // TODO: should we break out of the loop on push timeout?
		    pTempFrame->SetChannelCount(contextAudio->channels);
		    pTempFrame->SetSampleRate(contextAudio->sample_rate);
		}
	    }
	    length -= len;
	    buf += len;
	}
    }
    catch (audioException ex)
    {
	switch (ex)
	{
	case WRONG_LENGTH:
	    esyslog("dxr3: audiodecoder: wrong length");
	    break;

	default:
	    esyslog("dxr3: audiodecoder: unexpected exception");
	    break;
	}
	esyslog("dxr3: audiodecoder: skipping %d broken data bytes", length);

	Init();
    }
}

// ==================================
//! decode lpcm
void cDxr3AudioDecoder::DecodeLpcm(cDxr3PesFrame *frame, uint32_t pts, cDxr3SyncBuffer &aBuf)
{
    const uint8_t *buf = frame->GetPayload();
    int length = frame->GetPayloadLength();

    if (length > (LPCM_HEADER_LENGTH + 2))
    {
	// only even number of bytes are allowed
	if ((length - LPCM_HEADER_LENGTH) % 2 != 0)
	{
	    esyslog("dxr3: audiodecoder: skipping %d lpcm bytes", length);
	    return;
	}

	uint8_t* pFrame = new uint8_t[length - LPCM_HEADER_LENGTH];
	for (int i = LPCM_HEADER_LENGTH; i < length; i += 2)
	{
	    pFrame[i - LPCM_HEADER_LENGTH] = buf[i + 1];
	    pFrame[i - LPCM_HEADER_LENGTH + 1] = buf[i];
	}

	int codedSpeed = (buf[5] >> 4) & 0x03;
	int speed = 0;

	switch (codedSpeed)
	{
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

	cFixedLengthFrame* pTempFrame = aBuf.Push(pFrame,
						  length - LPCM_HEADER_LENGTH,
						  pts);
	if (pTempFrame)
	{
	    pTempFrame->SetChannelCount(1);
	    pTempFrame->SetSampleRate(speed);
	}
	delete[] pFrame;
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
    while ((pFrame = rbuf.Get()))
    {
	if (pFrame && pFrame->Count())
	{
	    cDxr3PesFrame tempPes;
	    tempPes.parse(pFrame->Data(), pFrame->Count());
	    int pesHeaderLength = (int) (tempPes.GetPayload() - tempPes.GetPesStart());
	    uint8_t* pData = pFrame->Data() + pesHeaderLength + LPCM_HEADER_LENGTH;

	    for (int i = 0; i < pFrame->Count() - pesHeaderLength - LPCM_HEADER_LENGTH; i += 2)
	    {
		std::swap(pData[i], pData[i + 1]);
	    }

	    aBuf.Push(pFrame->Data() + pesHeaderLength + LPCM_HEADER_LENGTH, pFrame->Count() - pesHeaderLength - 7, tempPes.GetPts());
	    if (pFrame) rbuf.Drop(pFrame);
	}
    }
}

// ==================================
//! checking routine
bool cDxr3AudioDecoder::HeadCheck(unsigned long head)
{
    bool retval = false;

    uint8_t* phead = (uint8_t*) (&head);
    if (phead[0] != 0xFF)
    {
	retval = false;
    }
    else if (phead[1] != 0xFC && phead[1] != 0xFE)
    {
	retval = false;
    }
    else if ((phead[2] & 0xF0) == 0xF0)
    {
	retval = false;
    }
    else if ((phead[2] & 0xC) == 0xC)
    {
	retval = false;
    }
    else
    {
	retval = true;
    }

    return retval;
}

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
