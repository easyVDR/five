<?php
$sprachdatei = "lang/".$_SESSION["sprache"]."/m_txt_user.php";
include($sprachdatei);

echo "<nav class='navi2'><ul>";

if ($_SERVER['PHP_SELF']=="/login.php")
  {echo "<li><a href='login.php' class='pressed'>$txt_login</a></li>"; }
else 
  {echo "<li><a href='login.php'>$txt_login</a></li>"; }

if ($_SERVER['PHP_SELF']=="/user_einstellungen.php")
  {echo "<li><a href='user_einstellungen.php' class='pressed'>$txt_user</a></li>"; }
else 
  {echo "<li><a href='user_einstellungen.php'>$txt_user</a></li>"; }

echo "</ul></nav>";
?>
