/*
 * dxr3demuxdevice.h
 *
 * Copyright (C) 2002-2004 Kai Möller
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

#ifndef __DXR3_DEMUX_DEVICE_H
#define __DXR3_DEMUX_DEVICE_H

#include "dxr3generaldefines.h"
#include "dxr3syncbuffer.h"
#include "dxr3output.h"

const int AUDIO_MAX_BUFFER_SIZE = 200;
const int VIDEO_MAX_BUFFER_SIZE = 500;

const int AUIDO_MAX_FRAME_SIZE = 5000;
const int VIDEO_MAX_FRAME_SIZE = 3000;
const uint32_t PRE_BUFFER_LENGTH = 0;

// ==================================
// extract video and audio
class cDxr3DemuxDevice : private Uncopyable {
public:
    cDxr3DemuxDevice();
    ~cDxr3DemuxDevice();

    void setAudio(iAudio *audio);

    void Stop();
    void Resync();
    void Clear();
    void SetReplayMode();
    void SetTrickMode(eDxr3TrickState trickState, int Speed = 1);

    int DemuxPes(const uint8_t* buf, int length, bool bAc3Dts);
    int DemuxAudioPes(const uint8_t* buf, int length);
    void StillPicture();

    eDxr3DemuxMode GetDemuxMode()   { return demuxMode; }
    eDxr3TrickState GetTrickState() { return trickState; };

    bool Poll(int TimeoutMs)  {
        return  aBuf.Poll(TimeoutMs) && vBuf.Poll(TimeoutMs);
    };

private:
    cDxr3SyncBuffer aBuf;
    cDxr3SyncBuffer vBuf;
    eDxr3DemuxSynchState synchState;
    eDxr3DemuxMode demuxMode;
    eDxr3TrickState trickState;
    cDxr3AudioDecoder aDecoder;
    cDxr3AudioOutThread* audioThread;
    cDxr3VideoOutThread* videoThread;
    uint32_t stopScr;
};

#endif // __DXR3_DEMUX_DEVICE_H

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// tab-width: 4;
// indent-tabs-mode: nil
// End:
