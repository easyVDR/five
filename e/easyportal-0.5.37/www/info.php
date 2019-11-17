<?php
include('includes/kopf.php'); 

echo "<div id='kopf2'>? - $txt_kopf2</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_hilfe.php'); 

echo "<h2>$txt_ueberschrift</h2>";

// *************************************** Container Begin ****************************************************
container_begin(1, "Info.png", $txt_ue_info);

echo "$txt_text1<br/>";
echo "$txt_text2<br/>";
echo "$txt_text3<br/>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
