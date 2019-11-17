<?php
include('includes/kopf.php'); 


echo "<div id='kopf2'>Test-Seite</div>";

echo "<div id='inhalt'>";
echo "<center>";

//include('includes/subnavi_hilfe.php'); 

echo "<h2>Test-Seite</h2>";

// *************************************** Container Begin '' ****************************************************
container_begin(1, "Info.png", "Test");

echo "...<br/>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
