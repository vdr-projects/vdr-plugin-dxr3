/*
 * dxr3i18n.c
 *
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

#include "dxr3i18n.h"

const char *i18n_name = 0;

const tI18nPhrase Phrases[] = {
	{
		"DXR3", // English
		"DXR3", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"", // Fran�ais
		"", // Norsk
		"DXR3", // suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
#if VDRVERSNUM > 10302
		"", // Russian
# if VDRVERSNUM > 10307
		"", // Croatian
# endif
#endif
	},
	{
		"DXR3 Adjustment", // English
		"DXR3 Einstellungen", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"", // Fran�ais
		"", // Norsk
		"DXR3-s��timet", // suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
#if VDRVERSNUM > 10302
		"", // Russian
# if VDRVERSNUM > 10307
		"", // Croatian
# endif
#endif
	},
	{
		"Digital audio output", // English
		"Digitaler Audioausgang", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"", // Fran�ais
		"", // Norsk
		"Digitaalinen audio-ulostulo", // suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
#if VDRVERSNUM > 10302
		"", // Russian
# if VDRVERSNUM > 10307
		"", // Croatian
# endif
#endif
	},
	{
		"DXR3 card", // English
		"DXR3 Karte", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"", // Fran�ais
		"", // Norsk
		"DXR3-kortti", // suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
#if VDRVERSNUM > 10302
		"", // Russian
# if VDRVERSNUM > 10307
		"", // Croatian
# endif
#endif
	},
	{
		"DXR3 video mode", // English
		"DXR3 Videomodus", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"", // Fran�ais
		"", // Norsk
		"DXR3-n�ytt�tila", // suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
#if VDRVERSNUM > 10302
		"", // Russian
# if VDRVERSNUM > 10307
		"", // Croatian
# endif
#endif
	},
	{
		"Reset DXR3 hardware", // English
		"Reset DXR3 Hardware", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"", // Fran�ais
		"", // Norsk
		"Alusta DXR3-laitteisto", // suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
#if VDRVERSNUM > 10302
		"", // Russian
# if VDRVERSNUM > 10307
		"", // Croatian
# endif
#endif
	},
	{
		"Toggle Force Letterbox", // English
		"Letterbox erzwingen", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"", // Fran�ais
		"", // Norsk
		"Vaihda letterbox-n�ytt�tilan pakotus p��lle/pois", // suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
#if VDRVERSNUM > 10302
		"", // Russian
# if VDRVERSNUM > 10307
		"", // Croatian
# endif
#endif
	},
	{
		"Analog Output", // English
		"Analoge Ausgabe", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"", // Fran�ais
		"", // Norsk
		"Kytke analoginen audio-ulostulo", // suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
#if VDRVERSNUM > 10302
		"", // Russian
# if VDRVERSNUM > 10307
		"", // Croatian
# endif
#endif
	},
	{
		"AC3 output on", // English
		"AC3 Ausgabe Ein", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"", // Fran�ais
		"", // Norsk
		"AC3-ulostulo p��ll�", // suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
#if VDRVERSNUM > 10302
		"", // Russian
# if VDRVERSNUM > 10307
		"", // Croatian
# endif
#endif
	},
	{
		"AC3 output off", // English
		"AC3 Ausgabe Aus", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"", // Fran�ais
		"", // Norsk
		"AC3-ulostulo pois", // suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
#if VDRVERSNUM > 10302
		"", // Russian
# if VDRVERSNUM > 10307
		"", // Croatian
# endif
#endif
	},
	{
		"Digital output", // English
		"Digitaler Ausgang", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"", // Fran�ais
		"", // Norsk
		"Kytke digitaalinen audio-ulostulo", // suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
#if VDRVERSNUM > 10302
		"", // Russian
# if VDRVERSNUM > 10307
		"", // Croatian
# endif
#endif
	},
	{
		"DXR3: Releasing devices", // English
		"DXR3: Releasing Devices", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"", // Fran�ais
		"", // Norsk
		"DXR3: vapautetaan laitteet", // suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
#if VDRVERSNUM > 10302
		"", // Russian
# if VDRVERSNUM > 10307
		"", // Croatian
# endif
#endif
	},
	{
		"Color settings", // English
		"Farbeinstellungen", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"", // Fran�ais
		"", // Norsk
		"V�riasetukset", // suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
#if VDRVERSNUM > 10302
		"", // Russian
# if VDRVERSNUM > 10307
		"", // Croatian
# endif
#endif
	},
	{
		"Brightness", // English
		"Helligkeit", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"", // Fran�ais
		"", // Norsk
		"Kirkkaus", // suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
#if VDRVERSNUM > 10302
		"", // Russian
# if VDRVERSNUM > 10307
		"", // Croatian
# endif
#endif
	},
	{
		"Contrast", // English
		"Kontrast", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"", // Fran�ais
		"", // Norsk
		"Kontrasti", // suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
#if VDRVERSNUM > 10302
		"", // Russian
# if VDRVERSNUM > 10307
		"", // Croatian
# endif
#endif
	},
	{
		"Saturation", // English
		"S�ttigung", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"", // Fran�ais
		"", // Norsk
		"V�rikyll�isyys", // suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
#if VDRVERSNUM > 10302
		"", // Russian
# if VDRVERSNUM > 10307
		"", // Croatian
# endif
#endif
	},
	{
		"Debug mode", // English
		"", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"", // Fran�ais
		"", // Norsk
		"Debug-tila", // suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
#if VDRVERSNUM > 10302
		"", // Russian
# if VDRVERSNUM > 10307
		"", // Croatian
# endif
#endif
	},
	{
		"Debug level", // English
		"", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Portugu�s
		"", // Fran�ais
		"", // Norsk
		"Debug-taso", // suomi
		"", // Polski
		"", // Espa�ol
		"", // Ellinika
		"", // Svenska
		"", // Romaneste
		"", // Magyar
		"", // Catala
#if VDRVERSNUM > 10302
		"", // Russian
# if VDRVERSNUM > 10307
		"", // Croatian
# endif
#endif
	},
	{ NULL }
};
