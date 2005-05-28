#ifndef _DXR3OSD_SUBPICTURE_H_
#define _DXR3OSD_SUBPICTURE_H_

#include "dxr3vdrincludes.h"
#include "dxr3interface_spu_encoder.h"

// ==================================
// osd interface
class cDxr3SubpictureOsd : public cOsd
{
private:
    cSPUEncoder* Spu;		///< interface to cSPUEncoder
    bool shown;			///< is the osd shown?
    cPalette* Palette;		///< global palette (needed by all bitmaps)
#if VDRVERSNUM >= 10318
    cTimeMs *last;
#else
    int last;
#endif

public:
    cDxr3SubpictureOsd(int Left, int Top/*, int SpuDev*/);
    ~cDxr3SubpictureOsd();

    eOsdError CanHandleAreas(const tArea *Areas, int NumAreas);
    void SaveRegion(int x1, int y1, int x2, int y2);
    void RestoreRegion(void);

    void Flush();
};

#endif /*_DXR3OSD_SUBPICTURE_H_*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
