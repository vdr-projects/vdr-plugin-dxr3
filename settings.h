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
#include "accessors.h"
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
    cSettings() : useDigitalOut(0), card(0), forceLetterBox(0), videoMode(PAL),
                  brightness(500), contrast(500), saturation(500),
                  hideMenu(0), useWss(0) {}

    Accessors<int> useDigitalOut;
    Accessors<int> card;
    Accessors<int> forceLetterBox;
    Accessors<eVideoMode> videoMode;

    Accessors<int> brightness;
    Accessors<int> contrast;
    Accessors<int> saturation;

    Accessors<int> hideMenu;
    Accessors<int> useWss;
};

#endif /*_DXR3_CONFIGDATA_H_*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// tab-width: 4;
// indent-tabs-mode: nil
// End:
