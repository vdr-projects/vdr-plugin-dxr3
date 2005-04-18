/*
 * dxr3log.h
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

#ifndef _DXR3_LOG_H_
#define _DXR3_LOG_H_

#include <fstream>
#include <string>
#include "dxr3singleton.h"
#include "dxr3vdrincludes.h"

// ==================================
//!  A log class. 
/*!
	With this nice util dxr3plugin generates and mange a log file. In this
	file the developer/enduser can find informations and can find errors,
	problems and ohter stuff.
	The logging class is now thread-safe!
*/
class cLog : public Singleton<cLog>
{
public: 
	cLog();		// use default log file
	cLog(std::string FileName);
	
	~cLog()		{ Close(); }
	
	void SetForceFlush(const bool v)	{ Lock(); m_ForeFlush = v; Unlock(); }
	bool GetForceFlush() const			{ return m_ForeFlush; }
	
	// write type data to log file.
	template <class Type>
	inline cLog& operator << ( Type item ) 
	{
		Lock();
		if (m_LogOpen)
		{
			m_LogStream << item;
			if (m_ForeFlush) m_LogStream.flush();
		}
		Unlock();
		return *this;
	}
	inline cLog& operator << ( size_t item ) 
	{
		Lock();
		if (m_LogOpen)
		{
			m_LogStream << (unsigned int)item;
			if (m_ForeFlush) m_LogStream.flush();
		}
		Unlock();
		return *this;
	}
	inline cLog& operator << ( bool item ) 
	{
		Lock();
		if (m_LogOpen) 
		{
			if (item == true) 
			{
				m_LogStream << "true";
			} 
			else 
			{
				m_LogStream << "false";
			}
			if (m_ForeFlush) m_LogStream.flush();
		}
		Unlock();
		return *this;
	}

private:
	std::ofstream	m_LogStream;	///< Our logstream
	bool			m_LogOpen;		///< Is log open?
	bool			m_ForeFlush;	///< Do we want to flush log everytime?

	void Open(std::string Filename);	// with this function we open our logfile
	void Close();						// with this function we close our logfile

protected:
    static cMutex* m_pMutex;				///< mutex

    static void Lock()		{ cLog::m_pMutex->Lock(); }
    static void Unlock()	{ cLog::m_pMutex->Unlock(); }
};

#endif /*_DXR3_LOG_H_*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// indent-tabs-mode: t
// End:
