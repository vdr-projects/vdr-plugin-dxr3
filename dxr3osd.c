/*
 * dxr3osd.c
 *
 * Copyright (C) 2002 Stefan Schluenss
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <linux/em8300.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/unistd.h>

#include "dxr3vdrincludes.h"
#include "dxr3interface.h"

#include "dxr3osd.h"
#include "dxr3interface_spu_encoder.h"

#include "dxr3osd_subpicture.h"

// ==================================
// ! create osd at (Left, Top)
cOsd *cDxr3OsdProvider::CreateOsd(int Left, int Top)
{
    return new cDxr3SubpictureOsd(Left, Top);
}

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
