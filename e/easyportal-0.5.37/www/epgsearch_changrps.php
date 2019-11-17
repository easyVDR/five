<?php
include('includes/kopf.php'); 

echo "<div id='kopf2'>Timer - epgsearch - epgsearchchangrps.conf</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_timer.php'); 
echo "<br/>";
include('includes/subnavi_epgsearch.php'); 

echo "<h2>epgsearchchangrps.conf</h2>";


function channel2string ($channel) {
    $channel_str_array = explode('-', $channel);
    $befehl = "cat /var/lib/vdr/channels.conf | grep $channel_str_array[3]";
    //$txt_debug_meldungen[] = "Befehl: $befehl"; 
    $channel_str = exec($befehl);
    $stringposition = strpos($channel_str, ";");
    $channel_str = substr($channel_str, 0, $stringposition);
    $stringposition = strpos($channel_str, ",");
    if ($stringposition) {$channel_str = substr($channel_str, 0, $stringposition);}
    return $channel_str;
}

// *************************************** Container Begin '' ****************************************************
container_begin(1, "activetimer.png", "Kanalgruppen");

echo "<table class=tab1>";
echo "<tr><th>Kanalgruppe</th><th>Kanäle</th><th>Aktionen</th>";
//echo "<th></th>";
echo "</tr>";


$datei=file("/var/lib/vdr/plugins/epgsearch/epgsearchchangrps.conf");
unset($inhalt,$eintrage);
for ($i=0;$i<count($datei)+1;$i++){ // Wir lesen die gesammte Datei in die Variable $inhalt
  $zeile = "$datei[$i]";
  if ($zeile != "") {
    echo "<tr>";
    $eintrage = explode('|',$zeile);


    $stringposition = strpos($zeile, "|");
    $kanale = substr($zeile, $stringposition+1);
    
    $kanale_array = explode('|',$kanale);
    $kanale_str = "";
    
    foreach ($kanale_array as $datensatz) 
    {
      //echo $datensatz."<br/>";
      if ($kanale_str != "") { $kanale_str .= ", "; }
      $kanale_str .= channel2string($datensatz);
    }
    echo "<td><nobr>$eintrage[0]</nobr></td><td>$kanale_str</td>";
    echo "<td>&nbsp;</td>";
    echo "</tr>";
  }
}

echo "</table>";

 container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
