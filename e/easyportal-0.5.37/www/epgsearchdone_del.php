<?php
include('includes/kopf.php'); 

$id     = safeget('id');
$aktion = safeget('aktion');

echo "<div id='kopf2'>epgsearchdone Datensatz</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_timer.php'); 

echo "<h2>epgsearchdone Datensatz löschen</h2>";

// *************************************** Container Begin '' ****************************************************
container_begin(1, "Info.png", "epgsearchdone Datensatz löschen");

$txt_del = "";
if ($id=="") {
  echo "Kein Datensatz angegeben!<br/><br/>";
}
else {
  if ($aktion=="del") {
    $resultSet = $db->executeQuery("delete FROM epgsearchdone WHERE id='$id'");
    echo "Datensatz wurde gelöscht!<br/><br/>";
  }
}
  
echo "<a href='epgsearchdone.php'><button>zur epgsearchdone.data</button></a>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
