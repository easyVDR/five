<?php
include('includes/kopf.php'); 

function dir2array($dir) {
  global $array ; 
  $handle = opendir($dir); 
  while ($datei = readdir ($handle)) {
    $pos = strpos($dir, ".rec");
    if (($datei!=".") && ($datei!="..") && ($datei=="info") && ($pos !== false)){
      $rec = substr($dir,8);
      $rec = substr($rec, 0, strlen($rec)-strlen(strrchr($rec, "/")));

      $datum_all = substr($dir, strrpos($dir,"/") + 1, strlen($dir) ); 
      $datum = substr($datum_all, 0, 10);
      $zeit  = substr($datum_all, 11, 5);
      $array[] = array($rec, $datum_all, $datum, $zeit);
    }
    if (is_dir($dir."/".$datei) && $datei!="." && $datei!="..") {
      dir2array($dir."/".$datei);
    }
  }
  closedir($handle);
  sort($array);
}

echo "<div id='kopf2'>? - Test-Seite</div>";

echo "<div id='inhalt'>";
echo "<center>";

//include('includes/subnavi_hilfe.php'); 

echo "<h2>Test-Seite</h2>";

// *************************************** Container Begin 'Developer' ****************************************************
container_begin(1, "Info.png", "Test-Seite");

$video_dir = "/video0";
dir2array($video_dir);
print_r($video_array);

echo "<table border=1>";
echo "<tr><th>Aufzeichnung</th><th>Datum/Zeit</th><th>Aktionen</th></tr>";

foreach($array as $wert) {
  $dir_encode = urlencode($wert[0]."/".$wert[1]);
  echo "<tr>";
  echo "<tr><td>".str_replace("_", " ", $wert[0])."</td><td>$wert[2] $wert[3]</td>";
  echo "<td><a class='b' href='aufnahmen.php?del=$dir_encode'>Del</a> &nbsp;&nbsp;&nbsp;"; 
  echo "    <a class='b' href='aufnahme_rename.php?rec=$dir_encode'>Rename</a>  &nbsp;&nbsp;&nbsp;";
  echo "    <a class='b' href='aufnahme.php?info=$dir_encode'>Edit-Info</a>";
  echo "</td>";
  echo "</tr>";
}
echo "</table>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
