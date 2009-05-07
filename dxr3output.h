/*
 * dxr3output.h
 *
 * Copyright (C) 2002-2004 Kai Möller
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

#ifndef _DXR3OUTPUTTHREAD_H_
#define _DXR3OUTPUTTHREAD_H_

#include "dxr3syncbuffer.h"
#include "dxr3audiodecoder.h"
#include "Uncopyable.h"

class iAudio;

// ==================================
class cDxr3OutputThread : public cThread, private Uncopyable {
public:
    cDxr3OutputThread(cDxr3Interface& dxr3Device, cDxr3SyncBuffer& buffer);
    virtual ~cDxr3OutputThread() {
        m_buffer.Stop();
        Cancel(3);
    };

    bool NeedResync() const     { return m_bNeedResync; }
    void ClearResyncRequest()   { m_bNeedResync = false; }

protected:
    virtual void Action() = 0;

    cDxr3Interface& m_dxr3Device;
    cDxr3SyncBuffer& m_buffer;
    bool m_bNeedResync;
};

// ==================================
class cDxr3AudioOutThread : public cDxr3OutputThread {
public:
    cDxr3AudioOutThread(cDxr3Interface& dxr3Device, cDxr3SyncBuffer& buffer);

    void setAudio(iAudio *a)    { audioOutput = a; }

protected:
    void Action();

private:
    iAudio *audioOutput;

    void PlayFrame(cFixedLengthFrame *frame);
    void PlayFrame(cDxr3PesFrame *frame);
};

// ==================================
class cDxr3VideoOutThread : public cDxr3OutputThread {
public:
    cDxr3VideoOutThread(cDxr3Interface& dxr3Device, cDxr3SyncBuffer& buffer);

protected:
    void Action();
};

#endif /*_DXR3OUTPUTTHREAD_H_*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
