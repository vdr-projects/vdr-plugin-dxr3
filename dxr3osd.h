#ifndef _DXR3_OSD_H_
#define _DXR3_OSD_H_

#include "dxr3vdrincludes.h"
#include "dxr3interface_spu_encoder.h"

// ==================================
// osd plugin provider
class cDxr3OsdProvider : public cOsdProvider
{
public:
    cDxr3OsdProvider() {}
    virtual cOsd *CreateOsd(int Left, int Top);
};

#endif /*_DXR3_OSD_H_*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
