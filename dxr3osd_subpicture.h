#ifndef _DXR3OSD_SUBPICTURE_H_
#define _DXR3OSD_SUBPICTURE_H_

#include "dxr3vdrincludes.h"
#include "dxr3interface_spu_encoder.h"

#if VDRVERSNUM >= 10307

/*
  The OSD goes berserk in some setups if it's refreshed too often.
  If it doesn't happen for you or you don't like the small delay this
  causes, change this to 0.  Experiment with larger values if the default
  of 25ms does not calm down your OSD.
*/
#define FLUSHRATE 25

// ==================================
// osd interface for => vdr1,3,7
class cDxr3SubpictureOsd : public cOsd
{
private:
	cSPUEncoder* Spu;
	bool shown;
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

#endif /*VDRVERSNUM*/
#endif /*_DXR3OSD_SUBPICTURE_H_*/
