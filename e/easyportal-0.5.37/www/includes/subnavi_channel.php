<?php
$sprachdatei = "lang/".$_SESSION["sprache"]."/m_txt_channel.php";
include($sprachdatei);

echo "<br/>";
echo "<nav class='navi3'><ul>";

if ($_SERVER['PHP_SELF']=="/channel.php")
  {echo "<li><a href='channel.php' class='pressed'>$txt_channel_waehlen</a></li>"; }
else 
  {echo "<li><a href='channel.php'>$txt_channel_waehlen</a></li>"; }

if ($_SERVER['PHP_SELF']=="/channel_upload.php")
  {echo "<li><a href='channel_upload.php' class='pressed'>$txt_channel_upload</a></li>"; }
else 
  {echo "<li><a href='channel_upload.php'>$txt_channel_upload</a></li>"; }

echo "<script type='text/javascript'>selFensterSize('<br/><br/>', 550, '');</script>";
  
if ($_SERVER['PHP_SELF']=="/channelpedia.php")
  {echo "<li><a href='channelpedia.php' class='pressed'>$txt_channelpedia</a></li>"; }
else 
  {echo "<li><a href='channelpedia.php'>$txt_channelpedia</a></li>"; }

if ($_SERVER['PHP_SELF']=="/channel_delete.php")
  {echo "<li><a href='channel_delete.php' class='pressed'>$txt_channel_delete</a></li>"; }
else 
  {echo "<li><a href='channel_delete.php'>$txt_channel_delete</a></li>"; }

echo "</ul></nav>";
  
?>
