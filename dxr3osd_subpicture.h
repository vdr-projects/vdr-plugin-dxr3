#ifndef _DXR3OSD_SUBPICTURE_H_
#define _DXR3OSD_SUBPICTURE_H_

#include "dxr3vdrincludes.h"
#include "dxr3spuencoder.h"

#if VDRVERSNUM >= 10307

// ==================================
// osd interface for => vdr1,3,7
class cDxr3SubpictureOsd : public cOsd
{
private:
	cSPUEncoder* Spu;	///< interface to cSPUEncoder
	bool shown;			///< is the osd shown?

public:
	cDxr3SubpictureOsd(int Left, int Top/*, int SpuDev*/);
	~cDxr3SubpictureOsd();

	eOsdError CanHandleAreas(const tArea *Areas, int NumAreas);
	void SaveRegion(int x1, int y1, int x2, int y2);
	void RestoreRegion(void);

    void Flush();
};

#endif /*VDRVERSNUM*/
#endif /*_DXR3OSD_SUBPICTURE_H_*/
