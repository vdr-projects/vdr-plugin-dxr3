/*
 * dxr3output.h
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

#ifndef _DXR3OUTPUTTHREAD_H_
#define _DXR3OUTPUTTHREAD_H_

#include "dxr3syncbuffer.h"
#include "dxr3audiodecoder.h"
#include "uncopyable.h"

class iAudio;

// ==================================
class cDxr3OutputThread : public cThread, private Uncopyable {
public:
    cDxr3OutputThread(cDxr3SyncBuffer& buffer);
    virtual ~cDxr3OutputThread() {
        m_buffer.Stop();
        Cancel(3);
    };

    bool NeedResync() const     { return m_bNeedResync; }
    void ClearResyncRequest()   { m_bNeedResync = false; }

protected:
    virtual void Action() = 0;

    cDxr3Interface *m_dxr3Device;
    cDxr3SyncBuffer& m_buffer;
    bool m_bNeedResync;
};

// ==================================
class cDxr3AudioOutThread : public cDxr3OutputThread {
public:
    cDxr3AudioOutThread(cDxr3SyncBuffer& buffer);

    void setAudio(iAudio *a)    { audioOutput = a; }
    iAudio *audio()             { return audioOutput; }

protected:
    void Action();

private:
    iAudio *audioOutput;

    void PlayFrame(cFixedLengthFrame *frame);
};

// ==================================
class cDxr3VideoOutThread : public cDxr3OutputThread {
public:
    cDxr3VideoOutThread(cDxr3SyncBuffer& buffer);

protected:
    void Action();
};

#endif /*_DXR3OUTPUTTHREAD_H_*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// tab-width: 4;
// indent-tabs-mode: nil
// End:
