#ifndef _DXR3_OSD_H_
#define _DXR3_OSD_H_

#include "dxr3interface_spu_encoder.h"

// ==================================
// osd plugin provider
class cDxr3OsdProvider : public cOsdProvider {
public:
    cDxr3OsdProvider() {}
    virtual cOsd *CreateOsd(int Left, int Top, uint Level);
};

static const int MAXNUMWINDOWS = 7;

// ==================================
// osd interface
class cDxr3Osd : public cOsd {
public:
    cDxr3Osd(int Left, int Top, uint Level);
    ~cDxr3Osd();

    eOsdError CanHandleAreas(const tArea *Areas, int NumAreas);
    eOsdError SetAreas(const tArea *Areas, int NumAreas);

    void Flush();

protected:
    virtual void SetActive(bool On);

private:
    cSPUEncoder* Spu;		///< interface to cSPUEncoder
    bool shown;			///< is the osd shown?
    cPalette* Palette;		///< global palette (needed by all bitmaps)
    cTimeMs *last;
    tArea areas[MAXNUMWINDOWS];
    int numAreas;
};

#endif /*_DXR3_OSD_H_*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// tab-width: 4;
// indent-tabs-mode: nil
// End:
