/*
 * dxr3sysclock.c
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#ifndef _DXR3_VDRINCLUDES_H_
#define _DXR3_VDRINCLUDES_H_

#include <string>
#include <algorithm>
#include <vector>
//using namespace std;

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
