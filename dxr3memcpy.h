/*
 * dxr3memcpy.h
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

/*****************************************************************************
 * memcpy.h: functions and definitions of the MMX/MMX2/SSE optimized versions
 * of memcpy
 *****************************************************************************
 * Copyright (C) 2001 Keuleu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111, USA.
 *****************************************************************************
 *
 * Original code:
 * 
 * Copyright (C) 2000-2001 the xine project
 * 
 * This file is part of xine, a unix video player.
 * 
 *****************************************************************************/

#ifndef _DXR3MEMCPY_H_
#define _DXR3MEMCPY_H_

#include "dxr3vdrincludes.h"

// ==================================
/*! \def BUFSIZE
    \brief size of buffers for benchmark :)
*/
#define BUFSIZE 1024*1024

// ==================================
struct memcpy_routine
{
	std::string name;	///< name of memcpy methode
	void *(* function)(void *to, const void *from, size_t len);	///< our memcopy methode
	unsigned long long time;	///< needed time for banchmark
	uint32_t cpu_require;		///< caps from dxr3cpu.h
};

// ==================================
//! Little class to do a nice benchmark
/*
	Whith this class we can get the fastest memcyp
	methode for target computer.
*/
class cDxr3MemcpyBench
{
public:
	cDxr3MemcpyBench(uint32_t config_flags = 0);

private:
	unsigned long long int Rdtsc(uint32_t config_flags);

	std::vector<memcpy_routine>	m_methods;	///< a std::vector with all methodes
};

// ==================================
//! optimized/fast memcpy
extern void *(* dxr3_memcpy)(void *to, const void *from, size_t len);

#endif /*_DXR3MEMCPY_H_*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End: