<?php
// programmierte Timer
$txt_NoTimer = "Keine Timer programmiert";

$TIMERS     = "/etc/vdr/timers.conf";
$TimersShow = safeget('TimersShow');

if ($TimersShow == "") { $TimersShow = 26; }  // Anzahl der anzuzeigenen anstehenden Timer

exec("sudo -u root $SH_EASYPORTAL timer",$Result);
$Anz_Timer = count($Result);
$txt_debug_meldungen[] = "Anzahl Timer: $Anz_Timer";

$i = 0;
$DATUM = "";

// *************************************** Container Begin '' ****************************************************
container_begin(2, "activetimer.png", $txt_ue_timer);

echo "<div id='rec_countdown'>$txt_zeit_berechnung</div><br/>";

echo "<table border='0' cellspacing='3'>";
if ($Anz_Timer > 0) {
  foreach ($Result as $Satz) {
    if ($i < $TimersShow) {
      $Daten = explode(";;",$Satz);
      $Tag = explode("-",$Daten[0]);
      if ($DATUM <> $Daten[0]) {
        echo "<tr><td align='left' colspan='2'><b>". $Tag[2].".".$Tag[1].".".$Tag[0]."</b></td></tr>";
      }
      echo "<tr>";
      
      $timestamp_akt = time();
      $timestamp_von = mktime(substr($Daten[1],0,2),substr($Daten[1],2,4),0,$Tag[1],$Tag[2],$Tag[0]);
      $timestamp_bis = mktime(substr($Daten[2],0,2),substr($Daten[2],2,4),0,$Tag[1],$Tag[2],$Tag[0]);
      if (($timestamp_von < $timestamp_akt) ) { $color='red';} else {$color='black';}
      // and ($timestamp_bis > $timestamp_akt)
      echo "<td align='right' valign='top'><font color='$color'><nobr>";
      echo substr($Daten[1],0,2).":".substr($Daten[1],2,4)."-".substr($Daten[2],0,2).":".substr($Daten[2],2,4);
      echo "</nobr></font></td>";
      // ... <div style='text-decoration: blink;'>
      echo "<td align='left'><font color='$color'>$Daten[3]</font></td>";
      echo "</tr>";
      $DATUM=$Daten[0];
      $i++;
    }
  }
}

echo "</table>";
echo "<p>";

if ($Anz_Timer > $TimersShow) { 
  echo "<b><a class='b' href='".$_SERVER['PHP_SELF']."?TimersShow=999'>alle Timer anzeigen ($Anz_Timer Timer)</a></b>";
}

if (($Anz_Timer < $TimersShow) and ($Anz_Timer > 26)) { 
  echo "<b><a class='b' href='".$_SERVER['PHP_SELF']."'>weniger Timer anzeigen...</a></b>";
}

if ($Anz_Timer == 0) { 
  echo "<b>$txt_NoTimer</b>";
}

container_end();
// *************************************** Container Ende ****************************************************

?>