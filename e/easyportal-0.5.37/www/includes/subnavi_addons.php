<?php
$sprachdatei = "lang/".$_SESSION["sprache"]."/m_txt_addons.php";
include($sprachdatei);

echo "<br/>";
echo "<nav class='navi3'><ul>";

if ($_SERVER['PHP_SELF']=="/addons.php")
  {echo "<li><a href='addons.php' class='pressed'>$txt_addons_installieren</a></li>"; }
else 
  {echo "<li><a href='addons.php'>$txt_addons_installieren</a></li>"; }

if ($_SERVER['PHP_SELF']=="/plugins.php")
  {echo "<li><a href='plugins.php' class='pressed'>$txt_plugins_aktivieren</a></li>"; }
else 
  {echo "<li><a href='plugins.php'>$txt_plugins_aktivieren</a></li>"; }

echo "</ul></nav>";
?>
