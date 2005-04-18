/*
 * dxr3configdata.c
 *
 * Copyright (C) 2002-2004 Kai M�ller
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

#include "dxr3configdata.h"

// ==================================
//! constructor
cDxr3ConfigData::cDxr3ConfigData() 
{
	// set default values
	m_Digitaloutput = 0;
	m_AC3output = 0;
	m_Card = 0;
	m_ForceLetterbox = 0;
    m_VideoMode = PAL;
	m_MenuMode = SUBPICTURE;
	m_Debug = 1;
	m_DebugLevel = 0;
	m_Brightness	= 500;
	m_Contrast		= 500;
	m_Saturation	= 500;
}

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// indent-tabs-mode: t
// End:
