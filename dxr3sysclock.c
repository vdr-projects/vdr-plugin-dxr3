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
