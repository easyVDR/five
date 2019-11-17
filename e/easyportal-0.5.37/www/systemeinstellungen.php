<?php
include('includes/kopf.php'); 

$VDR_FRONTEND        = trim(safepost('VDR_FRONTEND'));
$VIDMODE             = trim(safepost('VIDMODE'));
$FB_RECEIVER         = trim(safepost('FB_RECEIVER'));
$FB_CONTROLLER       = trim(safepost('FB_CONTROLLER'));
$SHUTDOWNMETHOD      = trim(safepost('SHUTDOWNMETHOD'));
$WAKEUP_REGULAR_DAYS = trim(safepost('WAKEUP_REGULAR_DAYS'));
$WAKEUP_REGULAR_TIME = trim(safepost('WAKEUP_REGULAR_TIME'));
$WAKEUP_START_AHEAD  = trim(safepost('WAKEUP_START_AHEAD'));
$NVRAM_COMMANDLINE   = trim(safepost('NVRAM_COMMANDLINE'));
$NVRAM_FORCE_REBOOT  = trim(safepost('NVRAM_FORCE_REBOOT'));
$MEDIADEVICE         = trim(safepost('MEDIADEVICE'));
$HDDSPINDOWNTIME     = trim(safepost('HDDSPINDOWNTIME'));
$HDDSPINDOWNDISKS    = trim(safepost('HDDSPINDOWNDISKS'));
$START_SMARTMON      = trim(safepost('START_SMARTMON'));
$SMARTMON_DRIVES     = trim(safepost('SMARTMON_DRIVES'));
$SMARTMON_OPTS       = trim(safepost('SMARTMON_OPTS'));
$START_HDDTEMP       = trim(safepost('START_HDDTEMP'));
$HDDTEMP_DRIVES      = trim(safepost('HDDTEMP_DRIVES'));
$HDDTEMP_DRIVES_WO_SENSORS = trim(safepost('HDDTEMP_DRIVES_WO_SENSORS'));
$HDDTEMP_INTERFACE   = trim(safepost('HDDTEMP_INTERFACE'));
$HDDTEMP_PORT        = trim(safepost('HDDTEMP_PORT'));
$HDDTEMP_INTERVALL   = trim(safepost('HDDTEMP_INTERVALL'));
$HDDTEMP_OPTS        = trim(safepost('HDDTEMP_OPTS'));
$CREATE_RAMDISK      = trim(safepost('CREATE_RAMDISK'));
$RAMDISK_SIZE        = trim(safepost('RAMDISK_SIZE'));
$VTX_RAMDISK         = trim(safepost('VTX_RAMDISK'));
$EPG_RAMDISK         = trim(safepost('EPG_RAMDISK'));
$EPGIMAGES_RAMDISK   = trim(safepost('EPGIMAGES_RAMDISK'));

if (safepost('submit2')) {
  exec("sudo $SH_EASYPORTAL stopvdr");
  exec("sudo $SH_EASYPORTAL setsysconfig VDR_FRONTEND $VDR_FRONTEND");
  exec("sudo $SH_EASYPORTAL setsysconfig VIDMODE $VIDMODE");
  exec("sudo $SH_EASYPORTAL setsysconfig FB_RECEIVER $FB_RECEIVER");
  exec("sudo $SH_EASYPORTAL setsysconfig FB_CONTROLLER $FB_CONTROLLER");
  exec("sudo $SH_EASYPORTAL setsysconfig SHUTDOWNMETHOD $SHUTDOWNMETHOD");
  exec("sudo $SH_EASYPORTAL setsysconfig WAKEUP_REGULAR_DAYS $WAKEUP_REGULAR_DAYS");
  exec("sudo $SH_EASYPORTAL setsysconfig WAKEUP_REGULAR_TIME $WAKEUP_REGULAR_TIME");
  exec("sudo $SH_EASYPORTAL setsysconfig WAKEUP_START_AHEAD $WAKEUP_START_AHEAD");
  exec("sudo $SH_EASYPORTAL setsysconfig NVRAM_COMMANDLINE $NVRAM_COMMANDLINE");
  exec("sudo $SH_EASYPORTAL setsysconfig NVRAM_FORCE_REBOOT $NVRAM_FORCE_REBOOT");
  exec("sudo $SH_EASYPORTAL setsysconfig MEDIADEVICE $MEDIADEVICE");
  exec("sudo $SH_EASYPORTAL setsysconfig HDDSPINDOWNTIME '$HDDSPINDOWNTIME'");
  exec("sudo $SH_EASYPORTAL setsysconfig START_SMARTMON $START_SMARTMON");
  exec("sudo $SH_EASYPORTAL setsysconfig SMARTMON_DRIVES $SMARTMON_DRIVES");
  exec("sudo $SH_EASYPORTAL setsysconfig SMARTMON_OPTS $SMARTMON_OPTS");
  exec("sudo $SH_EASYPORTAL setsysconfig HDDSPINDOWNDISKS '$HDDSPINDOWNDISKS'");
  exec("sudo $SH_EASYPORTAL setsysconfig START_HDDTEMP $START_HDDTEMP");
  exec("sudo $SH_EASYPORTAL setsysconfig HDDTEMP_DRIVES_WO_SENSORS $HDDTEMP_DRIVES_WO_SENSORS");
  exec("sudo $SH_EASYPORTAL setsysconfig HDDTEMP_INTERFACE $HDDTEMP_INTERFACE");
  exec("sudo $SH_EASYPORTAL setsysconfig HDDTEMP_PORT $HDDTEMP_PORT");
  exec("sudo $SH_EASYPORTAL setsysconfig HDDTEMP_INTERVALL $HDDTEMP_INTERVALL");
  exec("sudo $SH_EASYPORTAL setsysconfig HDDTEMP_OPTS $HDDTEMP_OPTS");
  exec("sudo $SH_EASYPORTAL setsysconfig CREATE_RAMDISK $CREATE_RAMDISK");
  exec("sudo $SH_EASYPORTAL setsysconfig RAMDISK_SIZE $RAMDISK_SIZE");
  exec("sudo $SH_EASYPORTAL setsysconfig VTX_RAMDISK $VTX_RAMDISK");
  exec("sudo $SH_EASYPORTAL setsysconfig EPG_RAMDISK $EPG_RAMDISK");
  exec("sudo $SH_EASYPORTAL setsysconfig EPGIMAGES_RAMDISK $EPGIMAGES_RAMDISK");
  exec("sudo /usr/lib/vdr/easyvdr-set-settings frontend");  
  exec("sudo /usr/lib/vdr/easyvdr-set-settings lirc");  
  exec("sudo /usr/lib/vdr/easyvdr-set-settings shutdownmethod");  
  exec("sudo /usr/lib/vdr/easyvdr-set-settings fstab");  
  exec("sudo /usr/lib/vdr/easyvdr-set-settings smartmon");  
  exec("sudo /usr/lib/vdr/easyvdr-set-settings hddtemp");  
  exec("sudo $SH_EASYPORTAL startvdr");
//  echo "<h2>Werte wurden abgespeichert </h2>\n";
} 


if (!safepost('VDR_FRONTEND')) {
  $lines = file ('/var/lib/vdr/sysconfig');
  foreach ($lines as $line)  {
    // Frontend-Einstellungen
    if(strpos($line, 'VDR_FRONTEND=')        !== false)  { $VDR_FRONTEND        = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'VIDMODE=')             !== false)  { $VIDMODE             = trim(str_replace ('"', '', strstr($line, '"'))); }
    
    // Fernbedienung
    if(strpos($line, 'FB_RECEIVER=')         !== false)  { $FB_RECEIVER         = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'FB_CONTROLLER=')       !== false)  { $FB_CONTROLLER       = trim(str_replace ('"', '', strstr($line, '"'))); }
    
    // Wakeup/Powernoff-Einstellungen
    if(strpos($line, 'SHUTDOWNMETHOD=')      !== false)  { $SHUTDOWNMETHOD      = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'WAKEUP_REGULAR_DAYS=') !== false)  { $WAKEUP_REGULAR_DAYS = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'WAKEUP_REGULAR_TIME=') !== false)  { $WAKEUP_REGULAR_TIME = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'WAKEUP_START_AHEAD=')  !== false)  { $WAKEUP_START_AHEAD  = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'NVRAM_COMMANDLINE=')   !== false)  { $NVRAM_COMMANDLINE   = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'NVRAM_FORCE_REBOOT=')  !== false)  { $NVRAM_FORCE_REBOOT  = trim(str_replace ('"', '', strstr($line, '"'))); }
    
    // Festplatten-Einstellungen
    if(strpos($line, 'MEDIADEVICE=')         !== false)  { $MEDIADEVICE         = trim(str_replace ('"', '', strstr($line, '"'))); } 
    if(strpos($line, 'HDDSPINDOWNTIME=')     !== false)  { $HDDSPINDOWNTIME     = trim(str_replace ('"', '', strstr($line, '"'))); } 
    if(strpos($line, 'HDDSPINDOWNDISKS=')    !== false)  { $HDDSPINDOWNDISKS    = trim(str_replace ('"', '', strstr($line, '"'))); } 
    if(strpos($line, 'START_SMARTMON=')      !== false)  { $START_SMARTMON      = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'SMARTMON_DRIVES=')     !== false)  { $SMARTMON_DRIVES     = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'SMARTMON_OPTS=')       !== false)  { $SMARTMON_OPTS       = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'START_HDDTEMP=')       !== false)  { $START_HDDTEMP       = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'HDDTEMP_DRIVES=')      !== false)  { $HDDTEMP_DRIVES      = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'HDDTEMP_DRIVES_WO_SENSORS=') !== false)  { $HDDTEMP_DRIVES_WO_SENSORS    = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'HDDTEMP_INTERFACE=')   !== false)  { $HDDTEMP_INTERFACE   = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'HDDTEMP_PORT=')        !== false)  { $HDDTEMP_PORT        = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'HDDTEMP_INTERVALL=')   !== false)  { $HDDTEMP_INTERVALL   = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'HDDTEMP_OPTS=')        !== false)  { $HDDTEMP_OPTS        = trim(str_replace ('"', '', strstr($line, '"'))); }
    
    // Ramdisk-Einstellungen
    if(strpos($line, 'CREATE_RAMDISK=')      !== false)  { $CREATE_RAMDISK      = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'RAMDISK_SIZE=')        !== false)  { $RAMDISK_SIZE        = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'VTX_RAMDISK=')         !== false)  { $VTX_RAMDISK         = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'EPG_RAMDISK=')         !== false)  { $EPG_RAMDISK         = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'EPGIMAGES_RAMDISK=')   !== false)  { $EPGIMAGES_RAMDISK   = trim(str_replace ('"', '', strstr($line, '"'))); }  
  }
}

echo "<form action='".$_SERVER['PHP_SELF']."' method='post'>";

echo "<div id='kopf2'><a href='einstellungen.php' class='w'>$txt_einstellungen</a> - $txt_kopf2</div>";
echo "<div id='inhalt'>";
echo "<center>";
include('includes/subnavi_einstellungen.php'); 
echo "<h2>$txt_ueberschrift</h2>";
echo "<div class='links'>";

// *************************************** Container Begin 'Frontend-Einstellungen' ****************************************************
container_begin(2, "bilder.png", $txt_ue_frontend);

echo "<table border='0' cellspacing='2' width='100%'>";
echo "  <tr>";
echo "    <td>$txt_frontend</td>";
echo "    <td width='260px'><select name='VDR_FRONTEND' size='1' style='width: 240px' onChange='this.form.submit()'>";
echo "            <option "; if ($VDR_FRONTEND == "keines")      {echo " selected";}  echo ">keines</option>";
echo "            <option "; if ($VDR_FRONTEND == "DVB-FF")      {echo " selected";}  echo ">DVB-FF</option>";
echo "            <option "; if ($VDR_FRONTEND == "DVBS2-FF")    {echo " selected";}  echo ">DVBS2-FF</option>";
echo "            <option "; if ($VDR_FRONTEND == "reelbox")     {echo " selected";}  echo ">reelbox";
echo "            <option "; if ($VDR_FRONTEND == "PVR350")      {echo " selected";}  echo ">PVR350</option>";
echo "            <option "; if ($VDR_FRONTEND == "vdr-sxfe")    {echo " selected";}  echo ">vdr-sxfe</option>";
echo "            <option "; if ($VDR_FRONTEND == "xine")        {echo " selected";}  echo ">xine</option>";
echo "            <option "; if ($VDR_FRONTEND == "XBMC")        {echo " selected";}  echo ">XBMC";
echo "            <option "; if ($VDR_FRONTEND == "Softhddevice"){echo " selected";}  echo ">Softhddevice</option>";
echo "         </select>";
echo "   </td>";
if ($VDR_FRONTEND == "DVB-FF") {
  echo "  <tr>";
  echo "    <td>$txt_ff_modus</td>";
  if ($VIDMODE == "0")   {$checked_4 = " checked";} else {$checked_0 = ""; }
  if ($VIDMODE == "1")   {$checked_1 = " checked";} else {$checked_1 = ""; }
  if ($VIDMODE == "2")   {$checked_2 = " checked";} else {$checked_2 = ""; }
  if ($VIDMODE == "3")   {$checked_3 = " checked";} else {$checked_3 = ""; }
  echo "    <td>0 <input type='radio' name='VIDMODE' value='0' ".$checked_0.">";
  echo "&nbsp;&nbsp; 1 <input type='radio' name='VIDMODE' value='1' ".$checked_1.">";
  echo "&nbsp;&nbsp; 2 <input type='radio' name='VIDMODE' value='2' ".$checked_2.">";
  echo "&nbsp;&nbsp; 3 <input type='radio' name='VIDMODE' value='3' ".$checked_3."></td>";
  echo "  </tr>\n";
}
else {
  echo "<input type='hidden' name='VIDMODE'  value='$VIDMODE'>";
}
echo "</table>";

container_end();
// *************************************** Container Ende ****************************************************

// *************************************** Container Begin 'Fernbedienung' ****************************************************
container_begin(2, "RemoteControl.png", $txt_ue_fb);

echo "<table border='0' cellspacing='2' width='100%'>";
echo "  <tr>";
echo "    <td>$txt_fb_empfaenger</td>";
echo "    <td width='260px'><input type='text' name='FB_RECEIVER' size='37' value='$FB_RECEIVER'></td>";
echo "  </tr>";
echo "  <tr>";
echo "    <td>$txt_fb_sender &nbsp;&nbsp;&nbsp; (<a class='b' href='fernbedienungen.php'>Übersicht Fernbedienungen</a>)</td>";
echo "    <td><input type='text' name='FB_CONTROLLER' size='37' value='$FB_CONTROLLER'></td>";
echo "  </tr>";
echo "</table>";

container_end();
// *************************************** Container Ende ****************************************************

// *************************************** Container Begin 'Wakeup/Poweroff-Einstellungen' ****************************************************
container_begin(2, "Remote.png", $txt_ue_wakeup);

