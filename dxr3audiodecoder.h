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

#include <stdlib.h>
#include <stdint.h>

#include "dxr3ffmpeg.h"
#include "dxr3syncbuffer.h"
#include "dxr3multichannelaudio.h"
#include "dxr3log.h"

// ==================================
//!  Decode given audiostream in usable audioformat
/*!
	Here we can decode lpcm, ac3 and pcm to useable
	bitstream for ffmpeg. ffmpeg will convert
	the datastream into mp2.
*/
class cDxr3AudioDecoder 
{
public:
    cDxr3AudioDecoder();
    ~cDxr3AudioDecoder();

    void Init();

    void Decode(const uint8_t* buf, int length, uint32_t pts, cDxr3SyncBuffer &aBuf);
    void DecodeLpcm(const uint8_t* buf, int length, uint32_t pts, cDxr3SyncBuffer &aBuf);
    void DecodeAc3Dts(const uint8_t* pPes, const uint8_t* buf, int length, uint32_t pts, cDxr3SyncBuffer &aBuf);

    int GetRate() const			{ return m_Rate; }
    int GetChannelCount() const { return m_Channels; }
    int GetFrameSize() const	{ return m_FrameSize; }
    void Reset()				{ m_AC3dtsDecoder.Clear(); m_RBuf.Clear(); }
    
private:
    bool HeadCheck(unsigned long head);
    
	struct Dxr3Codec m_Codec;

    cRingBufferFrame m_RBuf;
    cMultichannelAudio m_AC3dtsDecoder;
    
    bool m_AudioSynched;
    bool m_DecoderOpened;
    uint8_t m_LastHeader[4];
    int m_Rate;
    int m_Channels;
    uint32_t m_FrameSize;
    uint8_t m_PcmBuf[AVCODEC_MAX_AUDIO_FRAME_SIZE];
    int m_Volume;
    bool m_FoundHeader;
    bool m_DecodeAudio;

    cDxr3AudioDecoder(cDxr3AudioDecoder&); // no copy constructor
};

#endif /*_DXR3_AUDIODECODER_H_*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
