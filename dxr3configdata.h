#ifndef _DXR3_CONFIGDATA_H_
#define _DXR3_CONFIGDATA_H_

//#include <stdio.h>

#include "dxr3singleton.h"

// ==================================
// possible video modes
enum eVideoMode 
{
	PAL = 0,
	PAL60,
    NTSC
}; 

// ==================================
// possible menu modes
enum eMenuMode 
{
	SUBPICTURE = 0,
    MPEG
}; 

// ==================================
// possible debug levels
enum eDebugLevel
{
	LOW = 0,
	EVERYTHING
};

// ==================================
// global interface to access all config
// datas of this plugin
class cDxr3ConfigData : public Singleton<cDxr3ConfigData>
{
public:
    cDxr3ConfigData();
	~cDxr3ConfigData()	{}

    int GetUseDigitalOut() const						{ return m_digitaloutputt; }
    int SetUseDigitalOut(int value)						{ return m_digitaloutput = value; }
    int GetDxr3Card() const								{ return m_card; }
    int SetDxr3Card(int value)							{ return m_card = value; }
    int GetForceLetterBox()	const						{ return m_forceletterbox; }
    int SetForceLetterBox(int value)					{ return m_forceletterbox = value; }
    int GetAc3OutPut() const							{ return m_ac3output; }
    int SetAc3OutPut(int value)							{ return m_ac3output = value;}

    eVideoMode GetVideoMode() const						{ return m_videomode;}
    eVideoMode SetVideoMode(eVideoMode videoMode)		{ return m_videomode = videoMode;}
    eMenuMode GetMenuMode()	const						{ return m_menumode; }
    eMenuMode SetMenuMode(eMenuMode menuMode)			{ return m_menumode = menuMode; }

	int GetDebug() const								{ return m_debug; }
	int SetDebug(int value)								{ return m_debug = value; }
	int GetDebugLevel() const							{ return m_debuglevel; }
	int SetDebugLevel(int value)						{ return m_debuglevel = value; }

	// some little helpers to save some writing
	int GetDebugLow() const								{ return (m_debug && !m_debuglevel); }
	int GetDebugEverything() const						{ if (m_debug && !m_debuglevel) { return 1; } else { return 0; } }

	int GetBrightness() const							{ return m_brightness; }
	int SetBrightness(int value)						{ return m_brightness = value; }
	int GetContrast() const								{ return m_contrast; }
	int SetContrast(int value)							{ return m_contrast = value; }
	int GetSaturation() const							{ return m_saturation; }
	int SetSaturation(int value)						{ return m_saturation = value; }

protected:
    eVideoMode	m_videomode;
    eMenuMode	m_menumode;

	int m_digitaloutput;
	int m_ac3output;
	int m_card;
	int m_forceletterbox;
	int m_debug;
	int m_debuglevel;
	int m_brightness;
	int m_contrast;
	int m_saturation;
};

#endif /*_DXR3_CONFIGDATA_H_*/
