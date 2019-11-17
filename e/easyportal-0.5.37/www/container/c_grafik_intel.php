<?php

# *** Grafikkartentyp erkennen ***
$vga = shell_exec("$SH_EASYVDR_HW --php --vga2");

# *** VA API Version erkennen ***
$va_api_version = trim(exec("sudo $SH_BEFEHL vainfo |grep 'VA-API version' |cut -d: -f3"));

# *** VA Driver Version erkennen ***
$va_driver_version = trim(exec("sudo $SH_BEFEHL vainfo |grep 'Driver version' |cut -d: -f3"));

# *** VA Driver Modul erkennen ***
$va_driver_module = trim(exec("sudo $SH_BEFEHL vainfo |grep 'libva info: Trying to open' |cut -d/ -f2-"));

# *** CPU-Temperatur erkennen ***
$va_temperatur = trim(exec("sensors |grep 'Core 0' |cut -d: -f2 |cut -d\( -f1"));
$va_temperatur = (str_replace("+", "", $va_temperatur));
$va_temperatur = (str_replace(" C", " °C", $va_temperatur));

# *** Aufloesung, Aktuelle Bildwiederholrate und empfohlene Rate mit xrand ***
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


// *************************************** Container Begin 'Container Grafik Intel' ****************************************************
container_begin(2, "intel.png", $txt_ue_intel);

echo "<table class='tab2'>";
echo "<tr><th valign=top>".$txt_grafikkarte.":</th>      <td align='left'>".$vga."</td></tr>";
echo "<tr><th valign=top>".$txt_va_api_version.":</th>   <td align='left'>".$va_api_version."</td></tr>";
echo "<tr><th valign=top>".$txt_va_driver_version.":</th><td align='left'>".$va_driver_version."</td></tr>";
echo "<tr><th valign=top>".$txt_va_driver_module.":</th> <td align='left'>".$va_driver_module."</td></tr>";
echo "<tr><th valign=top>".$txt_va_temperatur.":</th>    <td align='left'>".$va_temperatur."</td></tr>";
echo "<tr><th valign=top>".$txt_aufloesung.":</th>       <td align='left'>".$aufloesung."</td></tr>";
echo "<tr><th valign=top>".$txt_akt_refresh_rate.":</th> <td align='left'>".$akt_rate."</td></tr>";
echo "<tr><th valign=top>".$txt_best_refresh_rate.":</th><td align='left'>".$empfohlenen_rate."</td></tr>";
echo "</table>";

container_end();
// *************************************** Container Ende ****************************************************
?>
