/*
 *           _                 _             _                 _          _____
 * __   ____| |_ __      _ __ | |_   _  __ _(_)_ __         __| |_  ___ _|___ /
 * \ \ / / _` | '__|____| '_ \| | | | |/ _` | | '_ \ _____ / _` \ \/ / '__||_ \
 *  \ V / (_| | | |_____| |_) | | |_| | (_| | | | | |_____| (_| |>  <| |  ___) |
 *   \_/ \__,_|_|       | .__/|_|\__,_|\__, |_|_| |_|      \__,_/_/\_\_| |____/
 *                      |_|            |___/
 *
 * Copyright (C) 2010 Christian Gmeiner
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

#include <getopt.h>
#include <vdr/tools.h>
#include "settings.h"

bool cSettings::processArgs(int argc, char *argv[])
{
    static struct option long_options[] = {
        { "firmware-loading", no_argument, NULL, 'f' },
        { "audio-driver", required_argument, NULL, 'a' },
        { NULL }
    };

    int c;
    while ((c = getopt_long(argc, argv, "f:a", long_options, NULL)) != -1) {
        switch (c) {
        case 'f':
            dsyslog("[dxr3-settings] enableing automatic firmware loading");
            loadFirmware(true);
            break;

        case 'a':
            if (optarg && strcmp(optarg, "alsa") == 0) {
                audioDriver(ALSA);
#ifdef PULSEAUDIO
            } else if (optarg && strcmp(optarg, "pa") == 0) {
                audioDriver(PA);
#endif
            } else {
                esyslog("[dxr3-settings] unkown audio driver - default audio driver will be used");
            }
            break;

        default:
            return false;
        }
    }
    return true;
}

void cSettings::registerObserver(iSettingsObserver *observer)
{
    if (!observer)
        return;

    observers.push_back(observer);
}

void cSettings::emitChange(SettingsChange change)
{
    std::vector<iSettingsObserver *>::iterator iter = observers.begin();

    while(iter != observers.end()) {
        (*iter)->settingsChange(change);
        iter++;
    }
}
