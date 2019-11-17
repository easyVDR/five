<?php
$sprachdatei = "lang/".$_SESSION["sprache"]."/m_txt_hilfe.php";
include($sprachdatei);

echo "<nav class='navi2'><ul>";

if ($_SERVER['PHP_SELF']=="/info.php")
  {echo "<li><a href='info.php' class='pressed'>$txt_info</a></li>"; }
else 
  {echo "<li><a href='info.php'>$txt_info</a></li>"; }

if ($_SERVER['PHP_SELF']=="/links.php")
  {echo "<li><a href='links.php' class='pressed'>$txt_links</a></li>"; }
else 
  {echo "<li><a href='links.php'>$txt_links</a></li>"; }

if ($_SERVER['PHP_SELF']=="/phpinfo.php")
  {echo "<li><a href='phpinfo.php' class='pressed'>$txt_phpinfo</a></li>"; }
else 
  {echo "<li><a href='phpinfo.php'>$txt_phpinfo</a></li>"; }

if (($_SERVER['PHP_SELF']=="/sidemap.php") or ($_SERVER['PHP_SELF']=="/view_datei.php"))
  {echo "<li><a href='sidemap.php' class='pressed'>$txt_sidemap</a></li>"; }
else 
  {echo "<li><a href='sidemap.php'>$txt_sidemap</a></li>"; }

  if ($_SERVER['PHP_SELF']=="/statistik.php")
  {echo "<li><a href='statistik.php' class='pressed'>$txt_statistik</a></li>"; }
else 
  {echo "<li><a href='statistik.php'>$txt_statistik</a></li>"; }
  
if ($_SERVER['PHP_SELF']=="/easyportal_log.php")
  {echo "<li><a href='easyportal_log.php' class='pressed'>$txt_easyportallog</a></li>"; }
else 
  {echo "<li><a href='easyportal_log.php'>$txt_easyportallog</a></li>"; }

if ($_SERVER['PHP_SELF']=="/changelog.php")
  {echo "<li><a href='changelog.php' class='pressed'>$txt_changelog</a></li>"; }
else 
  {echo "<li><a href='changelog.php'>$txt_changelog</a></li>"; }

echo "</ul></nav>";
?>
