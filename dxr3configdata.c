//#include <stdio.h>
#include "dxr3configdata.h"

// ==================================
// constr.
cDxr3ConfigData::cDxr3ConfigData() 
{
	m_digitaloutput = 0;
	m_ac3output = 0;
	m_card = 0;
	m_forceletterbox = 0;
    m_videomode = PAL;
	m_menumode = SUBPICTURE;
	m_debug = 1;
	m_debuglevel = 0;
	m_brightness	= 500;
	m_contrast		= 500;
	m_saturation	= 500;
}
