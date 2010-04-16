/*
 *           _                 _             _                 _          _____
 * __   ____| |_ __      _ __ | |_   _  __ _(_)_ __         __| |_  ___ _|___ /
 * \ \ / / _` | '__|____| '_ \| | | | |/ _` | | '_ \ _____ / _` \ \/ / '__||_ \
 *  \ V / (_| | | |_____| |_) | | |_| | (_| | | | | |_____| (_| |>  <| |  ___) |
 *   \_/ \__,_|_|       | .__/|_|\__,_|\__, |_|_| |_|      \__,_/_/\_\_| |____/
 *                      |_|            |___/
 *
 * Copyright (C) 2004-2010 Christian Gmeiner
 *
 * This file is part of vdr-plugin-dxr3.
 *
 * vdr-plugin-dxr3 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation version 2.
 *
 * vdr-plugin-dxr3 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with dxr3-plugin.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef SETTINGS_H
#define SETTINGS_H

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

enum AudioDriver {
    OSS = 0,
    ALSA,
#ifdef PULSEAUDIO
    PA
#endif
};

enum Ac3AudioMode {
	PCM_ENCAPSULATION = 0,
	AC3_PASSTHROUGH
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
                  hideMenu(0), useWss(0), loadFirmware(false), audioDriver(OSS),
                  ac3AudioMode(PCM_ENCAPSULATION) {}

    bool processArgs(int argc, char *argv[]);

    Accessors<int> useDigitalOut;
    Accessors<int> card;
    Accessors<int> forceLetterBox;
    Accessors<eVideoMode> videoMode;

    Accessors<int> brightness;
    Accessors<int> contrast;
    Accessors<int> saturation;

    Accessors<int> hideMenu;
    Accessors<int> useWss;
    Accessors<bool> loadFirmware;
    Accessors<AudioDriver> audioDriver;
    Accessors<Ac3AudioMode> ac3AudioMode;
};

#endif /*SETTINGS_H*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// tab-width: 4;
// indent-tabs-mode: nil
// End:
