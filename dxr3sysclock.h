/*
 * dxr3sysclock.h
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

#ifndef _DXR3_SYSCLOCK_H_
#define _DXR3_SYSCLOCK_H_

#include <stdint.h>
#include <linux/em8300.h>
#include <vdr/thread.h>
#include "Uncopyable.h"

// ==================================
// work with dxr3's clock
class cDxr3SysClock : private Uncopyable {
public:
    cDxr3SysClock(int fd_control, int fd_video, int fd_spu):
	m_fdcontrol(fd_control),
	m_fdvideo(fd_video),
	m_fdspu(fd_spu),
	m_offset(0) {};

    virtual ~cDxr3SysClock() {};

public:
    void SetSysClock(uint32_t scr);
    uint32_t GetSysClock(void);
    void SetPts(uint32_t pts);
    void SetSpuPts(uint32_t pts);

protected:
    int m_fdcontrol;
    int m_fdvideo;
    int m_fdspu;
    uint32_t m_offset;
    cMutex mutex;
};

#endif /*_DXR3_SYSCLOCK_H_*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
