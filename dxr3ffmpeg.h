#ifndef _DXR3_FFMPEG_H_
#define _DXR3_FFMPEG_H_

extern "C" 
{
	#include <avcodec.h>
};

//#include <stdlib.h>
//#include <stdint.h>
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
