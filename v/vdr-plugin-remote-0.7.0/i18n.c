/*
 * Remote Control plugin for the Video Disk Recorder
 *
 * i18n.c: Internationalization
 *
 * Copyright (C) 2002-2012 Oliver Endriss <o.endriss@gmx.de>
 *
 * Translations provided by:
 * Finnish      Ville Skyttä <ville dot skytta at iki dot fi>
 * French       Pierre Briec <eclipse dot vdr at gmail dot com>
 * Italian      Nicola Franchi <franchi at cefriel dot it>
 * Italian      Gringo <vdr-italian@tiscali.it>
 * Polish       Jakub Wolnicki
 * Russian      Waldemar Nikel
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * Or, point your browser to http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 */

#include "i18n.h"

/* 
  // The name of the language (this MUST be the first phrase!):
  { "English",
    "Deutsch",
    "Slovenski",
    "Italiano",
    "Nederlands",
    "Português",
    "Français",
    "Norsk",
    "suomi",
    "Polski",
    "Español",
    "Ellinika",
    "Svenska",
    "Romaneste",
    "Magyar",
    "Català",
    "ÀãááÚØÙ", // Russian
  },
*/

const tI18nPhrase remotePhrases[] = {

  // Menu titles:
  { "Remote control",
    "Fernbedienung",
    "",
    "Telecomando",
    "",
    "",
    "Télécommande",
    "",
    "Kaukosäädin",
    "Zdalne sterowanie",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "¿ãÛìâ ãßàÐÒÛÕÝØï",
#endif
  },

  { "Press any key to use pre-loaded keymap",
    "Taste drücken, um zuvor geladene Keymap zu verwenden",
    "",
    "Premi un tasto per usare una tabella dei tasti precaricata",
    "",
    "",
    "Appuyez sur une touche pour utiliser un Keymap préchargé",
    "",
    "Paina mitä tahansa näppäintä käyttääksesi oletusnäppäinkarttaa",
    "Nacisnij dowolny klawisz, aby uzyc zaladowanej mapy klawiszy",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "½ÐÖÜØâÕ ÛîÑãî ÚÝÞßÚã ÔÛï ØáßÞÛì×ÞÒÐÝØï àÐÝÕÕ ×ÐÓàãÖÕÝÞÙ àÐáÚÛÐÔÚØ",
#endif
  },

  { "User-supplied keymap will be used",
    "Benutzerdefinierte Keymap wird verwendet",
    "",
    "Verrà usata una mappa dei tasti fornita dall'utente",
    "",
    "",
    "Keymap personnalisé va être utilisé",
    "",
    "Käytetään käyttäjän määrittelemää näppäinkarttaa",
    "Uzyta zostanie mapa klawiszy, zdefinowana przez uzytkownika",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "±ãÔÕâ ØáßÞÛì×ÞÒÐâìáï àÐáÚÛÐÔÚÐ ßÞÛì×ÞÒÐâÕÛï",
#endif
  },


  { "Remote control test - press and hold down any key",
    "Test der Fernbedienung - beliebige Taste gedrückt halten",
    "",
    "Prova telecomando - premi e tieni premuto un tasto",
    "",
    "",
    "Test télécommande - Appuyer et maintenir une touche enfoncée",
    "",
    "Kaukosäädintesti - paina ja pidä alhaalla jotain kaukosäätimen näppäintä",
    "Test zdalnego sterowania - nacisnij i przytrzymaj dowolny klawisz",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "¿àÞÒÕàÚÐ ßãÛìâÐ - ÝÐÖÜØâÕ Ø ãÔÕàÖØâÕ ÛîÑãî ÚÝÞßÚã",
#endif
  },

  { "RC5 protocol detected",
    "RC5-Protokoll erkannt",
    "",
    "Rilevato protocollo RC5",
    "",
    "",
    "Protocole RC5 détecté",
    "",
    "RC5-yhteyskäytäntö tunnistettu",
    "Wykryto protokol RC5",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "¾ßÞ×ÝÐÝ ßàÞâÞÚÞÛ RC5",
#endif
  },

  { "RC5 protocol detected (inverted signal)",
    "RC5-Protokoll erkannt (invertiertes Signal)",
    "",
    "Rilevato protocollo RC5 (segnale invertito)",
    "",
    "",
    "Protocole RC5 détecté (signal inversé)",
    "",
    "RC5-yhteyskäytäntö tunnistettu (käänteinen signaali)",
    "Wykryto protokol RC5 (sygnal odwrocony)",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "¾ßÞ×ÝÐÝ ßàÞâÞÚÞÛ RC5 (á ØÝÒÕàâØàÞÒÐÝØÕÜ áØÓÝÐÛÐ)",
#endif
  },

  { "RCMM protocol detected",
    "RCMM-Protokoll erkannt",
    "",
    "Rilevato protocollo RCMM",
    "",
    "",
    "Protocole RCMM détecté",
    "",
    "RCMM-yhteyskäytäntö tunnistettu",
    "Wykryto protokol RCMM",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "¾ßÞ×ÝÐÝ ßàÞâÞÚÞÛ RCMM",
#endif
  },

  { "RCMM protocol detected (inverted signal)",
    "RCMM-Protokoll erkannt (invertiertes Signal)",
    "",
    "Rilevato protocollo RCMM (segnale invertito)",
    "",
    "",
    "Protocole RCMM détecté (signal inversé)",
    "",
    "RCMM-yhteyskäytäntö tunnistettu (käänteinen signaali)",
    "Wykryto protokol RCMM (sygnal odwrocony)",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "¾ßÞ×ÝÐÝ ßàÞâÞÚÞÛ RCMM (á ØÝÒÕàâØàÞÒÐÝØÕÜ áØÓÝÐÛÐ)",
#endif
  },

  { "No remote control detected",
    "Keine Fernbedienung erkannt",
    "",
    "Nessun telecomando rilevato",
    "",
    "",
    "Aucune télécommande détectée",
    "",
    "Kaukosäädintä ei tunnistettu",
    "Nie znaleziono zdalnego sterowania",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "¿ãÛìâ ãßàÐÒÛÕÝØï ÝÕ ÞßÞ×ÝÐÝ",
#endif
  },

  { "Error uploading keymap",
    "Fehler beim Laden der Tastenbelegung",
    "",
    "Errore caricamento tabella dei tasti",
    "",
    "",
    "Erreur chargement tables de touches",
    "",
    "Näppäinkartan lataaminen epäonnistui",
    "Blad przy przesylaniu mapy klawiszy",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "¾èØÑÚÐ ßàØ ×ÐÓàã×ÚÕ àÐáÚÛÐÔÚØ",
#endif
  },

  { "%s: %s",
    "%s: %s",
    "",
    "%s: %s",
    "",
    "",
    "%s: %s",
    "",
    "%s: %s",
    "%s: %s",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "%s: %s",
#endif
  },

  { NULL }
  };
