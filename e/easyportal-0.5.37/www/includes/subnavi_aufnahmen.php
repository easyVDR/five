<?php
$sprachdatei = "lang/".$_SESSION["sprache"]."/m_txt_aufnahmen.php";
include($sprachdatei);

echo "<nav class='navi2'><ul>";

if (($_SERVER['PHP_SELF']=="/aufnahmen.php") or ($_SERVER['PHP_SELF']=="/aufnahme.php")
  or ($_SERVER['PHP_SELF']=="/aufnahme_edit.php") or ($_SERVER['PHP_SELF']=="/aufnahme_rename.php"))
  {echo "<li><a href='aufnahmen.php' class='pressed'>$txt_aufnahmen</a></li>"; }
else 
  {echo "<li><a href='aufnahmen.php'>$txt_aufnahmen</a></li>"; }

if (($_SERVER['PHP_SELF']=="/serien.php") or ($_SERVER['PHP_SELF']=="/serie.php")) 
  {echo "<li><a href='serien.php' class='pressed'>$txt_serien</a></li>"; }
else 
  {echo "<li><a href='serien.php'>$txt_serien</a></li>"; }

if ($_SERVER['PHP_SELF']=="/tatort.php") 
  {echo "<li><a href='tatort.php' class='pressed'>$txt_tatort</a></li>"; }
else 
  {echo "<li><a href='tatort.php'>$txt_tatort</a></li>"; }
  
if (($_SERVER['PHP_SELF']=="/eplist.php") or ($_SERVER['PHP_SELF']=="/eplist_view_episodes.php"))
  {echo "<li><a href='eplist.php' class='pressed'>$txt_eplist</a></li>"; }
else 
  {echo "<li><a href='eplist.php'>$txt_eplist</a></li>"; }

if (($_SERVER['PHP_SELF']=="/aufnahmen_undelete.php") or ($_SERVER['PHP_SELF']=="/eplist_view_episodes.php"))
  {echo "<li><a href='aufnahmen_undelete.php' class='pressed'>$txt_undelete</a></li>"; }
else 
  {echo "<li><a href='aufnahmen_undelete.php'>$txt_undelete</a></li>"; }
  
if ($_SERVER['PHP_SELF']=="/aufnahme_tools.php") 
  {echo "<li><a href='aufnahme_tools.php' class='pressed'>$txt_aufnahme_tools</a></li>"; }
else 
  {echo "<li><a href='aufnahme_tools.php'>$txt_aufnahme_tools</a></li>"; }
  
echo "</ul></nav>";
?>
