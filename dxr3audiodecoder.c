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

#include <stdio.h>
#include "dxr3audiodecoder.h"
#include "dxr3pesframe.h"

// ==================================
const int LPCM_HEADER_LENGTH = 7;

// ==================================
//! constructor
cDxr3AudioDecoder::cDxr3AudioDecoder() : rbuf(50000), ac3dtsDecoder(&rbuf)
{
    decoderOpened = false;
    audioSynched = false;
    volume = 255;
    Codec.id = CODEC_ID_MP2;

    Init();

    lastHeader[0] = 0xFF;
    lastHeader[1] = lastHeader[2] = lastHeader[3] = 0;
};

// ==================================
//! deconst.
cDxr3AudioDecoder::~cDxr3AudioDecoder()
{
    // close codec, if it is open
    cDxr3Ffmepg::Instance().CloseCodec(Codec);
};

// ==================================
//! (re)init ffmpeg codec
void cDxr3AudioDecoder::Init()
{
    // (re)init codec
    cDxr3Ffmepg::Instance().CloseCodec(Codec);
    if (cDxr3Ffmepg::Instance().FindCodec(Codec))
    {
	cDxr3Ffmepg::Instance().OpenCodec(Codec);
	rate = channels = -1;
	frameSize = Codec.codec_context.frame_size;
	decoderOpened = true;
	foundHeader = false;
	decodeAudio = true;

	//lastHeader[0] = 0xFF;
	//lastHeader[1] = lastHeader[2] = lastHeader[3] = 0;
    }
    else
    {
	decoderOpened = false;
    }
}

// ==================================
//! decode given buffer
void cDxr3AudioDecoder::Decode(const uint8_t* buf, int length, uint32_t pts,
			       cDxr3SyncBuffer &aBuf)
{
    if (!decoderOpened)
    {
	// No decoder is open, so it
	// is better to stop here.
	return;
    }

    int len;
    int out_size;

    enum audioException
    {
	WRONG_LENGTH,
	UNEXPECTED_PARAMETER_CHANGE
    };

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
	    len = avcodec_decode_audio(&Codec.codec_context,
				       (short *)(&pcmbuf), &out_size,
				       const_cast<uint8_t *>(buf), length);
	    if (len < 0 || out_size < 0)
		throw WRONG_LENGTH;

	    if (Codec.codec_context.sample_rate != rate)
	    {
		dsyslog("dxr3: audiodecoder: sample rate=%d",
			Codec.codec_context.sample_rate);
		if (rate != -1) throw UNEXPECTED_PARAMETER_CHANGE;
		rate = Codec.codec_context.sample_rate;
	    }
	    if (Codec.codec_context.channels != channels + 1)
	    {
		dsyslog("dxr3: audiodecoder: channels=%d",
			Codec.codec_context.channels);
		if (channels != -1)
		    throw UNEXPECTED_PARAMETER_CHANGE;
		channels = (Codec.codec_context.channels == 2) ? 1 : 0;
	    }
	    if (out_size)
	    {
		cFixedLengthFrame* pTempFrame = aBuf.Push(pcmbuf,
							  out_size, pts);
		pTempFrame->SetChannelCount(channels);
		pTempFrame->SetDataRate(rate);
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

	case UNEXPECTED_PARAMETER_CHANGE:
	    esyslog("dxr3: audiodecoder: unexpected parameter change");
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
void cDxr3AudioDecoder::DecodeLpcm(const uint8_t* buf, int length,
				   uint32_t pts, cDxr3SyncBuffer &aBuf)
{
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
	pTempFrame->SetChannelCount(1);
	pTempFrame->SetDataRate(speed);

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
	    tempPes.ExtractNextFrame(pFrame->Data(), pFrame->Count());
	    int pesHeaderLength = (int) (tempPes.GetEsStart() - tempPes.GetPesStart());
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
