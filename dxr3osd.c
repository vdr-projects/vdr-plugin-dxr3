/*
 *           _                 _             _                 _          _____
 * __   ____| |_ __      _ __ | |_   _  __ _(_)_ __         __| |_  ___ _|___ /
 * \ \ / / _` | '__|____| '_ \| | | | |/ _` | | '_ \ _____ / _` \ \/ / '__||_ \
 *  \ V / (_| | | |_____| |_) | | |_| | (_| | | | | |_____| (_| |>  <| |  ___) |
 *   \_/ \__,_|_|       | .__/|_|\__,_|\__, |_|_| |_|      \__,_/_/\_\_| |____/
 *                      |_|            |___/
 *
 * Copyright (C) 2002 Stefan Schluenss
 * Copyright (C) 2004-2009 Christian Gmeiner
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

#include "dxr3osd.h"
#include "dxr3interface.h"
#include "spuencoder.h"
#include "scaler.h"

// ==================================
// ! create osd at (Left, Top, Level)
cOsd *cDxr3OsdProvider::CreateOsd(int Left, int Top, uint Level)
{
    return new cDxr3Osd(Left, Top, Level);
}

// ==================================
//! constructor
cDxr3Osd::cDxr3Osd(int Left, int Top, uint Level)
    : cOsd(Left, Top, Level)
{
    mergedBitmap = NULL;
    shown = false;
    Palette = new cPalette(4);
}

// ==================================
cDxr3Osd::~cDxr3Osd()
{
    SetActive(false);
    delete Palette;

    if (mergedBitmap)
        delete mergedBitmap;
}

// ==================================
void cDxr3Osd::SetActive(bool On)
{
    if (On != Active()) {
        cOsd::SetActive(On);
        if (!On) {
            cSpuEncoder::instance()->clearOsd();
        }
    }
}

// ==================================
eOsdError cDxr3Osd::CanHandleAreas(const tArea *Areas, int NumAreas)
{

    eOsdError Result = cOsd::CanHandleAreas(Areas, NumAreas);
    if (Result == oeOk) {

        if (NumAreas > MAXNUMWINDOWS) {
	    return oeTooManyAreas;
	}

        for (int i = 0; i < NumAreas; i++) {
	    if (Areas[i].bpp != 1 &&
		Areas[i].bpp != 2 &&
                Areas[i].bpp != 4)
	    {
		return oeBppNotSupported;
	    }

            if ((Areas[i].Width() & (8 / Areas[i].bpp - 1)) != 0) {
		return oeWrongAlignment;
	    }
	}
    }
    return Result;
}

eOsdError cDxr3Osd::SetAreas(const tArea *Areas, int NumAreas)
{
    if (shown) {
	shown = false;
    }

    // store area informations
    memcpy(areas, Areas, sizeof(tArea) * NumAreas);
    numAreas = NumAreas;

    // allocate a new bitmap, which will contain all areas
    if (numAreas > 1) {

        int w = areas[numAreas - 1].x2;
        int h = areas[numAreas - 1].y2;

        if (mergedBitmap)
            delete mergedBitmap;

        mergedBitmap = new cBitmap(w, h, 4, 0, 0);

        // reset current used merged palette
        Palette->Reset();
    }

    return cOsd::SetAreas(Areas, NumAreas);
}

// ==================================
void cDxr3Osd::Flush()
{
    if (!Active())
        return;

    cBitmap *bmap = NULL;
    int x1 = 0, y1 = 0, x2 = 0, y2 = 0;

    // check if we need to encode an OSD, which is build by only
    // one area, or if we need to create a big bitmap containing
    // all areas
    if (numAreas == 1) {

        // we only need to set bmap pointer to a valid bitmap
        bmap = GetBitmap(0);

        // set global used palette - is needed if we need to scale the osd
        Palette->Replace(*bmap);

    } else {

        // determine the palette used by all bitmaps
        bool success = true;

        for (int i = 0; i < numAreas; i++) {

            cBitmap *tmp = GetBitmap(i);
            int numColors;
            const tColor *color = tmp->Colors(numColors);
            transPair pair[16];
            int numPair = 0;

            for (int c = 0; c < numColors; c++) {
                int idx = Palette->Index(color[c]);

                if (idx == -1) {
                    success = false;
                    break;
                }

                if (c != idx) {
                    pair[numPair] = transPair(c, idx);
                    numPair++;
                }
            }

            if (!success) {
                esyslog("[dxr3-osd] too many colors used by OSD");
            }

            // if part is not dirty, we can continue to the next area
            if (!tmp->Dirty(x1, y1, x2, y2)) {
                continue;
            }

            // copy data into mergedBitmap and mark part as clean
            copy(tmp, i, pair, numPair);
            tmp->Clean();
        }

        // replace color palette
        mergedBitmap->Replace(*Palette);

        // set pointer to our valid mergedBitmap
        bmap = mergedBitmap;
    }

    // if our final bitmap is not dirty, we need
    // not to scale and draw it
    if (!bmap->Dirty(x1, y1, x2, y2)) {
        return;
    }

    // mark bitmap as clean
    bmap->Clean();

    uint32_t horizontal, vertical;
    cDxr3Interface::instance()->dimension(horizontal, vertical);

    int top = Top();
    int left = Left();
    bool scaling = false;

    // check if we need to scale the osd
    if (horizontal < 720 || vertical < 576) {

        // calculate ratio
        float alpha = 720.f / bmap->Width();
        float beta = 576.f / bmap->Height();

        // apply ration to get size of scaled bitmap
        int width = horizontal / alpha;
        int height = vertical / beta;

        // calculate ratio for top/left
        alpha = 720.f / left;
        beta = 576.f / top;

        // apply ration to top/left
        left = horizontal / alpha;
        top = vertical / beta;

        // scale and set used colors
        cBitmap *scaled = cScaler::scaleBitmap(bmap, width, height);
        scaled->Replace(*Palette);
        scaling = true;
        bmap = scaled;
    }

    // encode bitmap
    cSpuEncoder::instance()->encode(bmap, top, left);
    shown = true;

    // check if we need to free the bitmap allocated by the method
    // cScaler::scaleBitmap
    if (scaling) {
        delete bmap;
    }
}

void cDxr3Osd::copy(cBitmap *part, int area, transPair pair[16], int numPair)
{
    tArea usedArea = areas[area];

    // a small optimization
    if (numPair == 0) {

        // we do not need to transform anything, just do a copy

        for (int x = 0; x < part->Width(); x++) {
            for (int y = 0; y < part->Height(); y++) {

                tIndex val = *part->Data(x, y);
                mergedBitmap->SetIndex(x + usedArea.x1, y + usedArea.y1, val);
            }
        }

    } else {

        // we need to transform and to do a copy

         for (int x = 0; x < part->Width(); x++) {
            for (int y = 0; y < part->Height(); y++) {

                tIndex val = *part->Data(x, y);

                for (int t = 0; t < numPair; t++) {
                    if (val == pair[t].first) {
                        val = pair[t].second;
                        break;
                    }
                }

                mergedBitmap->SetIndex(x + usedArea.x1, y + usedArea.y1, val);
            }
        }
    }
}

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// c-basic-offset: 4
// indent-tabs-mode: nil
// End:
