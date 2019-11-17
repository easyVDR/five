<?php
include('includes/kopf.php'); 

$auswahl = safepost('auswahl');
//if ($auswahl == "") { $auswahl = "all";}
if ($auswahl == "") { $auswahl = "my";}

echo "<div id='kopf2'>$txt_kopf2</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_aufnahmen.php'); 

echo "<h2>$txt_ueberschrift</h2>";

// *************************************** Container Begin ****************************************************
container_begin(1, "Info.png", $txt_ue_serien);

if ($auswahl == "all") { 
  // Alle Serien
  $eplist_dir_array = scandir($eplists_dir); 
  unset($eplist_dir_array[array_search(".", $eplist_dir_array)]);
  unset($eplist_dir_array[array_search("..", $eplist_dir_array)]);
  $eplist_dir_array = array_values($eplist_dir_array);  
  //print_r($eplist_dir_array);
}
else { 
  // Meine Serien
  $eplist_dir_array = array();
  $resultSet = $db->executeQuery("SELECT DISTINCT Serie FROM aufnahmen WHERE Serie <> '' ");
  if ($resultSet->getRowCount() > 0) {
   while($resultSet->next()) {
    $eplist_dir_array[] = $resultSet->getCurrentValueByName("Serie").".episodes";
   }
  }
}

$anzahlDatensaetze = count($eplist_dir_array);
$txt_debug_meldungen[] = "AnzahlDatensaetze: $anzahlDatensaetze";

// Anzahl der Spalten
$maxSpalten = 5;
$spalten = isset($_POST["spalten"]) ? $_POST["spalten"] : 1;
$spalten = $spalten > $maxSpalten || $spalten < 1 ? 1 : $spalten;
if (isset($_POST["spalten"])) { 
  $_SESSION['spalten'] = $spalten; 
}
else { 
  if (isset($_SESSION['spalten'])) { $spalten = $_SESSION['spalten']; }  else { $spalten = 4; /* Spaltenstandard */ }
}

// Breite der Spalten
$spaltenBreite = floor(100/$spalten);
$txt_debug_meldungen[] = "SpaltenBreite: $spaltenBreite";

// Formular Spalten
echo "<form name='Form' action='' method='post'>";

if ($auswahl == "all"){$checked_all = " checked"; } else {$checked_all = ""; }
if ($auswahl == "my") {$checked_my = " checked"; } else {$checked_my = ""; }
echo "<td>$txt_alle_serien <input type='radio' name='auswahl' value='all' ".$checked_all." onChange='this.form.submit()'>&nbsp;&nbsp; ";
echo " $txt_meine_serien <input type='radio' name='auswahl' value='my' ".$checked_my." onChange='this.form.submit()'><br/><br/>";

echo "$txt_anz_spalten: <select name='spalten' size='1' onChange='this.form.submit()'>";
for ($i = 1; $i <= $maxSpalten; $i++) {
  echo "<option ";
  if ($spalten == $i) {echo "selected ";}
  echo ">".$i."</option>";
}
echo "</select></form>";
echo "<br/><br/>";

$nr=0;
echo "<table class=tab1>";
echo "<tr>";
foreach ($eplist_dir_array as $file) { // Ausgabeschleife
  $file=ltrim($file);
  $file_anzeige = Str_replace("_", " ", substr($file,0,Strlen($file)-9));
  
  $resultSet = $db->executeQuery("SELECT * FROM aufnahmen WHERE Serie='$file_anzeige'");
  $anz_rec = $resultSet->getRowCount();
  
  $resultSet2 = $db->executeQuery("SELECT * FROM epgsearchdone WHERE ep_serie='$file_anzeige'");
  $anz_done = $resultSet2->getRowCount();
  
  
  if ($anz_rec > 0) {$anz_anzeige = "($anz_rec Aufnahmen)";} else {$anz_anzeige = "";};
  if ($anz_done > 0) {$anzdone_anzeige = "($anz_done epgsearchdone)";} else {$anzdone_anzeige = "";};
  
  echo "<td width='".$spaltenBreite."%'><a class='b' href='serie.php?serie=".urlencode($file_anzeige)."'>$file_anzeige</a> $anz_anzeige $anzdone_anzeige</td>";
  //echo "'$nr', '$spalten', '$anzahlDatensaetze'<br/>";
  //print_r(in_array($nr, range($spalten-1, $anzahlDatensaetze, $spalten)))."<br/>";
  //print_r(range($spalten-1, $anzahlDatensaetze, $spalten));
  if (in_array($nr, range($spalten-1, $anzahlDatensaetze, $spalten))) { echo '</tr><tr>'; }
  $nr++;
}
echo "</tr>";
echo "</table>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
