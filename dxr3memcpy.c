/*
 * dxr3memcpy.c
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
 *
 * Orgianal from xine:
 *
 * Copyright (C) 2001-2003 the xine project
 *
 * This file is part of xine, a free video player.
 *
 * These are the MMX/MMX2/SSE optimized versions of memcpy
 *
 * This code was adapted from Linux Kernel sources by Nick Kurshev to
 * the mplayer program. (http://mplayer.sourceforge.net)
 *
 */

#include "dxr3log.h"
#include "dxr3cpu.h"
#include "dxr3memcpy.h"

void *(* dxr3_memcpy)(void *to, const void *from, size_t len);

#ifdef __i386__
// ==================================
// for small memory blocks (<256 bytes) this version is faster
#define small_memcpy(to,from,n) { register unsigned long int dummy; __asm__ __volatile__("rep; movsb":"=&D"(to), "=&S"(from), "=&c"(dummy) :"0" (to), "1" (from),"2" (n) : "memory"); }
/*
// -- dosn't compile with 2.95 gcc --
#define small_memcpy(to,from,n)\
{\
register unsigned long int dummy;\
__asm__ __volatile__(\
  "rep; movsb"\
  :"=&D"(to), "=&S"(from), "=&c"(dummy)\
  :"0" (to), "1" (from),"2" (n)\
  : "memory");\
}
*/
// ==================================
// linux kernel __memcpy (from: /include/asm/string.h)
static __inline__ void * __memcpy (
			       void * to,
			       const void * from,
			       size_t n)
{
int d0, d1, d2;

  if( n < 4 ) {
    small_memcpy(to,from,n);
  }
  else
    __asm__ __volatile__(
    "rep ; movsl\n\t"
    "testb $2,%b4\n\t"
    "je 1f\n\t"
    "movsw\n"
    "1:\ttestb $1,%b4\n\t"
    "je 2f\n\t"
    "movsb\n"
    "2:"
    : "=&c" (d0), "=&D" (d1), "=&S" (d2)
    :"0" (n/4), "q" (n),"1" ((long) to),"2" ((long) from)
    : "memory");

  return (to);
}

#define SSE_MMREG_SIZE 16
#define MMX_MMREG_SIZE 8

#define MMX1_MIN_LEN 0x800  /* 2K blocks */
#define MIN_LEN 0x40  /* 64-byte blocks */


// ==================================
/* SSE note: i tried to move 128 bytes a time instead of 64 but it
didn't make any measureable difference. i'm using 64 for the sake of
simplicity. [MF] */
static void * sse_memcpy(void * to, const void * from, size_t len)
{
  void *retval;
  size_t i;
  retval = to;

  /* PREFETCH has effect even for MOVSB instruction ;) */
  __asm__ __volatile__ (
    "   prefetchnta (%0)\n"
    "   prefetchnta 32(%0)\n"
    "   prefetchnta 64(%0)\n"
    "   prefetchnta 96(%0)\n"
    "   prefetchnta 128(%0)\n"
    "   prefetchnta 160(%0)\n"
    "   prefetchnta 192(%0)\n"
    "   prefetchnta 224(%0)\n"
    "   prefetchnta 256(%0)\n"
    "   prefetchnta 288(%0)\n"
    : : "r" (from) );

  if(len >= MIN_LEN)
  {
    register unsigned long int delta;
    /* Align destinition to MMREG_SIZE -boundary */
    delta = ((unsigned long int)to)&(SSE_MMREG_SIZE-1);
    if(delta)
    {
      delta=SSE_MMREG_SIZE-delta;
      len -= delta;
      small_memcpy(to, from, delta);
    }
    i = len >> 6; /* len/64 */
    len&=63;
    if(((unsigned long)from) & 15)
      /* if SRC is misaligned */
      for(; i>0; i--)
      {
        __asm__ __volatile__ (
        "prefetchnta 320(%0)\n"
       "prefetchnta 352(%0)\n"
        "movups (%0), %%xmm0\n"
        "movups 16(%0), %%xmm1\n"
        "movups 32(%0), %%xmm2\n"
        "movups 48(%0), %%xmm3\n"
        "movntps %%xmm0, (%1)\n"
        "movntps %%xmm1, 16(%1)\n"
        "movntps %%xmm2, 32(%1)\n"
        "movntps %%xmm3, 48(%1)\n"
        : : "r" (from), "r" (to) : "memory");
        ((const unsigned char *)from)+=64;
        ((unsigned char *)to)+=64;
      }
    else
      /*
         Only if SRC is aligned on 16-byte boundary.
         It allows to use movaps instead of movups, which required data
         to be aligned or a general-protection exception (#GP) is generated.
      */
      for(; i>0; i--)
      {
        __asm__ __volatile__ (
        "prefetchnta 320(%0)\n"
       "prefetchnta 352(%0)\n"
        "movaps (%0), %%xmm0\n"
        "movaps 16(%0), %%xmm1\n"
        "movaps 32(%0), %%xmm2\n"
        "movaps 48(%0), %%xmm3\n"
        "movntps %%xmm0, (%1)\n"
        "movntps %%xmm1, 16(%1)\n"
        "movntps %%xmm2, 32(%1)\n"
        "movntps %%xmm3, 48(%1)\n"
        : : "r" (from), "r" (to) : "memory");
        ((const unsigned char *)from)+=64;
        ((unsigned char *)to)+=64;
      }
    /* since movntq is weakly-ordered, a "sfence"
     * is needed to become ordered again. */
    __asm__ __volatile__ ("sfence": : :"memory");
    /* enables to use FPU */
    __asm__ __volatile__ ("emms": : :"memory");
  }
  /*
   *	Now do the tail of the block
   */
  if(len) __memcpy(to, from, len);
  return retval;
}

// ==================================
static void * mmx_memcpy(void * to, const void * from, size_t len)
{
  void *retval;
  size_t i;
  retval = to;

  if(len >= MMX1_MIN_LEN)
  {
    register unsigned long int delta;
    /* Align destinition to MMREG_SIZE -boundary */
    delta = ((unsigned long int)to)&(MMX_MMREG_SIZE-1);
    if(delta)
    {
      delta=MMX_MMREG_SIZE-delta;
      len -= delta;
      small_memcpy(to, from, delta);
    }
    i = len >> 6; /* len/64 */
    len&=63;
    for(; i>0; i--)
    {
      __asm__ __volatile__ (
      "movq (%0), %%mm0\n"
      "movq 8(%0), %%mm1\n"
      "movq 16(%0), %%mm2\n"
      "movq 24(%0), %%mm3\n"
      "movq 32(%0), %%mm4\n"
      "movq 40(%0), %%mm5\n"
      "movq 48(%0), %%mm6\n"
      "movq 56(%0), %%mm7\n"
      "movq %%mm0, (%1)\n"
      "movq %%mm1, 8(%1)\n"
      "movq %%mm2, 16(%1)\n"
      "movq %%mm3, 24(%1)\n"
      "movq %%mm4, 32(%1)\n"
      "movq %%mm5, 40(%1)\n"
      "movq %%mm6, 48(%1)\n"
      "movq %%mm7, 56(%1)\n"
      : : "r" (from), "r" (to) : "memory");
      ((const unsigned char *)from)+=64;
      ((unsigned char *)to)+=64;
    }
    __asm__ __volatile__ ("emms": : :"memory");
  }
  /*
   *	Now do the tail of the block
   */
  if(len) __memcpy(to, from, len);
  return retval;
}

// ==================================
static void * mmx2_memcpy(void * to, const void * from, size_t len)
{
  void *retval;
  size_t i;
  retval = to;

  /* PREFETCH has effect even for MOVSB instruction ;) */
  __asm__ __volatile__ (
    "   prefetchnta (%0)\n"
    "   prefetchnta 32(%0)\n"
    "   prefetchnta 64(%0)\n"
    "   prefetchnta 96(%0)\n"
    "   prefetchnta 128(%0)\n"
    "   prefetchnta 160(%0)\n"
    "   prefetchnta 192(%0)\n"
    "   prefetchnta 224(%0)\n"
    "   prefetchnta 256(%0)\n"
    "   prefetchnta 288(%0)\n"
    : : "r" (from) );

  if(len >= MIN_LEN)
  {
    register unsigned long int delta;
    /* Align destinition to MMREG_SIZE -boundary */
    delta = ((unsigned long int)to)&(MMX_MMREG_SIZE-1);
    if(delta)
    {
      delta=MMX_MMREG_SIZE-delta;
      len -= delta;
      small_memcpy(to, from, delta);
    }
    i = len >> 6; /* len/64 */
    len&=63;
    for(; i>0; i--)
    {
      __asm__ __volatile__ (
      "prefetchnta 320(%0)\n"
      "prefetchnta 352(%0)\n"
      "movq (%0), %%mm0\n"
      "movq 8(%0), %%mm1\n"
      "movq 16(%0), %%mm2\n"
      "movq 24(%0), %%mm3\n"
      "movq 32(%0), %%mm4\n"
      "movq 40(%0), %%mm5\n"
      "movq 48(%0), %%mm6\n"
      "movq 56(%0), %%mm7\n"
      "movntq %%mm0, (%1)\n"
      "movntq %%mm1, 8(%1)\n"
      "movntq %%mm2, 16(%1)\n"
      "movntq %%mm3, 24(%1)\n"
      "movntq %%mm4, 32(%1)\n"
      "movntq %%mm5, 40(%1)\n"
      "movntq %%mm6, 48(%1)\n"
      "movntq %%mm7, 56(%1)\n"
      : : "r" (from), "r" (to) : "memory");
      ((const unsigned char *)from)+=64;
      ((unsigned char *)to)+=64;
    }
     /* since movntq is weakly-ordered, a "sfence"
     * is needed to become ordered again. */
    __asm__ __volatile__ ("sfence": : :"memory");
    __asm__ __volatile__ ("emms": : :"memory");
  }
  /*
   *	Now do the tail of the block
   */
  if(len) __memcpy(to, from, len);
  return retval;
}

// ==================================
static void *linux_kernel_memcpy(void *to, const void *from, size_t len) {
  return __memcpy(to,from,len);
}
#endif /*__i386__*/


// ==================================
// constr.
cDxr3MemcpyBench::cDxr3MemcpyBench(uint32_t config_flags)
{	
	//
	// add all aviable memcpy routines
	//

	memcpy_routine routine;

	// glibc memcpy
	routine.name = "glibc memcpy()";
	routine.function = memcpy;
	routine.time = 0;
	routine.cpu_require = 0;
	m_methods.push_back(routine);

	#ifdef __i386__

	// linux_kernel_memcpy
	routine.name = "linux_kernel_memcpy()";
	routine.function = linux_kernel_memcpy;
	routine.cpu_require = 0;
	m_methods.push_back(routine);

	// MMX optimized memcpy()
	routine.name = "MMX optimized memcpy()";
	routine.function = mmx_memcpy;
	routine.cpu_require = CC_MMX;
	m_methods.push_back(routine);

	// MMXEXT optimized memcpy()
	routine.name = "MMXEXT optimized memcpy()";
	routine.function = mmx2_memcpy;
	routine.cpu_require = CC_MMXEXT;
	m_methods.push_back(routine);

	#	ifndef __FreeBSD__

	// SSE optimized memcpy()
	routine.name = "SSE optimized memcpy()";
	routine.function = sse_memcpy;
	routine.cpu_require = CC_MMXEXT|CC_SSE;
	m_methods.push_back(routine);

	#	endif /*__FreeBSD__*/
	#endif /*__i386__*/


	//
	// run benchmarking
	// 

	unsigned long long t = 0;
	void *buf1, *buf2;
	int j, best = -1;

	if ((buf1 = malloc(BUFSIZE)) == NULL)
		return;
    
	if ((buf2 = malloc(BUFSIZE)) == NULL) 
	{
		free(buf1);
		return;
	}

	cLog::Instance() << "\nBenchmarking memcpy() methods (smaller is better):\n";
	// make sure buffers are present on physical memory
	memcpy(buf1,buf2,BUFSIZE);

	for (size_t i = 0; i < m_methods.size(); i++)
	{
		if ((config_flags & m_methods[i].cpu_require) != m_methods[i].cpu_require)
		{
			continue;
		}

		// count 100 runs of the memcpy function
		t = Rdtsc();
		for	(j = 0; j < 50; j++) 
		{
			m_methods[i].function(buf2,buf1,BUFSIZE);
			m_methods[i].function(buf1,buf2,BUFSIZE);
		}     
		t = Rdtsc() - t;

		m_methods[i].time = t;

		cLog::Instance() << m_methods[i].name.c_str() << ": " << (unsigned long long)t << "\n";

		if (best == -1 || t < m_methods[best].time)
		{
			best = i;
		}
	}
	cLog::Instance() << "\nBest one: " << m_methods[best].name.c_str() << "\n\n";

	dxr3_memcpy = m_methods[best].function;


	// clear unused memory
	free(buf1);
	free(buf2);
}

// ==================================
// neede for exact timing
unsigned long long int cDxr3MemcpyBench::Rdtsc()
{
	#ifdef __i386__
	unsigned long long int x;
	__asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));     
	return x;
	#else
	/* FIXME: implement an equivalent for using optimized memcpy on other
            architectures */
	return 0;
	#endif
}