echo "<table border='0' cellspacing='2' width='100%'>";
echo "  <tr>";
echo "    <td>$txt_wakeup_methode</td>";
echo "    <td width='260px'><select name='SHUTDOWNMETHOD' size='1' style='width: 240px' onChange='this.form.submit()'>";
echo "            <option "; if ($SHUTDOWNMETHOD == "kein")            {echo " selected";}  echo ">kein</option>";
echo "            <option "; if ($SHUTDOWNMETHOD == "ACPI")            {echo " selected";}  echo ">ACPI</option>";
echo "            <option "; if ($SHUTDOWNMETHOD == "Nvram-Wakeup")    {echo " selected";}  echo ">Nvram-Wakeup</option>";
echo "            <option "; if ($SHUTDOWNMETHOD == "Extb")            {echo " selected";}  echo ">Extb";
echo "            <option "; if ($SHUTDOWNMETHOD == "Jepsen-HW-Wakeup"){echo " selected";}  echo ">Jepsen-HW-Wakeup</option>";
echo "            <option "; if ($SHUTDOWNMETHOD == "STM32-IRMP-Wakeup"){echo " selected";}  echo ">STM32-IRMP-Wakeup</option>";
echo "         </select>";
echo "    </td>";
echo "  </tr>";
if ($SHUTDOWNMETHOD != "kein") {
  echo "  <tr>";
  echo "    <td>$txt_wakeup_intervall</td>";
  echo "    <td><input type='text' name='WAKEUP_REGULAR_DAYS' size='37' value='$WAKEUP_REGULAR_DAYS'></td>";
  echo "  </tr>";
  echo "  <tr>";
  echo "    <td>$txt_wakeup_time</td>";
  echo "    <td><input type='text' name='WAKEUP_REGULAR_TIME' size='37' value='$WAKEUP_REGULAR_TIME'></td>";
  echo "  </tr>";
  echo "  <tr>";
  echo "    <td>$txt_wakeup_x_min</td>";
  echo "    <td><input type='text' name='WAKEUP_START_AHEAD' size='37' value='$WAKEUP_START_AHEAD'></td>";
  echo "  </tr>";
  echo "  <tr>";
  if ($SHUTDOWNMETHOD == "Nvram-Wakeup") {
    echo "    <td>$txt_nvram_line</td>";
    echo "    <td><input type='text' name='NVRAM_COMMANDLINE' size='37' value='$NVRAM_COMMANDLINE'></td>";
    echo "  </tr>";
    echo "  <tr>";
    echo "    <td>$txt_nvram_reboot</td>";
    if ($NVRAM_FORCE_REBOOT == "yes"){$checked_y = " checked"; } else {$checked_y = ""; }
    if ($NVRAM_FORCE_REBOOT == "no") {$checked_n = " checked"; } else {$checked_n = ""; }
    echo "    <td>$txt_yes <input type='radio' name='NVRAM_FORCE_REBOOT' value='yes' ".$checked_y.">&nbsp;&nbsp; ";
    echo "        $txt_no <input type='radio' name='NVRAM_FORCE_REBOOT' value='no' ".$checked_n."></td>";
    echo "  </tr>\n";
  }
  else {
    echo "<input type='hidden' name='NVRAM_COMMANDLINE'  value='$NVRAM_COMMANDLINE'>";
    echo "<input type='hidden' name='NVRAM_FORCE_REBOOT' value='$NVRAM_FORCE_REBOOT'>";
  }
}
else {
  echo "<input type='hidden' name='WAKEUP_REGULAR_DAYS' value='$WAKEUP_REGULAR_DAYS'>";
  echo "<input type='hidden' name='WAKEUP_REGULAR_TIME' value='$WAKEUP_REGULAR_TIME'>";
  echo "<input type='hidden' name='WAKEUP_START_AHEAD'  value='$WAKEUP_START_AHEAD'>";
  echo "<input type='hidden' name='NVRAM_COMMANDLINE'   value='$NVRAM_COMMANDLINE'>";
  echo "<input type='hidden' name='NVRAM_FORCE_REBOOT'  value='$NVRAM_FORCE_REBOOT'>";
}

echo "</table>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</div>";  // class='links'
echo "<div class='rechts'>";

// *************************************** Container Begin 'Wakeup/Poweroff-Einstellungen' ****************************************************
container_begin(2, "hdd.png", $txt_ue_hdd);

