/*
 *     dxr3interface_spu_encoder.c - encodes an OSD bitmap as subpicture
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

#include "dxr3interface_spu_encoder.h"

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
      : m_dx(dx)
      , m_dy(dy)
      , m_eps(eps - m_dx)
      , m_y(y0)
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

  static uint8_t *ScaleBitmapLQ(const uint8_t *src, uint8_t *dest, int x0, int y0, int w, int h, int ws, int hs, int x1, int y1, int w1, int h1, const uint8_t transparentIndex)
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
		data = (u_char*)realloc(data, malloc_size += 2048);
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
    memset(OSD_Screen, 0x00 ,OSDWIDTH * OSDHEIGHT);
	memset(OSD_Screen2, 0x00 ,OSDWIDTH * OSDHEIGHT);
	memset(OSD_Screen3, 0x00 ,OSDWIDTH * OSDHEIGHT);

	// set active area to 0
	//m_x0 = m_x1 = m_y0 = m_y1 = 0;
}

// ==================================
// main function for the osd
// makes life nicer :)
int cSPUEncoder::Cmd(OSD_Command cmd, int color, int x0, int y0, int x1, int y1, const void *data)
{
    u_char *cp;
	unsigned char idx = 0;
	int opacity = 0;
#if VDRVERSNUM >= 10307
	const tColor *col;
#else
	eDvbColor *col;
#endif

	switch (cmd)
	{
	case OSD_SetWindow:
		//  (x0) set window with number 0<x0<8 as current
		if (x0 < 8 && x0 > 0)
		{
			m_lastwindow = x0;
			return 0;
		}

		return -1;
		break;

	case OSD_Open:
		// (x0,y0,x1,y1,BitPerPixel[2/4/8](color&0x0F),mix[0..15](color&0xF0))
		// Opens OSD with this size and bit depth
		// returns 0 on success, -1 on DRAM allocation error, -2 on "already open"
		m_windows[m_lastwindow].x0 = x0;
		m_windows[m_lastwindow].y0 = y0;
		m_windows[m_lastwindow].x1 = x1;
		m_windows[m_lastwindow].y1 = y1;

		return 0;
		break;

	case OSD_SetPalette:
		// Spu->Cmd(OSD_SetPalette, 0, NumColors - 1, 0, 0, 0, Colors);
		// (firstcolor{color},lastcolor{x0},data)
		// Set a number of entries in the palette
		// sets the entries "firstcolor" through "lastcolor" from the array "data"
		// data has 4 byte for each color:
		// R,G,B, and a opacity value: 0->transparent, 1..254->mix, 255->pixel

	#if VDRVERSNUM >= 10307
		col = (tColor*)data;
	#else
		eDvbColor *col;
		col = (eDvbColor*)data;
	#endif

		m_windows[m_lastwindow].NumColors = x0;

		for (int x = color, i = 0; x <= x0; x++,i++) 
		{
			m_palManager.AddColor((int)*col & 0xFFFFFF);

			idx = m_palManager.GetIndex((int)*col & 0xFFFFFF);
			if (m_palManager.HasChanged()) 
			{
				cDxr3Interface::Instance().SetPalette(m_palManager.GetPalette());
			}

			opacity = ((*col & 0xFF000000) >> 24) * 0xF / 0xFF;
			m_windows[m_lastwindow].colors[i] = (opacity << 4) | idx;
			m_windows[m_lastwindow].opacity[i] = opacity;
			col++;
		}

		return 0;
		break;

	case OSD_SetBlock:
		// (x0,y0,x1,y1,increment{color},data)
		// fills pixels x0,y0 through  x1,y1 with the content of data[]
		// inc contains the width of one line in the data block,
		// inc<=0 uses blockwidth as linewidth
		// returns 0 on success, -1 on clipping all pixel

		CopyBlockIntoOSD
			(
				color,
				m_windows[m_lastwindow].x0 + x0,
				m_windows[m_lastwindow].y0 + y0,
				m_windows[m_lastwindow].x0 + x1,
				m_windows[m_lastwindow].y0 + y1,
				(u_char *)data
			);

		// calculate osd size
		CalculateActiveOsdArea();

		//cLog::Instance() << "(" << m_x0 << ", " <<  m_x1 << ") - (" << m_y0 << ", " << m_y1 << ")";

		m_encodeddata.count = 0;
		EncodePixelbufRle(0,0, OSDWIDTH, OSDHEIGHT-1, OSD_Screen, 0, &m_encodeddata);

		if (cDxr3ConfigData::Instance().GetDebug())
		{
			cLog::Instance() << "OSD Datasize: " << m_encodeddata.count << "\n";
		}

		if (m_encodeddata.count <= DATASIZE) 
		{
			cDxr3Interface::Instance().WriteSpu((uint8_t*) &m_encodeddata, m_encodeddata.count);
			return 0;
		} 
		else 
		{
			cLog::Instance() << "Waring: SPU data (" << m_encodeddata.count << ") size exceeds limit\n";
			return -1;
	    }
		break;

	case OSD_Close:
		// clear colors from plattemanager

	#if VDRVERSNUM >= 10307
		if ((col = (tColor*)m_windows[m_lastwindow].colors) != NULL) 
	#else
		if ((col = (eDvbColor*)m_windows[m_lastwindow].colors) != NULL) 
	#endif
			{
			for (size_t i = 0; i < m_windows[m_lastwindow].NumColors; ++i) 
			{
				m_palManager.RemoveColor((int)(col[i]) & 0xFFFFFF);
			}
		}

		// clear osd
		for (size_t i = m_windows[m_lastwindow].y0; i <= m_windows[m_lastwindow].y1; ++i) 
		{
			cp = &OSD_Screen[i*OSDWIDTH + m_windows[m_lastwindow].x0];
			if ((cp+m_windows[m_lastwindow].x1-m_windows[m_lastwindow].x0+1) < &OSD_Screen[OSDWIDTH * OSDHEIGHT-1])
			{
				for (size_t xx=0; xx <= (m_windows[m_lastwindow].x1-m_windows[m_lastwindow].x0); xx++) 
				{
					*(cp+xx) = 0x00;
				}
			} 
			else 
			{
				continue;
			}
		}

		// encode rle
		m_encodeddata.count = 0;
		EncodePixelbufRle(0,0, OSDWIDTH, OSDHEIGHT-1, OSD_Screen, 0, &m_encodeddata);

		// set windows position to 0
		m_windows[m_lastwindow].x0 = 0;
		m_windows[m_lastwindow].y0 = 0;
		m_windows[m_lastwindow].x1 = 0;
		m_windows[m_lastwindow].y1 = 0;

		if (m_encodeddata.count <= DATASIZE) 
		{
			cDxr3Interface::Instance().WriteSpu((uint8_t*) &m_encodeddata, m_encodeddata.count);
			return 0;
		} 
		else 
		{
			cLog::Instance() << "Waring: SPU data (" << m_encodeddata.count << ") size exceeds limit\n";
			return -1;
	    }

		return 0;
		break;

	case OSD_Clear:
		// Sets all pixel to color 0
		// returns 0 on success

		// This should be done in cSPUEncoder::cSPUEncoder

		return 0;
		break;

	// not needed - at the moment
	case OSD_Show:
	case OSD_Hide:
	case OSD_Fill:
	case OSD_SetColor:
	case OSD_SetTrans:
	case OSD_SetPixel:
	case OSD_GetPixel:
	case OSD_SetRow:
	case OSD_FillRow:
	case OSD_FillBlock:
	case OSD_Line:
	case OSD_Query:
	case OSD_Test:
	case OSD_Text:
	case OSD_MoveWindow:
		break;
	};

	return -1;
}

// ==================================
// stamps window content into full osd bitmap
void cSPUEncoder::CopyBlockIntoOSD(int linewidth, int x0, int y0, int x1, int y1, u_char *data)
{
	int i;
	int w;
	u_char *cp;
	u_char *sp = data;


	// linewidth contains the width of one line in the data block,
	// linewidth<=0 uses blockwidth as linewidth
	if (linewidth <= 0)
	{
		w = m_windows[m_lastwindow].x1 - m_windows[m_lastwindow].x0;
	}
	else
	{
		w = linewidth;
	}
	
    for (i = y0; i <= y1; ++i) 
	{
        cp = &OSD_Screen[i*OSDWIDTH + x0];
        if ((cp+x1-x0+1) < &OSD_Screen[OSDWIDTH * OSDHEIGHT-1]) 
		{
            for (int xx=0; xx <= (x1-x0); xx++) 
			{
                *(cp+xx) = m_windows[m_lastwindow].colors[*(sp+xx) & 0x0f];
            }
        } 
		else 
		{
            continue;
        }
        sp += w;
	}
}

// ==================================
// taken from mplayer (spuenc.c)
void cSPUEncoder::EncodePixelbufRle(int x, int y, int w, int h, u_char *inbuf, int stride, encodedata *ed)
{
    pixbuf pb;
    int i, row;
    pb.x = w;
    pb.y = h;

#ifdef USE_XINE_SCALER
    int ws = cDxr3Interface::Instance().GetHorizontalSize();
    int hs = cDxr3Interface::Instance().GetVerticalSize();
    if (ws < 720 || hs < 576 )
        inbuf = XineScaler::ScaleBitmapLQ(inbuf, OSD_Screen2, 0, 0, OSDWIDTH, OSDHEIGHT, ws, hs, 0, 0, ws, hs, 0 /* clrTransparent */);
#else
    if (cDxr3Interface::Instance().GetHorizontalSize() < 700) 
	{
        double fac = (double)OSDWIDTH / (double)OSDWIDTH2;
        ScaleOSD(fac, inbuf,10);
        inbuf = OSD_Screen2;
    }
#endif /* USE_XINE_SCALER */

    m_ColorManager = new cColorManager();

	// Encode colors into highlight regions
    m_ColorManager->EncodeColors(w, h, inbuf, OSD_Screen3);
    inbuf = OSD_Screen3;

    pb.pixels = inbuf;
    ed->count= 4;
    ed->nibblewaiting= 0;

    row= 0;
    for (i= 0; i < pb.y; i++) 
	{
        encode_do_row(ed, &pb, row);
        row+= 2;
        if (row > pb.y) 
		{
            row= 1;
            ed->oddstart= ed->count;
        }
    }
    encode_do_control(x,y, ed, &pb);

    delete m_ColorManager;
}

#ifndef USE_XINE_SCALER
// ==================================
void cSPUEncoder::ScaleOSD(double fac, unsigned char* buf, unsigned char NumColors)
{
    int y,x,s,d;
    unsigned char dline[2 * OSDWIDTH + 10];

    memset(OSD_Screen2, 0x00 ,OSDWIDTH * OSDHEIGHT);

    if (cDxr3Interface::Instance().GetHorizontalSize() < 470)
    {
	for (y = 0; y < OSDHEIGHT; y++)
	    for (s = 0, d = 0; d < OSDWIDTH; s++, d += 2)
		OSD_Screen2[y * OSDWIDTH + s] = buf[y * OSDWIDTH + d];
    } else {
    for (y = 0; y < OSDHEIGHT; y++) 
	{
        memset(dline,0,2*OSDWIDTH+10);
		
        for (s=0,d=0; s < OSDWIDTH; s++,d+=2) 
		{ 
			// stretch line to double width to 1440
            dline[d]  = buf[y*OSDWIDTH + s];
        }

        for (d=1; d < (2*OSDWIDTH); d+=2) 
		{ 
			#if VDRVERSNUM <= 10307
			// 'interpolate' values
            if ((dline[d-1] == BLACK) || (dline[d+1] == BLACK)) 
			{
                dline[d] = BLACK;
            }
			else if ((dline[d-1] == WHITE) || (dline[d+1] == WHITE)) 
			{
                dline[d] = WHITE;
            }
			else if ((dline[d-1] == CYAN) || (dline[d+1] == CYAN)) 
			{
                dline[d] = CYAN;
            }
			else 
			{
                dline[d] = dline[d+1];
            }
			#else /*VDRVERSNUM*/
				dline[d] = dline[d+1];
			#endif /*VDRVERSNUM*/
        }

        for (s=0, x = 0; x < OSDWIDTH2; x++,s+=3) 
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
        ed->data[ed->count++]|= nibble;
        ed->nibblewaiting= 0;
    } 
	else 
	{
        ed->data[ed->count]= nibble<<4;
        ed->nibblewaiting= 1;
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
                encode_put_nibble(ed, (number & 0xC0)>>6);
            }
        }
        encode_put_nibble(ed, (number & 0x3C)>>2);
    }
    encode_put_nibble(ed, ((number & 0xF)<<2) | color);
}

// ==================================
// taken from mplayer (spuenc.c)
void cSPUEncoder::encode_eol(encodedata* ed) 
{
    if (ed->nibblewaiting) 
	{
        ed->count++;
        ed->nibblewaiting= 0;
    }
    ed->data[ed->count++]= 0x00;
    ed->data[ed->count++]= 0x00;
}

// ==================================
// taken from mplayer (spuenc.c)
void cSPUEncoder::encode_do_row(encodedata* ed, pixbuf* pb, int row) 
{
    int i= 0;
    u_char* pix= pb->pixels + row * pb->x;
    int color= *pix & 0x03;
    int n= 0; /* the number of pixels of this color */

    while (i++ < pb->x)
	{
        /* FIXME: watch this space for EOL */
        if ((*pix & 0x03) != color || n == 255 ) 
		{
            encode_pixels( ed, color, n );
            color= *pix & 0x03;
            n= 1;
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
        encode_eol( ed );
    } 
	else 
	{
        encode_pixels( ed, color, n );
    }

    if (ed->nibblewaiting) 
	{
        ed->count++;
        ed->nibblewaiting= 0;
    }
}

// ==================================
// taken from mplayer (spuenc.c)
void cSPUEncoder::encode_do_control(int x,int y, encodedata* ed, pixbuf* pb) 
{
    int controlstart= ed->count;
    int x1;
    int i;
    u_int top, left, bottom, right;

    top= y; //this forces the first bit to be visible on a TV
    left= x; //you could actually pass in x/y and do some nice

    bottom= top + pb->y - 1;
    right= left + pb->x - 1;

    /* start at x0+2*/
    i= controlstart;

    x1= (i); //marker for last command block address

    /* display duration... */
    ed->data[i++]= 0x00;
    ed->data[i++]= 0x00; //duration before turn on command occurs (will not be used)

    /* x1 */
    ed->data[i++]= x1 >> 8;  //since this is the last command block, this
    ed->data[i++]= x1 & 0xff;//points back to itself

    /* 0x00: force displaying */
    ed->data[i++]= 0x00;

    /* 0x03: palette info */
    ed->data[i++]= 0x03;
    ed->data[i++]= 0x01;
    ed->data[i++]= 0x23;

    /* 0x04: transparency info (reversed) */
    ed->data[i++]= 0x04;	// SET_CONTR
    ed->data[i++]= 0xFF;
    ed->data[i++]= 0x70;

    /* 0x05: coordinates */
    ed->data[i++]= 0x05;	// SET_DAREA
    ed->data[i++]= left >> 4;
    ed->data[i++]= ((left&0xf)<<4)+(right>>8);
    ed->data[i++]= (right&0xff);
    ed->data[i++]= top >> 4;
    ed->data[i++]= ((top&0xf)<<4)+(bottom>>8);
    ed->data[i++]= (bottom&0xff);

    /* 0x06: both fields' offsets */
    ed->data[i++]= 0x06;	// SET_DSPXA
    ed->data[i++]= 0x00;
    ed->data[i++]= 0x04;
    ed->data[i++]= ed->oddstart >> 8;
    ed->data[i++]= ed->oddstart & 0xff;

    int len;
    unsigned char *spudata;

    spudata = m_ColorManager->GetSpuData(len);

    for (int si= 0; si < len; si++) 
	{
        ed->data[i++] = *(spudata + si);
    }

    /* 0xFF: end sequence */
    ed->data[i++]= 0xFF;
    if (! i&1 ) 
	{
        ed->data[i++]= 0xff;
    }

    /* x0 */
    ed->data[2]= (controlstart) >> 8;
    ed->data[3]= (controlstart) & 0xff;

    /* packet size */
    ed->data[0]= i >> 8;
    ed->data[1]= i & 0xff;

    ed->count= i;
}

// ==================================
// we _only_ write usefull data
void cSPUEncoder::CalculateActiveOsdArea()
{
	// reset
	//m_x0 = m_x1 = m_y0 = m_y1 = 0;

	// calculate
/*	for (int i = 1; i < 8; i++)
	{
		m_x0 = max(m_x0, m_windows[i].x0);
		m_x1 = max(m_x1, m_windows[i].y0);
		m_y0 = max(m_y0, m_windows[i].x1);
		m_y1 = max(m_y1, m_windows[i].y1);
	}
*/
}
