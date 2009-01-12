#ifndef _DXR3_OSD_H_
#define _DXR3_OSD_H_

#include "dxr3interface_spu_encoder.h"

// ==================================
// osd plugin provider
class cDxr3OsdProvider : public cOsdProvider
{
public:
    cDxr3OsdProvider() {}
    virtual cOsd *CreateOsd(int Left, int Top, uint Level);
};

// ==================================
// osd interface
class cDxr3Osd : public cOsd
{
private:
    cSPUEncoder* Spu;		///< interface to cSPUEncoder
    bool shown;			///< is the osd shown?
    cPalette* Palette;		///< global palette (needed by all bitmaps)
    cTimeMs *last;
protected:
    virtual void SetActive(bool On);
public:
    cDxr3Osd(int Left, int Top, uint Level);
    ~cDxr3Osd();

    eOsdError CanHandleAreas(const tArea *Areas, int NumAreas);
    eOsdError SetAreas(const tArea *Areas, int NumAreas);

    void Flush();
};

#endif /*_DXR3_OSD_H_*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
