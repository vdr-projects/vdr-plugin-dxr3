/*
 * dxr3configdata.h
 *
 * Copyright (C) 2002-2004 Kai Möller
 * Copyright (C) 2004 Christian Gmeiner
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#ifndef _DXR3_CONFIGDATA_H_
#define _DXR3_CONFIGDATA_H_

#include "dxr3singleton.h"

// ==================================
//! possible video modes
enum eVideoMode 
{
	PAL = 0,	///< use PAL as videomode
	PAL60,		///< use PAL60 as videomode
    NTSC		///< use NTSC as videomode 
}; 

// ==================================
// possible menu modes
enum eMenuMode 
{
	SUBPICTURE = 0,
    MPEG
}; 

// ==================================
//! possible debug levels
enum eDebugLevel
{
	LOW = 0,		///< only log basic warings and errors
	EVERYTHING		///< log everything - note logfile can become very big
};

// ==================================
//! global interface to access all config datas of this plugin
/*
	With this singleton you can access very easy all possible
	config settings of the plugin.
*/
class cDxr3ConfigData : public Singleton<cDxr3ConfigData>
{
public:
    cDxr3ConfigData();
	~cDxr3ConfigData()	{}

    int GetUseDigitalOut() const						{ return m_Digitaloutput; }
    int SetUseDigitalOut(int value)						{ return m_Digitaloutput = value; }
    int GetDxr3Card() const								{ return m_Card; }
    int SetDxr3Card(int value)							{ return m_Card = value; }
    int GetForceLetterBox()	const						{ return m_ForceLetterbox; }
    int SetForceLetterBox(int value)					{ return m_ForceLetterbox = value; }
    int GetAc3OutPut() const							{ return m_AC3output; }
    int SetAc3OutPut(int value)							{ return m_AC3output = value;}

    eVideoMode GetVideoMode() const						{ return m_VideoMode;}
    eVideoMode SetVideoMode(eVideoMode videoMode)		{ return m_VideoMode = videoMode;}
    eMenuMode GetMenuMode()	const						{ return m_MenuMode; }
    eMenuMode SetMenuMode(eMenuMode menuMode)			{ return m_MenuMode = menuMode; }

	int GetDebug() const								{ return m_Debug; }
	int SetDebug(int value)								{ return m_Debug = value; }
	int GetDebugLevel() const							{ return m_DebugLevel; }
	int SetDebugLevel(int value)						{ return m_DebugLevel = value; }

	// some little helpers to save some writing
	int GetDebugLow() const								{ return (m_Debug && !m_DebugLevel); }
	int GetDebugEverything() const						{ if (m_Debug && !m_DebugLevel) { return 1; } else { return 0; } }

	int GetBrightness() const							{ return m_Brightness; }
	int SetBrightness(int value)						{ return m_Brightness = value; }
	int GetContrast() const								{ return m_Contrast; }
	int SetContrast(int value)							{ return m_Contrast = value; }
	int GetSaturation() const							{ return m_Saturation; }
	int SetSaturation(int value)						{ return m_Saturation = value; }

protected:
    eVideoMode	m_VideoMode;///< videomode value
    eMenuMode	m_MenuMode;	///< menumode value

	int m_Digitaloutput;	///< digitaloutput value
	int m_AC3output;		///< ac3output value
	int m_Card;				///< card value
	int m_ForceLetterbox;	///< forcebox value
	int m_Debug;			///< debug value
	int m_DebugLevel;		///< debug level value
	int m_Brightness;		///< brightness value
	int m_Contrast;			///< contrast value
	int m_Saturation;		///< saturation value
};

#endif /*_DXR3_CONFIGDATA_H_*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
