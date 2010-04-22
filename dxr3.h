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
    int newUseWSS;
    int newHideMenu;
    int newOsdFlushRate;
    int newBrightness;
    int newContrast;
    int newSaturation;
};


// ==================================
enum eDxr3OsdItem
{
    DXR3_FORCE_LETTER_BOX,
    DXR3_DIGITAL_OUT,
    DXR3_ANALOG_OUT
};

// ==================================
// osd item
class cDxr3OsdItem : public cOsdItem
{
public:
    cDxr3OsdItem(const char* text, eDxr3OsdItem item) :
	cOsdItem(text), m_item(item) {}

    // process fb input
    eOSState ProcessKey(eKeys Key);

protected:
    eDxr3OsdItem m_item;
};

// ==================================
// main screen
class cDxr3OsdMenu : public cOsdMenu
{
public:
    cDxr3OsdMenu(): cOsdMenu(tr(MAINMENUENTRY))
    {
	Clear();
	SetHasHotkeys();
	Add(new cDxr3OsdItem(hk(tr("Toggle force letterbox")),
			     DXR3_FORCE_LETTER_BOX));

        if (cSettings::instance()->useDigitalOut())
	    Add(new cDxr3OsdItem(hk(tr("Switch to analog audio output")),
				 DXR3_ANALOG_OUT));
	else
	    Add(new cDxr3OsdItem(hk(tr("Switch to digital audio output")),
				 DXR3_DIGITAL_OUT));
    }
};

#endif /*_DXR3_H_*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// tab-width: 4;
// indent-tabs-mode: nil
// End:
