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

#include "spuregion.h"

cSpuRegion::cSpuRegion() : startLine(0), endLine(0), usedSections(0)
{
    memset(sections, 0, sizeof(sections));
}

uint8_t cSpuRegion::openSections()
{
    // return the count of open sections.
    // keep in mind that it will return usedSections + 1
    // to allow easy for-looping like used in stl containers.
    return usedSections + 1;
}

bool cSpuRegion::newSection()
{
    if (usedSections + 1 == MAX_SECTIONS) {
        return false;
    }

    usedSections++;
    return true;
}

sSection *cSpuRegion::section(uint8_t num)
{
    if (usedSections == MAX_SECTIONS) {
        return NULL;
    }

    return &sections[num];
}

bool cSpuRegion::addIndex(tIndex idx)
{
    sSection *sec = &sections[usedSections];

    if (sec->usedColors == MAX_COLORS) {
        return false;
    }

    sec->cmap[idx] = sec->usedColors;
    sec->colIndex[sec->usedColors++] = idx;
    return true;
}

bool cSpuRegion::containsIndex(tIndex idx)
{
    sSection *sec = &sections[usedSections];

    for (uint8_t i = 0; i < sec->usedColors; i++) {
        if (sec->colIndex[i] == idx) {
            return true;
        }
    }

    return false;
}
