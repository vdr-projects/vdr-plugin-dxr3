/*
 * dxr3audio.c
 *
 * Copyright (C) 2009 Christian Gmeiner
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

#include <string.h>     // for memeset
#include <math.h>       // for pow
#include <vdr/device.h> // for MAXVOLUME

#include "dxr3audio.h"

const static int AUDIO_STEREO = 0;
const static int AUDIO_MONO_LEFT = 1;
const static int AUDIO_MONO_RIGHT = 2;

void iAudio::changeVolume(int16_t* pcmbuf, size_t size)
{
    if (vol == 0) {
	    memset(pcmbuf, 0, size);
    } else if (vol < MAXVOLUME) {

	    int factor = (int)(pow(10.0, -2.5 + 2.5*vol/256.0)*0x7FFF);
        size = size / 2;

        while (size > 0) {
            register int32_t tmp = (int32_t)(*pcmbuf) * factor;
            *pcmbuf = (int16_t) (tmp>>15);
            pcmbuf++;
            size--;
        }
    }

    // respect audio channel setting
    if (audioChannel == AUDIO_STEREO)
        return;

    if (audioChannel == AUDIO_MONO_RIGHT) {

        for (unsigned int i = 0; i < size / sizeof(short); i++) {
            if (!(i & 0x1))
                pcmbuf[i] = pcmbuf[i+1];
        }
    } else if (audioChannel == AUDIO_MONO_LEFT) {

        for (unsigned int i = 0; i < size / sizeof(short); i++) {
            if ((i & 0x1))
                pcmbuf[i] = pcmbuf[i-1];
        }
    }
}
