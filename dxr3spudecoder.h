/*
 * dxr3spudecoder.h
 *
 * Copyright (C) 2004 Christian Gmeiner
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

#ifndef _DXR3SPUDECODER_H_
#define _DXR3SPUDECODER_H_

#include <vdr/osd.h>
#include <vdr/spu.h>
#include <inttypes.h>

// ==================================
typedef struct sDxr3SpuPalDescr
{
    uint8_t index;
    uint8_t trans;

    bool operator != (const sDxr3SpuPalDescr pd) const
    {
	return index != pd.index && trans != pd.trans;
    };
} aDxr3SpuPalDescr[4];

// ==================================
struct sDxr3SpuRect
{
    int x1, y1;
    int x2, y2;

    int width()
    {
	return x2 - x1 + 1;
    };

    int height()
    {
	return y2 - y1 + 1;
    };

    bool operator != (const sDxr3SpuRect r) const
    {
	return r.x1 != x1 || r.y1 != y1 || r.x2 != x2 || r.y2 != y2;
    };
};

// ==================================
class cDxr3SpuPalette
{
private:
    uint32_t palette[16];

public:
    void setPalette(const uint32_t * pal);
    uint32_t getColor(uint8_t idx, uint8_t trans) const;
};

// ==================================
class cDxr3SpuBitmap
{
private:
    sDxr3SpuRect bmpsize;
    sDxr3SpuRect minsize[4];
    uint8_t *bmp;

    void putPixel(int xp, int yp, int len, uint8_t colorid);
    void putFieldData(int field, uint8_t * data, uint8_t * endp);

public:
    cDxr3SpuBitmap(sDxr3SpuRect size, uint8_t * fodd, uint8_t * eodd,
		   uint8_t * feven, uint8_t * eeven);
    ~cDxr3SpuBitmap();

    bool getMinSize(const aDxr3SpuPalDescr paldescr,
		    sDxr3SpuRect & size) const;
    cBitmap *getBitmap(const aDxr3SpuPalDescr paldescr,
		       const cDxr3SpuPalette & pal, sDxr3SpuRect & size) const;
};

// ==================================
class cDxr3SpuDecoder : public cSpuDecoder
{
public:
    cDxr3SpuDecoder();
    ~cDxr3SpuDecoder();

    int setTime(uint32_t pts);

    cSpuDecoder::eScaleMode getScaleMode(void);
    void setScaleMode(cSpuDecoder::eScaleMode ScaleMode);
    void setPalette(uint32_t * pal);
    void setHighlight(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey,
		      uint32_t palette);
    void clearHighlight();
    void Empty();
    void processSPU(uint32_t pts, uint8_t * buf, bool AllowedShow);

    void Hide();
    void Draw();
    bool IsVisible()
    {
	return osd != NULL;
    }

private:
    cOsd * osd;

    // processing state
    uint8_t *spu;
    uint32_t spupts;
    bool clean;
    bool ready;
    bool allowedShow;

    enum spFlag
    {
	spNONE,
	spHIDE,
	spSHOW,
	spMENU
    };
    spFlag state;

    cSpuDecoder::eScaleMode scaleMode;

    // highligh area
    bool highlight;
    sDxr3SpuRect hlpsize;
    aDxr3SpuPalDescr hlpDescr;

    // palette
    cDxr3SpuPalette palette;

    // spu info's
    sDxr3SpuRect size;
    aDxr3SpuPalDescr palDescr;

    uint16_t DCSQ_offset;
    uint16_t prev_DCSQ_offset;

    cDxr3SpuBitmap *spubmp;

    int cmdOffs()
    {
	return ((spu[2] << 8) | spu[3]);
    }
    int spuSize()
    {
	return ((spu[0] << 8) | spu[1]);
    }

    int ScaleYcoord(int value);
    int ScaleYres(int value);
    void DrawBmp(sDxr3SpuRect & size, cBitmap * bmp);
};

// ==================================
inline uint32_t cDxr3SpuPalette::getColor(uint8_t idx, uint8_t trans) const
{
    uint8_t t = trans == 0x0f ? 0xff : trans << 4;
    return palette[idx] | (t << 24);
}

#endif /*_DXR3SPUDECODER_H_*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
