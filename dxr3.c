/*
* dxr3.c: A plugin for the Video Disk Recorder
*
* See the README file for copyright information and how to reach the author.
*
* $Id: dxr3.c,v 1.3 2005/03/22 07:52:40 scop Exp $
*/


#include "dxr3vdrincludes.h"
#include "dxr3device.h"
#include "dxr3syncbuffer.h"
#include "dxr3configdata.h"
#include "dxr3interface.h"
#include "dxr3i18n.h"
#include "dxr3cpu.h"
#include "dxr3dolbydigital.h"
#include "dxr3.h"

static const char *VERSION        = "0.3.0-cvs";
static const char *DESCRIPTION    = "DXR3-MPEG decoder plugin";
static const char *MAINMENUENTRY  = "DXR3";

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
// special osd item to change color settings
cDxr3OsdColorItem::cDxr3OsdColorItem(const char* Name, eDxr3ColorItem item)
:cMenuEditItem(Name), m_item(item)
{
	m_value = 0;
	m_min = 0;
	m_max = 32;

	// get vale from setup.conf
	switch (m_item)
	{
	case DXR3_BRIGHTNESS:
		m_value = cDxr3ConfigData::Instance().GetBrightness();
		break;

	case DXR3_CONTRAST:
		m_value = cDxr3ConfigData::Instance().GetContrast();
		break;

	case DXR3_SATURATION:
		m_value = cDxr3ConfigData::Instance().GetSaturation();
		break;
	};

	Set();
}

// ==================================
// react to keypresses
eOSState cDxr3OsdColorItem::ProcessKey(eKeys Key)
{
	eOSState state = cMenuEditItem::ProcessKey(Key);
	
	if (state == osUnknown) 
	{
		int newValue = m_value;
		Key = NORMALKEY(Key);
		switch (Key) 
		{
		case kNone: break;
		case k0 ... k9:
            if (fresh) 
			{
				m_value = 0;
				fresh = false;
			}
            newValue = m_value * 10 + (Key - k0);
            break;
		case kLeft: // TODO might want to increase the delta if repeated quickly?
            newValue = m_value - 1;
            fresh = true;
            break;
		case kRight:
            newValue = m_value + 1;
            fresh = true;
            break;
		default:
            if (m_value < m_min) { m_value = m_min; Set(); }
            if (m_value > m_max) { m_value = m_max; Set(); }
            return state;
		}
		if ((!fresh || m_min <= newValue) && newValue <= m_max) 
		{
			m_value = newValue;
			Set();
        }
		state = osContinue;
	}
	return state;
}

// ==================================
// set caption and call hardware ioctl's
void cDxr3OsdColorItem::Set()
{
	// real value with rounding
	int realValue = int(m_value * 31.25);
	
	char buf[16];
	snprintf(buf, sizeof(buf), "%s", "Hallo");
	SetValue(buf);
}

/*
// ==================================
// special osd item to change color settings
cDxr3OsdColorItem::cDxr3OsdColorItem(const char* Name, eDxr3ColorItem item)
:cOsdItem(Name), m_item(item)
{
	// get vale from setup.conf
	switch (m_item)
	{
	case DXR3_BRIGHTNESS:
		m_value = cDxr3ConfigData::Instance().GetBrightness();
		break;

	case DXR3_CONTRAST:
		m_value = cDxr3ConfigData::Instance().GetContrast();
		break;

	case DXR3_SATURATION:
		m_value = cDxr3ConfigData::Instance().GetSaturation();
		break;
	};

	// allowed values: 0 - 32
	m_min = 0;
	m_max = 32;

	m_name = strdup(Name);
	m_caption = NULL;

	Set();
}

// ==================================
// set caption and call hardware ioctl's
void cDxr3OsdColorItem::Set()
{
/*
	SetValue(SettingBar[m_value]);

	// real value with rounding
	int realValue = int(m_value * 31.25);

	switch (m_item)
	{
	case DXR3_BRIGHTNESS:
		cDxr3Interface::Instance().SetBrightness(realValue);
		cDxr3ConfigData::Instance().SetBrightness(m_value);
		break;

	case DXR3_CONTRAST:
		cDxr3Interface::Instance().SetContrast(realValue);
		cDxr3ConfigData::Instance().SetContrast(m_value);
		break;

	case DXR3_SATURATION:
		cDxr3Interface::Instance().SetSaturation(realValue);
		cDxr3ConfigData::Instance().SetSaturation(m_value);
		break;
	};
*//*
}

// ==================================
void cDxr3OsdColorItem::SetValue(const char *Value)
{
/*
	free(m_caption);
	m_caption = strdup(Value);
	char *buffer = NULL;
	asprintf(&buffer, "%s:\t%s", m_name, m_caption);
	SetText(buffer, false);
	cStatus::MsgOsdCurrentItem(buffer);
*//*
}

// ==================================
// react to keypresses
eOSState cDxr3OsdColorItem::ProcessKey(eKeys Key)
{
/*
	eOSState state = osUnknown;
	
	if (state == osUnknown) 
	{
		int newValue = m_value;
		Key = NORMALKEY(Key);
		
		switch (Key) 
		{
		case kNone: 
			break;
			
		case k0 ... k9:
            if (fresh) 
			{
				m_value = 0;
				fresh = false;
            }
            newValue = m_value * 10 + (Key - k0);
            break;
			
		case kLeft: // TODO might want to increase the delta if repeated quickly?
            newValue = m_value - 1;
            fresh = true;
            break;
			
		case kRight:
            newValue = m_value + 1;
            fresh = true;
            break;
			
		default:
            if (m_value < m_min) { m_value = m_min; Set(); }
            if (m_value > m_max) { m_value = m_max; Set(); }
            return state;
		}
		
		if ((!fresh || m_min <= newValue) && newValue <= m_max) 
		{
			m_value = newValue;
			Set();
        }
		state = osContinue;
	}
	return state;
*/
//}


// ==================================
// the main menu of the plugin
cDxr3OsdMenu::cDxr3OsdMenu() : cOsdMenu(tr("DXR3 Adjustment"))
{
	b = 0;
	c = 10;
	s = 0;

	Clear();
	SetHasHotkeys();
	Add(new cDxr3OsdItem(tr("Reset DXR3 Hardware"), DXR3_RESET_HARDWARE));        
	Add(new cDxr3OsdItem(tr("Toggle Force LetterBox"), DXR3_FORCE_LETTER_BOX));

	// switch between differen output modes
	if (cDxr3ConfigData::Instance().GetUseDigitalOut())
	{
		Add(new cDxr3OsdItem(tr("Analog Output"), DXR3_ANALOG_OUT));
	}
	else
	{
		Add(new cDxr3OsdItem(tr("Digital Output"), DXR3_DIGITAL_OUT));
	}

	//SettingBar
	Add(new cDxr3OsdColorItem(tr("Brightness"), DXR3_BRIGHTNESS));
//	Add(new cMenuEditStraItem(tr("Brightness"),	&b, 33, SettingBar));
//	Add(new cMenuEditStraItem(tr("Contrast"),	&c, 33, SettingBar));
//	Add(new cMenuEditStraItem(tr("Saturation"),	&s, 33, SettingBar));
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
	SetupStore("UseDigitalOut",		cDxr3ConfigData::Instance().SetUseDigitalOut(newUseDigitalOut));
	SetupStore("Dxr3Card",			cDxr3ConfigData::Instance().SetDxr3Card(newDxr3Card));
	SetupStore("Dxr3VideoMode",		cDxr3ConfigData::Instance().SetVideoMode((eVideoMode) newVideoMode));
	SetupStore("Dxr3Debug",			cDxr3ConfigData::Instance().SetDebug(newDebug));
	SetupStore("Dxr3DebugLevel",	cDxr3ConfigData::Instance().SetDebugLevel(newDebugLevel));
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
	new cDxr3DolbyDigital();
    return true;
}

// ==================================
bool cPluginDxr3::Initialize() 
{
	RegisterI18n(Phrases);

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
	if (!strcasecmp(Name, "Dxr3Contrast"))		{ cDxr3ConfigData::Instance().SetContrast(atoi(Value)); return true;}
	if (!strcasecmp(Name, "Dxr3Brightness"))	{ cDxr3ConfigData::Instance().SetBrightness(atoi(Value)); return true;}
	if (!strcasecmp(Name, "Dxr3Saturation"))	{ cDxr3ConfigData::Instance().SetSaturation(atoi(Value)); return true;}
	
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
