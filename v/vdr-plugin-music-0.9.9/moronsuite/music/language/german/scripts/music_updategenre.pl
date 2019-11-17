#!/usr/bin/perl

# Module verwenden
use XML::Simple;

# your developer key. ask at shoutcast.com for it
#$devid="";
$devid="";


# path where music-plugin is looking for webstreams.dat ($PLUGINDIR/language/$LANGUAGE/data: e.g.: /opt/vdr/etc/plugins/music/language/german/data)
$output   = "$ARGV[0]";

$limits   = "'$ARGV[1]'";
$bitrates = "'$ARGV[2]'";


if ($ARGV[1] eq "") {
  die " Need parameter for output.\n Please run:  shoutcastGenre.pl 'PATH'\n where PATH is the place  vdrplugin-music search for webstreams.dat\n";
#  die "Need parameter for output. Please run / shoutcastGenre.pl 'Where_to_save_Genrelist' 'limits' 'bitrate' / !\n";
}  


#if ($ARGV[1] eq "") {
#  $limits = "'10'";
#}

#if ($ARGV[2] eq "") {
#  $bitrates = "'128'";
#}

# 
my $tempdir          = "/tmp";                                           # for temp. files
my $shoutcastscript  = "/VDR/bin/shout2vdr";				 # full path/link to this script
my $shoutcast2vdrfav = "/VDR/bin/shoutcast2vdr-fav.sh";
my $screen           = "/usr/bin/screen -A -m -d -S shout2vdr_update";	 # full path/link to screen (default)
## my $shoutcastxml     = "http://yp.shoutcast.com/sbin/newxml.phtml";      # don't change
my $shoutcastxml     = "http://api.shoutcast.com/legacy/genrelist?k=$devid";      # don't change


system("wget -T 10 -O '$tempdir/genres.xml'  '$shoutcastxml'");
if ( -e "$tempdir/genres.xml" ) {
  print "Genreliste '$tempdir/genres.xml' vorhanden\n\n";
  }
else {
  die "Genreliste nicht vorhanden -> ABBRUCH . $!\n";
}  

# Objekt erstellen
$xml = new XML::Simple (KeyAttr=>[], forcearray=>['genre name',]);

# XML-Datei einlesen
$data = $xml->XMLin("$tempdir/genres.xml");

print "Oeffne $output/shoutcast.dat\n";

open(OUT,">$output/shoutcast.dat") || die ("Could not open $!");


 print OUT "---- Top Streams ---------------------------------------------------------------------------- : echo\n";
 print OUT "  * Genre Top 40 *?            : $screen $shoutcastscript 'Top40' \n";
 print OUT "---- Electronic  ---------------------------------------------------------------------------- : echo\n";
 print OUT "  * Genre Dance *?             : $screen $shoutcastscript 'Dance' \n";
 print OUT "  * Genre Electronic *?        : $screen $shoutcastscript 'Electronic' \n";
 print OUT "  * Genre Chillout *?          : $screen $shoutcastscript 'Chill' \n";
 print OUT "  * Genre Disco *?             : $screen $shoutcastscript 'Disco' \n";
 print OUT "  * Genre Club *?              : $screen $shoutcastscript 'Club' \n";
 print OUT "  * Genre House *?             : $screen $shoutcastscript 'House' \n";
 print OUT "  * Genre Trance *?            : $screen $shoutcastscript 'Trance' \n";
 print OUT "  * Genre Techno *?            : $screen $shoutcastscript 'Techno' \n";
 print OUT "---- Pop/Rock    ---------------------------------------------------------------------------- : echo\n";
 print OUT "  * Genre Metal Rock *?        : $screen $shoutcastscript 'Metal' \n";
 print OUT "  * Genre Rock *?              : $screen $shoutcastscript 'Rock' \n";
 print OUT "  * Genre Pop *?               : $screen $shoutcastscript 'Pop' \n";
 print OUT "  * Genre 80s *?               : $screen $shoutcastscript '80s' \n";
 print OUT "  * Genre Oldies *?            : $screen $shoutcastscript 'Oldies' \n";
 print OUT "  * Genre Classic Rock/Pop *?  : $screen $shoutcastscript 'Classic' \n";
 #print OUT "--------------------------------------------------------------------------------------------------------------------------------   : echo\n";
 print OUT "--------------------------------------------------------------------------------------------------------------------------------   : echo\n";
 print OUT "  * Aktualisiere Favoriten *?  : $screen $shoutcast2vdrfav \n";
 print OUT "--------------------------------------------------------------------------------------------------------------------------------   : echo\n";
# print OUT "--------------------------------------------------------------------------------------------------------------------------------   : echo\n";
 print OUT "---- Shoutcast Genres------------------------------------------------------------------------ : echo\n";


foreach $e (@{$data->{genre}})
{
  my $genre = "$e->{name}";
  print OUT "$genre?                        : $screen $shoutcastscript '$genre' \n";
}

close OUT;
