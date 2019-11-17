<?php
include('includes/kopf.php'); 

$id = safeget('id');

$zeile = exec("cat /var/lib/vdr/plugins/epgsearch/epgsearch.conf | grep ^$id:");
$DS_Array = explode(':', $zeile);


echo "<div id='kopf2'>Timer - epgsearch - View Suchtimer</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_timer.php'); 
echo "<br/>";
include('includes/subnavi_epgsearch.php'); 

echo "<h2>View Suchtimer</h2>";

// *************************************** Container Begin '' ****************************************************
container_begin(1, "Info.png", "View Suchtimer");

echo "<a href='suchtimer.php'><button>Suchtimerliste</button></a> <a href='suchtimer_edit.php?id=$id'><button>Edit Suchtimer</button></a> ";
echo "<a href='epgsearchdone.php?timer_id=$id'><button>epgsearchdone Einträge dieses Suchtimers</button></a><br/>";
echo "<br/>";
echo "<table class='tab1'>";
echo "<tr><td>Suchtimer ID</td><td>$DS_Array[0]</td></tr>"; // Einmalige Suchtimer ID
echo "<tr><td>Suche</td><td>$DS_Array[1]</td></tr>";        // Suchstring

switch ($DS_Array[8]) {
  case 0: $txt_suchmodus = "Ausdruck";        break;
  case 1: $txt_suchmodus = "alle Worte";      break;
  case 2: $txt_suchmodus = "ein Wort";        break;
  case 3: $txt_suchmodus = "exakt";           break;
  case 4: $txt_suchmodus = "regulärer Ausdruck"; break;
  case 5: $txt_suchmodus = "unscharf";        break;
  default: $txt_suchmodus = $DS_Array[8];     break;
}
echo "<tr><td>Suchmodus</td><td>$txt_suchmodus</td></tr>";
if ($DS_Array[8] == 5) {
  echo "<tr><td>&nbsp;&nbsp;&nbsp;Toleranz</td><td>$DS_Array[41]</td></tr>"; // Fuzzy Toleranz für Suche
}

if ($DS_Array[7] == 1) { $txt_gr_kl = "ja"; } else { $txt_gr_kl = "nein"; } 
echo "<tr><td>Groß/Klein</td><td>$txt_gr_kl</td></tr>";

if ($DS_Array[9] == 1) { $txt_titel = "ja"; } else { $txt_titel = "nein"; } 
echo "<tr><td>Verwende Titel</td><td>$txt_titel</td></tr>";

if ($DS_Array[10] == 1) { $txt_untertitel = "ja"; } else { $txt_untertitel = "nein"; } 
echo "<tr><td>Verwende Untertitel</td><td>$txt_untertitel</td></tr>";

if ($DS_Array[11] == 1) { $txt_beschreibung = "ja"; } else { $txt_beschreibung = "nein"; } 
echo "<tr><td>Verwende Bescheibung</td><td>$txt_beschreibung</td></tr>";


echo "<tr><td>Kennung Inhalt</td><td>$DS_Array[52]</td></tr>"; 


if ($DS_Array[26] == 1) { $txt_erw_epg = "ja"; } else { $txt_erw_epg = "nein"; } 
echo "<tr><td>Verwende erweiterte EPG Info</td><td>$txt_erw_epg</td></tr>";
if ($DS_Array[26] == 1) { 
  echo "<tr><td>&nbsp;&nbsp;&nbsp;Felder des erweiterten EPGs</td><td>$DS_Array[27]</td></tr>";
  if ($DS_Array[49] == 1) { $txt_ign_fehlende_kat = "ja"; } else { $txt_ign_fehlende_kat = "nein"; } 
  echo "<tr><td>&nbsp;&nbsp;&nbsp;Ignoriere fehlende Kategorie</td><td>$txt_ign_fehlende_kat</td></tr>";

}


