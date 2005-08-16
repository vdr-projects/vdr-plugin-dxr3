/*
 * dxr3configdata.h
 *
 * Copyright (C) 2002-2004 Kai Möller
 * Copyright (C) 2004-2005 Christian Gmeiner
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef _DXR3_CONFIGDATA_H_
#define _DXR3_CONFIGDATA_H_

#include "dxr3singleton.h"

// ==================================
//! possible video modes
enum eVideoMode
{
    PAL = 0,		///< use PAL as videomode
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
//! global interface to access all config datas of this plugin
/*
  With this singleton you can access very easy all possible
  config settings of the plugin.
*/
class cDxr3ConfigData : public Singleton<cDxr3ConfigData>
{
public:
    cDxr3ConfigData();
    ~cDxr3ConfigData() {}

    inline int GetUseDigitalOut() const     { return m_digitaloutput; }
    inline int SetUseDigitalOut(int value)  { return m_digitaloutput = value; }
    inline int GetDxr3Card() const          { return m_card; }
    inline int SetDxr3Card(int value)       { return m_card = value; }
    inline int GetForceLetterBox() const    { return m_forceletterbox; }
    inline int SetForceLetterBox(int value) { return m_forceletterbox = value; }
    inline int GetAc3OutPut() const         { return m_ac3output; }
    inline int SetAc3OutPut(int value)      { return m_ac3output = value; }

    inline eVideoMode GetVideoMode() const                 { return m_videomode; }
    inline eVideoMode SetVideoMode(eVideoMode videoMode)   { return m_videomode = videoMode; }
    inline eMenuMode GetMenuMode() const                   { return m_menumode; }
    inline eMenuMode SetMenuMode(eMenuMode menuMode)       { return m_menumode = menuMode; }

    inline int GetBrightness() const       { return m_brightness; }
    inline int SetBrightness(int value)    { return m_brightness = value;}
    inline int GetContrast() const         { return m_contrast;}
    inline int SetContrast(int value)      { return m_contrast = value; }
    inline int GetSaturation() const       { return m_saturation; }
    inline int SetSaturation(int value)    { return m_saturation = value; }

    inline int GetHideMenu() const     { return m_hidemenu; }
    inline int SetHideMenu(int value)  { return m_hidemenu = value;}

    inline unsigned int GetOsdFlushRate() const    { return (unsigned) m_osdflushrate; }
    inline int SetOsdFlushRate(int value)          { return m_osdflushrate = value; }

protected:
    eVideoMode m_videomode;
    eMenuMode m_menumode;

    int m_digitaloutput;
    int m_ac3output;
    int m_card;
    int m_forceletterbox;
    int m_brightness;
    int m_contrast;
    int m_saturation;
    int m_hidemenu;
    int m_osdflushrate;
};

#endif /*_DXR3_CONFIGDATA_H_*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
