<?php
include('includes/kopf.php'); 

$url = 'http://'.$_SERVER['SERVER_NAME']."/easyinfo_dl2.php";
echo "<meta http-equiv='refresh' content='0; url=".$url."'>";

$log_dir2 = $EASYPORTAL_DOC_ROOT."log/easyinfo";
if (!file_exists($log_dir2)) { exec("mkdir $log_dir2"); }

exec("sudo $SH_EASYINFO2 $EASYPORTAL_VER");

echo "<div id='kopf2'><a href='easyinfo.php' class='w'>EasyInfo</a> - $txt_kopf2</div>";

echo "<div id='inhalt'>";
echo "<center>";
include('includes/subnavi_easyinfo.php'); 

echo "<h2>$txt_ueberschrift</h2>";

//echo "<table border=0 width='100%'>"; 
//echo "<tr><td align=center valign=top width='50%'>";

// *************************************** Container Begin '' ****************************************************
container_begin(1, "Info.png", $txt_ue_create);

echo "$txt_create1";
echo "<br/><br/>";
echo "<a href='easyinfo.php'><button>$txt_b_weiter</button></a>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";    // Inhalt

include('includes/fuss.php'); 
?>