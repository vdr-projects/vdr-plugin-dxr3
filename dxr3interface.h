/*
 * dxr3interface.h
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#ifndef _DXR3_INTERFACE_H_
#define _DXR3_INTERFACE_H_

#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <linux/em8300.h>
#include <sys/ioctl.h>

#include "dxr3vdrincludes.h"
#include "dxr3log.h"
#include "dxr3configdata.h"
#include "dxr3sysclock.h"
#include "dxr3osd.h"

// ==================================
class cFixedLengthFrame;

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

	// main
	void Start();
	void Stop();

	// audio
    void SetAudioAnalog();
    void SetAudioDigitalPCM();
    void SetAudioDigitalAC3();
	void SetVolume(int volume)				{ m_volume = volume;}
    void SetAudioSpeed(uint32_t speed);
    void SetChannelCount(uint32_t count);
	void SetAudioSampleSize(uint32_t sampleSize);

	// clock
    void SetSysClock(uint32_t scr);
    uint32_t GetSysClock() const;
    void SetPts(uint32_t pts);
    void SetSpuPts(uint32_t pts);

	// state changes
	void EnableSPU();
	void DisableSPU();
	void EnableVideo()			{ m_VideoActive = true; }
	void DisableVideo()			{ m_VideoActive = false; }
	void EnableAudio()			{ m_AudioActive = true; }
	void DisableAudio();
	void EnableOverlay();
	void DisanleOverlay();

	// set/get functions
	uint32_t GetAspectRatio() const;
	void SetAspectRatio(uint32_t ratio);
	uint32_t GetHorizontalSize() const			{ return m_horizontal; }
	void SetHorizontalSize(uint32_t horizontal) { m_horizontal = horizontal;};
	
	// play functions
    void SetPlayMode();
    void Pause();
    void SingleStep();
    void PlayVideoFrame(cFixedLengthFrame* pFrame, int times = 1); 
    void PlayVideoFrame(const uint8_t* pBuf, int length, int times = 1);
    void PlayAudioFrame(cFixedLengthFrame* pFrame);
    void PlayAudioFrame(uint8_t* pBuf, int length);
    void PlayAudioLpcmFrame(uint8_t* pBuf, int length);

	// external device access
    void ExternalReleaseDevices();
    void ExternalReopenDevices();
    bool IsExternalReleased() const		{ return m_ExternalReleased; }

	// tools
	void PlayBlackFrame();
	void ReOpenAudio();

	// osd/spu
	#if VDRVERSNUM < 10307
	cOsdBase* NewOsd(int x, int y);
	#endif

    void ClearOsd();
    void WriteSpu(const uint8_t* pBuf, int length);
    void SetButton(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t palette);
    void ClearButton();
    void SetPalette(unsigned int *pal = NULL);

	// overlay functions

	// helper functions for dxr3 main osd screen
	void ResetHardware();

	// set brightness/contrast/saturation
	void SetBrightness(int value);
	void SetContrast(int value);
	void SetSaturation(int value);

private:
	// file handles
	int m_fdControl;	///< filehandle for contol fifo of dxr3 card
    int m_fdVideo;		///< filehandle for video fifo of dxr3 card
    int m_fdAudio;		///< filehandle for audio fifo of dxr3 card
    int m_fdSpu;		///< filehandle for spu fifo of dxr3 card

	// dxr3 clock
	cDxr3SysClock*	m_pClock;	///< clock used for sync

    uint32_t		m_audioChannelCount;	///< how many channles has the current audiostream
    uint32_t		m_audioDataRate;		///< which rate is used for the current audiostream
    int				m_aspectDelayCounter;
    uint32_t		m_aspectRatio;			///< current used aspect ratio
    uint32_t		m_horizontal;			///< horizontal size of current videostream
    uint32_t		m_audioSampleSize;		///< how big is the sample size for the current audiostream
    uint32_t		m_audioMode;
	uint32_t		m_spuMode;
    bool			m_ExternalReleased;	// is dxr3 used by e.g. mplayer?
    int				m_volume;				///< volumevalue (0...255)
    bool			m_AudioActive;			///< is audio active?
    bool			m_VideoActive;			///< is video active?
	bool			m_OverlayActive;		///< is overlay active?

	// bcs
	em8300_bcs_t	m_bcs;					///< BrightnessContrastSaturation values

	// spu
//    cDxr3InterfaceSpu		m_SpuInterface;

	void UploadMicroCode();
	void ConfigureDevice();
	void ResampleVolume(short* pcmbuf, int size);
	void Resuscitation();

protected:
    static cMutex* m_pMutex;				///< mutex for dxr3interface

    static void Lock()		{ cDxr3Interface::m_pMutex->Lock(); }
    static void Unlock()	{ cDxr3Interface::m_pMutex->Unlock(); }
};

#endif /*_DXR3_INTERFACE_H_*/
