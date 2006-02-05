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
	"", // Português
	"", // Français
	"", // Norsk
	"DXR3-toiminnot",
	"", // Polski
	"DXR3",
	"", // ÅëëçíéêÜ (Greek)
	"", // Svenska
	"", // Românã
	"", // Magyar
	"DXR3",
	"", // ÀãááÚØÙ (Russian)
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
	"", // Português
	"", // Français
	"", // Norsk
	"MPEG-purkukortti",
	"", // Polski
	"Descodificador MPEG Hardware",
	"", // ÅëëçíéêÜ (Greek)
	"", // Svenska
	"", // Românã
	"", // Magyar
	"Descodificador MPEG Hardware",
	"", // ÀãááÚØÙ (Russian)
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
	"", // Português
	"", // Français
	"", // Norsk
	"DXR3-toiminnot",
	"", // Polski
	"Opciones DXR3",
	"", // ÅëëçíéêÜ (Greek)
	"", // Svenska
	"", // Românã
	"", // Magyar
	"Opcions DXR3",
	"", // ÀãááÚØÙ (Russian)
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
	"", // Português
	"", // Français
	"", // Norsk
	"Digitaalinen audioulostulo",
	"", // Polski
	"Salida audio digital",
	"", // ÅëëçíéêÜ (Greek)
	"", // Svenska
	"", // Românã
	"", // Magyar
	"Sortida àudio digital",
	"", // ÀãááÚØÙ (Russian)
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
	"", // Português
	"", // Français
	"", // Norsk
	"Kortin järjestysnumero",
	"", // Polski
	"Tarjeta",
	"", // ÅëëçíéêÜ (Greek)
	"", // Svenska
	"", // Românã
	"", // Magyar
	"Targeta",
	"", // ÀãááÚØÙ (Russian)
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
	"", // Português
	"", // Français
	"", // Norsk
	"Näyttötila",
	"", // Polski
	"Modalidad vídeo",
	"", // ÅëëçíéêÜ (Greek)
	"", // Svenska
	"", // Românã
	"", // Magyar
	"Modalitat vídeo",
	"", // ÀãááÚØÙ (Russian)
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
	"", // Português
	"", // Français
	"", // Norsk
	"PAL",
	"", // Polski
	"PAL",
	"", // ÅëëçíéêÜ (Greek)
	"", // Svenska
	"", // Românã
	"", // Magyar
	"PAL",
	"", // ÀãááÚØÙ (Russian)
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
	"", // Português
	"", // Français
	"", // Norsk
	"PAL60",
	"", // Polski
	"PAL60",
	"", // ÅëëçíéêÜ (Greek)
	"", // Svenska
	"", // Românã
	"", // Magyar
	"PAL60",
	"", // ÀãááÚØÙ (Russian)
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
	"", // Português
	"", // Français
	"", // Norsk
	"NTSC",
	"", // Polski
	"NTSC",
	"", // ÅëëçíéêÜ (Greek)
	"", // Svenska
	"", // Românã
	"", // Magyar
	"NTSC",
	"", // ÀãááÚØÙ (Russian)
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
	"", // Português
	"", // Français
	"", // Norsk
	"Palauta kortti alkutilaan",
	"", // Polski
	"Reset hardware DXR3",
	"", // ÅëëçíéêÜ (Greek)
	"", // Svenska
	"", // Românã
	"", // Magyar
	"Reset hardware DXR3",
	"", // ÀãááÚØÙ (Russian)
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
	"", // Português
	"", // Français
	"", // Norsk
	"Letterbox-näyttötilan pakotus päälle/pois",
	"", // Polski
	"Conmuta modalidad letterbox forzada",
	"", // ÅëëçíéêÜ (Greek)
	"", // Svenska
	"", // Românã
	"", // Magyar
	"Commuta modalitat letterbox forçada",
	"", // ÀãááÚØÙ (Russian)
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
	"", // Português
	"", // Français
	"", // Norsk
	"Kytke analoginen audioulostulo",
	"", // Polski
	"Selecciona salida audio analógica",
	"", // ÅëëçíéêÜ (Greek)
	"", // Svenska
	"", // Românã
	"", // Magyar
	"Selecciona sortida àudio analògica",
	"", // ÀãááÚØÙ (Russian)
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
	"", // Português
	"", // Français
	"", // Norsk
	"AC3-ulostulo päälle",
	"", // Polski
	"Activa salida AC3",
	"", // ÅëëçíéêÜ (Greek)
	"", // Svenska
	"", // Românã
	"", // Magyar
	"Activa sortida AC3",
	"", // ÀãááÚØÙ (Russian)
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
	"", // Português
	"", // Français
	"", // Norsk
	"AC3-ulostulo pois",
	"", // Polski
	"Desactiva salida AC3",
	"", // ÅëëçíéêÜ (Greek)
	"", // Svenska
	"", // Românã
	"", // Magyar
	"Desactiva sortida AC3",
	"", // ÀãááÚØÙ (Russian)
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
	"", // Português
	"", // Français
	"", // Norsk
	"Kytke digitaalinen audioulostulo",
	"", // Polski
	"Selecciona salida audio digital",
	"", // ÅëëçíéêÜ (Greek)
	"", // Svenska
	"", // Românã
	"", // Magyar
	"Selecciona sortida àudio digital",
	"", // ÀãááÚØÙ (Russian)
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
	"", // Português
	"", // Français
	"", // Norsk
	"DXR3: vapautetaan laitteet",
	"", // Polski
	"DXR3: liberando dispositivo",
	"", // ÅëëçíéêÜ (Greek)
	"", // Svenska
	"", // Românã
	"", // Magyar
	"DXR3: alliberant dispositiu",
	"", // ÀãááÚØÙ (Russian)
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
	"", // Português
	"", // Français
	"", // Norsk
	"Väriasetukset",
	"", // Polski
	"Ajustes color",
	"", // ÅëëçíéêÜ (Greek)
	"", // Svenska
	"", // Românã
	"", // Magyar
	"Ajustaments color",
	"", // ÀãááÚØÙ (Russian)
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
	"Luminosità",
	"", // Nederlands
	"", // Português
	"", // Français
	"", // Norsk
	"Kirkkaus",
	"", // Polski
	"Luminosidad",
	"", // ÅëëçíéêÜ (Greek)
	"", // Svenska
	"", // Românã
	"", // Magyar
	"Lluminositat",
	"", // ÀãááÚØÙ (Russian)
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
	"", // Português
	"", // Français
	"", // Norsk
	"Kontrasti",
	"", // Polski
	"Contraste",
	"", // ÅëëçíéêÜ (Greek)
	"", // Svenska
	"", // Românã
	"", // Magyar
	"Contrast",
	"", // ÀãááÚØÙ (Russian)
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
	"Sättigung",
	"", // Slovenski
	"Saturazione",
	"", // Nederlands
	"", // Português
	"", // Français
	"", // Norsk
	"Värikylläisyys",
	"", // Polski
	"Saturación",
	"", // ÅëëçíéêÜ (Greek)
	"", // Svenska
	"", // Românã
	"", // Magyar
	"Saturació",
	"", // ÀãááÚØÙ (Russian)
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
	"Hauptmenüeintrag verstecken",
	"", // Slovenski
	"Nascondi voce menù",
	"", // Nederlands
	"", // Português
	"", // Français
	"", // Norsk
	"Piilota valinta päävalikosta",
	"", // Polski
	"Esconde entrada menú",
	"", // ÅëëçíéêÜ (Greek)
	"", // Svenska
	"", // Românã
	"", // Magyar
	"Amaga entrada menú",
	"ÁÚàëâì ÚÞÜÐÝÔã Ò ÓÛÐÒÝÞÜ ÜÕÝî",
	"", // Hrvatski
#if VDRVERSNUM > 10312
	"Peida valik peamenüüs",
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
	"", // Português
	"", // Français
	"", // Norsk
	"Kuvaruutunäytön päivitysväli (ms)",
	"", // Polski
	"Tiempo mínimo refresco OSD (ms)",
	"", // ÅëëçíéêÜ (Greek)
	"", // Svenska
	"", // Românã
	"", // Magyar
	"Temps mínim refresc OSD (ms)",
	"", // ÀãááÚØÙ (Russian)
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
