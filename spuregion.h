/*
 *           _                 _             _                 _          _____
 * __   ____| |_ __      _ __ | |_   _  __ _(_)_ __         __| |_  ___ _|___ /
 * \ \ / / _` | '__|____| '_ \| | | | |/ _` | | '_ \ _____ / _` \ \/ / '__||_ \
 *  \ V / (_| | | |_____| |_) | | |_| | (_| | | | | |_____| (_| |>  <| |  ___) |
 *   \_/ \__,_|_|       | .__/|_|\__,_|\__, |_|_| |_|      \__,_/_/\_\_| |____/
 *                      |_|            |___/
 *
 * Copyright (C) 2009 Christian Gmeiner
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

#ifndef SPUREGION_H
#define SPUREGION_H

#include <vdr/osd.h>

const static int MAX_COLORS = 4;

struct sSection {
    uint16_t startColumn;

    uint8_t cmap[16];
    tIndex colIndex[MAX_COLORS];
    uint8_t usedColors;
};

class cSpuRegion {
public:
    cSpuRegion();

    uint16_t startLine;
    uint16_t endLine;
    sSection sections[15];  // sections per region

    bool addColIndex(tIndex idx);
    bool containsColIndex(tIndex idx);

private:
    uint8_t usedSections;
};

#endif // SPUREGION_H
