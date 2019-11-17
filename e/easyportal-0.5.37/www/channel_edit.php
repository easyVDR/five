<?php
include('includes/kopf.php'); 
//include('includes/functions.inc');

$channel = safeget('channel');
//$channel = "_test-channels.conf";
if (safeget('lnr')!="") {
  $lnr_aktuell = safeget('lnr');
} else {
  $lnr_aktuell = 2;
}

echo "<div id='kopf2'><a href='einstellungen.php' class='w'>Einstellungen</a> - <a href='channel.php' class='w'>Kanalliste</a> - $txt_kopf2</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_einstellungen.php'); 
include('includes/subnavi_channel.php'); 

echo "<h2>$txt_ueberschrift: $channel</h2>";

echo "<div class='links'>";

// *************************************** Container Begin 'Kanalliste' ****************************************************
container_begin(2, "Info.png", "Kanalliste");

echo "<form action=".$_SERVER['PHP_SELF']." method='post'>\n";
echo "<div style='height:450px; overflow:auto;'>";
echo "<pre>";
$datei=fopen("/var/lib/vdr/channels/$channel","r");
$lnr=0;   // Line-Nummer
while (!feof($datei)) {
  $line = fgets($datei); 
  $lnr = $lnr + 1;
  if($line{0}==":") { 
    $line = "<font color='black'>".$line."</font>";
    echo $line;
  }
  else
  {
    $kanal = explode(';', $line);
    if ($lnr == $lnr_aktuell) { 
      echo "<a href='".$_SERVER['PHP_SELF']."?channel=$channel&lnr=$lnr'><font color='red'>".$kanal[0]."</font></a><br/>";
    }
    else {
      echo "<a class='b' href='".$_SERVER['PHP_SELF']."?channel=$channel&lnr=$lnr'>".$kanal[0]."</a><br/>";
    }
  }  
}
fclose($datei);
echo "</pre>";
echo "</div>"; 
echo "</form>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</div>";  // class='links'
echo "<div class='rechts'>";

// *************************************** Container Begin 'Kanal-Parameter' ****************************************************
container_begin(2, "Channels.png", "Kanal-Einstellungen");

echo "Zeile: $lnr_aktuell<br>";
echo "<br/>";

$datei=fopen("/var/lib/vdr/channels/$channel","r");
$lnr=0;   // Line-Nummer
while (!feof($datei)) {
  $line = fgets($datei); 
  $lnr = $lnr + 1;
  //  echo $lnr;
  if ($lnr == $lnr_aktuell) { 
    $line_array  = explode(';', $line);
    $stringposition = strpos($line, ";");
    $new_string = substr($line, $stringposition+1);
    $line_array2 = explode(':', $new_string);

    echo "<table class='tab1'>";
    echo "<tr><td onmouseover=\"Tip('$txt_tip_Kanalname')\" width='100px'>Kanalname</td>";
    echo "    <td>$line_array[0]</td>";
    echo "</tr>";
    echo "<tr><td onmouseover=\"Tip('$txt_tip_Provider')\">Serviceprovider</td>";
    echo "    <td>$line_array2[0]</td>";
    echo "</tr>";
    echo "<tr><td onmouseover=\"Tip('$txt_tip_Frequenz')\">Frequenz</td>";
    echo "    <td>$line_array2[1]</td>";
    echo "</tr>";
    echo "<tr><td onmouseover=\"Tip('$txt_tip_Parameter', WIDTH, 300)\">Parameter</td>";
    echo "    <td>$line_array2[2]</td>";
    echo "</tr>";
    echo "<tr><td onmouseover=\"Tip('$txt_tip_Signalquelle')\">Signalquelle</td>";
    echo "    <td>$line_array2[3]</td>";
    echo "</tr>";
    echo "<tr><td onmouseover=\"Tip('$txt_tip_Symbolrate')\">Symbolrate</td>";
    echo "    <td>$line_array2[4]</td>";
    echo "</tr>";
    echo "<tr><td onmouseover=\"Tip('$txt_tip_VPID')\">VPID</td>";
    echo "    <td>$line_array2[5]</td>";
    echo "</tr>";
    echo "<tr><td onmouseover=\"Tip('$txt_tip_APID')\">APID</td>";
    echo "    <td>$line_array2[6]</td>";
    echo "</tr>";
    echo "<tr><td onmouseover=\"Tip('$txt_tip_TPID')\">TPID</td>";
    echo "    <td>$line_array2[7]</td>";
    echo "</tr>";
    echo "<tr><td onmouseover=\"Tip('$txt_tip_CAID', WIDTH, 300)\">CAID</td>";
    echo "    <td>$line_array2[8]</td>";
    echo "</tr>";
    echo "<tr><td onmouseover=\"Tip('$txt_tip_SID', WIDTH, 300)\">SID</td>";
    echo "    <td>$line_array2[9]</td>";
    echo "</tr>";
    echo "<tr><td onmouseover=\"Tip('$txt_tip_NID', WIDTH, 300)\">NID</td>";
    echo "    <td>$line_array2[10]</td>";
    echo "</tr>";
    echo "<tr><td onmouseover=\"Tip('$txt_tip_TID', WIDTH, 300)\">TID</td>";
    echo "    <td>$line_array2[11]</td>";
    echo "</tr>";
    echo "<tr><td onmouseover=\"Tip('$txt_tip_RID', WIDTH, 300)\">RID</td>";
    echo "    <td>$line_array2[12]</td>";
    echo "</tr>";
    echo "</table>";
    echo "<br/>";
    //echo $line;
  }
}
fclose($datei);

//echo $lines[$lnr_aktuell];
//echo $lines[4];

container_end();
// *************************************** Container Ende ****************************************************

echo "</div>";  // class='rechts'
echo "<div class='clear'></div>"; // Ende 2-spaltig
echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>