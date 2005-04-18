#include "dxr3osd_subpicture.h"

#if VDRVERSNUM >= 10307

#define MAXNUMWINDOWS 7 // OSD windows are counted 1...7

// ==================================
//! constructor
cDxr3SubpictureOsd::cDxr3SubpictureOsd(int Left, int Top) : cOsd(Left, Top)
{
	shown = false;
	Spu = &cSPUEncoder::Instance();

	// must clear all windows here to avoid flashing effects - doesn't work if done
	// in Flush() only for the windows that are actually used...
	for (int i = 0; i < MAXNUMWINDOWS; i++) 
	{
		Spu->Cmd(OSD_SetWindow, 0, i + 1);
		Spu->Cmd(OSD_Clear);
	}

}

// ==================================
cDxr3SubpictureOsd::~cDxr3SubpictureOsd()
{
	if (shown) 
	{
		cBitmap *Bitmap;
		for (int i = 0; (Bitmap = GetBitmap(i)) != NULL; i++) 
		{
			Spu->Cmd(OSD_SetWindow, 0, i + 1);
			Spu->Cmd(OSD_Close);
		}
	}
}

// ==================================
eOsdError cDxr3SubpictureOsd::CanHandleAreas(const tArea *Areas, int NumAreas)
{

	eOsdError Result = cOsd::CanHandleAreas(Areas, NumAreas);
	if (Result == oeOk) 
	{
		if (NumAreas > MAXNUMWINDOWS)
		{
			return oeTooManyAreas;
		}
		
		for (int i = 0; i < NumAreas; i++) 
		{
			// at the moment we dont support 256 color palette
			if (Areas[i].bpp != 1 && Areas[i].bpp != 2 && Areas[i].bpp != 4/* && Areas[i].bpp != 8*/)
			{
				return oeBppNotSupported;
			}
			
			if ((Areas[i].Width() & (8 / Areas[i].bpp - 1)) != 0)
			{
				return oeWrongAlignment;
			}
		}
	}
	return Result;
}

// ==================================
void cDxr3SubpictureOsd::SaveRegion(int x1, int y1, int x2, int y2)
{
	// ToDo?
}

// ==================================
void cDxr3SubpictureOsd::RestoreRegion()
{
	// ToDo?
}

// ==================================
void cDxr3SubpictureOsd::Flush()
{
	cBitmap *Bitmap;
	
	for (int i = 0; (Bitmap = GetBitmap(i)) != NULL; i++) 
	{
		Spu->Cmd(OSD_SetWindow, 0, i + 1);
		
		if (!shown)
		{
			Spu->Cmd(OSD_Open, Bitmap->Bpp(), Left() + Bitmap->X0(), Top() + Bitmap->Y0(), Left() + Bitmap->X0() + Bitmap->Width() - 1, Top() + Bitmap->Y0() + Bitmap->Height() - 1, (void *)1); // initially hidden!
		}
		
		int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
		if (Bitmap->Dirty(x1, y1, x2, y2)) 
		{
			//TODO Workaround: apparently the bitmap sent to the driver always has to be a multiple
			//TODO of 8 bits wide, and (dx * dy) also has to be a multiple of 8.
			//TODO Fix driver (should be able to handle any size bitmaps!)
			
			while ((x1 > 0 || x2 < Bitmap->Width() - 1) && ((x2 - x1) & 7) != 7) 
			{
				if (x2 < Bitmap->Width() - 1)
				{
					x2++;
				}
				else if (x1 > 0)
				{
					x1--;
				}
			}
			
			//TODO "... / 2" <==> Bpp???
			while ((y1 > 0 || y2 < Bitmap->Height() - 1) && (((x2 - x1 + 1) * (y2 - y1 + 1) / 2) & 7) != 0) 
			{
				if (y2 < Bitmap->Height() - 1)
				{
					y2++;
				}
				else if (y1 > 0)
				{
					y1--;
				}
			}
			
			while ((x1 > 0 || x2 < Bitmap->Width() - 1) && (((x2 - x1 + 1) * (y2 - y1 + 1) / 2) & 7) != 0) 
			{
				if (x2 < Bitmap->Width() - 1)
				{
					x2++;
				}
				else if (x1 > 0)
				{
					x1--;
				}
			}
			
			// commit colors:
			int NumColors;
			const tColor *Colors = Bitmap->Colors(NumColors);
			
			if (Colors) 
			{

				// TODO this should be fixed in the driver!
				tColor colors[NumColors];
				for (int i = 0; i < NumColors; i++) 
				{
					// convert AARRGGBB to AABBGGRR (the driver expects the colors the wrong way):
					colors[i] = (Colors[i] & 0xFF000000) | ((Colors[i] & 0x0000FF) << 16) | (Colors[i] & 0x00FF00) | ((Colors[i] & 0xFF0000) >> 16);
				}
				
				Colors = colors;
				//TODO end of stuff that should be fixed in the driver
				Spu->Cmd(OSD_SetPalette, 0, NumColors - 1, 0, 0, 0, Colors);
			}
			// commit modified data:
			//Spu->Cmd(OSD_SetBlock, Bitmap->Width(), x1, y1, x2, y2, Bitmap->Data(x1, y1));
			Spu->Cmd(OSD_SetBlock, Bitmap->Width(), x1, y1, x2, y2, Bitmap->Data(x1, y1));
		}
		Bitmap->Clean();
	}
	
	if (!shown) 
	{
		// Showing the windows in a separate loop to avoid seeing them come up one after another
		for (int i = 0; (Bitmap = GetBitmap(i)) != NULL; i++) 
		{
			Spu->Cmd(OSD_SetWindow, 0, i + 1);
			Spu->Cmd(OSD_MoveWindow, 0, Left() + Bitmap->X0(), Top() + Bitmap->Y0());
		}
		shown = true;
	}
}

#endif /*VDRVERSNUM*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
