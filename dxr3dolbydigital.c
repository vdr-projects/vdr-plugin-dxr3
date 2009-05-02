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
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
*/

#include "dxr3dolbydigital.h"
#include "dxr3device.h"
#include "dxr3configdata.h"

// ==================================
//! play
void cDxr3DolbyDigital::Play(const uchar *Data, int Length)
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

