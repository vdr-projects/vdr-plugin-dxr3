#ifndef _DXR3MULTICHANNELAUDIO_H_
#define _DXR3MULTICHANNELAUDIO_H_

#include "dxr3vdrincludes.h"

class cAudioEncapsulator;
class cRingBufferFrame;

// ==================================
// Based on AC3overDVB Patch maintained 
// by Stefan Huelswitt 
class cMultichannelAudio : public cMutex 
{
private:
    cAudioEncapsulator *encapsulator;
    cRingBufferFrame *ringBuffer;
    int ptsFlags, ptsDelay, offset;
    uchar *ptsData;
    bool fixed;

public:
    cMultichannelAudio(cRingBufferFrame *rb);
    virtual ~cMultichannelAudio();

    int Check(uchar *b, int length, uchar *header);
    int Offset(void)
    {
	return offset;
    }
    void Encapsulate(uchar *b, int length);
    void Clear();
    void Reset();
    //void Mute(bool Mute);
};

#endif /*_DXR3MULTICHANNELAUDIO_H_*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
