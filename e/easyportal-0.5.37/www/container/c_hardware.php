<?php
// Hardware

# *** Mainboard / Hersteller erkennen ***
exec("sudo $SH_BEFEHL dmidecode -t 2 |grep 'Product Name' |cut -d:  -f2-", $mainboard);
$mainboard = trim($mainboard[0]);
exec("sudo $SH_BEFEHL dmidecode -t 2 |grep 'Manufacturer' |cut -d:  -f2-", $mainboard_hersteller);
$mainboard_hersteller = trim($mainboard_hersteller[0]);

# *** Prozessor erkennen ***
//$cpu = shell_exec("/var/www/scripts/easyvdr_hw.sh --php --cpu");
$befehl_cpu = "cat /proc/cpuinfo |grep \"model name\"|tr -s \" \"|sed q|sed 's/.*://'";
exec("sudo $SH_BEFEHL $befehl_cpu", $cpu_a);
$cpu = $cpu_a[0];

# *** Prozessorfrequenz erkennen ***
//$cpufreq = shell_exec("/var/www/scripts/easyvdr_hw.sh --php --takt");
$befehl_cpufreq = "cat /proc/cpuinfo |grep \"cpu MHz\"|tr -s \" \"|sed q|sed 's/.*://'";
exec("sudo $SH_BEFEHL $befehl_cpufreq", $cpufreq_a);
$cpufreq = $cpufreq_a[0];
$cpufreq = $cpufreq/1000;

# *** Speicher ermitteln ***
$memory = shell_exec("$SH_EASYVDR_HW --php --mem");
$memory = substr($memory/1000000,0,4);

# *** Grafikkarte erkennen ***
$vga = shell_exec("$SH_EASYVDR_HW --php --vga2");

# *** Soundkarte erkennen ***
$sound = shell_exec("$SH_EASYVDR_HW --php --audio");

# *** Netzwerkkarte erkennen ***
$ethernet = shell_exec("$SH_EASYVDR_HW --php --net");

# ***  Laufwerkserkennung HDD***
$hd = shell_exec("$SH_EASYVDR_HW --php --hd");

# ***  Laufwerkserkennung CD/DVD***
$cd      = "";
$cd_a[0] = "";
$befehl_cd = "ls /dev/disk/by-id/ -l|grep \"/sr[0-9]\"|cut -b 44-|sed -e 's/->.*$//'";
exec("sudo $SH_BEFEHL $befehl_cd", $cd_a);
$cd = $cd_a[0];
$cd = trim(str_replace("_", " ", $cd));

if ($cd=="") { $cd = shell_exec("$SH_EASYVDR_HW --php --cd"); }

# *** TV-Karten erkennen ***
$tv = shell_exec("$SH_EASYVDR_HW --php --tv");
if ($tv == "") {
  $tv = shell_exec("cat /var/log/easyvdr/setup.state | grep video_input_hardware | cut -d' ' -f3");
  $tv = trim(str_replace("_", " ", $tv));
}

# *** Herstellerlogos ***
function herstellerbild($string, $dir_array) {
//print_r($dir_array);
//echo "'$string'<br>";
  $bildaufruf = "";
  foreach ($dir_array as $datensatz) {
    $datensatz = substr($datensatz,0,strrpos($datensatz,".")); 
    if (strpos(strtolower($string), trim(str_replace("_", " ", $datensatz))) !== false)  { 
      $bildaufruf = $bildaufruf."<img height='25px' src='images/hersteller/$datensatz.png'>"; 
    }
 //   else {
 //   }
  }
  if (substr($string,0,2) == "ST") {
    $bildaufruf = $bildaufruf."<img height='25px' src='images/hersteller/seagate.png'>";
  }

  $bildaufruf = "<td align='center' valign='top'>".$bildaufruf."</td>"; 
 // if ($bildaufruf == "") {$bildaufruf = "<td>&nbsp;</td>"; }
  return $bildaufruf;
}

$hersteller_dir = $EASYPORTAL_DOC_ROOT."images/hersteller/";
$dir_array = scandir($hersteller_dir);

unset($dir_array[array_search(".", $dir_array)]);
$dir_array = array_values($dir_array);  // Index wiederherstellen
unset($dir_array[array_search("..", $dir_array)]);
$dir_array = array_values($dir_array);  // Index wiederherstellen

