<?php
include('includes/kopf.php');

$IP = $_SERVER['SERVER_NAME'];

echo "<div id='kopf2'>Tools - SSH - Risiko</div>";

echo "<div id='inhalt'>";
echo "<center>";
include('includes/subnavi_tools.php'); 

echo "<h2>SSH</h2>";

// *************************************** Container Begin ****************************************************
container_begin(1, "dummydevice.png", "Web-SSH-Zugriff");

echo "<h3>Sind Sie sich des Risikos bewusst?</h3><br>";
echo "Bitte beachten Sie unbedingt wenn Sie Ihren VDR vom Internet erreichbar machen, dass Sie mittels des Web-SSH-Zugriffs jedem Angreifer die Möglichkeit geben, sich einzuloggen!!! ";
echo "Deshalb ändern Sie unbedingt das Zugangspasswort von <b>easyvdr</b> auf ein beliebiges Passwort mit mindestens 8 Zeichen, in dem auch Zahlen, Groß- und ";
echo "Kleinschreibung, sowie Sonderzeichen vorkommen!!!<br><br>";
echo "Ihr aktuelles Passwort ändern Sie, indem Sie sich als User easyvdr per SSH einloggen, und in der Konsole<br>";
echo "<br>";
echo "<i><b>passwd</b></i><br>";
echo "<br>";
echo "eingeben. Nun werden Sie nach dem aktuellen Passwort gefragt. Danach können Sie ein neues setzen.<br><br>
Bitte beachten Sie, dass jeder PC, der vom Internet (oder einem Firmennetzwerk) ereichbar ist, ein potentielles Risiko darstellt gehackt zu werden! Wir übernehmen keine Verantwortung für Ihr Handeln.<br><br>
Der Webzugriff per SSH wird mittels shellinabox realisiert!!!<br><br>
<b>Wenn Sie sich des Risikos bewusst sind, dann bestätigen Sie, dass Sie weiterarbeiten möchten. </b><br><br>";

echo "<a href='https://$IP:4200' target=_blank><button>ja, ich bin das Risiko bewusst</button></a>";

container_end();
// *************************************** Container Ende ****************************************************

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 
?>
