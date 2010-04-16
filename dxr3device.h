/*
 *           _                 _             _                 _          _____
 * __   ____| |_ __      _ __ | |_   _  __ _(_)_ __         __| |_  ___ _|___ /
 * \ \ / / _` | '__|____| '_ \| | | | |/ _` | | '_ \ _____ / _` \ \/ / '__||_ \
 *  \ V / (_| | | |_____| |_) | | |_| | (_| | | | | |_____| (_| |>  <| |  ___) |
 *   \_/ \__,_|_|       | .__/|_|\__,_|\__, |_|_| |_|      \__,_/_/\_\_| |____/
 *                      |_|            |___/
 *
 * Copyright (C) 2002-2004 Kai Möller
 * Copyright (C) 2004-2010 Christian Gmeiner
 *
 * This file is part of vdr-plugin-dxr3.
 *
 * vdr-plugin-dxr3 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation version 2.
 *
 * vdr-plugin-dxr3 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with dxr3-plugin.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef _DXR3_DEVICE_H_
#define _DXR3_DEVICE_H_

//#include <string>
#include <vdr/device.h>

#include <linux/em8300.h>
#include "dxr3audiodecoder.h"
#include "dxr3spudecoder.h"
#include "dxr3audio.h"
#include "settings.h"
#include "uncopyable.h"

class cDxr3Name {
public:
    cDxr3Name(const char *name, int n) {
        snprintf(buffer, sizeof(buffer), "%s%s-%d", "/dev/em8300", name, n);
    }
    const char *operator*() { return buffer; }

private:
    char buffer[PATH_MAX];
};

/*!
  cDxr3Device is the interface for VDR devices.
  Is is the part, which VDR "talks" with our plugin.
*/
class cDxr3Device : public cDevice, public Uncopyable, public iSettingsObserver {
public:
    static cDxr3Device *instance();

    static int Dxr3Open(const char *name, int mode, bool report_error = true);
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

#if VDRVERSNUM >= 10710
    virtual void GetVideoSize(int &Width, int &Height, double &VideoAspect);
#endif

    // additional functions
    virtual void SetVideoFormat(bool VideoFormat16_9);
    virtual void SetVolumeDevice(int Volume);
    virtual void SetAudioChannelDevice(int AudioChannel);
    virtual int GetAudioChannelDevice();
    virtual void SetDigitalAudioDevice(bool on);

    // osd
    virtual cSpuDecoder *GetSpuDecoder();
    iAudio *getAudioOutput() const      { return audioOut; }

    void turnPlugin(bool on);

    void dimension(uint32_t &horizontal, uint32_t &vertical);

    void setPalette(uint8_t *pal);
    void writeSpu(const uint8_t* data, int length);
    void setButton(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t palette);
    void clearButton();

    int ossSetPlayMode(uint32_t mode);
    int ossFlush();

    // observer
    virtual void settingsChange(SettingsChange change);

private:
    cDxr3Device();
    ~cDxr3Device();

    void claimDevices();
    void releaseDevices();
    void uploadFirmware();
    void setPlayMode();
    void playVideoFrame(cDxr3PesFrame *frame, uint32_t pts);
    void playBlackFrame();
    void playSilentAudio();
    void writeRegister(int reg, int value);
    void setScr(uint32_t val);

    int offset;
    cMutex mutex;
    uchar *silentAudio;
    cDxr3AudioDecoder *aDecoder;
    cDxr3SpuDecoder* spuDecoder;
    iAudio *audioOut;
    bool pluginOn;

    em8300_bcs_t bcs;

    int fdControl;
    int fdVideo;
    int fdSpu;

    uint32_t vPts;
    bool scrSet;
    int playCount;

    uint32_t horizontal;
    uint32_t vertical;

    static cDxr3Device *inst;
};

#endif /*_DXR3_DEVICE_H_*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// tab-width: 4;
// indent-tabs-mode: nil
// End:
