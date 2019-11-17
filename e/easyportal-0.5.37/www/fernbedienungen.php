<?php
include('includes/kopf.php'); 

$txt_kopf2              = "Fernbedienungen";
$txt_ueberschrift       = "Fernbedienungen";
$txt_ue_fernbedienungen = "Fernbedienungen";
$txt_tabelle_fb         = "Fernbedienung";
$txt_tabelle_bilder     = "Bilder";
$txt_tabelle_help       = "Hilfe";

echo "<div id='kopf2'>$txt_kopf2</div>";

echo "<div id='inhalt'>";
echo "<center>";

// include('includes/subnavi_einstellungen.php'); 

// jetzt alle Dateien und Ordner in ein Array packen und sortieren
$fb_dir = "/var/lib/vdr/remotes/";
$fb_array = scandir($fb_dir);
sort($fb_array);

echo "<h2>$txt_ueberschrift</h2>";

// *************************************** Container Begin '' ****************************************************
container_begin(2, "RemoteControl.png", $txt_ue_fernbedienungen);

echo "<table class='tab1'>";
echo "<tr>";
echo "  <th>$txt_tabelle_fb</th> <th>$txt_tabelle_bilder</th> <th>$txt_tabelle_help</th>";
echo "</tr>";
// jetzt alle Array Inhalte durchgehen
foreach($fb_array as $fb){
  // schauen ob die Zeichen "." oder ".." enthalten ist
  if($fb != "." && $fb != "..") {
    //jetzt schauen, obs ein Verzeichniss ist
    if (is_dir($fb_dir.$fb)) {
      echo "</tr>";
      echo "<td>".str_replace("_", " ", $fb)."&nbsp;</td>";  // Fernbedienungsname
      echo "<td height='90px' align=center>&nbsp;";          // Bilder anzeigen
      $fb2_array = scandir($fb_dir.$fb."/");
      foreach($fb2_array as $fb2){
        if($fb2 != "." && $fb2 != "..") {
        //echo $fb_dir.$fb."/".$fb2."<br/>";
          $Bilddaten = @getimagesize($fb_dir.$fb."/".$fb2);  // !!! mit dem @ werden Fehlermeldungen unterdrückt
          if($Bilddaten[2] == 1 || $Bilddaten[2] == 2 || $Bilddaten[2] == 3) {
            echo "<a href='#' onclick=\"window.open('bild.php?url=/$fb_dir/$fb/$fb2', '', 'scrollbars=0, width=180, height=500');\"><img src='bild.php?url=/$fb_dir/$fb/$fb2' height='90px'></a>&nbsp;";
            // style=\"-moz-transform: rotate(90deg); -o-transform: rotate(90deg); -webkit-transform: rotate(90deg); transform: rotate(90deg); filter: progid:DXImageTransform.Microsoft.BasicImage(rotation=1)\" 
          }
        }
      }
      echo "</td>";
      echo "<td>";                                          // Hilfe
        $desc = shell_exec("cat $fb_dir$fb/remote_controller.help");
        $desc2 = str_replace("\n", "<br/>", $desc);
        //echo "<pre>$desc</pre>";
        echo $desc2;
      echo "</td>";
      echo "</tr>";
    }
  }
}
echo "</table>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
