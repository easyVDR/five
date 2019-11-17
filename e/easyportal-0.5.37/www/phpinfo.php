<?php
include('includes/kopf.php'); 

echo "<div id='kopf2'>? - phpInfo</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_hilfe.php'); 

echo "<h2>phpInfo</h2>";

// *************************************** Container Begin 'phpInfo' ****************************************************
container_begin(1, "Info.png", "PHP-Info");

ob_start();
phpinfo();
$pinfo = ob_get_contents();
ob_end_clean();
 
$pinfo = preg_replace( '%^.*<body>(.*)</body>.*$%ms','$1',$pinfo);
echo $pinfo;

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";    // Inhalt

include('includes/fuss.php'); 
?>