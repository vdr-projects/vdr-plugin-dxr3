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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#ifndef _DXR3_FFMPEG_H_
#define _DXR3_FFMPEG_H_

extern "C" 
{
	#include <avcodec.h>
};

#include <string.h> 
#include "dxr3singleton.h"

// ==================================
// a codec used by this plugin
struct Dxr3Codec
{
	Dxr3Codec() : Open(false) {}

    AVCodec* codec;
    AVCodecContext codec_context;
	enum CodecID id;
	bool Open;
};

// ==================================
// class to work with ffmpeg
class cDxr3Ffmepg : public Singleton<cDxr3Ffmepg> 
{
public:
    cDxr3Ffmepg();
    ~cDxr3Ffmepg()	{}

	bool FindCodec(struct Dxr3Codec& Codec);
	bool OpenCodec(struct Dxr3Codec& Codec);
	void CloseCodec(struct Dxr3Codec& Codec);

private:
    cDxr3Ffmepg(cDxr3Ffmepg&); // no copy constructor
};

#endif /*_DXR3_FFMPEG_H_*/
