/*
 * dxr3audiodecoder.h
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#ifndef _DXR3_AUDIODECODER_H_
#define _DXR3_AUDIODECODER_H_

#include <stdlib.h>
#include <stdint.h>

#include "dxr3ffmpeg.h"
#include "dxr3syncbuffer.h"
#include "dxr3multichannelaudio.h"
#include "dxr3log.h"

// ==================================
// decode audio to mp2 or use DD :)
class cDxr3AudioDecoder 
{
public:
    cDxr3AudioDecoder();
    ~cDxr3AudioDecoder();

    void Init(void); // init in const?                                

    void Decode(const uint8_t* buf, int length, uint32_t pts, cDxr3SyncBuffer &aBuf);
    void DecodeLpcm(const uint8_t* buf, int length, uint32_t pts, cDxr3SyncBuffer &aBuf);
    void DecodeAc3Dts(const uint8_t* pPes, const uint8_t* buf, int length, uint32_t pts, cDxr3SyncBuffer &aBuf);

    int GetRate(void) const			{ return rate; }
    int GetChannelCount(void) const { return channels; }
    int GetFrameSize(void) const	{ return frameSize; }
    void Reset(void)				{ ac3dtsDecoder.Clear(); rbuf.Clear(); }
    
private:
    bool HeadCheck(unsigned long head);
    
	struct Dxr3Codec Codec;

    cRingBufferFrame rbuf;
    cMultichannelAudio ac3dtsDecoder;
    
    bool audioSynched;
    bool decoderOpened;
    uint8_t lastHeader[4];
    int rate;
    int channels;
    uint32_t frameSize;
    uint8_t pcmbuf[AVCODEC_MAX_AUDIO_FRAME_SIZE];
    int volume;
    bool foundHeader;
    bool decodeAudio;

    cDxr3AudioDecoder(cDxr3AudioDecoder&); // no copy constructor
};

#endif /*_DXR3_AUDIODECODER_H_*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
