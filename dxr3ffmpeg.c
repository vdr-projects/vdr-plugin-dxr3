/*
 * dxr3audiodecoder.c: 
 *
 * See the main source file 'dxr3.c' for copyright information and
 * how to reach the author.
 *
 */

#include "dxr3ffmpeg.h"

#include "dxr3configdata.h"
#include "dxr3log.h"

// ==================================
cDxr3Ffmepg::cDxr3Ffmepg()
{
	avcodec_init();
    avcodec_register_all();
}

// ==================================
bool cDxr3Ffmepg::FindCodec(struct Dxr3Codec& Codec)
{
	// find codec
    Codec.codec = avcodec_find_decoder(Codec.id);

	if (!Codec.codec) 
	{
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
bool cDxr3Ffmepg::OpenCodec(struct Dxr3Codec& Codec)
{
	// try to open codec
    int result = avcodec_open(&Codec.codec_context, Codec.codec); 

	if (result < 0) 
	{
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
void cDxr3Ffmepg::CloseCodec(struct Dxr3Codec& Codec)
{
    if (Codec.Open) 
	{
        avcodec_close(&Codec.codec_context);
		Codec.Open = false;
    }
}
