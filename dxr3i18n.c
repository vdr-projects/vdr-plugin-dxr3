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
	"DXR3",
	"DXR3",
	"", // ÅëëçíéêÜ (Greek)
	"", // Svenska
	"", // Românã
	"", // Magyar
	"DXR3",
	"", // ÀãááÚØÙ (Russian)
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
	"", // Português
	"", // Français
	"", // Norsk
	"MPEG-purkukortti",
	"Sprzêtowy dekoder MPEG",
	"Descodificador MPEG Hardware",
	"", // ÅëëçíéêÜ (Greek)
	"", // Svenska
	"", // Românã
	"", // Magyar
	"Descodificador MPEG Hardware",
	"", // ÀãááÚØÙ (Russian)
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
	"", // Português
	"", // Français
	"", // Norsk
	"DXR3-toiminnot",
	"Ustawienia DXR3",
	"Opciones DXR3",
	"", // ÅëëçíéêÜ (Greek)
	"", // Svenska
	"", // Românã
	"", // Magyar
	"Opcions DXR3",
	"", // ÀãááÚØÙ (Russian)
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
	"", // Português
	"", // Français
	"", // Norsk
	"Digitaalinen audioulostulo",
	"Cyfrowe wyj¶cie d¼wiêku",
	"Salida audio digital",
	"", // ÅëëçíéêÜ (Greek)
	"", // Svenska
	"", // Românã
	"", // Magyar
	"Sortida àudio digital",
	"", // ÀãááÚØÙ (Russian)
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
	"", // Português
	"", // Français
	"", // Norsk
	"Kortin järjestysnumero",
	"Numer karty",
	"Tarjeta",
	"", // ÅëëçíéêÜ (Greek)
	"", // Svenska
	"", // Românã
	"", // Magyar
	"Targeta",
	"", // ÀãááÚØÙ (Russian)
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
	"", // Português
	"", // Français
	"", // Norsk
	"Näyttötila",
	"Tryb obrazu",
	"Modalidad vídeo",
	"", // ÅëëçíéêÜ (Greek)
	"", // Svenska
	"", // Românã
	"", // Magyar
	"Modalitat vídeo",
	"", // ÀãááÚØÙ (Russian)
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
	"", // Português
	"", // Français
	"", // Norsk
	"PAL",
	"PAL",
	"PAL",
	"", // ÅëëçíéêÜ (Greek)
	"", // Svenska
	"", // Românã
	"", // Magyar
	"PAL",
	"", // ÀãááÚØÙ (Russian)
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
	"", // Português
	"", // Français
	"", // Norsk
	"PAL60",
	"PAL60",
	"PAL60",
	"", // ÅëëçíéêÜ (Greek)
	"", // Svenska
	"", // Românã
	"", // Magyar
	"PAL60",
	"", // ÀãááÚØÙ (Russian)
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
	"", // Português
	"", // Français
	"", // Norsk
	"NTSC",
	"NTSC",
	"NTSC",
	"", // ÅëëçíéêÜ (Greek)
	"", // Svenska
	"", // Românã
	"", // Magyar
	"NTSC",
	"", // ÀãááÚØÙ (Russian)
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
	"", // Português
	"", // Français
	"", // Norsk
	"Käytä WSS-signalointia",
	"", // Polski
	"", // Español
	"", // ÅëëçíéêÜ (Greek)
	"", // Svenska
	"", // Românã
	"", // Magyar
	"", // Català
	"", // ÀãááÚØÙ (Russian)
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
	"", // Português
	"", // Français
	"", // Norsk
	"Palauta kortti alkutilaan",
	"Zerowanie dekodera DXR3",
	"Reset hardware DXR3",
	"", // ÅëëçíéêÜ (Greek)
	"", // Svenska
	"", // Românã
	"", // Magyar
	"Reset hardware DXR3",
	"", // ÀãááÚØÙ (Russian)
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
	"", // Português
	"", // Français
	"", // Norsk
	"Letterbox-näyttötilan pakotus päälle/pois",
	"Prze³±cz wymuszanie trybu letterbox",
	"Conmuta modalidad letterbox forzada",
	"", // ÅëëçíéêÜ (Greek)
	"", // Svenska
	"", // Românã
	"", // Magyar
	"Commuta modalitat letterbox forçada",
	"", // ÀãááÚØÙ (Russian)
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
	"", // Português
	"", // Français
	"", // Norsk
	"Kytke analoginen audioulostulo",
	"Prze³±cz w tryb d¼wiêku analogowego",
	"Selecciona salida audio analógica",
	"", // ÅëëçíéêÜ (Greek)
	"", // Svenska
	"", // Românã
	"", // Magyar
	"Selecciona sortida àudio analògica",
	"", // ÀãááÚØÙ (Russian)
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
	"", // Português
	"", // Français
	"", // Norsk
	"Kytke digitaalinen audioulostulo",
	"Prze³±cz w tryb d¼wiêku cyfrowego",
	"Selecciona salida audio digital",
	"", // ÅëëçíéêÜ (Greek)
	"", // Svenska
	"", // Românã
	"", // Magyar
	"Selecciona sortida àudio digital",
	"", // ÀãááÚØÙ (Russian)
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
	"", // Português
	"", // Français
	"", // Norsk
	"DXR3: vapautetaan laitteet",
	"DXR3: zwalnianie sprzêtu",
	"DXR3: liberando dispositivo",
	"", // ÅëëçíéêÜ (Greek)
	"", // Svenska
	"", // Românã
	"", // Magyar
	"DXR3: alliberant dispositiu",
	"", // ÀãááÚØÙ (Russian)
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
	"", // Português
	"", // Français
	"", // Norsk
	"Väriasetukset",
	"Ustawienia koloru",
	"Ajustes color",
	"", // ÅëëçíéêÜ (Greek)
	"", // Svenska
	"", // Românã
	"", // Magyar
	"Ajustaments color",
	"", // ÀãááÚØÙ (Russian)
	"", // Hrvatski
	"", // Eesti
	"", // Dansk
	"", // Czech
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
	"Jasno¶æ",
	"Luminosidad",
	"", // ÅëëçíéêÜ (Greek)
	"", // Svenska
	"", // Românã
	"", // Magyar
	"Lluminositat",
	"", // ÀãááÚØÙ (Russian)
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
	"", // Português
	"", // Français
	"", // Norsk
	"Kontrasti",
	"Kontrast",
	"Contraste",
	"", // ÅëëçíéêÜ (Greek)
	"", // Svenska
	"", // Românã
	"", // Magyar
	"Contrast",
	"", // ÀãááÚØÙ (Russian)
	"", // Hrvatski
	"", // Eesti
	"", // Dansk
	"", // Czech
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
	"Nasycenie",
	"Saturación",
	"", // ÅëëçíéêÜ (Greek)
	"", // Svenska
	"", // Românã
	"", // Magyar
	"Saturació",
	"", // ÀãááÚØÙ (Russian)
	"", // Hrvatski
	"", // Eesti
	"", // Dansk
	"", // Czech
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
	"Ukryj g³ówny wpis w menu",
	"Esconde entrada menú",
	"", // ÅëëçíéêÜ (Greek)
	"", // Svenska
	"", // Românã
	"", // Magyar
	"Amaga entrada menú",
	"ÁÚàëâì ÚŞÜĞİÔã Ò ÓÛĞÒİŞÜ ÜÕİî",
	"", // Hrvatski
	"Peida valik peamenüüs",
	"", // Dansk
	"", // Czech
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
	"Czas od¶wie¿ania OSD (ms)",
	"Tiempo mínimo refresco OSD (ms)",
	"", // ÅëëçíéêÜ (Greek)
	"", // Svenska
	"", // Românã
	"", // Magyar
	"Temps mínim refresc OSD (ms)",
	"", // ÀãááÚØÙ (Russian)
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
