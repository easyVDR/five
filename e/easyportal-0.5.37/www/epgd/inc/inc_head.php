<?php
session_start();

$errors = array();

//error_reporting(E_ALL);
//ini_set("display_errors", 1);  

$dbhost = $_SESSION['dbhost'];
$dbuser = $_SESSION['dbuser'];
$dbpass = $_SESSION['dbpass'];
$dbname = $_SESSION['dbname'];

/*
echo "dbhost = '$dbhost' <br/>";
echo "dbuser = '$dbuser' <br/>";
echo "dbpass = '$dbpass' <br/>";
echo "dbname = '$dbname' <br/>";
*/

mysql_connect($dbhost,$dbuser,$dbpass) or die ("Keine Verbindung möglich");
mysql_select_db($dbname) or die ("Die Datenbank existiert nicht");

// ******************************  Funktionen *********************************

function datum2de($datum) 
{
    list($jahr, $monat, $tag) = explode("-", $datum);
    return sprintf("%02d.%02d.%04d", $tag, $monat, $jahr);
}


function datetime2de($datum_zeit) 
{
    list($datum, $zeit) = explode(" ", $datum_zeit);
    list($jahr, $monat, $tag) = explode("-", $datum);
    list($stunde, $minute, $sekunde) = explode(":", $zeit);
    return sprintf("%02d.%02d.%04d", $tag, $monat, $jahr)." ".$stunde.":".$minute." Uhr";
}


function fehlerausgabe($errors) 
{
    // Wurde bei der Formularprüfung ein Fehler gefunden wird er ausgegeben
    if(isset($errors))
    {
         echo "<div id='rahmen_rot'>";
//         echo "<font color='#FF0000'>Fehler:</font>";
         echo "<ul>";
         foreach ($errors as $error)  { echo "<li>".$error."</li>"; }
         echo "</ul>";
         echo "</div>";
         echo "<br>";
    }
}

function format_zahl2 ($zahl)
{
//  $zahl = trim($zahl);
    $zahl = str_replace(',', '.', $zahl);
    $zahl = number_format($zahl,2);
    $zahl = eregi_replace(",", "", $zahl);
    return ($zahl);
}

function format_text ($text)
{
    $text = trim($text);
    return ($text);
}



// *****************************************************************************

echo "<html>";
echo "<head>";
echo "    <META http-equiv='content-type' content='text/html; charset='utf-8'>";
echo "    <META http-equiv='content-language' content='de'>";
echo "    <META name='author' content='Neusel'>";
echo "    <META name='publisher' content='Neusel'>";
echo "    <link rel='stylesheet' type='text/css' href='inc/epgd.css'>";
//echo "    <link rel='stylesheet' type='text/css' href='../inc/tcal.css'>";
//echo "    <script type='text/javascript' src='../inc/tcal.js'></script>";
echo "    <title>epg Datenbank</title>";
echo "</head>";

echo "<body>";
echo "<div id='alles'>";

// *********************************************** Menü *********************************************


echo "<div id='links'>";
//echo "    <img src='../images/logo_kreis_2.gif' width='150'>";
echo "<table border='0' cellpadding='0' cellspacing='0' align='center'>";

echo "<tr height='10'><td></td></tr><tr height='30'>";
echo "<td class='tableheads_off' onmouseover=\"this.className='tableheads_on'\" onmouseout=\"this.className='tableheads_off'\" align='center' width='110'>";
echo "<a href='series.php' style='display:block;margin:0px;width:100%;height:100%;'>Serien</a>";
echo "</td></tr>";

echo "<tr height='10'><td></td></tr><tr height='30'>";
echo "<td class='tableheads_off' onmouseover=\"this.className='tableheads_on'\" onmouseout=\"this.className='tableheads_off'\" align='center' width='110'>";
echo "<a href='movies.php' style='display:block;margin:0px;width:100%;height:100%;'>Filme</a>";
echo "</td></tr>";

echo "<tr height='10'><td></td></tr><tr height='30'>";
echo "<td class='tableheads_off' onmouseover=\"this.className='tableheads_on'\" onmouseout=\"this.className='tableheads_off'\" align='center' width='110'>";
echo "<a href='actors.php' style='display:block;margin:0px;width:100%;height:100%;'>Schauspieler</a>";
echo "</td></tr>";


echo "</table>";

//echo "<br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br>";
//echo session_id();
//echo "<br><br>";

echo "</div>";

// *********************************************** Kopf *********************************************

echo "<div id='kopf'>";
echo "  epg Datenbank";
echo "</div>";


// *********************************************** Inhalt *********************************************

echo "<div id='inhalt'>";

?>