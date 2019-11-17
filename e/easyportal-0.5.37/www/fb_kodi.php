<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML Basic 1.1//EN"
    "http://www.w3.org/TR/xhtml-basic/xhtml-basic11.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="de">
<head>
  <title>KODI-Fernbedienung</title>
  <meta http-equiv="content-type" content="text/html; charset=UTF-8"/>
  <link rel="stylesheet" href="/css/fb.css"> 
  <meta name="viewport" content="width=device-width, initial-scale=1.0, minimum-scale=1.0, maximum-scale=1.0, user-scalable=no">
</head>
<body>

<?php
session_start();

$key = $_GET[key];
$pc  = $_GET[pc];
$ws  = $_GET[ws];

$SH_BEFEHL   = "/var/www/scripts/befehl.sh";
$pcscript    = "/usr/share/easyvdr/program-changer/program-changer-easy-portal.sh";
$guisettings = "/home/easyvdr/.kodi/userdata/guisettings.xml";
$advancedsettings = "/home/easyvdr/.kodi/userdata/advancedsettings.xml";


/**************************** Funktionen ***********************************/

function ausgabe_fb() {
  // Ausgabe-Funktion der Fernbedienung
  if ($_SESSION["sprache"] == "") { $sprache = "Deutsch"; } else { $sprache = $_SESSION["sprache"]; }
  $sprachdatei = "lang/".$sprache."/txt_fb_kodi.php";
  include($sprachdatei);

  //Farben der Buttons
  $Steuerkreuz_Farbe = "#6e87bf";
  $Funktion_Farbe    = "#6e87bf";
  $Text_Shadow       = "text-shadow:1px 1px 1px black,1px -1px 1px black,-1px  1px 1px black,-1px -1px 1px black";

  echo "<center>";

  echo "<div class='zeile'>";
  echo "<a href='fb.php?pc=0' class='button b_vdr'>&nbsp;</a>";
  echo "<a href='?key='       class='button'>&nbsp</a>";
  echo "<a href='?pc=1'       class='button' style='$Text_Shadow;'>$txt_kodi</a>";
  echo "</div>";

  echo "<div class='zeile'>";
  echo "<a href='?key=Red'    class='button4' style='background-color:red; color:black;       '>$txt_red</a>";
  echo "<a href='?key=Green'  class='button4' style='background-color:lightgreen; color:black;'>$txt_green</a>";
  echo "<a href='?key=Yellow' class='button4' style='background-color:yellow; color:black;    '>$txt_yellow</a>";
  echo "<a href='?key=Blue'   class='button4' style='background-color:lightblue; color:black; '>$txt_blue</a>";
  echo "</div>";

  echo "<div class='zeile'>";
  echo "<a href='?key=List'   class='button4' style='background-color:$Funktion_Farbe; $Text_Shadow;'>$txt_playlist</a>";
  echo "<a href='?key=Queue'  class='button4' style='background-color:$Funktion_Farbe;'>$txt_queue</a>";
  echo "<a href='?key=Del'    class='button4' style='background-color:$Funktion_Farbe;'>$txt_delete</a>";
  echo "<a href='?key=Screen' class='button4' style='background-color:$Funktion_Farbe; $Text_Shadow;'>$txt_fullscreen</a>";
  echo "</div>";

  echo "<div class='zeile'>";
  echo "<a href='?key=Osd'    class='button4' style='background-color:$Funktion_Farbe;'>$txt_osd</a>";
  echo "<a href='?key=M_down' class='button4' style='background-color:$Funktion_Farbe;'>$txt_moveitemdown</a>";
  echo "<a href='?key=M_up'   class='button4' style='background-color:$Funktion_Farbe;'>$txt_moveitemup</a>";
  echo "<a href='?key=Setup'  class='button4' style='background-color:$Funktion_Farbe;'>$txt_setup</a>";
  echo "</div>";

  echo "<div class='zeile'>";
  echo "<a href='?key=Home' class='button4' style='background-color:$Funktion_Farbe; $Text_Shadow;'>$txt_home</a>";
  echo "<a href='?key=Sub'  class='button4' style='background-color:$Funktion_Farbe;'>$txt_sub</a>";
  echo "<a href='?key=Info' class='button4' style='background-color:$Funktion_Farbe;'>$txt_info</a>";
  echo "<a href='?key=RMT'  class='button4' style='background-color:$Funktion_Farbe; $Text_Shadow;'>$txt_rmt</a>";
  echo "</div>";

  echo "<div class='zeile'>";
  echo "<a href='?key=Mute' class='button' style='$Text_Shadow;'>$txt_mute</a>";
  echo "<a href='?key='     class='button'>&nbsp</a>";
  echo "<a href='?key=Back' class='button' style='$Text_Shadow;'>$txt_back</a>";
  echo "</div>";

  echo "<div class='zeile'>";
  echo "<a href='?key=VolumeUp' class='button'>$txt_vol_up</a>";
  echo "<a href='?key=Up'       class='button' style='background-color:$Steuerkreuz_Farbe; $Text_Shadow;'>$txt_up</a>";
  echo "<a href='?key=Page_up'  class='button'>$txt_page_up</a>";
  echo "</div>";

  echo "<div class='zeile'>";
  echo "<a href='?key=Left'  class='button' style='background-color:$Steuerkreuz_Farbe; $Text_Shadow;'>$txt_left</a>";
  echo "<a href='?key=Ok'    class='button' style='$Text_Shadow;'>$txt_ok</a>";
  echo "<a href='?key=Right' class='button' style='background-color:$Steuerkreuz_Farbe; $Text_Shadow;'>$txt_right</a>";
  echo "</div>";

  echo "<div class='zeile'>";
  echo "<a href='?key=VolumeDown' class='button'>$txt_vol_down</a>";
  echo "<a href='?key=Down'       class='button' style='background-color:$Steuerkreuz_Farbe; $Text_Shadow;'>$txt_down</a>";
  echo "<a href='?key=Page_down'  class='button'>$txt_page_down</a>";
  echo "</div>";

  echo "<div class='zeile'>";
  echo "<a href='?key=Skip_Prev' class='button4'style='$Text_Shadow;'>$txt_skipprevious</a>";
  echo "<a href='?key=Prev'      class='button4'style='$Text_Shadow;'>$txt_previous</a>";
  echo "<a href='?key=Next'      class='button4'style='$Text_Shadow;'>$txt_next</a>";
  echo "<a href='?key=Skip_Next' class='button4'style='$Text_Shadow;'>$txt_skipnext</a>";
  echo "</div>";

  echo "<div class='zeile'>";
  echo "<a href='?key=Play'  class='button'style='$Text_Shadow;'>$txt_play</a>";
  echo "<a href='?key=Pause' class='button'style='$Text_Shadow;'>$txt_pause</a>";
  echo "<a href='?key=Stop'  class='button'style='$Text_Shadow;'>$txt_stop</a>";
  echo "</div>";

  echo "</center>";
}


function fb_befehl($key) {
  // Funktion, um ein Kodi-Befehl abzusetzen
  $guisettings = "/home/easyvdr/.kodi/userdata/guisettings.xml";
  $xml = simplexml_load_file($guisettings);
  $webserverport = $xml->services->webserverport; 

  // unescaped z.B.: http://localhost:8080/jsonrpc?request={"jsonrpc":"2.0", "method":"Input.Left", "id":1}

  $url_a = "http://localhost:".$webserverport."/jsonrpc?request=%7B%22jsonrpc%22%3A%222.0%22%2C";
  $url_b = "%2C%22id%22%3A1%7D";

  $Kodi_Action = "'method':'Input.ExecuteAction','params':";
  $Kodi_Window = "'method':'GUI.ActivateWindow','params':";

  switch ($key) {
   case "Mute":       $kodi_befehl = "$Kodi_Action{'action':'mute'}";          break;
   case "Left":       $kodi_befehl = "$Kodi_Action{'action':'left'}";          break;
   case "Right":      $kodi_befehl = "$Kodi_Action{'action':'right'}";         break;
   case "Up":         $kodi_befehl = "$Kodi_Action{'action':'up'}";            break;
   case "Down":       $kodi_befehl = "$Kodi_Action{'action':'down'}";          break;
   case "Osd":        $kodi_befehl = "$Kodi_Action{'action':'osd'}";           break;
   case "Ok":         $kodi_befehl = "$Kodi_Action{'action':'select'}";        break;
   case "Home":       $kodi_befehl = "$Kodi_Window{'window':'home'}";          break;
   case "Back":       $kodi_befehl = "$Kodi_Action{'action':'back'}";          break;
   case "Info":       $kodi_befehl = "$Kodi_Action{'action':'info'}";          break;
   case "Play":       $kodi_befehl = "$Kodi_Action{'action':'play'}";          break;
   case "Stop":       $kodi_befehl = "$Kodi_Action{'action':'stop'}";          break;
   case "Pause":      $kodi_befehl = "$Kodi_Action{'action':'pause'}";         break;
   case "VolumeUp":   $kodi_befehl = "$Kodi_Action{'action':'volumeup'}";      break;
   case "VolumeDown": $kodi_befehl = "$Kodi_Action{'action':'volumedown'}";    break;
   case "Skip_Next":  $kodi_befehl = "$Kodi_Action{'action':'skipnext'}";      break;
   case "Skip_Prev":  $kodi_befehl = "$Kodi_Action{'action':'skipprevious'}";  break;
   case "Next":       $kodi_befehl = "$Kodi_Action{'action':'fastforward'}";   break;
   case "Prev":       $kodi_befehl = "$Kodi_Action{'action':'rewind'}";        break;
   case "Screen":     $kodi_befehl = "$Kodi_Action{'action':'fullscreen'}";    break;
   case "Red":        $kodi_befehl = "$Kodi_Window{'window':'tvchannels'}";    break;
   case "Green":      $kodi_befehl = "$Kodi_Window{'window':'video'}";         break;
   case "Yellow":     $kodi_befehl = "$Kodi_Window{'window':'music'}";         break;
   case "Blue":       $kodi_befehl = "$Kodi_Window{'window':'pictures'}";      break;
   case "M_down":     $kodi_befehl = "$Kodi_Action{'action':'moveitemdown'}";  break;
   case "M_up":       $kodi_befehl = "$Kodi_Action{'action':'moveitemup'}";    break;
   case "Del":        $kodi_befehl = "$Kodi_Action{'action':'delete'}";        break;
   case "List":       $kodi_befehl = "$Kodi_Window{'window':'musicplaylist'}"; break;
   case "Queue":      $kodi_befehl = "$Kodi_Action{'action':'queue'}";         break;
   case "Page_up":    $kodi_befehl = "$Kodi_Action{'action':'pageup'}";        break;
   case "Page_down":  $kodi_befehl = "$Kodi_Action{'action':'pagedown'}";      break;
   case "Sub":        $kodi_befehl = "$Kodi_Action{'action':'nextsubtitle'}";  break;
   case "Favourites": $kodi_befehl = "$Kodi_Window{'window':'favourites'}";    break;
   case "RMT":        $kodi_befehl = "$Kodi_Action{'action':'contextmenu'}";   break;
   case "Setup":      $kodi_befehl = "$Kodi_Window{'window':'systemsettings'}";break;
  }

  $kodi_befehl = str_replace("'", "%22", $kodi_befehl);
  $kodi_befehl = str_replace(":", "%3A", $kodi_befehl);
  $kodi_befehl = str_replace(",", "%2C", $kodi_befehl);
  $kodi_befehl = str_replace("{", "%7B", $kodi_befehl);
  $kodi_befehl = str_replace("}", "%7D", $kodi_befehl);

  $url = $url_a . $kodi_befehl . $url_b;
  $SH_BEFEHL   = "/var/www/scripts/befehl.sh";
  $befehl = "sudo $SH_BEFEHL curl -L $url";
  // echo $befehl;
  $x = exec("$befehl");
  // echo $x;
}


/*********************************************** Ende Funktionen **************************************************/

$xml = simplexml_load_file($guisettings);
$webserver     = $xml->services->webserver; 
$webserverport = $xml->services->webserverport; 

$xml2 = simplexml_load_file($advancedsettings);
$webserver2     = $xml2->services->webserver; 


if ($ws=="on") {
  exec("sudo $SH_BEFEHL chown vdr:vdr $advancedsettings");
  
  //exec("sudo $SH_BEFEHL stop easyvdr-kodi");
  //sleep(10);
  
  exec("sudo $SH_BEFEHL echo '<advancedsettings>'               > $advancedsettings");
  exec("sudo $SH_BEFEHL echo '  <services>'                    >> $advancedsettings");
  exec("sudo $SH_BEFEHL echo '    <webserver>true</webserver>' >> $advancedsettings");
  exec("sudo $SH_BEFEHL echo '  </services>'                   >> $advancedsettings");
  exec("sudo $SH_BEFEHL echo '</advancedsettings>'             >> $advancedsettings");
  
/*
  $xml = simplexml_load_file($advancedsettings);

  $x1 = exec("sudo $SH_BEFEHL cat $advancedsettings| grep advancedsettings;");
  if ($x1 == "") {
    exec("sudo $SH_BEFEHL echo '<advancedsettings>' >> $advancedsettings");
    exec("sudo $SH_BEFEHL echo '</advancedsettings>' >> $advancedsettings");
  }

  $x2 = exec("sudo $SH_BEFEHL cat $advancedsettings| grep webserver;");
  if ($x2 == "") {
    $bericht = $xml->addChild("services");
    $bericht->addChild("webserver", "true");
  }

  $handle = fopen($advancedsettings, "wb"); 
  fwrite($handle, $xml->asXML());
  fclose($handle);
*/
  //exec("sudo $SH_BEFEHL start easyvdr-kodi");
  exec("sudo $SH_BEFEHL $pcscript 1");
  $webserver = "true";
}


if ($key=="") {
  //echo "Webserver : $webserver<br/>";
  //echo "Webserver2: $webserver2<br/><br/>";
  if (($webserver == "false") AND ($webserver2 != "true")){
    echo "KODI-Webserver ist nicht eingeschalten!<br/><br/>";
    echo "(Beim einschalten wird advancedsettings überschrieben)<br/><br/>";

    echo "<a href='?ws=on' class='button'>einschalten</a>";
    echo "<a href='fb.php' class='button'>vdr-fb</a>";
  }
  else {
    if ($pc==1) { exec("sudo $SH_BEFEHL $pcscript 1"); }   // Kodi starten
    ausgabe_fb();
  }
}
else {
  fb_befehl($key);
  ausgabe_fb();
}

echo "</body>";
echo "</html>";
?>
