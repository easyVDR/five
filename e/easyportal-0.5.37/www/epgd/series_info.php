<?php
$series_id = $_GET[series_id];

include("inc/inc_head.php");
include("inc/inc_series.php");

echo "<h1>Info<h1>";

echo "<table id=tab1>";
echo "  <tr>";
echo "    <th>Serien-ID</th>";
echo "    <td>".$row->series_id."&nbsp;</td>";
echo "  </tr>";
echo "  <tr>";
echo "    <th>Serien-Name</th>";
echo "    <td>".$row->series_name."&nbsp;</td>";
echo "  </tr>";
echo "  <tr>";
echo "    <th>Serien-Info</th>";
echo "    <td>".$row->series_overview."&nbsp;</td>";
echo "  </tr>";
echo "  <tr>";
echo "    <th>Serien-Start</th>";
echo "    <td>".datum2de($row->series_firstaired)."&nbsp;</td>";
echo "  </tr>";
echo "  <tr>";
echo "    <th>Serien-Network</th>";
echo "    <td>".$row->series_network."&nbsp;</td>";
echo "  </tr>";
echo "  <tr>";
echo "    <th>Serien-Genre</th>";
echo "    <td>".$row->series_genre."&nbsp;</td>";
echo "  </tr>";
echo "  <tr>";
echo "    <th>Serien-Rating</th>";
echo "    <td>".$row->series_rating."&nbsp;</td>";
echo "  </tr>";


echo "</table>";
echo "<br>";

include("inc/inc_foot.php");
?>