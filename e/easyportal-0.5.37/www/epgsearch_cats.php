<?php
include('includes/kopf.php'); 

echo "<div id='kopf2'>Timer - epgsearch - epgsearchcats.conf</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_timer.php'); 
echo "<br/>";
include('includes/subnavi_epgsearch.php'); 

echo "<h2>epgsearchcats.conf</h2>";


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
container_begin(1, "activetimer.png", "epgsearchcats.conf");

echo "<table class=tab1>";
echo "<tr><th>ID</th><th>Kategoriename</th><th>name in menu</th><th>values</th><th>searchmode</th>";
echo "</tr>";


$datei=file("/var/lib/vdr/plugins/epgsearch/epgsearchcats.conf");
unset($inhalt,$eintrage);
for ($i=0;$i<count($datei)+1;$i++){ // Wir lesen die gesammte Datei in die Variable $inhalt
  $zeile = trim($datei[$i]);
  //echo "'$zeile'<br/>";
  if (($zeile != "") and ( substr($zeile,0,1)!= "#")) {
    echo "<tr>";
    $eintrage = explode('|',$zeile);

    echo "<td><nobr>$eintrage[0]</nobr></td><td>$eintrage[1]</td><td>$eintrage[2]</td><td>$eintrage[3]</td><td>$eintrage[4]</td>";
    echo "</tr>";
  }
}

echo "</table>";

 container_end();
// *************************************** Container Ende ****************************************************

// *************************************** Container Begin '' ****************************************************
container_begin(1, "Info.png", "Hilfe");


echo "The order of items determines the order listed in epgsearch. It does not depend on the ID, which is used by epgsearch.<br/>";
echo "Format:<br/>";
echo "ID|category name|name in menu|values separated by ',' (option)|searchmode(option)<br/><br/>";
echo "- 'ID' should be a unique positive integer (changing the id later on will force you to reedit your search timers!)<br/>";
echo "- 'category name' is the name in your epg.data<br/>";
echo "- 'name in menu' is the name displayed in epgsearch.<br/>";
echo "- 'values' is an optional list of possible values if you omit the list, the entry turns to an edit field in epgsearch, else it's a list of items to select from<br/>";
echo "- 'searchmode' is an optional parameter specifying the mode of search:<br/><br/>";
echo "text comparison:<br/>";
echo "0 - the whole term must appear as substring<br/>";
echo "1 - all single words (delimiters are ',', ';', '|' or '~') must exist as substrings. This is the default search mode.<br/>";
echo "2 - at least one word (delimiters are ',', ';', '|' or '~') must exist as substring.<br/>";
echo "3 - matches exactly<br/>";
echo "4 - regular expression<br/><br/>";
echo "numerical comparison:<br/>";
echo "10 - less<br/>";
echo "11 - less or equal<br/>";
echo "12 - greater<br/>";
echo "13 - greater or equal<br/>";
echo "14 - equal<br/>";
echo "15 - not equal<br/>";


container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
