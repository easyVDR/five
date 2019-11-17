<?php
include('includes/kopf.php'); 

$txt_yes = "ja";
$txt_no  = "nein";

$id = safeget('id');
if ($id!="") {
  $zeile = exec("cat /var/lib/vdr/plugins/epgsearch/epgsearch.conf | grep ^$id:");
  $DS_Array = explode(':', $zeile);
  //$id         = $DS_Array[0];
  $suchstring      = $DS_Array[1];
  $verwende_zeit   = $DS_Array[2];
  $start           = $DS_Array[3];
  $stop            = $DS_Array[4];
  $verwende_kanal  = $DS_Array[5];
  $kanal_id        = $DS_Array[6];
  $gross_klein     = $DS_Array[7];
  $suchmodus       = $DS_Array[8];
  $titel_yn        = $DS_Array[9];
  $untertitel_yn   = $DS_Array[10];
  $beschreibung_yn = $DS_Array[11];
  $dauer_yn        = $DS_Array[12];
  $min_dauer       = $DS_Array[13];
  $max_dauer       = $DS_Array[14];
  $verwende_st     = $DS_Array[15];
  $verwende_wt     = $DS_Array[16];
  $wochentag       = $DS_Array[17];
  $verwende_serie  = $DS_Array[18];
  $verzeichnis     = $DS_Array[19];
  $prioritaet      = $DS_Array[20];
  $lebensdauer     = $DS_Array[21];
  $vorlauf_begin   = $DS_Array[22];
  $vorlauf_end     = $DS_Array[23];
  $vps             = $DS_Array[24];
  $aktion          = $DS_Array[25];
  $erw_epg_yn      = $DS_Array[26];
  $erw_epg_felder  = $DS_Array[27]; 
  $vermeide_wiederholungen = $DS_Array[28]; 
  $anz_wiederholungen      = $DS_Array[29]; 
  $vergleiche_titel_yn     = $DS_Array[30];
  $vergleiche_untertitel_yn = $DS_Array[31];
  $vergleiche_beschreibung_yn = $DS_Array[32];
  $wiederholung_x_tage     = $DS_Array[34]; 
  $del_x_day               = $DS_Array[35]; 
  $behalte_x_aufnahmen     = $DS_Array[36]; 
  $aktion_x_min_vor_start  = $DS_Array[37]; 
  $pause_wenn_x_aufnahmen  = $DS_Array[38];
  $verwende_ausschlusliste = $DS_Array[39];
  $ausschluss_ids          = $DS_Array[40];
  $fuzzy_toleranz          = $DS_Array[41]; // Fuzzy Toleranz für Suche
  $auto_del                = $DS_Array[44]; 
  $del_nach_x_aufnahmen    = $DS_Array[45];
  $del_nach_x_tagen        = $DS_Array[46];
  $datum_erster_tag        = $DS_Array[47]; 
  $datum_letzter_tag       = $DS_Array[48]; 
  $ign_fehlende_kat_yn     = $DS_Array[49];
  $ton_an                  = $DS_Array[50];
  $min_uebereinstimmung    = $DS_Array[51];
  $kennung_inhalt          = $DS_Array[52];
  $vergleiche_zeitpunkt    = $DS_Array[53];
}
else {
  $id              = safepost('id');
  $suchstring      = safepost('suchstring');
  $verwende_zeit   = safepost('verwende_zeit');
  $start           = safepost('start');
  $stop            = safepost('stop');
  $verwende_kanal  = safepost('verwende_kanal');
  $kanal_id        = safepost('kanal_id');
  $gross_klein     = safepost('gross_klein');
  $suchmodus       = safepost('suchmodus');
  $titel_yn        = safepost('titel_yn');
  $untertitel_yn   = safepost('untertitel_yn');
  $beschreibung_yn = safepost('beschreibung_yn');
  $dauer_yn        = safepost('dauer_yn');
  $min_dauer       = safepost('min_dauer');
  $max_dauer       = safepost('max_dauer');
  $verwende_st     = safepost('verwende_st');
  $verwende_wt     = safepost('verwende_wt');
  $wochentag       = safepost('wochentag');
  $verwende_serie  = safepost('verwende_serie');
  $verzeichnis     = safepost('verzeichnis');
  $prioritaet      = safepost('prioritaet');
  $lebensdauer     = safepost('lebensdauer');
  $vorlauf_begin   = safepost('vorlauf_begin');
  $vorlauf_end     = safepost('vorlauf_end');
  $vps             = safepost('vps');
  $aktion          = safepost('aktion');
  $erw_epg_yn      = safepost('erw_epg_yn');
  $erw_epg_felder  = safepost('erw_epg_felder');
  $vermeide_wiederholungen = safepost('vermeide_wiederholungen');
  $anz_wiederholungen      = safepost('anz_wiederholungen');
  $vergleiche_titel_yn     = safepost('vergleiche_titel_yn');
  $vergleiche_untertitel_yn = safepost('vergleiche_untertitel_yn');
  $vergleiche_beschreibung_yn = safepost('vergleiche_beschreibung_yn');
  $wiederholung_x_tage     = safepost('wiederholung_x_tage');
  $del_x_day               = safepost('del_x_day');
  $behalte_x_aufnahmen     = safepost('behalte_x_aufnahmen');
  $aktion_x_min_vor_start  = safepost('aktion_x_min_vor_start');
  $pause_wenn_x_aufnahmen  = safepost('pause_wenn_x_aufnahmen');
  $verwende_ausschlusliste = safepost('verwende_ausschlusliste');
  $ausschluss_ids          = safepost('ausschluss_ids');
  $fuzzy_toleranz          = safepost('fuzzy_toleranz');
  $auto_del                = safepost('auto_del');
  $del_nach_x_aufnahmen    = safepost('del_nach_x_aufnahmen');
  $del_nach_x_tagen        = safepost('del_nach_x_tagen');
  $datum_erster_tag        = safepost('datum_erster_tag');
  $datum_letzter_tag       = safepost('datum_letzter_tag');
  $ign_fehlende_kat_yn     = safepost('ign_fehlende_kat_yn');
  $ton_an                  = safepost('ton_an');
  $min_uebereinstimmung    = safepost('min_uebereinstimmung');
  $kennung_inhalt          = safepost('kennung_inhalt');
  $vergleiche_zeitpunkt    = safepost('vergleiche_zeitpunkt');
}

