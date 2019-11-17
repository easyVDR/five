<?php
include('includes/kopf.php'); 
include('txt-db-api/txt-db-api.php');

echo "<div id='kopf2'>Datenbank Tabellen</div>";

echo "<div id='inhalt'>";
echo "<center>";

//include('includes/subnavi_hilfe.php'); 

echo "<h2>Datenbank - alle Tabellen</h2>";

// *************************************** Container Begin '' ****************************************************
container_begin(1, "Info.png", "Datenbank");

echo "<table border=0>";
echo "<tr><td>txtdbapi_version:</td><td>".txtdbapi_version()."</td></tr>";
echo "<tr><td>DB_DIR:</d><td>".$DB_DIR."</td></tr>";
echo "</table>";

echo "<br/><b>Datenbank-Tabellen:</b><br/><br/>";

//$db_dir = scandir('$DB_DIR');
$db_dir = scandir('/var/www/databases/portal');
$dir = "";

echo "<table class='tab1'>";
foreach ($db_dir as $datei) { // Ausgabeschleife
  if (!is_dir($dir."/".$datei) && $datei!="." && $datei!=".." && substr($datei,strlen($datei)-4,4)==".txt"){
    $table = substr ($datei,0, strlen($datei)-4);
    echo "<tr><td>&nbsp;<a class='b' href=datenbank_table.php?table=$table>$table</a></td></tr>";
  }
}
echo "</table>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
