//#include <stdio.h>
#include "dxr3configdata.h"

// ==================================
// constr.
cDxr3ConfigData::cDxr3ConfigData() 
{
    UseDigitalOut = 0;
    Dxr3Card = 0;
    ForceLetterBox = 0;
    Ac3OutPut = 0;
    m_videoMode = PAL;
	m_menuMode = SUBPICTURE;
	m_debug = 1;
	m_debuglevel = 0;

	m_brightness	= 500;
	m_contrast		= 500;
	m_saturation	= 500;
}
