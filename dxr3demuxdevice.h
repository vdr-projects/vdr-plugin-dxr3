/*
 * dxr3demuxdevice.h
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

#ifndef __DXR3_DEMUX_DEVICE_H
#define __DXR3_DEMUX_DEVICE_H

#include "dxr3vdrincludes.h"
#include "dxr3generaldefines.h"
#include "dxr3syncbuffer.h"
#include "dxr3audiodecoder.h"
#include "dxr3outputthread.h"

const int AUDIO_MAX_BUFFER_SIZE = 200;
const int VIDEO_MAX_BUFFER_SIZE = 500;

const int AUIDO_MAX_FRAME_SIZE = 5000;
const int VIDEO_MAX_FRAME_SIZE = 3000;
const uint32_t PRE_BUFFER_LENGTH = 0;

// ==================================
// extract video and audio
class cDxr3DemuxDevice 
{
public:
	cDxr3DemuxDevice();
    cDxr3DemuxDevice(cDxr3Interface& dxr3Device);
    ~cDxr3DemuxDevice();

public:
    void Stop(void);
    void Resync(void);
    void Clear(void);
    void Init(void);
    void SetTvMode(void);
    void SetAudioOnlyMode(void);
    void SetVideoOnlyMode(void);
    void SetReplayMode(void);
    void SetTrickMode(eDxr3TrickState trickState, int Speed = 1);

    int DemuxPes(const uint8_t* buf, int length, bool bAc3Dts = false);
    int DemuxAudioPes(const uint8_t* buf, int length);
    void StillPicture(const uint8_t* buf, int length);

    eDxr3DemuxMode GetDemuxMode(void) { return m_demuxMode;};
    eDxr3TrickState GetTrickState(void) { return m_trickState;};
    bool Poll(int TimeoutMs){ return  m_aBuf.Poll(TimeoutMs) && m_vBuf.Poll(TimeoutMs); };
    // { return  m_demuxMode == DXR3_DEMUX_AUDIO_ONLY_MODE ? m_aBuf.Poll(TimeoutMs) : m_aBuf.Poll(TimeoutMs); };

protected:
    cDxr3Interface&			m_dxr3Device;
    cDxr3SyncBuffer			m_aBuf;
    cDxr3SyncBuffer			m_vBuf;
    eDxr3DemuxSynchState	m_synchState;
    eDxr3DemuxMode			m_demuxMode;
    eDxr3TrickState			m_trickState;
    cDxr3AudioDecoder		m_aDecoder;
    cDxr3AudioOutThread*	m_pAudioThread;
    cDxr3VideoOutThread*	m_pVideoThread;
    uint32_t				m_stopScr;
	int						m_ReUseFrame;	// how often a frame should be used

private:
    cDxr3DemuxDevice(cDxr3DemuxDevice&); // no copy constructor
};

#endif // __DXR3_DEMUX_DEVICE_H

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
