/*
 * dxr3outputthread.h
 *
 * Copyright (C) 2002-2004 Kai Möller
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

#ifndef _DXR3OUTPUTTHREAD_H_
#define _DXR3OUTPUTTHREAD_H_

#include "dxr3vdrincludes.h"
#include "dxr3syncbuffer.h"
#include "dxr3audiodecoder.h"

// ==================================
class cDxr3OutputThread : public cThread
{
public:
    cDxr3OutputThread(cDxr3Interface& dxr3Device, cDxr3SyncBuffer& buffer);
    virtual ~cDxr3OutputThread()
    {
	Cancel();
    };
    /*
    virtual void Start(void)
    {
	cThread::Start();
    };
    */
    void SetStopSignal();
    bool NeedResync()
    {
	return m_bNeedResync;
    };
    void ClearResyncRequest()
    {
	m_bNeedResync = false;
    };

protected:
    virtual void Action() = 0;
    bool GetStopSignal();

    cDxr3Interface& m_dxr3Device;
    cDxr3SyncBuffer& m_buffer;
    bool m_bStopThread;
    bool m_bNeedResync;

private:
    cDxr3OutputThread(cDxr3OutputThread&); // no copy contructor
};

// ==================================
class cDxr3AudioOutThread : public cDxr3OutputThread
{
public:
    cDxr3AudioOutThread(cDxr3Interface& dxr3Device, cDxr3SyncBuffer& buffer);
    virtual ~cDxr3AudioOutThread();

protected:
    void Action();

private:
    //cDxr3AudioOutThread(); // no standard constructor
    cDxr3AudioOutThread(cDxr3AudioOutThread&); // no copy constructor
};

// ==================================
class cDxr3VideoOutThread : public cDxr3OutputThread
{
public:
    cDxr3VideoOutThread(cDxr3Interface& dxr3Device, cDxr3SyncBuffer& buffer);
    virtual ~cDxr3VideoOutThread();

protected:
    void Action();

private:
    //cDxr3VideoOutThread(); // no standard constructor
    cDxr3VideoOutThread(cDxr3VideoOutThread&); // no copy constructor
};

#endif /*_DXR3OUTPUTTHREAD_H_*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
