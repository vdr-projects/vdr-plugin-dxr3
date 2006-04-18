/*
 * dxr3spudecoder.c
 *
 * Copyright (C) 2004 Christian Gmeiner
 *
 * Orginal:
 *    Copyright (C) 2001.2002 Andreas Schultz <aschultz@warp10.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <assert.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>

#include "dxr3spudecoder.h"
#include "dxr3interface.h"
#include "dxr3tools.h"

// ==================================
#define CMD_SPU_MENU            0x00
#define CMD_SPU_SHOW            0x01
#define CMD_SPU_HIDE            0x02
#define CMD_SPU_SET_PALETTE     0x03
#define CMD_SPU_SET_ALPHA       0x04
#define CMD_SPU_SET_SIZE        0x05
#define CMD_SPU_SET_PXD_OFFSET  0x06
#define CMD_SPU_CHG_COLCON      0x07
#define CMD_SPU_EOF             0xff

#define spuU32(i) ((spu[i] << 8) + spu[i+1])


/*
 * cDxr3Spubitmap:
 *
 * this is a bitmap of the full screen and two palettes
 * the normal palette for the background and the highlight palette
 *
 * Inputs:
 *  - a SPU rle encoded image on creation, which will be decoded into
 *    the full screen indexed bitmap
 *
 * Output:
 *  - a minimal sized cDxr3SpuBitmap a given palette, the indexed bitmap
 *    will be scanned to get the smallest possible resulting bitmap considering
 *    transparencies
 */

// ==================================
void cDxr3SpuPalette::setPalette(const uint32_t * pal)
{
    for (int i = 0; i < 16; i++)
	palette[i] = Tools::YUV2Rgb(pal[i]);
}

// ==================================
#define setMin(a, b) if (a > b) a = b
#define setMax(a, b) if (a < b) a = b

#define spuXres 720
#define spuYres 576

#define revRect(r1, r2) { r1.x1 = r2.x2; r1.y1 = r2.y2; r1.x2 = r2.x1; r1.y2 = r2.y1; }

// ==================================
cDxr3SpuBitmap::cDxr3SpuBitmap(sDxr3SpuRect size, uint8_t * fodd,
			       uint8_t * eodd, uint8_t * feven,
			       uint8_t * eeven)
{
    if (size.x1 < 0 || size.y1 < 0 || size.x2 >= spuXres || size.y2 >= spuYres)
	throw;

    bmpsize = size;
    revRect(minsize[0], size);
    revRect(minsize[1], size);
    revRect(minsize[2], size);
    revRect(minsize[3], size);

    if (!(bmp = new uint8_t[spuXres * spuYres * sizeof(uint8_t)]))
	throw;

    memset(bmp, 0, spuXres * spuYres * sizeof(uint8_t));
    putFieldData(0, fodd, eodd);
    putFieldData(1, feven, eeven);
}

// ==================================
cDxr3SpuBitmap::~cDxr3SpuBitmap()
{
    delete[]bmp;
}

// ==================================
cBitmap *cDxr3SpuBitmap::getBitmap(const aDxr3SpuPalDescr paldescr,
				   const cDxr3SpuPalette & pal,
				   sDxr3SpuRect & size) const
{
    int h = size.height();
    int w = size.width();

    if (size.y1 + h >= spuYres)
    {
	h = spuYres - size.y1 - 1;
    }
    if (size.x1 + w >= spuXres)
    {
	w = spuXres - size.x1 - 1;
    }

    if (w & 0x03)
    {
	w += 4 - (w & 0x03);
    }

    cBitmap *ret = new cBitmap(w, h, 2);

    // set the palette
    for (int i = 0; i < 4; i++)
    {
	uint32_t color = pal.getColor(paldescr[i].index, paldescr[i].trans);
	ret->SetColor(i, (tColor) color);
    }

    // set the content
    for (int yp = 0; yp < h; yp++)
    {
	for (int xp = 0; xp < w; xp++)
	{
	    uint8_t idx = bmp[(size.y1 + yp) * spuXres + size.x1 + xp];
	    ret->SetIndex(xp, yp, idx);
	}
    }
    return ret;
}

// ==================================
// find the minimum non-transparent area
bool cDxr3SpuBitmap::getMinSize(const aDxr3SpuPalDescr paldescr,
				sDxr3SpuRect & size) const
{
    bool ret = false;
    for (int i = 0; i < 4; i++)
    {
	if (paldescr[i].trans != 0)
	{
	    if (!ret)
	    {
		size = minsize[i];
	    }
	    else
	    {
		setMin(size.x1, minsize[i].x1);
		setMin(size.y1, minsize[i].y1);
		setMax(size.x2, minsize[i].x2);
		setMax(size.y2, minsize[i].y2);
	    }
	    ret = true;
	}
    }
    dsyslog("dxr3: getminsize: (%d,%d) x (%d,%d)",
	    size.x1, size.y1, size.x2, size.y2);
    if (size.x1 > size.x2 || size.y1 > size.y2)
	return false;
    return ret;
}

// ==================================
void cDxr3SpuBitmap::putPixel(int xp, int yp, int len, uint8_t colorid)
{
    memset(bmp + spuXres * yp + xp, colorid, len);
    setMin(minsize[colorid].x1, xp);
    setMin(minsize[colorid].y1, yp);
    setMax(minsize[colorid].x2, xp + len - 1);
    setMax(minsize[colorid].y2, yp + len - 1);
}

// ==================================
static uint8_t getBits(uint8_t * &data, uint8_t & bitf)
{
    uint8_t ret = *data;
    if (bitf)
    {
	ret >>= 4;
    }
    else
    {
	data++;
    }

    bitf ^= 1;

    return (ret & 0xf);
}

// ==================================
void cDxr3SpuBitmap::putFieldData(int field, uint8_t * data, uint8_t * endp)
{
    int xp = bmpsize.x1;
    int yp = bmpsize.y1 + field;
    uint8_t bitf = 1;

    while (data < endp)
    {
	uint16_t vlc = getBits(data, bitf);
	if (vlc < 0x0004)
	{
	    vlc = (vlc << 4) | getBits(data, bitf);
	    if (vlc < 0x0010)
	    {
		vlc = (vlc << 4) | getBits(data, bitf);
		if (vlc < 0x0040)
		{
		    vlc = (vlc << 4) | getBits(data, bitf);
		}
	    }
	}

	uint8_t color = vlc & 0x03;
	int len = vlc >> 2;

	// if len == 0 -> end sequence - fill to end of line
	len = len ? len : bmpsize.x2 - xp + 1;
	putPixel(xp, yp, len, color);
	xp += len;

	if (xp > bmpsize.x2)
	{
	    // nextLine
	    if (!bitf)
		data++;
	    bitf = 1;
	    xp = bmpsize.x1;
	    yp += 2;
	    if (yp > bmpsize.y2)
		return;
	}
    }
}

// ==================================
// ! constructor
cDxr3SpuDecoder::cDxr3SpuDecoder()
{
    clean = true;
    scaleMode = eSpuNormal;
    spu = NULL;
    osd = NULL;
    spubmp = NULL;
    allowedShow = true;
}

// ==================================
cDxr3SpuDecoder::~cDxr3SpuDecoder()
{
    delete spubmp;
    delete spu;
    delete osd;
}

// ==================================
// ! send spu data to dxr3
void cDxr3SpuDecoder::processSPU(uint32_t pts, uint8_t * buf, bool AllowedShow)
{
    setTime(pts);
    dsyslog("dxr3: spudec push: pts=%d", pts);

    delete spubmp;
    spubmp = NULL;
    delete[]spu;
    spu = buf;
    spupts = pts;

    DCSQ_offset = cmdOffs();
    prev_DCSQ_offset = 0;

    clean = true;
    allowedShow = AllowedShow;
}

// ==================================
// ! get scalemode
cSpuDecoder::eScaleMode cDxr3SpuDecoder::getScaleMode(void)
{
    return scaleMode;
}

// ==================================
// ! set scalemode
void cDxr3SpuDecoder::setScaleMode(cSpuDecoder::eScaleMode ScaleMode)
{
    scaleMode = ScaleMode;
}

// ==================================
// ! send palette to dxr3
void cDxr3SpuDecoder::setPalette(uint32_t * pal)
{
    palette.setPalette(pal);
}

// ==================================
// ! send highlight to dxr3
void cDxr3SpuDecoder::setHighlight(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t palette)
{
    aDxr3SpuPalDescr pld;
    for (int i = 0; i < 4; i++)
    {
	pld[i].index = 0xf & (palette >> (16 + 4 * i));
	pld[i].trans = 0xf & (palette >> (4 * i));
    }

    bool ne = hlpsize.x1 != sx || hlpsize.y1 != sy ||
	hlpsize.x2 != ex || hlpsize.y2 != ey ||
	pld[0] != hlpDescr[0] || pld[1] != hlpDescr[1] ||
	pld[2] != hlpDescr[2] || pld[3] != hlpDescr[3];

    if (ne)
    {
	dsyslog("dxr3: spu highlight: %d, %d, %d, %d", sx, sy, ex, ey);

	hlpsize.x1 = sx;
	hlpsize.y1 = sy;
	hlpsize.x2 = ex;
	hlpsize.y2 = ey;
	memcpy(hlpDescr, pld, sizeof(aDxr3SpuPalDescr));
	highlight = true;
	clean = false;
    }
}

// ==================================
// ! clear highlight
void cDxr3SpuDecoder::clearHighlight()
{
    clean &= !highlight;
    highlight = false;
    hlpsize.x1 = -1;
    hlpsize.y1 = -1;
    hlpsize.x2 = -1;
    hlpsize.y2 = -1;
}

// ==================================
int cDxr3SpuDecoder::ScaleYcoord(int value)
{
    if (scaleMode == eSpuLetterBox)
    {
	int offset =
	    cDevice::PrimaryDevice()->GetVideoSystem() == vsPAL ? 72 : 60;
	return lround((value * 3.0) / 4.0) + offset;
    }
    return value;
}

// ==================================
int cDxr3SpuDecoder::ScaleYres(int value)
{
    if (scaleMode == eSpuLetterBox)
    {
	return lround((value * 3.0) / 4.0);
    }
    return value;
}

// ==================================
void cDxr3SpuDecoder::DrawBmp(sDxr3SpuRect & size, cBitmap * bmp)
{
    int x2 = size.x2;
    while ((x2 - size.x1 + 1) & 0x03)
	x2++;
    tArea Area = { size.x1, size.y1, x2, size.y2, 2 };
    osd->SetAreas(&Area, 1);
    if (x2 > size.x2)
	osd->DrawRectangle(size.x2 + 1, size.y1, x2, size.y2, clrTransparent);
    osd->DrawBitmap(size.x1, size.y1, *bmp);
    delete bmp;
}

// ==================================
// ! draw nav, subtitles, ...
void cDxr3SpuDecoder::Draw()
{
    Hide();

    if (!spubmp)
    {
	return;
    }

    cBitmap *fg = NULL;
    cBitmap *bg = NULL;
    sDxr3SpuRect bgsize;
    sDxr3SpuRect hlsize;

    hlsize.x1 = hlpsize.x1;
    hlsize.y1 = ScaleYcoord(hlpsize.y1);
    hlsize.x2 = hlpsize.x2;
    hlsize.y2 = ScaleYcoord(hlpsize.y2);

    if (highlight)
    {
	fg = spubmp->getBitmap(hlpDescr, palette, hlsize);
    }

    if (spubmp->getMinSize(palDescr, bgsize))
    {
	bg = spubmp->getBitmap(palDescr, palette, bgsize);
	if (scaleMode == eSpuLetterBox)
	{
	    // the coordinates have to be modified for letterbox
	    int y1 = ScaleYres(bgsize.y1) + bgsize.height();
	    bgsize.y2 = y1 + bgsize.height();
	    bgsize.y1 = y1;
	}
    }

    if (bg || fg)
    {
	if (osd == NULL)
	    if ((osd = cOsdProvider::NewOsd(0, 0)) == NULL)
	    {
		esyslog("dxr3: could not instantiate new OSD");
		return;
	    }

	if (fg)
	{
	    DrawBmp(hlsize, fg);
	}

	if (bg)
	{
	    DrawBmp(bgsize, bg);
	}

	osd->Flush();
    }

    clean = true;
}

// ==================================
// ! hide nav, subtitles, ...
void cDxr3SpuDecoder::Hide()
{
    delete osd;
    osd = NULL;
}

// ==================================
// ! clear highlight and osd
void cDxr3SpuDecoder::Empty()
{
    Hide();

    delete spubmp;
    spubmp = NULL;

    delete[]spu;
    spu = NULL;

    clearHighlight();
    clean = true;
}

// ==================================
// ! set pts
int cDxr3SpuDecoder::setTime(uint32_t pts)
{
    if (!spu)
    {
	return 0;
    }

    if (spu && !clean)
    {
	Draw();
    }

    while (DCSQ_offset != prev_DCSQ_offset)
    {
	// Display Control Sequences
	int i = DCSQ_offset;
	state = spNONE;

	uint32_t exec_time = spupts + spuU32(i) * 1024;
	if ((pts != 0) && (exec_time > pts))
	{
	    return 0;
	}

	if (pts != 0)
	{
	    uint16_t feven = 0;
	    uint16_t fodd = 0;

	    i += 2;

	    prev_DCSQ_offset = DCSQ_offset;
	    DCSQ_offset = spuU32(i);
	    i += 2;

	    while (spu[i] != CMD_SPU_EOF)
	    {
		// Command Sequence
		switch (spu[i])
		{
		case CMD_SPU_SHOW:
		    // show subpicture
		    dsyslog("dxr3: spu show");
		    state = spSHOW;
		    i++;
		    break;

		case CMD_SPU_HIDE:
		    // hide subpicture
		    dsyslog("dxr3: spu hide");
		    state = spHIDE;
		    i++;
		    break;

		case CMD_SPU_SET_PALETTE:
		    // CLUT
		    palDescr[0].index = spu[i + 2] & 0xf;
		    palDescr[1].index = spu[i + 2] >> 4;
		    palDescr[2].index = spu[i + 1] & 0xf;
		    palDescr[3].index = spu[i + 1] >> 4;
		    i += 3;
		    break;

		case CMD_SPU_SET_ALPHA:
		    // transparency palette
		    palDescr[0].trans = spu[i + 2] & 0xf;
		    palDescr[1].trans = spu[i + 2] >> 4;
		    palDescr[2].trans = spu[i + 1] & 0xf;
		    palDescr[3].trans = spu[i + 1] >> 4;
		    i += 3;
		    break;

		case CMD_SPU_SET_SIZE:
		    // image coordinates
		    size.x1 = (spu[i + 1] << 4) | (spu[i + 2] >> 4);
		    size.x2 = ((spu[i + 2] & 0x0f) << 8) | spu[i + 3];

		    size.y1 = (spu[i + 4] << 4) | (spu[i + 5] >> 4);
		    size.y2 = ((spu[i + 5] & 0x0f) << 8) | spu[i + 6];

		    dsyslog("dxr3: spu size (%d,%d) x (%d,%d)",
			    size.x1, size.y1, size.x2, size.y2);
		    i += 7;
		    break;

		case CMD_SPU_SET_PXD_OFFSET:
		    // image 1 / image 2 offsets
		    fodd = spuU32(i + 1);
		    feven = spuU32(i + 3);

		    dsyslog("dxr3: spu offset: odd=%d, even=%d", fodd, feven);
		    i += 5;
		    break;

		case CMD_SPU_CHG_COLCON:
		{
		    int size = spuU32(i + 1);
		    i += 1 + size;
		}
		break;

		case CMD_SPU_MENU:
		    dsyslog("dxr3: spu menu");
		    state = spMENU;
		    i++;
		    break;

		default:
		    esyslog("dxr3: invalid sequence in control header (%.2x)",
			    spu[i]);
		    assert(0);
		    i++;
		    break;
		}
	    }
	    if (fodd != 0 && feven != 0)
	    {
		delete spubmp;
		spubmp = new cDxr3SpuBitmap(size, spu + fodd, spu + feven,
					    spu + feven, spu + cmdOffs());
	    }
	}
	else if (!clean)
	{
	    state = spSHOW;
	}

	if ((state == spSHOW && allowedShow) || state == spMENU)
	{
	    Draw();
	}

	if (state == spHIDE)
	{
	    Hide();
	}

	if (pts == 0)
	{
	    return 0;
	}
    }

    return 1;
}

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
