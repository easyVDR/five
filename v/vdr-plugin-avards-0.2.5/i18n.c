/*
 * i18n.c: Internationalization
 *
 * See the main source file 'avards.c' for copyright information and
 * how to reach the author.
 *
 */

#include "i18n.h"

#if VDRVERSNUM < 10507
const tI18nPhrase Phrases[] = {
// START I18N - automatically generated by po2i18n.pl
  { "Auto start plugin",
    "Plugin Autostart",
    "",
    "",
    "Plugin Autostart",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
#if VDRVERSNUM >= 10502
    "",
#endif
  },
  { "Avards start",
    "Avards starten",
    "",
    "",
    "Avards starten",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
#if VDRVERSNUM >= 10502
    "",
#endif
  },
  { "Avards stop",
    "Avards stoppen",
    "",
    "",
    "Avards Stoppen",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
#if VDRVERSNUM >= 10502
    "",
#endif
  },
  { "Enable L>16:9 (2,4:1)",
    "Erlaube L>16:9 (2,4:1)",
    "",
    "",
    "Toestaan L>16:9 (2,4:1)",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
#if VDRVERSNUM >= 10502
    "",
#endif
  },
  { "Pan tone max. black",
    "Pan Tone max. Schwarzwert",
    "",
    "",
    "Pan toon max Zwartwaarde",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
#if VDRVERSNUM >= 10502
    "",
#endif
  },
  { "Pan tone tolerance",
    "Pan Tone Toleranz",
    "",
    "",
    "Pan toon tolerantie",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
#if VDRVERSNUM >= 10502
    "",
#endif
  },
  { "Show message on WSS switch",
    "Zeige Nachricht bei Umschaltung",
    "",
    "",
    "Toon bericht bij omzetten WSS",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
#if VDRVERSNUM >= 10502
    "",
#endif
  },
  { "VDR OSD is",
    "VDR OSD ist",
    "",
    "",
    "VDR OSD is",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
#if VDRVERSNUM >= 10502
    "",
#endif
  },
  { "WSS mode",
    "WSS-Modus",
    "",
    "",
    "WSS-modus",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
#if VDRVERSNUM >= 10502
    "",
#endif
  },
  { "auto",
    "automatisch",
    "",
    "",
    "automatisch",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
#if VDRVERSNUM >= 10502
    "",
#endif
  },
  { "delay (in polls)",
    "Verz�gerung (in Polls)",
    "",
    "",
    "vertraging (in polls)",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
#if VDRVERSNUM >= 10502
    "",
#endif
  },
  { "detection",
    "Erkennung",
    "",
    "",
    "detectie",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
#if VDRVERSNUM >= 10502
    "",
#endif
  },
  { "force 16:9",
    "erzwinge 16:9",
    "",
    "",
    "afdwingen 16:9",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
#if VDRVERSNUM >= 10502
    "",
#endif
  },
  { "force 4:3",
    "erzwinge 4:3",
    "",
    "",
    "afdwingen 4:3",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
#if VDRVERSNUM >= 10502
    "",
#endif
  },
  { "force L14:9",
    "erzwinge L14:9",
    "",
    "",
    "afdwingen L14:9",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
#if VDRVERSNUM >= 10502
    "",
#endif
  },
  { "force L16:9",
    "erzwinge L16:9",
    "",
    "",
    "afdwingen L16:9",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
#if VDRVERSNUM >= 10502
    "",
#endif
  },
  { "force L>16:9",
    "erzwinge L>16:9",
    "",
    "",
    "afdwingen L>16:9",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
#if VDRVERSNUM >= 10502
    "",
#endif
  },
  { "logo width (%)",
    "Logobreite (%)",
    "",
    "",
    "logobreedte (%)",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
#if VDRVERSNUM >= 10502
    "",
#endif
  },
  { "mode",
    "Modus",
    "",
    "",
    "modus",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
#if VDRVERSNUM >= 10502
    "",
#endif
  },
  { "overscan (%)",
    "Overscan (%)",
    "",
    "",
    "overscan (%)",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
#if VDRVERSNUM >= 10502
    "",
#endif
  },
  { "poll rate (milliseconds)",
    "Poll Rate (Millisekunden)",
    "",
    "",
    "poll rate (miliseconden)",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
#if VDRVERSNUM >= 10502
    "",
#endif
  },
  { "preserve aspect ratio",
    "behalte Aspect Ratio",
    "",
    "",
    "bewaar aspect ratio",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
#if VDRVERSNUM >= 10502
    "",
#endif
  },
  { "show all lines",
    "zeige alle Zeilen",
    "",
    "",
    "toon alle lijnen",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
#if VDRVERSNUM >= 10502
    "",
#endif
  },
  { "start/stop entry in main menu",
    "Start/Stop Eintrag im Hauptmen�",
    "",
    "",
    "Start/Stop in hoofdmenu",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
#if VDRVERSNUM >= 10502
    "",
#endif
  },
  { "switching WSS mode to",
    "WSS umgeschaltet auf",
    "",
    "",
    "zet WSS mode om naar",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
#if VDRVERSNUM >= 10502
    "",
#endif
  },
  { "switching off WSS",
    "WSS abgeschaltet",
    "",
    "",
    "WSS uitzetten",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
#if VDRVERSNUM >= 10502
    "",
#endif
  },
  { "test if frontend has lock",
    "Frontend Lock testen",
    "",
    "",
    "frontend lock testen",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
#if VDRVERSNUM >= 10302
    "",
#endif
#if VDRVERSNUM >= 10307
    "",
#endif
#if VDRVERSNUM >= 10313
    "",
#endif
#if VDRVERSNUM >= 10316
    "",
#endif
#if VDRVERSNUM >= 10342
    "",
#endif
#if VDRVERSNUM >= 10502
    "",
#endif
  },
// END I18N - automatically generated by po2i18n.pl
  { NULL }
  };
#endif