if ($del_x_day == "")              {$del_x_day = 0;}
if ($pause_wenn_x_aufnahmen == "") {$pause_wenn_x_aufnahmen = 0;}
if ($prioritaet == "")             {$prioritaet = 50;}
if ($lebensdauer == "")            {$lebensdauer = 99;}
if ($vorlauf_begin == "")          {$vorlauf_begin = 5;}
if ($vorlauf_end == "")            {$vorlauf_end = 10;}
if ($del_nach_x_aufnahmen == "")   {$del_nach_x_aufnahmen = 0;}
if ($del_nach_x_tagen == "")       {$del_nach_x_tagen = 0;}

?>
<style type="text/css">
  #tab td { height: 20px; }
</style>
<?php

echo "<div id='kopf2'>Timer - epgsearch - Edit Suchtimer</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_timer.php'); 
echo "<br/>";
include('includes/subnavi_epgsearch.php'); 

echo "<h2>Edit Suchtimer</h2>";

// *************************************** Container Begin '' ****************************************************
container_begin(1, "Info.png", "Edit Suchtimer");

echo "<a href='suchtimer.php'><button>Suchtimerliste</button></a> <a href='suchtimer_view.php?id=$id'><button>View Suchtimer</button></a><br/>";
echo "<br/>";

echo "$zeile<br/><br/>";


echo "<form action='".$_SERVER['PHP_SELF']."' method='post'>";
echo "<input type='hidden' name='id' value='$id'>";

echo "<table id='tab' class='tab1'>";
echo "<tr><td>Suchtimer ID</td><td>$id</td></tr>"; // Einmalige Suchtimer ID
echo "<tr><td>Suche</td><td><input type='text' name='suchstring' size='40' value='$suchstring'></td></tr>";        // Suchstring

echo "<tr><td>Suchmodus</td><td><select name='suchmodus' size='1' style='width: 240px' onChange='this.form.submit()'>";
echo "          <option value=0 ";
if ($suchmodus == "0"){echo " selected";}
echo                  ">Ausdruck</option>";
echo "            <option value=1 ";
if ($suchmodus == "1"){echo " selected";}
echo                  ">alle Worte</option>";
echo "            <option value=2 ";
if ($suchmodus == "2"){echo " selected";}
echo                  ">ein Wort</option>";
echo "            <option value=3 ";
if ($suchmodus == "3"){echo " selected";}
echo                  ">exakt</option>";
echo "            <option value=4 ";
if ($suchmodus == "4"){echo " selected";}
echo                  ">regulärer Ausdruck</option>";
echo "            <option value=5 ";
if ($suchmodus == "5"){echo " selected";}
echo                  ">unscharf</option>";
echo "         </select></td></tr>";

if ($suchmodus == 5) { echo "<tr><td>&nbsp;&nbsp;&nbsp;Toleranz</td><td><input type='text' name='fuzzy_toleranz' size='40' value='$fuzzy_toleranz'></td></tr>"; } // Fuzzy Toleranz für Suche
else { echo "<input type='hidden' name='fuzzy_toleranz' value=''>"; }

echo "<tr><td>groß/klein</td>";
if ($gross_klein == "1") {$checked_y = " checked"; $checked_n = ""; } else {$checked_y = ""; $checked_n = " checked";}
echo "<td>$txt_yes <input type='radio' name='gross_klein' value='1' ".$checked_y.">&nbsp;&nbsp; ";
echo "    $txt_no <input type='radio' name='gross_klein' value='0' ".$checked_n."></td>";
echo "</tr>";

echo "<tr><td>Verwende Titel</td>";
if ($titel_yn == "1") {$checked_y = " checked"; $checked_n = ""; } else {$checked_y = ""; $checked_n = " checked";}
echo "<td>$txt_yes <input type='radio' name='titel_yn' value='1' ".$checked_y.">&nbsp;&nbsp; ";
echo "    $txt_no <input type='radio' name='titel_yn' value='0' ".$checked_n."></td>";
echo "</tr>";

echo "<tr><td>Verwende Untertitel</td>";
if ($untertitel_yn == "1") {$checked_y = " checked"; $checked_n = ""; } else {$checked_y = ""; $checked_n = " checked";}
echo "<td>$txt_yes <input type='radio' name='untertitel_yn' value='1' ".$checked_y.">&nbsp;&nbsp; ";
echo "    $txt_no <input type='radio' name='untertitel_yn' value='0' ".$checked_n."></td>";
echo "</tr>";

echo "<tr><td>Verwende Bescheibung</td>";
if ($beschreibung_yn == "1") {$checked_y = " checked"; $checked_n = ""; } else {$checked_y = ""; $checked_n = " checked";}
echo "<td>$txt_yes <input type='radio' name='beschreibung_yn' value='1' ".$checked_y.">&nbsp;&nbsp; ";
echo "    $txt_no <input type='radio' name='beschreibung_yn' value='0' ".$checked_n."></td>";
echo "</tr>";



echo "<tr><td>Kennung für Inhalt</td><td><input type='text' name='kennung_inhalt' size='40' value='$kennung_inhalt'></td></tr>";



echo "<tr><td>Verwende erweiterte EPG Info</td>";
if ($erw_epg_yn == "1") {$checked_y = " checked"; $checked_n = ""; } else {$checked_y = ""; $checked_n = " checked";}
echo "<td>$txt_yes <input type='radio' name='erw_epg_yn' value='1' ".$checked_y." onChange='this.form.submit()'>&nbsp;&nbsp; ";
echo "      $txt_no <input type='radio' name='erw_epg_yn' value='0' ".$checked_n." onChange='this.form.submit()' ></td>";
echo "</tr>";

if ($erw_epg_yn == 1) { 
  echo "<tr><td>&nbsp;&nbsp;&nbsp;Felder des erweiterten EPGs</td><td><input type='text' name='erw_epg_felder' size='40' value='$erw_epg_felder'></td></tr>";
 
  echo "<tr><td>&nbsp;&nbsp;&nbsp;Ignoriere fehlende Kategorie</td>";
  if ($ign_fehlende_kat_yn == "1") {$checked_y = " checked"; $checked_n = ""; } else {$checked_y = ""; $checked_n = " checked";}
  echo "<td>$txt_yes <input type='radio' name='ign_fehlende_kat_yn' value='1' ".$checked_y." onChange='this.form.submit()'>&nbsp;&nbsp; ";
  echo "      $txt_no <input type='radio' name='ign_fehlende_kat_yn' value='0' ".$checked_n." onChange='this.form.submit()' ></td>";
  echo "</tr>";
}

echo "<tr><td>Verwende Kanal</td><td><select name='verwende_kanal' size='1' style='width: 240px' onChange='this.form.submit()'>";
echo "          <option value=0 ";
if ($verwende_kanal == "0"){echo " selected";}
echo                  ">nein</option>";
echo "            <option value=1 ";
if ($verwende_kanal == "1"){echo " selected";}
echo                  ">Bereich</option>";
echo "            <option value=2 ";
if ($verwende_kanal == "2"){echo " selected";}
echo                  ">Kanalgruppe</option>";
echo "            <option value=3 ";
if ($verwende_kanal == "3"){echo " selected";}
echo                  ">ohne PayTV</option>";
echo "         </select></td></tr>";

if ($verwende_kanal == 1) {
  echo "<tr><td>&nbsp;&nbsp;&nbsp;KanalID</td><td><input type='text' name='kanal_id' size='40' value='$kanal_id'></td></tr>";
}
if ($verwende_kanal == 2) {
  echo "<tr><td>&nbsp;&nbsp;&nbsp;Kanalgruppe</td><td><select name='kanal_id' size='1' style='width: 240px'>";
  $datei=file("/var/lib/vdr/plugins/epgsearch/epgsearchchangrps.conf");
  //unset($inhalt,$eintrage);
  for ($i=0;$i<count($datei)+1;$i++){ 
    $zeile = $datei[$i];
    if ($zeile != "") {
      $eintrage = explode('|',$zeile);
      echo "<option value=".$eintrage[0];
      if ($eintrage[0] == $kanal_id){echo " selected";}
      echo ">".$eintrage[0]."</option>";
    }
  }
  echo "</select>&nbsp;<a onmouseover=\"Tip('Kanalgruppen editieren')\" href='epgsearch_changrps.php'>&nbsp;<img src=/images/icon/edit.png>&nbsp;</a></td></tr>";
}

echo "<tr><td>Verwende Uhrzeit</td>";
if ($verwende_zeit == "1") {$checked_y = " checked"; $checked_n = ""; } else {$checked_y = ""; $checked_n = " checked";}
echo "<td>$txt_yes <input type='radio' name='verwende_zeit' value='1' ".$checked_y." onChange='this.form.submit()'>&nbsp;&nbsp; ";
echo "      $txt_no <input type='radio' name='verwende_zeit' value='0' ".$checked_n." onChange='this.form.submit()' ></td>";
echo "</tr>";

if ($verwende_zeit == 1) {
  //$txt_start = substr ($start,0,2) .":". substr ($start,2,2);
  //$txt_stop  = substr ($stop,0,2) .":". substr ($stop,2,2);
  echo "<tr><td>&nbsp;&nbsp;&nbsp;Start nach</td><td><input type='text' name='start' size='40' value='$start'></td></tr>";
  echo "<tr><td>&nbsp;&nbsp;&nbsp;Start vor</td> <td><input type='text' name='stop' size='40' value='$stop'></td></tr>";
}

echo "<tr><td>Verwende Dauer</td>";
if ($dauer_yn == "1") {$checked_y = " checked"; $checked_n = ""; } else {$checked_y = ""; $checked_n = " checked";}
echo "<td>$txt_yes <input type='radio' name='dauer_yn' value='1' ".$checked_y." onChange='this.form.submit()'>&nbsp;&nbsp; ";
echo "      $txt_no <input type='radio' name='dauer_yn' value='0' ".$checked_n." onChange='this.form.submit()' ></td>";
echo "</tr>";

if ($dauer_yn == 1) {
  //$txt_min_dauer = substr ($min_dauer,0,2) .":". substr ($min_dauer,2,2);
  //$txt_max_dauer = substr ($max_dauer,0,2) .":". substr ($max_dauer,2,2);
  echo "<tr><td>&nbsp;&nbsp;&nbsp;Minimale Dauer</td><td><input type='text' name='min_dauer' size='40' value='$min_dauer'></td></tr>"; // Minimale Länge der Sendung in HHMM
  echo "<tr><td>&nbsp;&nbsp;&nbsp;Maximale Dauer</td><td><input type='text' name='max_dauer' size='40' value='$max_dauer'></td></tr>"; // Maximale Länge der Sendung in HHMM
}

echo "<tr><td>Verwende Wochentag</td>";
if ($verwende_wt == "1") {$checked_y = " checked"; $checked_n = ""; } else {$checked_y = ""; $checked_n = " checked";}
echo "<td>$txt_yes <input type='radio' name='verwende_wt' value='1' ".$checked_y." onChange='this.form.submit()'>&nbsp;&nbsp; ";
echo "      $txt_no <input type='radio' name='verwende_wt' value='0' ".$checked_n." onChange='this.form.submit()' ></td>";
echo "</tr>";


if ($verwende_wt == 1) {
  echo "<tr><td>&nbsp;&nbsp;&nbsp;Tag der Woche</td><td><select name='wochentag' size='1' style='width: 240px' onChange='this.form.submit()'>";
  echo "          <option value=0 ";
  if ($wochentag == "0"){echo " selected";}
  echo                  ">Sontag</option>";
  echo "            <option value=1 ";
  if ($wochentag == "1"){echo " selected";}
  echo                  ">Montag</option>";
  echo "            <option value=2 ";
  if ($wochentag == "2"){echo " selected";}
  echo                  ">Dienstag</option>";
  echo "            <option value=3 ";
  if ($wochentag == "3"){echo " selected";}
  echo                  ">Mittwoch</option>";
  echo "            <option value=4 ";
  if ($wochentag == "4"){echo " selected";}
  echo                  ">Donnerstag</option>";
  echo "            <option value=5 ";
  if ($wochentag == "5"){echo " selected";}
  echo                  ">Freitag</option>";
  echo "            <option value=6 ";
  if ($wochentag == "6"){echo " selected";}
  echo                  ">Samstag</option>";
  //if ($wochentag == "") { $wt_nr = -99; } else { $wt_nr = $wochentag; } 
  echo "            <option value=-99 ";
  if ($wochentag < "0"){echo " selected";}
  echo                  ">benutzerdefiniert</option>";
  echo "         </select></td></tr>";
}

if (($verwende_wt == 1) and ($wochentag < "0")) {
  echo "<tr><td>&nbsp;&nbsp;&nbsp;Auswahl Tage</td><td>";
  echo "Mo <input type='checkbox' name='wt_mo'> "; 
  echo "Di <input type='checkbox' name='wt_di'> ";
  echo "Mi <input type='checkbox' name='wt_mi'> ";
  echo "Do <input type='checkbox' name='wt_do'> ";
  echo "Fr <input type='checkbox' name='wt_fr'> "; 
  echo "Sa <input type='checkbox' name='wt_sa'> ";
  echo "So <input type='checkbox' name='wt_so'> ";
  echo "</td></tr>";

}



echo "<tr><td>Verwende Ausschlussliste</td><td><select name='verwende_ausschlusliste' size='1' style='width: 240px' onChange='this.form.submit()'>";
echo "          <option value=0 ";
if ($verwende_ausschlusliste == "0"){echo " selected";}
echo                  ">nur globale</option>";
echo "            <option value=1 ";
if ($verwende_ausschlusliste == "1"){echo " selected";}
echo                  ">Auswahl</option>";
echo "            <option value=2 ";
if ($verwende_ausschlusliste == "2"){echo " selected";}
echo                  ">alle</option>";
echo "            <option value=3 ";
if ($verwende_ausschlusliste == "3"){echo " selected";}
echo                  ">keine</option>";
echo "         </select></td></tr>";

if ($verwende_ausschlusliste == 1) { 
  echo "<tr><td>&nbsp;&nbsp;&nbsp;Verwende diese Ausschluss-Suchtimer (IDs)</td><td><input type='text' name='ausschluss_ids' size='40' value='$ausschluss_ids'></td></tr>";
}

echo "<tr><td>Als Suchtimer verwenden</td><td><select name='verwende_st' size='1' style='width: 240px' onChange='this.form.submit()'>";
echo "          <option value=0 ";
if ($verwende_st == "0"){echo " selected";}
echo                  ">nein</option>";
echo "            <option value=1 ";
if ($verwende_st == "1"){echo " selected";}
echo                  ">ja</option>";
echo "            <option value=2 ";
if ($verwende_st == "2"){echo " selected";}
echo                  ">benutzerdefiniert</option>";
echo "         </select></td></tr>";

if ($verwende_st == 2) {
  $datum_erster_tag = date("d.m.Y",$datum_erster_tag);
  $datum_letzter_tag = date("d.m.Y",$datum_letzter_tag);
  echo "<tr><td>&nbsp;&nbsp;&nbsp;Aktivierung des Suchtimers: Erster Tag</td><td><input type='text' name='datum_erster_tag' size='40' value='$datum_erster_tag'></td></tr>";
  echo "<tr><td>&nbsp;&nbsp;&nbsp;Aktivierung des Suchtimers: Letzter Tag</td><td><input type='text' name='datum_letzter_tag' size='40' value='$datum_letzter_tag'></td></tr>";
}
if (($verwende_st == 1) or ($verwende_st == 2)){ // Wenn als Suchtimer verwenden = ja

  echo "<tr><td>Aktion</td><td><select name='aktion' size='1' style='width: 240px' onChange='this.form.submit()'>";
  echo "          <option value=0 ";
  if ($aktion == "0"){echo " selected";}
  echo                  ">Aufnehmen</option>";
  echo "            <option value=1 ";
  if ($aktion == "1"){echo " selected";}
  echo                  ">per OSD ankündigen</option>";
  echo "            <option value=2 ";
  if ($aktion == "2"){echo " selected";}
  echo                  ">Nur umschalten</option>";
  echo "            <option value=3 ";
  if ($aktion == "3"){echo " selected";}
  echo                  ">Ankündigen und Umschalten</option>";
  echo "            <option value=4 ";
  if ($aktion == "4"){echo " selected";}
  echo                  ">per Mail ankündigen</option>";
  echo "         </select></td></tr>";

  if ($aktion == 0) {  // Wenn Aktion = Aufnehmen

    echo "<tr><td>Serienaufnahme</td>";
    if ($verwende_serie == "1") {$checked_y = " checked"; $checked_n = ""; } else {$checked_y = ""; $checked_n = " checked";}
    echo "<td>$txt_yes <input type='radio' name='verwende_serie' value='1' ".$checked_y." >&nbsp;&nbsp; ";
    echo "      $txt_no <input type='radio' name='verwende_serie' value='0' ".$checked_n." ></td>";
    echo "</tr>";
    
    echo "<tr><td>Verzeichnis</td><td><input type='text' name='verzeichnis' size='40' value='$verzeichnis'></td></tr>";

    echo "<tr><td>Aufnahme nach ... Tagen löschen</td><td><input type='text' name='del_x_day' size='40' value='$del_x_day' onChange='this.form.submit()'></td></tr>"; // Lösche eine Aufnahme automatisch nach x Tagen
    if ($del_x_day > 0) {
      echo "<tr><td>&nbsp;&nbsp;&nbsp;Behalte ... Aufnahmen</td><td><input type='text' name='behalte_x_aufnahmen' size='40' value='$behalte_x_aufnahmen'></td></tr>"; // Aber behalte mindestens x Aufnahmen
    }
    echo "<tr><td>Pause, wenn ... Aufnahmen existieren</td><td><input type='text' name='pause_wenn_x_aufnahmen' size='40' value='$pause_wenn_x_aufnahmen'></td></tr>"; // Pausiere das Anlegene von Timern wenn x Aufnahmen vorhanden sind

    echo "<tr><td>Vermeide Wiederholung</td>";
    if ($vermeide_wiederholungen == "1") {$checked_y = " checked"; $checked_n = ""; } else {$checked_y = ""; $checked_n = " checked";}
    echo "<td>$txt_yes <input type='radio' name='vermeide_wiederholungen' value='1' ".$checked_y." onChange='this.form.submit()'>&nbsp;&nbsp; ";
    echo "      $txt_no <input type='radio' name='vermeide_wiederholungen' value='0' ".$checked_n." onChange='this.form.submit()' ></td>";
    echo "</tr>";

    if ($vermeide_wiederholungen == 1) { // Vermeide Wiederholung = ja
      echo "<tr><td>&nbsp;&nbsp;&nbsp;Erlaubte Wiederholungen</td><td><input type='text' name='anz_wiederholungen' size='40' value='$anz_wiederholungen' onChange='this.form.submit()'></td></tr>"; // erlaubte Anzahl Wiederholungen
      if ($anz_wiederholungen > 0) {
        echo "<tr><td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Nur Wiederholungen innerhalb ... Tagen</td><td><input type='text' name='wiederholung_x_tage' size='40' value='$wiederholung_x_tage'></td></tr>"; // Erlaube Wiederholungen nur innerhalb x Tagen
      }

      echo "<tr><td>&nbsp;&nbsp;&nbsp;Vergleiche Titel</td>";   // Vergleiche Titel bei Prüfung auf Wiederholung? 0/1
      if ($vergleiche_titel_yn == "1") {$checked_y = " checked"; $checked_n = ""; } else {$checked_y = ""; $checked_n = " checked";}
      echo "<td>$txt_yes <input type='radio' name='vergleiche_titel_yn' value='1' ".$checked_y." onChange='this.form.submit()'>&nbsp;&nbsp; ";
      echo "      $txt_no <input type='radio' name='vergleiche_titel_yn' value='0' ".$checked_n." onChange='this.form.submit()' ></td>";
      echo "</tr>";

      echo "<tr><td>&nbsp;&nbsp;&nbsp;Vergleiche Untertitel</td>";   // Vergleiche Untertitel bei Prüfung auf Wiederholung? 0/1
      if ($vergleiche_untertitel_yn == "1") {$checked_y = " checked"; $checked_n = ""; } else {$checked_y = ""; $checked_n = " checked";}
      echo "<td>$txt_yes <input type='radio' name='vergleiche_untertitel_yn' value='1' ".$checked_y." onChange='this.form.submit()'>&nbsp;&nbsp; ";
      echo "      $txt_no <input type='radio' name='vergleiche_untertitel_yn' value='0' ".$checked_n." onChange='this.form.submit()' ></td>";
      echo "</tr>";
      
      echo "<tr><td>&nbsp;&nbsp;&nbsp;Vergleiche Beschreibung</td>";   // Vergleiche Beschreibung bei Prüfung auf Wiederholung? 0/1
      if ($vergleiche_beschreibung_yn == "1") {$checked_y = " checked"; $checked_n = ""; } else {$checked_y = ""; $checked_n = " checked";}
      echo "<td>$txt_yes <input type='radio' name='vergleiche_beschreibung_yn' value='1' ".$checked_y." onChange='this.form.submit()'>&nbsp;&nbsp; ";
      echo "      $txt_no <input type='radio' name='vergleiche_beschreibung_yn' value='0' ".$checked_n." onChange='this.form.submit()' ></td>";
      echo "</tr>";
      
      if ($vergleiche_beschreibung_yn == 1) {
        echo "<tr><td>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Min. Übereinstimmung in %</td><td><input type='text' name='min_uebereinstimmung' size='40' value='$min_uebereinstimmung' ></td></tr>";
      }

      echo "<tr><td>&nbsp;&nbsp;&nbsp;Kennung für Inhalt</td><td><input type='text' name='kennung_inhalt' size='40' value='$kennung_inhalt' ></td></tr>";

      echo "<tr><td>&nbsp;&nbsp;&nbsp;Vergleiche Zeitpunkt</td><td><select name='vergleiche_zeitpunkt' size='1' style='width: 240px' onChange='this.form.submit()'>";
      echo "          <option value=0 ";
      if ($vergleiche_zeitpunkt == "0"){echo " selected";}
      echo                  ">nein</option>";
      echo "            <option value=1 ";
      if ($vergleiche_zeitpunkt == "1"){echo " selected";}
      echo                  ">gleicher Tag</option>";
      echo "            <option value=2 ";
      if ($vergleiche_zeitpunkt == "2"){echo " selected";}
      echo                  ">gleiche Woche</option>";
      echo "            <option value=2 ";
      if ($vergleiche_zeitpunkt == "3"){echo " selected";}
      echo                  ">gleicher Monat</option>";
      echo "         </select></td></tr>";
    }

    echo "<tr><td>Priorität</td><td><input type='text' name='prioritaet' size='40' value='$prioritaet' ></td></tr>";
    echo "<tr><td>Lebensdauer</td><td><input type='text' name='lebensdauer' size='40' value='$lebensdauer' ></td></tr>";
    echo "<tr><td>Vorlauf zum Timer-Beginn (min)</td><td><input type='text' name='vorlauf_begin' size='40' value='$vorlauf_begin' ></td></tr>";
    echo "<tr><td>Nachlauf am Timer-Ende (min)</td><td><input type='text' name='vorlauf_end' size='40' value='$vorlauf_end' ></td></tr>";

    echo "<tr><td>VPS</td>"; // Verwende VPS?
    if ($vps == "1") {$checked_y = " checked"; $checked_n = ""; } else {$checked_y = ""; $checked_n = " checked";}
    echo "<td>$txt_yes <input type='radio' name='vps' value='1' ".$checked_y.">&nbsp;&nbsp; ";
    echo "      $txt_no <input type='radio' name='vps' value='0' ".$checked_n."></td>";
    echo "</tr>";

    echo "<tr><td>automatisch löschen</td><td><select name='auto_del' size='1' style='width: 240px' onChange='this.form.submit()'>";
    echo "          <option value=0 ";
    if ($auto_del == "0"){echo " selected";}
    echo                  ">nein</option>";
    echo "            <option value=1 ";
    if ($auto_del == "1"){echo " selected";}
    echo                  ">Anzahl Aufnahmen</option>";
    echo "            <option value=2 ";
    if ($auto_del == "2"){echo " selected";}
    echo                  ">Anzahl Tage</option>";
    echo "         </select></td></tr>";

    if ($auto_del == 1) {
      echo "<tr><td>&nbsp;&nbsp;&nbsp;nach ... Aufnahmen</td><td><input type='text' name='del_nach_x_aufnahmen' size='40' value='$del_nach_x_aufnahmen' ></td></tr>";
    }

    if ($auto_del == 2) {
      echo "<tr><td>&nbsp;&nbsp;&nbsp;nach ... Tagen nach erster Aufnahme</td><td><input type='text' name='del_nach_x_tagen' size='40' value='$del_nach_x_tagen' ></td></tr>";
    }
  }

 
  if ($aktion == 2) {  // Wenn Aktion = Nur umschalten
    echo "<tr><td>Umschalten ... Minuten vor Start</td><td><input type='text' name='aktion_x_min_vor_start' size='40' value='$aktion_x_min_vor_start' ></td></tr>"; // Schalte x Minuten vor der Sendung um, wenn Aktion = 2
    echo "<tr><td>Ton anschalten</td><td><input type='text' name='ton_an' size='40' value='$ton_an' ></td></tr>";
  }
  
  if ($aktion == 3) {  // Wenn Aktion = Ankündigen und umschalten
    echo "<tr><td>Nachfrage ... Minuten vor Start</td><td><input type='text' name='aktion_x_min_vor_start' size='40' value='$aktion_x_min_vor_start' ></td></tr>"; // Schalte x Minuten vor der Sendung um, wenn Aktion = 2
    echo "<tr><td>Ton anschalten</td><td><input type='text' name='ton_an' size='40' value='$ton_an' ></td></tr>";
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


echo "</form>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
