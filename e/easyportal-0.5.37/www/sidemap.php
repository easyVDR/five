<?php
include('includes/kopf.php'); 
include('includes/php_file_tree.php');

echo "<div id='kopf2'>? - Sidemap</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_hilfe.php'); 

echo "<h2>Sidemap</h2>";

// *************************************** Container Begin 'Developer' ****************************************************
container_begin(1, "Info.png", "Sidemap");

echo "<div class='filetree'>";
echo php_file_tree("/var/www/", "view_datei.php?url=[link]" );
echo "</div>";

//"javascript:alert('You clicked on [link]');"
/*
$verzeichnis = dir(".");
$endung = ".php";
while($datei = $verzeichnis->read()) {
  if(substr($datei,strlen($endung)*(-1)) == $endung) {
    echo "<A HREF=\"$datei\">$datei</A> - ";
    $fp = fopen($datei,"r");
    while (!feof($fp)) {
      $linie = fgets($fp, 1024);
      if(eregi("<title>(.*)</title>", $linie, $title))
      break;
    }
    $title = $title[1];
    echo "$title<BR>";
    fclose($fp);
  }
}
$verzeichnis->close(); 
*/

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
