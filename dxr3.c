/*
* dxr3.c: A plugin for the Video Disk Recorder
*
* See the README file for copyright information and how to reach the author.
*
* $Id: dxr3.c,v 1.1 2004/08/05 23:05:21 austriancoder Exp $
*/


#include "dxr3vdrincludes.h"
#include "dxr3device.h"
#include "dxr3syncbuffer.h"
#include "dxr3configdata.h"
#include "dxr3interface.h"
#include "dxr3.h"

static const char *VERSION        = "0.2.3-pre3-cvs";
static const char *DESCRIPTION    = "DXR3-MPEG decoder plugin";
static const char *MAINMENUENTRY  = "DXR3";

#include "dxr3cpu.h"

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
			cDxr3Device::Instance().Reset();
			break;

		case DXR3_FORCE_LETTER_BOX:
			cDxr3ConfigData::Instance().SetForceLetterBox(!cDxr3ConfigData::Instance().GetForceLetterBox());
			break;

		case DXR3_ANALOG_OUT:
			cLog::Instance() << "Changing audio to analog\n";
			cDxr3ConfigData::Instance().SetUseDigitalOut(0);
			cDxr3ConfigData::Instance().SetAc3OutPut(0);
			cDxr3Device::Instance().Reset();            
			break;

		case DXR3_DIGITAL_OUT:
			cLog::Instance() << "Changing audio to digital\n";
			cDxr3ConfigData::Instance().SetUseDigitalOut(1);
			cDxr3ConfigData::Instance().SetAc3OutPut(0);
			cDxr3Device::Instance().Reset();                    
			break;

		case DXR3_AC3_OUT:
			cLog::Instance() << "Changing audio to ac3\n";
			cDxr3ConfigData::Instance().SetAc3OutPut(!cDxr3ConfigData::Instance().GetAc3OutPut());
			cDxr3Device::Instance().Reset();                    
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
	Add(new cMenuEditIntItem(tr("DXR3 card"), &newDxr3Card));
	newVideoMode = (int) cDxr3ConfigData::Instance().GetVideoMode();
	Add(new cMenuEditStraItem(tr("DXR3 video mode"), &newVideoMode, 3, menuVideoModes));
	newDebug = (int) cDxr3ConfigData::Instance().GetDebug();
	Add(new cMenuEditBoolItem(tr("Debug mode"), &newDebug));
	newDebugLevel = (int) cDxr3ConfigData::Instance().GetDebugLevel();
	Add(new cMenuEditStraItem(tr("Debug level"), &newDebugLevel, 2, menuDebugModes));
}

// ==================================
// save menu values
void cMenuSetupDxr3::Store(void)
{  
	SetupStore("UseDigitalOut", cDxr3ConfigData::Instance().SetUseDigitalOut(newUseDigitalOut));
	SetupStore("Dxr3Card",      cDxr3ConfigData::Instance().SetDxr3Card(newDxr3Card));
	SetupStore("Dxr3VideoMode", cDxr3ConfigData::Instance().SetVideoMode((eVideoMode) newVideoMode));
	SetupStore("Dxr3Debug", cDxr3ConfigData::Instance().SetDebug(newDebug));
	SetupStore("Dxr3DebugLevel", cDxr3ConfigData::Instance().SetDebugLevel(newDebugLevel));
}

// ==================================
class cPluginDxr3 : public cPlugin 
{
private:
	// Add any member variables or functions you may need here.
public:
	cPluginDxr3();    
	~cPluginDxr3();
	const char *Version()			{ return VERSION; }
	const char *Description()		{ return DESCRIPTION; }
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
	new cDxr3CPU();
	cDxr3Device::Instance();
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
	// Parse your own setup parameters and store their values.
	if (!strcasecmp(Name, "UseDigitalOut"))		{ cDxr3ConfigData::Instance().SetUseDigitalOut(atoi(Value)); return true; }
	if (!strcasecmp(Name, "Dxr3Card"))			{ cDxr3ConfigData::Instance().SetDxr3Card(atoi(Value));      return true; }
	if (!strcasecmp(Name, "Dxr3Debug"))			{ cDxr3ConfigData::Instance().SetDebug(atoi(Value));      return true; }
	if (!strcasecmp(Name, "Dxr3VideoMode"))		{ cDxr3ConfigData::Instance().SetVideoMode((eVideoMode) atoi(Value)); return true;}
	if (!strcasecmp(Name, "Dxr3DebugLevel"))	{ cDxr3ConfigData::Instance().SetDebugLevel(atoi(Value)); return true;}
	
	return false;
}

// ==================================
const char* cPluginDxr3::MainMenuEntry() 
{
	return tr(MAINMENUENTRY);
}

// ==================================
cOsdObject* cPluginDxr3::MainMenuAction() 
{
	return new cDxr3OsdMenu;
}

VDRPLUGINCREATOR(cPluginDxr3); // Don't touch this!
