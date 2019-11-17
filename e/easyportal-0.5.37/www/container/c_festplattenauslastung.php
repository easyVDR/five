<?php

if (safeget('aktion') == "startvdr") { 
  shell_exec("sudo $SH_EASYPORTAL startvdr");
  sleep(10);
  //redirect('/system.php');
}

// ################################## Container Festplattenauslastung ###################################################

// *************************************** Container Begin 'Festplattenauslaustung' ****************************************************
container_begin(2, "hdd.png", $txt_ue_festplattenauslastung);

echo "<table border='0' cellpadding='0' cellspacing='0' width='100%' align='center'>";
echo "<tr><td>";
$DEBUG="";
// Plattenauslastungen holen
exec("df -h | sort | grep /dev/[h,s,m] | awk '{ print $1 \";\" $6 \";\" $5 \";\" $3 \";\" $4 \";\" $2 }'", $HD_CHECK);
// Ergebnis Satzweise durchlaufen
$TOOLTIP = "";
foreach ($HD_CHECK as $Satz) {
  $Daten = explode(";",$Satz);
  // Plattentemperatur ermitteln
  $Temp = shell_exec("sudo $SH_EASYPORTAL hddtemp ".substr($Daten[0],0,8));
  // Debugmeldungen
  if ($DEBUG) {
    echo "<font color=red>Command: $SH_EASYPORTAL hddtemp ".substr($Daten[0],0,8)."<br>";
    echo "Search /nicht verfügbar/: " . preg_match("/nicht verfügbar/",$Temp)."<br>";
    echo "Search /sleeping/: " . preg_match("/sleeping/",$Temp)."<br>";
    echo "\$Temp: '".$Temp."'</font>";
  }
  // Stati der Platten ermitteln
  if (preg_match("/S.M.A.R.T nicht verfügbar/",$Temp)) {
    $Temp="";
  } else {
    if (preg_match("/sleeping/",$Temp)) {
      $Temp="<img src=images/sleeping.gif>";
    } else {
      if(strlen($Temp)==0 or strlen($Temp) > 6) {
        $Temp="";
      } else {
        $Temp = substr(ltrim($Temp),0,strlen($Temp-2))."&#176;C";
      }
    }
  }
  // Plattenmodell ermitteln
  $HDDNAME=shell_exec("sudo -u root $SH_EASYPORTAL hddname ".substr($Daten[0],0,8));
  if($Daten[2] == "100%") { $Colspan="1"; } else { $Colspan="2"; }
  
  // Tooltip zusammenbasteln
  echo "<table width='100%' class='tabBorder' onmouseover=\"Tip('$TOOLTIP',TITLE,'". trim($HDDNAME) ."')\">";
  echo "<tr>";
  echo "<td colspan='$Colspan'>";
  echo "<table border='0' width='100%'>";
  echo   "<tr>";
  echo     "<td width='60px' align='left' style='font-weight: bold;'>$Daten[0]:</td>";
  if ($Daten[1] == "/media/easyvdr01") { $Daten[1] = "/video0"; }
  echo     "<td align='left' width='110px'>$Daten[1]</td>";
  echo     "<td align='left' width='90px' style='color:#CC0000;'>Belegt: $Daten[3]B</td>";
  echo     "<td align='left' style='color:green;'>Frei: $Daten[4]B</td>";
  echo     "<td align='right'>$Temp</td>";
  echo   "</tr>";
  echo "</table>";

  echo "</td>";
  echo "</tr>";
  echo "<tr>";
  echo "<td class='balken' width='$Daten[2]' bgcolor='#CC0000'>";
  echo $Daten[2];
  echo "</td>";
  if ($Colspan == "2") {
     echo "<td class='balken' width='";
     echo 100 - substr($Daten[2],0,2);
     echo "%' bgcolor='green'>";
     echo 100 - substr($Daten[2],0,2);
     echo "%</td>";
  }
  echo "</tr></table>";
}
//$VDR = shell_exec("ps ax|grep -v grep|grep vdr &>/dev/null; echo $?");
$VDR = shell_exec("pidof vdr");

//if ($VDR != 1 ) {
if ($VDR != "" ) {
  exec("svdrpsend STAT disk | grep 250 | awk '{print \";\" $2 \";\" $3 \";\" $4 }'", $VDR_REC);
  $Daten  = explode(";",$VDR_REC[0]);
  $BELEGT = round(($Daten[1] - $Daten[2])/1024,1) . "GB";
  $GESAMT = round($Daten[1]/1024,1)."GB";
  $FREI   = round($Daten[2]/1024,1)."GB";

  if($Daten[3] == "100%") {
    $Colspan="1";
  }
  else {
    $Colspan="2";
  }
  $TOOLTIP="<table><tr><td>$txt_Gesamt:&nbsp;</td><td>$GESAMT</td></tr><tr><td>$txt_Belegt:</td><td>$BELEGT</td></tr><tr><td>$txt_Frei:</td><td>$FREI</td></tr></table>";

  echo "<br>";
  echo "<table border='0' width='100%' onmouseover=\"Tip('$TOOLTIP',TITLE,'Aufnahmekapazit&#228;t',WIDTH,150)\">";
  echo "<tr>";
  echo   "<td align='left' colspan='$Colspan'>$txt_Aufnahmekap: $GESAMT &nbsp;&nbsp;&nbsp;";
  echo     "<span style='color:#CC0000;'>$txt_Belegt: $BELEGT</span>&nbsp;&nbsp;&nbsp;";
  echo     "<span style='color:green;'>$txt_Frei: $FREI</span>";
  echo   "</td>";
  echo "</tr>";
  echo "<tr>";

  if($Daten[3] == "0%") {
 //    echo "<td class='balken' width='0%". 100 - substr($Daten[3],0,2) ."%' bgcolor='green'>";
    echo "<td class='balken' width='0%' bgcolor='green'>";
    echo 100 - substr($Daten[3],0,2) . "%";
  }
  else {
    echo "<td class='balken' width='$Daten[3]' bgcolor='#CC0000'>";
    echo $Daten[3];
    echo "</td>";
    if ($Colspan == "2") {
      echo "<td class='balken' width='0%";
      echo 100 - substr($Daten[3],0,2);
      echo "%' bgcolor='green'>";
      echo 100 - substr($Daten[3],0,2) . "%";
      echo "</td>";
    }
  }
  echo "</tr>";
  echo "</table>";
}
else {
  echo "<br/>";
  echo "<table cellspacing='5' width='100%' bgcolor='#FFFF99'>";
  echo "<tr>";
  echo   "<td><img src='images/warning.png' alt='Achtung'></td>";
  echo   "<td><b>$txt_error1<br/><br/>";
  echo       "<a onmouseover=\"Tip('$txt_tip_platten_start')\" href='system.php?aktion=startvdr'><button>$txt_b_vdr_start</button></a>";
  echo       "</b>";
  echo   "</td>";
  echo "</tr>";
  echo "</table>";
}

echo "</td>";
echo "</tr>";
echo "</table>";

container_end();
// *************************************** Container Ende ****************************************************
?>