echo "<table border='0' cellspacing='2' width='100%'>";
echo "  <tr>";
echo "    <td>$txt_mediadevice</td>";
echo "    <td width='260px'><input type='text' name='MEDIADEVICE' size='37' value='$MEDIADEVICE'></td>";
echo "  </tr>";
echo "  <tr>";
echo "    <td>$txt_hddspindowntime</td>";
echo "    <td><select name='HDDSPINDOWNTIME' size='1' style='width: 240px' onChange='this.form.submit()'>";
echo "            <option "; if ($HDDSPINDOWNTIME == "nicht abschalten"){echo " selected";}  echo ">nicht abschalten</option>";
echo "            <option "; if ($HDDSPINDOWNTIME == "5 Minuten")       {echo " selected";}  echo ">5 Minuten</option>";
echo "            <option "; if ($HDDSPINDOWNTIME == "10 Minuten")      {echo " selected";}  echo ">10 Minuten</option>";
echo "            <option "; if ($HDDSPINDOWNTIME == "15 Minuten")      {echo " selected";}  echo ">15 Minuten";
echo "            <option "; if ($HDDSPINDOWNTIME == "20 Minuten")      {echo " selected";}  echo ">20 Minuten</option>";
echo "         </select>";
echo "    </td>";
echo "  </tr>";
if ($HDDSPINDOWNTIME != "nicht abschalten") {
  echo "  <tr>";
  echo "    <td>$txt_hddspindowndisks</td>";
  echo "    <td><input type='text' name='HDDSPINDOWNDISKS' size='37' value='$HDDSPINDOWNDISKS'></td>";
  echo "  </tr>";
  echo "  <tr>";
}
else {
  echo "<input type='hidden' name='HDDSPINDOWNDISKS' value='$HDDSPINDOWNDISKS'>";
}
echo "    <td>$txt_start_smartmon</td>";
if ($START_SMARTMON == "yes"){$checked_y = " checked"; } else {$checked_y = ""; }
if ($START_SMARTMON == "no") {$checked_n = " checked"; } else {$checked_n = ""; }
echo "    <td>$txt_yes <input type='radio' name='START_SMARTMON' value='yes' ".$checked_y." onChange='this.form.submit()'>&nbsp;&nbsp; ";
echo "        $txt_no <input type='radio' name='START_SMARTMON' value='no' ".$checked_n." onChange='this.form.submit()'></td>";
echo "  </tr>\n";
if ($START_SMARTMON == "yes") {
  echo "  <tr>";
  echo "    <td>$txt_smartmon_drives</td>";
  echo "    <td><input type='text' name='SMARTMON_DRIVES' size='37' value='$SMARTMON_DRIVES'></td>";
  echo "  </tr>";
  echo "  <tr>";
  echo "    <td>$txt_smartmon_opts</td>";
  echo "    <td><input type='text' name='SMARTMON_OPTS' size='37' value='$SMARTMON_OPTS'></td>";
  echo "  </tr>";
}
else {
  echo "<input type='hidden' name='SMARTMON_OPTS' value='$SMARTMON_OPTS'>";
}
echo "  <tr>";
echo "    <td>$txt_start_hddtemp</td>";
if ($START_HDDTEMP == "yes"){$checked_y = " checked"; } else {$checked_y = ""; }
if ($START_HDDTEMP == "no") {$checked_n = " checked"; } else {$checked_n = ""; }
echo "    <td>$txt_yes <input type='radio' name='START_HDDTEMP' value='yes' ".$checked_y." onChange='this.form.submit()'>&nbsp;&nbsp; ";
echo "        $txt_no <input type='radio' name='START_HDDTEMP' value='no' ".$checked_n." onChange='this.form.submit()'></td>";
echo "  </tr>\n";
if ($START_HDDTEMP == "yes") {
  echo "  <tr>";
  echo "    <td>$txt_hddtemp_drives</td>";
  echo "    <td><input type='text' name='HDDTEMP_DRIVES' size='37' value='$HDDTEMP_DRIVES'></td>";
  echo "  </tr>";
  echo "  <tr>";
  echo "    <td>$txt_hddtemp_drives_wo_sensors</td>";
  echo "    <td><input type='text' name='HDDTEMP_DRIVES_WO_SENSORS' size='37' value='$HDDTEMP_DRIVES_WO_SENSORS'></td>";
  echo "  </tr>";
  echo "  <tr>";
  echo "    <td>$txt_hddtemp_interface</td>";
  echo "    <td><input type='text' name='HDDTEMP_INTERFACE' size='37' value='$HDDTEMP_INTERFACE'></td>";
  echo "  </tr>";
  echo "  <tr>";
  echo "    <td>$txt_hddtemp_port</td>";
  echo "    <td><input type='text' name='HDDTEMP_PORT' size='37' value='$HDDTEMP_PORT'></td>";
  echo "  </tr>";
  echo "  <tr>";
  echo "    <td>$txt_hddtemp_intervall</td>";
  echo "    <td><input type='text' name='HDDTEMP_INTERVALL' size='37' value='$HDDTEMP_INTERVALL'></td>";
  echo "  </tr>";
  echo "  <tr>";
  echo "    <td>$txt_hddtemp_opts</td>";
  echo "    <td><input type='text' name='HDDTEMP_OPTS' size='37' value='$HDDTEMP_OPTS'></td>";
  echo "  </tr>";
}
else {
  echo "<input type='hidden' name='HDDTEMP_DRIVES'            value='$HDDTEMP_DRIVES'>";
  echo "<input type='hidden' name='HDDTEMP_DRIVES_WO_SENSORS' value='$HDDTEMP_DRIVES_WO_SENSORS'>";
  echo "<input type='hidden' name='HDDTEMP_INTERFACE'         value='$HDDTEMP_INTERFACE'>";
  echo "<input type='hidden' name='HDDTEMP_PORT'              value='$HDDTEMP_PORT'>";
  echo "<input type='hidden' name='HDDTEMP_INTERVALL'         value='$HDDTEMP_INTERVALL'>";
  echo "<input type='hidden' name='HDDTEMP_OPTS'              value='$HDDTEMP_OPTS'>";
}
echo "</table>";

container_end();
// *************************************** Container Ende ****************************************************

// *************************************** Container Begin 'Ramdisk-Einstellungen' ****************************************************
container_begin(2, "ram_drive.png", $txt_ue_ramdisk);

echo "<table border='0' cellspacing='2' width='100%'>";
echo "  <tr>";
echo "    <td>$txt_create_ramdisk</td>";
if ($CREATE_RAMDISK == "yes"){$checked_y = " checked"; } else {$checked_y = ""; }
if ($CREATE_RAMDISK == "no") {$checked_n = " checked"; } else {$checked_n = ""; }
echo "    <td width='260px'>$txt_yes <input type='radio' name='CREATE_RAMDISK' value='yes' ".$checked_y." onChange='this.form.submit()'>&nbsp;&nbsp; ";
echo "        $txt_no <input type='radio' name='CREATE_RAMDISK' value='no' ".$checked_n." onChange='this.form.submit()'></td>";
echo "  </tr>\n";
if ($CREATE_RAMDISK == "yes") {
  echo "  <tr>";
  echo "    <td>$txt_ramdisk_size</td>";
  echo "    <td><input type='text' name='RAMDISK_SIZE' size='37' value='$RAMDISK_SIZE'></td>";
  echo "  </tr>";
  echo "  <tr>";
  echo "    <td>$txt_vtx_ramdisk</td>";
  if ($VTX_RAMDISK == "yes"){$checked_y = " checked"; } else {$checked_y = ""; }
  if ($VTX_RAMDISK == "no") {$checked_n = " checked"; } else {$checked_n = ""; }
  echo "    <td>$txt_yes <input type='radio' name='VTX_RAMDISK' value='yes' ".$checked_y.">&nbsp;&nbsp; ";
  echo "        $txt_no <input type='radio' name='VTX_RAMDISK' value='no' ".$checked_n."></td>";
  echo "  </tr>\n";
  echo "  <tr>";
  echo "    <td>$txt_epg_ramdisk</td>";
  if ($EPG_RAMDISK == "yes"){$checked_y = " checked"; } else {$checked_y = ""; }
  if ($EPG_RAMDISK == "no") {$checked_n = " checked"; } else {$checked_n = ""; }
  echo "    <td>$txt_yes <input type='radio' name='EPG_RAMDISK' value='yes' ".$checked_y.">&nbsp;&nbsp; ";
  echo "        $txt_no <input type='radio' name='EPG_RAMDISK' value='no' ".$checked_n."></td>";
  echo "  </tr>\n";
  echo "  <tr>";
  echo "    <td>$txt_epgimages_ramdisk</td>";
  if ($EPGIMAGES_RAMDISK == "yes"){$checked_y = " checked"; } else {$checked_y = ""; }
  if ($EPGIMAGES_RAMDISK == "no") {$checked_n = " checked"; } else {$checked_n = ""; }
  echo "    <td>$txt_yes <input type='radio' name='EPGIMAGES_RAMDISK' value='yes' ".$checked_y.">&nbsp;&nbsp; ";
  echo "        $txt_no <input type='radio' name='EPGIMAGES_RAMDISK' value='no' ".$checked_n."></td>";
  echo "  </tr>\n";
}
else {
  echo "<input type='hidden' name='RAMDISK_SIZE'      value='$RAMDISK_SIZE'>";
  echo "<input type='hidden' name='VTX_RAMDISK'       value='$VTX_RAMDISK'>";
  echo "<input type='hidden' name='EPG_RAMDISK'       value='$EPG_RAMDISK'>";
  echo "<input type='hidden' name='EPGIMAGES_RAMDISK' value='$EPGIMAGES_RAMDISK'>";
}
echo "</table>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</div>";  // class='rechts'
echo "<div class='clear'></div>"; // Ende 2-spaltig

echo "<input type='submit' name='submit2' value='$txt_b_save' onclick=\"wait('WaitSave')\">";
echo "</form>";

echo "</div>";   // Inhalt

include('includes/fuss.php'); 
?>