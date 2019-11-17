<?php
$sprachdatei = "lang/".$_SESSION["sprache"]."/m_txt_easyinfo.php";
include($sprachdatei);

echo "<nav class='navi2'><ul>";

if ($_SERVER['PHP_SELF']=="/easyinfo.php")
  {echo "<li><a href='easyinfo.php' class='pressed'>$txt_auswahl</a></li>"; }
else 
  {echo "<li><a href='easyinfo.php'>$txt_auswahl</a></li>"; }

if ($_SERVER['PHP_SELF']=="/easyinfo_dl.php")
  {echo "<li><a href='easyinfo_dl.php' class='pressed' onclick=\"wait('WaitEasyInfo')\">$txt_akt_easyinfo_download</a></li>"; }
else 
  {echo "<li><a href='easyinfo_dl.php' onclick=\"wait('WaitEasyInfo')\">$txt_akt_easyinfo_download</a></li>"; }

if ($_SERVER['PHP_SELF']=="/easyinfo_admin.php")
  {echo "<li><a href='easyinfo_admin.php' class='pressed'>$txt_admin</a></li>"; }
else 
  {echo "<li><a href='easyinfo_admin.php' >$txt_admin</a></li>"; }

echo "</ul></nav>";
?>
