/*
 * dxr3osd.c
 *
 * Copyright (C) 2002 Stefan Schluenss
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

#include "dxr3interface.h"
#include "spuencoder.h"

#include "dxr3osd.h"
#include "dxr3interface_spu_encoder.h"
#include "scaler.h"

// ==================================
// ! create osd at (Left, Top, Level)
cOsd *cDxr3OsdProvider::CreateOsd(int Left, int Top, uint Level)
{
    return new cDxr3Osd(Left, Top, Level);
}

// Enables some time measure debugging code
// (taken from the osdteletext plugin, thanks folks)
#ifdef timingdebug
#include <sys/timeb.h>
class cTime
{
    // Debugging: Simple class to measure time
    timeb start;
public:
    void Start()
    {
	ftime(&start);
    }
    void Stop(char *txt)
    {
	timeb t;
	ftime(&t);
	int s = t.time-start.time;
	int ms = t.millitm - start.millitm;
	if (ms<0)
	{
	    s--;
	    ms += 1000;
	}
	printf("%s: %i.%03i\n", txt, s, ms);
    }
};
#endif

// ==================================
//! constructor
cDxr3Osd::cDxr3Osd(int Left, int Top, uint Level)
    : cOsd(Left, Top, Level)
{
    mergedBitmap = NULL;
    shown = false;
    Palette = new cPalette(4);
    last = new cTimeMs();
    last->Set(-cDxr3ConfigData::instance()->GetOsdFlushRate());
    Spu = cSPUEncoder::instance();
}

// ==================================
cDxr3Osd::~cDxr3Osd()
{
    SetActive(false);
    delete Palette;
    delete last;

    if (mergedBitmap)
        delete mergedBitmap;
}

// ==================================
void cDxr3Osd::SetActive(bool On)
{
  if (On != Active())
  {
      // Clears the OSD screen image when it becomes active
      // removes it from screen when it becomes inactive
      cOsd::SetActive(On);
      if (On)
      {
	  Spu->Clear();
      }
      else
      {
          cSpuEncoder::instance()->clearOsd();
      }
  }
}

// ==================================
eOsdError cDxr3Osd::CanHandleAreas(const tArea *Areas, int NumAreas)
{

    eOsdError Result = cOsd::CanHandleAreas(Areas, NumAreas);
    if (Result == oeOk)
    {
	if (NumAreas > MAXNUMWINDOWS)
	{
	    return oeTooManyAreas;
	}

	for (int i = 0; i < NumAreas; i++)
	{
	    if (Areas[i].bpp != 1 &&
		Areas[i].bpp != 2 &&
                Areas[i].bpp != 4)
	    {
		return oeBppNotSupported;
	    }

	    if ((Areas[i].Width() & (8 / Areas[i].bpp - 1)) != 0)
	    {
		return oeWrongAlignment;
	    }
	}
    }
    return Result;
}

eOsdError cDxr3Osd::SetAreas(const tArea *Areas, int NumAreas)
{
    if (shown) {
	Spu->Clear();
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

    uint32_t horizontal, vertical;
    cDxr3Interface::instance()->dimension(horizontal, vertical);

    // check if we need to scale the osd
    if (horizontal < 720 || vertical < 576) {

        // TODO
        dsyslog("SCALE ME");
    }

    if (!bmap)
        return;

    // encode bitmap
    if (bmap->Dirty(x1, y1, x2, y2)) {
        cSpuEncoder::instance()->encode(bmap, Top(), Left());
        shown = true;
        bmap->Clean();
     }

#if 0
    if (last->Elapsed() < cDxr3ConfigData::instance()->GetOsdFlushRate())
	return;
    last->Set();

#ifdef timingdebug
    cTime t;
    t.Start();
#endif

    cBitmap *Bitmap;
    int oldi;
    int newi;
    int i;
    int indexfree[16];
    int firstfree = -1;
    int indexnoassigned[16];
    int firstnoassigned = -1;
    bool colfree[16];
    int NumNewColors;
    int NumOldColors;

    //determine the palette used by all bitmaps (without alpha channel)

    cPalette *newPalette = new cPalette(4);
    for (i = 0; i < 16; i++)
	colfree[i]=true;
    for (i = 0; (Bitmap = GetBitmap(i)) != NULL; i++)
    {
	int nc;
	const tColor *col = Bitmap->Colors(nc);
	if (col)
	    for (int kk = 0; kk < nc; kk++)
		newPalette->Index(col[kk] & 0x00FFFFFF);
    }
    const tColor *newColors = newPalette->Colors(NumNewColors);
    const tColor *oldColors = Palette->Colors(NumOldColors);
    // colors already assigned
    for (newi = 0; newi < NumNewColors; newi++)
    {
	for (oldi = 0; oldi < NumOldColors; oldi++)
	{
	    if (newColors[newi] == oldColors[oldi])
	    {
		colfree[oldi] = false;
		break;
	    }
	}
	if (oldi >= NumOldColors)
	{
	    firstnoassigned++;
	    indexnoassigned[firstnoassigned] = newi;
	}
    }
    // unused colors
    for (i = 0; i < NumOldColors; i++)
    {
	if (colfree[i]) {
	    firstfree++;
	    indexfree[firstfree] = i;
	}
    }
    // replace unused colors with unassigned ones
    for (i = 0; i <= firstnoassigned; i++)
    {
	newi = indexnoassigned[i];
	if (firstfree >= 0)
	{
	    Palette->SetColor(indexfree[firstfree], newColors[newi]);
	    firstfree--;
	} else {
	    Palette->Index(newColors[newi]);
	}
    }
    delete newPalette;

    // Shove the bitmaps to the OSD global bitmap
    for (int i = 0; (Bitmap = GetBitmap(i)) != NULL; i++)
    {
	int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	if (Bitmap->Dirty(x1, y1, x2, y2))
	{
	    /* TODO workaround:
	       apparently the bitmap sent to the driver always has to be a
	       multiple of 8 bits wide, and (dx * dy) also has to be a
	       multiple of 8. Fix driver (should be able to handle any size
	       bitmaps!)
	       This isn't actually necessary with this plugin, but since other
	       plugins rely on this behaviour to work correctly, I left it
	       here. It doesn't hurt too much.
	    */

