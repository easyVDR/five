<?php
include('includes/kopf.php'); 

?>
<style type="text/css">
  @media (max-width: 700px) { .pc_size { width: 110px; padding:2px; } }
  @media (min-width: 701px) { .pc_size { width: 180px; padding:4px; } }
</style>
<?php


$button   = safeget('button');
$pcscript = "/usr/share/easyvdr/program-changer/program-changer-easy-portal.sh";

if (!file_exists("/var/www/includes/variablen_pchanger.php")) {
  // echo "konfig nicht vorhan";
  exec("sudo $SH_EASYPORTAL program_changer_variablen");
}
include('includes/variablen_pchanger.php'); 

if ($button != "") {
  // echo "button: '$button'";
  exec("sudo $SH_BEFEHL $pcscript $button");
}

echo "<div id='kopf2'>Tools - Program-Changer</div>";

echo "<div id='inhalt'>";
echo "<center>";
include('includes/subnavi_tools.php'); 

echo "<h2>Program-Changer</h2>";

// *************************************** Container Begin ****************************************************
container_begin(1, "Befehle.png", "Program-Changer");

echo "<center>";
for ($i = 0; $i < 99; $i++) {
  if (isset($name_button[$i])) {
    echo "<a href='".$_SERVER['PHP_SELF']."?button=$i' onclick=\"wait('WaitSeite')\"><img src='bild.php?url=$picture_button[$i]' onmouseover=\"Tip('$name_button[$i]')\" class='pc_size'></a>";
  }
}
echo "</center>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

echo "<font color='white'>";
include('includes/fuss.php'); 
?>
