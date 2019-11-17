<?php
include('includes/kopf.php'); 
include('includes/function_rec_countdown.php');

echo "<div id='kopf2'>$txt_kopf2</div>";
echo "<div id='inhalt'>";
echo "<center>";
include('includes/subnavi_einstellungen.php'); 
echo "<h2>$txt_ueberschrift</h2>";

// *************************************** Container Begin ****************************************************
container_begin(1, "Info.png", $txt_ue_info);

echo "$txt_info1<br/><br/>";

echo "<table class='tab2'>";
echo "<tr><th><a href='addons.php'                 class='b'>$txt_addons</a>:</th>     <td>$txt_info_addons</td>";
echo "<tr><th><a href='channel.php'                class='b'>$txt_kanalliste</a>:</th> <td>$txt_info_channel</td>";
// echo "<tr><th><a href='netzwerk.php'               class='b'>$txt_netzwerk</a>:</th>   <td>$txt_info_lan</td>";
echo "<tr><th><a href='fernzugriff.php'            class='b'>$txt_fernzugriff</a>:</th><td>$txt_info_fernzugriff</td>";
echo "<tr><th><a href='systemeinstellungen.php'    class='b'>$txt_system</a>:</th>     <td>$txt_info_system</td>";
echo "<tr><th><a href='sonstige_einstellungen.php' class='b'>$txt_sonstiges</a>:</th>  <td>$txt_info_sonst</td>";
echo "<tr><th><a href='bootlogo.php'               class='b'>$txt_bootlogo</a>:</th>   <td>$txt_info_bootlogo</td>";
echo "<tr><th><a href='root_login.php'             class='b'>$txt_rootlogin</a>:</th>  <td>$txt_info_rootlogin</td>";
echo "</table>";

echo "<br/><font color='red'>$txt_info_warn_rot</font>&nbsp;$txt_info_warn2<br/>";
echo "<div id='rec_countdown'>$txt_zeit_berechnung</div><br/>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>