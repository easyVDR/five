<?php
include('includes/kopf.php'); 

$quelle = safeget('quelle');
if ($quelle=="") {$quelle="DVB-S";}
$DL  = safeget('DL'); 
$art = safeget('art'); 
 
echo "<div id='kopf2'>";
echo "<a href='einstellungen.php' class='w'>$txt_einstellungen</a> - $txt_kopf2";
echo "</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_einstellungen.php'); 
include('includes/subnavi_channel.php'); 

echo "<h2>$txt_ueberschrift</h2>";

// *************************************** Container Begin ****************************************************
container_begin(1, "Channels.png", $txt_ue_channelpedia);

if ($DL){
  if ($art == "transponder") {$art_txt = "_complete.channels.conf";}
  if ($art == "group")       {$art_txt = "_complete_sorted_by_groups.channels.conf";}
  $cl_channelpedia = "http://channelpedia.yavdr.com/gen/".$DL.$art_txt;
  //echo "Channel_Channelpedia: $cl_channelpedia<br/>";
  
  $t= Str_replace("]", "", Str_replace("[", "", substr($DL,strrpos($DL,"/")+2,strlen($DL)))); 
  $cl_easyvdr = "/var/lib/vdr/channels/".$quelle."_".$t.$art_txt;
  //echo "Channel_EasyVDR: $cl_easyvdr<br/><br/>";
  
  $datei_cp   = fopen($cl_channelpedia,"r");
  $datei_easy = fopen($cl_easyvdr,"a");
  
  while ( $inhalt = fgets($datei_cp)) { fwrite($datei_easy, $inhalt); }
  fclose($datei_cp);
  fclose($datei_easy);

  echo "<b>Kanalliste '".$quelle."_".$t.$art_txt."' wurde runtergeladen und kann nun <a class='b' href='channel.php'>ausgewählt</a> werden!</b><br/><br/>";
}

echo "<form action=".$_SERVER['PHP_SELF']." method='get'>\n";

if ($quelle=="DVB-S") {$checked_s="checked";} else {$checked_s="";}
if ($quelle=="DVB-C") {$checked_c="checked";} else {$checked_c="";}
if ($quelle=="DVB-T") {$checked_t="checked";} else {$checked_t="";}

echo "<table border=0><tr>";
echo   "<td>$txt_empfangsart:</td><td>";
echo   " <nobr><input type='radio' name='quelle' value='DVB-S' $checked_s onChange='this.form.submit()'> DVB-S</nobr>&nbsp;";
echo   " <nobr><input type='radio' name='quelle' value='DVB-C' $checked_c onChange='this.form.submit()'> DVB-C</nobr>&nbsp;";
echo   " <nobr><input type='radio' name='quelle' value='DVB-T' $checked_t onChange='this.form.submit()'> DVB-T</nobr>&nbsp; </td>";
echo "</tr>";
echo "</table>";
echo "<br/>";

echo "<img src='images/dvb/$quelle.png'><br/><br/>";

echo "<table class='tab1'>";
echo "<tr>";
echo "  <th>$txt_table_channellist</th>";
echo "  <th align='center'>$txt_table_download</th>";
echo "</tr>";


$resultSet = $db->executeQuery("SELECT * FROM channelpedia WHERE Quelle='$quelle'");
while($resultSet->next()) {
  $Name = $resultSet->getCurrentValueByName("Name");
  $Url  = $resultSet->getCurrentValueByName("Url");

  echo "<tr><td>";
  $ausgabe = substr($Name, 7, strlen($Name));
  if (substr($ausgabe,0,3)=="at_") { echo "<img src='images/flaggen/at.png'> ";}
  if (substr($ausgabe,0,3)=="de_") { echo "<img src='images/flaggen/de.png'> ";}
  if (substr($ausgabe,0,3)=="dk_") { echo "<img src='images/flaggen/dk.png'> ";}
  if (substr($ausgabe,0,3)=="fi_") { echo "<img src='images/flaggen/fi.png'> ";}
  if (substr($ausgabe,0,3)=="ie_") { echo "<img src='images/flaggen/ie.png'> ";}
  if (substr($ausgabe,0,3)=="nl_") { echo "<img src='images/flaggen/nl.png'> ";}  
  $ausgabe = str_replace("_", " ", $ausgabe);
  
  echo "$ausgabe";
  echo "</td>";
  echo "<td>";
  echo "<a class='b' href='".$_SERVER['PHP_SELF']."?DL=$Url&quelle=$quelle&art=group' onclick=\"javascript:this.form.submit()\"><nobr>order-by-Group</nobr></a>";
  echo "&nbsp;&nbsp;<a class='b' href='".$_SERVER['PHP_SELF']."?DL=$Url&quelle=$quelle&art=transponder' onclick=\"javascript:this.form.submit()\"><nobr>order-by-Transponder</nobr></a>";
  echo "</td>";
  echo "</tr>";
}
echo "</table>";
echo "</form>";

container_end();
// *************************************** Container Ende ****************************************************

// *************************************** Container Begin ****************************************************
container_begin(1, "Info.png", $txt_ue_info);

echo $txt_info1."<br/>";
echo $txt_info2."<br/>";
echo $txt_info3."<br/>";
echo $txt_info4;

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
