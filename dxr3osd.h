#ifndef _DXR3_OSD_H_
#define _DXR3_OSD_H_

#include "dxr3vdrincludes.h"
#include "dxr3interface_spu_encoder.h"

#if VDRVERSNUM >= 10307

// ==================================
// osd plugin provider
class cDxr3OsdProvider : public cOsdProvider 
{
public:
    cDxr3OsdProvider()	{}
    virtual cOsd *CreateOsd(int Left, int Top);
};

#else /*VDRVERSNUM*/

// ==================================
// osd interface for =< vdr1,3,7
class cDxr3Osd : public cOsdBase 
{
private:
    cSPUEncoder* Spu;	///< interface to cSPUEncoder

	bool SetWindow(cWindow*);

public:
    cDxr3Osd(int x, int y);
    ~cDxr3Osd();

    virtual bool OpenWindow(cWindow *Window);
    virtual void CommitWindow(cWindow *Window);
    virtual void ShowWindow(cWindow *Window);
    virtual void HideWindow(cWindow *Window, bool Hide);
    virtual void CloseWindow(cWindow *Window);
    virtual void MoveWindow(cWindow *Window, int x, int y);
};

#endif /*VDRVERSNUM*/
#endif /*_DXR3_OSD_H_*/
