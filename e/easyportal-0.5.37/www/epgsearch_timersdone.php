<?php
include('includes/kopf.php'); 

?>
<script type="text/javascript" src="/includes/js/jquery.tablesorter.js"></script> 
<script type="text/javascript">
  $(document).ready(function() { $("#sortTable").tablesorter(
    {sortList: [[1,0]] }
  ); } );
</script>
<?php

echo "<div id='kopf2'>Timer - epgsearch - timersdone.conf</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_timer.php'); 
echo "<br/>";
include('includes/subnavi_epgsearch.php'); 

echo "<h2>timersdone.conf</h2>";

// *************************************** Container Begin '' ****************************************************
container_begin(1, "activetimer.png", "timersdone.conf");

echo "<table id='sortTable' class='tab1 tablesorter'>";
echo "<thead>";
echo "<tr><th>Kanal</th><th>Begin</th><th>Ende</th><th onmouseover=\"Tip('Suchtimer')\" >ST</th><th>Titel</th><th>Subtitel</th>";
//echo "<th></th>";
echo "</tr>";
echo "</thead>";
echo "<tbody>";

$datei=file("/var/lib/vdr/plugins/epgsearch/timersdone.conf");
unset($inhalt,$eintrage);
for ($i=0;$i<count($datei)+1;$i++){ // Wir lesen die gesammte Datei in die Variable $inhalt
  $zeile ="$datei[$i]";
  if ($zeile != "") {
    echo "<tr>";
    $zeile ="$datei[$i]";
    $eintrage = explode(':',$zeile);

    $channel_str_array = explode('-', $eintrage[0]);
    $befehl = "cat /var/lib/vdr/channels.conf | grep $channel_str_array[3]";
    //$txt_debug_meldungen[] = "Befehl: $befehl"; 
    $channel_str = exec($befehl);
    $stringposition = strpos($channel_str, ";");
    $channel_str = substr($channel_str, 0, $stringposition);
    $stringposition = strpos($channel_str, ",");
    if ($stringposition) {$channel_str = substr($channel_str, 0, $stringposition);}

    //$datum_von = date("d.m.Y H:i",$eintrage[1]);
    $datum_von = date("Y-m-d H:i",$eintrage[1]);
    $datum_bis = date("H:i",$eintrage[2]);
    
    echo "<td>$channel_str</td><td>$datum_von</td><td>$datum_bis</td>";
    echo "<td align=right><a class='b' href='suchtimer_view.php?id=$eintrage[3]'>$eintrage[3]</a></td>";
    echo "<td>$eintrage[4]</td><td>$eintrage[5]</td>";
    // echo "<td>$eintrage[6]</td>";
    echo "</tr>";
  }
}
echo "</tbody>";
echo "</table>";

 container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
