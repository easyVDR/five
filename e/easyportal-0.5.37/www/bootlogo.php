<?php
include('includes/kopf.php'); 
include('includes/function_redirect.php');

$logo_dir = "/usr/share/easyvdr/images/logo";

$logo = safepost('logo');

if ($logo!="") {
  exec("sudo $SH_BEFEHL ln -s -f $logo_dir/$logo /usr/share/easyvdr/images/easyvdr-logo.png");
}
else
{
  $logo = trim(exec("ls -l /usr/share/easyvdr/images/easyvdr-logo.png | cut -d/ -f12"));;
  if ($logo==""){
    $logo = "easyvdr-logo_16-9_orginal_2.0.png";
    exec("sudo $SH_BEFEHL ln -s -f $logo_dir/$logo /usr/share/easyvdr/images/easyvdr-logo.png");
  }
}

$upload_dir = $EASYPORTAL_DOC_ROOT."upload";
if (!file_exists($upload_dir)) { exec("mkdir $upload_dir"); }

$destdir = "/usr/share/easyvdr/images/logo/" ;
if (!file_exists($destdir)) { exec("mkdir $destdir"); }


echo "<div id='kopf2'>$txt_kopf2</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_einstellungen.php'); 

echo "<h2>$txt_ueberschrift</h2>";

// *************************************** Container Begin 'Bootlogo auswählen' ****************************************************
container_begin(1, "bilder.png", $txt_ue_logo_waehlen);

// Logos aus '/usr/share/easyvdr/images/logo'

echo "<div class='links'>";

$logo_dir_array = scandir($logo_dir);
echo "<form action=".$_SERVER['PHP_SELF']." method='post'>\n";
echo "<table class='tab1'>";
echo "<tr><th>$txt_tab_logo</th><th>$txt_tab_wahl</th>";
foreach ($logo_dir_array as $file) { // Ausgabeschleife
  if ($file != "." && $file != "..")  {
    if ($file == $logo) {$checked="checked";} else {$checked="";}
    $file_ansicht = str_replace("_", " ", $file);
    $file_ansicht = substr($file_ansicht, 0, strlen($file_ansicht)-4);
    
    echo "<tr>";
    echo "<td>$file_ansicht</td>";
    echo "<td align=center><input type='radio' name='logo' value='$file' $checked onChange='this.form.submit()'></td>";
    echo "</tr>"; 
  }
}
echo "</table>";
echo "</form>";

echo "</div>";  // class='links'
echo "<div class='rechts'>";

echo "<br/>&nbsp;&nbsp;<b>$txt_tab_vorschau:</b><br/>";
echo "&nbsp;&nbsp;<img src='bild.php?url=$logo_dir/$logo' class='responsive_img' style='max-width:400px; '>";

echo "</div>";  // class='rechts'
echo "<div class='clear'></div>"; // Ende 2-spaltig

container_end();
// *************************************** Container Ende ****************************************************

// *************************************** Container Begin 'Bootlogo Upload' ****************************************************
container_begin(1, "upload.png", $txt_ue_logo_upload);

echo "$txt_upload1<br/>";
echo "$txt_upload2<br/>";

if (isset($_FILES['uploaddatei']['name'])) {
  // Datei wurde durch HTML-Formular hochgeladen und kann nun weiterverarbeitet werden
  if ( $_FILES['uploaddatei']['name'] <> '') {
    move_uploaded_file ($_FILES['uploaddatei']['tmp_name'] , 'upload/'. $_FILES['uploaddatei']['name'] );
    echo "<p>Hochladen war erfolgreich: ";
    echo '<a href="upload/'. $_FILES['uploaddatei']['name'] .'">';
    echo 'upload/'. $_FILES['uploaddatei']['name'];
    echo '</a>';
    $src = "/var/www/upload/".$_FILES['uploaddatei']['name'];
    exec("sudo $SH_BEFEHL mv $src $destdir");
    redirect('/bootlogo.php');
    exit();  
  }
  else {
    echo "<p>Dateiname ist nicht zulässig</p>";
  }
}
echo "<br>";
echo "<form enctype='multipart/form-data' action='bootlogo.php' method='POST'>";
echo "<input name='uploaddatei' type='file' />";
echo "<br><br>";
echo "<input type='submit' value='$txt_b_upload' />";
echo "</form>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
