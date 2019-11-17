<?php
// Zeigt ein Bild auch ausserhalb vom www-root 
// Einbindung z.B.:
//echo "<img src='bild.php?url=/var/lib/vdr/test.gif' width='500px'>";

if(!file_exists($_REQUEST["url"])) die("bild nicht gefunden!");
$info = getimagesize($_REQUEST["url"]);
switch($info[2]) {
case 1: // gif
  header("Content-type: image/gif");
  break;
case 2: // jpeg
  header("Content-type: image/jpeg");
  break;
case 3: // png
  header("Content-type: image/png");
  break;
}
readfile($_REQUEST["url"]);
?>


