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

#ifndef SPUENCODER_H
#define SPUENCODER_H

#include <stdint.h>
#include <vdr/osd.h>

static const int MAX_SPU_DATA = 65220;  // TODO vaidate this value

class cSpuEncoder {
public:
    void clearOsd();
    void encode(cBitmap *bmap, int top, int left);

private:
    cBitmap *bitmap;

    uint8_t spu[MAX_SPU_DATA];
    uint8_t *p;                 // pointer to current spu data
    uint8_t nholder;            // nibble holder
    uint32_t ncnt;              // nibble count
    int32_t written;            // how much data are written

    int numColors;              // len of tColor array of current bitmap
    const tColor* colors;       // pointer to tColor array from current bitmap
    tColor opacity[16];
    tColor palcolors[16];

    void writeNibble(uint8_t val);
    void generateColorPalette();
};

#endif // SPUENCODER_H
