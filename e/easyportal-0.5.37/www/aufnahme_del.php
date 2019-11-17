<?php
include('includes/kopf.php'); 

$del1 = safeget('rec');
$del2 = safeget('del2');

echo "<div id='kopf2'>$txt_aufnahmen - $txt_kopf2</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_aufnahmen.php'); 

echo "<h2>$txt_ueberschrift</h2>";

// *************************************** Container Begin ****************************************************
container_begin(1, "film.png", $rec);

if ($del2 != "") {
  $del2 = "/video0/$del2";
  $del2 = str_replace('"', '\"', $del2);
  $del3 = str_replace("rec", "del", $del2);
  $txt_debug_meldungen[] = "Aktion: Löschen";
  $txt_debug_meldungen[] = "Alter Name: $del2";
  $txt_debug_meldungen[] = "Neuer Name: $del3";
  exec("sudo $SH_BEFEHL mv $del2 $del3");
  exec("sudo $SH_BEFEHL touch /video0/.update"); 
  echo "Aufnahme '$del2' wurde gelöscht!<br/><br/>";
  echo "<a class='b' href='aufnahmen.php?action=recsuche'><button>$txt_b_weiter</button></a><br/><br/>"; 
}

if ($del1 != "") {
  echo "$txt_wirklich_loeschen<br/><br/>";
  echo "<b>/video0/".$del1."</b><br/>";
  $del_encode = urlencode($del1);
  echo "<br/><br/>";  
  echo "<a href=aufnahmen.php><button>$txt_b_liste</button></a>";   
  echo "<a href=aufnahme.php?info=$del_encode><button>$txt_b_aufnahme</button></a>";   
  echo "<a href=aufnahme_del.php?del2=$del_encode><button>$txt_b_del</button></a>";   
}

container_end();
// *************************************** Container Ende ****************************************************
 
echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
