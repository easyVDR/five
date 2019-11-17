<?php
include('includes/kopf.php'); 

//if ($_POST[NETWORK_TYPE]) {
//echo "Daten aus Form<br/>";
//}

$NETWORK_TYPE     = safepost('NETWORK_TYPE');
$HOSTNAME         = safepost('HOSTNAME');

$NAMESERVER1_1    = safepost('NAMESERVER1_1');
$NAMESERVER1_2    = safepost('NAMESERVER1_2');
$NAMESERVER1_3    = safepost('NAMESERVER1_3');
$NAMESERVER1_4    = safepost('NAMESERVER1_4');
$NAMESERVER1      = "$NAMESERVER1_1.$NAMESERVER1_2.$NAMESERVER1_3.$NAMESERVER1_4";
if ($NAMESERVER1 == "...") {$NAMESERVER1 = "";}

$NAMESERVER2_1    = safepost('NAMESERVER2_1');
$NAMESERVER2_2    = safepost('NAMESERVER2_2');
$NAMESERVER2_3    = safepost('NAMESERVER2_3');
$NAMESERVER2_4    = safepost('NAMESERVER2_4');
$NAMESERVER2      = "$NAMESERVER2_1.$NAMESERVER2_2.$NAMESERVER2_3.$NAMESERVER2_4";
if ($NAMESERVER2 == "...") {$NAMESERVER2 = "";}

$NAMESERVER3_1    = safepost('NAMESERVER3_1');
$NAMESERVER3_2    = safepost('NAMESERVER3_2');
$NAMESERVER3_3    = safepost('NAMESERVER3_3');
$NAMESERVER3_4    = safepost('NAMESERVER3_4');
$NAMESERVER3      = "$NAMESERVER3_1.$NAMESERVER3_2.$NAMESERVER3_3.$NAMESERVER3_4";
if ($NAMESERVER3 == "...") {$NAMESERVER3 = "";}

$DOMAIN           = safepost('DOMAIN');
$SHUTDOWN_ETHERNET= safepost('SHUTDOWN_ETHERNET');
$WAKE_ON_LAN      = safepost('WAKE_ON_LAN');
$LAN_USE_DHCP     = safepost('LAN_USE_DHCP');

$LAN_IP_1    = safepost('LAN_IP_1');
$LAN_IP_2    = safepost('LAN_IP_2');
$LAN_IP_3    = safepost('LAN_IP_3');
$LAN_IP_4    = safepost('LAN_IP_4');
$LAN_IP      = "$LAN_IP_1.$LAN_IP_2.$LAN_IP_3.$LAN_IP_4";
if ($LAN_IP == "...") {$LAN_IP = "";}

$LAN_MASK_1    = safepost('LAN_MASK_1');
$LAN_MASK_2    = safepost('LAN_MASK_2');
$LAN_MASK_3    = safepost('LAN_MASK_3');
$LAN_MASK_4    = safepost('LAN_MASK_4');
$LAN_MASK      = "$LAN_MASK_1.$LAN_MASK_2.$LAN_MASK_3.$LAN_MASK_4";
if ($LAN_MASK == "...") {$LAN_MASK = "";}

$LAN_GATEWAY_1    = safepost('LAN_GATEWAY_1');
$LAN_GATEWAY_2    = safepost('LAN_GATEWAY_2');
$LAN_GATEWAY_3    = safepost('LAN_GATEWAY_3');
$LAN_GATEWAY_4    = safepost('LAN_GATEWAY_4');
$LAN_GATEWAY      = "$LAN_GATEWAY_1.$LAN_GATEWAY_2.$LAN_GATEWAY_3.$LAN_GATEWAY_4";
if ($LAN_GATEWAY == "...") {$LAN_GATEWAY = "";}

$WLAN_USE_DHCP    = safepost('WLAN_USE_DHCP');

$WLAN_IP_1    = safepost('WLAN_IP_1');
$WLAN_IP_2    = safepost('WLAN_IP_2');
$WLAN_IP_3    = safepost('WLAN_IP_3');
$WLAN_IP_4    = safepost('WLAN_IP_4');
$WLAN_IP      = "$WLAN_IP_1.$WLAN_IP_2.$WLAN_IP_3.$WLAN_IP_4";
if ($WLAN_IP == "...") {$WLAN_IP = "";}

$WLAN_MASK_1    = safepost('WLAN_MASK_1');
$WLAN_MASK_2    = safepost('WLAN_MASK_2');
$WLAN_MASK_3    = safepost('WLAN_MASK_3');
$WLAN_MASK_4    = safepost('WLAN_MASK_4');
$WLAN_MASK      = "$WLAN_MASK_1.$WLAN_MASK_2.$WLAN_MASK_3.$WLAN_MASK_4";
if ($WLAN_MASK == "...") {$WLAN_MASK = "";}

$WLAN_GATEWAY_1    = safepost('WLAN_GATEWAY_1');
$WLAN_GATEWAY_2    = safepost('WLAN_GATEWAY_2');
$WLAN_GATEWAY_3    = safepost('WLAN_GATEWAY_3');
$WLAN_GATEWAY_4    = safepost('WLAN_GATEWAY_4');
$WLAN_GATEWAY      = "$WLAN_GATEWAY_1.$WLAN_GATEWAY_2.$WLAN_GATEWAY_3.$WLAN_GATEWAY_4";
if ($WLAN_GATEWAY == "...") {$WLAN_GATEWAY = "";}

