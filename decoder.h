/*
 *           _                 _             _                 _          _____
 * __   ____| |_ __      _ __ | |_   _  __ _(_)_ __         __| |_  ___ _|___ /
 * \ \ / / _` | '__|____| '_ \| | | | |/ _` | | '_ \ _____ / _` \ \/ / '__||_ \
 *  \ V / (_| | | |_____| |_) | | |_| | (_| | | | | |_____| (_| |>  <| |  ___) |
 *   \_/ \__,_|_|       | .__/|_|\__,_|\__, |_|_| |_|      \__,_/_/\_\_| |____/
 *                      |_|            |___/
 *
 * Copyright (C) 2002-2004 Kai Möller
 * Copyright (C) 2004-2010 Christian Gmeiner
 *
 * This file is part of vdr-plugin-dxr3.
 *
 * vdr-plugin-dxr3 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation version 2.
 *
 * vdr-plugin-dxr3 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with dxr3-plugin.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef _DECODER_H_
#define _DECODER_H_

extern "C" {
#    include <libavcodec/avcodec.h>
#    include <libavformat/avformat.h>
}

#include <vdr/ringbuffer.h>
#include "dxr3multichannelaudio.h"
#include "uncopyable.h"

class cDxr3PesFrame;
class iAudio;

// ==================================
// allround decoder
class cDecoder : private Uncopyable {
public:
    cDecoder();
    ~cDecoder();

    void decode(cDxr3PesFrame *frame, iAudio *audio);
    void ac3dts(cDxr3PesFrame *frame, iAudio *audio);
/*
    void DecodeLpcm(cDxr3PesFrame *frame, uint32_t pts, cDxr3SyncBuffer &aBuf);
*/
    void Reset()
    {
    	ac3dtsDecoder.Clear();
    	rbuf.Clear();
    }

private:
    bool checkMpegAudioHdr(const uint8_t *head);
    int calcFrameSize(const uint8_t *header);

    AVCodec *audio;
    AVCodec *video;
    AVCodecContext *contextAudio;
    AVCodecContext *contextVideo;
    AVPacket avpkt;

    cRingBufferFrame rbuf;
    cMultichannelAudio ac3dtsDecoder;

    int frameSize;
    uint8_t lastBitrate;
    uint8_t pcmbuf[AVCODEC_MAX_AUDIO_FRAME_SIZE];

    void Init();
};

#endif /*_DECODER_H_*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// tab-width: 4;
// indent-tabs-mode: nil
// End:
