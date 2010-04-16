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

static const char VERSION[]       = "0.3.0-git";
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
            //cDxr3Interface::instance()->ResetHardware();
	    //cDxr3Device::Instance().Reset();
	    break;

	case DXR3_FORCE_LETTER_BOX:
            cSettings::instance()->forceLetterBox(
                !cSettings::instance()->forceLetterBox());
	    break;

	case DXR3_ANALOG_OUT:
            cSettings::instance()->useDigitalOut(0);
	    //cDxr3Device::Instance().Reset();
	    break;

	case DXR3_DIGITAL_OUT:
            cSettings::instance()->useDigitalOut(1);
	    //cDxr3Device::Instance().Reset();
	    break;
	}
    }

    return Key == kOk ? osBack : cOsdItem::ProcessKey(Key);
}

// ==================================
// setup menu
cMenuSetupDxr3::cMenuSetupDxr3()
{
    newBrightness = cSettings::instance()->brightness();
    Add(new cMenuEditIntItem(tr("Brightness"), &newBrightness, 0, 999));
    newContrast = cSettings::instance()->contrast();
    Add(new cMenuEditIntItem(tr("Contrast"), &newContrast, 0, 999));
    newSaturation = cSettings::instance()->saturation();
    Add(new cMenuEditIntItem(tr("Saturation"), &newSaturation, 0, 999));

    newVideoMode = (int) cSettings::instance()->videoMode();
    menuVideoModes[0] = tr("PAL");
    menuVideoModes[1] = tr("PAL60");
    menuVideoModes[2] = tr("NTSC");
    Add(new cMenuEditStraItem(tr("Video mode"), &newVideoMode, 3, menuVideoModes));

    menuAc3AudioModes[0] = tr("PCM encapsulation");
    menuAc3AudioModes[1] = tr("AC3 passthrough");
    Add(new cMenuEditStraItem(tr("AC3 mode"), &newAc3AudioMode, 2, menuAc3AudioModes));

    newUseWSS = cSettings::instance()->useWss();
#ifdef EM8300_IOCTL_SET_WSS
    Add(new cMenuEditBoolItem(tr("Use widescreen signaling (WSS)"), &newUseWSS));
#endif
    newUseDigitalOut = cSettings::instance()->useDigitalOut();
    Add(new cMenuEditBoolItem(tr("Digital audio output"), &newUseDigitalOut));
    newHideMenu = cSettings::instance()->hideMenu();
    Add(new cMenuEditBoolItem(tr("Hide main menu entry"), &newHideMenu));
    newDxr3Card = cSettings::instance()->card();
    Add(new cMenuEditIntItem(tr("Card number"), &newDxr3Card, 0, DXR3_MAX_CARDS - 1));
}

// ==================================
// save menu values
void cMenuSetupDxr3::Store()
{
    SetupStore("Brightness", cSettings::instance()->brightness(newBrightness));
    SetupStore("Contrast", cSettings::instance()->contrast(newContrast));
    SetupStore("Saturation", cSettings::instance()->saturation(newSaturation));
    SetupStore("Dxr3VideoMode", cSettings::instance()->videoMode((eVideoMode) newVideoMode));
    SetupStore("UseWSS", cSettings::instance()->useWss(newUseWSS));
    SetupStore("UseDigitalOut", cSettings::instance()->useDigitalOut(newUseDigitalOut));
    SetupStore("HideMenu", cSettings::instance()->hideMenu(newHideMenu));
    SetupStore("Dxr3Card", cSettings::instance()->card(newDxr3Card));
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

    virtual const char *CommandLineHelp();
    virtual bool ProcessArgs(int argc, char *argv[]);

    virtual const char **SVDRPHelpPages(void);
    virtual cString SVDRPCommand(const char *Command, const char *Option,
				 int &ReplyCode);

    cMenuSetupPage *SetupMenu();
    bool SetupParse(const char *Name, const char *Value);
};

// ==================================
cPluginDxr3::cPluginDxr3()
{
    // Initialize any member variables here.
    // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
    // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
    cSettings::instance();
}

// ==================================
cPluginDxr3::~cPluginDxr3()
{
}

// ==================================
bool cPluginDxr3::Initialize()
{
    cDxr3Device::instance();
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
        cSettings::instance()->useDigitalOut(atoi(Value));
	return true;
    }
    if (!strcasecmp(Name, "Dxr3Card"))
    {
        cSettings::instance()->card(atoi(Value));
	return true;
    }
    if (!strcasecmp(Name, "Dxr3VideoMode"))
    {
        cSettings::instance()->videoMode((eVideoMode) atoi(Value));
	return true;
    }
    if (!strcasecmp(Name, "UseWSS"))
    {
        cSettings::instance()->useWss(atoi(Value));
	return true;
    }
    if (!strcasecmp(Name, "HideMenu"))
    {
        cSettings::instance()->hideMenu(atoi(Value));
	return true;
    }
    if (!strcasecmp(Name, "Brightness"))
    {
        cSettings::instance()->brightness(atoi(Value));
	return true;
    }
    if (!strcasecmp(Name, "Contrast"))
    {
        cSettings::instance()->contrast(atoi(Value));
	return true;
    }
    if (!strcasecmp(Name, "Saturation"))
    {
        cSettings::instance()->saturation(atoi(Value));
	return true;
    }

    return false;
}

// ==================================
const char* cPluginDxr3::MainMenuEntry()
{
    return cSettings::instance()->hideMenu() ?
	NULL : tr(MAINMENUENTRY);
}

// ==================================
cOsdObject* cPluginDxr3::MainMenuAction()
{
    return new cDxr3OsdMenu;
}

const char *cPluginDxr3::CommandLineHelp()
{
    return "  -f          --firmware-loading        Enable automatic firmware loading\n" \
           "  -a DRIVER   --audio-driver=DRIVER     Select wanted audio driver" \
           "                                         - alsa "
#ifdef PULSEAUDIO
           "                                         - pa (PulseAudio)"
#endif
           "                                         - oss (default)";
}

bool cPluginDxr3::ProcessArgs(int argc, char *argv[])
{
    return cSettings::instance()->processArgs(argc, argv);
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
        cSettings::instance()->brightness(value);
        //cDxr3Interface::instance()->updateBcsValues();
        return cString::sprintf("Brightness set to %d", value);
    }
    if (!strcasecmp(Command, "CON")) {
        cSettings::instance()->contrast(value);
        //cDxr3Interface::instance()->updateBcsValues();
        return cString::sprintf("Contrast set to %d", value);
    }
    if (!strcasecmp(Command, "SAT")) {
        cSettings::instance()->saturation(value);
        //cDxr3Interface::instance()->updateBcsValues();
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
        cDxr3Device::instance()->turnPlugin(true);
        return "vdr-plugin-dxr3 turned on";
    }
    if (!strcasecmp(Command, "DOF")) {
        cDxr3Device::instance()->turnPlugin(false);
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
