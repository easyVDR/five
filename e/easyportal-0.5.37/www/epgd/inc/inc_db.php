<?php
session_start();
/*
$dbhost = "192.168.1.23";
$dbuser = "epg2vdr";
$dbpass = "epg";
$dbname = "epg2vdr";
*/
$dbhost = $_SESSION['dbhost'];
$dbuser = $_SESSION['dbuser'];
$dbpass = $_SESSION['dbpass'];
$dbname = $_SESSION['dbname'];

mysql_connect($dbhost,$dbuser,$dbpass) or die ("Keine Verbindung möglich");
mysql_select_db($dbname) or die ("Die Datenbank existiert nicht");
?>