$bild_hersteller_mainboard = herstellerbild($mainboard_hersteller, $dir_array);
$bild_hersteller_cpu       = herstellerbild($cpu, $dir_array);
$bild_hersteller_hdd       = herstellerbild($hd, $dir_array);
//if (($bild_hersteller_hdd == "<td>&nbsp;</td>") and (substr($hd,0,2) == "ST")) {
//  $bild_hersteller_hdd = "<td align='center'><img height='25px' src='images/hersteller/seagate.png'></td>";
//}
$bild_hersteller_cd        = herstellerbild($cd, $dir_array);
$bild_hersteller_ethernet  = herstellerbild($ethernet, $dir_array);
$bild_hersteller_vga       = herstellerbild($vga, $dir_array);
$bild_hersteller_sound     = herstellerbild($sound, $dir_array);
$bild_hersteller_tv        = herstellerbild($tv, $dir_array);

// *************************************** Container Begin 'Hardware' ****************************************************
container_begin(2, "devstatus.png", $txt_ue_hardware);

echo "<table class='tab2 w100'>";
echo "<tr onmouseover=\"Tip('$TIP_SYSTEM_MAINBOARD')\">";
echo "<td valign='top'><img src='images/menuicons/motherboard_small.png' height='25px'></td>";
echo "<th valign='top' align='right'>$txt_mainboard:</th><td valign='top' align='left'>$mainboard_hersteller $mainboard</td>";
echo $bild_hersteller_mainboard;
//herstellerbild("digitaldevices");
echo "</tr>";
echo "<tr onmouseover=\"Tip('$TIP_SYSTEM_CPU')\">";
echo "<td valign='top'><img src='images/menuicons/cpu.png' height='25px'></td>";
echo "<th valign='top' align='right'>$txt_cpu:</th><td valign='top' align='left'>".$cpu."</td>";
echo $bild_hersteller_cpu;
echo "</tr>";
echo "<tr onmouseover=\"Tip('$TIP_SYSTEM_CPUFREQ')\">";
echo "<td valign='top'><img src='images/menuicons/Audio-Frequency-Icon.png' height='25px'></td>";
echo "<th valign='top' align='right'>$txt_cpu_freq:</th><td valign='top' align='left'>".$cpufreq."GHz</td>";
echo "<td>&nbsp;</td>";
echo "</tr>";
echo "<tr onmouseover=\"Tip('$TIP_SYSTEM_RAM')\">";
echo "<td valign='top'><img src='images/menuicons/ram_drive.png' height='25px'></td>";
echo "<th valign='top' align='right'>$txt_ram:</th><td valign='top' align='left'>".$memory." GB</td>";
echo "<td>&nbsp;</td>";
echo "</tr>";
echo "<tr onmouseover=\"Tip('$TIP_SYSTEM_HDD')\">";
echo "<td valign='top'><img src='images/menuicons/hdd.png' height='25px'></td>";
echo "<th valign='top' align='right'>$txt_hdd:</th>";
echo "<td valign='top' align='left'>".$hd."</td>";
echo $bild_hersteller_hdd;
echo "</tr>";
echo "<tr onmouseover=\"Tip('$TIP_SYSTEM_OPT')\">";
echo "<td valign='top'><img src='images/menuicons/cd.png' height='25px'></td>";
echo "<td valign='top' align='right'><b>$txt_dvd:</b></td><td valign='top' align='left'>".$cd."</td>";
echo $bild_hersteller_cd;
echo "</tr>";
echo "<tr onmouseover=\"Tip('$TIP_SYSTEM_NET')\">";
echo "<td valign='top'><img src='images/menuicons/network_ethernet.png' height='25px'></td>";
echo "<th valign='top' align='right'>$txt_lan:</th><td valign='top' align='left'>".$ethernet."</td>";
echo $bild_hersteller_ethernet;
echo "</tr>";
echo "<tr onmouseover=\"Tip('$TIP_SYSTEM_VGA')\">";
echo "<td valign='top'><img src='images/menuicons/hdmi.png' height='25px'></td>";
echo "<th valign='top' align='right'>$txt_vga:</th><td valign='top' align='left'>".$vga."</td>";
echo $bild_hersteller_vga;
echo "</tr>";
echo "<tr onmouseover=\"Tip('$TIP_SYSTEM_SND')\">";
echo "<td valign='top'><img src='images/menuicons/audio-card.png' height='25px'></td>";
echo "<th valign='top' align='right'>$txt_sound:</th><td valign='top' align='left'>".$sound."</td>";
echo $bild_hersteller_sound;
echo "</tr>";
echo "<tr onmouseover=\"Tip('$TIP_SYSTEM_DVB')\">";
echo "<td valign='top'><img src='images/menuicons/dvb.png' height='25px'></td> ";
echo "<th valign='top' align='right'>$txt_dvb:</th><td valign='top' align='left'>".$tv."</td>";
echo $bild_hersteller_tv;
echo "</tr>";
echo "</table>";

container_end();
// *************************************** Container Ende ****************************************************
?>