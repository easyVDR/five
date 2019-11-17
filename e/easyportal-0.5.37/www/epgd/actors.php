<?php
include("inc/inc_head.php");

echo "<h1>Schauspieler<h1>";

echo "<table id=tab1>";
echo "  <tr>";
echo "    <th>ID</th>";
echo "    <th>Schauspieler</th>";
//echo "    <th>Serienstart</th>";
//echo "    <th>Genre</th>";
echo "  </tr>";

$abfrage = "SELECT * FROM movie_actor ORDER BY actor_name";
$ergebnis = mysql_query($abfrage);
while($row = mysql_fetch_object($ergebnis))
{
    echo "  <tr>";
    echo "    <td>".$row->actor_id."&nbsp;</td>";
    echo "    <td><a href='actor.php?actor_id=".$row->actor_id."'>".$row->actor_name."</a>&nbsp;</td>";
//    echo "    <td>".datum2de($row->series_firstaired)."&nbsp;</td>";
//    echo "    <td>".$row->series_genre."&nbsp;</td>";
//    echo "    <td><img src='../images/kategorie/".$row->kategorie.".jpg' height='50'>&nbsp;</td>";
//    echo "    <td><a href='v_kategorie_edit.php?ID=".$row->ID."'><button class='button1'>Edit</button></a></td>";
    echo "  </tr>";
}
echo "</table>";
echo "<br>";

include("inc/inc_foot.php");
?>