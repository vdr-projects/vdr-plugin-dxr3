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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include "dxr3ffmpeg.h"
#include "dxr3configdata.h"
#include "dxr3log.h"

// ==================================
//! constructor
cDxr3Ffmepg::cDxr3Ffmepg()
{
	avcodec_init();
    avcodec_register_all();
}

// ==================================
//! look if Codec is supported by ffmpeg
bool cDxr3Ffmepg::FindCodec(struct Dxr3Codec& Codec)
{
	// find codec
    Codec.codec = avcodec_find_decoder(Codec.id);

	if (!Codec.codec) 
	{
		// codec is't supported by ffmpeg
		if (cDxr3ConfigData::Instance().GetDebug())
		{
			cLog::Instance() << "cDxr3Ffmepg::OpenCodec(struct Dxr3Codec& Codec) codec not found (" << Codec.id << ")\n";
		}
		return false;
	}

	// init codec_context
    memset(&Codec.codec_context, 0, sizeof(Codec.codec_context));

	return true;
}

// ==================================
//! try to open Codec
bool cDxr3Ffmepg::OpenCodec(struct Dxr3Codec& Codec)
{
	// try to open codec
    int result = avcodec_open(&Codec.codec_context, Codec.codec); 

	if (result < 0) 
	{
		// we could not open codec
		if (cDxr3ConfigData::Instance().GetDebug())
		{
			cLog::Instance() << "cDxr3Ffmepg::OpenCodec(struct Dxr3Codec& Codec) coudnt open codec (" << Codec.id << ")\n";
		}
		return false;
	}
	else
	{
		Codec.Open = true;
	}

	return true;
}

// ==================================
// close codec
void cDxr3Ffmepg::CloseCodec(struct Dxr3Codec& Codec)
{
    if (Codec.Open) 
	{
        avcodec_close(&Codec.codec_context);
		Codec.Open = false;
    }
}
