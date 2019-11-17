<?php
// ################################## Variablen für Container Grafik ###############################################
# *** Grafikkartentyp erkennen ***
$vga = shell_exec("$SH_EASYVDR_HW --php --vga2");
//$txt_debug_meldungen[] = "SH_EASYVDR_HW: $SH_EASYVDR_HW";
//$txt_debug_meldungen[] = "EASYPORTAL_DOC_ROOT: $EASYPORTAL_DOC_ROOT";

# *** NVIDIA-Temperaturen ($GPUCoreTemp) ***
exec("nvidia-settings -c :0 -tq GPUCoreTemp", $GPUCoreTemp_Array);
if (isset($GPUCoreTemp_Array[0])) {
  $GPUCoreTemp = $GPUCoreTemp_Array[0]." °C";
} else {
  $GPUCoreTemp = "";
}

# *** NVIDIA-Current Link Speed ($PCIECurrentLinkSpeed) ***
exec("sudo $SH_BEFEHL nvidia-settings -c :0 -tq PCIECurrentLinkSpeed 2>&1", $PCIECurrentLinkSpeed_Array);
$i = 0;
$exit="n";
$PCIECurrentLinkSpeed = "";
while(($i < count($PCIECurrentLinkSpeed_Array)) and ($exit=="n")) {
   $PCIECurrentLinkSpeed_Array[$i] = trim(str_replace("ERROR: Error querying target relations", "", $PCIECurrentLinkSpeed_Array[$i]));
 //  if (($PCIECurrentLinkSpeed != "") and ($PCIECurrentLinkSpeed_Array[$i]!="")) { $PCIECurrentLinkSpeed = $PCIECurrentLinkSpeed . "; "; }
   if ($PCIECurrentLinkSpeed_Array[$i]!="") { $PCIECurrentLinkSpeed = $PCIECurrentLinkSpeed.$PCIECurrentLinkSpeed_Array[$i]. " MHz"; $exit="y";}
   $i++;
}
//if ($PCIECurrentLinkSpeed[0]=="E") {$PCIECurrentLinkSpeed="";}

# *** NVIDIA-Current Link Width (Pcie Lanes) ***
$PCIECurrentLinkWidth = shell_exec("sudo $SH_BEFEHL nvidia-settings -c :0 -tq PCIECurrentLinkWidth 2>&1");
$PCIECurrentLinkWidth = trim(str_replace("ERROR: Error querying target relations", "", $PCIECurrentLinkWidth));
if (($PCIECurrentLinkWidth[0] != "") and ($PCIECurrentLinkWidth[0] != "E")){
  $PCIECurrentLinkWidth = "x".$PCIECurrentLinkWidth[0].$PCIECurrentLinkWidth[1];
} else {
  $PCIECurrentLinkWidth = "";
}

# *** NVIDIA-SMI ***
//    $NVIDIA_smi = shell_exec("sudo $SH_BEFEHL nvidia-smi  2>&1");

# *** NVIDIA-SMI VBIOS***
$NVIDIA_vbios = trim(exec("nvidia-smi -a | grep VBIOS |cut -d':' -f2"));

# *** NVIDIA-Treiberversion ***
$NVIDIA_Driver = trim(exec("nvidia-smi | grep 'Driver Version' | cut -d: -f2 |cut -d'|' -f1"));

# *** NVIDIA-Lüftergeschwindigkeit ***
$GPUCurrentFanSpeed = shell_exec("sudo $SH_BEFEHL nvidia-settings -c :0 -t -q [fan:0]/GPUCurrentFanSpeed 2>&1");
$pos = strpos($GPUCurrentFanSpeed, "ERROR");
if ($pos !== false) { $GPUCurrentFanSpeed = $txt_nicht_auslesbar;}

# *** Auflösung, Aktuelle Bildwiederholrate und empfohlene Rate mit xrand ***
#     ($aufloesung, $akt_rate, $empfohlenen_rate)
$xrandr = shell_exec("sudo $SH_BEFEHL DISPLAY=:0 xrandr | grep '*' 2>&1");

$xrandr = trim($xrandr);
$stringposition = strpos($xrandr, " ");
$aufloesung = substr($xrandr, 0, $stringposition);

$xrandr_Array = explode('  ', $xrandr);
// var_dump($xrandr_Array);
foreach ($xrandr_Array as $feld) {
   if (strpos($feld, "*") !== false) { $akt_rate = $feld;}
   if (strpos($feld, "+") !== false) { $empfohlenen_rate = $feld;}
}
$stringposition = strpos($akt_rate, "*");
$akt_rate = substr($akt_rate, 0, $stringposition)." Hz" ;
$stringposition = strpos($empfohlenen_rate, "+");
$empfohlenen_rate = substr($empfohlenen_rate, 0, $stringposition-1)." Hz" ;


// *************************************** Container Begin 'Container Grafik NVIDIA' ****************************************************
container_begin(2, "nvidia.png", $txt_ue_nvidia);

echo "<table class='tab2'>";
echo "<tr><th valign=top>".$txt_grafikkarte.":</th>      <td align='left'>".$vga."</td></tr>";
echo "<tr><th valign=top>".$txt_inst_nv_drv.":</th>      <td align='left'>".$NVIDIA_Driver."</td></tr>";
echo "<tr><th valign=top>".$txt_nv_gpu_temp.":</th>      <td align='left'>".$GPUCoreTemp."</td></tr>";
echo "<tr><th valign=top>".$txt_gpu_fanspeed.":</th>     <td align='left'>".$GPUCurrentFanSpeed."</td></tr>";
echo "<tr><th valign=top>".$txt_curr_pcie_lanes.":</th>  <td align='left'>".$PCIECurrentLinkWidth."</td></tr>";
echo "<tr><th valign=top>".$txt_pcie_link_speed.":</th>  <td align='left'>".$PCIECurrentLinkSpeed."</td></tr>";
echo "<tr><th valign=top>".$txt_nvidia_vbios.":</th>     <td align='left'>".$NVIDIA_vbios."</td></tr>";
echo "<tr><th valign=top>".$txt_aufloesung.":</th>       <td align='left'>".$aufloesung."</td></tr>";
echo "<tr><th valign=top>".$txt_akt_refresh_rate.":</th> <td align='left'>".$akt_rate."</td></tr>";
echo "<tr><th valign=top>".$txt_best_refresh_rate.":</th><td align='left'>".$empfohlenen_rate."</td></tr>";
echo "</table>";

container_end();
// *************************************** Container Ende ****************************************************
?>