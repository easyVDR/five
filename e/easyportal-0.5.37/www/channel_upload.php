<?php
//include('includes/head.php'); 
include('includes/kopf.php'); 
include('includes/function_redirect.php');

$upload_dir = $EASYPORTAL_DOC_ROOT."upload";
if (!file_exists($upload_dir)) { exec("mkdir $upload_dir"); }

echo "<div id='kopf2'><a href='einstellungen.php' class='w'>$txt_einstellungen</a> - $txt_kopf2</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_einstellungen.php'); 
include('includes/subnavi_channel.php'); 

echo "<h2>Kanalliste hochladen</h2>";

// *************************************** Container Begin ****************************************************
container_begin(1, "upload.png", $txt_ueberschrift);

if (isset($_FILES['uploaddatei']['name'])) {
  // Datei wurde durch HTML-Formular hochgeladen
  // und kann nun weiterverarbeitet werden
  if ( $_FILES['uploaddatei']['name'] <> '' ) {
    move_uploaded_file ($_FILES['uploaddatei']['tmp_name'] , 'upload/'. $_FILES['uploaddatei']['name'] );
    echo "<p>Hochladen war erfolgreich: ";
    echo '<a href="upload/'. $_FILES['uploaddatei']['name'] .'">';
    echo 'upload/'. $_FILES['uploaddatei']['name'];
    echo '</a>';
    $src = "/var/www/upload/".$_FILES['uploaddatei']['name'];
    $destfile = "/var/lib/vdr/channels/".$_FILES['uploaddatei']['name'] ;
    $destdir = "/var/lib/vdr/channels/" ;
    exec("sudo $SH_EASYPORTAL mvchannel");
    redirect('/channel.php');
    exit();  
  }
  else {
    echo "<p>Dateiname ist nicht zulässig</p>";
  }
}
echo "<br>";
echo "<form enctype='multipart/form-data' action='channel_upload.php' method='POST'>";
// echo "<input type='hidden' name='MAX_FILE_SIZE' value='100M' />";
echo "<input name='uploaddatei' type='file' />";
echo "<br><br>";
echo "<input type='submit' value='$txt_b_upload' />";
echo "</form>";

container_end();
// *************************************** Container Ende ****************************************************

// *************************************** Container Begin ****************************************************
container_begin(1, "Info.png", $txt_ue_info);
echo "$txt_info1<br/><br/>$txt_info2<br/>";
container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";   // inhalt

include('includes/fuss.php'); 

?>