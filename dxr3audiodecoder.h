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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef _DXR3_AUDIODECODER_H_
#define _DXR3_AUDIODECODER_H_

extern "C" {
#    include <libavcodec/avcodec.h>
}

#include "dxr3syncbuffer.h"
#include "dxr3multichannelaudio.h"
#include "Uncopyable.h"

class cDxr3PesFrame;

// ==================================
// decode audio to mp2 or use DD :)
class cDxr3AudioDecoder : private Uncopyable {
public:
    cDxr3AudioDecoder();
    ~cDxr3AudioDecoder();

    void Init();

    void Decode(cDxr3PesFrame *frame, uint32_t pts, cDxr3SyncBuffer &aBuf);
    void DecodeLpcm(cDxr3PesFrame *frame, uint32_t pts, cDxr3SyncBuffer &aBuf);
    void DecodeAc3Dts(const uint8_t* pPes, const uint8_t* buf, int length,
		      uint32_t pts, cDxr3SyncBuffer &aBuf);


    void decode(cDxr3PesFrame *frame);

    void Reset()
    {
    	ac3dtsDecoder.Clear();
    	rbuf.Clear();
    }

private:
    bool HeadCheck(unsigned long head);

    AVCodec *audio;
    AVCodecContext *contextAudio;

    cRingBufferFrame rbuf;
    cMultichannelAudio ac3dtsDecoder;

    bool audioSynched;
    bool foundHeader;
    bool decodeAudio;

    uint8_t lastHeader[4];
    uint8_t pcmbuf[AVCODEC_MAX_AUDIO_FRAME_SIZE];
};

#endif /*_DXR3_AUDIODECODER_H_*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
