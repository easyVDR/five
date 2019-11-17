<?php
include('includes/kopf.php'); 

$aktion   = safeget('aktion');
$easyinfo = safeget('info');

function lastModification ($dir) {
  if ( is_file ( $dir ) ) return false;
  $lastfile = '';
  if( strlen( $dir ) - 1 != '\\' || strlen( $dir ) - 1 != '/' ) { $dir .= '/'; }
  $handle = @opendir($dir);
  if( !$handle ) { return false;}
  while ( ( $file = readdir( $handle ) ) !== false ) {
    if( $file != '.' && $file != '..' && is_file ( $dir.$file ) ) {
      if( filemtime( $dir.$file ) >= filemtime( $dir.$lastfile ) ) { $lastfile = $file; }
      if ( empty( $lastfile ) ) { $lastfile = $file; }
    }
  }
  closedir( $handle );
  return $lastfile;
}

// Löschen von Alt-Lasten
exec("sudo $SH_EASYPORTAL delAltlasten");

$log_path = $EASYPORTAL_DOC_ROOT."log/easyinfo/";   // $log_path = '/var/www/log/';

if ($aktion == "erstellen") {
  exec("sudo $SH_EASYINFO2 $EASYPORTAL_VER");
}

if ($aktion == "aktiv") {
  unlink($log_path."easyinfo.log");
  symlink($log_path.$easyinfo, $log_path."easyinfo.log");
}

if ($aktion == "del") {
  unlink($log_path.$easyinfo);
  if (substr(readlink ( '/var/www/log/easyinfo/easyinfo.log' ), 13) == $log_path.$easyinfo)
  { unlink($log_path."easyinfo.log"); }
}

if (!file_exists($log_path."*.log")) {
  $exist = glob($log_path."*.log");
  if ($exist[0]=="") {
    exec("sudo $SH_EASYINFO2");	 
  }
}

if (!file_exists($log_path."easyinfo.log")) {
  unlink ($log_path."easyinfo.log");
  symlink ($log_path. lastModification($log_path) , $log_path."easyinfo.log");
}

$uploadtxt = "";
if (isset($_FILES['uploaddatei']['name'])) {
//if ( $_FILES['uploaddatei']['name'] <> "" ) {
  // Datei wurde durch HTML-Formular hochgeladen
  if ( $_FILES['uploaddatei']['name'] <> '' ) {
    $array = pathinfo($_FILES['uploaddatei']['name']);
    if(strtolower($array['extension'])!="log") {
      $uploadtxt = "<p><font color='red'>$txt_no_log</font>";  
    } else {
      move_uploaded_file ($_FILES['uploaddatei']['tmp_name'] , 'log/'. $_FILES['uploaddatei']['name'] );
      unlink ($log_path."easyinfo.log");
      symlink ( $log_path.$_FILES['uploaddatei']['name'] , $log_path."easyinfo.log");
      $uploadtxt = "<p>$txt_upload_ok_1 '". $_FILES['uploaddatei']['name']. "' $txt_upload_ok_2";
    }
  } else {
    $uploadtxt = "<p>$txt_no_allowed</p>";
  }
}

echo "<div id='kopf2'><a href='easyinfo.php' class='w'>EasyInfo</a> - $txt_kopf2</div>";

echo "<div id='inhalt'>";
echo "<center>";
include('includes/subnavi_easyinfo.php'); 

$akt_easyinfo = substr(readlink ( '/var/www/log/easyinfo/easyinfo.log' ), 13);
echo "<h2>easyInfo: '$akt_easyinfo'</h2>";

echo "<div class='links'>";

// *************************************** Container Begin '' ****************************************************
container_begin(2, "Info.png", $txt_ue_erstellen);

echo "$txt_erstellen1<br/>";
echo "$txt_erstellen2<br/><br/>";
echo "<a href='easyinfo_admin.php?aktion=erstellen' onclick=\"wait('WaitEasyInfo')\"><button>$txt_b_erstellen</button></a>";

container_end();
// *************************************** Container Ende ****************************************************

// *************************************** Container Begin '' ****************************************************
container_begin(2, "upload.png", $txt_ue_hochladen);

echo "$txt_hochladen1<br/>";
echo "$txt_hochladen2<br/><br/>";
echo "<form enctype='multipart/form-data' action='easyinfo_admin.php' method='POST'>";
echo "    <input name='uploaddatei' type='file' /><br><br>";
echo "    <input type='submit' value='$txt_b_hochladen' />";
echo "</form>";
echo "<br/>";
echo "$uploadtxt";

container_end();
// *************************************** Container Ende ****************************************************

echo "</div>";  // class='links'
echo "<div class='rechts'>";

// *************************************** Container Begin '' ****************************************************
container_begin(2, "Info.png", $txt_ue_save);

//echo "<a href='../log/easyinfo.zip'><img src='images/down.png'></a>";
$pfad = "log/easyinfo/";

echo "<table class='tab1'>";
echo "<tr align='center' height='30px'>";
echo "  <td><b>$txt_filename</b></td>";
echo "  <td><b>$txt_filesize</b></td>";
echo "  <td><b>$txt_filedate</b></td>";
echo "  <td><b>$txt_fileaction</b></td>";
echo "</tr>";
$dir = opendir($pfad) or die("$pfad $txt_error1");
$dir2 = array();
while($file = readdir($dir)){
  if($file != "." && $file != ".."){
    $dir2[] = $file;
  }
} 

$fce = "</font>";
rsort($dir2); 

foreach($dir2 as $file){ 
  $array = pathinfo($log_path.$file);
  if (($file != ".") && ($file != "..") && ($file != "easyinfo.log") && (strtolower($array['extension'])=="log")) {
    if ("easyinfo/".$file == $akt_easyinfo) { 
      $fc = "<font color='#993333'>"; } 
    else { 
      $fc = "<font color='#000000'>"; 
    } 
    echo "<tr><td>".$fc.$file.$fce."</td>";
    echo     "<td align='right'>".$fc.bcdiv(filesize($pfad.$file),1024,1)." kB".$fce."</td>";
    echo     "<td>".$fc. date("d.m.Y H:i:s",filemtime($pfad.$file)) .$fce."</td>";
    echo     "<td align=center><a onmouseover=\"Tip('$TIP_dl')\" href='log/easyinfo/$file'><img src='/images/icon/down.png'></a>";
    if ("easyinfo/".$file == $akt_easyinfo) { echo "&nbsp;<img src='/images/icon/ok.png'>";} 
    else { echo "<a href='?aktion=aktiv&info=$file' onmouseover=\"Tip('$TIP_aktiv')\">&nbsp;<img src='/images/icon/ok_no.png'>"; }
    echo "<a onmouseover=\"Tip('$TIP_del')\" href='easyinfo_admin.php?aktion=del&info=$file' onclick=\"wait('WaitEasyInfo')\">&nbsp;<img src=/images/icon/del.png></a></td>";
    echo "</tr>";
  }
}
closedir($dir);
echo "</table>";

echo "<br/>$txt_save1<br/>$txt_save2<br/>$txt_save3</b>";
echo "<br/><br/>";
echo "$txt_save4";

container_end();
// *************************************** Container Ende ****************************************************

echo "</div>";  // class='rechts'
echo "<div class='clear'></div>"; // Ende 2-spaltig
echo "</center>";
echo "</div>";    // Inhalt

include('includes/fuss.php'); 
?>