$WLAN_SSID        = safepost('WLAN_SSID');
$WLAN_WEP_MODE    = safepost('WLAN_WEP_MODE');
$WLAN_CHANNEL     = safepost('WLAN_CHANNEL');
$WLAN_SEC         = safepost('WLAN_SEC');
$WLAN_WEP_KEYMODE = safepost('WLAN_WEP_KEYMODE');
$WLAN_WEP_KEY_1   = safepost('WLAN_WEP_KEY_1');
$WLAN_WEP_KEY_2   = safepost('WLAN_WEP_KEY_2');
$WLAN_WEP_KEY_3   = safepost('WLAN_WEP_KEY_3');
$WLAN_WEP_KEY_4   = safepost('WLAN_WEP_KEY_4');
$WLAN_DEFAULT_WEP_KEY = safepost('WLAN_DEFAULT_WEP_KEY');
$WLAN_WPA_CONFIG_FILE = safepost('WLAN_WPA_CONFIG_FILE');
$WLAN_WPA_DRIVER  = safepost('WLAN_WPA_DRIVER');
$WLAN_WPA_EAPOL   = safepost('WLAN_WPA_EAPOL');
$WLAN_WPA_APSCAN  = safepost('WLAN_WPA_APSCAN');
$WLAN_WPA_PROTO   = safepost('WLAN_WPA_PROTO');
$WLAN_WPA_PAIRWISE  = safepost('WLAN_WPA_PAIRWISE');
$WLAN_WPA_GROUP   = safepost('WLAN_WPA_GROUP');
$WLAN_WPA_PASSPHRASETYPE = safepost('WLAN_WPA_PASSPHRASETYPE');
$WLAN_WPA_PASSPHRASE     = safepost('WLAN_WPA_PASSPHRASE');

//if ($_SERVER["REQUEST_METHOD"] == "POST") {

if (safepost('submit2')) {
  echo "Wird gespeichert<br/>";
  exec("sudo $SH_EASYPORTAL stopvdr");
  exec("sudo $SH_EASYPORTAL setsysconfig NETWORK_TYPE $NETWORK_TYPE");
  exec("sudo $SH_EASYPORTAL setsysconfig HOSTNAME $HOSTNAME");
  exec("sudo $SH_EASYPORTAL setsysconfig NAMESERVER1 $NAMESERVER1");
  exec("sudo $SH_EASYPORTAL setsysconfig NAMESERVER2 $NAMESERVER2");
  exec("sudo $SH_EASYPORTAL setsysconfig NAMESERVER3 $NAMESERVER3");
  exec("sudo $SH_EASYPORTAL setsysconfig DOMAIN $DOMAIN");
  exec("sudo $SH_EASYPORTAL setsysconfig SHUTDOWN_ETHERNET $SHUTDOWN_ETHERNET");
  exec("sudo $SH_EASYPORTAL setsysconfig WAKE_ON_LAN $WAKE_ON_LAN");
  exec("sudo $SH_EASYPORTAL setsysconfig LAN_USE_DHCP $LAN_USE_DHCP");
  exec("sudo $SH_EASYPORTAL setsysconfig LAN_IP $LAN_IP");
  exec("sudo $SH_EASYPORTAL setsysconfig LAN_MASK $LAN_MASK");
  exec("sudo $SH_EASYPORTAL setsysconfig LAN_GATEWAY $LAN_GATEWAY");
  exec("sudo $SH_EASYPORTAL setsysconfig WLAN_USE_DHCP $WLAN_USE_DHCP");
  exec("sudo $SH_EASYPORTAL setsysconfig WLAN_IP $WLAN_IP");
  exec("sudo $SH_EASYPORTAL setsysconfig WLAN_MASK $WLAN_MASK");
  exec("sudo $SH_EASYPORTAL setsysconfig WLAN_GATEWAY $WLAN_GATEWAY");
  exec("sudo $SH_EASYPORTAL setsysconfig WLAN_SSID $WLAN_SSID");
  exec("sudo $SH_EASYPORTAL setsysconfig WLAN_WEP_MODE $WLAN_WEP_MODE");
  exec("sudo $SH_EASYPORTAL setsysconfig WLAN_CHANNEL $WLAN_CHANNEL");
  exec("sudo $SH_EASYPORTAL setsysconfig WLAN_SEC $WLAN_SEC");
  exec("sudo $SH_EASYPORTAL setsysconfig WLAN_WEP_KEYMODE $WLAN_WEP_KEYMODE");
  exec("sudo $SH_EASYPORTAL setsysconfig WLAN_WEP_KEY_1 $WLAN_WEP_KEY_1");
  exec("sudo $SH_EASYPORTAL setsysconfig WLAN_WEP_KEY_2 $WLAN_WEP_KEY_2");
  exec("sudo $SH_EASYPORTAL setsysconfig WLAN_WEP_KEY_3 $WLAN_WEP_KEY_3");
  exec("sudo $SH_EASYPORTAL setsysconfig WLAN_WEP_KEY_4 $WLAN_WEP_KEY_4");
  exec("sudo $SH_EASYPORTAL setsysconfig WLAN_DEFAULT_WEP_KEY $WLAN_DEFAULT_WEP_KEY");
  exec("sudo $SH_EASYPORTAL setsysconfig WLAN_WPA_CONFIG_FILE $WLAN_WPA_CONFIG_FILE");
  exec("sudo $SH_EASYPORTAL setsysconfig WLAN_WPA_DRIVER $WLAN_WPA_DRIVER");
  exec("sudo $SH_EASYPORTAL setsysconfig WLAN_WPA_EAPOL $WLAN_WPA_EAPOL");
  exec("sudo $SH_EASYPORTAL setsysconfig WLAN_WPA_APSCAN $WLAN_WPA_APSCAN");
  exec("sudo $SH_EASYPORTAL setsysconfig WLAN_WPA_PROTO $WLAN_WPA_PROTO");
  exec("sudo $SH_EASYPORTAL setsysconfig WLAN_WPA_PAIRWISE $WLAN_WPA_PAIRWISE");
  exec("sudo $SH_EASYPORTAL setsysconfig WLAN_WPA_GROUP $WLAN_WPA_GROUP");
  exec("sudo $SH_EASYPORTAL setsysconfig WLAN_WPA_PASSPHRASETYPE $WLAN_WPA_PASSPHRASETYPE");
  exec("sudo $SH_EASYPORTAL setsysconfig WLAN_WPA_PASSPHRASE $WLAN_WPA_PASSPHRASE");
  exec("sudo $SH_BEFEHL /usr/lib/vdr/easyvdr-set-settings network");
  exec("sudo $SH_EASYPORTAL startvdr");
//  echo "<h2>Werte wurden abgespeichert</h2>\n";              
} 


if (!safepost('NETWORK_TYPE')) {
  $lines = file ('/var/lib/vdr/sysconfig');
  foreach ($lines as $line) {
    // Netzwerkeinstellungen Allgemein
    if(strpos($line, 'NETWORK_TYPE=')            !== false)  { $NETWORK_TYPE  = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'HOSTNAME=')                !== false)  { $HOSTNAME      = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'NAMESERVER1=')             !== false)  { $NAMESERVER1   = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'NAMESERVER2=')             !== false)  { $NAMESERVER2   = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'NAMESERVER3=')             !== false)  { $NAMESERVER3   = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'DOMAIN=')                  !== false)  { $DOMAIN        = trim(str_replace ('"', '', strstr($line, '"'))); }

    $NAMESERVER1_A = explode('.', $NAMESERVER1); 
    if (!isset($NAMESERVER1_A[0])) {$NAMESERVER1_A[0] = "";}
    if (!isset($NAMESERVER1_A[1])) {$NAMESERVER1_A[1] = "";}
    if (!isset($NAMESERVER1_A[2])) {$NAMESERVER1_A[2] = "";}
    if (!isset($NAMESERVER1_A[3])) {$NAMESERVER1_A[3] = "";}
    $NAMESERVER1_1 = $NAMESERVER1_A[0];   $NAMESERVER1_2 = $NAMESERVER1_A[1];   $NAMESERVER1_3 = $NAMESERVER1_A[2];   $NAMESERVER1_4 = $NAMESERVER1_A[3];
    $NAMESERVER2_A = explode('.', $NAMESERVER2); 
    if (!isset($NAMESERVER2_A[0])) {$NAMESERVER2_A[0] = "";}
    if (!isset($NAMESERVER2_A[1])) {$NAMESERVER2_A[1] = "";}
    if (!isset($NAMESERVER2_A[2])) {$NAMESERVER2_A[2] = "";}
    if (!isset($NAMESERVER2_A[3])) {$NAMESERVER2_A[3] = "";}
    $NAMESERVER2_1 = $NAMESERVER2_A[0];   $NAMESERVER2_2 = $NAMESERVER2_A[1];   $NAMESERVER2_3 = $NAMESERVER2_A[2];   $NAMESERVER2_4 = $NAMESERVER2_A[3];
    $NAMESERVER3_A = explode('.', $NAMESERVER3); 
    if (!isset($NAMESERVER3_A[0])) {$NAMESERVER3_A[0] = "";}
    if (!isset($NAMESERVER3_A[1])) {$NAMESERVER3_A[1] = "";}
    if (!isset($NAMESERVER3_A[2])) {$NAMESERVER3_A[2] = "";}
    if (!isset($NAMESERVER3_A[3])) {$NAMESERVER3_A[3] = "";}
    $NAMESERVER3_1 = $NAMESERVER3_A[0];   $NAMESERVER3_2 = $NAMESERVER3_A[1];   $NAMESERVER3_3 = $NAMESERVER3_A[2];   $NAMESERVER3_4 = $NAMESERVER3_A[3];
    
    // LAN-Einstellungen
    if(strpos($line, 'SHUTDOWN_ETHERNET=')       !== false)  { $SHUTDOWN_ETHERNET     = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'WAKE_ON_LAN=')             !== false)  { $WAKE_ON_LAN           = trim(str_replace ('"', '', strstr($line, '"'))); }
    if((strpos($line, 'LAN_USE_DHCP=')  !== false) AND ((strpos($line, 'WLAN_USE_DHCP=')) === false)) { $LAN_USE_DHCP  = trim(str_replace ('"', '', strstr($line, '"'))); }
    if((strpos($line, 'LAN_IP=')        !== false) AND ((strpos($line, 'WLAN_IP='))       === false)) { $LAN_IP        = trim(str_replace ('"', '', strstr($line, '"'))); } 
    if((strpos($line, 'LAN_MASK=')      !== false) AND ((strpos($line, 'WLAN_MASK='))     === false)) { $LAN_MASK      = trim(str_replace ('"', '', strstr($line, '"'))); }
    if((strpos($line, 'LAN_GATEWAY=')   !== false) AND ((strpos($line, 'WLAN_GATEWAY='))  === false)) { $LAN_GATEWAY   = trim(str_replace ('"', '', strstr($line, '"'))); }
    $LAN_IP_A = explode('.', $LAN_IP); 
    if (!isset($LAN_IP_A[0])) {$LAN_IP_A[0] = "";}
    if (!isset($LAN_IP_A[1])) {$LAN_IP_A[1] = "";}
    if (!isset($LAN_IP_A[2])) {$LAN_IP_A[2] = "";}
    if (!isset($LAN_IP_A[3])) {$LAN_IP_A[3] = "";}
    $LAN_IP_1 = $LAN_IP_A[0];   $LAN_IP_2 = $LAN_IP_A[1];   $LAN_IP_3 = $LAN_IP_A[2];   $LAN_IP_4 = $LAN_IP_A[3];
    $LAN_MASK_A = explode('.', $LAN_MASK); 
    if (!isset($LAN_MASK_A[0])) {$LAN_MASK_A[0] = "";}
    if (!isset($LAN_MASK_A[1])) {$LAN_MASK_A[1] = "";}
    if (!isset($LAN_MASK_A[2])) {$LAN_MASK_A[2] = "";}
    if (!isset($LAN_MASK_A[3])) {$LAN_MASK_A[3] = "";}
    $LAN_MASK_1 = $LAN_MASK_A[0];   $LAN_MASK_2 = $LAN_MASK_A[1];   $LAN_MASK_3 = $LAN_MASK_A[2];   $LAN_MASK_4 = $LAN_MASK_A[3];
    $LAN_GATEWAY_A = explode('.', $LAN_GATEWAY); 
    if (!isset($LAN_GATEWAY_A[0])) {$LAN_GATEWAY_A[0] = "";}
    if (!isset($LAN_GATEWAY_A[1])) {$LAN_GATEWAY_A[1] = "";}
    if (!isset($LAN_GATEWAY_A[2])) {$LAN_GATEWAY_A[2] = "";}
    if (!isset($LAN_GATEWAY_A[3])) {$LAN_GATEWAY_A[3] = "";}
    $LAN_GATEWAY_1 = $LAN_GATEWAY_A[0];   $LAN_GATEWAY_2 = $LAN_GATEWAY_A[1];   $LAN_GATEWAY_3 = $LAN_GATEWAY_A[2];   $LAN_GATEWAY_4 = $LAN_GATEWAY_A[3];
    
    // W-Lan-Einstellungen
    if(strpos($line, 'WLAN_USE_DHCP=')           !== false)  { $WLAN_USE_DHCP         = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'WLAN_IP=')                 !== false)  { $WLAN_IP               = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'WLAN_MASK=')               !== false)  { $WLAN_MASK             = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'WLAN_GATEWAY=')            !== false)  { $WLAN_GATEWAY          = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'WLAN_SSID=')               !== false)  { $WLAN_SSID             = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'WLAN_WEP_MODE=')           !== false)  { $WLAN_WEP_MODE         = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'WLAN_CHANNEL=')            !== false)  { $WLAN_CHANNEL          = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'WLAN_SEC=')                !== false)  { $WLAN_SEC              = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'WLAN_WEP_KEYMODE=')        !== false)  { $WLAN_WEP_KEYMODE      = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'WLAN_WEP_KEY_1=')          !== false)  { $WLAN_WEP_KEY_1        = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'WLAN_WEP_KEY_2=')          !== false)  { $WLAN_WEP_KEY_2        = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'WLAN_WEP_KEY_3=')          !== false)  { $WLAN_WEP_KEY_3        = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'WLAN_WEP_KEY_4=')          !== false)  { $WLAN_WEP_KEY_4        = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'WLAN_DEFAULT_WEP_KEY=')    !== false)  { $WLAN_DEFAULT_WEP_KEY  = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'WLAN_WPA_CONFIG_FILE=')    !== false)  { $WLAN_WPA_CONFIG_FILE  = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'WLAN_WPA_DRIVER=')         !== false)  { $WLAN_WPA_DRIVER       = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'WLAN_WPA_EAPOL=')          !== false)  { $WLAN_WPA_EAPOL        = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'WLAN_WPA_APSCAN=')         !== false)  { $WLAN_WPA_APSCAN       = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'WLAN_WPA_PROTO=')          !== false)  { $WLAN_WPA_PROTO        = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'WLAN_WPA_PAIRWISE=')       !== false)  { $WLAN_WPA_PAIRWISE     = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'WLAN_WPA_GROUP=')          !== false)  { $WLAN_WPA_GROUP        = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'WLAN_WPA_PASSPHRASETYPE=') !== false)  { $WLAN_WPA_PASSPHRASETYPE = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'WLAN_WPA_PASSPHRASE=')     !== false)  { $WLAN_WPA_PASSPHRASE   = trim(str_replace ('"', '', strstr($line, '"'))); }
    $WLAN_IP_A = explode('.', $WLAN_IP); 
    if (!isset($WLAN_IP_A[0])) {$WLAN_IP_A[0] = "";}
    if (!isset($WLAN_IP_A[1])) {$WLAN_IP_A[1] = "";}
    if (!isset($WLAN_IP_A[2])) {$WLAN_IP_A[2] = "";}
    if (!isset($WLAN_IP_A[3])) {$WLAN_IP_A[3] = "";}
    $WLAN_IP_1 = $WLAN_IP_A[0];   $WLAN_IP_2 = $WLAN_IP_A[1];   $WLAN_IP_3 = $WLAN_IP_A[2];   $WLAN_IP_4 = $WLAN_IP_A[3];
    $WLAN_MASK_A = explode('.', $WLAN_MASK); 
    if (!isset($WLAN_MASK_A[0])) {$WLAN_MASK_A[0] = "";}
    if (!isset($WLAN_MASK_A[1])) {$WLAN_MASK_A[1] = "";}
    if (!isset($WLAN_MASK_A[2])) {$WLAN_MASK_A[2] = "";}
    if (!isset($WLAN_MASK_A[3])) {$WLAN_MASK_A[3] = "";}
    $WLAN_MASK_1 = $WLAN_MASK_A[0];   $WLAN_MASK_2 = $WLAN_MASK_A[1];   $WLAN_MASK_3 = $WLAN_MASK_A[2];   $WLAN_MASK_4 = $WLAN_MASK_A[3];
    $WLAN_GATEWAY_A = explode('.', $WLAN_GATEWAY); 
    if (!isset($WLAN_GATEWAY_A[0])) {$WLAN_GATEWAY_A[0] = "";}
    if (!isset($WLAN_GATEWAY_A[1])) {$WLAN_GATEWAY_A[1] = "";}
    if (!isset($WLAN_GATEWAY_A[2])) {$WLAN_GATEWAY_A[2] = "";}
    if (!isset($WLAN_GATEWAY_A[3])) {$WLAN_GATEWAY_A[3] = "";}
    $WLAN_GATEWAY_1 = $WLAN_GATEWAY_A[0];   $WLAN_GATEWAY_2 = $WLAN_GATEWAY_A[1];   $WLAN_GATEWAY_3 = $WLAN_GATEWAY_A[2];   $WLAN_GATEWAY_4 = $WLAN_GATEWAY_A[3];

  }
}


echo "<form action='".$_SERVER['PHP_SELF']."' method='post'>";

echo "<div id='kopf2'><a href='einstellungen.php' class='w'>$txt_einstellungen</a> - $txt_kopf2</div>";

echo "<div id='inhalt'>";
echo "<center>\n";

include('includes/subnavi_einstellungen.php'); 
echo "<h2>$txt_ueberschrift</h2>";

echo "<div class='links'>";

// *************************************** Container Begin 'Netzwerkeinstellungen allgemein' ****************************************************
container_begin(2, "Lan.png", $txt_ue_lan_allg);

echo "<table border='0' cellspacing='2' width='100%'>\n";
echo "  <tr>";
echo "    <td>$txt_lan_art</td>";
echo "    <td width='260px'><select name='NETWORK_TYPE' size='1' style='width: 240px'>";
echo "            <option ";
if ($NETWORK_TYPE == "LAN"){echo " selected";}
echo                  ">Lan</option>";
echo "            <option ";
if ($NETWORK_TYPE == "W-Lan"){echo " selected";}
echo                  ">W-Lan</option>";
echo "            <option ";
if ($NETWORK_TYPE == "Lan und W-Lan"){echo " selected";}
echo                  ">Lan und W-Lan</option>";
echo "            <option ";
if ($NETWORK_TYPE == "Debian-Standard"){echo " selected";}
echo                  ">Debian-Standard</option>";
echo "         </select>";
echo "   </td>";
echo "  </tr>\n";
echo "  <tr>";
echo "  <td>$txt_hostname</td>";
echo "    <td><input type='text' name='HOSTNAME' size='37' value='$HOSTNAME'></td>";
echo "  </tr>\n";
echo "  <tr>";
echo "    <td>$txt_nameserver1</td>";
echo "    <td><input type='text' name='NAMESERVER1_1' size='3' value='$NAMESERVER1_1'>
            . <input type='text' name='NAMESERVER1_2' size='3' value='$NAMESERVER1_2'>
            . <input type='text' name='NAMESERVER1_3' size='3' value='$NAMESERVER1_3'>
            . <input type='text' name='NAMESERVER1_4' size='3' value='$NAMESERVER1_4'></td>";
echo "  </tr>\n";
echo "  <tr>";
echo "    <td>$txt_nameserver2</td>";
echo "    <td><input type='text' name='NAMESERVER2_1' size='3' value='$NAMESERVER2_1'>
            . <input type='text' name='NAMESERVER2_2' size='3' value='$NAMESERVER2_2'>
            . <input type='text' name='NAMESERVER2_3' size='3' value='$NAMESERVER2_3'>
            . <input type='text' name='NAMESERVER2_4' size='3' value='$NAMESERVER2_4'></td>";
echo "  </tr>\n";
echo "  <tr>";
echo "    <td>$txt_nameserver3</td>";
echo "    <td><input type='text' name='NAMESERVER3_1' size='3' value='$NAMESERVER3_1'>
            . <input type='text' name='NAMESERVER3_2' size='3' value='$NAMESERVER3_2'>
            . <input type='text' name='NAMESERVER3_3' size='3' value='$NAMESERVER3_3'>
            . <input type='text' name='NAMESERVER3_4' size='3' value='$NAMESERVER3_4'></td>";
echo "  </tr>\n";
echo "  <tr>";
echo "    <td>$txt_domain</td>";
echo "    <td><input type='text' name='DOMAIN' size='37' value='$DOMAIN'></td>";
echo "  </tr>\n";
echo "</table>";

container_end();
// *************************************** Container Ende ****************************************************

// *************************************** Container Begin 'LAN-Einstellungen' ****************************************************
container_begin(2, "Lan.png", $txt_ue_lan);

echo "<table border='0' cellspacing='2' width='100%'>\n";
echo "  <tr>";
echo "    <td>$txt_lan_by_reboot</td>";
if ($SHUTDOWN_ETHERNET == "yes"){$checked_y = " checked"; } else {$checked_y = ""; }
if ($SHUTDOWN_ETHERNET == "no") {$checked_n = " checked"; } else {$checked_n = ""; }
echo "    <td width='260px'>$txt_yes <input type='radio' name='SHUTDOWN_ETHERNET' value='yes' ".$checked_y.">&nbsp;&nbsp; ";
echo "        $txt_no <input type='radio' name='SHUTDOWN_ETHERNET' value='no' ".$checked_n."></td>";
echo "  </tr>\n";
echo "  <tr>";
echo "    <td>$txt_wol</td>";
if ($WAKE_ON_LAN == "yes"){$checked_y = " checked"; } else {$checked_y = ""; }
if ($WAKE_ON_LAN == "no") {$checked_n = " checked"; } else {$checked_n = ""; }
echo "    <td>$txt_yes <input type='radio' name='WAKE_ON_LAN' value='yes' ".$checked_y.">&nbsp;&nbsp; ";
echo "        $txt_no <input type='radio' name='WAKE_ON_LAN' value='no' ".$checked_n."></td>";
echo "  </tr>\n";
echo "  <tr>";
echo "    <td>$txt_lan_dhcp</td>";
if ($LAN_USE_DHCP == "yes"){$checked_y = " checked";} else {$checked_y = ""; }
if ($LAN_USE_DHCP == "no") {$checked_n = " checked";} else {$checked_n = ""; }
echo "    <td>$txt_yes <input type='radio' name='LAN_USE_DHCP' value='yes' ".$checked_y.">&nbsp;&nbsp; $txt_no <input type='radio' name='LAN_USE_DHCP' value='no' ".$checked_n."></td>";
echo "  </tr>\n";
echo "  <tr>";
echo "    <td>$txt_lan_ip</td>";
echo "    <td style='white-space: nowrap;'><input type='text' name='LAN_IP_1' size='3' value='$LAN_IP_1'>
            . <input type='text' name='LAN_IP_2' size='3' value='$LAN_IP_2'>
            . <input type='text' name='LAN_IP_3' size='3' value='$LAN_IP_3'>
            . <input type='text' name='LAN_IP_4' size='3' value='$LAN_IP_4'></td>";
echo "  </tr>\n";
echo "  <tr>";
echo "    <td>$txt_lan_mask</td>";
echo "    <td style='white-space: nowrap;'><input type='text' name='LAN_MASK_1' size='3' value='$LAN_MASK_1'>
            . <input type='text' name='LAN_MASK_2' size='3' value='$LAN_MASK_2'>
            . <input type='text' name='LAN_MASK_3' size='3' value='$LAN_MASK_3'>
            . <input type='text' name='LAN_MASK_4' size='3' value='$LAN_MASK_4'></td>";
echo "  </tr>\n";
echo "  <tr>";
echo "    <td>$txt_lan_gateway</td>";
echo "    <td style='white-space: nowrap;'><input type='text' name='LAN_GATEWAY_1' size='3' value='$LAN_GATEWAY_1'>
            . <input type='text' name='LAN_GATEWAY_2' size='3' value='$LAN_GATEWAY_2'>
            . <input type='text' name='LAN_GATEWAY_3' size='3' value='$LAN_GATEWAY_3'>
            . <input type='text' name='LAN_GATEWAY_4' size='3' value='$LAN_GATEWAY_4'></td>";
echo "  </tr>\n";
echo "</table>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</div>";  // class='links'
echo "<div class='rechts'>";

// *************************************** Container Begin 'W-LAN-Einstellungen' ****************************************************
container_begin(2, "wifi.png", $txt_ue_wlan);

echo "<table border='0' cellspacing='2' width='100%'>\n";
echo "  <tr>";
echo "    <td>$txt_wlan_dhcp</td>";
if ($WLAN_USE_DHCP == "yes"){$checked_y = " checked";} else {$checked_y = ""; }
if ($WLAN_USE_DHCP == "no") {$checked_n = " checked";} else {$checked_n = ""; }
echo "    <td width='260px'>$txt_yes <input type='radio' name='WLAN_USE_DHCP' value='yes' ".$checked_y.">&nbsp;&nbsp; $txt_no <input type='radio' name='WLAN_USE_DHCP' value='no' ".$checked_n."></td>";
echo "  </tr>\n";
echo "  <tr>";
echo "    <td>$txt_wlan_ip</td>";
echo "    <td style='white-space: nowrap;'><input type='text' name='WLAN_IP_1' size='3' value='$WLAN_IP_1'>
            . <input type='text' name='WLAN_IP_2' size='3' value='$WLAN_IP_2'>
            . <input type='text' name='WLAN_IP_3' size='3' value='$WLAN_IP_3'>
            . <input type='text' name='WLAN_IP_4' size='3' value='$WLAN_IP_4'></td>";
echo "  </tr>\n";
echo "  <tr>";
echo "    <td>$txt_wlan_mask</td>";
echo "    <td style='white-space: nowrap;'><input type='text' name='WLAN_MASK_1' size='3' value='$WLAN_MASK_1'>
            . <input type='text' name='WLAN_MASK_2' size='3' value='$WLAN_MASK_2'>
            . <input type='text' name='WLAN_MASK_3' size='3' value='$WLAN_MASK_3'>
            . <input type='text' name='WLAN_MASK_4' size='3' value='$WLAN_MASK_4'></td>";
echo "  </tr>\n";
echo "  <tr>";
echo "    <td>$txt_wlan_gateway</td>";
echo "    <td style='white-space: nowrap;'><input type='text' name='WLAN_GATEWAY_1' size='3' value='$WLAN_GATEWAY_1'>
            . <input type='text' name='WLAN_GATEWAY_2' size='3' value='$WLAN_GATEWAY_2'>
            . <input type='text' name='WLAN_GATEWAY_3' size='3' value='$WLAN_GATEWAY_3'>
            . <input type='text' name='WLAN_GATEWAY_4' size='3' value='$WLAN_GATEWAY_4'></td>";
echo "  </tr>\n";
echo "  <tr>";
echo "    <td>$txt_wlan_ssid</td>";
echo "    <td><input type='text' name='WLAN_SSID' size='37' value='$WLAN_SSID'></td>";
echo "  </tr>\n";
echo "  <tr>";
echo "    <td>$txt_wlan_mode</td>";
if ($WLAN_WEP_MODE == "Managed"){$checked_m = " checked";} else {$checked_m = ""; }
if ($WLAN_WEP_MODE == "Ad-hoc") {$checked_a = " checked";} else {$checked_a = ""; }
echo "    <td>Managed <input type='radio' name='WLAN_WEP_MODE' size='37' value='Managed' ".$checked_m.">&nbsp;&nbsp; Ad-hoc <input type='radio' name='WLAN_WEP_MODE' value='Ad-hoc' ".$checked_a."></td>";
echo "  </tr>\n";
echo "  <tr>";
echo "    <td>$txt_wlan_channel</td>";
echo "    <td><input type='text' name='WLAN_CHANNEL' size='37' value='$WLAN_CHANNEL'></td>";
echo "  </tr>\n";
echo "  <tr>";
echo "    <td>$txt_wlan_sec</td>";
if (($WLAN_SEC == "keine") or ($WLAN_SEC == "KEINE")){$checked_k = " checked";} else {$checked_k = ""; }
if ($WLAN_SEC == "WEP") {$checked_wep = " checked";} else {$checked_wep = ""; }
if ($WLAN_SEC == "WPA") {$checked_wpa = " checked";} else {$checked_wpa = ""; }
echo "    <td>$txt_keine <input type='radio' onChange='this.form.submit()' name='WLAN_SEC' value='keine' ".$checked_k.">&nbsp;&nbsp; 
              WEP   <input type='radio' onChange='this.form.submit()' name='WLAN_SEC' value='WEP' ".$checked_wep.">&nbsp;&nbsp; 
              WPA   <input type='radio' onChange='this.form.submit()' name='WLAN_SEC' value='WPA' ".$checked_wpa."></td>";
echo "  </tr>\n";
echo "  <tr>";

if ($WLAN_SEC == "WEP") {
  echo "    <td>WLAN WEP Authentifizierung</td>";
  if ($WLAN_WEP_KEYMODE == "open")       {$checked_o = " checked";} else {$checked_o = ""; }
  if ($WLAN_WEP_KEYMODE == "restricted") {$checked_r = " checked";} else {$checked_r = ""; }
  echo "    <td>open <input type='radio' name='WLAN_WEP_KEYMODE' value='open' ".$checked_o.">&nbsp;&nbsp; restricted <input type='radio' name='WLAN_WEP_KEYMODE' value='restricted' ".$checked_r."></td>";
  echo "  <tr>";
  echo "    <td>WLAN WEP KEY 1</td>";
  echo "    <td><input type='text' name='WLAN_WEP_KEY_1' size='37' value='$WLAN_WEP_KEY_1'></td>";
  echo "  </tr>\n";
  echo "  <tr>";
  echo "    <td>WLAN WEP KEY 2</td>";
  echo "    <td><input type='text' name='WLAN_WEP_KEY_2' size='37' value='$WLAN_WEP_KEY_2'></td>";
  echo "  </tr>\n";
  echo "  <tr>";
  echo "    <td>WLAN WEP KEY 3</td>";
  echo "    <td><input type='text' name='WLAN_WEP_KEY_3' size='37' value='$WLAN_WEP_KEY_3'></td>";
  echo "  </tr>\n";
  echo "  <tr>";
  echo "    <td>WLAN WEP KEY 4</td>";
  echo "    <td><input type='text' name='WLAN_WEP_KEY_4' size='37' value='$WLAN_WEP_KEY_4'></td>";
  echo "  </tr>\n";
  echo "  <tr>";
  echo "    <td>WLAN DEFAULT WEP KEY</td>";
  if ($WLAN_DEFAULT_WEP_KEY == "1")   {$checked_1 = " checked";} else {$checked_1 = ""; }
  if ($WLAN_DEFAULT_WEP_KEY == "2")   {$checked_2 = " checked";} else {$checked_2 = ""; }
  if ($WLAN_DEFAULT_WEP_KEY == "3")   {$checked_3 = " checked";} else {$checked_3 = ""; }
  if ($WLAN_DEFAULT_WEP_KEY == "4")   {$checked_4 = " checked";} else {$checked_4 = ""; }
  echo "    <td>1 <input type='radio' name='WLAN_DEFAULT_WEP_KEY' value='1' ".$checked_1.">";
  echo "&nbsp;&nbsp; 2 <input type='radio' name='WLAN_DEFAULT_WEP_KEY' value='2' ".$checked_2.">";
  echo "&nbsp;&nbsp; 3 <input type='radio' name='WLAN_DEFAULT_WEP_KEY' value='3' ".$checked_3.">";
  echo "&nbsp;&nbsp; 4 <input type='radio' name='WLAN_DEFAULT_WEP_KEY' value='4' ".$checked_4."></td>";
  echo "  </tr>\n";
}
else {
  echo "<input type='hidden' name='WLAN_WEP_KEYMODE' value='$WLAN_WEP_KEYMODE'>";
  echo "<input type='hidden' name='WLAN_WEP_KEY_1'   value='$WLAN_WEP_KEY_1'>";
  echo "<input type='hidden' name='WLAN_WEP_KEY_2'   value='$WLAN_WEP_KEY_2'>";
  echo "<input type='hidden' name='WLAN_WEP_KEY_3'   value='$WLAN_WEP_KEY_3'>";
  echo "<input type='hidden' name='WLAN_WEP_KEY_4'   value='$WLAN_WEP_KEY_4'>";
  echo "<input type='hidden' name='WLAN_DEFAULT_WEP_KEY' value='$WLAN_DEFAULT_WEP_KEY'>";
}

if ($WLAN_SEC == "WPA") {
  echo "  <tr>";
  echo "    <td>WLAN WPA-Configfile</td>";
  echo "    <td><input type='text' name='WLAN_WPA_CONFIG_FILE' size='37' value='$WLAN_WPA_CONFIG_FILE'></td>";
  echo "  </tr>\n";
  echo "  <tr>";
  echo "    <td>WLAN WPA-Treiber</td>";
  echo "    <td><input type='text' name='WLAN_WPA_DRIVER' size='37' value='$WLAN_WPA_DRIVER'></td>";
  echo "  </tr>\n";
  echo "  <tr>";
  echo "    <td>WLAN WPA Eapol-Version</td>";
  echo "    <td><input type='text' name='WLAN_WPA_EAPOL' size='37' value='$WLAN_WPA_EAPOL'></td>";
  echo "  </tr>\n";
  echo "  <tr>";
  echo "    <td>WLAN WPA AP-Scan</td>";
  echo "    <td><input type='text' name='WLAN_WPA_APSCAN' size='37' value='$WLAN_WPA_APSCAN'></td>";
  echo "  </tr>\n";
  echo "  <tr>";
  echo "    <td>WLAN WPA-Protokol</td>";
  if ($WLAN_WPA_PROTO == "RSN")    {$checked_r = " checked";} else {$checked_r = ""; }
  if ($WLAN_WPA_PROTO == "WPA")    {$checked_w = " checked";} else {$checked_w = ""; }
  echo "    <td>RSN <input type='radio' name='WLAN_WPA_PROTO' value='RSN' ".$checked_r.">&nbsp;&nbsp; WPA <input type='radio' name='WLAN_WPA_PROTO' value='WPA' ".$checked_w."></td>";
  echo "  </tr>\n";
  echo "  <tr>";
  echo "    <td>WLAN WPA Pairwise</td>";
  if ($WLAN_WPA_PAIRWISE == "CCMP")      {$checked_c = " checked";}  else {$checked_c  = ""; }
  if ($WLAN_WPA_PAIRWISE == "CCMP TKIP") {$checked_ct = " checked";} else {$checked_ct = ""; }
  if ($WLAN_WPA_PAIRWISE == "TKIP")      {$checked_t = " checked";}  else {$checked_t  = ""; }
  echo "<td> CCMP <input type='radio' name='WLAN_WPA_PAIRWISE' value='CCMP' ".$checked_c.">";
  echo "&nbsp;&nbsp; CCMP TKIP <input type='radio' name='WLAN_WPA_PAIRWISE' value='CCMP TKIP' ".$checked_ct.">";
  echo "&nbsp;&nbsp; TKIP <input type='radio' name='WLAN_WPA_PAIRWISE' value='TKIP' ".$checked_t."></td>";
  echo "  </tr>\n";
  echo "  <tr>";
  echo "    <td>WLAN WPA Group</td>";
  if ($WLAN_WPA_GROUP == "CCMP")      {$checked_c = " checked";}  else {$checked_c  = ""; }
  if ($WLAN_WPA_GROUP == "CCMP TKIP") {$checked_ct = " checked";} else {$checked_ct = ""; }
  if ($WLAN_WPA_GROUP == "TKIP")      {$checked_t = " checked";}  else {$checked_t  = ""; }
  echo "<td> CCMP <input type='radio' name='WLAN_WPA_GROUP' value='CCMP' ".$checked_c.">";
  echo "&nbsp;&nbsp; CCMP TKIP <input type='radio' name='WLAN_WPA_GROUP' value='CCMP TKIP' ".$checked_ct.">";
  echo "&nbsp;&nbsp; TKIP <input type='radio' name='WLAN_WPA_GROUP' value='TKIP' ".$checked_t."></td>";
  echo "  </tr>\n";
  echo "  <tr>";
  echo "    <td>WLAN WPA Passphrase-Art</td>";
  if ($WLAN_WPA_PASSPHRASETYPE == "ASCI")   {$checked_a = " checked";} else {$checked_a = ""; }
  if ($WLAN_WPA_PASSPHRASETYPE == "HEX")    {$checked_h = " checked";} else {$checked_h = ""; }
  echo "    <td>ASCI <input type='radio' name='WLAN_WPA_PASSPHRASETYPE' value='ASCI' ".$checked_a.">&nbsp;&nbsp; HEX <input type='radio' name='WLAN_WPA_PASSPHRASETYPE' value='HEX' ".$checked_h."></td>";
  echo "  </tr>\n";
  echo "  <tr>";
  echo "    <td>WLAN WPA Passphrase</td>";
  echo "    <td><input type='text' name='WLAN_WPA_PASSPHRASE' size='37' value='$WLAN_WPA_PASSPHRASE'></td>";
  echo "  </tr>\n\n";
}
else {
  echo "<input type='hidden' name='WLAN_WPA_CONFIG_FILE' value='$WLAN_WPA_CONFIG_FILE'>";
  echo "<input type='hidden' name='WLAN_WPA_DRIVER'      value='$WLAN_WPA_DRIVER'>";
  echo "<input type='hidden' name='WLAN_WPA_EAPOL'       value='$WLAN_WPA_EAPOL'>";
  echo "<input type='hidden' name='WLAN_WPA_APSCAN'      value='$WLAN_WPA_APSCAN'>";
  echo "<input type='hidden' name='WLAN_WPA_PROTO'       value='$WLAN_WPA_PROTO'>";
  echo "<input type='hidden' name='WLAN_WPA_PAIRWISE'    value='$WLAN_WPA_PAIRWISE'>";
  echo "<input type='hidden' name='WLAN_WPA_GROUP'       value='$WLAN_WPA_GROUP'>";
  echo "<input type='hidden' name='WLAN_WPA_PASSPHRASETYPE' value='$WLAN_WPA_PASSPHRASETYPE'>";
  echo "<input type='hidden' name='WLAN_WPA_PASSPHRASE'  value='$WLAN_WPA_PASSPHRASE'>";
}
echo "</table>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</div>";  // class='rechts'
echo "<div class='clear'></div>"; // Ende 2-spaltig

echo "<input type='submit' name='submit2' value='$txt_b_save' onclick=\"wait('WaitSave')\">";
//echo "<input type='submit' name='submit' value='neu laden' onclick=\"wait('WaitSave')\">";

echo "</form>";
echo "</div>"; // Inhalt

include('includes/fuss.php'); 
?>