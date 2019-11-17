<?php
include('includes/kopf.php'); 

$url = safeget('url');

echo "<div id='kopf2'>View Datei</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_hilfe.php'); 

echo "<h2>View Datei '$url'</h2>";

// *************************************** Container Begin '' ****************************************************
container_begin(1, "Info.png", "View Datei '$url'");

echo "<a href='sidemap.php'><button>zurück zum Sidemap</button></a><br/>";
echo "<br/>";

echo "<table class=tab1>";
echo "<tr><th>Dateiname:       </th><td>$url</td></tr>";
if ((substr($url, strlen($url)-3, 3) == "png") or (substr($url, strlen($url)-3, 3) == "jpg") or (substr($url, strlen($url)-3, 3) == "gif")){
  $link = substr($url, 9, strlen($url)-9) ;
  echo "<tr><th valign='top'>Bildvorschau</th><td><img src='$link'></td></tr>";
}
echo "<tr><th>Dateigröße:      </th><td>".filesize($url)." Byte</td></tr>";
echo "<tr><th>Dateityp:        </th><td>".filetype($url)."</td></tr>";
echo "<tr><th>Zugriffsrechte:  </th><td>".substr(sprintf('%o', fileperms($url)), -4)."</td></tr>";  // Anzeige als Oktal-Wert
echo "<tr><th>letzter Zugriff: </th><td>".date("d.m.Y H:i:s", fileatime($url))."</td></tr>";
echo "<tr><th>letzte Änderung Datei-Inode: </th><td>".date("d.m.Y H:i:s", filectime($url))."</td></tr>";
echo "<tr><th valign='top'>Gruppenzugehörigkeit: </th><td><pre>";
//print_r(posix_getgrgid(filegroup($url)));
Debug::dump(posix_getgrgid(filegroup($url)), true, '', true);
echo "</pre></td></tr>";
echo "<tr><th>letzte Änderung </th><td>".date("d.m.Y H:i:s", filemtime($url))."</td></tr>";
echo "<tr><th valign='top'>Dateieigentümer       </th><td><pre>";
//print_r(posix_getpwuid(fileowner($url)));
Debug::dump(posix_getpwuid(fileowner($url)), true, '', true);
echo "</pre></td></tr>";
echo "</table>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
