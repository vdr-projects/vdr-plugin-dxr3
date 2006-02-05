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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "dxr3i18n.h"

const char *i18n_name = 0;

const tI18nPhrase Phrases[] = {
    {
	"DXR3",
	"DXR3",
	"", // Slovenski
	"DXR3",
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"DXR3-toiminnot",
	"", // Polski
	"DXR3",
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"DXR3",
	"", // ������� (Russian)
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"", // Eesti
#if VDRVERSNUM > 10315
	"", // Dansk
#if VDRVERSNUM > 10341
	"", // Czech
#endif
#endif
#endif
    },
    {
	"Hardware MPEG decoder",
	"", // Deutsch
	"", // Slovenski
	"Decoder MPEG Hardware",
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"MPEG-purkukortti",
	"", // Polski
	"Descodificador MPEG Hardware",
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"Descodificador MPEG Hardware",
	"", // ������� (Russian)
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"", // Eesti
#if VDRVERSNUM > 10315
	"", // Dansk
#if VDRVERSNUM > 10341
	"", // Czech
#endif
#endif
#endif
    },
    {
	"DXR3 Adjustment",
	"DXR3 Einstellungen",
	"", // Slovenski
	"Opzioni DXR3",
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"DXR3-toiminnot",
	"", // Polski
	"Opciones DXR3",
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"Opcions DXR3",
	"", // ������� (Russian)
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"", // Eesti
#if VDRVERSNUM > 10315
	"", // Dansk
#if VDRVERSNUM > 10341
	"", // Czech
#endif
#endif
#endif
    },
    {
	"Digital audio output",
	"Digitaler Audioausgang",
	"", // Slovenski
	"Uscita audio digitale",
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"Digitaalinen audioulostulo",
	"", // Polski
	"Salida audio digital",
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"Sortida �udio digital",
	"", // ������� (Russian)
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"", // Eesti
#if VDRVERSNUM > 10315
	"", // Dansk
#if VDRVERSNUM > 10341
	"", // Czech
#endif
#endif
#endif
    },
    {
	"Card number",
	"Karte",
	"", // Slovenski
	"Scheda",
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"Kortin j�rjestysnumero",
	"", // Polski
	"Tarjeta",
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"Targeta",
	"", // ������� (Russian)
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"", // Eesti
#if VDRVERSNUM > 10315
	"", // Dansk
#if VDRVERSNUM > 10341
	"", // Czech
#endif
#endif
#endif
    },
    {
	"Video mode",
	"Videomodus",
	"", // Slovenski
	"Modo Video",
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"N�ytt�tila",
	"", // Polski
	"Modalidad v�deo",
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"Modalitat v�deo",
	"", // ������� (Russian)
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"", // Eesti
#if VDRVERSNUM > 10315
	"", // Dansk
#if VDRVERSNUM > 10341
	"", // Czech
#endif
#endif
#endif
    },
    {
	"PAL",
	"PAL",
	"", // Slovenski
	"PAL",
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"PAL",
	"", // Polski
	"PAL",
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"PAL",
	"", // ������� (Russian)
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"", // Eesti
#if VDRVERSNUM > 10315
	"", // Dansk
#if VDRVERSNUM > 10341
	"", // Czech
#endif
#endif
#endif
    },
    {
	"PAL60",
	"PAL60",
	"", // Slovenski
	"PAL60",
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"PAL60",
	"", // Polski
	"PAL60",
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"PAL60",
	"", // ������� (Russian)
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"", // Eesti
#if VDRVERSNUM > 10315
	"", // Dansk
#if VDRVERSNUM > 10341
	"", // Czech
#endif
#endif
#endif
    },
    {
	"NTSC",
	"NTSC",
	"", // Slovenski
	"NTSC",
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"NTSC",
	"", // Polski
	"NTSC",
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"NTSC",
	"", // ������� (Russian)
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"", // Eesti
#if VDRVERSNUM > 10315
	"", // Dansk
#if VDRVERSNUM > 10341
	"", // Czech
#endif
#endif
#endif
    },
    {
	"Reset DXR3 hardware",
	"Reset DXR3 Hardware",
	"", // Slovenski
	"Reset hardware DXR3",
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"Palauta kortti alkutilaan",
	"", // Polski
	"Reset hardware DXR3",
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"Reset hardware DXR3",
	"", // ������� (Russian)
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"", // Eesti
#if VDRVERSNUM > 10315
	"", // Dansk
#if VDRVERSNUM > 10341
	"", // Czech
#endif
#endif
#endif
    },
    {
	"Toggle force letterbox",
	"Letterbox erzwingen",
	"", // Slovenski
	"Commuta modo letterbox forzato ",
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"Letterbox-n�ytt�tilan pakotus p��lle/pois",
	"", // Polski
	"Conmuta modalidad letterbox forzada",
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"Commuta modalitat letterbox for�ada",
	"", // ������� (Russian)
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"", // Eesti
#if VDRVERSNUM > 10315
	"", // Dansk
#if VDRVERSNUM > 10341
	"", // Czech
#endif
#endif
#endif
    },
    {
	"Switch to analog audio output",
	"Analoge Ausgabe",
	"", // Slovenski
	"Seleziona uscita audio analogica",
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"Kytke analoginen audioulostulo",
	"", // Polski
	"Selecciona salida audio anal�gica",
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"Selecciona sortida �udio anal�gica",
	"", // ������� (Russian)
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"", // Eesti
#if VDRVERSNUM > 10315
	"", // Dansk
#if VDRVERSNUM > 10341
	"", // Czech
#endif
#endif
#endif
    },
    {
	"AC3 output on",
	"AC3 Ausgabe Ein",
	"", // Slovenski
	"Attiva uscita AC3",
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"AC3-ulostulo p��lle",
	"", // Polski
	"Activa salida AC3",
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"Activa sortida AC3",
	"", // ������� (Russian)
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"", // Eesti
#if VDRVERSNUM > 10315
	"", // Dansk
#if VDRVERSNUM > 10341
	"", // Czech
#endif
#endif
#endif
    },
    {
	"AC3 output off",
	"AC3 Ausgabe Aus",
	"", // Slovenski
	"Disattiva uscita AC3",
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"AC3-ulostulo pois",
	"", // Polski
	"Desactiva salida AC3",
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"Desactiva sortida AC3",
	"", // ������� (Russian)
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"", // Eesti
#if VDRVERSNUM > 10315
	"", // Dansk
#if VDRVERSNUM > 10341
	"", // Czech
#endif
#endif
#endif
    },
    {
	"Switch to digital audio output",
	"Digitaler Ausgang",
	"", // Slovenski
	"Seleziona uscita audio digitale",
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"Kytke digitaalinen audioulostulo",
	"", // Polski
	"Selecciona salida audio digital",
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"Selecciona sortida �udio digital",
	"", // ������� (Russian)
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"", // Eesti
#if VDRVERSNUM > 10315
	"", // Dansk
#if VDRVERSNUM > 10341
	"", // Czech
#endif
#endif
#endif
    },
    {
	"DXR3: releasing devices",
	"DXR3: Releasing Devices",
	"", // Slovenski
	"DXR3: liberando dispositivo",
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"DXR3: vapautetaan laitteet",
	"", // Polski
	"DXR3: liberando dispositivo",
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"DXR3: alliberant dispositiu",
	"", // ������� (Russian)
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"", // Eesti
#if VDRVERSNUM > 10315
	"", // Dansk
#if VDRVERSNUM > 10341
	"", // Czech
#endif
#endif
#endif
    },
    {
	"Color settings",
	"Farbeinstellungen",
	"", // Slovenski
	"Regolazioni colore",
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"V�riasetukset",
	"", // Polski
	"Ajustes color",
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"Ajustaments color",
	"", // ������� (Russian)
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"", // Eesti
#if VDRVERSNUM > 10315
	"", // Dansk
#if VDRVERSNUM > 10341
	"", // Czech
#endif
#endif
#endif
    },
    {
	"Brightness",
	"Helligkeit",
	"", // Slovenski
	"Luminosit�",
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"Kirkkaus",
	"", // Polski
	"Luminosidad",
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"Lluminositat",
	"", // ������� (Russian)
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"", // Eesti
#if VDRVERSNUM > 10315
	"", // Dansk
#if VDRVERSNUM > 10341
	"", // Czech
#endif
#endif
#endif
    },
    {
	"Contrast",
	"Kontrast",
	"", // Slovenski
	"Contrasto",
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"Kontrasti",
	"", // Polski
	"Contraste",
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"Contrast",
	"", // ������� (Russian)
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"", // Eesti
#if VDRVERSNUM > 10315
	"", // Dansk
#if VDRVERSNUM > 10341
	"", // Czech
#endif
#endif
#endif
    },
    {
	"Saturation",
	"S�ttigung",
	"", // Slovenski
	"Saturazione",
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"V�rikyll�isyys",
	"", // Polski
	"Saturaci�n",
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"Saturaci�",
	"", // ������� (Russian)
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"", // Eesti
#if VDRVERSNUM > 10315
	"", // Dansk
#if VDRVERSNUM > 10341
	"", // Czech
#endif
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
	"Esconde entrada men�",
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"Amaga entrada men�",
	"������ ������� � ������� ����",
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"Peida valik peamen��s",
#if VDRVERSNUM > 10315
	"", // Dansk
#if VDRVERSNUM > 10341
	"", // Czech
#endif
#endif
#endif
    },
    {
	"OSD flush rate (ms)",
	"", // Deutsch
	"", // Slovenski
	"Tempo minimo rinfresco OSD (ms)",
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"Kuvaruutun�yt�n p�ivitysv�li (ms)",
	"", // Polski
	"Tiempo m�nimo refresco OSD (ms)",
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"Temps m�nim refresc OSD (ms)",
	"", // ������� (Russian)
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"", // Eesti
#if VDRVERSNUM > 10315
	"", // Dansk
#if VDRVERSNUM > 10341
	"", // Czech
#endif
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
