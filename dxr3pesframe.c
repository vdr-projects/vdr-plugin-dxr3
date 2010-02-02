/*
 * dxr3pesframe.c
 *
 * Copyright (C) 2002-2004 Kai Möller
 * Copyright (C) 2009 Christian Gmeiner
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

#include "dxr3pesframe.h"

#include <linux/em8300.h>
#include <vdr/tools.h>

// ==================================
bool cDxr3PesFrame::parse(const uint8_t *pes, uint32_t length)
{
    // look if at the beginning there is the pes packet start indicator
    if (pes[0] != 0 || pes[1] != 0 || pes[2] != 1) {
        esyslog("[cDxr3PesFrame]: no start indicator found...");
        return false;
    }

    // handle stream id
    switch (pes[3]) {
    case 0xBD: // private stream 1
        m_pesDataType = PES_PRIVATE_DATA;
        break;

    case 0xC0 ... 0xDF: // audio stream
        m_pesDataType = PES_AUDIO_DATA;
        break;

    case 0xE0 ... 0xEF: // video stream
        m_pesDataType = PES_VIDEO_DATA;
        break;

    default:
        return false;
    }

    // store start of pes frame
    m_pesStart = pes;

    // read pes header len
    uint8_t pesHeaderDataLength = pes[8];

    // look if there is a pts
    if (pes[7] & (1 << 7)) {
        // read pts
        uint64_t pts  = (((int64_t)pes[ 9]) & 0x0e) << 29;
        pts |= ( (int64_t)pes[10])         << 22;
        pts |= (((int64_t)pes[11]) & 0xfe) << 14;
        pts |= ( (int64_t)pes[12])         <<  7;
        pts |= (((int64_t)pes[13]) & 0xfe) >>  1;

        m_pts = pts >> 1;
    }

    // set pointer to start of payload
    int payloadStart = 9 + pesHeaderDataLength;
    m_payload = &pes[payloadStart];
    m_payloadLength = length - payloadStart;

    if (m_pesDataType == PES_VIDEO_DATA) {

        // we can get some informations about the video payload
        // of this pes frame. For more informations have a look
        // at http://dvd.sourceforge.net/dvdinfo/mpeghdrs.html

        const uint8_t *video = &pes[payloadStart];

        // look if there is the start code prefix (0000 0000 0000 0000 0000 0001)
        if (video[0] != 0 || video[1] != 0 || video[2] != 1) {
            return true;
        }

        // check stream id
        if (video[3] == 0xb3) {

            // in the sequence header we get informations about horizontal
            // and vertical size of the video and the current aspect ratio.

            m_horizontalSize = (video[5] & 0xf0) >> 4 | video[4] << 4;
            m_verticalSize = (video[5] & 0x0f) << 8 | video[6];

            switch (video[7] & 0xf0) {
            case 0x20:
                m_aspectRatio = EM8300_ASPECTRATIO_4_3;
                break;

            case 0x30:
                m_aspectRatio = EM8300_ASPECTRATIO_16_9;
                break;
            }

            m_staticAspectRatio = m_aspectRatio;
        }
    }

    return true;
}


// ==================================
uint32_t cDxr3PesFrame::m_staticAspectRatio = EM8300_ASPECTRATIO_4_3;

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// tab-width: 4;
// indent-tabs-mode: nil
// End:
