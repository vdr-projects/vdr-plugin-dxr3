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

#ifndef _DXR3_OSD_H_
#define _DXR3_OSD_H_

#include <utility>
#include <vdr/osd.h>

// ==================================
// osd plugin provider
class cDxr3OsdProvider : public cOsdProvider {
public:
    cDxr3OsdProvider() {}
    virtual cOsd *CreateOsd(int Left, int Top, uint Level);
};

static const int MAXNUMWINDOWS = 7;
typedef std::pair<tIndex, tIndex> transPair;

// ==================================
// osd interface
class cDxr3Osd : public cOsd {
public:
    cDxr3Osd(int Left, int Top, uint Level);
    virtual ~cDxr3Osd();

    virtual eOsdError CanHandleAreas(const tArea *Areas, int NumAreas);
    virtual eOsdError SetAreas(const tArea *Areas, int NumAreas);

    virtual void Flush();

protected:
    virtual void SetActive(bool On);

private:
    bool shown;			///< is the osd shown?
    cPalette* Palette;		///< global palette (needed by all bitmaps)
    tArea areas[MAXNUMWINDOWS];
    int numAreas;
    cBitmap *mergedBitmap;

    void copy(cBitmap *part, int area, transPair pair[16], int numPair);
};

#endif /*_DXR3_OSD_H_*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// tab-width: 4;
// indent-tabs-mode: nil
// End:
