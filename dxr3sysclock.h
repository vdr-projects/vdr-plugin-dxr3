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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#ifndef _DXR3_SYSCLOCK_H_
#define _DXR3_SYSCLOCK_H_

#include "dxr3vdrincludes.h"
#include <linux/em8300.h>

// ==================================
// work with dxr3's clock
class cDxr3SysClock 
{
 public:
  cDxr3SysClock(int fd_control, int fd_video, int fd_spu) : m_fdcontrol(fd_control),
    m_fdvideo(fd_video), m_fdspu(fd_spu), m_offset(0) {};
    
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
    
 protected:
  cDxr3SysClock(); // you are not allowed to use this contructor
  cDxr3SysClock(cDxr3SysClock&); // no copy constructor
};

#endif /*_DXR3_SYSCLOCK_H_*/
