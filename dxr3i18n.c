#include "dxr3i18n.h"

const char *i18n_name = 0;

// vdr 1.1.32 -> 16
// vdr 1.3.2  -> 17
// vdr 1.3.7  -> 18

// Digital audio output
// DXR3 card
// DXR3 video mode
// DXR3 menu mode
// Reset DXR3 Hardware
// Toggle Force LetterBox
// Analog Output
// AC3 Output On
// AC3 Output Off
// Digital Output
// Sub-Picutre
// MPEG
// Dxr3: Releasing devices

// add
// Dxr3: Releasing devices
/*
 * Translations provided by:
 *
 * Slovenian   Miha Setina <mihasetina@softhome.net> and Matjaz Thaler <matjaz.thaler@guest.arnes.si>
 * Italian     Alberto Carraro <bertocar@tin.it> and Antonio Ospite <ospite@studenti.unina.it>
 * Dutch       Arnold Niessen <niessen@iae.nl> <arnold.niessen@philips.com> and Hans Dingemans <hans.dingemans@tacticalops.nl>
 * Portuguese  Paulo Lopes <pmml@netvita.pt>
 * French      Jean-Claude Repetto <jc@repetto.org>, Olivier Jacques <jacquesolivier@hotmail.com> and Gregoire Favre <greg@magma.unil.ch>
 * Norwegian   Jørgen Tvedt <pjtvedt@online.no> and Truls Slevigen <truls@slevigen.no>
 * Finnish     Hannu Savolainen <hannu@opensound.com>, Jaakko Hyvätti <jaakko@hyvatti.iki.fi>, Niko Tarnanen <niko.tarnanen@hut.fi> and Rolf Ahrenberg <rahrenbe@cc.hut.fi>
 * Polish      Michael Rakowski <mrak@gmx.de>
 * Spanish     Ruben Nunez Francisco <ruben.nunez@tang-it.com>
 * Greek       Dimitrios Dimitrakos <mail@dimitrios.de>
 * Swedish     Tomas Prybil <tomas@prybil.se> and Jan Ekholm <chakie@infa.abo.fi>
 * Romanian    Paul Lacatus <paul@campina.iiruc.ro>
 * Hungarian   Istvan Koenigsberger <istvnko@hotmail.com> and Guido Josten <guido.josten@t-online.de>
 * Catalanian  Marc Rovira Vall <tm05462@salleURL.edu>, Ramon Roca <ramon.roca@xcombo.com> and Jordi Vilà <jvila@tinet.org>
 * Russian     Vyacheslav Dikonov <sdiconov@mail.ru>
 * Croatian    Drazen Dupor <drazen.dupor@dupor.com>
 *
 */


#if VDRVERSNUM >= 10307
// vdr 1.3.7  -> 18 languages

/*
const tI18nPhrase Phrases[] = {
  // The name of the language (this MUST be the first phrase!):
  { "English",
    "Deutsch",
    "Slovenski",
    "Italiano",
    "Nederlands",
    "Português",
    "Français",
    "Norsk",
    "suomi", // this is not a typo - it's really lowercase!
    "Polski",
    "Español",
    "ÅëëçíéêÜ", // Greek
    "Svenska",
    "Romaneste",
    "Magyar",
    "Català",
    "ÀãááÚØÙ", // Russian
    "Hrvatski",
  },
*/

const tI18nPhrase Phrases[] = {
  { 
    "DXR3",
    "DXR3",
    "DXR3",
    "DXR3",
    "DXR3",
    "DXR3",
    "DXR3",
    "DXR3",
    "DXR3",
    "DXR3",
    "DXR3",
    "DXR3",
    "DXR3",
    "DXR3",
    "DXR3",
    "DXR3",
    "DXR3",
	"DXR3",
  },
  { 
    "Digital audio output",
    "Digitaler Audioausgang",
    "", // TODO
    "", // TODO
    "", // TODO
    "Saída áudio digital",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
  },
  { "DXR3 card",
    "DXR3 Karte",
    "", // TODO
    "", // TODO
    "", // TODO
    "Placa DXR3",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
	"", // TODO
	"", // TODO
  },
  { "DXR3 video mode",
    "DXR3 Video-Modus",
    "", // TODO
    "", // TODO
    "", // TODO
    "Modo vídeo DXR3",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
	"", // TODO
	"", // TODO
  },
  { "DXR3 menu mode",
    "DXR3 Menü-Modus",
    "", // TODO
    "", // TODO
    "", // TODO
    "Modo menu DXR3",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
	"", // TODO
	"", // TODO
  },
  { "Reset DXR3 Hardware",
    "Resete DXR3 Hardware",
    "", // TODO
    "", // TODO
    "", // TODO
    "Reiniciar DXR3",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
	"", // TODO
	"", // TODO
  },
  { "Toggle Force LetterBox",
    "LetterBox erzwingen",
    "", // TODO
    "", // TODO
    "", // TODO
    "Activar Forçar LetterBox",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
	"", // TODO
	"", // TODO
  },
  { "Analog Output",
    "Analoge Ausgabe",
    "", // TODO
    "", // TODO
    "", // TODO
    "Saída Analógica",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
	"", // TODO
	"", // TODO
  },
  { "AC3 Output On",
    "AC3 Ausgabe Ein",
    "", // TODO
    "", // TODO
    "", // TODO
    "Saída AC3 Activa",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
	"", // TODO
	"", // TODO
  },
  { "AC3 Output Off",
    "AC3 Ausgabe Aus",
    "", // TODO
    "", // TODO
    "", // TODO
    "Saída AC3 Inactiva",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
	"", // TODO
	"", // TODO
  },
  { "Digital Output",
    "Digitaler Ausgang",
    "", // TODO
    "", // TODO
    "", // TODO
    "Saída Digital",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
	"", // TODO
	"", // TODO
  },
  { "Sub-Picture",
    "Sub-Picture",
    "Sub-Picture",
    "Sub-Picture",
    "Sub-Picture",
    "Sub-Imagem",
    "Sub-Picture",
    "Sub-Picture",
    "Sub-Picture",
    "Sub-Picture",
    "Sub-Picture",
    "Sub-Picture",
    "Sub-Picture",
    "Sub-Picture",
    "Sub-Picture",
    "Sub-Picture",
	"Sub-Picture",
	"Sub-Picture",
  },
  { "MPEG",
    "MPEG",
    "MPEG",
    "MPEG",
    "MPEG",
    "MPEG",
    "MPEG",
    "MPEG",
    "MPEG",
    "MPEG",
    "MPEG", 
    "MPEG",
    "MPEG",
    "MPEG",
    "MPEG",
    "MPEG",
	"MPEG",
	"MPEG",
  },
  { "DXR3: Releasing devices",
    "DXR3: Releasing devices",
    "", // TODO
    "", // TODO
    "", // TODO
    "DXR3: A libertar dispositivos",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
	"", // TODO
	"", // TODO
  },
  { NULL }
};


#elif VDRVERSNUM <= 10302
// vdr 1.1.32 -> 16 languages

const tI18nPhrase Phrases[] = {
  { 
    "Digital audio output",
    "Digitaler Audioausgang",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
  },
  { "DXR3 card",
    "DXR3 Karte",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
  },
  { "DXR3 video mode",
    "DXR3 Video-Modus",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
  },
  { "DXR3 menu mode",
    "DXR3 Menü-Modus",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
  },
  { "Reset DXR3 Hardware",
    "Resete DXR3 Hardware",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
  },
  { "Toggle Force LetterBox",
    "LetterBox erzwingen",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
  },
  { "Analog Output",
    "Analoge Ausgabe",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
  },
  { "AC3 Output On",
    "AC3 Ausgabe Ein",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
  },
  { "AC3 Output Off",
    "AC3 Ausgabe Aus",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
  },
  { "Digital Output",
    "Digitaler Ausgang",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
  },
  { "Sub-Picture",
    "Sub-Picture",
    "Sub-Picture",
    "Sub-Picture",
    "Sub-Picture",
    "Sub-Picture",
    "Sub-Picture",
    "Sub-Picture",
    "Sub-Picture",
    "Sub-Picture",
    "Sub-Picture",
    "Sub-Picture",
    "Sub-Picture",
    "Sub-Picture",
    "Sub-Picture",
    "Sub-Picture",
  },
  { "MPEG",
    "MPEG",
    "MPEG",
    "MPEG",
    "MPEG",
    "MPEG",
    "MPEG",
    "MPEG",
    "MPEG",
    "MPEG",
    "MPEG", 
    "MPEG",
    "MPEG",
    "MPEG",
    "MPEG",
    "MPEG",
	"MPEG",
  },
  { NULL }
};

#else
// vdr 1.3.2  -> 17 languages

const tI18nPhrase Phrases[] = {
  { 
    "DXR3",
    "DXR3",
    "DXR3",
    "DXR3",
    "DXR3",
    "DXR3",
    "DXR3",
    "DXR3",
    "DXR3",
    "DXR3",
    "DXR3",
    "DXR3",
    "DXR3",
    "DXR3",
    "DXR3",
    "DXR3",
    "DXR3",
  },
  { 
    "Digital audio output",
    "Digitaler Audioausgang",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
  },
  { "DXR3 card",
    "DXR3 Karte",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
	"", // TODO
	"", // TODO
  },
  { "DXR3 video mode",
    "DXR3 Video-Modus",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
	"", // TODO
	"", // TODO
  },
  { "DXR3 menu mode",
    "DXR3 Menü-Modus",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
	"", // TODO
	"", // TODO
  },
  { "Reset DXR3 Hardware",
    "Resete DXR3 Hardware",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
	"", // TODO
	"", // TODO
  },
  { "Toggle Force LetterBox",
    "LetterBox erzwingen",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
	"", // TODO
	"", // TODO
  },
  { "Analog Output",
    "Analoge Ausgabe",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
	"", // TODO
	"", // TODO
  },
  { "AC3 Output On",
    "AC3 Ausgabe Ein",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
	"", // TODO
	"", // TODO
  },
  { "AC3 Output Off",
    "AC3 Ausgabe Aus",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
	"", // TODO
	"", // TODO
  },
  { "Digital Output",
    "Digitaler Ausgang",
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
    "", // TODO
	"", // TODO
	"", // TODO
  },
  { "Sub-Picture",
    "Sub-Picture",
    "Sub-Picture",
    "Sub-Picture",
    "Sub-Picture",
    "Sub-Picture",
    "Sub-Picture",
    "Sub-Picture",
    "Sub-Picture",
    "Sub-Picture",
    "Sub-Picture",
    "Sub-Picture",
    "Sub-Picture",
    "Sub-Picture",
    "Sub-Picture",
	"Sub-Picture",
	"Sub-Picture",
  },
  { "MPEG",
    "MPEG",
    "MPEG",
    "MPEG",
    "MPEG",
    "MPEG",
    "MPEG",
    "MPEG",
    "MPEG",
    "MPEG", 
    "MPEG",
    "MPEG",
    "MPEG",
    "MPEG",
    "MPEG",
	"MPEG",
	"MPEG",
  },
  { NULL }
};

#endif
