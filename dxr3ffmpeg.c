/*
 * dxr3ffmpeg.c
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

#include "dxr3ffmpeg.h"
#include "dxr3configdata.h"
#include <vdr/tools.h>

// ==================================
//! constructor
cDxr3Ffmpeg::cDxr3Ffmpeg()
{
    avcodec_init();
    // Only the MP2 decoder would be needed, but individual registration of
    // codecs bundled with ffmpeg itself does not appear to be really supported
    // as of ffmpeg 2007-07-xx and later.
    avcodec_register_all();
}

// ==================================
//! look if Codec is supported by ffmpeg
bool cDxr3Ffmpeg::FindCodec(struct Dxr3Codec& Codec)
{
    // find codec
    Codec.codec = avcodec_find_decoder(Codec.id);

    if (!Codec.codec)
    {
	esyslog("dxr3: ffmpeg: codec %#.5x not found - not supported"
		" by FFmpeg?", Codec.id);
	return false;
    }

    // init codec_context
    memset(&Codec.codec_context, 0, sizeof(Codec.codec_context));

    return true;
}

// ==================================
//! try to open Codec
bool cDxr3Ffmpeg::OpenCodec(struct Dxr3Codec& Codec)
{
    // try to open codec
    int result = avcodec_open(&Codec.codec_context, Codec.codec);

    if (result < 0)
    {
	esyslog("dxr3: ffmpeg: couldn't open codec %#.5x", Codec.id);
	return false;
    }
    else
    {
	Codec.Open = true;
    }

    return true;
}

// ==================================
//! close codec
void cDxr3Ffmpeg::CloseCodec(struct Dxr3Codec& Codec)
{
    if (Codec.Open)
    {
	avcodec_close(&Codec.codec_context);
	Codec.Open = false;
    }
}

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
