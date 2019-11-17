<?PHP

function linecolor($line, $colorart, $erstezeile)
{
  $line_color = $line;
  # H für Kommentar grün  
  # E für Error rot
  # 1 für 1.Zeile blau
  # I für ini (Wertzuweisung)
  # X für xorg.conf 
  # Y für xorg.log
  # M für XML-Dateien
  # S für 1. Spalte Text
  # D für dpkg
  # L für ls -l


  # # für Kommentar grün  
  if ($colorart!=str_replace("H", "", $colorart)) {
    if (str_replace("#", "", $line)) {
      if($line{0}=="#") { 
        $line_color = "<font color='green'>".$line_color."</font>";
      } 
    } 
  }
  
  # E für Error rot
  if ($colorart!=str_replace("E", "", $colorart)) {
    if (($line!=str_replace("error","",$line)) or ($line!=str_replace("Error","",$line)) or ($line!=str_replace("ERROR","",$line))){ 
      $line_color = "<font color='red'>".$line_color."</font>";
    } 
  }

  # 1 für 1.Zeile blau 
  if ($colorart!=str_replace("1", "", $colorart)) { 
    if ($erstezeile==1){ 
      $line_color = "<font color='blue'>".$line_color."</font>";
    } 
  } 

  # I für ini (Wertzuweisung)
  if ($colorart!=str_replace("I", "", $colorart)) {
    if (($line!=str_replace("=","",$line))){ 
      $line_a = explode("=",$line,2); 
      $line_color = "<font color='#0000FF'>$line_a[0]<b>=</b></font><font color='#993333'>$line_a[1]</font>";
    }
  }

  # X für xorg.conf 
  if ($colorart!=str_replace("X", "", $colorart)) {
    $i=0;
    if (($line!=str_replace("Section","",$line)) and ($line==str_replace("SubSection","",$line))){ $line_color = str_replace("Section", "<font color='#FFFF40'>Section</font><font color='#770066'>", $line)."</font>"; $i=1;}
    if ($line!=str_replace("SubSection","",$line)){ $line_color = str_replace("SubSection", "<font color='#FFFF40'>SubSection</font><font color='#770066'>", $line)."</font>"; $i=1;}
    if ($line!=str_replace("EndSection","",$line)){ $line_color = str_replace("EndSection", "<font color='#FFFF40'>EndSection</font>", $line); $i=1;}
    if ($line!=str_replace("EndSubSection","",$line)){ $line_color = str_replace("EndSubSection", "<font color='#FFFF40'>EndSubSection</font>", $line); $i=1;}
    if (($i==0) and ($line{0}!="#")) {
      $pos = strpos($line, "\""); 
      if ($pos > 0) {
        $line_color = "<font color='blue'>". substr($line, 0, $pos) . "</font><font color='770066'>" . substr($line, $pos-strlen($line)) . "</font>"; 
      }
      else {
        $line_color = "<font color='blue'>". $line . "</font>"; 
      }
    }
  }

  # Y für xorg.log
  if ($colorart!=str_replace("Y", "", $colorart)) {
    $i=0;

    if ($line!=str_replace("(--) probed","",$line)) { $line_color = str_replace("(--) probed", "<font color='#0000FF'>(--) probed</font>", $line); }
    if ($line!=str_replace("(**) from config file","",$line)) { $line_color = str_replace("(**) from config file", "<font color='#003300'>(**) from config file</font>", $line); }
    if ($line!=str_replace("(==) default setting","",$line))  { $line_color = str_replace("(==) default setting", "<font color='yellow'>(==) default setting</font>", $line); }

    if ($line!=str_replace("(++) from command line, (!!) notice, (II) informational,","",$line)) { 
      $line_color = "        (++) from command line, (!!) notice, <font color='green'>(II) informational</font>,<br/>";
      $i=1;
    } 
    if ($line!=str_replace("(WW) warning, (EE) error, (NI) not implemented, (??) unknown.","",$line)) { 
      $line_color = "	 <font color='#FF6600'>(WW) warning</font>, <font color='red'>(EE) error</font>, (NI) not implemented, (??) unknown.<br/>";
      $i=1;
    } 
   
    if ($i==0) {
      if ($line!=str_replace(" (--) ","",$line)) { $line_color = "<font color='#0000FF'>$line_color</font>"; } 
      if ($line!=str_replace(" (**) ","",$line)) { $line_color = "<font color='#003300'>$line_color</font>"; } 
      if ($line!=str_replace(" (==) ","",$line)) { $line_color = "<font color='yellow'>$line_color</font>"; } 
      if ($line!=str_replace(" (II) ","",$line)) { $line_color = "<font color='green'>$line_color</font>"; } 
      if ($line!=str_replace(" (EE) ","",$line)) { $line_color = "<font color='red'>$line_color</font>"; } 
      if ($line!=str_replace(" (WW) ","",$line)) { $line_color = "<font color='#FF6600'>$line_color</font>"; }   
    }	
  }

  # M für XML-Dateien
  if ($colorart!=str_replace("M", "", $colorart)) {
    if (($line!=str_replace("<","",$line_color))){ $line_color = str_replace("<", "&lt;", $line_color); }
    if (($line!=str_replace(">","",$line_color))){ $line_color = str_replace(">", "&gt;", $line_color); }
//  if (($line!=str_replace("&","",$line_color))){ $line_color = str_replace("&", "&amp;", $line_color); }  
//  if (($line!=str_replace("\"","",$line_color))){ $line_color = str_replace("\"", "quot;", $line_color); }
//  $line_color = "<pre><code>$line_color</code></pre>";
      }

  # S für 1. Spalte Text
  if ($colorart!=str_replace("S", "", $colorart)) {
    if($line{0}!=" ") { 
      $line_color = "<font color='blue'>".$line_color."</font>";
    } 
  }

  # D für dpkg
  if ($colorart!=str_replace("D", "", $colorart)) {
    if (($line!=str_replace("Desired=Unknown/Install/Remove/Purge/Hold","",$line))){ 
      $line_color = "<font color='blue'>Desired=</font>Unknown/Install/Remove/Purge/Hold<br/>";
    } 
    if (($line!=str_replace("Status=Not/Inst/Conf-files/Unpacked/halF-conf/Half-inst/trig-aWait/Trig-pend","",$line))){ 
      $line_color = "<font color='blue'>| Status=</font>Not/Inst/Conf-files/Unpacked/halF-conf/Half-inst/trig-aWait/Trig-pend<br/>";
    } 
    if (($line!=str_replace("Err?=(none)/Reinst-required (Status,Err: uppercase=bad)","",$line))){ 
      $line_color = "<font color='blue'>|/ Err?=</font>(none)/Reinst-required (Status,Err: uppercase=bad)<br/>";
    } 
    if (($line!=str_replace("||/ Name","",$line))){ 
      $line_color = "<font color='blue'>$line</font>";
    } 
    if (($line!=str_replace("+++-====","",$line))){ 
      $line_color = "<font color='blue'>$line</font>";
    } 
  }

  # L für ls -l
  if ($colorart!=str_replace("L", "", $colorart)) {
    $lineArray = explode(' ', $line);
    if ($lineArray[1]=="2") {
      $line_color = substr ($line,0,strlen($line)-strlen(end($lineArray))) . "<font color=blue>" . substr ($line,strlen($line)-strlen(end($lineArray)),strlen($line)) . "</font>";   
    }
    if ($lineArray[1]=="1") {
      $line_color = substr ($line,0,strlen($line)-strlen(end($lineArray))) . "<font color=yellow>" . substr ($line,strlen($line)-strlen(end($lineArray)),strlen($line)) . "</font>";   
      if (($line!=str_replace("->","",$line))){
//      $line_color = substr ($line_color,0,strlen($line_color)-strlen(end($lineArray)))."</font><br>";
//      $lineArray2 = explode(' ', $line);
      }
    }
  }

  return $line_color;
}

?>
