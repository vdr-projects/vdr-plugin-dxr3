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
    cTimeMs *last;

public:
#if APIVERSNUM < 10509
    cDxr3SubpictureOsd(int Left, int Top);
#else
    cDxr3SubpictureOsd(int Left, int Top, uint Level);
#endif
    ~cDxr3SubpictureOsd();

    eOsdError CanHandleAreas(const tArea *Areas, int NumAreas);

    void Flush();
};

#endif /*_DXR3OSD_SUBPICTURE_H_*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
