<?php
// laufende Services

//$XBMC_pid           = exec("sudo $SH_BEFEHL pidof xbmc.bin");
//$XBMC_WEBIF_PORT    = exec("cat /home/easyvdr/.xbmc/userdata/guisettings.xml |grep webserverport |cut -d'>' -f2- |cut -d'<' -f1");
$XBMC_pid           = exec("sudo $SH_BEFEHL pidof kodi.bin");
$XBMC_WEBIF_PORT    = exec("cat /home/easyvdr/.kodi/userdata/guisettings.xml |grep webserverport |cut -d'>' -f2- |cut -d'<' -f1");
if ($XBMC_pid != "") {$XBMC_laeuft = "y";} else {$XBMC_laeuft = "n";}

if (safeget('xbmc')){
  $pcscript = "/usr/share/easyvdr/program-changer/program-changer-easy-portal.sh";
  if (safeget('xbmc') == 'start') {
     //exec("sudo $SH_BEFEHL sh /usr/share/easyvdr/program-changer/program-changer.sh 1");
     exec("sudo $SH_BEFEHL $pcscript 1");
     $XBMC_laeuft = "y";
  }
  if (safeget('xbmc') == 'stop') {
     //exec("sudo $SH_BEFEHL sh /usr/share/easyvdr/program-changer/program-changer.sh 0");
     exec("sudo $SH_BEFEHL $pcscript 0");
     $XBMC_laeuft = "n";
  }
}

// *************************************** Container Begin '' ****************************************************
container_begin(2, "Tools.png", $txt_ue_services);

echo "<center>";
echo "<table border='0' cellspacing='3'>";
$port = shell_exec("sudo $SH_EASYPORTAL port");
$IP = $_SERVER['SERVER_NAME'];

$resultSet = $db->executeQuery("SELECT * FROM dienste");
while($resultSet->next()) {
  $Aktiv   = "1";
  $Dienst  = $resultSet->getCurrentValueByName("Dienst");
  $Name    = $resultSet->getCurrentValueByName("Name");
  $Port    = $resultSet->getCurrentValueByName("Port");
  $Link    = $resultSet->getCurrentValueByName("Link");
  $Link    = str_replace("IP", $IP, $Link);
  $CheckID = $resultSet->getCurrentValueByName("CheckID");
  
  switch ($CheckID) {
  case "a":
    // VDR
//    echo "vdr-";
    $vdrpid = exec("sudo $SH_BEFEHL pidof vdr");
    if ($vdrpid != "") { $Aktiv = "0"; }
    break;
  case "b":
    // allgemein
//    echo "alg-";
    $Aktiv = shell_exec("ps ax|grep -v grep|grep $Dienst &>/dev/null; echo $?");
    break;
  }
  
  $Aktiv = trim($Aktiv);
//  echo "'$Aktiv'--";
  if ($Aktiv == 0){
    if ($Link == "") {
      echo "<tr onmouseover=\"Tip('$txt_tip_run')\"><td valign='top'><img src='images/greenled.png' alt='Aktiv'></td><td align='left'>&nbsp;$Name</td></tr>";
    }
    else {
      echo "<tr onmouseover=\"Tip('$txt_tip_run')\"><td valign='top'><img src='images/greenled.png' alt='Aktiv'></td><td align='left'>&nbsp;<a class='b' href='$Link' target='_blank'>$Name</a></td></tr>";
    }
  }
  if (($Aktiv == 1) AND ($Dienst == "vdr")){
    echo "<tr onmouseover=\"Tip('vdr läuft NICHT')\"><td valign='top'><img src='images/redled.png' alt='Nicht Aktiv' width='15' height='15'></td><td align='left'>&nbsp;$Name</td></tr>";
  }
}

if ($XBMC_laeuft == "y") {
   echo "<tr><td valign='top'><img src='images/greenled.png' alt='Aktiv'></td><td align='left'><a onmouseover=\"Tip('Kodi läuft...<br>zum KODI Web-Frontend')\" class='b' href='http://".$IP.":".$XBMC_WEBIF_PORT."' target='_blank'>&nbsp;KODI</a>";
   echo "&nbsp;&nbsp;(<a onmouseover=\"Tip('Kodi läuft<br>Klicken, um zum VDR zu wechseln')\" href=index.php?xbmc=stop class='b' onclick=\"wait('WaitXBMC')\">KODI Stoppen</a>)";
} else { 
   echo "<tr><td valign='top'><img src='images/redled.png' width='15' height='15' alt='Nicht Aktiv'></td><td align='left'>&nbsp;KODI";
   echo "&nbsp;&nbsp;(<a onmouseover=\"Tip('Kodi läuft nicht...<br>Klicken, um KODI zu starten')\" href=index.php?xbmc=start class='b' onclick=\"wait('WaitXBMC')\">KODI Starten</a>)";
}
/*
if ($XBMC_laeuft == "y") {
   echo "<tr onmouseover=\"Tip('$txt_tip_run')\"><td valign='top'><img src='images/greenled.png' alt='Aktiv'></td><td align='left'><a class='b' href='http://".$IP.":".$XBMC_WEBIF_PORT."' target='_blank'>&nbsp;XBMC</a>";
   echo "&nbsp;&nbsp;(<a href=index.php?xbmc=stop class='b' onclick=\"wait('WaitXBMC')\">XBMC Stoppen</a>)";
} else { 
   echo "<tr><td valign='top'><img src='images/redled.png' width='15' height='15' alt='Nicht Aktiv'></td><td align='left'>&nbsp;XBMC";
   echo "&nbsp;&nbsp;(<a href=index.php?xbmc=start class='b' onclick=\"wait('WaitXBMC')\">XBMC Starten</a>)";
}
*/
//echo $XBMC_pid ;
echo "</td></tr>";
echo "</table>";

echo "</center>";

container_end();
// *************************************** Container Ende ****************************************************
?>