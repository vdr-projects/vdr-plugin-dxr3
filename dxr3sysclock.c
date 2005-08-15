/*
 * dxr3sysclock.c
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

#include <sys/ioctl.h>
#include "dxr3sysclock.h"

// ==================================
void cDxr3SysClock::SetSysClock(uint32_t scr) 
{
    uint32_t sc;
    
    mutex.Lock();
    ioctl(m_fdcontrol, EM8300_IOCTL_SCR_GET, &sc);
    m_offset = scr - sc;
    mutex.Unlock();
}

// ==================================
uint32_t cDxr3SysClock::GetSysClock(void) 
{
    uint32_t sc;
    uint32_t retval;
    
    mutex.Lock();
    ioctl(m_fdcontrol, EM8300_IOCTL_SCR_GET, &sc);
    retval = sc + m_offset;
    mutex.Unlock();

    return retval;
}

// ==================================
void cDxr3SysClock::SetPts(uint32_t pts) 
{
    uint32_t newPts = 0;
    
    mutex.Lock();
    newPts =  pts - m_offset;
    ioctl(m_fdvideo, EM8300_IOCTL_VIDEO_SETPTS, &newPts);
    mutex.Unlock();
}

// ==================================
void cDxr3SysClock::SetSpuPts(uint32_t pts) 
{
    uint32_t newPts = 0;

    mutex.Lock();
    newPts = (pts - m_offset) << 1;  // fix for DVD subtitles
    ioctl(m_fdspu, EM8300_IOCTL_SPU_SETPTS, &newPts);
    mutex.Unlock();
}

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
