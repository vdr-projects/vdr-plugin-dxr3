#ifndef _DXR3_DEVICE_H_
#define _DXR3_DEVICE_H_

#include "dxr3interface.h"
#include "dxr3log.h"
#include "dxr3demuxdevice.h"
#include "dxr3spudecoder.h"
#include <string>
using namespace std;

// ==================================
// our device :)
/*!
	cDxr3Device is the interface for VDR devices.
	Is is the part, which VDR "talks" with our plugin.
*/
class cDxr3Device : public cDevice, public Singleton<cDxr3Device>
{
public:
    cDxr3Device();
	cDxr3Device(cDxr3Interface& demuxDevice);
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
	virtual void PlayAudio(const uchar *Data, int Length);

	// addition functions
	virtual bool GrabImage(const char *FileName, bool Jpeg = true, int Quality = -1, int SizeX = -1, int SizeY = -1);
	virtual void SetVideoFormat(bool VideoFormat16_9);
	virtual void SetVolumeDevice(int Volume);

	// osd
	virtual cSpuDecoder *GetSpuDecoder();
	#if VDRVERSNUM < 10307
    virtual cOsdBase* NewOsd(int x, int y);
	#endif

	// helper function
	void Reset()		{ m_CalledBySet = true; SetPlayMode(m_PlayMode); m_CalledBySet = false; }

protected:
    ePlayMode			m_PlayMode;
	cDxr3DemuxDevice	m_DemuxDevice;
    bool				m_AC3Present;
    bool				m_CalledBySet;
	string				m_strBuf;
	int					m_Offset;

    //virtual bool SetPlayMode(ePlayMode PlayMode);
    //uint8_t m_pBuffer[MAX_VIDEO_BUFFER_SIZE];
    //cDxr3StartStopThread* m_pStartStopThread;
    cDxr3SpuDecoder* m_spuDecoder;
};

#endif /*_DXR3_DEVICE_H_*/
