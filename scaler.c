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

#include "scaler.h"
#include <vdr/osd.h>

// scaling based on article "Image Scaling With Bresenham" by Thiadmer Riemersma
// http://www.ddj.com/184405045

static void scaleLine(const tIndex *src, tIndex *dest, int srcWidth, int destWidth)
{
    int numPixels = destWidth;
    int intPart = srcWidth / destWidth;
    int fractPart = srcWidth % destWidth;
    int e = 0;

    while (numPixels-- > 0) {
        *dest++ = *src;
        src += intPart;

        e += fractPart;
        if (e >= destWidth) {
            e -= destWidth;
            src++;
        }
    }
}

static void scaleRect(const tIndex *src, tIndex *dest, int srcWidth, int srcHeight, int destWidth, int destHeight)
{
    int numPixels = destHeight;
    int intPart = (srcHeight / destHeight) * srcWidth;
    int fractPart = srcHeight % destHeight;
    int e = 0;

    while (numPixels-- > 0) {

        scaleLine(src, dest, srcWidth, destWidth);

        dest += destWidth;
        src += intPart;

        e += fractPart;
        if (e >= destHeight) {
            e -= destHeight;
            src += srcWidth;
        }
    }
}

cBitmap *cScaler::scaleBitmap(cBitmap *source, int width, int height)
{
    cBitmap *scaled = new cBitmap(width, height, 4, 0, 0);

    int osdWidth = source->Width();
    int osdHeight = source->Height();

    const tIndex *src = source->Data(0, 0);
    tIndex *dest = (tIndex *)scaled->Data(0, 0);

    scaleRect(src, dest, osdWidth, osdHeight, width, height);

    return scaled;
}
