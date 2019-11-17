<?php
include('includes/kopf.php'); 

echo "<div id='kopf2'>$txt_aufnahmen - $txt_kopf2</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_aufnahmen.php'); 

echo "<h2>$txt_ueberschrift</h2>";

// *************************************** Container Begin ****************************************************
container_begin(1, "film.png", "$txt_ue_rename: ".safeget('rec'));

if (safeget('rec')) {
  $rec = "/video0/".safeget('rec');
  $rec_alt = substr($rec, 8, strlen($rec)-8);
  $rec_alt = substr($rec_alt,0,strrpos($rec_alt,"/")  ); 
  //$rec_alt = str_replace("'", "\'", $rec_alt);

  $dir_encode = urlencode(safeget('rec'));
  
  echo "<form action='".$_SERVER['PHP_SELF']."' method='post'>";
  echo "<input type='hidden' name='rec_alt' value=\"$rec_alt\">";
  echo "<input type='hidden' name='rec' value=\"$rec\">";
  echo "<table>";
  echo "<tr><td>Alter Name:</td><td> $rec_alt </td></tr>";
  echo "<tr><td>Neuer Name:</td><td><input type='text' name='rec_neu' size='100' value=\"$rec_alt\">  </td></tr>";
  echo "</table>";
  echo "<br/><br/>";
  
  echo "<a href='aufnahmen.php'><button type='button'>Aufnahmenliste</button></a>";
  echo "<a href='aufnahme.php?info=$dir_encode'><button type='button'>Aufnahme</button></a>";
  echo "<input type='submit' name='submit' value='$txt_b_save'>";
  echo "</form>";
}

function maskieren($Str) 
{
  $Str=str_replace('(','\(',$Str);
  $Str=str_replace(')','\)',$Str);
  $Str=str_replace('&','\&',$Str);
  $Str=str_replace("'","\'",$Str);
  return $Str;
}

if (safepost('rec_alt')) {
  $rec_dir_alt = safepost('rec');
  $txt_debug_meldungen[] = "Rec_dir_alt: $rec_dir_alt";
  $rec_alt = safepost('rec_alt');
  $rec_neu = safepost('rec_neu');
  $rec_neu = str_replace(" ", "_", $rec_neu);
  $verz = substr($rec_dir_alt, strrpos($rec_dir_alt,"/") + 1, strlen($rec_dir_alt) ); 
  $rec_dir_neu = "/video0/$rec_neu/$verz";
  $txt_debug_meldungen[] = "Rec_dir_neu: $rec_dir_neu"; 
  //$txt_debug_meldungen[] = "mkdir: /video0/$rec_neu"; 

  $befehl = "mkdir -p /video0/".maskieren($rec_neu);
  $txt_debug_meldungen[] = "Befehl: $befehl"; 
  exec($befehl);

  $befehl = "mv ".maskieren($rec_dir_alt)." ".maskieren($rec_dir_neu)."";
  $txt_debug_meldungen[] = "Befehl: $befehl"; 
  exec($befehl);
  
  echo "Aufnahme wurde umbenannt:<br/><br/>";
  
  echo "<table>";
  echo "<tr><td>Alter Name:</td><td> $rec_alt </td></tr>";
  echo "<tr><td>Neuer Name:</td><td> $rec_neu </td></tr>";
  echo "</table>";

  echo "<br/><br/>";
  echo "<a href='aufnahmen.php?action=recsuche'><button type='button'>Aufnahmenliste</button></a>";  
  //echo "<a href='aufnahme.php?info=$rec_neu/$verz'><button type='button'>Aufnahme</button></a>";
}

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
