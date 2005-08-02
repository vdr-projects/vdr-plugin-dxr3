/*
 * dxr3.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: dxr3.c,v 1.1.2.16 2005/08/02 11:10:50 scop Exp $
 *
 */

#include "dxr3vdrincludes.h"
#include "dxr3device.h"
#include "dxr3syncbuffer.h"
#include "dxr3configdata.h"
#include "dxr3interface.h"
#include "dxr3.h"
#include "dxr3i18n.h"

static const char *VERSION        = "0.2.3-cvs";
static const char *DESCRIPTION    = "Hardware MPEG decoder";
static const char *MAINMENUENTRY  = "DXR3";

#if VDRVERSNUM && VDRVERSNUM < 10311
#error "This version of the DXR3 plugin needs VDR version >= 1.3.11"
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
	    cDxr3ConfigData::Instance().SetForceLetterBox(!cDxr3ConfigData::Instance().GetForceLetterBox());
	    break;

	case DXR3_ANALOG_OUT:
	    cDxr3ConfigData::Instance().SetUseDigitalOut(0);
	    cDxr3ConfigData::Instance().SetAc3OutPut(0);
	    if (cDxr3Device::InstanceP())
		cDxr3Device::InstanceP()->Reset();
	    break;

	case DXR3_DIGITAL_OUT:
	    cDxr3ConfigData::Instance().SetUseDigitalOut(1);
	    cDxr3ConfigData::Instance().SetAc3OutPut(0);
	    if (cDxr3Device::InstanceP())
		cDxr3Device::InstanceP()->Reset();
	    break;

	case DXR3_AC3_OUT:
	    cDxr3ConfigData::Instance().SetAc3OutPut(!cDxr3ConfigData::Instance().GetAc3OutPut());
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
    newUseDigitalOut = cDxr3ConfigData::Instance().GetUseDigitalOut();
    Add(new cMenuEditBoolItem(tr("Digital audio output"), &newUseDigitalOut));
    newDxr3Card = cDxr3ConfigData::Instance().GetDxr3Card();
    Add(new cMenuEditIntItem(tr("Card number"),
			     &newDxr3Card, 0, DXR3_MAX_CARDS - 1));
    newVideoMode = (int) cDxr3ConfigData::Instance().GetVideoMode();
    menuVideoModes[0] = tr("PAL");
    menuVideoModes[1] = tr("PAL60");
    menuVideoModes[2] = tr("NTSC");
    Add(new cMenuEditStraItem(tr("Video mode"),
			      &newVideoMode, 3, menuVideoModes));
    newHideMenu = cDxr3ConfigData::Instance().GetHideMenu();
    Add(new cMenuEditBoolItem(tr("Hide main menu entry"), &newHideMenu));
    newOsdFlushRate = cDxr3ConfigData::Instance().GetOsdFlushRate();
    Add(new cMenuEditIntItem(tr("OSD flush rate (ms)"),
			     &newOsdFlushRate, 0, 255));
}

// ==================================
// save menu values
void cMenuSetupDxr3::Store(void)
{
    SetupStore("UseDigitalOut",
	       cDxr3ConfigData::Instance().SetUseDigitalOut(newUseDigitalOut));
    SetupStore("Dxr3Card",
	       cDxr3ConfigData::Instance().SetDxr3Card(newDxr3Card));
    SetupStore("Dxr3VideoMode",
	       cDxr3ConfigData::Instance().SetVideoMode((eVideoMode) newVideoMode));
    SetupStore("HideMenu",
	       cDxr3ConfigData::Instance().SetHideMenu(newHideMenu));
    SetupStore("OsdFlushRate",
	       cDxr3ConfigData::Instance().SetOsdFlushRate(newOsdFlushRate));
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
    const char *CommandLineHelp();
    bool ProcessArgs(int argc, char *argv[]);
    bool Initialize();
    bool Start();
    void Housekeeping();
    cMenuSetupPage *SetupMenu();
    bool SetupParse(const char *Name, const char *Value);
    const char* MainMenuEntry();
    cOsdObject* MainMenuAction();
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
const char *cPluginDxr3::CommandLineHelp()
{
    return NULL;
}

// ==================================
bool cPluginDxr3::ProcessArgs(int argc, char *argv[])
{
    return true;
}

// ==================================
bool cPluginDxr3::Start()
{
    return true;
}

// ==================================
bool cPluginDxr3::Initialize()
{
    RegisterI18n(Phrases);

    cDxr3Device::InstanceP();

    return true;
}

// ==================================
void cPluginDxr3::Housekeeping()
{
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
