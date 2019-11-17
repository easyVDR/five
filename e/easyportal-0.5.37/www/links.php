<?php
include('includes/kopf.php'); 

echo "<div id='kopf2'>? - Links</div>";

echo "<div id='inhalt'>";
echo "<center>";

include('includes/subnavi_hilfe.php'); 

echo "<h2>Links</h2>";

echo "<div class='links'>";

// *************************************** Container Begin ****************************************************
container_begin(2, "book_link.png", "Forum");
echo "<center><a href='http://www.easy-vdr.de/portal.php' target='_new'><img src='images/links/hp_forum_s.jpg' class='responsive_img' style='max-width:400px;'></a></center>";
container_end();
// *************************************** Container Ende ****************************************************

// *************************************** Container Begin ****************************************************
container_begin(2, "book_link.png", "Wiki");
echo "<center><a href='http://wiki.easy-vdr.de/' target='_new'><img src='images/links/hp_wiki_s.jpg' class='responsive_img' style='max-width:400px;'></a></center>";
container_end();
// *************************************** Container Ende ****************************************************

echo "</div>";  // class='links'
echo "<div class='rechts'>";

// *************************************** Container Begin ****************************************************
container_begin(2, "book_link.png", "Ubuntu Pakete");
echo "<center><a href='https://launchpad.net/~easyvdr-team' target='_new'><img src='images/links/hp_ppa_s.jpg' class='responsive_img' style='max-width:400px;'></a></center>";
container_end();
// *************************************** Container Ende ****************************************************

// *************************************** Container Begin ****************************************************
container_begin(2, "book_link.png", "Sourcecode");
echo "<center><a href='http://www.easy-vdr.de/git/?p=trusty/.git' target='_new'><img src='images/links/hp_git_s.jpg' class='responsive_img' style='max-width:400px;'></a></center>";
container_end();
// *************************************** Container Ende ****************************************************

echo "</div>";  // class='rechts'
echo "<div class='clear'></div>"; // Ende 2-spaltig

echo "</center>";
echo "</div>";  // Inhalt

include('includes/fuss.php'); 

?>