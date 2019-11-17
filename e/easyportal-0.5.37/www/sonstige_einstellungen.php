<?php
include('includes/kopf.php'); 

$LCD              = safepost('LCD');
$LCD_DRIVER       = safepost('LCD_DRIVER');
//$LCD_RESOLUTION   = safepost('LCD_RESOLUTION');
$START_GRAPHTFT   = safepost('START_GRAPHTFT');
$VDRLOGLEVEL      = safepost('VDRLOGLEVEL');
$TITLESELECT      = safepost('TITLESELECT');
$MINLENGTH        = safepost('MINLENGTH');
$LANGUAGE         = safepost('LANGUAGE');
$SAMELENGTH       = safepost('SAMELENGTH');
$NOAD             = safepost('NOAD');
$NOAD_PARALLEL    = safepost('NOAD_PARALLEL');
$NOAD_OSD         = safepost('NOAD_OSD');
$NOAD_ON_CHANNELS = safepost('NOAD_ON_CHANNELS');
$AC3DECODER       = safepost('AC3DECODER');
$AC3_SETS         = safepost('AC3_SETS');

if (isset($_POST["submit2"])) {
  exec("sudo $SH_EASYPORTAL stopvdr");
  exec("sudo $SH_EASYPORTAL setsysconfig LCD $LCD");
  exec("sudo $SH_EASYPORTAL setsysconfig LCD_DRIVER $LCD_DRIVER");
//  exec("sudo $SH_EASYPORTAL setsysconfig LCD_RESOLUTION $LCD_RESOLUTION");
//  exec("sudo $SH_EASYPORTAL setsysconfig START_GRAPHTFT $START_GRAPHTFT");
  exec("sudo $SH_EASYPORTAL setsysconfig VDRLOGLEVEL $VDRLOGLEVEL");
  exec("sudo $SH_EASYPORTAL setsysconfig TITLESELECT $TITLESELECT");
  exec("sudo $SH_EASYPORTAL setsysconfig MINLENGTH $MINLENGTH");
  exec("sudo $SH_EASYPORTAL setsysconfig LANGUAGE $LANGUAGE");
  exec("sudo $SH_EASYPORTAL setsysconfig SAMELENGTH $SAMELENGTH");
  exec("sudo $SH_EASYPORTAL setsysconfig NOAD $NOAD");
  exec("sudo $SH_EASYPORTAL setsysconfig NOAD_PARALLEL $NOAD_PARALLEL");
  exec("sudo $SH_EASYPORTAL setsysconfig NOAD_OSD $NOAD_OSD");
  exec("sudo $SH_EASYPORTAL setsysconfig NOAD_ON_CHANNELS $NOAD_ON_CHANNELS");
  exec("sudo $SH_EASYPORTAL setsysconfig AC3DECODER $AC3DECODER");
  exec("sudo $SH_EASYPORTAL setsysconfig AC3_SETS $AC3_SETS");
  exec("sudo $SH_BEFEHL /usr/lib/vdr/easyvdr-set-settings setglcd");  
  exec("sudo $SH_EASYPORTAL startvdr");
//  echo "<h2>Werte wurden abgespeichert</h2>\n";              
} 

if (!isset($_POST["LCD"])) {
  $lines = file ('/var/lib/vdr/sysconfig');
  foreach ($lines as $line)
  {
    // Info-Display Einstellungen
    if(strpos($line, 'LCD=')              !== false)  { $LCD              = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'LCD_DRIVER=')       !== false)  { $LCD_DRIVER       = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'LCD_RESOLUTION=')   !== false)  { $LCD_RESOLUTION   = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'START_GRAPHTFT=')   !== false)  { $START_GRAPHTFT   = trim(str_replace ('"', '', strstr($line, '"'))); }

    // Log Optionen
    if(strpos($line, 'VDRLOGLEVEL=')      !== false)  { $VDRLOGLEVEL      = trim(str_replace ('"', '', strstr($line, '"'))); }
    
    // RipDVDtoVDR-Einstellungen
    if(strpos($line, 'TITLESELECT=')      !== false)  { $TITLESELECT      = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'MINLENGTH=')        !== false)  { $MINLENGTH        = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'LANGUAGE=')         !== false)  { $LANGUAGE         = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'SAMELENGTH=')       !== false)  { $SAMELENGTH       = trim(str_replace ('"', '', strstr($line, '"'))); }
    
    // Noad Einstellungen
    if(strpos($line, 'NOAD=')             !== false)  { $NOAD             = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'NOAD_PARALLEL=')    !== false)  { $NOAD_PARALLEL    = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'NOAD_OSD=')         !== false)  { $NOAD_OSD         = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'NOAD_ON_CHANNELS=') !== false)  { $NOAD_ON_CHANNELS = trim(str_replace ('"', '', strstr($line, '"'))); }
    
    // AC3 Einstellungen
    if(strpos($line, 'AC3DECODER=')       !== false)  { $AC3DECODER       = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'AC3_SETS=')         !== false)  { $AC3_SETS         = trim(str_replace ('"', '', strstr($line, '"'))); } 
  }
}

echo "<form action='".$_SERVER['PHP_SELF']."' method='post'>";

echo "<div id='kopf2'><a href='einstellungen.php' class='w'>$txt_einstellungen</a> - $txt_kopf2</div>";
echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_einstellungen.php'); 
echo "<h2>$txt_ueberschrift</h2>";

echo "<div class='links'>";

// *************************************** Container Begin 'Info-Display Einstellungen' ****************************************************
container_begin(2, "graphlcd.png", $txt_ue_info_display);

echo "<table border='0' cellspacing='2' width='100%'>";
echo "  <tr>";
echo "    <td>$txt_lcd</td>";
echo "    <td width='260px'><select name='LCD' size='1' style='width: 240px' onChange='this.form.submit()'>";
echo "            <option "; if ($LCD == "keins")   {echo " selected";}  echo  ">keins</option>";
echo "            <option "; if ($LCD == "lcdproc") {echo " selected";}  echo  ">lcdproc</option>";
echo "            <option "; if ($LCD == "graphlcd"){echo " selected";}  echo  ">graphlcd</option>";
echo "    </td>";
echo "  </tr>";
if ($LCD != "keins") {
  echo "  <tr>";
  echo "    <td>$txt_lcd_driver</td>";
  echo "    <td><select name='LCD_DRIVER' size='1' style='width: 240px'>";
  echo "          <option "; if ($LCD_DRIVER == "ks0108")       {echo " selected";}  echo ">ks0108</option>";
  echo "          <option "; if ($LCD_DRIVER == "t6963c")       {echo " selected";}  echo ">t6963c</option>";
  echo "          <option "; if ($LCD_DRIVER == "gu140x32f")    {echo " selected";}  echo ">gu140x32f</option>";
  echo "          <option "; if ($LCD_DRIVER == "gu256x64-372") {echo " selected";}  echo ">gu256x64-372</option>";
  echo "          <option "; if ($LCD_DRIVER == "gu256x64-3900"){echo " selected";}  echo ">gu256x64-3900</option>";
  echo "          <option "; if ($LCD_DRIVER == "image")        {echo " selected";}  echo ">image</option>";
  echo "          <option "; if ($LCD_DRIVER == "sed1330")      {echo " selected";}  echo ">sed1330</option>";
  echo "          <option "; if ($LCD_DRIVER == "sed1520")      {echo " selected";}  echo ">sed1520</option>";
  echo "          <option "; if ($LCD_DRIVER == "simlcd")       {echo " selected";}  echo ">simlcd</option>";
  echo "          <option "; if ($LCD_DRIVER == "serdisp")      {echo " selected";}  echo ">serdisp</option>";
  echo "          <option "; if ($LCD_DRIVER == "noritake800")  {echo " selected";}  echo ">noritake800</option>";
  echo "          <option "; if ($LCD_DRIVER == "hd61830")      {echo " selected";}  echo ">hd61830</option>";
  echo "    </td>";
  echo "  </tr>";
  //echo "  <tr>";
  //echo "    <td>LCD-Aufl&ouml;sung f&uuml;r Bootlogo</td>";
  //if ($LCD_RESOLUTION == "128x64")  {$checked_1 = " checked"; } else {$checked_1 = ""; }
  //if ($LCD_RESOLUTION == "240x128") {$checked_2 = " checked"; } else {$checked_2 = ""; }
  //echo "    <td>128x64 <input type='radio' name='LCD_RESOLUTION' value='128x64' ".$checked_1.">&nbsp;&nbsp; ";
  //echo "        240x128 <input type='radio' name='LCD_RESOLUTION' value='240x128' ".$checked_2."></td>";
  //echo "  </tr>";
}
else {
  echo "<input type='hidden' name='LCD_DRIVER' value='$LCD_DRIVER'>";
  // echo "<input type='hidden' name='LCD_RESOLUTION' value='$LCD_RESOLUTION'>";
}
echo "  <tr>";
echo "    <td>$txt_start_graphtft</td>";
echo "    <td><select name='START_GRAPHTFT' size='1' style='width: 240px'>";
echo "            <option "; if ($START_GRAPHTFT == "no")                               {echo " selected";}  echo ">no</option>";
echo "            <option "; if ($START_GRAPHTFT == "auf Display 0.0 (Ausgabe ueber x)"){echo " selected";}  echo ">auf Display 0.0 (Ausgabe ueber x)</option>";
echo "            <option "; if ($START_GRAPHTFT == "auf Display 0.1 (Ausgabe ueber x)"){echo " selected";}  echo ">auf Display 0.1 (Ausgabe ueber x)</option>";
echo "            <option "; if ($START_GRAPHTFT == "auf Display 1.0 (Ausgabe ueber x)"){echo " selected";}  echo ">auf Display 1.0 (Ausgabe ueber x)</option>";
echo "            <option "; if ($START_GRAPHTFT == "auf Display 1.1 (Ausgabe ueber x)"){echo " selected";}  echo ">auf Display 1.1 (Ausgabe ueber x)</option>";
echo "            <option "; if ($START_GRAPHTFT == "auf vdr/1")                        {echo " selected";}  echo ">auf vdr/1</option>";
echo "            <option "; if ($START_GRAPHTFT == "auf directFB")                     {echo " selected";}  echo ">auf directFB</option>";
echo "            <option "; if ($START_GRAPHTFT == "ueber FrameBuffer /dev/fb0")       {echo " selected";}  echo ">ueber FrameBuffer /dev/fb0</option>";
echo "            <option "; if ($START_GRAPHTFT == "ueber FrameBuffer /dev/fb1")       {echo " selected";}  echo ">ueber FrameBuffer /dev/fb1</option>";
echo "            <option "; if ($START_GRAPHTFT == "auto detect")                      {echo " selected";}  echo ">auto detect</option>";
echo "    </td>";
echo "  </tr>\n";
echo "</table>";

container_end();
// *************************************** Container Ende ****************************************************

// *************************************** Container Begin 'Log-Optionen' ****************************************************
container_begin(2, "Log.png", $txt_ue_loglevel);

echo "<table border='0' cellspacing='2' width='100%'>";
echo "  <tr>";
echo "    <td>$txt_vdrloglevel</td>";
if ($VDRLOGLEVEL == "0")   {$checked_4 = " checked";} else {$checked_0 = ""; }
if ($VDRLOGLEVEL == "1")   {$checked_1 = " checked";} else {$checked_1 = ""; }
if ($VDRLOGLEVEL == "2")   {$checked_2 = " checked";} else {$checked_2 = ""; }
if ($VDRLOGLEVEL == "3")   {$checked_3 = " checked";} else {$checked_3 = ""; }
echo "    <td width='260px'>0 <input type='radio' name='VDRLOGLEVEL' value='0' ".$checked_0.">";
echo "&nbsp;&nbsp; 1 <input type='radio' name='VDRLOGLEVEL' value='1' ".$checked_1.">";
echo "&nbsp;&nbsp; 2 <input type='radio' name='VDRLOGLEVEL' value='2' ".$checked_2.">";
echo "&nbsp;&nbsp; 3 <input type='radio' name='VDRLOGLEVEL' value='3' ".$checked_3."></td>";
echo "  </tr>";
echo "</table>";

container_end();
// *************************************** Container Ende ****************************************************

// *************************************** Container Begin 'ripDVDtoVDR-Einstellungen' ****************************************************
container_begin(2, "Log.png", $txt_ue_ripdvdtovdr);

echo "<table border='0' cellspacing='2' width='100%'>";
echo "  <tr>";
echo "    <td>$txt_titleselect</td>";
echo "    <td width='260px'><input type='text' name='TITLESELECT' size='37' value='$TITLESELECT'></td>";
echo "  </tr>";
echo "  <tr>";
echo "    <td>$txt_minlength</td>";
echo "    <td><input type='text' name='MINLENGTH' size='37' value='$MINLENGTH'></td>";
echo "  </tr>";
echo "  <tr>";
echo "    <td>$txt_language</td>";
if ($LANGUAGE == "de") {$checked_de = " checked"; } else {$checked_de = ""; }
if ($LANGUAGE == "en") {$checked_en = " checked"; } else {$checked_en = ""; }
echo "    <td>de <input type='radio' name='LANGUAGE' value='de' ".$checked_de.">&nbsp;&nbsp; ";
echo "        en <input type='radio' name='LANGUAGE' value='en' ".$checked_en."></td>";
echo "  </tr>\n";
echo "  <tr>";
echo "    <td>$txt_samelength</td>";
if ($SAMELENGTH == "YES"){$checked_y = " checked"; } else {$checked_y = ""; }
if ($SAMELENGTH == "NO") {$checked_n = " checked"; } else {$checked_n = ""; }
echo "    <td>$txt_yes <input type='radio' name='SAMELENGTH' value='YES' ".$checked_y.">&nbsp;&nbsp; ";
echo "        $txt_no <input type='radio' name='SAMELENGTH' value='NO' ".$checked_n."></td>";
echo "  </tr>\n";
echo "</table>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</div>";  // class='links'
echo "<div class='rechts'>";

// *************************************** Container Begin 'NOAD-Einstellungen' ****************************************************
container_begin(2, "film.png", $txt_ue_noad);

echo "<table border='0' cellspacing='2' width='100%'>";
echo "  <tr>";
echo "    <td>$txt_noad</td>";
echo "    <td width='260px'><select name='NOAD' size='1' style='width: 240px' onChange='this.form.submit()'>";
echo "            <option ";
if ($NOAD == "Deaktiviert"){echo " selected";}
echo                  ">Deaktiviert</option>";
echo "            <option ";
if ($NOAD == "Offline"){echo " selected";}
echo                  ">Offline</option>";
echo "            <option ";
if ($NOAD == "Online nur Live Aufnahmen"){echo " selected";}
echo                  ">Online nur Live Aufnahmen</option>";
echo "            <option ";
if ($NOAD == "Online alle Aufnahmen"){echo " selected";}
echo                  ">Online alle Aufnahmen</option>";
echo "    </td>";
echo "  </tr>\n";
if ($NOAD != "Deaktiviert") {
  echo "  <tr>";
  echo "    <td>$txt_noad_parallel</td>";
  if ($NOAD_PARALLEL == "yes"){$checked_y = " checked"; } else {$checked_y = ""; }
  if ($NOAD_PARALLEL == "no") {$checked_n = " checked"; } else {$checked_n = ""; }
  echo "    <td>$txt_yes <input type='radio' name='NOAD_PARALLEL' value='yes' ".$checked_y.">&nbsp;&nbsp; ";
  echo "        $txt_no <input type='radio' name='NOAD_PARALLEL' value='no' ".$checked_n."></td>";
  echo "  </tr>\n";
  echo "  <tr>";
  echo "    <td>$txt_noad_osd</td>";
  if ($NOAD_OSD == "yes"){$checked_y = " checked"; } else {$checked_y = ""; }
  if ($NOAD_OSD == "no") {$checked_n = " checked"; } else {$checked_n = ""; }
  echo "    <td>$txt_yes <input type='radio' name='NOAD_OSD' value='yes' ".$checked_y.">&nbsp;&nbsp; ";
  echo "        $txt_no <input type='radio' name='NOAD_OSD' value='no' ".$checked_n."></td>";
  echo "  </tr>\n";
  echo "  <tr>";
  echo "    <td>$txt_noad_on_channels</td>";
  if ($NOAD_ON_CHANNELS == "yes"){$checked_y = " checked"; } else {$checked_y = ""; }
  if ($NOAD_ON_CHANNELS == "no") {$checked_n = " checked"; } else {$checked_n = ""; }
  echo "    <td>$txt_yes <input type='radio' name='NOAD_ON_CHANNELS' value='yes' ".$checked_y.">&nbsp;&nbsp; ";
  echo "        $txt_no <input type='radio' name='NOAD_ON_CHANNELS' value='no' ".$checked_n."></td>";
  echo "  </tr>\n";
}
else {
  echo "<input type='hidden' name='NOAD_PARALLEL' value='$NOAD_PARALLEL'>";
  echo "<input type='hidden' name='NOAD_OSD' value='$NOAD_OSD'>";
  echo "<input type='hidden' name='NOAD_ON_CHANNELS' value='$NOAD_ON_CHANNELS'>";
}
echo "</table>";

container_end();
// *************************************** Container Ende ****************************************************

// *************************************** Container Begin 'AC3-Einstellungen' ****************************************************
container_begin(2, "sound.png", $txt_ue_ac3);

echo "<table border='0' cellspacing='2' width='100%'>";
echo "  <tr>";
echo "    <td>$txt_ac3decoder</td>";
if ($AC3DECODER == "Deaktiviert"){$checked_d = " checked"; } else {$checked_d = ""; }
if ($AC3DECODER == "ac3dec") {$checked_ac3 = " checked"; } else {$checked_ac3 = ""; }
if ($AC3DECODER == "a52dec") {$checked_a52 = " checked"; } else {$checked_a52 = ""; }
echo "    <td width='260px'>Deaktiviert <input type='radio' name='AC3DECODER' value='Deaktiviert' ".$checked_d." onChange='this.form.submit()'>&nbsp;&nbsp; ";
echo "        ac3dec <input type='radio' name='AC3DECODER' value='ac3dec' ".$checked_ac3." onChange='this.form.submit()'>&nbsp;&nbsp; ";
echo "        a52dec <input type='radio' name='AC3DECODER' value='a52dec' ".$checked_a52." onChange='this.form.submit()'></td>";
echo "  </tr>\n";
if ($AC3DECODER != "Deaktiviert"){
  echo "  <tr>";
  echo "    <td>$txt_ac3_sets</td>";
  echo "    <td><input type='text' name='AC3_SETS' size='37' value='$AC3_SETS'></td>";
  echo "  </tr>";
}
else {
  echo "<input type='hidden' name='AC3_SETS' value='$AC3_SETS'>";
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