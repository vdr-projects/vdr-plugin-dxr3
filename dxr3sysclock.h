#ifndef _DXR3_SYSCLOCK_H_
#define _DXR3_SYSCLOCK_H_

//#include <stdint.h>
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
