<?php
$sprachdatei = "lang/".$_SESSION["sprache"]."/m_txt_epgsearch.php";
include($sprachdatei);

echo "<nav class='navi2'><ul>";

if ($_SERVER['PHP_SELF']=="/epgsearchdone_admin.php") 
  {echo "<li><a href='epgsearchdone_admin.php' class='pressed'>$txt_epgsearchdone_admin</a></li>"; }
else 
  {echo "<li><a href='epgsearchdone_admin.php'>$txt_epgsearchdone_admin</a></li>"; }

if (($_SERVER['PHP_SELF']=="/suchtimer.php") or ($_SERVER['PHP_SELF']=="/suchtimer_view.php") or ($_SERVER['PHP_SELF']=="/suchtimer_edit.php"))
  {echo "<li><a href='suchtimer.php' class='pressed'>$txt_suchtimer</a></li>"; }
else 
  {echo "<li><a href='suchtimer.php'>$txt_suchtimer</a></li>"; }

if ($_SERVER['PHP_SELF']=="/epgsearch_timersdone.php") 
  {echo "<li><a href='epgsearch_timersdone.php' class='pressed'>$txt_epgsearch_timersdone</a></li>"; }
else 
  {echo "<li><a href='epgsearch_timersdone.php'>$txt_epgsearch_timersdone</a></li>"; }
  
if (($_SERVER['PHP_SELF']=="/epgsearchdone.php") or ($_SERVER['PHP_SELF']=="/epgsearchdone_view.php"))
  {echo "<li><a href='epgsearchdone.php' class='pressed'>$txt_epgsearchdone</a></li>"; }
else 
  {echo "<li><a href='epgsearchdone.php'>$txt_epgsearchdone</a></li>"; }
  
if ($_SERVER['PHP_SELF']=="/epgsearch_changrps.php") 
  {echo "<li><a href='epgsearch_changrps.php' class='pressed'>$txt_epgsearch_changrps</a></li>"; }
else 
  {echo "<li><a href='epgsearch_changrps.php'>$txt_epgsearch_changrps</a></li>"; }

if ($_SERVER['PHP_SELF']=="/epgsearch_cats.php") 
  {echo "<li><a href='epgsearch_cats.php' class='pressed'>$txt_epgsearch_cats</a></li>"; }
else 
  {echo "<li><a href='epgsearch_cats.php'>$txt_epgsearch_cats</a></li>"; }

  
echo "</ul></nav>";
?>
