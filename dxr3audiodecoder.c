/*
 * dxr3audiodecoder.c
 *
 * Copyright (C) 2002-2004 Kai M�ller
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


/*
ToDo:
	- cDxr3AudioDecoder::Init: Why are we always reinit the codec?
*/


#include <stdio.h>
#include "dxr3audiodecoder.h"
#include "dxr3pesframe.h"

// ==================================
const int cLPCM_HEADER_LENGTH = 7;

// ==================================
//! constructor
cDxr3AudioDecoder::cDxr3AudioDecoder() : m_RBuf(50000), m_AC3dtsDecoder(&m_RBuf) 
{
	// default start values
    m_DecoderOpened = false;
    m_AudioSynched = false;
    m_Volume = 255;
	m_Codec.id = CODEC_ID_MP2;

    Init();

	// setuo our header
    m_LastHeader[0] = 0xFF;
    m_LastHeader[1] = m_LastHeader[2] = m_LastHeader[3] = 0;
};

// ==================================
//! deconst.
cDxr3AudioDecoder::~cDxr3AudioDecoder() 
{
	// close codec, if it is open
	cDxr3Ffmepg::Instance().CloseCodec(m_Codec);
};

// ==================================
//! (re)init ffmpeg codec
void cDxr3AudioDecoder::Init() 
{
	// (re)init codec
	cDxr3Ffmepg::Instance().CloseCodec(m_Codec);
	if (cDxr3Ffmepg::Instance().FindCodec(m_Codec))
	{
		cDxr3Ffmepg::Instance().OpenCodec(m_Codec);
		m_Rate = m_Channels = -1;
		m_FrameSize = m_Codec.codec_context.frame_size;
		m_DecoderOpened = true;
		m_FoundHeader = false;
		m_DecodeAudio = true;
	}
	else
	{
		m_DecoderOpened = false;
	}
}

// ==================================
//! decode given buffer
void cDxr3AudioDecoder::Decode(const uint8_t* buf, int length, uint32_t pts, cDxr3SyncBuffer &aBuf) 
{
	if (!m_DecoderOpened)
	{
		// No decoder is open, so it
		// is better to stop here.
		return;
	}

    int len;
    int out_size;

    enum audioException 
	{
        WRONG_LENGTH,
        UNEXPECTED_PARAMETER_CHANGE
    };

    int i = 0;
    for (i = 0; i < length-4 && !m_FoundHeader; i++) 
	{
        unsigned int tempHead = *((unsigned int*)(buf+i));
        if (HeadCheck(tempHead)) 
		{
            if ((buf[i+2] & 0xFC) != (m_LastHeader[2] & 0xFC)) 
			{
				cLog::Instance() << "cDxr3AudioDecoder::Decode Found different audio header -> init\n";
                cLog::Instance() << "cDxr3AduioDecoder::Decode Old header 0x" << std::hex << *((uint32_t*) m_LastHeader) << " new header 0x" << *((uint32_t*) (buf+i))<< std::dec << "\n";

                Init();
                m_LastHeader[0] = buf[i];
                m_LastHeader[1] = buf[i+1];
                m_LastHeader[2] = buf[i+2];
                m_LastHeader[3] = buf[i+3];
            }
            m_FoundHeader = true;
        }
    }

    if (m_AudioSynched) 
	{
		// no header found
        m_DecodeAudio = true;
    } 
	else 
	{
        if (m_FoundHeader && pts) 
		{
            m_DecodeAudio = true;
            m_AudioSynched = true;
        }
    }

    try 
	{
        while (length > 0 && m_DecodeAudio) 
		{
            len = avcodec_decode_audio(&m_Codec.codec_context, (short *)(&m_PcmBuf), &out_size,
            const_cast<uint8_t *>(buf), length);
            if (len < 0 || out_size < 0) throw WRONG_LENGTH;

            if (m_Codec.codec_context.sample_rate != m_Rate) 
			{
				cLog::Instance() << "cDxr3AudioDecoder::Decode Sample rate = " << m_Codec.codec_context.sample_rate << "\n";
                if (m_Rate != -1) throw UNEXPECTED_PARAMETER_CHANGE;
                m_Rate = m_Codec.codec_context.sample_rate;
            }
            if (m_Codec.codec_context.channels != m_Channels+1 ) 
			{
                if (m_Channels != -1) throw UNEXPECTED_PARAMETER_CHANGE;
                m_Channels = (m_Codec.codec_context.channels == 2) ? 1 : 0;
                cLog::Instance() << "cDxr3AudioDecoder::Decode channels = " << m_Codec.codec_context.channels << "\n";
            }
            if (out_size) 
			{
                cFixedLengthFrame* pTempFrame = aBuf.Push(m_PcmBuf, out_size, pts);
                pTempFrame->SetChannelCount(m_Channels);
                pTempFrame->SetDataRate(m_Rate);
            }
            length -= len;
            buf += len;
        }
    } 
	catch (audioException ex) 
	{
        switch (ex) 
		{
            case WRONG_LENGTH:
				cLog::Instance() << "cDxr3AudioDecoder::Decode wrong length\n";
			    break;

            case UNEXPECTED_PARAMETER_CHANGE:
                cLog::Instance() << "cDxr3AudioDecoder::Decode unexpected parameter change\n";
				break;

            default:
                cLog::Instance() << "cDxr3AudioDecoder::Decode unexpeced exception\n";
			    break;
        }
        dsyslog("cDxr3AudioDecoder::Decode skipping %d broken data bytes", length);

        Init();
    }
}

// ==================================
//! decode lpcm
void cDxr3AudioDecoder::DecodeLpcm(const uint8_t* buf, int length, uint32_t pts, cDxr3SyncBuffer &aBuf) 
{
    if (length > (cLPCM_HEADER_LENGTH + 2)) 
	{
        uint8_t* pFrame = new uint8_t[length - cLPCM_HEADER_LENGTH];
        assert(!((length - cLPCM_HEADER_LENGTH) % 2)); // only even number of bytes are allowed

        for (int i = cLPCM_HEADER_LENGTH; i < length; i += 2) 
		{
            pFrame[i - cLPCM_HEADER_LENGTH] = buf[i + 1];
            pFrame[i - cLPCM_HEADER_LENGTH + 1] = buf[i];
        }

        int codedSpeed = (buf[5] >> 4) & 0x03;
        int speed = 0;

        switch (codedSpeed) 
		{
            case 1:
                speed = 96000;
			    break;

            case 2:
                speed = 44100;
				break;

            case 3:
                speed = 32000;
				break;

            default:
                speed = 48000;
				break;
        }

        cFixedLengthFrame* pTempFrame = aBuf.Push(pFrame, length - cLPCM_HEADER_LENGTH, pts);
        pTempFrame->SetChannelCount(1);
        pTempFrame->SetDataRate(speed);
        
        delete[] pFrame;
    }
}

// ==================================
//! decode ac3
void cDxr3AudioDecoder::DecodeAc3Dts(const uint8_t* pPes, const uint8_t* buf, int length, uint32_t pts, cDxr3SyncBuffer &aBuf) 
{
    int headerLength = (int) (buf - pPes);

    uint8_t* pBuf = (uint8_t*) pPes;
    m_AC3dtsDecoder.Check(pBuf + headerLength, length, pBuf);
    m_AC3dtsDecoder.Encapsulate(pBuf + headerLength, length);
                
    cFrame* pFrame = 0;
    while ((pFrame = m_RBuf.Get())) 
	{
        if (pFrame && pFrame->Count()) 
		{
            cDxr3PesFrame tempPes;
            tempPes.ExtractNextFrame(pFrame->Data(), pFrame->Count());
            int pesHeaderLength = (int) (tempPes.GetEsStart() - tempPes.GetPesStart());
            uint8_t* pData = pFrame->Data() + pesHeaderLength + cLPCM_HEADER_LENGTH;

            for (int i = 0; i < pFrame->Count() - pesHeaderLength - cLPCM_HEADER_LENGTH; i += 2) 
			{
                std::swap(pData[i], pData[i + 1]);
            }
        
            aBuf.Push(pFrame->Data() + pesHeaderLength + cLPCM_HEADER_LENGTH, pFrame->Count() - pesHeaderLength - 7, tempPes.GetPts());
            if (pFrame) m_RBuf.Drop(pFrame);
        }
    }
}    

// ==================================
//! checking routine
bool cDxr3AudioDecoder::HeadCheck(unsigned long head) 
{
    bool retval = false;
    
    uint8_t* phead = (uint8_t*) (&head);
    if (phead[0] != 0xFF) 
	{
        retval = false;
    } 
	else if (phead[1] != 0xFC && phead[1] != 0xFE) 
	{
        retval = false;
    } 
	else if ((phead[2] & 0xF0) == 0xF0) 
	{
        retval = false;
    } 
	else if ((phead[2] & 0xC) == 0xC) 
	{
        retval = false;
    }
	else 
	{
        retval = true;
    }

    return retval;
}
