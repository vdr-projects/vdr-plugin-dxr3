/*
 * dxr3.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: dxr3.c,v 1.1.2.43 2009/01/02 22:59:57 scop Exp $
 *
 */

#include <vdr/plugin.h>
#include "dxr3device.h"
#include "dxr3.h"

static const char VERSION[]       = "0.2.9+git";
static const char DESCRIPTION[]   = trNOOP("Hardware MPEG decoder");

#if APIVERSNUM && APIVERSNUM < 10600
#error "This version of the DXR3 plugin needs VDR API version >= 1.6.0"
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
	    cDxr3Interface::instance()->ResetHardware();
	    //cDxr3Device::Instance().Reset();
	    break;

	case DXR3_FORCE_LETTER_BOX:
	    cDxr3ConfigData::instance()->SetForceLetterBox(
		!cDxr3ConfigData::instance()->GetForceLetterBox());
	    break;

	case DXR3_ANALOG_OUT:
	    cDxr3ConfigData::instance()->SetUseDigitalOut(0);
	    //cDxr3Device::Instance().Reset();
	    break;

	case DXR3_DIGITAL_OUT:
	    cDxr3ConfigData::instance()->SetUseDigitalOut(1);
	    //cDxr3Device::Instance().Reset();
	    break;
	}
    }

    return Key == kOk ? osBack : cOsdItem::ProcessKey(Key);
}

// ==================================
// setup menu
cMenuSetupDxr3::cMenuSetupDxr3(void)
{
    newBrightness = cDxr3ConfigData::instance()->GetBrightness();
    Add(new cMenuEditIntItem(tr("Brightness"),
			     &newBrightness, 0, 999));
    newContrast = cDxr3ConfigData::instance()->GetContrast();
    Add(new cMenuEditIntItem(tr("Contrast"),
			     &newContrast, 0, 999));
    newSaturation = cDxr3ConfigData::instance()->GetSaturation();
    Add(new cMenuEditIntItem(tr("Saturation"),
			     &newSaturation, 0, 999));
    newVideoMode = (int) cDxr3ConfigData::instance()->GetVideoMode();
    menuVideoModes[0] = tr("PAL");
    menuVideoModes[1] = tr("PAL60");
    menuVideoModes[2] = tr("NTSC");
    Add(new cMenuEditStraItem(tr("Video mode"),
			      &newVideoMode, 3, menuVideoModes));
    newUseWSS = cDxr3ConfigData::instance()->GetUseWSS();
#ifdef EM8300_IOCTL_SET_WSS
    Add(new cMenuEditBoolItem(tr("Use widescreen signaling (WSS)"),
			      &newUseWSS));
#endif
    newUseDigitalOut = cDxr3ConfigData::instance()->GetUseDigitalOut();
    Add(new cMenuEditBoolItem(tr("Digital audio output"), &newUseDigitalOut));
    newHideMenu = cDxr3ConfigData::instance()->GetHideMenu();
    Add(new cMenuEditBoolItem(tr("Hide main menu entry"), &newHideMenu));
    newDxr3Card = cDxr3ConfigData::instance()->GetDxr3Card();
    Add(new cMenuEditIntItem(tr("Card number"),
			     &newDxr3Card, 0, DXR3_MAX_CARDS - 1));
}

// ==================================
// save menu values
void cMenuSetupDxr3::Store(void)
{
    SetupStore("Brightness",
	       cDxr3ConfigData::instance()->SetBrightness(newBrightness));
    SetupStore("Contrast",
	       cDxr3ConfigData::instance()->SetContrast(newContrast));
    SetupStore("Saturation",
	       cDxr3ConfigData::instance()->SetSaturation(newSaturation));
    SetupStore("Dxr3VideoMode",
	       cDxr3ConfigData::instance()->SetVideoMode(
		   (eVideoMode) newVideoMode));
    SetupStore("UseWSS",
	       cDxr3ConfigData::instance()->SetUseWSS(newUseWSS));
    SetupStore("UseDigitalOut",
	       cDxr3ConfigData::instance()->SetUseDigitalOut(newUseDigitalOut));
    SetupStore("HideMenu",
	       cDxr3ConfigData::instance()->SetHideMenu(newHideMenu));
    SetupStore("Dxr3Card",
	       cDxr3ConfigData::instance()->SetDxr3Card(newDxr3Card));

    // Apply (some of the) settings
    cDxr3Interface::instance()->updateBcsValues();
    //cDxr3Device::instance()->Reset();
}

// ==================================
class cPluginDxr3 : public cPlugin {
public:
    cPluginDxr3();
    ~cPluginDxr3();

    const char *Version()       { return VERSION; }
    const char *Description()   { return tr(DESCRIPTION); }

    bool Initialize();

    const char* MainMenuEntry();
    cOsdObject* MainMenuAction();

    virtual const char **SVDRPHelpPages(void);
    virtual cString SVDRPCommand(const char *Command, const char *Option,
				 int &ReplyCode);

    cMenuSetupPage *SetupMenu();
    bool SetupParse(const char *Name, const char *Value);

private:
    cDxr3Device *device;
};

// ==================================
cPluginDxr3::cPluginDxr3()
{
    // Initialize any member variables here.
    // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
    // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
    cDxr3ConfigData::instance();
}

// ==================================
cPluginDxr3::~cPluginDxr3()
{
}

// ==================================
bool cPluginDxr3::Initialize()
{
    device = new cDxr3Device();
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
	cDxr3ConfigData::instance()->SetUseDigitalOut(atoi(Value));
	return true;
    }
    if (!strcasecmp(Name, "Dxr3Card"))
    {
	cDxr3ConfigData::instance()->SetDxr3Card(atoi(Value));
	return true;
    }
    if (!strcasecmp(Name, "Dxr3VideoMode"))
    {
	cDxr3ConfigData::instance()->SetVideoMode((eVideoMode) atoi(Value));
	return true;
    }
    if (!strcasecmp(Name, "UseWSS"))
    {
	cDxr3ConfigData::instance()->SetUseWSS(atoi(Value));
	return true;
    }
    if (!strcasecmp(Name, "HideMenu"))
    {
	cDxr3ConfigData::instance()->SetHideMenu(atoi(Value));
	return true;
    }
    if (!strcasecmp(Name, "Brightness"))
    {
	cDxr3ConfigData::instance()->SetBrightness(atoi(Value));
	return true;
    }
    if (!strcasecmp(Name, "Contrast"))
    {
	cDxr3ConfigData::instance()->SetContrast(atoi(Value));
	return true;
    }
    if (!strcasecmp(Name, "Saturation"))
    {
	cDxr3ConfigData::instance()->SetSaturation(atoi(Value));
	return true;
    }

    return false;
}

// ==================================
const char* cPluginDxr3::MainMenuEntry()
{
    return cDxr3ConfigData::instance()->GetHideMenu() ?
	NULL : tr(MAINMENUENTRY);
}

// ==================================
cOsdObject* cPluginDxr3::MainMenuAction()
{
    return new cDxr3OsdMenu;
}

// ==================================
// TODO: localize command descriptions?
const char **cPluginDxr3::SVDRPHelpPages(void)
{
    static const char *HelpPages[] = {
	"SAT\n"
	"    Set saturation (0..999).",
	"CON\n"
	"    Set contrast (0..999).",
	"BRI\n"
	"    Set brightness (0..999).",
	"SAO\n"
	"    Switch to analog audio output.",
	"SDO\n"
	"    Switch to digital PCM audio output.",
	"SAC3\n"
	"    Switch to digital AC3 audio output.",
    "DON\n"
    "    Start plugin - take control of dxr3 card.",
    "DOF\n"
    "    Stop plugin - release used dxr3 card so it can be used by e.g. MMS.",

	NULL
    };

    return HelpPages;
}

// TODO: localize returned strings?
cString cPluginDxr3::SVDRPCommand(const char *Command, const char *Option,
				  int &ReplyCode)
{
    int value = atoi(Option);

    if (!strcasecmp(Command, "BRI")) {
        cDxr3ConfigData::instance()->SetBrightness(value);
        cDxr3Interface::instance()->updateBcsValues();
        return cString::sprintf("Brightness set to %d", value);
    }
    if (!strcasecmp(Command, "CON")) {
        cDxr3ConfigData::instance()->SetContrast(value);
        cDxr3Interface::instance()->updateBcsValues();
        return cString::sprintf("Contrast set to %d", value);
    }
    if (!strcasecmp(Command, "SAT")) {
        cDxr3ConfigData::instance()->SetSaturation(value);
        cDxr3Interface::instance()->updateBcsValues();
        return cString::sprintf("Saturation set to %d", value);
    }
#if 0
    if (!strcasecmp(Command, "SDO")) {
        device->getAudioOutput()->setAudioMode(iAudio::DigitalPcm);
        return "Switched to digital PCM audio output";
    }
    if (!strcasecmp(Command, "SAO")) {
        device->getAudioOutput()->setAudioMode(iAudio::Analog);
        return "Switched to analog audio output";
    }
    if (!strcasecmp(Command, "SAC3")) {
        device->getAudioOutput()->setAudioMode(iAudio::Ac3);
        return "Switched to digital AC3 audio output";
    }
#endif
    if (!strcasecmp(Command, "DON")) {
        device->turnPlugin(true);
        return "vdr-plugin-dxr3 turned on";
    }
    if (!strcasecmp(Command, "DOF")) {
        device->turnPlugin(false);
        return "vdr-plugin-dxr3 turned off";
    }

    return NULL;
}

VDRPLUGINCREATOR(cPluginDxr3); // Don't touch this!

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// tab-width: 4;
// indent-tabs-mode: nil
// End:
