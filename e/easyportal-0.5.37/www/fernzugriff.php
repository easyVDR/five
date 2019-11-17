<?php
include('includes/kopf.php'); 

$FRIENDLY_FILENAMES  = safepost('FRIENDLY_FILENAMES');
$START_SSHD          = safepost('START_SSHD');
$START_XXV           = safepost('START_XXV');
$START_LIGHTTPD      = safepost('START_LIGHTTPD');
$START_VDRADMIN      = safepost('START_VDRADMIN');
$START_NFS           = safepost('START_NFS');
$STREAMQUALITY       = safepost('STREAMQUALITY');
$WAKE_SERVER         = safepost('WAKE_SERVER');
$SERVER_MAC          = safepost('SERVER_MAC');
$START_SAMBA         = safepost('START_SAMBA');
$SAMBA_WORKGROUP     = safepost('SAMBA_WORKGROUP');
$SAMBA_SERVER_STRING = safepost('SAMBA_SERVER_STRING');
$SAMBA_SHARE_VIDEO0  = safepost('SAMBA_SHARE_VIDEO0');
$SAMBA_SHARE_MP3     = safepost('SAMBA_SHARE_MP3');
$SAMBA_SHARE_PHOTOS  = safepost('SAMBA_SHARE_PHOTOS');
$SAMBA_SHARE_FILME1  = safepost('SAMBA_SHARE_FILME1');
$SAMBA_SHARE_FILME2  = safepost('SAMBA_SHARE_FILME2');
$SAMBA_SHARE_FILME3  = safepost('SAMBA_SHARE_FILME3');
$SAMBA_SHARE_MEDIA   = safepost('SAMBA_SHARE_MEDIA');
$SAMBA_SHARE_ROOT    = safepost('SAMBA_SHARE_ROOT');
$SAMBA_LOG           = safepost('SAMBA_LOG');
$START_PROFTPD       = safepost('START_PROFTPD');
$FTPD_USER           = safepost('FTPD_USER');
$FTPD_PASSWD         = safepost('FTPD_PASSWD');
$FTPD_DEFAULT_DIR    = safepost('FTPD_DEFAULT_DIR');
$FTPD_PORT           = safepost('FTPD_PORT');
$FTPD_LOG            = safepost('FTPD_LOG');


if (safepost('submit2')) {
  exec("sudo $SH_EASYPORTAL stopvdr");
  exec("sudo $SH_EASYPORTAL setsysconfig FRIENDLY_FILENAMES $FRIENDLY_FILENAMES");
  exec("sudo $SH_EASYPORTAL setsysconfig START_SSHD $START_SSHD");
  exec("sudo $SH_EASYPORTAL setsysconfig START_XXV $START_XXV");
  exec("sudo $SH_EASYPORTAL setsysconfig START_LIGHTTPD $START_LIGHTTPD");
  exec("sudo $SH_EASYPORTAL setsysconfig START_VDRADMIN $START_VDRADMIN");
  exec("sudo $SH_EASYPORTAL setsysconfig START_NFS $START_NFS");
  exec("sudo $SH_EASYPORTAL setsysconfig STREAMQUALITY $STREAMQUALITY");
  exec("sudo $SH_EASYPORTAL setsysconfig WAKE_SERVER $WAKE_SERVER");
  exec("sudo $SH_EASYPORTAL setsysconfig SERVER_MAC $SERVER_MAC");
  exec("sudo $SH_EASYPORTAL setsysconfig START_SAMBA $START_SAMBA");
  exec("sudo $SH_EASYPORTAL setsysconfig SAMBA_WORKGROUP $SAMBA_WORKGROUP");
  exec("sudo $SH_EASYPORTAL setsysconfig SAMBA_SERVER_STRING $SAMBA_SERVER_STRING");
  exec("sudo $SH_EASYPORTAL setsysconfig SAMBA_SHARE_VIDEO0 $SAMBA_SHARE_VIDEO0");
  exec("sudo $SH_EASYPORTAL setsysconfig SAMBA_SHARE_MP3 $SAMBA_SHARE_MP3");
  exec("sudo $SH_EASYPORTAL setsysconfig SAMBA_SHARE_PHOTOS $SAMBA_SHARE_PHOTOS");
  exec("sudo $SH_EASYPORTAL setsysconfig SAMBA_SHARE_FILME1 $SAMBA_SHARE_FILME1");
  exec("sudo $SH_EASYPORTAL setsysconfig SAMBA_SHARE_FILME2 $SAMBA_SHARE_FILME2");
  exec("sudo $SH_EASYPORTAL setsysconfig SAMBA_SHARE_FILME3 $SAMBA_SHARE_FILME3");
  exec("sudo $SH_EASYPORTAL setsysconfig SAMBA_SHARE_MEDIA $SAMBA_SHARE_MEDIA");
  exec("sudo $SH_EASYPORTAL setsysconfig SAMBA_SHARE_ROOT $SAMBA_SHARE_ROOT");
  exec("sudo $SH_EASYPORTAL setsysconfig SAMBA_LOG $SAMBA_LOG");
  exec("sudo $SH_EASYPORTAL setsysconfig START_PROFTPD $START_PROFTPD");
  exec("sudo $SH_EASYPORTAL setsysconfig FTPD_USER $FTPD_USER");
  exec("sudo $SH_EASYPORTAL setsysconfig FTPD_PASSWD $FTPD_PASSWD");
  exec("sudo $SH_EASYPORTAL setsysconfig FTPD_DEFAULT_DIR $FTPD_DEFAULT_DIR");
  exec("sudo $SH_EASYPORTAL setsysconfig FTPD_PORT $FTPD_PORT");
  exec("sudo $SH_EASYPORTAL setsysconfig FTPD_LOG $FTPD_LOG");
  exec("sudo $SH_BEFEHL /usr/lib/vdr/easyvdr-set-settings samba");
  exec("sudo $SH_BEFEHL /usr/lib/vdr/easyvdr-set-settings proftpd");  
  exec("sudo $SH_EASYPORTAL startvdr");
//  echo "<h2>Werte wurden abgespeichert</h2>\n";              
} 
  
if (!safepost('START_SSHD')) {
  $lines = file ('/var/lib/vdr/sysconfig');
  foreach ($lines as $line)
  {
    // Fernzugriffs-Tools
    if(strpos($line, 'FRIENDLY_FILENAMES=')  !== false)  { $FRIENDLY_FILENAMES  = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'START_SSHD=')          !== false)  { $START_SSHD          = trim(str_replace ('"', '', strstr($line, '"'))); }
  //  if(strpos($line, 'START_XXV=')           !== false)  { $START_XXV           = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'START_LIGHTTPD=')      !== false)  { $START_LIGHTTPD      = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'START_VDRADMIN=')      !== false)  { $START_VDRADMIN      = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'START_NFS=')           !== false)  { $START_NFS           = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'STREAMQUALITY=')       !== false)  { $STREAMQUALITY       = trim(str_replace ('"', '', strstr($line, '"'))); }

    // Server-WOL
    if(strpos($line, 'WAKE_SERVER=')         !== false)  { $WAKE_SERVER         = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'SERVER_MAC=')          !== false)  { $SERVER_MAC          = trim(str_replace ('"', '', strstr($line, '"'))); }
      
    // Samba-Einstellungen
    if(strpos($line, 'START_SAMBA=')         !== false)  { $START_SAMBA         = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'SAMBA_WORKGROUP=')     !== false)  { $SAMBA_WORKGROUP     = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'SAMBA_SERVER_STRING=') !== false)  { $SAMBA_SERVER_STRING = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'SAMBA_SHARE_VIDEO0=')  !== false)  { $SAMBA_SHARE_VIDEO0  = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'SAMBA_SHARE_MP3=')     !== false)  { $SAMBA_SHARE_MP3     = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'SAMBA_SHARE_PHOTOS=')  !== false)  { $SAMBA_SHARE_PHOTOS  = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'SAMBA_SHARE_FILME1=')  !== false)  { $SAMBA_SHARE_FILME1  = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'SAMBA_SHARE_FILME2=')  !== false)  { $SAMBA_SHARE_FILME2  = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'SAMBA_SHARE_FILME3=')  !== false)  { $SAMBA_SHARE_FILME3  = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'SAMBA_SHARE_MEDIA=')   !== false)  { $SAMBA_SHARE_MEDIA   = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'SAMBA_SHARE_ROOT=')    !== false)  { $SAMBA_SHARE_ROOT    = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'SAMBA_LOG=')           !== false)  { $SAMBA_LOG           = trim(str_replace ('"', '', strstr($line, '"'))); }
    
    // FTP-Setup
    if(strpos($line, 'START_PROFTPD=')       !== false)  { $START_PROFTPD       = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'FTPD_USER=')           !== false)  { $FTPD_USER           = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'FTPD_PASSWD=')         !== false)  { $FTPD_PASSWD         = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'FTPD_DEFAULT_DIR=')    !== false)  { $FTPD_DEFAULT_DIR    = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'FTPD_PORT=')           !== false)  { $FTPD_PORT           = trim(str_replace ('"', '', strstr($line, '"'))); }
    if(strpos($line, 'FTPD_LOG=')            !== false)  { $FTPD_LOG            = trim(str_replace ('"', '', strstr($line, '"'))); }
  }
}

echo "<form action='".$_SERVER['PHP_SELF']."' method='post'>";

echo "<div id='kopf2'><a href='einstellungen.php' class='w'>$txt_einstellungen</a> - $txt_kopf2</div>";

echo "<div id='inhalt'>";
echo "<center>";
include('includes/subnavi_einstellungen.php'); 
echo "<h2>$txt_ueberschrift</h2>";

echo "<div class='links'>";

// *************************************** Container Begin 'Fernzugriffs-Tools' ****************************************************
container_begin(2, "Lan.png", $txt_ue_ra_tools);

echo "<table border='0' cellspacing='2' width='100%'>";

echo "<tr>";
echo "  <td>$txt_friendly_filenames</td>";
if ($FRIENDLY_FILENAMES == "yes"){$checked_y = " checked"; } else {$checked_y = ""; }
if ($FRIENDLY_FILENAMES == "no") {$checked_n = " checked"; } else {$checked_n = ""; }
echo "  <td>$txt_yes <input type='radio' name='FRIENDLY_FILENAMES' value='yes' ".$checked_y.">&nbsp;&nbsp; ";
echo "      $txt_no <input type='radio' name='FRIENDLY_FILENAMES' value='no' ".$checked_n."></td>";
echo "</tr>\n";
echo "<tr>";
echo "  <td>$txt_start_ssh</td>";
if ($START_SSHD == "yes"){$checked_y = " checked"; } else {$checked_y = ""; }
if ($START_SSHD == "no") {$checked_n = " checked"; } else {$checked_n = ""; }
echo "  <td width='260px'>$txt_yes <input type='radio' name='START_SSHD' value='yes' ".$checked_y.">&nbsp;&nbsp; ";
echo "      $txt_no <input type='radio' name='START_SSHD' value='no' ".$checked_n."></td>";
echo "</tr>";
/*
echo "<tr>";
echo "  <td>$txt_start_xxv</td>";
if ($START_XXV == "yes"){$checked_y = " checked"; } else {$checked_y = ""; }
if ($START_XXV == "no") {$checked_n = " checked"; } else {$checked_n = ""; }
echo "  <td>$txt_yes <input type='radio' name='START_XXV' value='yes' ".$checked_y.">&nbsp;&nbsp; ";
echo "      $txt_no <input type='radio' name='START_XXV' value='no' ".$checked_n."></td>";
echo "</tr>\n";
*/
echo "<tr>";
echo "  <td>$txt_start_webserver</td>";
if ($START_LIGHTTPD == "yes"){$checked_y = " checked"; } else {$checked_y = ""; }
if ($START_LIGHTTPD == "no") {$checked_n = " checked"; } else {$checked_n = ""; }
echo "  <td>$txt_yes <input type='radio' name='START_LIGHTTPD' value='yes' ".$checked_y.">&nbsp;&nbsp; ";
echo "      $txt_no <input type='radio' name='START_LIGHTTPD' value='no' ".$checked_n."></td>";
echo "</tr>\n";
echo "<tr>";
echo "  <td>$txt_start_vdradmin</td>";
if ($START_VDRADMIN == "yes"){$checked_y = " checked"; } else {$checked_y = ""; }
if ($START_VDRADMIN == "no") {$checked_n = " checked"; } else {$checked_n = ""; }
echo "  <td>$txt_yes <input type='radio' name='START_VDRADMIN' value='yes' ".$checked_y.">&nbsp;&nbsp; ";
echo "      $txt_no <input type='radio' name='START_VDRADMIN' value='no' ".$checked_n."></td>";
echo "</tr>\n";
echo "<tr>";
echo "  <td>$txt_start_nfs</td>";
if ($START_NFS == "yes"){$checked_y = " checked"; } else {$checked_y = ""; }
if ($START_NFS == "no") {$checked_n = " checked"; } else {$checked_n = ""; }
echo "  <td>$txt_yes <input type='radio' name='START_NFS' value='yes' ".$checked_y.">&nbsp;&nbsp; ";
echo "      $txt_no <input type='radio' name='START_NFS' value='no' ".$checked_n."></td>";
echo "</tr>\n";
echo "<tr>";
echo "  <td>$txt_stream_quality</td>";
echo "  <td><select name='streamquality' size='1' style='width: 240px'>";
echo "          <option ";
if ($STREAMQUALITY == "LAN10"){echo " selected";}
echo                  ">LAN10</option>";
echo "            <option ";
if ($STREAMQUALITY == "DSL1000"){echo " selected";}
echo                  ">DSL1000</option>";
echo "            <option ";
if ($STREAMQUALITY == "DSL2000"){echo " selected";}
echo                  ">DSL2000</option>";
echo "            <option ";
if ($STREAMQUALITY == "DSL3000"){echo " selected";}
echo                  ">DSL3000</option>";
echo "            <option ";
if ($STREAMQUALITY == "DSL6000"){echo " selected";}
echo                  ">DSL6000</option>";
echo "            <option ";
if ($STREAMQUALITY == "DSL16000"){echo " selected";}
echo                  ">DSL16000</option>";
echo "            <option ";
if ($STREAMQUALITY == "WLAN11"){echo " selected";}
echo                  ">WLAN11</option>";
echo "            <option ";
if ($STREAMQUALITY == "WLAN54"){echo " selected";}
echo                  ">WLAN54</option>";
echo "         </select></td>";
echo "</tr>";
echo "</table>";

container_end();
// *************************************** Container Ende ****************************************************

// *************************************** Container Begin 'FTP-Setup' ****************************************************
container_begin(2, "ftp.png", $txt_ue_ftp);

echo "<table border='0' cellspacing='2' width='100%'>";
echo "  <tr>";
echo "    <td>$txt_start_ftp</td>";
if ($START_PROFTPD == "yes"){$checked_y = " checked"; } else {$checked_y = ""; }
if ($START_PROFTPD == "no") {$checked_n = " checked"; } else {$checked_n = ""; }
echo "    <td width='260px'>$txt_yes <input type='radio' name='START_PROFTPD' value='yes' ".$checked_y." onChange='this.form.submit()'>&nbsp;&nbsp; ";
echo "        $txt_no <input type='radio' name='START_PROFTPD' value='no' ".$checked_n." onChange='this.form.submit()'></td>";
echo "  </tr>\n";
if ($START_PROFTPD == "yes") {
  echo "  <tr>";
  echo "    <td>$txt_ftp_username</td>";
  echo "    <td><input type='text' name='FTPD_USER' size='37' value='$FTPD_USER'></td>";
  echo "  </tr>";
  echo "  <tr>";
  echo "    <td>$txt_ftp_password</td>";
  echo "    <td><input type='text' name='FTPD_PASSWD' size='37' value='$FTPD_PASSWD'></td>";
  echo "  </tr>";
  echo "  <tr>";
  echo "    <td>$txt_ftp_dir</td>";
  echo "    <td><input type='text' name='FTPD_DEFAULT_DIR' size='37' value='$FTPD_DEFAULT_DIR'></td>";
  echo "  </tr>";
  echo "  <tr>";
  echo "    <td>$txt_ftp_port</td>";
  echo "    <td><input type='text' name='FTPD_PORT' size='37' value='$FTPD_PORT'></td>";
  echo "  </tr>";
  echo "  <tr>";
  echo "    <td>$txt_ftp_log</td>";
  if ($FTPD_LOG == "yes"){$checked_y = " checked"; } else {$checked_y = ""; }
  if ($FTPD_LOG == "no") {$checked_n = " checked"; } else {$checked_n = ""; }
  echo "    <td>$txt_yes <input type='radio' name='FTPD_LOG' value='yes' ".$checked_y.">&nbsp;&nbsp; ";
  echo "        $txt_no <input type='radio' name='FTPD_LOG' value='no' ".$checked_n."></td>";
  echo "  </tr>\n";
}
else {
  echo "<input type='hidden' name='FTPD_USER'        value='$FTPD_USER'>";
  echo "<input type='hidden' name='FTPD_PASSWD'      value='$FTPD_PASSWD'>";
  echo "<input type='hidden' name='FTPD_DEFAULT_DIR' value='$FTPD_DEFAULT_DIR'>";
  echo "<input type='hidden' name='FTPD_PORT'        value='$FTPD_PORT'>";
  echo "<input type='hidden' name='FTPD_LOG'         value='$FTPD_LOG'>";
}

echo "</table>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</div>";  // class='links'
echo "<div class='rechts'>";

// *************************************** Container Begin 'Samba-Einstellungen' ****************************************************
container_begin(2, "samba.png", $txt_ue_samba);

echo "<table border='0' cellspacing='2' width='100%'>";
echo "  <tr>";
echo "    <td>$txt_start_samba</td>";
if ($START_SAMBA == "yes"){$checked_y = " checked"; } else {$checked_y = ""; }
if ($START_SAMBA == "no") {$checked_n = " checked"; } else {$checked_n = ""; }
echo "    <td width='260px'>$txt_yes <input type='radio' name='START_SAMBA' value='yes' ".$checked_y." onChange='this.form.submit()'>&nbsp;&nbsp; ";
echo "        $txt_no <input type='radio' name='START_SAMBA' value='no' ".$checked_n." onChange='this.form.submit()'></td>";
echo "  </tr>\n";
if ($START_SAMBA == "yes") {
  echo "  <tr>";
  echo "    <td>$txt_workgroup</td>";
  echo "    <td><input type='text' name='SAMBA_WORKGROUP' size='37' value='$SAMBA_WORKGROUP'></td>";
  echo "  </tr>";
  echo "  <tr>";
  echo "    <td>$txt_serverstring</td>";
  echo "    <td><input type='text' name='SAMBA_SERVER_STRING' size='37' value='$SAMBA_SERVER_STRING'></td>";
  echo "  </tr>";
  echo "  <tr>";
  echo "    <td>$txt_smb_video0</td>";
  if ($SAMBA_SHARE_VIDEO0 == "yes"){$checked_y = " checked"; } else {$checked_y = ""; }
  if ($SAMBA_SHARE_VIDEO0 == "no") {$checked_n = " checked"; } else {$checked_n = ""; }
  echo "    <td>$txt_yes <input type='radio' name='SAMBA_SHARE_VIDEO0' value='yes' ".$checked_y.">&nbsp;&nbsp; ";
  echo "        $txt_no <input type='radio' name='SAMBA_SHARE_VIDEO0' value='no' ".$checked_n."></td>";
  echo "  </tr>\n";
  echo "  <tr>";
  echo "    <td>$txt_smb_mp3</td>";
  if ($SAMBA_SHARE_MP3 == "yes"){$checked_y = " checked"; } else {$checked_y = ""; }
  if ($SAMBA_SHARE_MP3 == "no") {$checked_n = " checked"; } else {$checked_n = ""; }
  echo "    <td>$txt_yes <input type='radio' name='SAMBA_SHARE_MP3' value='yes' ".$checked_y.">&nbsp;&nbsp; ";
  echo "        $txt_no <input type='radio' name='SAMBA_SHARE_MP3' value='no' ".$checked_n."></td>";
  echo "  </tr>\n";
  echo "  <tr>";
  echo "    <td>$txt_smb_photos</td>";
  if ($SAMBA_SHARE_PHOTOS == "yes"){$checked_y = " checked"; } else {$checked_y = ""; }
  if ($SAMBA_SHARE_PHOTOS == "no") {$checked_n = " checked"; } else {$checked_n = ""; }
  echo "    <td>$txt_yes <input type='radio' name='SAMBA_SHARE_PHOTOS' value='yes' ".$checked_y.">&nbsp;&nbsp; ";
  echo "        $txt_no <input type='radio' name='SAMBA_SHARE_PHOTOS' value='no' ".$checked_n."></td>";
  echo "  </tr>\n";
  echo "  <tr>";
  echo "    <td>$txt_smb_filme1</td>";
  if ($SAMBA_SHARE_FILME1 == "yes"){$checked_y = " checked"; } else {$checked_y = ""; }
  if ($SAMBA_SHARE_FILME1 == "no") {$checked_n = " checked"; } else {$checked_n = ""; }
  echo "    <td>$txt_yes <input type='radio' name='SAMBA_SHARE_FILME1' value='yes' ".$checked_y.">&nbsp;&nbsp; ";
  echo "        $txt_no <input type='radio' name='SAMBA_SHARE_FILME1' value='no' ".$checked_n."></td>";
  echo "  </tr>\n";
  echo "  <tr>";
  echo "    <td>$txt_smb_filme2</td>";
  if ($SAMBA_SHARE_FILME2 == "yes"){$checked_y = " checked"; } else {$checked_y = ""; }
  if ($SAMBA_SHARE_FILME2 == "no") {$checked_n = " checked"; } else {$checked_n = ""; }
  echo "    <td>$txt_yes <input type='radio' name='SAMBA_SHARE_FILME2' value='yes' ".$checked_y.">&nbsp;&nbsp; ";
  echo "        $txt_no <input type='radio' name='SAMBA_SHARE_FILME2' value='no' ".$checked_n."></td>";
  echo "  </tr>\n";
  echo "  <tr>";
  echo "    <td>$txt_smb_filme3</td>";
  if ($SAMBA_SHARE_FILME3 == "yes"){$checked_y = " checked"; } else {$checked_y = ""; }
  if ($SAMBA_SHARE_FILME3 == "no") {$checked_n = " checked"; } else {$checked_n = ""; }
  echo "    <td>$txt_yes <input type='radio' name='SAMBA_SHARE_FILME3' value='yes' ".$checked_y.">&nbsp;&nbsp; ";
  echo "        $txt_no <input type='radio' name='SAMBA_SHARE_FILME3' value='no' ".$checked_n."></td>";
  echo "  </tr>\n";
  echo "  <tr>";
  echo "    <td>$txt_smb_media</td>";
  if ($SAMBA_SHARE_MEDIA == "yes"){$checked_y = " checked"; } else {$checked_y = ""; }
  if ($SAMBA_SHARE_MEDIA == "no") {$checked_n = " checked"; } else {$checked_n = ""; }
  echo "    <td>$txt_yes <input type='radio' name='SAMBA_SHARE_MEDIA' value='yes' ".$checked_y.">&nbsp;&nbsp; ";
  echo "        $txt_no <input type='radio' name='SAMBA_SHARE_MEDIA' value='no' ".$checked_n."></td>";
  echo "  </tr>\n";
  echo "  <tr>";
  echo "    <td>$txt_smb_root</td>";
  if ($SAMBA_SHARE_ROOT == "yes"){$checked_y = " checked"; } else {$checked_y = ""; }
  if ($SAMBA_SHARE_ROOT == "no") {$checked_n = " checked"; } else {$checked_n = ""; }
  echo "    <td>$txt_yes <input type='radio' name='SAMBA_SHARE_ROOT' value='yes' ".$checked_y.">&nbsp;&nbsp; ";
  echo "        $txt_no <input type='radio' name='SAMBA_SHARE_ROOT' value='no' ".$checked_n."></td>";
  echo "  </tr>\n";
  echo "  <tr>";
  echo "    <td>$txt_smb_log</td>";
  if ($SAMBA_LOG == "yes"){$checked_y = " checked"; } else {$checked_y = ""; }
  if ($SAMBA_LOG == "no") {$checked_n = " checked"; } else {$checked_n = ""; }
  echo "    <td>$txt_yes <input type='radio' name='SAMBA_LOG' value='yes' ".$checked_y.">&nbsp;&nbsp; ";
  echo "        $txt_no <input type='radio' name='SAMBA_LOG' value='no' ".$checked_n."></td>";
  echo "  </tr>\n";
}
else {
  echo "<input type='hidden' name='SAMBA_WORKGROUP' value='$SAMBA_WORKGROUP'>";
  echo "<input type='hidden' name='SAMBA_SERVER_STRING' value='$SAMBA_SERVER_STRING'>";
  echo "<input type='hidden' name='SAMBA_SHARE_VIDEO0' value='$SAMBA_SHARE_VIDEO0'>";
  echo "<input type='hidden' name='SAMBA_SHARE_MP3' value='$SAMBA_SHARE_MP3'>";
  echo "<input type='hidden' name='SAMBA_SHARE_PHOTOS' value='$SAMBA_SHARE_PHOTOS'>";
  echo "<input type='hidden' name='SAMBA_SHARE_FILME1' value='$SAMBA_SHARE_FILME1'>";
  echo "<input type='hidden' name='SAMBA_SHARE_FILME2' value='$SAMBA_SHARE_FILME2'>";
  echo "<input type='hidden' name='SAMBA_SHARE_FILME3' value='$SAMBA_SHARE_FILME3'>";
  echo "<input type='hidden' name='SAMBA_SHARE_MEDIA' value='$SAMBA_SHARE_MEDIA'>";
  echo "<input type='hidden' name='SAMBA_SHARE_ROOT' value='$SAMBA_SHARE_ROOT'>";
  echo "<input type='hidden' name='SAMBA_LOG' value='$SAMBA_LOG'>";
}
echo "</table>";

container_end();
// *************************************** Container Ende ****************************************************

// *************************************** Container Begin 'Server-WOL' ****************************************************
container_begin(2, "Lan.png", $txt_ue_wol);

echo "<table border='0' cellspacing='2' width='100%'>";
echo "  <tr>";
echo "    <td>$txt_server_wol</td>";
if ($WAKE_SERVER == "yes"){$checked_y = " checked"; } else {$checked_y = ""; }
if ($WAKE_SERVER == "no") {$checked_n = " checked"; } else {$checked_n = ""; }
echo "    <td width='260px'>$txt_yes <input type='radio' name='WAKE_SERVER' value='yes' ".$checked_y." onChange='this.form.submit()'>&nbsp;&nbsp; ";
echo "        $txt_no <input type='radio' name='WAKE_SERVER' value='no' ".$checked_n." onChange='this.form.submit()'></td>";
echo "  </tr>\n";
if ($WAKE_SERVER == "yes") {
  echo "  <tr>";
  echo "    <td>$txt_server_mac</td>";
  echo "    <td><input type='text' name='SERVER_MAC' size='37' value='$SERVER_MAC'></td>";
  echo "  </tr>";
}
else {
  echo "<input type='hidden' name='SERVER_MAC' value='$SERVER_MAC'>";
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