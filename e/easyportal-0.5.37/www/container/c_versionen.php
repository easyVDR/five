<?php
// Versionen

$EASYPORTAL_VERSION = exec('dpkg -l easyportal | grep ii | awk \'{ print $3 }\'');
$API                = shell_exec("$SH_EASYPORTAL vdrver");
$KERNEL             = shell_exec("$SH_EASYPORTAL kernel");

// *************************************** Container Begin 'Versionen' ****************************************************
container_begin(2, "Info.png", $txt_ue_versionen);

echo "<center>";
echo "<table class='tab2'>";
echo "  <tr><th align='right' width=50%'>easyVDR-Version:</th><td align='left' id='easyvdr_version'>wird ermittelt !!!</td></tr>";
echo "  <tr><th align='right'>easyVDR-Installations-DVD:</th> <td align='left' id='isover'         >wird ermittelt !!!</td></tr>";
echo "  <tr><th align='right'>easyPortal-Version:</th>        <td align='left'>$EASYPORTAL_VERSION</td></tr>";
echo "  <tr><th align='right'>VDR-Version:</th>               <td align='left'>$API</td></tr>";
echo "  <tr><th align='right'>Kernel-Version:</th>            <td align='left'>$KERNEL</td></tr>";
echo "  <tr><th align='right'>media-build-experimental-Version:</th><td align='left' id='dkms'     >wird ermittelt !!!</td></tr>";

echo "</table>";
echo "</center>";

container_end();
// *************************************** Container Ende ****************************************************

?>