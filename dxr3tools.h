/*
 * dxr3tools.h
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

#ifndef _DXR3TOOLS_H_
#define _DXR3TOOLS_H_

#include "dxr3vdrincludes.h"

namespace Tools
{
	// ==================================
	//! convert Rgb to CrCb
	inline unsigned int Rgb2YCrCb(unsigned long rgb) 
	{
		float Y,U,V;
		float R,G,B;
		unsigned int yuv = 0x0;
		
		B = ((rgb >> 16) & 0xFF);
		G = ((rgb >> 8) & 0xFF);
		R = (rgb & 0xFF);
		
		Y = (0.2578125 * R) + (0.50390625 * G) + (0.09765625 * B) + 16;
		U = (0.4375 * R) - (0.3671875 * G) - (0.0703125 * B) + 128;
		V =-(0.1484375 * R) - (0.2890625 * G) + (0.4375 * B) + 128;
		
		yuv = (int(Y) << 16) | (int(U) << 8) | int(V);
		
		return yuv;
	}

	// ==================================
	//! convert YUV to Rgb
	inline unsigned int YUV2Rgb(unsigned int yuv_color)
	{
		int Y, Cb, Cr;
		int Ey, Epb, Epr;
		int Eg, Eb, Er;
		
		Y = (yuv_color >> 16) & 0xff;
		Cb = (yuv_color) & 0xff;
		Cr = (yuv_color >> 8) & 0xff;
		
		Ey = (Y - 16);
		Epb = (Cb - 128);
		Epr = (Cr - 128);

		Eg = (298 * Ey - 100 * Epb - 208 * Epr) / 256;
		Eb = (298 * Ey + 516 * Epb) / 256;
		Er = (298 * Ey + 408 * Epr) / 256;
		
		if (Eg > 255)
			Eg = 255;
		if (Eg < 0)
			Eg = 0;
		
		if (Eb > 255)
			Eb = 255;
		if (Eb < 0)
			Eb = 0;
		
		if (Er > 255)
			Er = 255;
		if (Er < 0)
			Er = 0;
		
		return Eb | (Eg << 8) | (Er << 16);
	}

	// ==================================
	//! write a string via vdr to OSD
	inline void WriteInfoToOsd(std::string x)
	{
		#if VDRVERSNUM <= 10306
			Interface->Info(x.c_str());
		#else
			Skins.Message(mtInfo, x.c_str());
		#endif
	}

}

#endif /*_DXR3TOOLS_H_*/