switch ($DS_Array[5]) {
  case 0: $txt_kanal = "nein";        break; // no
  case 1: $txt_kanal = "Bereich";     break; // Interval
  case 2: $txt_kanal = "Kanalgruppe"; break; // Channel group
  case 3: $txt_kanal = "ohne PayTV";  break; // FTA only
  default: $txt_kanal = $DS_Array[5]; break;
}
echo "<tr><td>Verwende Kanal</td><td>$txt_kanal</td></tr>";
if ($DS_Array[5] == 1) {
  echo "<tr><td>&nbsp;&nbsp;&nbsp;KanalID</td><td>$DS_Array[6]</td></tr>";
}
if ($DS_Array[5] == 2) {
  echo "<tr><td>&nbsp;&nbsp;&nbsp;Kanalgruppe</td><td><a href='epgsearch_changrps.php?name='".$DS_Array[6]."'>$DS_Array[6]</a></td></tr>";
}


// Verwende Zeit: 0 = nein, 1= ja
if ($DS_Array[2] == 1) { $txt_zeit = "ja"; } else { $txt_zeit = "nein"; } 
echo "<tr><td>Verwende Uhrzeit</td><td>$txt_zeit</td></tr>";
if ($DS_Array[2] == 1) {
  $txt_start = substr ($DS_Array[3],0,2) .":". substr ($DS_Array[3],2,2);
  $txt_stop  = substr ($DS_Array[4],0,2) .":". substr ($DS_Array[4],2,2);
  echo "<tr><td>&nbsp;&nbsp;&nbsp;Start nach</td><td>$txt_start</td></tr>";
  echo "<tr><td>&nbsp;&nbsp;&nbsp;Start vor</td> <td>$txt_stop</td></tr>";
}
 
echo "<tr><td>Verwende Dauer</td><td>".$DS_Array[12]."</td></tr>";
 
if ($DS_Array[12] == 1) {
  $txt_min_dauer = substr ($DS_Array[13],0,2) .":". substr ($DS_Array[13],2,2);
  $txt_max_dauer = substr ($DS_Array[14],0,2) .":". substr ($DS_Array[14],2,2);
  echo "<tr><td>&nbsp;&nbsp;&nbsp;Minimale Dauer</td><td>$txt_min_dauer</td></tr>"; // Minimale Länge der Sendung in HHMM
  echo "<tr><td>&nbsp;&nbsp;&nbsp;Maximale Dauer</td><td>$txt_max_dauer</td></tr>"; // Maximale Länge der Sendung in HHMM
}

if ($DS_Array[16] == 1) { $txt_verwende_wt = "ja"; } else { $txt_verwende_wt = "nein"; } 
echo "<tr><td>Verwende Wochentag</td><td>$txt_verwende_wt</td></tr>";
if ($DS_Array[16] == 1) {
  switch ($DS_Array[17]) {
    case 0: $txt_wt = "Sontag";     break;
    case 1: $txt_wt = "Montag";     break;
    case 2: $txt_wt = "Dienstag";   break;
    case 3: $txt_wt = "Mittwoch";   break;
    case 4: $txt_wt = "Donnerstag"; break;
    case 5: $txt_wt = "Freitag";    break;
    case 6: $txt_wt = "Samstag";    break;
    default: $txt_wt = $DS_Array[17];    break;
  }
  echo "<tr><td>&nbsp;&nbsp;&nbsp;Tag der Woche</td><td>$txt_wt</td></tr>";
}

switch ($DS_Array[39]) {
  case 0: $txt_verwende_ausschlusliste = "nur globale";  break;
  case 1: $txt_verwende_ausschlusliste = "Auswahl";      break;
  case 2: $txt_verwende_ausschlusliste = "alle";         break;
  case 3: $txt_verwende_ausschlusliste = "keine";        break;
  default: $txt_verwende_ausschlusliste = $DS_Array[39]; break;
}
echo "<tr><td>Verwende Ausschlussliste</td><td>$txt_verwende_ausschlusliste</td></tr>"; // Modus der Ausschlussliste
if ($DS_Array[39] == 1) { 
  echo "<tr><td>&nbsp;&nbsp;&nbsp;Verwende diese Ausschluss-Suchtimer (IDs)</td><td>$DS_Array[40]</td></tr>";
}


switch ($DS_Array[15]) {
  case 0: $txt_verwende_st = "nein"; break;
  case 1: $txt_verwende_st = "ja";   break;
  case 2: $txt_verwende_st = "benutzerdefiniert";     break;
  default: $txt_verwende_st = $DS_Array[15];  break;
}
echo "<tr><td>Als Suchtimer verwenden</td><td>$txt_verwende_st</td></tr>";
if ($DS_Array[15] == 2) {
  $datum_erster_tag = date("d.m.Y",$DS_Array[47]);
  $datum_letzter_tag = date("d.m.Y",$DS_Array[48]);
  echo "<tr><td>&nbsp;&nbsp;&nbsp;Aktivierung des Suchtimers: Erster Tag</td><td>$datum_erster_tag</td></tr>";
  echo "<tr><td>&nbsp;&nbsp;&nbsp;Aktivierung des Suchtimers: Letzter Tag</td><td>$datum_letzter_tag</td></tr>";
}
if (($DS_Array[15] == 1) or ($DS_Array[15] == 2)){ // Wenn als Suchtimer verwenden = ja
  switch ($DS_Array[25]) {
    case 0: $txt_aktion = "Aufnehmen";                 break;
    case 1: $txt_aktion = "per OSD ankündigen";        break;
    case 2: $txt_aktion = "Nur umschalten";            break;
    case 3: $txt_aktion = "Ankündigen und Umschalten"; break;
    case 4: $txt_aktion = "per Mail ankündigen";       break;
    default: $txt_aktion = $DS_Array[25];              break;
  }
  echo "<tr><td>Aktion</td><td>$txt_aktion</td></tr>";
  if ($DS_Array[25] == 0) {  // Wenn Aktion = Aufnehmen
    if ($DS_Array[18] == 1) { $txt_verwende_serie = "ja"; } else { $txt_verwende_serie = "nein"; } 
    echo "<tr><td>Serienaufnahme</td><td>$txt_verwende_serie</td></tr>";
    echo "<tr><td>Verzeichnis</td><td>$DS_Array[19]</td></tr>";
    echo "<tr><td>Aufnahme nach ... Tagen löschen</td><td>$DS_Array[35]</td></tr>"; // Lösche eine Aufnahme automatisch nach x Tagen
    if ($DS_Array[35] > 0) {
      echo "<tr><td>&nbsp;&nbsp;&nbsp;Behalte ... Aufnahmen</td><td>$DS_Array[36]</td></tr>"; // Aber behalte mindestens x Aufnahmen
    }
    echo "<tr><td>Pause, wenn ... Aufnahmen existieren</td><td>$DS_Array[38]</td></tr>"; // Pausiere das Anlegene von Timern wenn x Aufnahmen vorhanden sind

    if ($DS_Array[28] == 1) { $txt_vermeide_wiederholungen = "ja"; } else { $txt_vermeide_wiederholungen = "nein"; } 
    echo "<tr><td>Vermeide Wiederholung</td><td>$txt_vermeide_wiederholungen</td></tr>";
    if ($DS_Array[28] == 1) { // Vermeide Wiederholung = ja
      echo "<tr><td>&nbsp;&nbsp;&nbsp;Erlaubte Wiederholungen</td><td>$DS_Array[29]</td></tr>"; // erlaubte Anzahl Wiederholungen
      if ($DS_Array[29] > 0) {
        echo "<tr><td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Nur Wiederholungen innerhalb ... Tagen</td><td>$DS_Array[34]</td></tr>"; // Erlaube Wiederholungen nur innerhalb x Tagen
      }
      if ($DS_Array[30] == 1) { $txt_vergleiche_titel = "ja"; } else { $txt_vergleiche_titel = "nein"; } 
      echo "<tr><td>&nbsp;&nbsp;&nbsp;Vergleiche Titel</td><td>$txt_vergleiche_titel</td></tr>"; // Vergleiche Titel bei Prüfung auf Wiederholung? 0/1
      if ($DS_Array[31] == 1) { $txt_vergleiche_untertitel = "wenn vorhanden"; } else { $txt_vergleiche_untertitel = "nein"; } 
      echo "<tr><td>&nbsp;&nbsp;&nbsp;Vergleiche Untertitel</td><td>$txt_vergleiche_untertitel</td></tr>"; // Vergleiche Untertitel bei Prüfung auf Wiederholung? 0/1
      if ($DS_Array[32] == 1) { $txt_vergleiche_beschreibung = "ja"; } else { $txt_vergleiche_beschreibung = "nein"; } 
      echo "<tr><td>&nbsp;&nbsp;&nbsp;Vergleiche Beschreibung</td><td>$txt_vergleiche_beschreibung</td></tr>"; // Vergleiche Beschreibung bei Prüfung auf Wiederholung? 0/1
      if ($DS_Array[32] == 1) {
        echo "<tr><td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Min. Übereinstimmung in %</td><td>$DS_Array[51]</td></tr>";
      }
      
      echo "<tr><td>&nbsp;&nbsp;&nbsp;Kennung für Inhalt</td><td>$DS_Array[52]</td></tr>";
      
      switch ($DS_Array[53]) {
        case 0: $txt_vergleiche_zeitpunkt = "nein";            break;
        case 1: $txt_vergleiche_zeitpunkt = "gleicher Tag";    break;
        case 2: $txt_vergleiche_zeitpunkt = "gleiche Woche";   break;
        case 3: $txt_vergleiche_zeitpunkt = "gleicher Monat";  break;
        default: $txt_vergleiche_zeitpunkt = $DS_Array[53];    break;
      }
      echo "<tr><td>&nbsp;&nbsp;&nbsp;Vergleiche Zeitpunkt</td><td>$txt_vergleiche_zeitpunkt</td></tr>";
    }
    
    echo "<tr><td>Priorität</td><td>$DS_Array[20]</td></tr>";
    echo "<tr><td>Lebensdauer</td><td>$DS_Array[21]</td></tr>";
    echo "<tr><td>Vorlauf zum Timer-Beginn (min)</td><td>$DS_Array[22]</td></tr>";
    echo "<tr><td>Nachlauf am Timer-Ende (min)</td><td>$DS_Array[23]</td></tr>";

    if ($DS_Array[24] == 1) { $txt_vps = "ja"; } else { $txt_vps = "nein"; } 
    echo "<tr><td>VPS</td><td>$txt_vps</td></tr>"; // Verwende VPS?

    switch ($DS_Array[44]) {
      case 0: $txt_auto_del = "nein";              break;
      case 1: $txt_auto_del = "Anzahl Aufnahmen";  break;
      case 2: $txt_auto_del = "Anzahl Tage";       break;
      default: $txt_auto_del = $DS_Array[44];      break;
    }
    echo "<tr><td>automatisch löschen</td><td>$txt_auto_del</td></tr>";
    if ($DS_Array[44] == 1) {
      echo "<tr><td>&nbsp;&nbsp;&nbsp;nach ... Aufnahmen</td><td>$DS_Array[45]</td></tr>";
    }
    if ($DS_Array[44] == 2) {
      echo "<tr><td>&nbsp;&nbsp;&nbsp;nach ... Tagen nach erster Aufnahme</td><td>$DS_Array[46]</td></tr>";
    }
  }
  
  if ($DS_Array[25] == 2) {  // Wenn Aktion = Nur umschalten
    echo "<tr><td>Umschalten ... Minuten vor Start</td><td>$DS_Array[37]</td></tr>"; // Schalte x Minuten vor der Sendung um, wenn Aktion = 2
    if ($DS_Array[50] == 1) { $txt_ton_an = "ja"; } else { $txt_ton_an = "nein"; } 
    echo "<tr><td>Ton anschalten</td><td>$txt_ton_an</td></tr>";
  }
  
  if ($DS_Array[25] == 3) {  // Wenn Aktion = Ankündigen und umschalten
    echo "<tr><td>Nachfrage ... Minuten vor Start</td><td>$DS_Array[37]</td></tr>"; // Schalte x Minuten vor der Sendung um, wenn Aktion = 2
    if ($DS_Array[50] == 1) { $txt_ton_an = "ja"; } else { $txt_ton_an = "nein"; } 
    echo "<tr><td>Ton anschalten</td><td>$txt_ton_an</td></tr>";
  }

}

/*
echo "<tr><td>--------------</td><td>---------------</td></tr>";
echo "<tr><td>33 Vergleiche erweitertes EPG bei Prüfung auf Wiederholung?</td><td>$DS_Array[33]</td></tr>";
echo "<tr><td>42 Verwende diese Suche im Favoriten Menü</td><td>$DS_Array[42]</td></tr>";
echo "<tr><td>43 ID einer Menüvorlage für das Suchergebnis</td><td>$DS_Array[43]</td></tr>";
echo "<tr><td>53</td><td>$DS_Array[53]</td></tr>";
*/
echo "</table>";


container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
