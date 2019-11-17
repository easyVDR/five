<?php
$art = $_GET[art];

// Shell-Scripts für diverse OS-Kommandos
$SH_EASYPORTAL     = "/var/www/scripts/easyportal.sh";
$SH_BEFEHL         = "/var/www/scripts/befehl.sh";

//echo "r_index ";
switch ($art) {

  /*************************************** Easyvdr-Version ***************************************/
  case "easyvdr_version":
    $EASYVDR_VERSION = shell_exec("sudo $SH_EASYPORTAL ver");
    echo $EASYVDR_VERSION;
    break;

  /*************************************** easyVDR-Installations-DVD ***************************************/
  case "isover":
    $ISOVER = shell_exec("$SH_EASYPORTAL isover");
    echo $ISOVER;
    break;

  /************************************ media-build-experimental ***************************************/
  case "dkms":
    $dkms_status_mbe = shell_exec("dkms status |grep media-build-experimental");
    if ($dkms_status_mbe != "") {
      //echo $dkms_status_mbe;
      //$dkms_array  = explode(',', $dkms_status_mbe);
      $dkms_array2 = explode(',', $dkms_status_mbe);
      //$mbe_ver     = $dkms_array[1];
      $mbe_install = $dkms_array2[1];
      echo "$mbe_install";
    }
    else {
      echo "nicht installiert";
    }
    break;
    
    
  case "instdvd":
    echo "yyyy";
    
    break;
    
    
  /************************************* Default ***************************************/
  default:
    echo "!! Falscher Parameter !!";
}


?>