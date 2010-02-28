/*
 * dxr3multichannelaudio.c:
 *
 * taken from the AC3overDVB Patch maintained by Stefan Huelswitt
 *
 *
 */

#include <malloc.h>
#include <algorithm>
#include <vdr/ringbuffer.h>
#include "dxr3multichannelaudio.h"

//#define DEBUG(x...) printf(x)
#define DEBUG(x...)

//#define ED(x...) printf(x)
#define ED(x...)

#define aAC3  0x80
#define aDTS  0x88
#define aLPCM 0xA0
#define aMPEG 0xC0

#define aVDR  0x0B           // VDR specific audio substream
#define aSPU  0x20           // SPU stream

#define PES_HDR_SIZE   6     // length of PES header
#define PTS_SIZE       5     // length of PTS data
#define MAX_FRAMECOUNT 1536  // max. LPCM payload size

#define SYNC_SIZE      7     // how many bytes needed to sync on a audio header

#define AC3_SIZE       6144  // size of AC3 IEC paket
#define DTS_SIZE       2048  // size of DTS IEC paket
#define IEC_HDR_SIZE   8     // size of IEC header

// --- cAudioEncapsulator -----------------------------------------------------

class cAudioEncapsulator
{
private:
    int totalSize, frameCount;
    cFrame *frame;
    uchar *frameData;
    //
    uchar syncBuff[SYNC_SIZE];
    int have, length, skipped;
    //
    uchar ptsFlags;
    const uchar *ptsData;
    int ptsDelay;
    //
    void NewFrame(uchar PTSflags, const uchar *PTSdata);
    void SyncFound(const uchar *data);
protected:
    int streamType;
    cRingBufferFrame *ringBuffer;
    int fillup, firstBurst;
    bool mute, muteData;
    //
    void StartFrame(int size, uchar PTSflags, const uchar *PTSdata);
    void FinishFrame(void);
    void PutData(const uchar *data, int len);
    void SendIECpause(int type, uchar PTSflags, const uchar *PTSdata);
    //
    virtual int SyncInfo(const uchar *data)=0;
    virtual void StartIECFrame(const uchar *buf, int length, uchar PTSflags,
			       const uchar *PTSdata) = 0;
    virtual void FinishIECFrame(void);
public:
    cAudioEncapsulator(cRingBufferFrame *rb, int StreamType);
    virtual ~cAudioEncapsulator();
    void Clear(void);
    void Decode(const uchar *data, int len, uchar PTSflags, int PTSdelay,
		const uchar *PTSdata);
    int StreamType()
    {
	return streamType;
    }
    void Mute(bool Mute)
    {
	mute = Mute;
    }
};

cAudioEncapsulator::cAudioEncapsulator(cRingBufferFrame *rb, int StreamType)
{
    ringBuffer = rb;
    streamType = StreamType;
    frame = 0;
    firstBurst = 1;
    Clear();
}

cAudioEncapsulator::~cAudioEncapsulator()
{
    delete frame;
}

void cAudioEncapsulator::Clear(void)
{
    delete frame;
    frame = 0;
    frameCount = 0;
    fillup = 0;
    mute = muteData = false;
    have = length = skipped = 0;
}

void cAudioEncapsulator::StartFrame(int size, uchar PTSflags,
				    const uchar *PTSdata)
{
    if (frame)
    {
	DEBUG("StartFrame() with unfinished frame!\n");
	FinishFrame();
    }
    ED("StartFrame: size=%d ptsFlags=%d\n", size, PTSflags);
    totalSize = size;
    NewFrame(PTSflags, PTSdata);
}

void cAudioEncapsulator::NewFrame(uchar PTSflags, const uchar *PTSdata)
{
    if (!totalSize)
    {
	DEBUG("NewFrame: new frame requested, but totalSize=0\n");
	return;
    }
    static const int ptslen[] = { 0, 0, PTS_SIZE, PTS_SIZE*2 };
    const int plen = ptslen[PTSflags];
    int len = std::min(totalSize, MAX_FRAMECOUNT);
    ED("NewFrame: totalSize=%d frameCount=%d PTSflags=%d", totalSize, len, PTSflags);
    totalSize -= len;
    ED(" new totalSize=%d\n",totalSize);
    len += (plen + 3 + 7);
    frameCount = len + PES_HDR_SIZE;
    frameData = MALLOC(uchar, frameCount);
    if (frameData)
    {
	frame = new cFrame(frameData, -frameCount, ftUnknown);
	if (frame)
	{
	    uchar buf[10];
	    // build the PES header
	    buf[0] = 0x00;
	    buf[1] = 0x00;
	    buf[2] = 0x01;
	    buf[3] = 0xBD; // PRIVATE_STREAM1
	    buf[4] = (len >> 8) & 0xFF;
	    buf[5] = len & 0xFF;
	    buf[6] = 0x84;
	    buf[7] = plen ? (PTSflags << 6) : 0;
	    buf[8] = plen;
	    PutData(buf, 9);

	    if (plen)
		PutData(PTSdata, plen);

	    // build LPCM header
	    buf[0] = aLPCM; // substream ID
	    buf[1] = 0xFF;
	    buf[2] = 0x00;
	    buf[3] = 0x00;
	    buf[4] = 0x00;
	    buf[5] = 0x00;
	    buf[6] = 0x81;
	    PutData(buf, 7);
	    return;
	}
	else
	{
	    free(frameData);
	    frameData = 0;
	}
    }
    esyslog("Failed to build frame for audio encapsulation");
}

void cAudioEncapsulator::FinishFrame(void)
{
    if (frameCount)
    {
	DEBUG("FinishFrame() with frameCount>0\n");
	PutData(0, frameCount);
    }
    if (frame && frameData)
    {
	ED("FinishFrame: totalSize=%d\n", totalSize);
	if (!ringBuffer->Put(frame)) {
	    esyslog("Ringbuffer overflow. Encapsulated audio frame lost");
	    delete frame;
	}
    }
    frame = 0;
    frameData = 0;
    frameCount = 0;
}

void cAudioEncapsulator::PutData(const uchar *data, int len)
{
    if (!muteData)
    {
	if (!frameData) {
	    DEBUG("PutData() without frame\n");
	}
	while (frameData && len > 0)
	{
	    int l = std::min(len, frameCount);
	    if (data)
	    {
		memcpy(frameData, data, l);
		data += l;
	    }
	    else
		memset(frameData, 0, l);
	    frameData += l;
	    len -= l;
	    frameCount -= l;

	    ED("PutData: %s=%d len=%d frameCount=%d\n",
	       data ? "put" : "zero", l, len, frameCount);
	    if (!frameCount)
	    {
		FinishFrame();
		if (totalSize > 0)
		    NewFrame(0, 0);
	    }
	}
    }
}

void cAudioEncapsulator::SendIECpause(int type, uchar PTSflags,
				      const uchar *PTSdata)
{
    StartFrame(AC3_SIZE, PTSflags, PTSdata);
    uchar burst[IEC_HDR_SIZE];
    // prepare IEC 60958 data frame
    burst[0] = 0xF8;
    burst[1] = 0x72;
    burst[2] = 0x4E;
    burst[3] = 0x1F;

    switch (type) {
    default:
    case 0:
	burst[4] = 7 << 5;          // null frame, stream = 7
	burst[5] = 0x00;
	burst[6] = 0x00;            // No data therein
	burst[7] = 0x00;
	break;
    case 1:
	burst[4] = 0x00;            // Audio ES Channel empty, wait
	burst[5] = 0x03;            //   for DD Decoder or pause
	burst[6] = 0x00;            // Trailing frame size is 32 bits payload
	burst[7] = 0x20;
	break;
    case -1:
	burst[4] = 0x01;            // User stop, skip or error
	burst[5] = 0x03;
	burst[6] = 0x08;            // Trailing frame size is zero
	burst[7] = 0x00;
	break;
    }
    PutData(burst, sizeof(burst));
    PutData(0, AC3_SIZE - sizeof(burst));
    FinishFrame();
    muteData = true;
}

void cAudioEncapsulator::FinishIECFrame(void)
{
    if (!muteData)
    {
	ED("FinishIECFrame: fillup=%d\n", fillup);
	if (fillup)
	    PutData(0, fillup);
	FinishFrame();
    }
    muteData = false;
    fillup = 0;
}

void cAudioEncapsulator::SyncFound(const uchar *data)
{
    if (skipped)
    {
	DEBUG("Decode: skipped %d bytes\n", skipped);
	ED("skipped: "); for(int k = -skipped; k < 0; k++) ED("%02x ",data[k]);
	ED("\ndata: "); for(int k = 0; k<24; k++) ED("%02x ", data[k]);
	ED("\n");
	skipped = 0;
    }
    uchar pf = 0;
    ED("Decode: sync found ptsFlags=%d ptsDelay=%d\n", ptsFlags, ptsDelay);
    if (ptsFlags && ptsDelay <= 1)
    {
	pf = ptsFlags;
	ptsFlags = 0;
    }
    if (firstBurst || mute)
    {
	SendIECpause(1, pf, ptsData);
	if (firstBurst && ++firstBurst>10)
	    firstBurst = 0;
    }
    else
	StartIECFrame(data, length, pf, ptsData);
    PutData(data, SYNC_SIZE);
    have = SYNC_SIZE;
}

void cAudioEncapsulator::Decode(const uchar *data, int len, uchar PTSflags,
				int PTSdelay, const uchar *PTSdata)
{
    ED("Decode: enter length=%d have=%d len=%d PTSflags=%d PTSdelay=%d\n",
       length, have, len, PTSflags, PTSdelay);
    if (PTSflags)
    {
	// if we are close to the end of an audio frame, but are already
	// receiving the start of the next frame, assume a corrupted stream
	// and finish the incomplete frame.
	if (length && length-have < 20 && !PTSdelay && SyncInfo(data))
	{
	    int miss = length - have;
	    DEBUG("Decode: incomplete frame (stream corrupt?). syncing to next. miss=%d\n", miss);
	    PutData(0, miss);
	    FinishIECFrame();
	    length = have = 0;
	}
	/*
	// we only send PTS info if we're nearly at frame start, except
	// if we're signaled to delay the PTS
	if (length && have > 40)
	{
	    if(PTSdelay)
		ED("Decode: PTS delayed\n");
	    else
	    {
		DEBUG("Decode: PTS info dropped length=%d have=%d\n",
		      length, have);
		PTSflags = 0;
	    }
	}
	*/
	ptsFlags = PTSflags;
	ptsData = PTSdata;
	ptsDelay = PTSdelay;
	//ED("Decode: saved PTS flags=%d delay=%d\n", ptsFlags, ptsDelay);
    }

#if 0
    {
	printf("Decode: len=%d\n", len);
	for (int i = 0; i < len; )
	{
	    printf("%04x:", i);
	    for (int j = 0; j < 16 && i < len; j++)
		printf(" %02x", data[i++]);
	    printf("\n");
	}
    }
#endif

    int used = 0;
    while (used < len)
    {
	if (!length) // we are still searching for a header sync
	{
	    if (!have) // buffer is empty, work without buffering
	    {
		if (used + SYNC_SIZE < len)
		{
		    length = SyncInfo(&data[used]);
		    if (length)
		    {
			ED("Decode: sync found at offset %d (len=%d)\n",
			   used, length);
			SyncFound(&data[used]);
			used += SYNC_SIZE;
			ptsDelay -= SYNC_SIZE;
			continue;
		    }
		    else
		    {
			used++;
			skipped++;
		    }
		}
		else // not enough data to try a sync, buffer the rest
		{
		    ED("Decode: buffering started\n");
		    have = len - used;
		    memcpy(syncBuff, &data[used], have);
		    used += have;
		    ptsDelay -= have;
		}
	    }
	    else // unfortunately buffer is not empty, so continue with
		 // buffering until sync found
	    {
		int need = std::min(SYNC_SIZE - have, len - used);
		if (need)
		{
		    memcpy(&syncBuff[have], &data[used], need);
		    have += need;
		    used += need;
		    ptsDelay -= need;
		}
		if (have == SYNC_SIZE)
		{
		    length = SyncInfo(syncBuff);
		    if (length)
		    {
			ED("Decode: (buffered) sync found at offset %d (len=%d)\n",used-7,length);
			SyncFound(syncBuff);
			continue;
		    }
		    else
		    {
			memmove(syncBuff, syncBuff + 1, SYNC_SIZE - 1);
			have--;
			skipped++;
		    }
		}
	    }
	}
	else // we have a header sync and are copying data
	{
	    int need = std::min(length - have, len - used);
	    if (need)
	    {
		ED("Decode: writing %d\n", need);
		PutData(&data[used], need);
		have += need;
		used += need;
		ptsDelay -= need;
		if (have == length)
		{
		    FinishIECFrame();
		    length = have = 0;
		    continue;
		}
	    }
	}
    }
    ED("Decode: leave length=%d have=%d len=%d used=%d\n",
       length, have, len, used);
}

// --- cAudioEncapsulatorAC3 --------------------------------------------------

class cAudioEncapsulatorAC3 : public cAudioEncapsulator {
private:
    virtual int SyncInfo(const uchar *buf);
    virtual void StartIECFrame(const uchar *buf, int length, uchar PTSflags,
			       const uchar *PTSdata);
public:
    cAudioEncapsulatorAC3(cRingBufferFrame *rb, int StreamType);
};

cAudioEncapsulatorAC3::cAudioEncapsulatorAC3(cRingBufferFrame *rb,
					     int StreamType)
    : cAudioEncapsulator(rb, StreamType)
{}

int cAudioEncapsulatorAC3::SyncInfo(const uchar *buf)
{
    static const int rate[] = {  32,  40,  48,  56,  64,  80,  96, 112,
				128, 160, 192, 224, 256, 320, 384, 448,
				512, 576, 640 };

    if ((buf[0] != 0x0B) || (buf[1] != 0x77))   /* syncword */
	return 0;
    if (buf[5] >= 0x60)         /* bsid >= 12 */
	return 0;

    int frmsizecod = buf[4] & 63;
    if (frmsizecod >= 38)
	return 0;
    int bitrate = rate[frmsizecod >> 1];

    switch (buf[4] & 0xC0) {
    case 0:
	return 4 * bitrate;
    case 0x40:
	return 2 * (320 * bitrate / 147 + (frmsizecod & 1));
    case 0x80:
	return 6 * bitrate;
    default:
	return 0;
    }
}

void cAudioEncapsulatorAC3::StartIECFrame(const uchar *buf, int length,
					  uchar PTSflags, const uchar *PTSdata)
{
    StartFrame(AC3_SIZE, PTSflags, PTSdata);
    fillup = AC3_SIZE - IEC_HDR_SIZE - length;

    // prepare IEC 60958 data frame
    uchar burst[IEC_HDR_SIZE];
    burst[0] = 0xF8;
    burst[1] = 0x72;
    burst[2] = 0x4E;
    burst[3] = 0x1F;
    burst[4] = (buf[5] & 0x07);                          // Pc1
    burst[5] = 0x01;                                     // Pc2 AC-3
    burst[6] = ((length * 8) >> 8 ) & 0xFF;              // Pd1
    burst[7] = (length * 8) & 0xFF;                      // Pd2
    PutData(burst,sizeof(burst));
}

// --- cAudioEncapsulatorDTS --------------------------------------------------

class cAudioEncapsulatorDTS : public cAudioEncapsulator
{
private:
    virtual int SyncInfo(const uchar *buf);
    virtual void StartIECFrame(const uchar *buf, int length, uchar PTSflags,
			       const uchar *PTSdata);
public:
    cAudioEncapsulatorDTS(cRingBufferFrame *rb, int StreamType);
};

cAudioEncapsulatorDTS::cAudioEncapsulatorDTS(cRingBufferFrame *rb,
					     int StreamType)
    : cAudioEncapsulator(rb, StreamType)
{}

int cAudioEncapsulatorDTS::SyncInfo(const uchar *buf)
{
    if ((buf[0] != 0x7F) ||
	(buf[1] != 0xfE) ||
	(buf[2] != 0x80) ||
	(buf[3] != 0x01))
	return 0;

    int length = ((buf[5] & 0x03) << 12) |
	((buf[6] & 0xFF) << 4) |
	((buf[7] & 0xF0) >> 4);

    return length + 1;
}

void cAudioEncapsulatorDTS::StartIECFrame(const uchar *buf, int length,
					  uchar PTSflags, const uchar *PTSdata)
{
    uchar ac5_type = ((buf[4] & 0x01) <<  6) | ((buf[5] >>2) & 0x3F);
    uchar ac5_spdif_type;
    switch(ac5_type)
    {
    case 0x0F:
	ac5_spdif_type = 0x0B;  // DTS
	break;
    case 0x1F:
	ac5_spdif_type = 0x0C;  // DTS
	break;
    case 0x3F:
	ac5_spdif_type = 0x0D;  // DTS
	break;
    default:
	ac5_spdif_type = 0x00;  // DTS
	esyslog("DTS: SPDIF type not detected: ac5 type = %X!\n", ac5_type);
	break;
    }

    if (length > DTS_SIZE-IEC_HDR_SIZE)
    {
	DEBUG("DTS: length too long %d\n", length);
	return;
    }

    StartFrame(DTS_SIZE, PTSflags, PTSdata);
    fillup = DTS_SIZE - IEC_HDR_SIZE - length;

    // prepare IEC 60958 data frame
    uchar burst[IEC_HDR_SIZE];
    burst[0] = 0xF8;
    burst[1] = 0x72;
    burst[2] = 0x4E;
    burst[3] = 0x1F;
    burst[4] = 0x00;
    burst[5] = ac5_spdif_type;                    // DTS data
    burst[6] = ((length * 8) >> 8) & 0xFF;        // ac5_length * 8
    burst[7] = (length * 8) & 0xFF;
    PutData(burst,sizeof(burst));
}

// --- cMultichannelAudio -----------------------------------------------------

cMultichannelAudio::cMultichannelAudio(cRingBufferFrame *rb)
{
    encapsulator = 0;
    ringBuffer = rb;
    fixed = false;
    if (!ringBuffer) {
        DEBUG("multichannel: no ringbuffer!");
    }
}

cMultichannelAudio::~cMultichannelAudio()
{
    delete encapsulator;
}

void cMultichannelAudio::Clear()
{
    Lock();
    if (encapsulator)
	encapsulator->Clear();
    Unlock();
}

void cMultichannelAudio::Reset()
{
    Lock();
    delete encapsulator;
    encapsulator = 0;
    fixed = false;
    Unlock();
}

/*
void cMultichannelAudio::Mute(bool Mute)
{
    Lock();
    if (encapsulator)
	encapsulator->Mute(Mute);
    Unlock();
}
*/

int cMultichannelAudio::Check(uchar *b, int length, uchar *header)
{
    Lock();
    int res = 0;
    ptsDelay = 0;
    offset = 0;
    ptsData = 0;

    // get PTS information
    ptsFlags = header[7] >> 6;
    if (ptsFlags)
	ptsData = &header[9];

    // AC3 frames may span over multiple PES packets. Unfortunately the
    // continuation packets start with the aLPCM code sometimes. Some magic
    // here to detect this case.
    uchar subStreamType = b[0];
    if (subStreamType != aVDR)
	subStreamType&=0xF8;
    bool aligned = header[6] & 4;
    if (!aligned)
    {
	uchar ost = encapsulator ? encapsulator->StreamType() : 0;
	if (!ptsFlags)
	{
	    if ((subStreamType != aLPCM && subStreamType != aSPU) || fixed)
	    {
		if (ost > 0)
		{
		    ED("multichannel: crossing -> keep encapsulator\n");
		    subStreamType = ost;
		}
		else
		{
		    ED("multichannel: crossing -> skip\n");
		    res = 1;
		    goto out; // skip
		}
	    }
	}
	else if (fixed && ost > 0)
	{
	    ED("multichannel: fixed unaligned -> keep encapsulator\n");
	    subStreamType = ost;
	}
    }
    fixed = false;

    switch (subStreamType)
    {
    case aDTS:
    case aAC3:
	offset = 4; // skip the DVD stream infos
	break;
    default:
	if (aligned || !ptsFlags)
	{
	    if (encapsulator)
	    {
		Reset();
		DEBUG("multichannel: interrupted encapsulator stream (unknown)\n");
	    }
	    DEBUG("multichannel: unknown substream type %x (skipped)\n",
		  subStreamType);
	    res = 1;
	    goto out; // skip
	}
	subStreamType = aVDR;
	ED("multichannel: assuming aVDR for unknown substream type\n");
	// fall through
    case aVDR:
	fixed = true;
	break;
    case aLPCM:
	if (encapsulator)
	{
	    Reset();
	    DEBUG("multichannel: interrupted encapsulator stream (LPCM)\n");
	}
	ED("multichannel: LPCM\n");
	res = 2;
	goto out; // pass
    case aSPU:
	ED("multichannel: SPU stream (skipped)\n");
	res = 1;
	goto out; // skip
    }

    // If the SubStreamType has changed then select the right encapsulator
    if (!encapsulator || encapsulator->StreamType() != subStreamType)
    {
	DEBUG("multichannel: new encapsulator %x\n", subStreamType);
	Reset();
	switch (subStreamType)
	{
	case aAC3:
	case aVDR: // AC3
	    encapsulator = new cAudioEncapsulatorAC3(ringBuffer,subStreamType);
	    break;
	case aDTS: // Dts
	    encapsulator = new cAudioEncapsulatorDTS(ringBuffer,subStreamType);
	    break;
	}
	if (!encapsulator)
	{
	    DEBUG("multichannel: no encapsulator\n");
	    res = 1;
	    goto out; // skip
	}
    }

out:
    ED("HEADER type=%x sub=%x ptsflags=%d length=%d\n",
       header[3], subStreamType, ptsFlags, length);
    ED("head: ");   for(int k = 0; k < 24; k++) ED("%02x ", header[k]);
    ED("\ndata: "); for(int k = 0; k < 24; k++) ED("%02x ", b[k]);
    ED("\n");

    Unlock();
    return res;
}

void cMultichannelAudio::Encapsulate(uchar *b, int length)
{
    Lock();
    if(offset && ptsFlags)
    { // get start of the packet to which the PTS belong (DVD only)
	if (offset >= 2 && length > offset - 2)
	    ptsDelay |= b[offset - 2] * 256;
	if (offset >= 1 && length > offset - 1)
	    ptsDelay |= b[offset - 1];
    }
    if (length >= offset)
    {
	if (encapsulator)
	    encapsulator->Decode(b+offset, length - offset, ptsFlags,
				 ptsDelay, ptsData);
	ptsFlags = 0;
	ptsDelay = 0;
	offset = 0;
	ptsData = 0;
    }
    else
	offset -= length;
    Unlock();
}

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// c-basic-offset: 4
// indent-tabs-mode: nil
// End:
