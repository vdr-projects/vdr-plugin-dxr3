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
typedef std::pair<tIndex, tIndex> transPair;

// ==================================
// osd interface
class cDxr3Osd : public cOsd {
public:
    cDxr3Osd(int Left, int Top, uint Level);
    virtual ~cDxr3Osd();

    virtual eOsdError CanHandleAreas(const tArea *Areas, int NumAreas);
    virtual eOsdError SetAreas(const tArea *Areas, int NumAreas);

    virtual void Flush();

protected:
    virtual void SetActive(bool On);

private:
    cSPUEncoder* Spu;		///< interface to cSPUEncoder
    bool shown;			///< is the osd shown?
    cPalette* Palette;		///< global palette (needed by all bitmaps)
    cTimeMs *last;
    tArea areas[MAXNUMWINDOWS];
    int numAreas;
    cBitmap *mergedBitmap;

    void copy(cBitmap *part, int area, transPair pair[16], int numPair);
};

#endif /*_DXR3_OSD_H_*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// tab-width: 4;
// indent-tabs-mode: nil
// End:
