/*
 * i18n.c: Internationalization
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: i18n.c 1.3 2002/06/23 13:05:59 kls Exp $
 */

#include "i18n.h"

#if VDRVERSNUM < 10507

const tI18nPhrase Phrases[] = {
  { "Not active",// English
    "Nicht aktiv", //Deutsch
    "",// Slovenski
    "",// Italiano
    "",// Nederlands
    "",// Português
    "",// Francais
    "",// Norsk
    "Ei aktiivinen",// Suomi
    "",// Polski
    "",// Español
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
    "",// cat
    "",// rus
    "",// hrv
#if VDRVERSNUM >= 10313
    "",// est
#endif
#if VDRVERSNUM >= 10316
    "",// dan
#endif    
#if VDRVERSNUM >= 10342
    "",// cze
#endif    
#if VDRVERSNUM >= 10502
    "",// tur
#endif    
  },
  { "Disable sleeptimer in %d minutes",
		"Sleeptimer in %d Minuten abbrechen",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Peruuta %d minuutin ajastin?",// Suomi
    "",// Polski
    "",// Espaqol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
    "",// cat
    "",// rus
    "",// hrv
#if VDRVERSNUM >= 10313
    "",// est
#endif
#if VDRVERSNUM >= 10316
    "",// dan
#endif
#if VDRVERSNUM >= 10342
    "",// cze
#endif
#if VDRVERSNUM >= 10502
    "",// tur
#endif
  },
  { "Disable sleeptimer?",
		"Sleeptimer abbrechen?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Keskeytä ajastin",// Suomi
    "",// Polski
    "",// Espaqol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
    "",// cat
    "",// rus
    "",// hrv
#if VDRVERSNUM >= 10313
    "",// est
#endif
#if VDRVERSNUM >= 10316
    "",// dan
#endif
#if VDRVERSNUM >= 10342
    "",// cze
#endif
#if VDRVERSNUM >= 10502
    "",// tur
#endif
  },
  { "Timespan shorter than 2 minutes",
		"Zeitspanne kürzer 2 Minuten",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// suomi
    "",// Polski
    "",// Espaqol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
    "",// cat
    "",// rus
    "",// hrv
#if VDRVERSNUM >= 10313
    "",// est
#endif
#if VDRVERSNUM >= 10316
    "",// dan
#endif
#if VDRVERSNUM >= 10342
    "",// cze
#endif
#if VDRVERSNUM >= 10502
    "",// tur
#endif
  },  
  { "Activate sleeptimer at %i:%.2i?",
		"Sleeptimer um %i:%.2i aktivieren?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Aktivoi ajastin %i:%.2i?",// Suomi
    "",// Polski
    "",// Espaqol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
    "",// cat
    "",// rus
    "",// hrv
#if VDRVERSNUM >= 10313
    "",// est
#endif
#if VDRVERSNUM >= 10316
    "",// dan
#endif
#if VDRVERSNUM >= 10342
    "",// cze
#endif
#if VDRVERSNUM >= 10502
    "",// tur
#endif
  },
  { "Activate sleeptimer in %i minutes?",
		"Sleeptimer in %i Minuten aktivieren?",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Aktivoi %d minuutin ajastin?",// Suomi
    "",// Polski
    "",// Espaqol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
    "",// cat
    "",// rus
    "",// hrv
#if VDRVERSNUM >= 10313
    "",// est
#endif
#if VDRVERSNUM >= 10316
    "",// dan
#endif
#if VDRVERSNUM >= 10342
    "",// cze
#endif
#if VDRVERSNUM >= 10502
    "",// tur
#endif
  },
  { "Sleeptimer",
		"Sleeptimer",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ajastin",// Suomi
    "",// Polski
    "",// Espaqol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
    "",// cat
    "",// rus
    "",// hrv
#if VDRVERSNUM >= 10313
    "",// est
#endif
#if VDRVERSNUM >= 10316
    "",// dan
#endif
#if VDRVERSNUM >= 10342
    "",// cze
#endif
#if VDRVERSNUM >= 10502
    "",// tur
#endif
  },
  { "Action",
		"Aktion",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Toiminto",// Suomi
    "",// Polski
    "",// Espaqol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
    "",// cat
    "",// rus
    "",// hrv
#if VDRVERSNUM >= 10313
    "",// est
#endif
#if VDRVERSNUM >= 10316
    "",// dan
#endif
#if VDRVERSNUM >= 10342
    "",// cze
#endif
#if VDRVERSNUM >= 10502
    "",// tur
#endif
  },
  { "Going to sleep in about one minute",
		"Sleeptimer in einer Minute",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "Ajastin aktivoituu noin minuutin kuluttua",// Suomi
    "",// Polski
    "",// Espaqol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
    "",// cat
    "",// rus
    "",// hrv
#if VDRVERSNUM >= 10313
    "",// est
#endif
#if VDRVERSNUM >= 10316
    "",// dan
#endif
#if VDRVERSNUM >= 10342
    "",// cze
#endif
#if VDRVERSNUM >= 10502
    "",// tur
#endif
  },
  { "Default Timespan [min]",
		"Standard-Zeitspanne [min]",
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// TODO
    "",// suomi
    "",// Polski
    "",// Espaqol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
    "",// cat
    "",// rus
    "",// hrv
#if VDRVERSNUM >= 10313
    "",// est
#endif
#if VDRVERSNUM >= 10316
    "",// dan
#endif
#if VDRVERSNUM >= 10342
    "",// cze
#endif
#if VDRVERSNUM >= 10502
    "",// tur
#endif
  },
  { "Shutdown (time)",
	  "Shutdown (Uhrzeit)",
    "",
    "",
    "",
    "",
    "",
    "",
    "Sammutusaika", // Suomi
    "",// Polski
    "",// Espaqol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
    "",// cat
    "",// rus
    "",// hrv
#if VDRVERSNUM >= 10313
    "",// est
#endif
#if VDRVERSNUM >= 10316
    "",// dan
#endif
#if VDRVERSNUM >= 10342
    "",// cze
#endif
#if VDRVERSNUM >= 10502
    "",// tur
#endif
  },
  { "Shutdown [min]",
	  "Shutdown [min]",
    "",
    "",
    "",
    "",
    "",
    "",
    "Sammutus minuutteina",
    "",// Polski
    "",// Espaqol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
    "",// cat
    "",// rus
    "",// hrv
#if VDRVERSNUM >= 10313
    "",// est
#endif
#if VDRVERSNUM >= 10316
    "",// dan
#endif
#if VDRVERSNUM >= 10342
    "",// cze
#endif
#if VDRVERSNUM >= 10502
    "",// tur
#endif
  },


  { "Autoswitch",
	  "Automatisch",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",// Polski
    "",// Espaqol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
    "",// cat
    "",// rus
    "",// hrv
#if VDRVERSNUM >= 10313
    "",// est
#endif
#if VDRVERSNUM >= 10316
    "",// dan
#endif
#if VDRVERSNUM >= 10342
    "",// cze
#endif
#if VDRVERSNUM >= 10502
    "",// tur
#endif
  },

  { "Disable",
	  "Deaktivieren",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",// Polski
    "",// Espaqol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
    "",// cat
    "",// rus
    "",// hrv
#if VDRVERSNUM >= 10313
    "",// est
#endif
#if VDRVERSNUM >= 10316
    "",// dan
#endif
#if VDRVERSNUM >= 10342
    "",// cze
#endif
#if VDRVERSNUM >= 10502
    "",// tur
#endif
  },


  { "Shutdown",
	  "Herunterfahren",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",// Polski
    "",// Espaqol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
    "",// cat
    "",// rus
    "",// hrv
#if VDRVERSNUM >= 10313
    "",// est
#endif
#if VDRVERSNUM >= 10316
    "",// dan
#endif
#if VDRVERSNUM >= 10342
    "",// cze
#endif
#if VDRVERSNUM >= 10502
    "",// tur
#endif
  },


  { "Mute",
	  "Stummschalten",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",// Polski
    "",// Espaqol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
    "",// cat
    "",// rus
    "",// hrv
#if VDRVERSNUM >= 10313
    "",// est
#endif
#if VDRVERSNUM >= 10316
    "",// dan
#endif
#if VDRVERSNUM >= 10342
    "",// cze
#endif
#if VDRVERSNUM >= 10502
    "",// tur
#endif
  },


  { "Execute command",
	  "Befehl ausführen",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",// Polski
    "",// Espaqol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
    "",// cat
    "",// rus
    "",// hrv
#if VDRVERSNUM >= 10313
    "",// est
#endif
#if VDRVERSNUM >= 10316
    "",// dan
#endif
#if VDRVERSNUM >= 10342
    "",// cze
#endif
#if VDRVERSNUM >= 10502
    "",// tur
#endif
  },


  { "Command",
	  "Befehl",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",// Polski
    "",// Espaqol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
    "",// cat
    "",// rus
    "",// hrv
#if VDRVERSNUM >= 10313
    "",// est
#endif
#if VDRVERSNUM >= 10316
    "",// dan
#endif
#if VDRVERSNUM >= 10342
    "",// cze
#endif
#if VDRVERSNUM >= 10502
    "",// tur
#endif
  },


  { "Use default for autoswitch",
	  "Standard in Automatik",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",// Polski
    "",// Espaqol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
    "",// cat
    "",// rus
    "",// hrv
#if VDRVERSNUM >= 10313
    "",// est
#endif
#if VDRVERSNUM >= 10316
    "",// dan
#endif
#if VDRVERSNUM >= 10342
    "",// cze
#endif
#if VDRVERSNUM >= 10502
    "",// tur
#endif
  },


  { "Confirm",
	  "Bestätigen",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",// Polski
    "",// Espaqol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
    "",// cat
    "",// rus
    "",// hrv
#if VDRVERSNUM >= 10313
    "",// est
#endif
#if VDRVERSNUM >= 10316
    "",// dan
#endif
#if VDRVERSNUM >= 10342
    "",// cze
#endif
#if VDRVERSNUM >= 10502
    "",// tur
#endif
  },


  { "Sleeptimer in %i minute",
	  "Sleeptimer in %i Minute",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",// Polski
    "",// Espaqol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
    "",// cat
    "",// rus
    "",// hrv
#if VDRVERSNUM >= 10313
    "",// est
#endif
#if VDRVERSNUM >= 10316
    "",// dan
#endif
#if VDRVERSNUM >= 10342
    "",// cze
#endif
#if VDRVERSNUM >= 10502
    "",// tur
#endif
  },

  { "Sleeptimer in %i minutes",
	  "Sleeptimer in %i Minuten",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",// Polski
    "",// Espaqol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
    "",// cat
    "",// rus
    "",// hrv
#if VDRVERSNUM >= 10313
    "",// est
#endif
#if VDRVERSNUM >= 10316
    "",// dan
#endif
#if VDRVERSNUM >= 10342
    "",// cze
#endif
#if VDRVERSNUM >= 10502
    "",// tur
#endif
  },


  { "Sleeptimer disabled",
	  "Sleeptimer inaktiv",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",// Polski
    "",// Espaqol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
    "",// cat
    "",// rus
    "",// hrv
#if VDRVERSNUM >= 10313
    "",// est
#endif
#if VDRVERSNUM >= 10316
    "",// dan
#endif
#if VDRVERSNUM >= 10342
    "",// cze
#endif
#if VDRVERSNUM >= 10502
    "",// tur
#endif
  },


  { "Enable sleeptimer in %i minutes?",
	  "Sleeptimer in %i Minuten?",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",// Polski
    "",// Espaqol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
    "",// cat
    "",// rus
    "",// hrv
#if VDRVERSNUM >= 10313
    "",// est
#endif
#if VDRVERSNUM >= 10316
    "",// dan
#endif
#if VDRVERSNUM >= 10342
    "",// cze
#endif
#if VDRVERSNUM >= 10502
    "",// tur
#endif
  },


  { "Yes",
	  "Ja",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",// Polski
    "",// Espaqol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
    "",// cat
    "",// rus
    "",// hrv
#if VDRVERSNUM >= 10313
    "",// est
#endif
#if VDRVERSNUM >= 10316
    "",// dan
#endif
#if VDRVERSNUM >= 10342
    "",// cze
#endif
#if VDRVERSNUM >= 10502
    "",// tur
#endif
  },
  { "No",
	  "Nein",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",// Polski
    "",// Espaqol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
    "",// cat
    "",// rus
    "",// hrv
#if VDRVERSNUM >= 10313
    "",// est
#endif
#if VDRVERSNUM >= 10316
    "",// dan
#endif
#if VDRVERSNUM >= 10342
    "",// cze
#endif
#if VDRVERSNUM >= 10502
    "",// tur
#endif
  },
  { "Command",
	  "Befehl",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",// Polski
    "",// Espaqol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
    "",// cat
    "",// rus
    "",// hrv
#if VDRVERSNUM >= 10313
    "",// est
#endif
#if VDRVERSNUM >= 10316
    "",// dan
#endif
#if VDRVERSNUM >= 10342
    "",// cze
#endif
#if VDRVERSNUM >= 10502
    "",// tur
#endif
  },
  { "Execute command",
	  "Befehl ausführen",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",// Polski
    "",// Espaqol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
    "",// cat
    "",// rus
    "",// hrv
#if VDRVERSNUM >= 10313
    "",// est
#endif
#if VDRVERSNUM >= 10316
    "",// dan
#endif
#if VDRVERSNUM >= 10342
    "",// cze
#endif
#if VDRVERSNUM >= 10502
    "",// tur
#endif
  },
  { "Disable sleeptimer?",
	  "Sleeptimer deaktivieren?",
    "",
    "",
    "",
    "",
    "",
    "",
    "",
    "",// Polski
    "",// Espaqol
    "",// Greek
    "",// svenska
    "",// Romaneste
    "",// Magyar
    "",// cat
    "",// rus
    "",// hrv
#if VDRVERSNUM >= 10313
    "",// est
#endif
#if VDRVERSNUM >= 10316
    "",// dan
#endif
#if VDRVERSNUM >= 10342
    "",// cze
#endif
#if VDRVERSNUM >= 10502
    "",// tur
#endif
  },

  { NULL }
  };

#endif
