/*
 *     dxr3spuencoder.c - encodes an OSD bitmap as subpicture
 *
 *	Assimilated and adapted by
 *  Stefan Schluenss <dxr3_osd@schluenss.de>
 *  Nov. 2002
 *
 * Based on the subpicture encoding routines from MPlayer and
 * the information given by
 *   Samuel Hocevar
 *   Michel Lespinasse
 *   and http://members.aol.com/mpucoder/DVD/spu.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "dxr3interface_spu_encoder.h"
#include "dxr3tools.h"

/*
  ToDo:
  - cSPUEncoder::encode_do_row: FIXME: watch this space for EOL
*/


#ifdef USE_XINE_SCALER
/*=======================================================================
 *
 * Scaling functions taken from the xine plugin
 *
 */

#include <math.h>
#include <signal.h>
#include <string>
#include <vdr/plugin.h>

namespace XineScaler
{

    template <const int yInc = 1, class T = int>
    class cBresenham
    {
	const int m_dx;
	const int m_dy;

	int m_eps;
	T m_y;

    public:
	cBresenham(const int dy, const int dx, const int eps, T const y0 = 0)
	    : m_dx(dx), m_dy(dy), m_eps(eps - m_dx), m_y(y0)
	{
	}

	int eps() const
	{
	    return m_eps;
	}

	T step()
	{
	    m_eps += m_dy;
	    while (m_eps >= 0)
	    {
		m_eps -= m_dx;
		m_y += yInc;
	    }
	    return m_y;
	}

	T step(int n)
	{
	    if (n <= 0)
		return m_y;
	    while (--n > 0)
		step();
	    return step();
	}

	T stepRelative(int n = 1)
	{
	    T const y = m_y;
	    return step(n) - y;
	}
    };

    static uint8_t *ScaleBitmapLQ(const uint8_t *src, uint8_t *dest, int x0,
				  int y0, int w, int h, int ws, int hs, int x1,
				  int y1, int w1, int h1,
				  const uint8_t transparentIndex)
    {
	uint8_t *const screen = new uint8_t[ OSDHEIGHT * OSDWIDTH ];
	{
	    int x1 = x0 + w;
	    int y1 = y0 + h;
	    int x2 = OSDWIDTH;
	    int y2 = OSDHEIGHT;

	    if (x1 > x2)
		x1 = x2;

	    if (y1 > y2)
		y1 = y2;

	    uint8_t *dst = screen;

	    for (int y = 0; y < y0; y++)
	    {
		for (int x = 0; x < x2; x++)
		    *dst++ = transparentIndex;
	    }

	    for (int y = y0; y < y1; y++)
	    {
		for (int x = 0; x < x0; x++)
		    *dst++ = transparentIndex;

		for (int x = x0; x < x1; x++)
		    *dst++ = *src++;

		for (int x = x1; x < x2; x++)
		    *dst++ = transparentIndex;
	    }

	    for (int y = y1; y < y2; y++)
	    {
		for (int x = 0; x < x2; x++)
		    *dst++ = transparentIndex;
	    }
	}

	uint8_t *scaled = dest; //new uint8_t[ hs * ws ];
	{
	    int x2 = x1 + w1;
	    int y2 = y1 + h1;

	    if (x2 > ws)
	    {
		x2 = ws;
		w1 = x2 - x1;
		if (w1 < 0)
		    w1 = 0;
	    }

	    if (y2 > hs)
	    {
		y2 = hs;
		h1 = y2 - y1;
		if (h1 < 0)
		    h1 = 0;
	    }

	    cBresenham<OSDWIDTH, uint8_t *> yyBh(2 * OSDHEIGHT, 2 * hs, hs, screen);
	    uint8_t *screen0 = yyBh.step(y1); //(((2 * y1 + 1) * OSDHEIGHT / hs) / 2);

	    cBresenham<> xxBh0(2 * OSDWIDTH, 2 * ws, ws);
	    xxBh0.step(x1); //(((2 * x1 + 1) * OSDWIDTH / ws) / 2);

	    uint8_t *scaled0 = scaled + y1 * OSDWIDTH; //ws; ******

	    for (int y = y1; y < y2; y++)
	    {
		cBresenham<> xxBh(xxBh0);
		int xxx = xxBh.step(0); //(((2 * x1 + 1) * OSDWIDTH / ws) / 2);

		uint8_t *screen00 = screen0 + xxx;
		uint8_t *scaled00 = scaled0 + x1;

		for (int x = x1; x < x2; x++)
		{
		    *scaled00++ = *screen00;
		    screen00 += xxBh.stepRelative();
		}

		scaled0 += OSDWIDTH; //ws; *******
		screen0 = yyBh.step();
	    }
	}

	delete [] screen;
	return scaled;
    }

};

/*=======================================================================
 *
 * End of scaling functions taken from the xine plugin
 *
 */
#endif /* USE_XINE_SCALER */


// ==================================
// dec.
cSpuData::~cSpuData()
{
    Clear();
}

// ==================================
// free buffer and set it to 0
void cSpuData::Clear()
{
    if (data)
    {
	free(data);
	count = malloc_size = 0;
    }
}

// ==================================
// wirte a byte to spu buffer
void cSpuData::WriteByte(uint8_t byte)
{
    if (count >= malloc_size)
    {
	data = (u_char*) realloc(data, malloc_size += 2048);
    }
    data[count++] = byte;
}

// ==================================
void cSpuData::WriteNibble(int *higher_nibble, uint8_t nibble)
{
}

// ==================================
void cSpuData::WriteRle(int *higher_nibble, int length, int color)
{
}

// ==================================
cSPUEncoder::cSPUEncoder()
{
    // clear osd
    memset(OSD_Screen,  0x00, OSDWIDTH * OSDHEIGHT);
    memset(OSD_Screen2, 0x00, OSDWIDTH * OSDHEIGHT);
    memset(OSD_Screen3, 0x00, OSDWIDTH * OSDHEIGHT);

    // set active area to 0
    //m_x0 = m_x1 = m_y0 = m_y1 = 0;
}


//========================================
//Sets the palette indexes to use for one
//window taking into account the global
//palette (with colors needed by all windows)

void cSPUEncoder::SetPalette(int numWindow, cPalette* commonPalette,
			     cPalette* windowPalette)
{
    int NumColors;
    const tColor *Colors = windowPalette->Colors(NumColors);
    if (Colors)
    {
	for (int i = 0; i < NumColors; i++)
	{
	    int idx = commonPalette->Index(Colors[i] & 0x00FFFFFF);
	    int opacity = ((Colors[i] & 0xFF000000) >> 24) * 0xF / 0xFF;
	    bitmapcolor[numWindow][i] = (opacity<<4) | idx;
	}
    }
}

//========================================
//Clears the OSD bitmap

void cSPUEncoder::Clear(void)
{
    memset(OSD_Screen, 0, sizeof(OSD_Screen));
}

//=============================================================
//Sets the spu palette and flushes the OSD content into the spu
int cSPUEncoder::Flush(cPalette *Palette)
{
    int NumColors;
    const tColor *Colors = Palette->Colors(NumColors);
    if (Colors)
    {
	unsigned int palcolors[16];
	for (int i = 0; i < NumColors; i++)
	{
	    // convert AARRGGBB to AABBGGRR (the driver expected the the
	    // colors the wrong way, so does Rgb2YCrCb and friends)
	    unsigned int color = ((Colors[i] & 0x0000FF) << 16)
		| (Colors[i] & 0x00FF00) | ((Colors[i] & 0xFF0000) >> 16);
	    palcolors[i] = Tools::Rgb2YCrCb(color);
	}
	cDxr3Interface::Instance().SetPalette(palcolors);
    }

    // calculate osd size (actually dead code)
    CalculateActiveOsdArea();

    m_encodeddata.count = 0;
    EncodePixelbufRle(0, 0, OSDWIDTH, OSDHEIGHT-1, OSD_Screen, 0,
		      &m_encodeddata);

    dsyslog("dxr3: cSPUEncoder::Flush: OSD data size: %d",
	    m_encodeddata.count);

    if (m_encodeddata.count <= DATASIZE)
    {
	cDxr3Interface::Instance().WriteSpu((uint8_t*) &m_encodeddata,
					    m_encodeddata.count);
	return 0;
    }
    else
    {
	esyslog("dxr3: spu: warning: SPU data size (%d) exceeds limit (%d)",
		m_encodeddata.count, DATASIZE);
	return -1;
    }
}

// ==================================
// stamps window content into full osd bitmap
void cSPUEncoder::CopyBlockIntoOSD(int numWindow, int linewidth, int x0,
				   int y0, int x1, int y1, const tIndex *data)
{
    tIndex *cp;
    const tIndex *sp = data;

    if (x1 >= OSDWIDTH)
	x1 = OSDWIDTH - 1;
    if (y1 >= OSDHEIGHT)
	y1 = OSDHEIGHT - 1;
    cp = &OSD_Screen[y0 * OSDWIDTH + x0];

    for (int y = y0; y <= y1; y++)
    {
	for (int x = x0; x <= x1; x++)
	{
	    *(cp++) = bitmapcolor[numWindow][*(sp++) & 0x0f];
	}
	cp += OSDWIDTH - (x1 - x0 + 1);
	sp += linewidth - (x1 - x0 + 1);
    }
}

// ==================================
// taken from mplayer (spuenc.c)
void cSPUEncoder::EncodePixelbufRle(int x, int y, int w, int h, u_char *inbuf,
				    int stride, encodedata *ed)
{
    pixbuf pb;
    int i, row;
    pb.x = w;
    pb.y = h;

#ifdef USE_XINE_SCALER
    int ws = cDxr3Interface::Instance().GetHorizontalSize();
    int hs = cDxr3Interface::Instance().GetVerticalSize();
    if (ws < 720 || hs < 576)
	inbuf = XineScaler::ScaleBitmapLQ(inbuf, OSD_Screen2, 0, 0, OSDWIDTH,
					  OSDHEIGHT, ws, hs, 0, 0, ws, hs,
					  0 /* clrTransparent */);
#else
    if (cDxr3Interface::Instance().GetHorizontalSize() < 700)
    {
	double fac = (double)OSDWIDTH / (double)OSDWIDTH2;
	ScaleOSD(fac, inbuf, 10);
	inbuf = OSD_Screen2;
    }
#endif /* USE_XINE_SCALER */

    m_ColorManager = new cColorManager();

    // Encode colors into highlight regions
    m_ColorManager->EncodeColors(w, h, inbuf, OSD_Screen3);
    inbuf = OSD_Screen3;

    pb.pixels = inbuf;
    ed->count = 4;
    ed->nibblewaiting = 0;

    row = 0;
    for (i = 0; i < pb.y; i++)
    {
	encode_do_row(ed, &pb, row);
	row += 2;
	if (row > pb.y)
	{
	    row = 1;
	    ed->oddstart = ed->count;
	}
    }
    encode_do_control(x, y, ed, &pb);

    delete m_ColorManager;
}

#ifndef USE_XINE_SCALER
// ==================================
void cSPUEncoder::ScaleOSD(double fac, unsigned char* buf,
			   unsigned char NumColors)
{
    int y, x, s, d;
    unsigned char dline[2 * OSDWIDTH + 10];

    memset(OSD_Screen2, 0x00, OSDWIDTH * OSDHEIGHT);

    if (cDxr3Interface::Instance().GetHorizontalSize() < 470)
    {
	for (y = 0; y < OSDHEIGHT; y++)
	    for (s = 0, d = 0; d < OSDWIDTH; s++, d += 2)
		OSD_Screen2[y * OSDWIDTH + s] = buf[y * OSDWIDTH + d];
    }
    else
    {
	for (y = 0; y < OSDHEIGHT; y++)
	{
	    memset(dline, 0, 2 * OSDWIDTH + 10);

	    for (s = 0, d = 0; s < OSDWIDTH; s++, d += 2)
	    {
		// stretch line to double width to 1440
		dline[d] = buf[y * OSDWIDTH + s];
	    }

	    for (d = 1; d < (2 * OSDWIDTH); d += 2)
	    {
		dline[d] = dline[d + 1];
	    }

	    for (s = 0, x = 0; x < OSDWIDTH2; x++, s += 3)
	    {
		// now take every third pixel (1440/3=480)
		OSD_Screen2[y * OSDWIDTH + x] = dline[s];
	    }
	}
    }
}
#endif /* not USE_XINE_SCALER */

// ==================================
// taken from mplayer (spuenc.c)
void cSPUEncoder::encode_put_nibble(encodedata* ed, u_char nibble)
{
    if (ed->nibblewaiting)
    {
	ed->data[ed->count++] |= nibble;
	ed->nibblewaiting = 0;
    }
    else
    {
	ed->data[ed->count] = nibble << 4;
	ed->nibblewaiting = 1;
    }
}

// ==================================
// taken from mplayer (spuenc.c)
void cSPUEncoder::encode_pixels(encodedata* ed, int color, int number)
{
    if (number > 3)
    {
	if (number > 15)
	{
	    encode_put_nibble(ed, 0);
	    if (number > 63)
	    {
		encode_put_nibble(ed, (number & 0xC0) >> 6);
	    }
	}
	encode_put_nibble(ed, (number & 0x3C) >> 2);
    }
    encode_put_nibble(ed, ((number & 0xF) << 2) | color);
}

// ==================================
// taken from mplayer (spuenc.c)
void cSPUEncoder::encode_eol(encodedata* ed)
{
    if (ed->nibblewaiting)
    {
	ed->count++;
	ed->nibblewaiting = 0;
    }
    ed->data[ed->count++] = 0x00;
    ed->data[ed->count++] = 0x00;
}

// ==================================
// taken from mplayer (spuenc.c)
void cSPUEncoder::encode_do_row(encodedata* ed, pixbuf* pb, int row)
{
    int i = 0;
    u_char* pix = pb->pixels + row * pb->x;
    int color = *pix & 0x03;
    int n = 0; /* the number of pixels of this color */

    while (i++ < pb->x)
    {
	/* FIXME: watch this space for EOL */
	if ((*pix & 0x03) != color || n == 255)
	{
	    encode_pixels(ed, color, n);
	    color = *pix & 0x03;
	    n = 1;
	}
	else
	{
	    n++;
	}
	pix++;
    }

    /* this small optimization: (n>63) can save up to two bytes per line
     * I wonder if this is compatible with all the hardware... */
    if (color == 0 && n > 63)
    {
	encode_eol(ed);
    }
    else
    {
	encode_pixels(ed, color, n);
    }

    if (ed->nibblewaiting)
    {
	ed->count++;
	ed->nibblewaiting = 0;
    }
}

// ==================================
// taken from mplayer (spuenc.c)
void cSPUEncoder::encode_do_control(int x, int y, encodedata* ed, pixbuf* pb)
{
    int controlstart = ed->count;
    int x1;
    int i;
    u_int top, left, bottom, right;

    top = y; //this forces the first bit to be visible on a TV
    left = x; //you could actually pass in x/y and do some nice

    bottom = top + pb->y - 1;
    right = left + pb->x - 1;

    /* start at x0+2*/
    i = controlstart;

    x1 = (i); //marker for last command block address

    /* display duration... */
    ed->data[i++] = 0x00;
    ed->data[i++] = 0x00; //duration before turn on command occurs (will not be used)

    /* x1 */
    ed->data[i++] = x1 >> 8;   //since this is the last command block, this
    ed->data[i++] = x1 & 0xff; //points back to itself

    /* 0x00: force displaying */
    ed->data[i++] = 0x00;

    /* 0x03: palette info */
    ed->data[i++] = 0x03;
    ed->data[i++] = 0x01;
    ed->data[i++] = 0x23;

    /* 0x04: transparency info (reversed) */
    ed->data[i++] = 0x04;	// SET_CONTR
    ed->data[i++] = 0xFF;
    ed->data[i++] = 0x70;

    /* 0x05: coordinates */
    ed->data[i++] = 0x05;	// SET_DAREA
    ed->data[i++] = left >> 4;
    ed->data[i++] = ((left & 0xf) << 4) + (right >> 8);
    ed->data[i++] = (right & 0xff);
    ed->data[i++] = top >> 4;
    ed->data[i++] = ((top&0xf) << 4) + (bottom >> 8);
    ed->data[i++] = (bottom&0xff);

    /* 0x06: both fields' offsets */
    ed->data[i++] = 0x06;	// SET_DSPXA
    ed->data[i++] = 0x00;
    ed->data[i++] = 0x04;
    ed->data[i++] = ed->oddstart >> 8;
    ed->data[i++] = ed->oddstart & 0xff;

    int len;
    unsigned char *spudata;

    spudata = m_ColorManager->GetSpuData(len);
    //check that the highlight regions data wont overflow the buffer
    if (i + len + 2 > DATASIZE)
    {
	ed->count = DATASIZE + 1;
	return;
    }

    for (int si = 0; si < len; si++)
    {
	ed->data[i++] = *(spudata + si);
    }

    /* 0xFF: end sequence */
    ed->data[i++]= 0xFF;
    if (! i&1)
    {
	ed->data[i++]= 0xff;
    }

    /* x0 */
    ed->data[2] = (controlstart) >> 8;
    ed->data[3] = (controlstart) & 0xff;

    /* packet size */
    ed->data[0] = i >> 8;
    ed->data[1] = i & 0xff;

    ed->count = i;
}

// ==================================
// Stop spu display
void cSPUEncoder::StopSpu(void)
{
    uint8_t ed[10];
    /* packet size */
    ed[0] = 0;
    ed[1] = 10;

    /* pointer to the SP_DCSQT */
    ed[2] = 0;
    ed[3] = 4;

    /* SP_DCSQT */
    /* display duration... */
    ed[4] = 0x00;
    ed[5] = 0x00; //duration before turn on command occurs (will not be used)

    /* pointer to next command block */
    ed[6] = 0;  //since this is the last command block, this
    ed[7] = 4;//points back to itself

    /* 0x02: stop displaying */
    ed[8] = 0x02;

    /* 0xFF: end sequence */
    ed[9] = 0xFF;
    cDxr3Interface::Instance().WriteSpu(ed, 10);
}

// ==================================
// we _only_ write usefull data
void cSPUEncoder::CalculateActiveOsdArea()
{
    // reset
    //m_x0 = m_x1 = m_y0 = m_y1 = 0;

    // calculate
    /*
    for (int i = 1; i < 8; i++)
    {
	m_x0 = max(m_x0, m_windows[i].x0);
	m_x1 = max(m_x1, m_windows[i].y0);
	m_y0 = max(m_y0, m_windows[i].x1);
	m_y1 = max(m_y1, m_windows[i].y1);
    }
    */
}

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
