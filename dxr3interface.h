/*
 * dxr3interface.h
 *
 * Copyright (C) 2002-2004 Kai Möller
 * Copyright (C) 2004-2009 Christian Gmeiner
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

#ifndef _DXR3_INTERFACE_H_
#define _DXR3_INTERFACE_H_

#include <linux/em8300.h>
#include <sys/ioctl.h>
#include <linux/limits.h>

#include <vdr/tools.h>
#include "dxr3configdata.h"
#include "dxr3sysclock.h"

// ==================================
class cFixedLengthFrame;
class cDxr3PesFrame;

class cDxr3Name {
public:
    cDxr3Name(const char *name, int n) {
        snprintf(buffer, sizeof(buffer), "%s%s-%d", "/dev/em8300", name, n);
    }
    const char *operator*() { return buffer; }

private:
    char buffer[PATH_MAX];
};

// ==================================
//! interafce to dxr3-card
/*!
  cDxr3Interface is the interface to the dxr3
  driver and so to the card,
  so this is the layer between plugin and driver.
*/
class cDxr3Interface : public Singleton<cDxr3Interface>
{
public:
    cDxr3Interface();
    ~cDxr3Interface();

    static int Dxr3Open(const char *name, int mode, bool report_error = true) {
        const char *filename = *cDxr3Name(name, cDxr3ConfigData::Instance().GetDxr3Card());
        int fd = open(filename, mode);

        if (report_error && (fd < 0)) {
            LOG_ERROR_STR(filename);
        }
        return fd;
    }

    // audio
    bool IsOssAudio();
    int OssSetPlayMode(uint32_t mode);

    int IsAudioModeAC3()    { return 0; }

    // clock
    void SetSysClock(uint32_t scr);
    uint32_t GetSysClock() const;
    void SetPts(uint32_t pts);
    void SetSpuPts(uint32_t pts);
    int64_t GetPts();

    // state changes
    void EnableSPU();
    void DisableSPU();
    void EnableVideo()
    {
	m_VideoActive = true;
    }
    void DisableVideo()
    {
	m_VideoActive = false;
    }
    void EnableAudio()
    {
	m_AudioActive = true;
    }
    void DisableAudio();

    // set/get functions
    uint32_t GetAspectRatio() const;
    void SetAspectRatio(uint32_t ratio);
    uint32_t GetHorizontalSize() const    { return m_horizontal; }
    uint32_t GetVerticalSize() const      { return m_vertical; }

    void SetHorizontalSize(uint32_t value)
    {
        if (value > 0)
            m_horizontal = value;
    }

    void SetVerticalSize(uint32_t value)
    {
        if (value > 0)
            m_vertical = value;
    };

    // play functions
    void SetPlayMode();
    void Pause();
    void SingleStep();
    void PlayVideoFrame(cFixedLengthFrame* pFrame, int times = 1);
    void PlayVideoFrame(const uint8_t* pBuf, int length, int times = 1);
    void PlayVideoFrame(cDxr3PesFrame *frame);

    // device access
    void ClaimDevices();
    void ReleaseDevices();

    // tools
    void PlayBlackFrame();
    void ReOpenAudio();

    // osd/spu

    void ClearOsd();
    void WriteSpu(const uint8_t* pBuf, int length);
    void SetButton(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey,
		   uint32_t palette);
    void ClearButton();
    void SetPalette(unsigned int *pal = NULL);

    // helper functions for dxr3 main osd screen
    void ResetHardware();

    // get/set brightness/contrast/saturation
    int GetBrightness();
    void SetBrightness(int value);
    int GetContrast();
    void SetContrast(int value);
    int GetSaturation();
    void SetSaturation(int value);

private:
    // file handles
    int m_fdControl;		///< filehandle for contol fifo of dxr3 card
    int m_fdVideo;		///< filehandle for video fifo of dxr3 card
    int m_fdSpu;		///< filehandle for spu fifo of dxr3 card
    uint32_t m_lastSeenPts;

    // dxr3 clock
    cDxr3SysClock* m_pClock;	///< clock used for sync

    uint32_t m_aspectRatio;	///< current used aspect ratio
    uint32_t m_horizontal;	///< horizontal size of current videostream
    uint32_t m_vertical;	///< vertical size of current videostream
    uint32_t m_spuMode;
    bool m_AudioActive;		///< is audio active?
    bool m_VideoActive;		///< is video active?

    // bcs
    em8300_bcs_t m_bcs;		///< BrightnessContrastSaturation values

    void UploadMicroCode();
    void ConfigureDevice();
    void Resuscitation();

    static cMutex* m_pMutex;	///< mutex for dxr3interface

    static void Lock()
    {
        cDxr3Interface::m_pMutex->Lock();
    }
    static void Unlock()
    {
        cDxr3Interface::m_pMutex->Unlock();
    }
};

#endif /*_DXR3_INTERFACE_H_*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
