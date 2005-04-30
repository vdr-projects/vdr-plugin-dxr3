/*
* dxr3cpu.c
*
* Copyright (C) 2004 Christian Gmeiner
*
* Taken from Nesseia-Renderengine Copyright (C) 2003-2004 Christian Gmeiner
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

#include "dxr3cpu.h"
#include "dxr3log.h"
#include "dxr3memcpy.h"

// ==================================
//! constructor
cDxr3CPU::cDxr3CPU()
{
	unsigned long eax,ebx,edx,unused;
	
	// readout the vendor
	Cpuid(0,eax,ebx,unused,edx);
	
	// set Vendor to ""
	memset(m_Info.Vendor, 0, 16);
	
	// connect the single register values to the vendor string
	// maybe there is an better solution - i will google :)
	*(unsigned long *)(m_Info.Vendor) = ebx;
	*(unsigned long *)(m_Info.Vendor + 4) = edx;
	*(unsigned long *)(m_Info.Vendor + 8) = unused;
	
	// check the features
	// could we get the needed infos?
	if (Cpuid(1,eax,ebx,unused,edx))
	{
		m_Info.MMX = ((edx & 1<<23) != 0);
		m_Info.SSE = ((edx & 1<<25) != 0);
		m_Info.SSE2= ((edx & 1<<26) != 0);
		m_Info.RDTSC=((edx & 1<<4) != 0); /*0x10*/
		m_Info.HT  = ((edx & 1<<28) !=0);		// should we do here addinonal checks?
		
		// 3DNow is a litle bit harder to read out
		// We read the ext. CPUID level 0x80000000
		if (Cpuid(0x80000000,eax,ebx,unused,edx))
		{
			// now in eax there is the max. supported extended CPUID level
			// we check if theres an extended CPUID level support
			if (eax >= 0x80000001)
			{
				// If we can access the extended CPUID level 0x80000001 we get the
				// edx register
				if (Cpuid(0x80000001,eax,ebx,unused,edx))
				{
					// Now we can mask some AMD specific cpu extensions
					// 22 ... Extended MMX_MultimediaExtensions
					m_Info.MMXEXT = ((edx & 1<<22) != 0);
					m_Info.AMD64Bit = ((edx & 1<<29) != 0);
					// 30 ... Extended 3DNOW_InstructionExtensions
					m_Info.Now = ((edx & (1<<31)) != 0);
				}
			}
		}
	}

	// MPlayer, Xine-lib, Transcode: SSE implies MMXEXT
	m_Info.MMXEXT = m_Info.MMXEXT || m_Info.SSE;

	// fill cabs
	if (m_Info.MMX)
	{
		m_Info.caps |= CC_MMX;
	}

	if (m_Info.MMXEXT)
	{
		m_Info.caps |= CC_MMXEXT;
	}

	if (m_Info.SSE)
	{
		m_Info.caps |= CC_SSE;
	}

	if (m_Info.Now)
	{
		m_Info.caps |= CC_3DNOW;
	}

	// print some infos about cpu
	cLog::Instance() << "cpu vendor: " << m_Info.Vendor << "\n";
	cLog::Instance() << "cpu extensions:\n";
	cLog::Instance() << "mmx:      " << m_Info.MMX << "\n";
	cLog::Instance() << "mmx-ext:  " << m_Info.MMXEXT << "\n";
	cLog::Instance() << "sse:      " << m_Info.SSE << "\n";
	cLog::Instance() << "sse2:     " << m_Info.SSE2 << "\n";
	cLog::Instance() << "3dnow:    " << m_Info.Now << "\n";

	// now we select the best memcpy mehtode
	cDxr3MemcpyBench Benchmark(m_Info.caps);
}

// ==================================
//! does the cpu support cpuid instructions
bool cDxr3CPU::CheckCPUIDPresence()
{
	// todo
	return true;
}

// ==================================
//! cpuid function
bool cDxr3CPU::Cpuid(unsigned long function, unsigned long& out_eax, unsigned long& out_ebx, unsigned long& out_ecx, unsigned long& out_edx)
{
	// This works with PIC/non-PIC, from ffmpeg (libavcodec/i386/cputest.c)

#ifdef __x86_64__
#  define REG_b "rbx"
#  define REG_S "rsi"
#else
#  define REG_b "ebx"
#  define REG_S "esi"
#endif

	__asm __volatile					\
	  ("mov %%"REG_b", %%"REG_S"\n\t"			\
	   "cpuid\n\t"						\
	   "xchg %%"REG_b", %%"REG_S				\
	   : "=a" (out_eax), "=S" (out_ebx),			\
	     "=c" (out_ecx), "=d" (out_edx)			\
	   : "0" (function));
	return true;
}

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