// http://article.gmane.org/gmane.linux.vdr/21572
//#define optimize_bitmap_transfer
#ifdef optimize_bitmap_transfer
	    while ((x1 > 0 || x2 < Bitmap->Width() - 1) &&
		   ((x2 - x1) & 7) != 7)
	    {
		if (x2 < Bitmap->Width() - 1)
		{
		    x2++;
		}
		else if (x1 > 0)
		{
		    x1--;
		}
	    }

	    //TODO "... / 2" <==> Bpp???
	    while ((y1 > 0 || y2 < Bitmap->Height() - 1) &&
		   (((x2 - x1 + 1) * (y2 - y1 + 1) / 2) & 7) != 0)
	    {
		if (y2 < Bitmap->Height() - 1)
		{
		    y2++;
		}
		else if (y1 > 0)
		{
		    y1--;
		}
	    }

	    while ((x1 > 0 || x2 < Bitmap->Width() - 1) &&
		   (((x2 - x1 + 1) * (y2 - y1 + 1) / 2) & 7) != 0)
	    {
		if (x2 < Bitmap->Width() - 1)
		{
		    x2++;
		}
		else if (x1 > 0)
		{
		    x1--;
		}
	    }
#else
	    x1 = 0;
	    y1 = 0;
	    x2 = Bitmap->Width() - 1;
	    y2 = Bitmap->Height() - 1;
#endif

	    Spu->SetPalette(i + 1, Palette, Bitmap);
	    int origx = Left() + Bitmap->X0();
	    int origy = Top() + Bitmap->Y0();
	    Spu->CopyBlockIntoOSD(i + 1, Bitmap->Width(), origx + x1,
				  origy + y1, origx + x2, origy + y2,
				  Bitmap->Data(x1, y1));
	    Bitmap->Clean();
	}
    }

    Spu->Flush(Palette);
    shown = true;
#ifdef timingdebug
    t.Stop("cDxr3SubpictureOsd::Flush");
#endif
#endif
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
// tab-width: 4;
// indent-tabs-mode: nil
// End:
