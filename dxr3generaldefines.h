/*
 * dxr3generaldefines.h
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

#ifndef __DXR3_GENERAL_DEFINES_H
#define __DXR3_GENERAL_DEFINES_H

enum eDxr3DemuxSynchState {
    DXR3_DEMUX_AUDIO_SYNCHED,
    DXR3_DEMUX_VIDEO_SYNCHED,
    DXR3_DEMUX_SYNCHED,
    DXR3_DEMUX_UNSYNCHED
};

enum eDxr3DemuxMode {
    DXR3_DEMUX_TV_MODE,
    DXR3_DEMUX_REPLAY_MODE,
    DXR3_DEMUX_AUDIO_ONLY_MODE,
    DXR3_DEMUX_VIDEO_ONLY_MODE,
    DXR3_DEMUX_TRICK_MODE,
    DXR3_DEMUX_OFF_MODE
};

enum eDxr3TrickState {
    DXR3_FAST,
    DXR3_SLOW,
    DXR3_FREEZE,
};




#endif // __DXR3_GENERAL_DEFINES_H
