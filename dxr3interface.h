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
// interafce to dxr3-card
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

private:
	// file handles
	int m_fdControl;
    int m_fdVideo;
    int m_fdSpu;
    int m_fdAudio;

	// dxr3 clock
	cDxr3SysClock* m_pClock;

    uint32_t	m_audioChannelCount;
    uint32_t	m_audioDataRate;
    int			m_aspectDelayCounter;
    uint32_t	m_aspectRatio;
    uint32_t	m_horizontal;
    uint32_t	m_audioSampleSize;
    uint32_t	m_audioMode;
	uint32_t	m_spuMode;
    bool		m_ExternalReleased;	// is dxr3 used by e.g. mplayer?
    int			m_volume;
    bool		m_AudioActive;
    bool		m_VideoActive;
	bool		m_OverlayActive;

	// spu
//    cDxr3InterfaceSpu		m_SpuInterface;

	void UploadMicroCode();
	void ConfigureDevice();
	void ResampleVolume(short* pcmbuf, int size);
	void Resuscitation();

protected:
    static cMutex* m_pMutex;

    static void Lock()		{ cDxr3Interface::m_pMutex->Lock(); }
    static void Unlock()	{ cDxr3Interface::m_pMutex->Unlock(); }
};

#endif /*_DXR3_INTERFACE_H_*/
