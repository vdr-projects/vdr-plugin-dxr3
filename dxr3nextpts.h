/*
 * dxr3nextpts.h
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

#ifndef _DXR3NEXTPTS_H_
#define _DXR3NEXTPTS_H_

#include <unistd.h>
#include <stdint.h> 
#include "dxr3vdrincludes.h"
#include "dxr3singleton.h"

// ==================================
// pts program time stamp
// damit wird ermittelt welches audio frame zu welchem bild gehört
class cDxr3NextPts : public Singleton<cDxr3NextPts>
{
public:
    cDxr3NextPts()	{}
    ~cDxr3NextPts()	{}

    void SetNextPts(uint32_t pts)	{ Lock(); if (pts) m_nextPts = pts; Unlock(); }
    uint32_t GetNextPts()			{ Lock(); uint32_t tmpPts = m_nextPts; Unlock(); return tmpPts;}

    void Clear()	{ Lock(); m_nextPts = 0; Unlock();}
        
protected:
    static cMutex* m_pMutex;
    uint32_t m_nextPts;

    static void Lock()		{cDxr3NextPts::m_pMutex->Lock();}
    static void Unlock()	{cDxr3NextPts::m_pMutex->Unlock();}
    
private:
    cDxr3NextPts(cDxr3NextPts&); // no copy constructor    
};

#endif /*_DXR3NEXTPTS_H_*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
