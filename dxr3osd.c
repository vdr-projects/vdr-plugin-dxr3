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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include <linux/em8300.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/unistd.h>

#include "dxr3vdrincludes.h"
#include "dxr3osd.h"

#if VDRVERSNUM >= 10307

//#include "dxr3osd_mpeg.h"
#include "dxr3osd_subpicture.h"

// ==================================
// ! create osd at (Left, Top)
cOsd *cDxr3OsdProvider::CreateOsd(int Left, int Top)
{
//	if (cDxr3ConfigData::Instance().GetMenuMode() == (eMenuMode)SUBPICTURE)
//	{
		// use subpicture
		return new cDxr3SubpictureOsd(Left, Top);
/*	}
	else
	{
		// mpeg based menu system
		return new cDxr3MpegOsd(Left, Top);
	}*/
}

#else /*VDRVERSNUM*/

// ==================================
bool cDxr3Osd::SetWindow(cWindow *Window) 
{
	if (Window) 
	{
		// Window handles are counted 0...(MAXNUMWINDOWS - 1), but the actual window
		// numbers in the driver are used from 1...MAXNUMWINDOWS.
		int Handle = Window->Handle();
		if (0 <= Handle && Handle < MAXNUMWINDOWS) 
		{
			Spu->Cmd(OSD_SetWindow, 0, Handle + 1);
			return true;
		}
		esyslog("ERROR: illegal window handle: %d", Handle);

		if (cDxr3ConfigData::Instance().GetDebug())
		{
			cLog::Instance() << "cDxr3Osd::SetWindow: illegal window handle:" << Handle << "\n";
		}
	}
	return false;
}

// ==================================
cDxr3Osd::cDxr3Osd(int x, int y) : cOsdBase(x, y)
{    
    Spu = &cSPUEncoder::Instance();
}

// ==================================
cDxr3Osd::~cDxr3Osd()
{
  for (int i = 0; i < NumWindows(); i++)
  {
      CloseWindow(GetWindowNr(i));
  }
}

// ==================================
bool cDxr3Osd::OpenWindow(cWindow *Window)
{
	if (SetWindow(Window)) 
	{
		Spu->Cmd(OSD_Open, Window->Bpp(), X0() + Window->X0(), Y0() + Window->Y0(), X0() + Window->X0() + Window->Width() - 1, Y0() + Window->Y0() + Window->Height() - 1, (void *)1); // initially hidden!
		return true;
	}
	return false;
}

// ==================================
void cDxr3Osd::CommitWindow(cWindow *Window)
{
	if (SetWindow(Window)) 
	{
		int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
		
		if (Window->Dirty(x1, y1, x2, y2)) 
		{
			// commit colors:
			int FirstColor = 0, LastColor = 0;
			const eDvbColor *pal;
			while ((pal = Window->NewColors(FirstColor, LastColor)) != NULL)
				Spu->Cmd(OSD_SetPalette, FirstColor, LastColor, 0, 0, 0, pal);
			// commit modified data:
			Spu->Cmd(OSD_SetBlock, Window->Width(), x1, y1, x2, y2, Window->Data(x1, y1));
        }
	}
}

// ==================================
void cDxr3Osd::ShowWindow(cWindow *Window)
{
	if (SetWindow(Window))
	{
		Spu->Cmd(OSD_MoveWindow, 0, X0() + Window->X0(), Y0() + Window->Y0());
	}
}

// ==================================
void cDxr3Osd::HideWindow(cWindow *Window, bool Hide)
{
	if (SetWindow(Window))
	{
		Spu->Cmd(Hide ? OSD_Hide : OSD_Show, 0);
	}
}

// ==================================
void cDxr3Osd::CloseWindow(cWindow *Window)
{
	if (SetWindow(Window))
	{
		Spu->Cmd(OSD_Close);
	}
}

// ==================================
void cDxr3Osd::MoveWindow(cWindow *Window, int x, int y)
{
	if (SetWindow(Window))
	{
		Spu->Cmd(OSD_MoveWindow, 0, X0() + x, Y0() + y);
	}
}

#endif /*VDRVERSNUM*/
