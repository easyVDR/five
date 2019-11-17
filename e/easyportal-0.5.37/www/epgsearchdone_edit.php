<?php
include('includes/kopf.php'); 

$id   = safeget('id');
$txt_save = "";


if (!empty($_POST)) {
  //  echo 'Post request<br />';
  $id          = safepost('id');
  $Title       = trim(safepost('Title'));
  $SubTitel    = trim(safepost('SubTitel'));
  $Description = trim(safepost('Description'));
  $Description = trim(str_replace("\r\n", "|", $Description));

  $episodes = trim(shell_exec("ls $eplists_dir/*.episodes |cut -d'/' -f5-"));

  if (((!empty($Title)) && (strpos($episodes, $Title))) === false) {
    $pos = strpos(strtolower($episodes), strtolower($Title));
    if ($pos !== false) {
      $Title = substr($episodes, $pos, strlen($Title));
    }       
  }

  if (((!empty($Title)) && (!empty($SubTitel)) && (strpos($episodes, $Title))) === false) {
    $ep_se = $ep_ep = $ep_no = "";
  } else {
    $befehl = "grep -i '".$SubTitel."' '$eplists_dir/$Title.episodes' ";
    $epi = trim(shell_exec("$befehl"));
    $epi_array = preg_split("/[\t]/", $epi);
    $ep_se = $epi_array[0];
    $ep_se *= 1;
    if ($ep_se == 0) {$ep_se = "";}
    if (isset($epi_array[1])) { $ep_ep = $epi_array[1]; } else { $ep_ep = "";}
    if (isset($epi_array[2])) { $ep_no = $epi_array[2]; } else { $ep_no = "";}
    //$No = $epi_array[2];
  }
 
  $db->executeQuery("UPDATE epgsearchdone SET Title='$Title', SubTitel='$SubTitel', Description='$Description', ep_serie='$Title', 
           ep_title='$SubTitel', ep_se='$ep_se', ep_ep='$ep_ep', ep_no='$ep_no' WHERE id='$id'");
  $txt_save = "<br/><br/>Datensatz wurde gespeichert";

}

echo "<div id='kopf2'>epgsearchdone Datensatz</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_timer.php'); 

echo "<h2>epgsearchdone Datensatz</h2>";

// *************************************** Container Begin '' ****************************************************
container_begin(1, "Info.png", "epgsearchdone Datensatz editieren");


echo "<a href='epgsearchdone.php'><button>zur epgsearchdone.data</button></a>";
echo "<a href='epgsearchdone_view.php?id=$id'><button>zur Ansicht Datensatz</button></a><br/><br/>";

if ($txt_save != "") {
  echo $txt_save;
}
else
{
  echo "<form action='".$_SERVER['PHP_SELF']."' method='post'>";
  echo "<input type='hidden' name='id' value='$id'>";
  echo "<input type='submit' name='submit' value='Speichern'><br/><br/>";

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
  $Description = trim(str_replace("|", "\n", $Description));
  $Description = str_replace("'", " ", $Description);
  $Description = str_replace("\"", " ", $Description);

  $episodes = trim(shell_exec("ls $eplists_dir/*.episodes |cut -d'/' -f5-"));
  $ep_serie = $resultSet->getCurrentValueByName("ep_serie");


  echo "<table class='tab1'>";
  echo "<tr><td>Titel</td>";
  echo "    <td><input type='text' size='50' maxlength='50' name='Title' value='".$resultSet->getCurrentValueByName("Title")."' onChange='this.form.submit()'></td>";
  echo "</tr>";
  echo "<tr><td>SubTitel</td>";
  echo "    <td><input type='text' size='50' maxlength='50' name='SubTitel' value='".$resultSet->getCurrentValueByName("SubTitel")."' onChange='this.form.submit()'></td>";
  echo "</tr>";
  echo "<tr><td>Datum</td>       <td>$datum</td></tr>";
  echo "<tr><td>Dauer</td>       <td>$dauer min</td></tr>";
  echo "<tr><td>Kanal</td>       <td>$channel_str</td></tr>";
  echo "<tr><td>Beschreibung</td>";
  echo "    <td><textarea name='Description' cols='100' rows='30'>$Description</textarea>";
  echo "</tr>";
  echo "</table>";
}

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
