/*
 * dxr3.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: dxr3.c,v 1.1.2.30 2007/04/09 19:57:44 scop Exp $
 *
 */

#include "dxr3vdrincludes.h"
#include "dxr3device.h"
#include "dxr3syncbuffer.h"
#include "dxr3configdata.h"
#include "dxr3interface.h"
#include "dxr3.h"
#include "dxr3i18n.h"

static const char VERSION[]       = "0.2.7-cvs";
static const char DESCRIPTION[]   = "Hardware MPEG decoder";
static const char MAINMENUENTRY[] = "DXR3";

#if VDRVERSNUM && VDRVERSNUM < 10342
#error "This version of the DXR3 plugin needs VDR version >= 1.3.42"
#endif

#define DXR3_MAX_CARDS 4

// ==================================
// 'message-handler' for the main screen
eOSState cDxr3OsdItem::ProcessKey(eKeys Key)
{
    if (Key == kOk)
    {
	switch (m_item)
	{
	case DXR3_RESET_HARDWARE:
	    cDxr3Interface::Instance().ResetHardware();
	    if (cDxr3Device::InstanceP())
		cDxr3Device::InstanceP()->Reset();
	    break;

	case DXR3_FORCE_LETTER_BOX:
	    cDxr3ConfigData::Instance().SetForceLetterBox(
		!cDxr3ConfigData::Instance().GetForceLetterBox());
	    break;

	case DXR3_ANALOG_OUT:
	    cDxr3ConfigData::Instance().SetUseDigitalOut(0);
	    if (cDxr3Device::InstanceP())
		cDxr3Device::InstanceP()->Reset();
	    break;

	case DXR3_DIGITAL_OUT:
	    cDxr3ConfigData::Instance().SetUseDigitalOut(1);
	    if (cDxr3Device::InstanceP())
		cDxr3Device::InstanceP()->Reset();
	    break;
	}
    }

    return Key == kOk ? osBack : cOsdItem::ProcessKey(Key);
}

// ==================================
// setup menu
cMenuSetupDxr3::cMenuSetupDxr3(void)
{
    newBrightness = cDxr3ConfigData::Instance().GetBrightness();
    Add(new cMenuEditIntItem(tr("Brightness"),
			     &newBrightness, 0, 999));
    newContrast = cDxr3ConfigData::Instance().GetContrast();
    Add(new cMenuEditIntItem(tr("Contrast"),
			     &newContrast, 0, 999));
    newSaturation = cDxr3ConfigData::Instance().GetSaturation();
    Add(new cMenuEditIntItem(tr("Saturation"),
			     &newSaturation, 0, 999));
    newVideoMode = (int) cDxr3ConfigData::Instance().GetVideoMode();
    menuVideoModes[0] = tr("PAL");
    menuVideoModes[1] = tr("PAL60");
    menuVideoModes[2] = tr("NTSC");
    Add(new cMenuEditStraItem(tr("Video mode"),
			      &newVideoMode, 3, menuVideoModes));
    newUseDigitalOut = cDxr3ConfigData::Instance().GetUseDigitalOut();
    Add(new cMenuEditBoolItem(tr("Digital audio output"), &newUseDigitalOut));
    newOsdFlushRate = cDxr3ConfigData::Instance().GetOsdFlushRate();
    Add(new cMenuEditIntItem(tr("OSD flush rate (ms)"),
			     &newOsdFlushRate, 0, 255));
    newHideMenu = cDxr3ConfigData::Instance().GetHideMenu();
    Add(new cMenuEditBoolItem(tr("Hide main menu entry"), &newHideMenu));
    newDxr3Card = cDxr3ConfigData::Instance().GetDxr3Card();
    Add(new cMenuEditIntItem(tr("Card number"),
			     &newDxr3Card, 0, DXR3_MAX_CARDS - 1));
}

// ==================================
// save menu values
void cMenuSetupDxr3::Store(void)
{
    SetupStore("Brightness",
	       cDxr3ConfigData::Instance().SetBrightness(newBrightness));
    SetupStore("Contrast",
	       cDxr3ConfigData::Instance().SetContrast(newContrast));
    SetupStore("Saturation",
	       cDxr3ConfigData::Instance().SetSaturation(newSaturation));
    SetupStore("Dxr3VideoMode",
	       cDxr3ConfigData::Instance().SetVideoMode((eVideoMode) newVideoMode));
    SetupStore("UseDigitalOut",
	       cDxr3ConfigData::Instance().SetUseDigitalOut(newUseDigitalOut));
    SetupStore("OsdFlushRate",
	       cDxr3ConfigData::Instance().SetOsdFlushRate(newOsdFlushRate));
    SetupStore("HideMenu",
	       cDxr3ConfigData::Instance().SetHideMenu(newHideMenu));
    SetupStore("Dxr3Card",
	       cDxr3ConfigData::Instance().SetDxr3Card(newDxr3Card));

    // Apply (some of the) settings
    cDxr3Interface::Instance().SetBrightness(newBrightness);
    cDxr3Interface::Instance().SetContrast(newContrast);
    cDxr3Interface::Instance().SetSaturation(newSaturation);
    if (cDxr3Device::InstanceP())
	cDxr3Device::InstanceP()->Reset();
}

// ==================================
class cPluginDxr3 : public cPlugin
{
private:
    // Add any member variables or functions you may need here.
public:
    cPluginDxr3();
    ~cPluginDxr3();

    const char *Version()
    {
	return VERSION;
    }
    const char *Description()
    {
	return tr(DESCRIPTION);
    }

    bool Initialize();

    const char* MainMenuEntry();
    cOsdObject* MainMenuAction();

    cMenuSetupPage *SetupMenu();
    bool SetupParse(const char *Name, const char *Value);
};

// ==================================
cPluginDxr3::cPluginDxr3()
{
    // Initialize any member varaiables here.
    // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
    // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
    cDxr3ConfigData::Instance();
}

// ==================================
cPluginDxr3::~cPluginDxr3()
{
}

// ==================================
bool cPluginDxr3::Initialize()
{
    RegisterI18n(Phrases);

    cDxr3Device::InstanceP();

    return true;
}

// ==================================
cMenuSetupPage* cPluginDxr3::SetupMenu()
{
    return new cMenuSetupDxr3();
}

// ==================================
bool cPluginDxr3::SetupParse(const char *Name, const char *Value)
{
    if (!strcasecmp(Name, "UseDigitalOut"))
    {
	cDxr3ConfigData::Instance().SetUseDigitalOut(atoi(Value));
	return true;
    }
    if (!strcasecmp(Name, "Dxr3Card"))
    {
	cDxr3ConfigData::Instance().SetDxr3Card(atoi(Value));
	return true;
    }
    if (!strcasecmp(Name, "Dxr3VideoMode"))
    {
	cDxr3ConfigData::Instance().SetVideoMode((eVideoMode) atoi(Value));
	return true;
    }
    if (!strcasecmp(Name, "HideMenu"))
    {
	cDxr3ConfigData::Instance().SetHideMenu(atoi(Value));
	return true;
    }
    if (!strcasecmp(Name, "OsdFlushRate"))
    {
	cDxr3ConfigData::Instance().SetOsdFlushRate(atoi(Value));
	return true;
    }
    if (!strcasecmp(Name, "Brightness"))
    {
	cDxr3ConfigData::Instance().SetBrightness(atoi(Value));
	return true;
    }
    if (!strcasecmp(Name, "Contrast"))
    {
	cDxr3ConfigData::Instance().SetContrast(atoi(Value));
	return true;
    }
    if (!strcasecmp(Name, "Saturation"))
    {
	cDxr3ConfigData::Instance().SetSaturation(atoi(Value));
	return true;
    }

    return false;
}

// ==================================
const char* cPluginDxr3::MainMenuEntry()
{
    return cDxr3ConfigData::Instance().GetHideMenu() ?
	NULL : tr(MAINMENUENTRY);
}

// ==================================
cOsdObject* cPluginDxr3::MainMenuAction()
{
    return new cDxr3OsdMenu;
}

VDRPLUGINCREATOR(cPluginDxr3); // Don't touch this!

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
