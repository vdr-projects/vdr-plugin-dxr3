#ifndef _DXR3_VDRINCLUDES_H_
#define _DXR3_VDRINCLUDES_H_

#include <string>
#include <algorithm>
#include <vector>
using namespace std;

#ifndef __STL_CONFIG_H
#define __STL_CONFIG_H
#define __DXR3_UNDEF_STL_CONFIG_AFTERWARDS
#endif

// all includes from vdr
#if VDRVERSNUM >= 10307
	#include <vdr/osd.h>
#else
	#include <vdr/osdbase.h>
#endif
#include <vdr/config.h>
#include <vdr/thread.h>
#include <vdr/ringbuffer.h>
#include <vdr/spu.h>
#include <vdr/tools.h>
#include <vdr/device.h>
#include <vdr/status.h>
#include <vdr/plugin.h>
#include <vdr/audio.h>

#ifdef __DXR3_UNDEF_STL_CONFIG_AFTERWARDS
#undef __STL_CONFIG_H
#endif

#endif /*_DXR3_VDRINCLUDES_H_*/
