<?php
include('includes/kopf.php'); 
include('txt-db-api/txt-db-api.php');

$db = new Database("portal");

$table = safeget('table');
//$table = substr ($table,0, strlen($table)-4);

echo "<div id='kopf2'>Datenbank - Tabelle</div>";

echo "<div id='inhalt'>";
echo "<center>";

//include('includes/subnavi_hilfe.php'); 

echo "<h2>Datenbank Tabelle: $table</h2>";

// *************************************** Container Begin '' ****************************************************
container_begin(1, "Info.png", "Datenbank Table: $table");

echo "<a href='datenbank.php'><button>Tabellenübersicht</button></a><br/><br/>";

//echo "DB_DIR: ".$DB_DIR."<br/>";

$resultSet = $db->executeQuery("SELECT * FROM $table");
$spalte = $resultSet->getColumnNames();

echo "<table class='tab1'>";
echo "<tr>";
foreach($spalte as $value) { echo "<th>$value</th>"; }
echo "</tr>";

while($resultSet->next()) {
  echo "<tr>";
  foreach($spalte as $value) {
    echo "<td>".$resultSet->getCurrentValueByName("$value")."</td>";
  }
  echo "</tr>";
}
echo "</table>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
