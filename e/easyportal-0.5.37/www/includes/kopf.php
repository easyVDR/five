<?php
session_start();

$startzeit = explode(" ", microtime());
$startzeit = $startzeit[0]+$startzeit[1];


// ****************************** Fehler-Report *****************************************
if ($_SESSION['php_error_report'] == 1) { 
  //echo "<font color='yellow'>";
  error_reporting(E_ALL);
  ini_set("display_errors", 1);  
}


// ********************************* Datenbank *****************************************

include('txt-db-api/txt-db-api.php');
$db = new Database("portal");


// ********************************* User *****************************************
if (safepost('logout')) { 
  setcookie("user_id","",time() - (3600*10));
}

if (($_SESSION["set_cookie"] == "1") AND ($_SESSION['user_id'] != "")) { 
  setcookie("user_id", $_SESSION['user_id'], time()+(3600*24*100)); 
  $_SESSION["set_cookie"] = "0"; 
} 


if ((isset($_COOKIE["user_id"])) and (!isset($_SESSION["user_id"]))) {
  // Es gibt ein UserID-Cookie, aber noch keine Session-Variablen 
  //setcookie("user_id", $_SESSION["user_id"], time()+(3600*24*100));  // Session verlängern
  $_SESSION["user_id"] = $_COOKIE["user_id"];

  $resultSet = $db->executeQuery("SELECT * FROM user WHERE ID='".$_COOKIE['user_id']."'");
  $resultSet->next();
  $_SESSION["user_id"]   = $resultSet->getCurrentValueByName("ID");
  $_SESSION["username"]  = $resultSet->getCurrentValueByName("User");
  $_SESSION["sprache"]   = $resultSet->getCurrentValueByName("Sprache");
  $_SESSION["angemeldet_bleiben"] = $resultSet->getCurrentValueByName("angemeldet_bleiben");
  $_SESSION["r_edit"]    = $resultSet->getCurrentValueByName("r_edit");
  $_SESSION["r_expert"]  = $resultSet->getCurrentValueByName("r_expert");
  $_SESSION["design"]    = $resultSet->getCurrentValueByName("design");
}

if (!isset($_SESSION["user_id"])) {
  $_SESSION["sprache"]  = "Deutsch";
  $_SESSION["angemeldet_bleiben"] = "n";
  $_SESSION["r_edit"]   = "n";
  $_SESSION["r_expert"] = "n";
  $_SESSION["design"]   = "easyportal_20";
}


// ************************** Header *************************************

// <script src="/includes/js/prototype.js" type="text/javascript"></script>
// <script src="/includes/js/php_file_tree.js" type="text/javascript"></script>
// <script src="/includes/js/jquery-1.11.3.js" type="text/javascript"></script> 
// <script src="/includes/js/jquery-2.1.4.min.js" type="text/javascript"></script> 

?>
<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML Basic 1.1//EN"
    "http://www.w3.org/TR/xhtml-basic/xhtml-basic11.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="de">
<head>
  <title>easyVDR-Webportal</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link type="text/css" rel="stylesheet" href="/css/<?php echo $_SESSION["design"]; ?>.css"> 
  <link type="text/css" rel="stylesheet" href="/css/php_file_tree.css" media="screen" />
  <link type="image/vnd.microsoft.icon" rel="icon" href="images/favicon.ico">
  <script type="text/javascript" src="/includes/js/easyportal.js" ></script>
  <script type="text/javascript" src="/includes/js/jquery-2.1.4.min.js"></script> 
  <script type="text/javascript" src="/includes/js/php_file_tree_jquery.js" ></script> 
  <meta http-equiv="content-type" content="text/html; charset=UTF-8"/>

  <script type="text/javascript">
    function FensterOeffnenFB (Adresse) {
      MeinFenster = window.open(Adresse, "fb", "width=250,height=620,left=50,top=50,resizable=yes,directories=no,menubar=no,scrollable=no,status=no,location=no");
      MeinFenster.focus();
    }

    function selFensterSize(kleiner, pixel, groesser){
      var breit = window.innerWidth;
      if (breit >= pixel) { document.write(groesser); }
      else { document.write(kleiner); }
    }
    
    $(function() { $("table tr:nth-child(even)").addClass("striped"); });

  </script>
</head>
<body>
<?php



// ****************************** Variablen Allgemein *****************************************

$EASYPORTAL_VER      = "0.5.37";                       // Easyportal-Version
$EASYPORTAL_DOC_ROOT = "/var/www/";                    // Easyportal-Verzeichnis
$eplists_dir         = "/var/cache/eplists/episodes";  // Eplists-Verzeichnis

// Shell-Scripts für diverse OS-Kommandos
$SH_EASYINFO2      = $EASYPORTAL_DOC_ROOT . "scripts/easyinfo2.sh";
$SH_EASYINFO_ERROR = $EASYPORTAL_DOC_ROOT . "scripts/easyinfo_error.sh";
$SH_EASYPORTAL     = $EASYPORTAL_DOC_ROOT . "scripts/easyportal.sh";
$SH_BACKUP         = $EASYPORTAL_DOC_ROOT . "scripts/backup.sh";
$SH_UPDATE         = $EASYPORTAL_DOC_ROOT . "scripts/update.sh";
$SH_EASYVDR_HW     = $EASYPORTAL_DOC_ROOT . "scripts/easyvdr_hw.sh";
$SH_BEFEHL         = $EASYPORTAL_DOC_ROOT . "scripts/befehl.sh";

// ******************************** Funktionen *****************************************

include('class_debug.php'); 

# Importiere URL Parameter aus $_POST
function safepost ($param) {
  $retval="";
  if (isset($_POST["$param"])) $retval=$_POST["$param"];
  return $retval;
}

# Importiere URL Parameter aus $_GET
function safeget ($param) {
  $retval="";
  if (isset($_GET["$param"])) $retval=$_GET["$param"];
  return $retval;
}

function unhtmlentities($string) {
  // replace numeric entities
  $string = preg_replace('~&#x([0-9a-f]+);~ei', 'chr(hexdec("\\1"))', $string);
  $string = preg_replace('~&#([0-9]+);~e', 'chr("\\1")', $string);
  // replace literal entities
  $trans_tbl = get_html_translation_table(HTML_ENTITIES);
  $trans_tbl = array_flip($trans_tbl);
  return strtr($string, $trans_tbl);
}

function log_portal($logeintrag) {
  $fp = fopen("/var/www/log/easyportal.log", "a+");
  $datum = date("d.m.Y H:i:s",time());
  $eintrag = $datum." - ".$logeintrag."\n";
  fwrite($fp, $eintrag);
  fclose($fp); 
}

function container_begin($spalten, $bild, $ueberschrift) {
  // Spalten sind noch Altlasten, demnächst mal entfernen
  $id_link      = rand();
  $id_container = $id_link."container";
  ?>
  <script type="text/javascript">
  $(document).ready(function(){
    var src1 = "images/green_up.png";
    var src2 = "images/green_down.png";
    $('#<?php echo $id_link;?>').click(function(){
      $('#<?php echo $id_container;?>').toggle('slow');
      var src = $('#<?php echo $id_link;?> img').attr('src'); 
      if(src == src1){$('#<?php echo $id_link;?> img').attr('src',src2);}
      else{$('#<?php echo $id_link;?> img').attr('src',src1);}
    })
  });
  </script>
  <?php
  echo "<div class='Container'>";
  echo "<div class='title'><img src='images/menuicons/$bild' align='left' class='icon' width='30'>&nbsp;&nbsp;&nbsp;$ueberschrift";
  echo " <a align='right' href='#' id='$id_link'><img src='images/green_up.png' align='right' class='icon' width='30'></a>";
  echo "</div>";
  echo "<div class='ContainerInhalt' style='display: block' id='$id_container'>";
}

function container_end() {
  echo "</div>";  // Padding im Container-Inhalt
  echo "</div>";  // Container
}


// ******************************* Mobil oder Desktop ? **********************************

if ($_SESSION["style"] == "") {
  include('includes/function_check_mobile.php'); 
  $style = "desktop";
  if(check_mobile()) { $style = "mobile"; }
  $_SESSION["style"] = $style;
} else {
  $style = $_SESSION["style"];
}



// ********************************* Sprache *****************************************

$datei_t = substr ($_SERVER['PHP_SELF'], 1, strlen($_SERVER['PHP_SELF']));
$sprachdatei = "lang/".$_SESSION["sprache"]."/txt_".$datei_t;
if (file_exists($EASYPORTAL_DOC_ROOT.$sprachdatei)) {
  include($sprachdatei);
  $txt_debug_meldungen[] = "Sprachdatei Seite: $sprachdatei";
} else {
  $txt_debug_meldungen[] = "Sprachdatei Seite: --- existiert nicht ---";
}

//echo "Sprachdatei Seite: '$sprachdatei'";
$sprachdatei = "lang/".$_SESSION["sprache"]."/m_txt_hauptmenu.php";
include($sprachdatei);

$sprachdatei = "lang/".$_SESSION["sprache"]."/txt_kopf.php";
include($sprachdatei);


// ********************************* Wartemeldungen *****************************************
echo "<font color='black'>";
echo "<script type='text/javascript' src='includes/js/ws_tooltip.js'></script>";

$sprachdatei = "lang/".$_SESSION["sprache"]."/txt_b_wartemeldungen.php";
include($sprachdatei);
echo "<div class='wait' id='WaitSeite'          >$txt_WaitSeite</div>";
echo "<div class='wait' id='WaitEasyInfo'       >$txt_WaitEasyInfo</div>";
echo "<div class='wait' id='WaitBackup'         >$txt_WaitBackup</div>";
echo "<div class='wait' id='WaitSave'           >$txt_WaitSave</div>";
echo "<div class='wait' id='WaitUpdate'         >$txt_WaitUpdate</div>";
echo "<div class='wait' id='WaitUpgrade_s'      >$txt_WaitUpgrade_s</div>";
echo "<div class='wait' id='WaitUpgrade'        >$txt_WaitUpgrade</div>";
echo "<div class='wait' id='WaitDistUpgrade_s'  >$txt_WaitDistUpgrade_s</div>";
echo "<div class='wait' id='WaitDistUpgrade'    >$txt_WaitDistUpgrade</div>";
echo "<div class='wait' id='WaitStartvdr'       >$txt_WaitStartvdr</div>";
echo "<div class='wait' id='WaitStopvdr'        >$txt_WaitStopvdr</div>";
echo "<div class='wait' id='WaitInstall'        >$txt_WaitInstall</div>";
echo "<div class='wait' id='WaitAktivieren'     >$txt_WaitAktivieren</div>";
echo "<div class='wait' id='WaitLANSuche'       >$txt_WaitLANSuche</div>";
echo "<div class='wait' id='WaitPaketquellen'   >$txt_WaitPaketquellen</div>";
echo "<div class='wait' id='WaitXBMC'           >$txt_WaitXBMC</div>";
echo "<div class='wait' id='WaitSystem'         >$txt_WaitSystem</div>";
echo "<div class='wait' id='WaitNetzwerkgeraete'>$txt_WaitNetzwerkgeraete</div>";
echo "<div class='wait' id='WaitPPANews'        >$txt_WaitPPANews</div>";



// ########################## Ausgabe #####################################
// ************************** Kopf **************************************

echo "<div id='kopf1'>";

/*
echo "<div class='ue_logo'><img src=/images/logo.png></div>";
echo "<div class='ue_text'><br/>Easyportal</div>";
echo "<div style='clear:left;'></div>";
*/
echo "<table border=0 width=100%>";
echo "  <tr>";
echo "    <td align=right width=50%><img class='responsive_img' style='max-width: 200px;' src=/images/logo.png></td>";
echo "    <td align='left' valign='center' class='ue1'>EasyPortal</td>";
echo "  </tr>";
echo "</table>";

echo "</div>"; 

// ************************** Hauptmenü **************************************

echo "<nav class='navi1'><ul>";

// *** Home ***
if ($_SERVER['PHP_SELF']=="/index.php")
  { echo "<li><a href='index.php' onclick=\"wait('WaitSeite')\" class='pressed'>$txt_home</a></li>"; }
else 
  { echo "<li><a href='index.php' onclick=\"wait('WaitSeite')\">$txt_home</a></li>"; }

// *** System ***
if ($_SERVER['PHP_SELF']=="/system.php")
  { echo "<li><a href='system.php' onclick=\"wait('WaitSystem')\" class='pressed'>$txt_system</a></li>"; }
else 
  { echo "<li><a href='system.php' onclick=\"wait('WaitSystem')\">$txt_system</a></li>"; }

// *** EasyInfo ***
if (($_SERVER['PHP_SELF']=="/easyinfo.php") or ($_SERVER['PHP_SELF']=="/view_easyinfo.php") 
 or ($_SERVER['PHP_SELF']=="/easyinfo_admin.php") or ($_SERVER['PHP_SELF']=="/easyinfo_dl.php"))
  { echo "<li><a href='easyinfo.php' onclick=\"wait('WaitSeite')\" class='pressed'>$txt_easyinfo</a></li>"; }
else 
  { echo "<li><a href='easyinfo.php' onclick=\"wait('WaitSeite')\">$txt_easyinfo</a></li>"; }

// Einstellungen und Tools nur angemeldet
if(isset($_SESSION["user_id"])) {
  // *** Einstellungen ***
  if (($_SERVER['PHP_SELF']=="/einstellungen.php") or ($_SERVER['PHP_SELF']=="/channel.php") or ($_SERVER['PHP_SELF']=="/plugins.php") 
   or ($_SERVER['PHP_SELF']=="/netzwerk.php") or ($_SERVER['PHP_SELF']=="/fernzugriff.php") or ($_SERVER['PHP_SELF']=="/systemeinstellungen.php") 
   or ($_SERVER['PHP_SELF']=="/sonstige_einstellungen.php") or ($_SERVER['PHP_SELF']=="/channel_upload.php")
   or ($_SERVER['PHP_SELF']=="/view_plugin.php") or ($_SERVER['PHP_SELF']=="/channel_edit.php") or ($_SERVER['PHP_SELF']=="/channel_delete.php")
   or ($_SERVER['PHP_SELF']=="/addons.php") or ($_SERVER['PHP_SELF']=="/channelpedia.php")  
   or ($_SERVER['PHP_SELF']=="/bootlogo.php") or ($_SERVER['PHP_SELF']=="/root_login.php"))
    { echo "<li><a href='einstellungen.php' class='pressed'>$txt_einstellungen</a></li>"; }
  else 
    { echo "<li><a href='einstellungen.php'>$txt_einstellungen</a></li>"; }

  // *** Tools ***
  if (($_SERVER['PHP_SELF']=="/befehle.php") or ($_SERVER['PHP_SELF']=="/update.php") or ($_SERVER['PHP_SELF']=="/backup.php") 
   or ($_SERVER['PHP_SELF']=="/ssh_warnung.php") or ($_SERVER['PHP_SELF']=="/view_update.php")
   or ($_SERVER['PHP_SELF']=="/netzwerk_geraete.php") or ($_SERVER['PHP_SELF']=="/paketquellen.php") or ($_SERVER['PHP_SELF']=="/nachricht.php")
   or ($_SERVER['PHP_SELF']=="/pchanger.php") or ($_SERVER['PHP_SELF']=="/ppa_news.php") )
    { echo "<li><a href='befehle.php' class='pressed'>$txt_tools</a></li>"; }
   else 
    { echo "<li><a href='befehle.php'>$txt_tools</a></li>"; }
}


// *** Timer ***
if (($_SERVER['PHP_SELF']=="/timers.php") or ($_SERVER['PHP_SELF']=="/suchtimer.php") or ($_SERVER['PHP_SELF']=="/suchtimer_view.php")  or ($_SERVER['PHP_SELF']=="/suchtimer_edit.php")
 or ($_SERVER['PHP_SELF']=="/epgsearchdone.php") or ($_SERVER['PHP_SELF']=="/epgsearchdone_view.php") or ($_SERVER['PHP_SELF']=="/epgsearchdone_admin.php")
 or ($_SERVER['PHP_SELF']=="/timer_info.php") or ($_SERVER['PHP_SELF']=="/epgsearch_timersdone.php") or ($_SERVER['PHP_SELF']=="/epgsearch_changrps.php")
 or ($_SERVER['PHP_SELF']=="/epgsearch_cats.php"))
  { echo "<li><a href='timer_info.php' class='pressed'>$txt_timers</a></li>"; }
else 
  { echo "<li><a href='timer_info.php'>$txt_timers</a></li>"; }


// *** Aufnahmen ***
if (($_SERVER['PHP_SELF']=="/aufnahmen.php") or ($_SERVER['PHP_SELF']=="/aufnahme.php") or ($_SERVER['PHP_SELF']=="/aufnahme_rename.php")
  or ($_SERVER['PHP_SELF']=="/aufnahme_edit.php") or ($_SERVER['PHP_SELF']=="/eplist.php") or ($_SERVER['PHP_SELF']=="/eplist_view.php")
  or ($_SERVER['PHP_SELF']=="/serien.php") or ($_SERVER['PHP_SELF']=="/serie.php") or ($_SERVER['PHP_SELF']=="/tatort.php") or ($_SERVER['PHP_SELF']=="/aufnahme_tools.php")
  or ($_SERVER['PHP_SELF']=="/aufnahmen_undelete.php") )
  { echo "<li><a href='aufnahmen.php' class='pressed'>$txt_aufnahmen</a></li>"; }
else 
  { echo "<li><a href='aufnahmen.php'>$txt_aufnahmen</a></li>"; }

  
// *** FB ***
$KODI_pid = exec("sudo $SH_BEFEHL pidof kodi.bin");
if ($KODI_pid == "") 
  { echo "<li><a href='fb.php' target='fb' onclick=\"FensterOeffnenFB(this.href); return false\">FB</a></li>"; } 
else 
  { echo "<li><a href='fb_kodi.php' target='fb' onclick=\"FensterOeffnenFB(this.href); return false\">FB</a></li>"; }

  
// *** Login ***
if (($_SERVER['PHP_SELF']=="/login.php") or ($_SERVER['PHP_SELF']=="/user_einstellungen.php")   )
  { echo "<li><a href='login.php' class='pressed'>$txt_login</a></li>"; }
else 
  { echo "<li><a href='login.php'>$txt_login</a></li>"; }
  
  
// *** ? ***
if (($_SERVER['PHP_SELF']=="/info.php") or ($_SERVER['PHP_SELF']=="/links.php") or ($_SERVER['PHP_SELF']=="/phpinfo.php") 
  or ($_SERVER['PHP_SELF']=="/sidemap.php") or ($_SERVER['PHP_SELF']=="/changelog.php") or ($_SERVER['PHP_SELF']=="/view_datei.php") 
  or ($_SERVER['PHP_SELF']=="/easyportal_log.php")) 
  { echo "<li><a href='info.php' class='pressed'>$txt_fragezeichen</a></li>"; }
else 
  { echo "<li><a href='info.php'>$txt_fragezeichen</a></li>"; }

  
// *** epgd ***  
// echo "<li><a href='epgd.php' class='button'>epgd</a></li>";

echo "</ul></nav>";

?>
