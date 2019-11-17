<?php

// *************************************** Container Begin '' ****************************************************
container_begin(2, "activetimer.png", "Timer Info");

//echo "$timer_nr<br/>";
//$Result = exec("svdrpsend LSTT $timer_nr |grep 250");
//$Timer_Array_show = timer2array($Result);
//echo $Result;

if ($Timer_Array_show['aktiv'] == 1) { $aktiv_anzeige = "aktiv"; } else { $aktiv_anzeige = "nicht aktiv"; }

if ($Anz_Timer == 0) { 
  echo "Keine Timer programmiert";
} else {
  echo "<table class='tab1'>";
  echo "<tr><th>$txt_tab_nr:      </th><td>".$Timer_Array_show['timer_nr'].    "</td></tr>";
  echo "<tr><th>$txt_tab_datei:   </th><td>".$Timer_Array_show['datei'].       "</td></tr>";
  echo "<tr><th>$txt_tab_dir:     </th><td>".$Timer_Array_show['verzeichnis']. "</td></tr>";
  echo "<tr><th>$txt_tab_status:  </th><td>".$aktiv_anzeige.                   "</td></tr>";
  echo "<tr><th>$txt_tab_channel: </th><td>".$Timer_Array_show['channel'].     "</td></tr>";
  echo "<tr><th>$txt_tab_tag:     </th><td>".$Timer_Array_show['tag'].         "</td></tr>";
  echo "<tr><th>$txt_tab_anfang:  </th><td>".$Timer_Array_show['anfang'].      "</td></tr>";
  echo "<tr><th>$txt_tab_ende:    </th><td>".$Timer_Array_show['ende'].        "</td></tr>";
  echo "<tr><th>VPS:              </th><td>                                     </td></tr>";
  echo "<tr><th>$txt_tab_prio:    </th><td>".$Timer_Array_show['prio'].        "</td></tr>";
  echo "<tr><th>$txt_tab_lebensdauer:          </th><td>".$Timer_Array_show['lebensdauer']. "</td></tr>";
  echo "<tr><th>$txt_tab_kindersicherung:      </th><td>                                     </td></tr>";
  echo "<tr><th valign=top>$txt_tab_suchtimer: </th><td>".htmlentities($Timer_Array_show['suchtimer']).  "</td></tr>";
  //echo "<tr><th valign=top>$txt_tab_suchtimer: </th><td>".htmlspecialchars($Timer_Array_show['suchtimer']).  "</td></tr>";
  echo "</table>";
  echo "<br/>";

  echo "<a href='?aktion=edit&timer_nr=$Timer_Array_show[timer_nr]'><button>$txt_b_timer_edit</button></a><br/>";
}

container_end();
// *************************************** Container Ende ****************************************************

// *************************************** Container Begin '' ****************************************************
container_begin(2, "activetimer.png", "EPG Info");

$min_add = 10;
$Zeit = $Timer_Array_show['anfang'];
$zeit_array = explode(":", $Zeit);
$h    = $zeit_array[0];
$min  = $zeit_array[1];  
$Zeit = date("H:i:s",(mktime ($h,$min,0,date("m"),date("d"),date("y")))+(60*$min_add)); //12:30:00  

$datum = strtotime(datum_de($Timer_Array_show['tag'])." ".$Zeit);
exec("svdrpsend LSTE $Timer_Array_show[channel] at $datum | grep 215", $epginfo);

//print_r($epginfo);

//echo "<table border='1' cellspacing='3' width='100%'>";
echo "<table class='tab1'>";
foreach ($epginfo as $datensatz) 
{
  $key  = substr($datensatz, 0, 5);
  $wert = substr($datensatz, 5, strlen($datensatz)-5);
  switch ($key) {
   case "215-C":
     // ChannelID, Kanalname
     $key_txt = "Channel";
     echo "<tr><th>$key_txt</th><td>$wert</td></tr>";
     break;
   case "215-E":
     // EventID, Startzeit, Dauer, TableID, Version
     //$key_txt = "Event";
     //echo "<tr><th>$key_txt</th><td>$wert</td></tr>";
     
     $wert_array = explode(' ', trim($wert));
     $start = date("d.m.Y H:i",$wert_array[1]);
     $dauer = round($wert_array[2]/60);
     echo "<tr><th>Event ID</th>       <td>$wert_array[0]</td></tr>";
     echo "<tr><th>Event Startzeit</th><td>$start</td></tr>";
     echo "<tr><th>Event Dauer</th>    <td>$dauer Min.</td></tr>";
     echo "<tr><th>Event TableID</th>  <td>$wert_array[3]</td></tr>";
     echo "<tr><th>Event Version</th>  <td>$wert_array[4]</td></tr>";
     break;
   case "215-T":
     // Titel
     $key_txt = "Titel";
     echo "<tr><th>$key_txt</th><td>$wert</td></tr>";
     break;
   case "215-S":
     // Sub-Titel
     $key_txt = "Sub-Titel";
     echo "<tr><th>$key_txt</th><td>$wert</td></tr>";
     break;
   case "215-D":
     // Beschreibung
     $key_txt = "Beschreibung";
     $wert_anzeige = str_replace("|", "<br/>", $wert);
     echo "<tr><th valign='top'>$key_txt</th><td>$wert_anzeige</td></tr>";
     break;
   case "215-X":
     // Datenstromart, Typ, Sprache, Beschreibung
     $wert_Array = explode(' ', trim($wert));
     $key_txt = "Technische Details";
     //<div onmouseover=\"Tip('Datenstromart')\">$wert_Array[0]</div> $wert_Array[1] $wert_Array[2] $wert_Array[3] $wert_Array[4] $wert_Array[5] $wert_Array[6]
     echo "<tr><th style='white-space: nowrap;'>$key_txt</th><td>$wert</td></tr>";
     break;
   case "215-V":
     // VPS
     $key_txt = "VPS";
     echo "<tr><th>$key_txt</th><td>$wert</td></tr>";
     break;
   case "215-G":
     // Genrecodes
     $key_txt = "Genre";
     echo "<tr><th>$key_txt</th><td>$wert</td></tr>";
     break;
   default:
  }
}
echo "</table>";

container_end();
// *************************************** Container Ende ****************************************************

?>