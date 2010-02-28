/*
 * dxr3output.c
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

#include <time.h>
#include "dxr3output.h"

// ==================================
//! constructor
cDxr3OutputThread::cDxr3OutputThread(cDxr3SyncBuffer& buffer) :
    cThread(),
    m_dxr3Device(cDxr3Interface::instance()),
    m_buffer(buffer),
    m_bNeedResync(false)
{
}

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// c-basic-offset: 4
// indent-tabs-mode: nil
// End:
