<?php
$UP       = explode(",",exec("$SH_EASYPORTAL uptime"));
$vdrstart = exec("$SH_EASYPORTAL vdruptime");
if ($vdrstart!="") {
  $age1 = strtotime("now");
  $age2 = $vdrstart;
  $age3 = $age1 - $age2;
  $TAGE = floor($age3 / 86400);
  $age3 = $age3 - $TAGE * 86400;
  $STUNDEN = floor($age3 / 3600);
  $age3    = $age3 - $STUNDEN * 3600;
  $MINUTEN = floor($age3 / 60);
}

$vdrpid = exec("sudo $SH_BEFEHL pidof vdr");
?>

<body onload="countup1(); countup2();">

<style type="text/css">
#ZeitBox01 {
  position: relative;
  top: 0px;
  left: 0px;
  width: 250px;
  height: 20px;
  margin: 0px;
  padding: 0px;
  border: none;
}

#ZeitAnzeige {
  position: absolute;
  top: 0px;
  left: 0px;
  background: transparent;
  width: 250px;
  line-height: 20px;
  text-align: left;
  color: black;
  font-family: Verdana,Arial,Helvetica,sans-serif;
  font-size: 12px;
  font-weight: normal;
}
</style>

<script type="text/javascript" language="JavaScript">
<!-- Begin

// Array Wochentag
Wochentag = new Array("Sonntag","Montag","Dienstag","Mittwoch","Donnerstag","Freitag","Samstag");


// Funktionen für Anzeigen Tag Datum + dynamische Uhrzeit
function DisplayTime()
{ 
  var SystemDatum   = new Date();
  var CounterTag    = SystemDatum.getDate();
  var CounterMonat  = SystemDatum.getMonth() + 1;
  var CounterJahr   = SystemDatum.getFullYear();
  var CounterStd    = SystemDatum.getHours();
  var CounterMin    = SystemDatum.getMinutes();
  var CounterSek    = SystemDatum.getSeconds();
  var TagDerWoche   = SystemDatum.getDay();

  //  für zweistellige Anzeige
  var CounterTag2   = ((CounterTag < 10) ? "0" : "");
  var CounterMonat2 = ((CounterMonat < 10) ? ".0" : ".");
  var CounterStd2   = ((CounterStd < 10) ? "0" : "");
  var CounterMin2   = ((CounterMin < 10) ? ":0" : ":");
  var CounterSek2   = ((CounterSek < 10) ? ":0" : ":");

  // Die 3 Fragmente für die Anzeige Wochentag Datum Zeit
  // aktuelles Datum
  var DatumJetzt = CounterTag2 + CounterTag + CounterMonat2 + CounterMonat  + "." + CounterJahr;
  // aktuelle Zeit
  var ZeitJetzt = CounterStd2 + CounterStd + CounterMin2 + CounterMin + CounterSek2 + CounterSek + " Uhr";
  // Anzeige Wochentag + Datum + Zeit
  var DispString = Wochentag[TagDerWoche] + " " + DatumJetzt + " &nbsp;" + ZeitJetzt;

  document.getElementById("ZeitAnzeige").innerHTML = DispString;
  setTimeout("DisplayTime()", 1000);
}

window.setTimeout('DisplayTime()',1000);



// ***************  Up-Time *******************
var SystemDatum = new Date();
var tag         = SystemDatum.getDate() - <?php echo $UP[0]; ?>;
var monat       = SystemDatum.getMonth() + 1;
var jahr        = SystemDatum.getFullYear();
var stunde      = SystemDatum.getHours() - <?php echo $UP[1]; ?>;
var minute      = SystemDatum.getMinutes() - <?php echo $UP[2]; ?>;
var sekunde     = 0;

function countup1() {
  var startDatum=new Date(jahr,monat-1,tag,stunde,minute,sekunde);
  zielDatum=new Date(); // Aktuelles Datum

  if(startDatum<zielDatum)  {     // Countup erst berechnen und anzeigen, wenn Start-Datum überschritten wird
    var jahre=0, monate=0, tage=0, stunden=0, minuten=0, sekunden=0;

    // Jahre
    while(startDatum<zielDatum) {
      jahre++;
      startDatum.setFullYear(startDatum.getFullYear()+1);
    }
    startDatum.setFullYear(startDatum.getFullYear()-1);
    jahre--;

    // Monate
    while(startDatum<zielDatum) {
      monate++;
      startDatum.setMonth(startDatum.getMonth()+1);
    }
    startDatum.setMonth(startDatum.getMonth()-1);
    monate--;

    // Tage
    while(startDatum.getTime()+(24*60*60*1000)<zielDatum) {
      tage++;
      startDatum.setTime(startDatum.getTime()+(24*60*60*1000));
    }

    // Stunden
    stunden=Math.floor((zielDatum-startDatum)/(60*60*1000));
    startDatum.setTime(startDatum.getTime()+stunden*60*60*1000);

    // Minuten
    minuten=Math.floor((zielDatum-startDatum)/(60*1000));
    startDatum.setTime(startDatum.getTime()+minuten*60*1000);

    // Sekunden
    sekunden=Math.floor((zielDatum-startDatum)/1000);

    // Anzeige formatieren
    if (jahre==0) jahre="";
    if (jahre==1) jahre=jahre+" Jahr,  ";
    if (jahre>1) jahre=jahre+" Jahre,  ";

    if (monate==0) monate="";
    if (monate==1) monate=monate+" Monat,  ";
    if (monate>1) monate=monate+" Monate,  ";

    if (tage==0) tage="";
    if (tage==1) tage=tage+" Tag,  ";
    if (tage>1) tage=tage+" Tage,  ";

    if (stunden==0) stunden="";
    if (stunden==1) stunden=stunden+" Stunde,  ";
    if (stunden>1) stunden=stunden+" Stunden,  ";

    (minuten!=1)?minuten=minuten+" Minuten":minuten=minuten+" Minute";

    document.getElementById("Uptime").innerHTML = jahre+monate+tage+stunden+minuten;
  }
  // Anderenfalls alles auf Null setzen
  else document.getElementById("Uptime").innerHTML = "0 Jahre,  0 Monate,  0 Tage,  0 Stunden,  0 Minuten";

  setTimeout('countup1()',1000);
}


// ***************  VDR-Up-Time *******************
var SystemDatum = new Date();
var vdrup_tag         = SystemDatum.getDate() - <?php echo $TAGE; ?>;
var vdrup_monat       = SystemDatum.getMonth() + 1;
var vdrup_jahr        = SystemDatum.getFullYear();
var vdrup_stunde      = SystemDatum.getHours() - <?php echo $STUNDEN; ?>;
var vdrup_minute      = SystemDatum.getMinutes() - <?php echo $MINUTEN; ?>;
var vdrup_sekunde     = 0;

function countup2() {
  var startDatum=new Date(vdrup_jahr,vdrup_monat-1,vdrup_tag,vdrup_stunde,vdrup_minute,vdrup_sekunde);
  zielDatum=new Date(); // Aktuelles Datum

  if("<?php echo $vdrpid; ?>"!=="") {  // wenn vdr läuft
    var jahre=0, monate=0, tage=0, stunden=0, minuten=0, sekunden=0;

    // Jahre
    while(startDatum<zielDatum) {
      jahre++;
      startDatum.setFullYear(startDatum.getFullYear()+1);
    }
    startDatum.setFullYear(startDatum.getFullYear()-1);
    jahre--;

    // Monate
    while(startDatum<zielDatum) {
      monate++;
      startDatum.setMonth(startDatum.getMonth()+1);
    }
    startDatum.setMonth(startDatum.getMonth()-1);
    monate--;

    // Tage
    while(startDatum.getTime()+(24*60*60*1000)<zielDatum) {
      tage++;
      startDatum.setTime(startDatum.getTime()+(24*60*60*1000));
    }

    // Stunden
    stunden=Math.floor((zielDatum-startDatum)/(60*60*1000));
    startDatum.setTime(startDatum.getTime()+stunden*60*60*1000);

    // Minuten
    minuten=Math.floor((zielDatum-startDatum)/(60*1000));
    startDatum.setTime(startDatum.getTime()+minuten*60*1000);

    // Sekunden
    sekunden=Math.floor((zielDatum-startDatum)/1000);

    // Anzeige formatieren
    if (jahre==0) jahre="";
    if (jahre==1) jahre=jahre+" Jahr,  ";
    if (jahre>1) jahre=jahre+" Jahre,  ";

    if (monate==0) monate="";
    if (monate==1) monate=monate+" Monat,  ";
    if (monate>1) monate=monate+" Monate,  ";

    if (tage==0) tage="";
    if (tage==1) tage=tage+" Tag,  ";
    if (tage>1) tage=tage+" Tage,  ";

    if (stunden==0) stunden="";
    if (stunden==1) stunden=stunden+" Stunde,  ";
    if (stunden>1) stunden=stunden+" Stunden,  ";

    (minuten!=1)?minuten=minuten+" Minuten":minuten=minuten+" Minute";

    document.getElementById("vdrUptime").innerHTML = jahre+monate+tage+stunden+minuten;
  }
  else document.getElementById("vdrUptime").innerHTML = "vdr läuft nicht";

  setTimeout('countup2()',1000);
}

windows.setTimeout('countup1()',1000);
windows.setTimeout('countup2()',1000);
// -->
</script>

<?php


// *************************************** Container Begin 'Zeit / Uptime' ****************************************************
container_begin(2, "Uhr.png", $txt_ue_zeit);

echo "<center>";
echo "<table class='tab2'>";
echo "  <tr><th align='right' width=50%'>$txt_systemzeit:</th><td align='left'><div id='ZeitBox01'><div id='ZeitAnzeige'>wird ermittelt</div></div></td></tr>";
echo "  <tr><th align='right'>$txt_uptime:</th><td align='left'><div id='Uptime'>wird ermittelt</div></td></tr>";
echo "  <tr><th align='right'>$txt_vdr_uptime:</th><td align='left'><div id='vdrUptime'>wird ermittelt</div></td></tr>";
//echo "  <tr><th align='right'>vdr_pid:</th><td align='left'>$vdrpid </td></tr>";
echo "</table>";
echo "</center>";

container_end();
// *************************************** Container Ende ****************************************************

?>