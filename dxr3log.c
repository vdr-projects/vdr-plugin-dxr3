/*
 * dxr3log.c
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

#include "dxr3log.h"

// ==================================
cMutex* cLog::m_pMutex = new cMutex;

// ==================================
//! constructor
cLog::cLog()
{
	m_LogOpen = false;
	m_ForeFlush = true;
	
	std::string Filename;
	Filename = LOGPATH;
	Filename += "dxr3plugin.log";

	Open(Filename);
}

// ==================================
//! constructor with filename
cLog::cLog(std::string Filename)
{
	m_LogOpen = false;
	m_ForeFlush = true;
	
	Open(Filename);
}

// ==================================
//! open log file
void cLog::Open(std::string Filename)
{
	Lock();
	m_LogStream.open(Filename.c_str());
	m_LogOpen = true;
	Unlock();
}
	
// ==================================
//! close log file
void cLog::Close()
{
	Lock();
	if (m_LogOpen)
	{
		// close file now
		m_LogStream.flush();
		m_LogStream.close();
		
		m_LogOpen = false;
	}
	Unlock();
}

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// indent-tabs-mode: t
// End:
