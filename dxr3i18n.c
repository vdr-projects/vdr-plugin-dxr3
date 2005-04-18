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
		"", // Português
		"", // Français
		"", // Norsk
		"DXR3-toiminnot",
		"", // Polski
		"", // Español
		"", // ÅëëçíéêÜ (Greek)
		"", // Svenska
		"", // Românã
		"", // Magyar
		"", // Català
#if VDRVERSNUM > 10302
		"", // ÀãááÚØÙ (Russian)
# if VDRVERSNUM > 10307
		"", // Hrvatski
#  if VDRVERSNUM > 10312
		"", // Eesti
#   if VDRVERSNUM > 10315
		"", // Dansk
#   endif
#  endif
# endif
#endif
	},
	{
		"Hardware MPEG decoder",
		"", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"", // Français
		"", // Norsk
		"MPEG-purkukortti",
		"", // Polski
		"", // Español
		"", // ÅëëçíéêÜ (Greek)
		"", // Svenska
		"", // Românã
		"", // Magyar
		"", // Català
#if VDRVERSNUM > 10302
		"", // ÀãááÚØÙ (Russian)
# if VDRVERSNUM > 10307
		"", // Hrvatski
#  if VDRVERSNUM > 10312
		"", // Eesti
#   if VDRVERSNUM > 10315
		"", // Dansk
#   endif
#  endif
# endif
#endif
	},
	{
		"DXR3 Adjustment",
		"DXR3 Einstellungen",
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"", // Français
		"", // Norsk
		"DXR3-toiminnot",
		"", // Polski
		"", // Español
		"", // ÅëëçíéêÜ (Greek)
		"", // Svenska
		"", // Românã
		"", // Magyar
		"", // Català
#if VDRVERSNUM > 10302
		"", // ÀãááÚØÙ (Russian)
# if VDRVERSNUM > 10307
		"", // Hrvatski
#  if VDRVERSNUM > 10312
		"", // Eesti
#   if VDRVERSNUM > 10315
		"", // Dansk
#   endif
#  endif
# endif
#endif
	},
	{
		"Digital audio output",
		"Digitaler Audioausgang",
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"", // Français
		"", // Norsk
		"Digitaalinen audioulostulo",
		"", // Polski
		"", // Español
		"", // ÅëëçíéêÜ (Greek)
		"", // Svenska
		"", // Românã
		"", // Magyar
		"", // Català
#if VDRVERSNUM > 10302
		"", // ÀãááÚØÙ (Russian)
# if VDRVERSNUM > 10307
		"", // Hrvatski
#  if VDRVERSNUM > 10312
		"", // Eesti
#   if VDRVERSNUM > 10315
		"", // Dansk
#   endif
#  endif
# endif
#endif
	},
	{
		"Card number",
		"Karte",
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"", // Français
		"", // Norsk
		"Kortin järjestysnumero",
		"", // Polski
		"", // Español
		"", // ÅëëçíéêÜ (Greek)
		"", // Svenska
		"", // Românã
		"", // Magyar
		"", // Català
#if VDRVERSNUM > 10302
		"", // ÀãááÚØÙ (Russian)
# if VDRVERSNUM > 10307
		"", // Hrvatski
#  if VDRVERSNUM > 10312
		"", // Eesti
#   if VDRVERSNUM > 10315
		"", // Dansk
#   endif
#  endif
# endif
#endif
	},
	{
		"Video mode",
		"Videomodus",
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"", // Français
		"", // Norsk
		"Näyttötila",
		"", // Polski
		"", // Español
		"", // ÅëëçíéêÜ (Greek)
		"", // Svenska
		"", // Românã
		"", // Magyar
		"", // Català
#if VDRVERSNUM > 10302
		"", // ÀãááÚØÙ (Russian)
# if VDRVERSNUM > 10307
		"", // Hrvatski
#  if VDRVERSNUM > 10312
		"", // Eesti
#   if VDRVERSNUM > 10315
		"", // Dansk
#   endif
#  endif
# endif
#endif
	},
	{
		"PAL",
		"PAL",
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"", // Français
		"", // Norsk
		"PAL",
		"", // Polski
		"", // Español
		"", // ÅëëçíéêÜ (Greek)
		"", // Svenska
		"", // Românã
		"", // Magyar
		"", // Català
#if VDRVERSNUM > 10302
		"", // ÀãááÚØÙ (Russian)
# if VDRVERSNUM > 10307
		"", // Hrvatski
#  if VDRVERSNUM > 10312
		"", // Eesti
#   if VDRVERSNUM > 10315
		"", // Dansk
#   endif
#  endif
# endif
#endif
	},
	{
		"PAL60",
		"PAL60",
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"", // Français
		"", // Norsk
		"PAL60",
		"", // Polski
		"", // Español
		"", // ÅëëçíéêÜ (Greek)
		"", // Svenska
		"", // Românã
		"", // Magyar
		"", // Català
#if VDRVERSNUM > 10302
		"", // ÀãááÚØÙ (Russian)
# if VDRVERSNUM > 10307
		"", // Hrvatski
#  if VDRVERSNUM > 10312
		"", // Eesti
#   if VDRVERSNUM > 10315
		"", // Dansk
#   endif
#  endif
# endif
#endif
	},
	{
		"NTSC",
		"NTSC",
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"", // Français
		"", // Norsk
		"NTSC",
		"", // Polski
		"", // Español
		"", // ÅëëçíéêÜ (Greek)
		"", // Svenska
		"", // Românã
		"", // Magyar
		"", // Català
#if VDRVERSNUM > 10302
		"", // ÀãááÚØÙ (Russian)
# if VDRVERSNUM > 10307
		"", // Hrvatski
#  if VDRVERSNUM > 10312
		"", // Eesti
#   if VDRVERSNUM > 10315
		"", // Dansk
#   endif
#  endif
# endif
#endif
	},
	{
		"Reset DXR3 hardware",
		"Reset DXR3 Hardware",
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"", // Français
		"", // Norsk
		"Palauta kortti alkutilaan",
		"", // Polski
		"", // Español
		"", // ÅëëçíéêÜ (Greek)
		"", // Svenska
		"", // Românã
		"", // Magyar
		"", // Català
#if VDRVERSNUM > 10302
		"", // ÀãááÚØÙ (Russian)
# if VDRVERSNUM > 10307
		"", // Hrvatski
#  if VDRVERSNUM > 10312
		"", // Eesti
#   if VDRVERSNUM > 10315
		"", // Dansk
#   endif
#  endif
# endif
#endif
	},
	{
		"Toggle force letterbox",
		"Letterbox erzwingen",
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"", // Français
		"", // Norsk
		"Letterbox-näyttötilan pakotus päälle/pois",
		"", // Polski
		"", // Español
		"", // ÅëëçíéêÜ (Greek)
		"", // Svenska
		"", // Românã
		"", // Magyar
		"", // Català
#if VDRVERSNUM > 10302
		"", // ÀãááÚØÙ (Russian)
# if VDRVERSNUM > 10307
		"", // Hrvatski
#  if VDRVERSNUM > 10312
		"", // Eesti
#   if VDRVERSNUM > 10315
		"", // Dansk
#   endif
#  endif
# endif
#endif
	},
	{
		"Switch to analog audio output",
		"Analoge Ausgabe",
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"", // Français
		"", // Norsk
		"Kytke analoginen audioulostulo",
		"", // Polski
		"", // Español
		"", // ÅëëçíéêÜ (Greek)
		"", // Svenska
		"", // Românã
		"", // Magyar
		"", // Català
#if VDRVERSNUM > 10302
		"", // ÀãááÚØÙ (Russian)
# if VDRVERSNUM > 10307
		"", // Hrvatski
#  if VDRVERSNUM > 10312
		"", // Eesti
#   if VDRVERSNUM > 10315
		"", // Dansk
#   endif
#  endif
# endif
#endif
	},
	{
		"AC3 output on",
		"AC3 Ausgabe Ein",
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"", // Français
		"", // Norsk
		"AC3-ulostulo päälle",
		"", // Polski
		"", // Español
		"", // ÅëëçíéêÜ (Greek)
		"", // Svenska
		"", // Românã
		"", // Magyar
		"", // Català
#if VDRVERSNUM > 10302
		"", // ÀãááÚØÙ (Russian)
# if VDRVERSNUM > 10307
		"", // Hrvatski
#  if VDRVERSNUM > 10312
		"", // Eesti
#   if VDRVERSNUM > 10315
		"", // Dansk
#   endif
#  endif
# endif
#endif
	},
	{
		"AC3 output off",
		"AC3 Ausgabe Aus",
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"", // Français
		"", // Norsk
		"AC3-ulostulo pois",
		"", // Polski
		"", // Español
		"", // ÅëëçíéêÜ (Greek)
		"", // Svenska
		"", // Românã
		"", // Magyar
		"", // Català
#if VDRVERSNUM > 10302
		"", // ÀãááÚØÙ (Russian)
# if VDRVERSNUM > 10307
		"", // Hrvatski
#  if VDRVERSNUM > 10312
		"", // Eesti
#   if VDRVERSNUM > 10315
		"", // Dansk
#   endif
#  endif
# endif
#endif
	},
	{
		"Switch to digital audio output",
		"Digitaler Ausgang",
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"", // Français
		"", // Norsk
		"Kytke digitaalinen audioulostulo",
		"", // Polski
		"", // Español
		"", // ÅëëçíéêÜ (Greek)
		"", // Svenska
		"", // Românã
		"", // Magyar
		"", // Català
#if VDRVERSNUM > 10302
		"", // ÀãááÚØÙ (Russian)
# if VDRVERSNUM > 10307
		"", // Hrvatski
#  if VDRVERSNUM > 10312
		"", // Eesti
#   if VDRVERSNUM > 10315
		"", // Dansk
#   endif
#  endif
# endif
#endif
	},
	{
		"DXR3: releasing devices",
		"DXR3: Releasing Devices",
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"", // Français
		"", // Norsk
		"DXR3: vapautetaan laitteet",
		"", // Polski
		"", // Español
		"", // ÅëëçíéêÜ (Greek)
		"", // Svenska
		"", // Românã
		"", // Magyar
		"", // Català
#if VDRVERSNUM > 10302
		"", // ÀãááÚØÙ (Russian)
# if VDRVERSNUM > 10307
		"", // Hrvatski
#  if VDRVERSNUM > 10312
		"", // Eesti
#   if VDRVERSNUM > 10315
		"", // Dansk
#   endif
#  endif
# endif
#endif
	},
	{
		"Color settings",
		"Farbeinstellungen",
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"", // Français
		"", // Norsk
		"Väriasetukset",
		"", // Polski
		"", // Español
		"", // ÅëëçíéêÜ (Greek)
		"", // Svenska
		"", // Românã
		"", // Magyar
		"", // Català
#if VDRVERSNUM > 10302
		"", // ÀãááÚØÙ (Russian)
# if VDRVERSNUM > 10307
		"", // Hrvatski
#  if VDRVERSNUM > 10312
		"", // Eesti
#   if VDRVERSNUM > 10315
		"", // Dansk
#   endif
#  endif
# endif
#endif
	},
	{
		"Brightness",
		"Helligkeit",
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"", // Français
		"", // Norsk
		"Kirkkaus",
		"", // Polski
		"", // Español
		"", // ÅëëçíéêÜ (Greek)
		"", // Svenska
		"", // Românã
		"", // Magyar
		"", // Català
#if VDRVERSNUM > 10302
		"", // ÀãááÚØÙ (Russian)
# if VDRVERSNUM > 10307
		"", // Hrvatski
#  if VDRVERSNUM > 10312
		"", // Eesti
#   if VDRVERSNUM > 10315
		"", // Dansk
#   endif
#  endif
# endif
#endif
	},
	{
		"Contrast",
		"Kontrast",
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"", // Français
		"", // Norsk
		"Kontrasti",
		"", // Polski
		"", // Español
		"", // ÅëëçíéêÜ (Greek)
		"", // Svenska
		"", // Românã
		"", // Magyar
		"", // Català
#if VDRVERSNUM > 10302
		"", // ÀãááÚØÙ (Russian)
# if VDRVERSNUM > 10307
		"", // Hrvatski
#  if VDRVERSNUM > 10312
		"", // Eesti
#   if VDRVERSNUM > 10315
		"", // Dansk
#   endif
#  endif
# endif
#endif
	},
	{
		"Saturation",
		"Sättigung",
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"", // Français
		"", // Norsk
		"Värikylläisyys",
		"", // Polski
		"", // Español
		"", // ÅëëçíéêÜ (Greek)
		"", // Svenska
		"", // Românã
		"", // Magyar
		"", // Català
#if VDRVERSNUM > 10302
		"", // ÀãááÚØÙ (Russian)
# if VDRVERSNUM > 10307
		"", // Hrvatski
#  if VDRVERSNUM > 10312
		"", // Eesti
#   if VDRVERSNUM > 10315
		"", // Dansk
#   endif
#  endif
# endif
#endif
	},
	{
		"Debug mode",
		"", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"", // Français
		"", // Norsk
		"Debug-tila",
		"", // Polski
		"", // Español
		"", // ÅëëçíéêÜ (Greek)
		"", // Svenska
		"", // Românã
		"", // Magyar
		"", // Català
#if VDRVERSNUM > 10302
		"", // ÀãááÚØÙ (Russian)
# if VDRVERSNUM > 10307
		"", // Hrvatski
#  if VDRVERSNUM > 10312
		"", // Eesti
#   if VDRVERSNUM > 10315
		"", // Dansk
#   endif
#  endif
# endif
#endif
	},
	{
		"Debug level",
		"", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"", // Français
		"", // Norsk
		"Debug-taso",
		"", // Polski
		"", // Español
		"", // ÅëëçíéêÜ (Greek)
		"", // Svenska
		"", // Românã
		"", // Magyar
		"", // Català
#if VDRVERSNUM > 10302
		"", // ÀãááÚØÙ (Russian)
# if VDRVERSNUM > 10307
		"", // Hrvatski
#  if VDRVERSNUM > 10312
		"", // Eesti
#   if VDRVERSNUM > 10315
		"", // Dansk
#   endif
#  endif
# endif
#endif
	},
	{
		"low",
		"", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"", // Français
		"", // Norsk
		"alhainen",
		"", // Polski
		"", // Español
		"", // ÅëëçíéêÜ (Greek)
		"", // Svenska
		"", // Românã
		"", // Magyar
		"", // Català
#if VDRVERSNUM > 10302
		"", // ÀãááÚØÙ (Russian)
# if VDRVERSNUM > 10307
		"", // Hrvatski
#  if VDRVERSNUM > 10312
		"", // Eesti
#   if VDRVERSNUM > 10315
		"", // Dansk
#   endif
#  endif
# endif
#endif
	},
	{
		"everything",
		"", // Deutsch
		"", // Slovenski
		"", // Italiano
		"", // Nederlands
		"", // Português
		"", // Français
		"", // Norsk
		"kaikki",
		"", // Polski
		"", // Español
		"", // ÅëëçíéêÜ (Greek)
		"", // Svenska
		"", // Românã
		"", // Magyar
		"", // Català
#if VDRVERSNUM > 10302
		"", // ÀãááÚØÙ (Russian)
# if VDRVERSNUM > 10307
		"", // Hrvatski
#  if VDRVERSNUM > 10312
		"", // Eesti
#   if VDRVERSNUM > 10315
		"", // Dansk
#   endif
#  endif
# endif
#endif
	},
	{ NULL }
};

// Local variables:
// mode: c++
// c-file-style: "stroustrup"
// indent-tabs-mode: t
// End:
