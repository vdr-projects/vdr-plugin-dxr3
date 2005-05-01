/*
 * dxr3pesframe.c
 *
 * Copyright (C) 2002-2004 Kai Möller
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include "dxr3pesframe.h"
#include "dxr3log.h"

#include <linux/em8300.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// ==================================
bool cDxr3PesFrame::ExtractNextFrame(const uint8_t* pBuf, uint32_t length)
    throw (ePesFrameError)
{
    cDxr3SafeArray<uint8_t> pesArray((uint8_t*)pBuf, length);
    uint32_t pos = 0;
    m_pNextStart = pBuf;
    m_remainingLength = length;

    InitData();

    try
    {
	if (length > 9)
	{
	    for (; pos + 9 < length && !IsPesHeader(pesArray.SubArray(pos, 4)); pos++);
	    if (pos + 9 >= length)
	    {
		// Corrupt stream?
		m_remainingLength = 0;
		return m_bValid;
	    }
	    m_pPesStart = pBuf + pos;

	    if ((pesArray[pos + 6] & 0xC0) == 0x80
		/*|| (pesArray[pos + 6] & 0xC0) == 0x00*/)
	    {
		if (pos + 9 + pesArray[pos + 8] < length)
		{
		    m_pEsStart = pBuf + pos + 9 + pesArray[pos + 8];
		    if ((((int)pesArray[pos + 4]) * (int)256 + (int)pesArray[pos + 5]) > 0)
		    {
			m_esLength = ((int)pesArray[pos + 4]) *
			    (int)256 + (int)pesArray[pos + 5] + (int)6 -
			    (9 + (int)pesArray[pos + 8]);
			if (pos + 9 + pesArray[pos + 8] + m_esLength <= length)
			{
			    m_bValid = true;
			    m_pNextStart = m_pEsStart + m_esLength;
			    m_remainingLength = pBuf + length - (m_pEsStart + m_esLength);
			    if (pesArray[pos + 6] >> 6 == 2 &&
				pesArray[pos + 7] >> 7 != 0)
			    {
				ExtractPts(pesArray.SubArray(pos + 9, 5));
			    }
			    if (m_pesDataType == PES_VIDEO_DATA)
			    {
				int retval = ExtractVideoData(pesArray.SubArray(pos + 9 + pesArray[pos + 8], m_esLength));
				if (m_videoFrameType != UNKNOWN_FRAME && retval)
				    m_offset = retval + pos + 9 + pesArray[pos + 8];
			    }
			}
		    }
		}
	    }
	    else
	    {
		uint32_t fpos = pos + 6;
		m_esLength = ((int)pesArray[pos + 4]) * (int)256 + (int)pesArray[pos + 5];
		if (length >= pos + 6 + m_esLength)
		{
		    while (pesArray[fpos] == 0xff)
			++fpos; // skip stuffing bytes
		    if ((pesArray[fpos] & 0xC0) == 0x40)
			fpos += 2; // skip std buffer scale and size
		    if ((pesArray[fpos] & 0xF0) == 0x20)
		    {
			// pts only
			ExtractPts(pesArray.SubArray(fpos, 5));
			fpos += 5;
		    }
		    else if ((pesArray[fpos] & 0xF0) == 0x30)
		    {
			// pts and dts
			ExtractPts(pesArray.SubArray(fpos, 5));
			fpos += 10;
		    }
		    else
		    {
			++fpos;
		    }

		    if (m_esLength) m_esLength = m_esLength - (fpos - pos - 6);
		    m_pEsStart = pBuf + fpos;
		    m_pNextStart = m_pEsStart + m_esLength;
		    m_remainingLength = pBuf + length - (m_pEsStart + m_esLength);
		    m_bValid = true;
		    if (m_pesDataType == PES_VIDEO_DATA)
		    {
			int retval = ExtractVideoData(pesArray.SubArray(fpos, m_esLength));
			if (m_videoFrameType != UNKNOWN_FRAME && retval)
			    m_offset = 0;
		    }
		}
	    }
	}
    }
    catch (cDxr3SafeArray<uint8_t>::eSafeArrayException ex)
    {
	m_bValid = false;
	cLog::Instance() << "*** PES_GENERAL_ERROR ****\n";
	throw(PES_GENERAL_ERROR);
    }

    return m_bValid;
}

// ==================================
int cDxr3PesFrame::ExtractVideoData(cDxr3SafeArray<uint8_t> esFrame)
    throw (cDxr3SafeArray<uint8_t>::eSafeArrayException)
{
    int retval = 0;
    for (uint32_t i = 0; esFrame.GetLength() > (uint32_t) 8 && i < esFrame.GetLength() - 8; i++)
    {
	if (esFrame[i] == 0 && esFrame[i+1] == 0 && esFrame[i+2] == 1)
	{
	    // start code
	    if ((esFrame[i + 3] & 0xFF) == 0x00)
	    {
		// extract frame type
		if (m_offset == 0) retval = i;
		switch ((esFrame[ i + 5] >> 3) & 0x7)
		{
		case 0x1:
		    m_videoFrameType = I_FRAME;
		    break;

		case 0x2:
		    m_videoFrameType = P_FRAME;
		    break;

		case 0x3:
		    m_videoFrameType = B_FRAME;
		    break;

		default:
		    m_videoFrameType = UNKNOWN_FRAME;
		    break;
		}
	    }
	    else if ((esFrame[i + 3] & 0xFF) == 0xB3)
	    {
		// aspect ratio
		switch ((esFrame[i + 7]) & 0xF0)
		{
		case 0x20:
		    m_staticAspectRatio = m_aspectRatio = ASPECTRATIO_4_3;
		    break;

		case 0x30:
		    m_staticAspectRatio = m_aspectRatio = ASPECTRATIO_16_9;
		    break;

		default:
		    break;
		}
		m_staticHorizontalSize = m_horizontalSize = (esFrame[i + 5] & 0xF0) >> 4 | esFrame[i + 4] << 4;
		m_staticVerticalSize = m_verticalSize = ((esFrame[i + 5] & 0x0F) << 8) | (esFrame[i + 6]);
	    }
	}
    }
    return retval;
}

// ==================================
void cDxr3PesFrame::ExtractPts(cDxr3SafeArray<uint8_t> ptsData)
    throw (cDxr3SafeArray<uint8_t>::eSafeArrayException)
{
    m_pts  = ((ptsData[0] >> 1) & 0x07) << 29;
    m_pts |= ptsData[1] << 21;
    m_pts |= (ptsData[2] >> 1) << 14;
    m_pts |= ptsData[3] << 6;
    m_pts |= ptsData[4] >> 2;
}

// ==================================
bool cDxr3PesFrame::IsPesHeader(cDxr3SafeArray<uint8_t> header)
    throw (cDxr3SafeArray<uint8_t>::eSafeArrayException)
{
    bool ret = false;

    if (!header[0] && !header[1] && header[2] == 0x01 )
    {
	ret = true;
	switch (header[3])
	{
	case 0xC0 ... 0xDF: // audio stream
	    m_pesDataType = PES_AUDIO_DATA;
	    break;

	case 0xE0 ... 0xEF: // video stream
	    m_pesDataType = PES_VIDEO_DATA;
	    break;

	case 0xBD: // private stream 1
	    m_pesDataType = PES_PRIVATE_DATA;
	    break;

	case 0xBA:
	    ret = false;
	    break;

	case 0xBE: // padding stream
	    ret = false;
	    break;

	case 0xBC: // program stream map
	case 0xBF: // private stream 2
	case 0xF0: // ECM stream
	case 0xF1: // EMM stream
	case 0xF2: // DSMCC stream
	case 0xF3: // 13522 stream
	case 0xF4: // H.22.1 type A
	case 0xF5: // H.22.1 type B
	case 0xF6: // H.22.1 type C
	case 0xF7: // H.22.1 type D
	case 0xF8: // H.22.1 type E
	case 0xF9: // ancillary stream
	case 0xFA ... 0xFE: // reserved data stream
	case 0xFF: // program stream directory
	    break;
	default:
	    ret = false;
	    break;
	}
	m_streamId = header[3];
    }

    return ret;
}

// ==================================
uint32_t cDxr3PesFrame::m_staticAspectRatio = EM8300_ASPECTRATIO_4_3;
uint32_t cDxr3PesFrame::m_staticHorizontalSize = 720;
uint32_t cDxr3PesFrame::m_staticVerticalSize = 576;
const uint32_t cDxr3PesFrame::MAX_PES_HEADER_SIZE = 184;

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
