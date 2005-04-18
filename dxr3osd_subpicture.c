#include "dxr3osd_subpicture.h"

// Enables some time measure debugging code
// (taken from the osdteletext plugin, thanks folks)
#ifdef timingdebug
    #include <sys/timeb.h>
    
    class cTime {
        // Debugging: Simple class to measure time
        timeb start;
    public:
        void Start() {
            ftime(&start);
        }
        void Stop(char *txt) {
            timeb t;
            ftime(&t);
            int s=t.time-start.time;
            int ms=t.millitm-start.millitm;
            if (ms<0) {
                s--;
                ms+=1000;
            }
            printf("%s: %i.%03i\n",txt,s,ms);
        }
    };
#endif

#if VDRVERSNUM >= 10307

#define MAXNUMWINDOWS 7 // OSD windows are counted 1...7

// ==================================
//! constructor
cDxr3SubpictureOsd::cDxr3SubpictureOsd(int Left, int Top) : cOsd(Left, Top)
{
	shown = false;
	Palette = new cPalette(4);
#if VDRVERSNUM >= 10318
	last = new cTimeMs();
	last->Set(-FLUSHRATE);
#else
	last = time_ms() - FLUSHRATE;
#endif
	Spu = &cSPUEncoder::Instance();
	
	//Clears the OSD screen image
	Spu->Clear();
}

// ==================================
cDxr3SubpictureOsd::~cDxr3SubpictureOsd()
{
	//Remove the OSD from the screen
	Spu->StopSpu();
	delete Palette;
#if VDRVERSNUM >= 10318
	delete last;
#endif
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
			if (Areas[i].bpp != 1 && Areas[i].bpp != 2 && Areas[i].bpp != 4 && Areas[i].bpp != 8)
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
#if VDRVERSNUM >= 10318
	if (last->Elapsed()<FLUSHRATE) return;
	last->Set();
#else
	if (time_ms()-last<FLUSHRATE) return;
	last = time_ms();
#endif

        #ifdef timingdebug
          cTime t;
          t.Start();
        #endif

	cBitmap *Bitmap;
	int oldi;
	int newi;
	int i;
	int indexfree[16];
	int firstfree=-1;
	int indexnoassigned[16];
	int firstnoassigned=-1;
	bool colfree[16];
	int NumNewColors;
	int NumOldColors;
	
	//determine the palette used by all bitmaps (without alpha channel)
	
	cPalette *newPalette=new cPalette(4);
	for (i=0; i<16; i++) colfree[i]=true;
	for (i=0; (Bitmap = GetBitmap(i)) != NULL; i++) {
	  int nc;
	  const tColor *col=Bitmap->Colors(nc);
	  if (col) for (int kk=0; kk<nc; kk++) newPalette->Index(col[kk] & 0x00FFFFFF);
	}  
	const tColor *newColors=newPalette->Colors(NumNewColors);
	const tColor *oldColors=Palette->Colors(NumOldColors);
	//colors already assigned
	for (newi=0;newi<NumNewColors;newi++) {
	  for(oldi=0;oldi<NumOldColors;oldi++) {
	    if (newColors[newi]==oldColors[oldi]) {
	      colfree[oldi]=false;
	      break; 
	    }
	  }  
          if (oldi>=NumOldColors) {
            firstnoassigned++;
            indexnoassigned[firstnoassigned]=newi;
	  }
	}
	//unused colors
	for (i=0; i<NumOldColors; i++) {
	  if(colfree[i]) {
	    firstfree++;
	    indexfree[firstfree]=i;
	  }
	}
	//replace unused colors with unassigned ones
	for (i=0; i<=firstnoassigned; i++) {
	  newi=indexnoassigned[i];
	  if (firstfree>=0) {
	    Palette->SetColor(indexfree[firstfree], newColors[newi]);
	    firstfree--;
	  } else {
	    Palette->Index(newColors[newi]);
	  }
	}
	delete newPalette;
	
	//Shove the bitmaps to the OSD global bitmap
	for (int i = 0; (Bitmap = GetBitmap(i)) != NULL; i++) 
	{
		int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
		if (Bitmap->Dirty(x1, y1, x2, y2)) 
		{
			//TODO Workaround: apparently the bitmap sent to the driver always has to be a multiple
			//TODO of 8 bits wide, and (dx * dy) also has to be a multiple of 8.
			//TODO Fix driver (should be able to handle any size bitmaps!)
			//
			// This isn't actually necessary with this plugin, but since other plugins rely
			// on this behaviour to work correctly, I left it here. It doesn't hurt too much.
			
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
			
			Spu->SetPalette(i+1,Palette,Bitmap);
			int origx=Left()+Bitmap->X0();
			int origy=Top()+Bitmap->Y0();
			Spu->CopyBlockIntoOSD(i+1, Bitmap->Width(), origx+x1, origy+y1, origx+x2, origy+y2, Bitmap->Data(x1, y1));
    		        Bitmap->Clean();
		}
	}
	
	Spu->Flush(Palette);
	shown = true;
        #ifdef timingdebug
          t.Stop("cDxr3SubpictureOsd::Flush");
        #endif
}

#endif /*VDRVERSNUM*/

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// indent-tabs-mode: t
// End:
