/*
 * dxr3device.h
 *
 * Copyright (C) 2002-2004 Kai Möller
 * Copyright (C) 2004 Christian Gmeiner
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

#ifndef _DXR3_DEVICE_H_
#define _DXR3_DEVICE_H_

#include "dxr3interface.h"
#include "dxr3demuxdevice.h"
#include "dxr3spudecoder.h"
#include <string>
#include <vdr/device.h>

class iAudio;

// ==================================
// our device :)
/*!
  cDxr3Device is the interface for VDR devices.
  Is is the part, which VDR "talks" with our plugin.
*/
class cDxr3Device : public cDevice {
public:
    cDxr3Device();
    ~cDxr3Device();

    virtual void MakePrimaryDevice(bool On);

    // replaying
    virtual bool HasDecoder() const;
    virtual bool CanReplay() const;
    virtual bool SetPlayMode(ePlayMode PlayMode);
    virtual int64_t GetSTC();
    virtual void TrickSpeed(int Speed);
    virtual void Clear();
    virtual void Play();
    virtual void Freeze();
    virtual void Mute();
    virtual void StillPicture(const uchar *Data, int Length);
    virtual bool Poll(cPoller &Poller, int TimeoutMs = 0);
    virtual int PlayVideo(const uchar *Data, int Length);
    virtual int PlayAudio(const uchar *Data, int Length, uchar Id);

    // additional functions
    virtual void SetVideoFormat(bool VideoFormat16_9);
    virtual void SetVolumeDevice(int Volume);
    virtual void SetAudioChannelDevice(int AudioChannel);
    virtual int GetAudioChannelDevice(void);

    // osd
    virtual cSpuDecoder *GetSpuDecoder();

    // helper function
    void Reset()
    {
	m_CalledBySet = true;
	SetPlayMode(m_PlayMode);
	m_CalledBySet = false;
    }

protected:
    ePlayMode m_PlayMode;
    cDxr3DemuxDevice m_DemuxDevice;
    bool m_CalledBySet;
    std::string m_strBuf;
    int m_Offset;

    cDxr3SpuDecoder* m_spuDecoder;

private:
    iAudio *audioOut;
};

#endif /*_DXR3_DEVICE_H_*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
