/*
 * dxr3output-video.c
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <stdio.h>
#include <time.h>
#include "dxr3output.h"

#define SCR m_dxr3Device.GetSysClock()

// ==================================
//! constr.
cDxr3VideoOutThread::cDxr3VideoOutThread(cDxr3Interface& dxr3Device,
					 cDxr3SyncBuffer& buffer) :
    cDxr3OutputThread(dxr3Device, buffer)
{
    SetDescription("DXR3 video output");
}

//! destructor
cDxr3VideoOutThread::~cDxr3VideoOutThread()
{
    m_buffer.Stop();
    Cancel(3);
}

// ==================================
//! thread action
void cDxr3VideoOutThread::Action()
{
    uint32_t pts = 0;
    static uint32_t lastPts = 0;

    while (Running())
    {
	cFixedLengthFrame* pNext = m_buffer.Get();
	if (pNext)
	{
	    pts = pNext->GetPts();
	    if (pts == lastPts)
		pts = 0;

	    if (pts > SCR && abs((int)pts - (int)SCR) < 7500)
	    {
		m_dxr3Device.SetPts(pts);
	    }

	    if (!pts || pts < SCR)
	    {
		if (m_buffer.Available())
		{
		    m_dxr3Device.PlayVideoFrame(pNext);
		    m_buffer.Pop();
		}
	    }
	    else
	    {
		if ((pts > SCR) && abs((int)pts - (int)SCR) < 7500)
		{
		    m_dxr3Device.SetPts(pts);

		    if (m_buffer.Available() && pNext->GetData() &&
			pNext->GetCount())
		    {
			m_dxr3Device.PlayVideoFrame(pNext);
			m_buffer.Pop();
		    }
		}
		else
		{
		    if (pts < SCR)
		    {
			m_dxr3Device.PlayVideoFrame(pNext);
			m_buffer.Pop();
		    }
		}
	    }


	    if (m_dxr3Device.IsExternalReleased())
	    {
		m_bNeedResync = true;
		m_buffer.Clear();
	    }

	    if ((pts > SCR && abs((int)pts - (int)SCR) > 7500 ))
	    {
		cCondWait::SleepMs(10);
	    }
	}
    }
}

#undef SCR

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
