/*
 * dxr3ffmpeg.h
 *
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

#ifndef _DXR3_FFMPEG_H_
#define _DXR3_FFMPEG_H_

extern "C"
{
#include <avcodec.h>
}

#include <string.h>
#include "dxr3singleton.h"

// ==================================
//! a codec used by this plugin
struct Dxr3Codec
{
    Dxr3Codec() : Open(false) {}

    AVCodec* codec;			///< ffmpeg's AVCodec
    AVCodecContext codec_context;	///< ffmpeg's AVCodecContext
    enum CodecID id;			///< id's from ffmpeg, eg. CODEC_ID_MP2
    bool Open;				///< is codec open?
};

// ==================================
// class to work with ffmpeg
/*!
  With cDxr3Ffmepg you can easily handle as many
  codecs as you want.
  At the moment we need this only for
  the audiodecoder, but in future i want to use
  it for ohter nice stuff :)
*/
class cDxr3Ffmepg : public Singleton<cDxr3Ffmepg>
{
public:
    cDxr3Ffmepg();
    ~cDxr3Ffmepg() {}

    bool FindCodec(struct Dxr3Codec& Codec);
    bool OpenCodec(struct Dxr3Codec& Codec);
    void CloseCodec(struct Dxr3Codec& Codec);

private:
    cDxr3Ffmepg(cDxr3Ffmepg&); // no copy constructor
};

#endif /*_DXR3_FFMPEG_H_*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
