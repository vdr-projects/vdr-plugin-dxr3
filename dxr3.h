#ifndef _DXR3_H_
#define _DXR3_H_

#include <vdr/i18n.h>
#include "settings.h"

static const char MAINMENUENTRY[] = trNOOP("DXR3 Functions");

// --- cMenuSetupDxr3 -------------------------------------------------------

// ==================================
// setup screen
class cMenuSetupDxr3 : public cMenuSetupPage
{
public:
    cMenuSetupDxr3();

protected:
    virtual void Store();

private:
    int newUseDigitalOut;
    int newDxr3Card;
    int newVideoMode;
    int newAc3AudioMode;
    const char *menuVideoModes[3];
    const char *menuAc3AudioModes[2];
    int newForceLetterBox;
    int newUseWSS;
    int newHideMenu;
    int newOsdFlushRate;
    int newBrightness;
    int newContrast;
    int newSaturation;
};

#endif /*_DXR3_H_*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// tab-width: 4;
// indent-tabs-mode: nil
// End:
