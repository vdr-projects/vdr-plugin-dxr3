/*
 * dxr3output-audio.c
 *
 * Copyright (C) 2002-2004 Kai Möller
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#include <stdio.h>
#include <time.h>
#include "dxr3output.h"
#include "dxr3audio.h"
#include "dxr3pesframe.h"

// ==================================
const int AUDIO_OFFSET = 4500;
#define SCR m_dxr3Device->GetSysClock()
// ==================================

// ==================================
//! constr.
cDxr3AudioOutThread::cDxr3AudioOutThread(cDxr3SyncBuffer& buffer) :
    cDxr3OutputThread(buffer)
{
    SetDescription("DXR3 audio output");
}

// ==================================
//! thread action
void cDxr3AudioOutThread::Action()
{
    bool resync = false;
    uint32_t pts = 0;

    while (Running())
    {
	pts = 0;
	cFixedLengthFrame* pNext = m_buffer.Get();

	if (pNext) pts = pNext->GetPts();

	if ((pts && abs((int)pts-(int)SCR) > 30000))
	{
	    m_buffer.Clear();
	    m_bNeedResync = true;
	}
	else if (pNext)
	{
	    if (!pts || pts < SCR)
	    {
		if (!pts && resync)
		{
		    continue;
		}
		else
		{
		    resync = false;
		}

		if (pts && (pts < SCR) && ((SCR - pts) > 5000))
		{
		    m_dxr3Device->SetSysClock(pts + 1 * AUDIO_OFFSET);
		    PlayFrame(pNext);
		    if (m_buffer.IsPolled())
		    {
			m_buffer.Clear();
			m_bNeedResync = true;
		    }
		}
		else
		{
		    PlayFrame(pNext);
		    m_buffer.Pop();
		}
	    }
	    else
	    {
		if (abs((int)pts - (int)SCR) < (AUDIO_OFFSET ))
		{
		    PlayFrame(pNext);
		    m_buffer.Pop();
		}
	    }
	}

	if ((pts > SCR && abs((int)pts - (int)SCR) > AUDIO_OFFSET))
	{
	    cCondWait::SleepMs(10);
	}
    }
}

void cDxr3AudioOutThread::PlayFrame(cFixedLengthFrame *frame)
{
    // update audio context
    SampleContext ctx;
    ctx.samplerate = frame->GetSampleRate();
    ctx.channels = frame->GetChannelCount();

    // TODO find cause why we need this workaround
    if (ctx.samplerate != -1 && ctx.channels != -1) {
        audioOutput->setup(ctx);
    } else {
        dsyslog("[fixme] samplerate: %d channels: %d", ctx.samplerate, ctx.channels);
    }

    // volume changes
    if (!audio()->isAudioModeAC3()) {
        audioOutput->changeVolume((short *)frame->GetData(), (size_t)frame->GetCount());
    }

    audioOutput->write(frame->GetData(), frame->GetCount());
}

void cDxr3AudioOutThread::PlayFrame(cDxr3PesFrame *frame)
{
    // update audio context
    audioOutput->setup(frame->ctx);

    // volume changes
    if (!audio()->isAudioModeAC3()) {
        audioOutput->changeVolume((short *)frame->decoded, (size_t)frame->decodedSize);
    }

    audioOutput->write((uchar *)frame->decoded, frame->decodedSize);
}

#undef SCR

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// tab-width: 4;
// indent-tabs-mode: nil
// End:
