<?php
include('includes/kopf.php'); 
include('includes/function_redirect.php');

// Erster Aufruf - Datenbank "portal" erzeugen, falls nicht vorhanden.
/*
if (!file_exists("/var/www/databases/portal")) {
  $db = new Database(ROOT_DATABASE);
  $db->executeQuery("CREATE DATABASE portal");
}
*/
// Tabelle "netzwerkgeraete" erzeugen, falls nicht vorhanden.
if (!file_exists("/var/www/databases/portal/netzwerkgeraete.txt")) {
  //$db = new Database("portal");
  $db->executeQuery("CREATE TABLE netzwerkgeraete (IP str, Name str, MAC str, Hersteller str, istVDR str, VDRVersion str, streamdevserver str, streamdevclient str, peer str ,remotetimer str ,remoteosd str)");
}  

echo "<div id='kopf2'>Tools - Netzwerkgeräte Update</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_tools.php'); 

echo "<h2>Netzwerk-Geräte Update</h2>";

// *************************************** Container Begin '' ****************************************************
container_begin(1, "Info.png", "Netzwerkgeräte werden gesucht");

echo "<h1>Bitte warten !!!</h1><br><br>";
echo "Es werden weitere Geräte gesucht.<br>";
echo "Nach Beendigung der Suche wird automatisch zur Übersicht weitergeleitet.<br><br>";

// Aktuelle IP
$aktuelle_IP = shell_exec("sudo $SH_BEFEHL ifconfig eth0 |grep 'inet ' |cut -d: -f2 |awk '{ print $1}'");
$txt_debug_meldungen[] = "Aktuelle IP: $aktuelle_IP";

// Range
$range = substr($_SERVER['SERVER_ADDR'],0,strrpos($_SERVER['SERVER_ADDR'],"."));
$txt_debug_meldungen[] = "Range: $range"; 

function update_set($set, $variable, $name) {
  if ($name!="") {
    if ($set != "") { $set = $set.", ";}
    $set = $set.$variable."='".$name."'";
  }
  return($set);
}

exec("sudo $SH_BEFEHL nmap -sL $range.1-254 |grep '($range' |cut -d' ' -f5-", $array_lan_all);
//print_r($array_lan_all);
foreach ($array_lan_all as $line) {
  $set = $on = $ip = $mac = $hersteller = $ist_vdr = $vdr_version = "";
  $streamdevserver_version = $streamdevclient_version = $peer_version = $remotetimers_version = $remoteosd_version = "";

  $stringposition = strpos($line, "(");
  $name = trim(substr($line, 0, $stringposition));
  // echo "Name: '$name'<br>";
  $ip = trim(str_replace(")", "", substr($line, $stringposition+1)));
  // echo "IP: '$ip'<br>";
  
  // Onlinestatus
  $on[0] = "";
  exec("sudo $SH_BEFEHL nmap -sP $ip |grep 'Host is up'", $on);
  // print_r($on);
  if ($on[1]!="") {
    // echo "Onlinestatus: Online<br>";

    // MAC-Adresse holen
    if ($ip == $aktuelle_IP) {     // echo "Lokaler Rechner<br>";
      $mac = trim(shell_exec("sudo $SH_BEFEHL netstat -ei |grep 'eth0' |cut -d' ' -f11"));              
    } else {                          // echo "Anderer Rechner<br>";
      $mac = shell_exec("sudo $SH_BEFEHL nmap -sP $ip |grep MAC |cut -d' ' -f3");
    }
    // echo "MAC: $mac<br/>";
    
    // Hersteller
    exec("sudo $SH_BEFEHL nmap -sP $ip |grep MAC |cut -d' ' -f4-", $hersteller);
    $hersteller = substr($hersteller[0], 1, strlen($hersteller[0]));
    $hersteller = substr($hersteller, 0, strlen($hersteller)-1);
    //echo "hersteller: $hersteller<br/>";
    
    // Prüfen auf VDR
    $ip2 = escapeshellcmd($ip); 
    $vdr_version = exec("sudo $SH_BEFEHL svdrpsend -d $ip2 test |grep VideoDiskRecorder |cut -d' ' -f5 |cut -d';' -f1");
    // echo "VDR-Version: '$vdr_version'<br>";
    if ($vdr_version == "") { 
      // ist kein VDR
      $ist_vdr                 = 0;
      $streamdevserver_version = "";
      $streamdevclient_version = "";
      $peer_version            = "";
      $remotetimers_version    = "";
      $remoteosd_version       = "";
    } else {
      // ist VDR
      $ist_vdr = 1;

      $streamdevserver_version_t = exec("sudo $SH_BEFEHL svdrpsend -d $ip2 PLUG streamdev-server |grep streamdev-server");
      if ($streamdevserver_version_t != str_replace("not found","",$streamdevserver_version_t)) {
         $streamdevserver_version = "-";
       } else {
         $streamdevserver_version_Array = explode(' ', $streamdevserver_version_t);
         $streamdevserver_version = $streamdevserver_version_Array[2];
       }
      
      $streamdevclient_version_t = exec("sudo $SH_BEFEHL svdrpsend -d $ip2 PLUG streamdev-client |grep streamdev-client");
      if ($streamdevclient_version_t != str_replace("not found","",$streamdevclient_version_t)) {
         $streamdevclient_version = "-";
       } else {
         $streamdevclient_version_Array = explode(' ', $streamdevclient_version_t);
         $streamdevclient_version = $streamdevclient_version_Array[2];
       }
 
      $peer_version_t = exec("sudo $SH_BEFEHL svdrpsend -d $ip2 PLUG peer |grep peer");
      if ($peer_version_t != str_replace("not found", "", $peer_version_t)) {
         $peer_version = "-";
       } else {
         $peer_version_Array = explode(' ', $peer_version_t);
         $peer_version = $peer_version_Array[2];
       }

      $remotetimers_version_t = exec("sudo $SH_BEFEHL svdrpsend -d $ip2 PLUG remotetimers |grep remotetimers");
      if ($remotetimers_version_t != str_replace("not found", "", $remotetimers_version_t)) {
         $remotetimers_version = "-";
       } else {
         $remotetimers_version_Array = explode(' ', $remotetimers_version_t);
         $remotetimers_version = $remotetimers_version_Array[2];
       }

      $remoteosd_version_t = exec("sudo $SH_BEFEHL svdrpsend -d $ip2 PLUG remoteosd |grep remoteosd");
      if ($remoteosd_version_t != str_replace("not found", "", $remoteosd_version_t)) {
         $remoteosd_version = "-";
       } else {
         $remoteosd_version_Array = explode(' ', $remoteosd_version_t);
         $remoteosd_version = $remoteosd_version_Array[2];
       }
    }
  }
  
  $resultSet = $db->executeQuery("SELECT * FROM netzwerkgeraete WHERE IP='$ip'");
  //echo "<br>Anz: ".$resultSet->getRowCount()." <br>";
  if ($resultSet->getRowCount() == 0) {
    // Einfügen
     //echo "INSERT INTO netzwerkgeraete(IP, Name, MAC, Hersteller, istVDR, VDRVersion, streamdevserver, streamdevclient, peer ,remotetimer ,remoteosd) 
     //                  VALUES ('$ip', '$name', '$mac', '$hersteller', '$ist_vdr', '$vdr_version', '$streamdevserver_version', '$streamdevclient_version', '$peer_version', '$remotetimers_version', '$remoteosd_version')<br/>";
    
    $db->executeQuery("INSERT INTO netzwerkgeraete(IP, Name, MAC, Hersteller, istVDR, VDRVersion, streamdevserver, streamdevclient, peer ,remotetimer ,remoteosd) 
                       VALUES ('$ip', '$name', '$mac', '$hersteller', '$ist_vdr', '$vdr_version', '$streamdevserver_version', '$streamdevclient_version', '$peer_version', '$remotetimers_version', '$remoteosd_version')");
  }
  if ($resultSet->getRowCount() == 1) {
    // Update
    $set = update_set($set, "Name", $name);
    $set = update_set($set, "MAC", $mac);
    $set = update_set($set, "Hersteller", $hersteller);
    $set = update_set($set, "istVDR", $ist_vdr);
    $set = update_set($set, "VDRVersion", $vdr_version);
    $set = update_set($set, "streamdevserver", $streamdevserver_version);
    $set = update_set($set, "streamdevclient", $streamdevclient_version);
    $set = update_set($set, "peer", $peer_version);
    $set = update_set($set, "remotetimer", $remotetimers_version);
    $set = update_set($set, "remoteosd", $remoteosd_version);
    
    // echo "UPDATE netzwerkgeraete SET $set WHERE IP='$ip'<br/>";
    $db->executeQuery("UPDATE netzwerkgeraete SET $set WHERE IP='$ip'");
  }
}

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

redirect('/netzwerk_geraete.php');
//echo "<a href='netzwerk_geraete.php'>weiter</a>";
include('includes/fuss.php'); 
?>
