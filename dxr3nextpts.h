/*
 * dxr3nextpts.h:
 *
 * See the main source file 'dxr3.c' for copyright information and
 * how to reach the author.
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
