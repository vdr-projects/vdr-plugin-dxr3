/*
* dxr3dolbydigital.c
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

#include "dxr3dolbydigital.h"
#include "dxr3device.h"
#include "dxr3configdata.h"

// ==================================
//! play
#if VDRVERSNUM > 10320
void cDxr3DolbyDigital::Play(const uchar *Data, int Length, uchar Id)
// TODO: this fixes the build, but according to reports, there's no audio.
#else
void cDxr3DolbyDigital::Play(const uchar *Data, int Length)
#endif
{
	if (cDxr3ConfigData::Instance().GetUseDigitalOut())
	{
		// call simply cDxr3Device::PlayAudio
		cDxr3Device::Instance().PlayAudio(Data, Length);
	}
}

// ==================================
// 
void cDxr3DolbyDigital::Mute(bool On)
{
}

// ==================================
// 
void cDxr3DolbyDigital::Clear()
{
}

// ==================================
// 
void cDxr3DolbyDigital::Action()
{
}

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
