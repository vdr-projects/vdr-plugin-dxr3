/*
 * dxr3generaldefines.h: 
 *
 * See the main source file 'dxr3.c' for copyright information and
 * how to reach the author.
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
