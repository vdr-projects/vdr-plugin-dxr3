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
	"DXR3",
	"DXR3",
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"DXR3",
	"", // ������� (Russian)
	"", // Hrvatski
	"", // Eesti
	"", // Dansk
	"", // Czech
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
	"Sprz�towy dekoder MPEG",
	"Descodificador MPEG Hardware",
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"Descodificador MPEG Hardware",
	"", // ������� (Russian)
	"", // Hrvatski
	"", // Eesti
	"", // Dansk
	"", // Czech
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
	"Ustawienia DXR3",
	"Opciones DXR3",
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"Opcions DXR3",
	"", // ������� (Russian)
	"", // Hrvatski
	"", // Eesti
	"", // Dansk
	"", // Czech
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
	"Cyfrowe wyj�cie d�wi�ku",
	"Salida audio digital",
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"Sortida �udio digital",
	"", // ������� (Russian)
	"", // Hrvatski
	"", // Eesti
	"", // Dansk
	"", // Czech
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
	"Numer karty",
	"Tarjeta",
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"Targeta",
	"", // ������� (Russian)
	"", // Hrvatski
	"", // Eesti
	"", // Dansk
	"", // Czech
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
	"Tryb obrazu",
	"Modalidad v�deo",
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"Modalitat v�deo",
	"", // ������� (Russian)
	"", // Hrvatski
	"", // Eesti
	"", // Dansk
	"", // Czech
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
	"PAL",
	"PAL",
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"PAL",
	"", // ������� (Russian)
	"", // Hrvatski
	"", // Eesti
	"", // Dansk
	"", // Czech
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
	"PAL60",
	"PAL60",
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"PAL60",
	"", // ������� (Russian)
	"", // Hrvatski
	"", // Eesti
	"", // Dansk
	"", // Czech
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
	"NTSC",
	"NTSC",
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"NTSC",
	"", // ������� (Russian)
	"", // Hrvatski
	"", // Eesti
	"", // Dansk
	"", // Czech
    },
    {
	"Use widescreen signaling (WSS)",
	"", // Deutsch
	"", // Slovenski
	"", // Italiano
	"", // Nederlands
	"", // Portugu�s
	"", // Fran�ais
	"", // Norsk
	"K�yt� WSS-signalointia",
	"", // Polski
	"", // Espa�ol
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"", // Catal�
	"", // ������� (Russian)
	"", // Hrvatski
	"", // Eesti
	"", // Dansk
	"", // Czech
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
	"Zerowanie dekodera DXR3",
	"Reset hardware DXR3",
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"Reset hardware DXR3",
	"", // ������� (Russian)
	"", // Hrvatski
	"", // Eesti
	"", // Dansk
	"", // Czech
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
	"Prze��cz wymuszanie trybu letterbox",
	"Conmuta modalidad letterbox forzada",
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"Commuta modalitat letterbox for�ada",
	"", // ������� (Russian)
	"", // Hrvatski
	"", // Eesti
	"", // Dansk
	"", // Czech
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
	"Prze��cz w tryb d�wi�ku analogowego",
	"Selecciona salida audio anal�gica",
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"Selecciona sortida �udio anal�gica",
	"", // ������� (Russian)
	"", // Hrvatski
	"", // Eesti
	"", // Dansk
	"", // Czech
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
	"Prze��cz w tryb d�wi�ku cyfrowego",
	"Selecciona salida audio digital",
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"Selecciona sortida �udio digital",
	"", // ������� (Russian)
	"", // Hrvatski
	"", // Eesti
	"", // Dansk
	"", // Czech
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
	"DXR3: zwalnianie sprz�tu",
	"DXR3: liberando dispositivo",
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"DXR3: alliberant dispositiu",
	"", // ������� (Russian)
	"", // Hrvatski
	"", // Eesti
	"", // Dansk
	"", // Czech
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
	"Ustawienia koloru",
	"Ajustes color",
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"Ajustaments color",
	"", // ������� (Russian)
	"", // Hrvatski
	"", // Eesti
	"", // Dansk
	"", // Czech
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
	"Jasno��",
	"Luminosidad",
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"Lluminositat",
	"", // ������� (Russian)
	"", // Hrvatski
	"", // Eesti
	"", // Dansk
	"", // Czech
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
	"Kontrast",
	"Contraste",
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"Contrast",
	"", // ������� (Russian)
	"", // Hrvatski
	"", // Eesti
	"", // Dansk
	"", // Czech
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
	"Nasycenie",
	"Saturaci�n",
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"Saturaci�",
	"", // ������� (Russian)
	"", // Hrvatski
	"", // Eesti
	"", // Dansk
	"", // Czech
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
	"Ukryj g��wny wpis w menu",
	"Esconde entrada men�",
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"Amaga entrada men�",
	"������ ������� � ������� ����",
	"", // Hrvatski
	"Peida valik peamen��s",
	"", // Dansk
	"", // Czech
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
	"Czas od�wie�ania OSD (ms)",
	"Tiempo m�nimo refresco OSD (ms)",
	"", // �������� (Greek)
	"", // Svenska
	"", // Rom�n�
	"", // Magyar
	"Temps m�nim refresc OSD (ms)",
	"", // ������� (Russian)
	"", // Hrvatski
	"", // Eesti
	"", // Dansk
	"", // Czech
    },
    { NULL }
};

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// c-file-offsets: ((inline-open . 0))
// indent-tabs-mode: t
// End:
