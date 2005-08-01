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
	"DXR3",
	"DXR3",
	"", // Slovenski
	"", // Italiano
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"DXR3-toiminnot",
	"", // Polski
	"", // Espa�ol
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"", // Catal�
	"", // ������� (Russian)
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"", // Eesti
#if VDRVERSNUM > 10315
	"", // Dansk
#endif
#endif
    },
    {
	"Hardware MPEG decoder",
	"", // Deutsch
	"", // Slovenski
	"", // Italiano
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"MPEG-purkukortti",
	"", // Polski
	"", // Espa�ol
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"", // Catal�
	"", // ������� (Russian)
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"", // Eesti
#if VDRVERSNUM > 10315
	"", // Dansk
#endif
#endif
    },
    {
	"DXR3 Adjustment",
	"DXR3 Einstellungen",
	"", // Slovenski
	"", // Italiano
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"DXR3-toiminnot",
	"", // Polski
	"", // Espa�ol
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"", // Catal�
	"", // ������� (Russian)
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"", // Eesti
#if VDRVERSNUM > 10315
	"", // Dansk
#endif
#endif
    },
    {
	"Digital audio output",
	"Digitaler Audioausgang",
	"", // Slovenski
	"", // Italiano
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"Digitaalinen audioulostulo",
	"", // Polski
	"", // Espa�ol
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"", // Catal�
	"", // ������� (Russian)
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"", // Eesti
#if VDRVERSNUM > 10315
	"", // Dansk
#endif
#endif
    },
    {
	"Card number",
	"Karte",
	"", // Slovenski
	"", // Italiano
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"Kortin j�rjestysnumero",
	"", // Polski
	"", // Espa�ol
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"", // Catal�
	"", // ������� (Russian)
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"", // Eesti
#if VDRVERSNUM > 10315
	"", // Dansk
#endif
#endif
    },
    {
	"Video mode",
	"Videomodus",
	"", // Slovenski
	"", // Italiano
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"N�ytt�tila",
	"", // Polski
	"", // Espa�ol
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"", // Catal�
	"", // ������� (Russian)
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"", // Eesti
#if VDRVERSNUM > 10315
	"", // Dansk
#endif
#endif
    },
    {
	"PAL",
	"PAL",
	"", // Slovenski
	"", // Italiano
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"PAL",
	"", // Polski
	"", // Espa�ol
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"", // Catal�
	"", // ������� (Russian)
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"", // Eesti
#if VDRVERSNUM > 10315
	"", // Dansk
#endif
#endif
    },
    {
	"PAL60",
	"PAL60",
	"", // Slovenski
	"", // Italiano
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"PAL60",
	"", // Polski
	"", // Espa�ol
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"", // Catal�
	"", // ������� (Russian)
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"", // Eesti
#if VDRVERSNUM > 10315
	"", // Dansk
#endif
#endif
    },
    {
	"NTSC",
	"NTSC",
	"", // Slovenski
	"", // Italiano
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"NTSC",
	"", // Polski
	"", // Espa�ol
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"", // Catal�
	"", // ������� (Russian)
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"", // Eesti
#if VDRVERSNUM > 10315
	"", // Dansk
#endif
#endif
    },
    {
	"Reset DXR3 hardware",
	"Reset DXR3 Hardware",
	"", // Slovenski
	"", // Italiano
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"Palauta kortti alkutilaan",
	"", // Polski
	"", // Espa�ol
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"", // Catal�
	"", // ������� (Russian)
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"", // Eesti
#if VDRVERSNUM > 10315
	"", // Dansk
#endif
#endif
    },
    {
	"Toggle force letterbox",
	"Letterbox erzwingen",
	"", // Slovenski
	"", // Italiano
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"Letterbox-n�ytt�tilan pakotus p��lle/pois",
	"", // Polski
	"", // Espa�ol
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"", // Catal�
	"", // ������� (Russian)
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"", // Eesti
#if VDRVERSNUM > 10315
	"", // Dansk
#endif
#endif
    },
    {
	"Switch to analog audio output",
	"Analoge Ausgabe",
	"", // Slovenski
	"", // Italiano
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"Kytke analoginen audioulostulo",
	"", // Polski
	"", // Espa�ol
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"", // Catal�
	"", // ������� (Russian)
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"", // Eesti
#if VDRVERSNUM > 10315
	"", // Dansk
#endif
#endif
    },
    {
	"AC3 output on",
	"AC3 Ausgabe Ein",
	"", // Slovenski
	"", // Italiano
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"AC3-ulostulo p��lle",
	"", // Polski
	"", // Espa�ol
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"", // Catal�
	"", // ������� (Russian)
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"", // Eesti
#if VDRVERSNUM > 10315
	"", // Dansk
#endif
#endif
    },
    {
	"AC3 output off",
	"AC3 Ausgabe Aus",
	"", // Slovenski
	"", // Italiano
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"AC3-ulostulo pois",
	"", // Polski
	"", // Espa�ol
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"", // Catal�
	"", // ������� (Russian)
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"", // Eesti
#if VDRVERSNUM > 10315
	"", // Dansk
#endif
#endif
    },
    {
	"Switch to digital audio output",
	"Digitaler Ausgang",
	"", // Slovenski
	"", // Italiano
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"Kytke digitaalinen audioulostulo",
	"", // Polski
	"", // Espa�ol
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"", // Catal�
	"", // ������� (Russian)
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"", // Eesti
#if VDRVERSNUM > 10315
	"", // Dansk
#endif
#endif
    },
    {
	"DXR3: releasing devices",
	"DXR3: Releasing Devices",
	"", // Slovenski
	"", // Italiano
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"DXR3: vapautetaan laitteet",
	"", // Polski
	"", // Espa�ol
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"", // Catal�
	"", // ������� (Russian)
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"", // Eesti
#if VDRVERSNUM > 10315
	"", // Dansk
#endif
#endif
    },
    {
	"Color settings",
	"Farbeinstellungen",
	"", // Slovenski
	"", // Italiano
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"V�riasetukset",
	"", // Polski
	"", // Espa�ol
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"", // Catal�
	"", // ������� (Russian)
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"", // Eesti
#if VDRVERSNUM > 10315
	"", // Dansk
#endif
#endif
    },
    {
	"Brightness",
	"Helligkeit",
	"", // Slovenski
	"", // Italiano
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"Kirkkaus",
	"", // Polski
	"", // Espa�ol
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"", // Catal�
	"", // ������� (Russian)
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"", // Eesti
#if VDRVERSNUM > 10315
	"", // Dansk
#endif
#endif
    },
    {
	"Contrast",
	"Kontrast",
	"", // Slovenski
	"", // Italiano
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"Kontrasti",
	"", // Polski
	"", // Espa�ol
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"", // Catal�
	"", // ������� (Russian)
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"", // Eesti
#if VDRVERSNUM > 10315
	"", // Dansk
#endif
#endif
    },
    {
	"Saturation",
	"S�ttigung",
	"", // Slovenski
	"", // Italiano
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"V�rikyll�isyys",
	"", // Polski
	"", // Espa�ol
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"", // Catal�
	"", // ������� (Russian)
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"", // Eesti
#if VDRVERSNUM > 10315
	"", // Dansk
#endif
#endif
    },
    {
	"Hide main menu entry",
	"Hauptmen�eintrag verstecken",
	"", // Slovenski
	"Nascondi voce men�",
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"Piilota valinta p��valikosta",
	"", // Polski
	"", // Espa�ol
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"", // Catal�
	"������ ������� � ������� ����",
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"Peida valik peamen��s",
#if VDRVERSNUM > 10315
	"", // Dansk
#endif
#endif
    },
    {
	"OSD flush rate (ms)",
	"", // Deutsch
	"", // Slovenski
	"", // Italiano
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"Kuvaruutun�yt�n p�ivitysv�li (ms)",
	"", // Polski
	"", // Espa�ol
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"", // Catal�
	"", // ������� (Russian)
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"", // Eesti
#if VDRVERSNUM > 10315
	"", // Dansk
#endif
#endif
    },
    { NULL }
};

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
