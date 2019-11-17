<?php
include('includes/kopf.php'); 

$msg = safepost('msg');
  
echo "<div id='kopf2'>$txt_kopf2</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_tools.php'); 

echo "<h2>$txt_ueberschrift</h2>";

// *************************************** Container Begin '' ****************************************************
container_begin(1, "Info.png", $txt_ue_send_nachricht);

if ($msg != "") {
  exec("svdrpsend MESG '$msg'");
  echo "Nachricht '$msg' wurde ans OSD gesendet !!!<br/><br/>";
}

echo "$txt_nachricht_senden<br/>";

echo "<br/>";

echo "<form action='".$_SERVER['PHP_SELF']."' method='post'>";
echo "<input type='text' size='50' maxlength='50' name='msg' value='$msg' id='msg'>";

echo "<br/><br/>";

echo "<input type='submit' name='submit' value='$txt_b_senden'>";
echo "</form>";


container_end();
// *************************************** Container Ende ****************************************************

// *************************************** Container Begin '' ****************************************************
container_begin(1, "Info.png", $txt_ue_mlist);

echo "$txt_mlist1&nbsp;&nbsp;";

exec("svdrpsend PLUG mlist LSTM", $messages);
if (in_array("550 Plugin \"mlist\" not found (use PLUG for a list of plugins)", $messages)) {
  echo "<img onmouseover=\"Tip('Plugin nicht aktiviert<br/>bitte aktivieren')\" src='/images/icon/ok_no.png'>";
}
else {
  echo "<img onmouseover=\"Tip('Plugin aktiviert')\" src='/images/icon/ok.png'>";
}

echo "<br/><br/>";

foreach ($messages as $datensatz) 
{
   //echo "$datensatz<br/>";
   if ($datensatz[0] == 9) {echo substr($datensatz, 4, strlen($datensatz)-4)."<br/>";}
}

 container_end();
// *************************************** Container Ende ****************************************************


echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
