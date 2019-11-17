<?php
    // MySQL-Verbindung herstellen
    include("inc/inc_db.php");

    // Bild ausgeben
    $id = $_GET['id'];

    $result = mysql_query("SELECT media_content, media_type FROM series_media WHERE episode_id='$id'");
    $row = mysql_fetch_object($result);
    header("Content-type: $row->media_type");
    echo $row->media_content;
?>