/*
 * dxr3outputthread.h: 
 *
 * See the main source file 'dxr3.c' for copyright information and
 * how to reach the author.
 *
 */

#ifndef _DXR3OUTPUTTHREAD_H_
#define _DXR3OUTPUTTHREAD_H_

#include "dxr3vdrincludes.h"
#include "dxr3syncbuffer.h"
#include "dxr3audiodecoder.h"

// ==================================
class cDxr3OutputThread : public cThread 
{
public:
    cDxr3OutputThread(cDxr3Interface& dxr3Device, cDxr3SyncBuffer& buffer);
    virtual ~cDxr3OutputThread() {Cancel();};

    // virtual void Start(void) {cThread::Start()};
    void SetStopSignal();
    bool NeedResync()			{ return m_bNeedResync;};
    void ClearResyncRequest()	{ m_bNeedResync = false;};

protected:
    virtual void Action() = 0;
    bool GetStopSignal();

    cDxr3Interface& m_dxr3Device;
    cDxr3SyncBuffer& m_buffer;
    bool m_bStopThread;
    bool m_bNeedResync;

private:
    cDxr3OutputThread(cDxr3OutputThread&); // no copy contructor
};

// ==================================
class cDxr3AudioOutThread : public cDxr3OutputThread 
{
public:
    cDxr3AudioOutThread(cDxr3Interface& dxr3Device, cDxr3SyncBuffer& buffer);
    virtual ~cDxr3AudioOutThread() {}
    
protected:    
    void Action();

private:
//    cDxr3AudioOutThread(); // no standard constructor
    cDxr3AudioOutThread(cDxr3AudioOutThread&); // no copy constructor        
};

// ==================================
class cDxr3VideoOutThread : public cDxr3OutputThread 
{
public:
    cDxr3VideoOutThread(cDxr3Interface& dxr3Device, cDxr3SyncBuffer& buffer);
    virtual ~cDxr3VideoOutThread() {}

protected:    
    void Action();

private:
//    cDxr3VideoOutThread(); // no standard constructor
    cDxr3VideoOutThread(cDxr3VideoOutThread&); // no copy constructor
};

#endif /*_DXR3OUTPUTTHREAD_H_*/
