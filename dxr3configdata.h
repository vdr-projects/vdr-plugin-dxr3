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

    int GetUseDigitalOut() const						{ return UseDigitalOut; }
    int SetUseDigitalOut(int value)						{ return UseDigitalOut = value; }
    int GetDxr3Card() const								{ return Dxr3Card; }
    int SetDxr3Card(int value)							{ return Dxr3Card = value; }
    int GetForceLetterBox()	const						{ return ForceLetterBox; }
    int SetForceLetterBox(int value)					{ return ForceLetterBox = value; }
    int GetAc3OutPut() const							{ return Ac3OutPut; }
    int SetAc3OutPut(int value)							{ return Ac3OutPut = value;}

    eVideoMode GetVideoMode() const						{ return m_videoMode;}
    eVideoMode SetVideoMode(eVideoMode videoMode)		{ return m_videoMode = videoMode;}
    eMenuMode GetMenuMode()	const						{ return m_menuMode; }
    eMenuMode SetMenuMode(eMenuMode menuMode)			{ return m_menuMode = menuMode; }

	int GetDebug() const								{ return m_debug; }
	int SetDebug(int value)								{ return m_debug = value; }
	int GetDebugLevel() const							{ return m_debuglevel; }
	int SetDebugLevel(int value)						{ return m_debuglevel = value; }

	// some little helpers to save some writing
	int GetDebugLow() const								{ return (m_debug && !m_debuglevel); }
	int GetDebugEverything() const						{ if (m_debug == 1 && m_debuglevel == 0) { return 1; } else { return 0; } }

protected:
    int UseDigitalOut;
    int Dxr3Card;
    int ForceLetterBox;
    int Ac3OutPut;
    eVideoMode m_videoMode;
    eMenuMode m_menuMode;
	int m_debug;
	int m_debuglevel;
};

#endif /*_DXR3_CONFIGDATA_H_*/
