<?php
//include('includes/kopf.php'); 

function makeDownload($file, $dir, $type) {  
    header("Content-Type: $type");
    header("Content-Disposition: attachment; filename=\"$file\"");
    readfile($dir.$file);
} 

$type = "application/zip";
//$type = "application/force-download";
makeDownload("easyinfo.zip", "/var/www/log/easyinfo/", $type);

include('includes/fuss.php'); 
?>