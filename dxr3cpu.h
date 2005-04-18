/*
 * dxr3cpu.h
 *
 * Copyright (C) 2004 Christian Gmeiner
 *
 * Taken (modifized) from Nesseia-Renderengine Copyright (C) 2003-2004 Christian Gmeiner
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

#ifndef _DXR3_CPU_H_
#define _DXR3_CPU_H_

#include <inttypes.h>
#include "dxr3singleton.h"

// ==================================
//! all possible cabs
enum Cpu_cabs
{
	CC_MMX		= 0x80000000,	///< Value for mmx
	CC_3DNOW	= 0x40000000,	///< Value for 3dnow
	CC_MMXEXT	= 0x20000000,	///< Value for mmx ext
	CC_SSE		= 0x10000000,	///< Value for sse
	CC_SSE2		= 0x08000000	///< Value for sse2
};

// ==================================
//! easy and fast access to all infos
struct CPUInformation
{
	CPUInformation()
	{
		AMD			= false;
		INTEL		= false;
		MMX			= false;
		MMXEXT		= false;
		SSE			= false;
		SSE2		= false;
		Now			= false;
		RDTSC		= false;
		HT			= false;
		AMD64Bit	= false;
	}
	
	char Vendor[16];	///< vendorname
	
	bool AMD;			///< is it an AMD CPU?
	bool INTEL;			///< is it an Intel CPU?
	bool MMX;			///< is MMX-Technology supported?
	bool MMXEXT;		///< are Extended MMX_MultimediaExtensions supported?
	bool SSE;			///< is SSE-Technology supported?
	bool SSE2;			///< is SSE2-Technology supported?
	bool Now;			///< is 3DNow-Technology supported?
	bool RDTSC;			///< is RDTSC-Technology supported?
	bool HT;			///< is HyperThreading supported?
	bool AMD64Bit;		///< is it a 64 bit machine?

	uint32_t caps;		///< all features represanted as caps
};

// ==================================
//!  Grab some infos about the cpu(s)
/*!
	If you want to know what the cpu of the
	target machine can do, this class is for you :)
	It is used intern for the math and memcpy part.
*/
class cDxr3CPU : public Singleton<cDxr3CPU>
{
public:
	cDxr3CPU();
	~cDxr3CPU()	{}
	
	bool HasMMXSupport() const		{ return m_Info.MMX; }
	bool HasSSESupport() const		{ return m_Info.SSE; }
	bool HasSSE2Support() const		{ return m_Info.SSE2; }
	bool Has3DNowSupport() const	{ return m_Info.Now; }
	bool HasRDTSCSupport() const	{ return m_Info.RDTSC; }
	bool HasHTSupport() const		{ return m_Info.HT; }
	
	inline CPUInformation GetInfos() const	{ return m_Info ;}
	
private:
	bool CheckCPUIDPresence();

	// main function to get cpu(s) features
	bool Cpuid(unsigned long function, unsigned long& out_eax, unsigned long& out_ebx, unsigned long& out_ecx, unsigned long& out_edx);
	
	CPUInformation m_Info;
};

#endif /*_DXR3_CPU_H_*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// indent-tabs-mode: t
// End:
