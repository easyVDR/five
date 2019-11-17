<?php
include('includes/kopf.php'); 

$id   = safeget('id');


echo "<div id='kopf2'>epgsearchdone Datensatz</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_timer.php'); 
echo "<br/>";
include('includes/subnavi_epgsearch.php'); 

echo "<h2>epgsearchdone Datensatz</h2>";

// *************************************** Container Begin '' ****************************************************
container_begin(1, "Info.png", "epgsearchdone Datensatz");

echo "<a href='epgsearchdone.php'><button>zur epgsearchdone.data</button></a>";
echo "<a href='epgsearchdone_edit.php?id=$id'><button>Eintrag editieren</button></a><br/><br/>";

$resultSet = $db->executeQuery("SELECT * FROM epgsearchdone WHERE id='$id'");
$resultSet->next();

$record_str = $resultSet->getCurrentValueByName("Record");
$record_str_array = explode(' ', $record_str);
$datum = date("d.m.Y H:i",$record_str_array[0]);
$dauer = round($record_str_array[1]/60) ;

$channel_str = $resultSet->getCurrentValueByName("Channel");
$channel_str_array = explode('-', $channel_str);
$befehl = "cat /var/lib/vdr/channels.conf | grep $channel_str_array[3]";
//$txt_debug_meldungen[] = "Befehl: $befehl"; 
$channel_str = exec($befehl);
$stringposition = strpos($channel_str, ";");
$channel_str = substr($channel_str, 0, $stringposition);
$stringposition = strpos($channel_str, ",");
if ($stringposition) {$channel_str = substr($channel_str, 0, $stringposition);}

$Description = $resultSet->getCurrentValueByName("Description");
$Description = trim(str_replace("|", "<br/>", $Description));
$Description = str_replace("'", " ", $Description);
$Description = str_replace("\"", " ", $Description);

$episodes = trim(shell_exec("ls $eplists_dir/*.episodes |cut -d'/' -f5-"));
$ep_serie = $resultSet->getCurrentValueByName("ep_serie");


echo "<table class='tab1'>";
echo "<tr><td>Titel</td>";
echo "    <td><a onmouseover=\"Tip('zur epgsearchdone mit diesem Titel')\" href='epgsearchdone.php?title=".urlencode($ep_serie)."' class='b'>".$resultSet->getCurrentValueByName("Title")."</a></td>";
echo "</tr>";
echo "<tr><td>SubTitel</td>    <td>". $resultSet->getCurrentValueByName("SubTitel") ."</td></tr>";

if (((!empty($ep_serie)) && (strpos($episodes, $ep_serie))) === false) {
  // Keine Serie
  echo "<tr><td style='white-space: nowrap;'>gefundene Serie</td>";
  echo "    <td><i>Keine <a  onmouseover=\"Tip('zur Serien-Übersicht')\" href='serien.php' class='b'>Serie</a> in eplists gefunden</i></td>";
  echo "</tr>";
}
else {
  // Serie
  echo "<tr><td style='white-space: nowrap;'>gefundene Serie</td>";
  echo "    <td><a onmouseover=\"Tip('zur Serie')\" href='serie.php?serie=". urlencode($ep_serie) ."' class='b'>$ep_serie</a></td>";
  echo "</tr>";
  echo "<tr><td>Folge</td><td>"       . $resultSet->getCurrentValueByName("ep_title") ."</td>" ;
  echo "<tr><td>Staffel</td><td>"     . $resultSet->getCurrentValueByName("ep_se") ."</td>" ;
  echo "<tr><td>Staffelfolge</td><td>". $resultSet->getCurrentValueByName("ep_ep") ."</td>" ;
  echo "<tr><td>Folge-Nr</td><td>"    . $resultSet->getCurrentValueByName("ep_no") ."</td>" ;
}

echo "<tr><td>Datum</td>       <td>$datum</td></tr>";
echo "<tr><td>Dauer</td>       <td>$dauer min</td></tr>";
echo "<tr><td>Kanal</td>       <td>$channel_str</td></tr>";
echo "<tr><td>Beschreibung</td><td>$Description</td></tr>";
echo "</table>";


container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
