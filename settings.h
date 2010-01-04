/*
 * dxr3configdata.h
 *
 * Copyright (C) 2002-2004 Kai Möller
 * Copyright (C) 2004 Christian Gmeiner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#ifndef _DXR3_CONFIGDATA_H_
#define _DXR3_CONFIGDATA_H_

#include "singleton.h"
#include <linux/em8300.h>

// ==================================
//! possible video modes
enum eVideoMode
{
    PAL      = EM8300_VIDEOMODE_PAL,
    PAL60    = EM8300_VIDEOMODE_PAL60,
    NTSC     = EM8300_VIDEOMODE_NTSC
};

// ==================================
//! global interface to access all config datas of this plugin
/*
  With this singleton you can access very easy all possible
  config settings of the plugin.
*/
class cSettings : public Singleton<cSettings>
{
public:
    cSettings();

    int GetUseDigitalOut() const        { return m_digitaloutput; }
    int SetUseDigitalOut(int value)     { return m_digitaloutput = value; }
    int GetDxr3Card() const             { return m_card; }
    int SetDxr3Card(int value)          { return m_card = value; }
    int GetForceLetterBox() const       { return m_forceletterbox; }
    int SetForceLetterBox(int value)    { return m_forceletterbox = value; }

    eVideoMode GetVideoMode() const         { return m_videomode; }
    eVideoMode SetVideoMode(eVideoMode m)   { return m_videomode = m; }

    int GetBrightness() const           { return m_brightness; }
    int SetBrightness(int value)        { return m_brightness = value; }
    int GetContrast() const             { return m_contrast; }
    int SetContrast(int value)          { return m_contrast = value; }
    int GetSaturation() const           { return m_saturation; }
    int SetSaturation(int value)        { return m_saturation = value; }

    int GetHideMenu() const             { return m_hidemenu; }
    int SetHideMenu(int value)          { return m_hidemenu = value; }

    int GetUseWSS() const               { return m_usewss; }
    int SetUseWSS(int value)            { return m_usewss = value; }

private:
    eVideoMode m_videomode;
    int m_usewss;
    int m_digitaloutput;
    int m_card;
    int m_forceletterbox;
    int m_brightness;
    int m_contrast;
    int m_saturation;
    int m_hidemenu;
};

#endif /*_DXR3_CONFIGDATA_H_*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// tab-width: 4;
// indent-tabs-mode: nil
// End:
