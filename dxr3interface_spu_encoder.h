/*
 *     dxr3interface_spu_encoder.h - encodes an OSD bitmap as subpicture
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

#ifndef _DXR3_INTERFACE_SPU_ENCODER_
#define _DXR3_INTERFACE_SPU_ENCODER_

#include <stdlib.h>
#include <linux/dvb/osd.h>
#include "dxr3vdrincludes.h"
#include "dxr3colormanager.h"
#include "dxr3interface.h"
#include "dxr3singleton.h"

// ==================================
#define MAXWINDOWS	8
#define DATASIZE	53220
#define OSDWIDTH	720
#define OSDWIDTH2	480
#define OSDHEIGHT	576

// ==================================
// basic infos about one osd "window"
struct sOSD_Window
{
    size_t x0;
    size_t y0;
    size_t x1;
    size_t y1;

    unsigned char colors[16];
    unsigned char opacity[16];

	size_t NumColors;
};

// ==================================
// used to get active osd area
struct sRectal
{
	sRectal() : x0(0), x1(0), y0(0), y1(0)		{}

	size_t x0;
	size_t x1;
	size_t y0;
	size_t y1;
};

// ==================================
// our spu(data) with all needed routines
class cSpuData
{
public:
	cSpuData(): count(0), malloc_size(0) {}
	~cSpuData();

	void Clear();
	u_char* GetData() const		{ return data; }

	// write operations
	void WriteByte(uint8_t byte);
	void WriteNibble(int *higher_nibble, uint8_t nibble);
	void WriteRle(int *higher_nibble, int length, int color);

private:
	u_char *data;
	size_t count;			// the count of bytes written
	size_t malloc_size;		// size of data
};

// ==================================
struct  pixbuf
{
	int x, y;
	u_int rgb[4];
	u_char* pixels;
};

// ==================================
struct encodedata
{
	u_char data[DATASIZE];
	int count;				// the count of bytes written
	int oddstart;
	int nibblewaiting;
};

// ==================================
class cSPUEncoder : public Singleton<cSPUEncoder>
{
public:
	cSPUEncoder();
    ~cSPUEncoder() {}

	int Cmd(OSD_Command cmd, int color = 0, int x0 = 0, int y0 = 0, int x1 = 0, int y1 = 0, const void *data = 0);
	int Flush(void);
	void StopSpu(void);

private:
    cSPUEncoder(cSPUEncoder&);  // no copy constructor

	// helper functions
	void CopyBlockIntoOSD(int linewidth, int x0,int y0, int x1, int y1, u_char *data);
	void EncodePixelbufRle(int x, int y, int w, int h, u_char *inbuf, int stride, encodedata *ed);
#ifndef USE_XINE_SCALER
	void ScaleOSD(double fac, unsigned char* buf, unsigned char NumColors=4);
#endif
	void encode_put_nibble(encodedata* ed, u_char nibble);
	void encode_pixels(encodedata* ed, int color, int number);
	void encode_eol(encodedata* ed);
	void encode_do_row(encodedata* ed, pixbuf* pb, int row);
	void encode_do_control(int x,int y, encodedata* ed, pixbuf* pb);

	void CalculateActiveOsdArea();

	sOSD_Window			m_windows[8];
	cPalette		m_palManager; // name for hysterical raisins ;)
	cColorManager*		m_ColorManager;
	encodedata			m_encodeddata;
    int					m_lastwindow;

	// our osd :)
	u_char				OSD_Screen[OSDWIDTH * OSDHEIGHT];
	u_char				OSD_Screen2[OSDWIDTH * OSDHEIGHT];
	u_char				OSD_Screen3[OSDWIDTH * OSDHEIGHT];

	// 'active' osd sizes
	sRectal				m_active_osd;
};

#endif /*_DXR3_INTERFACE_SPU_ENCODER_*/
