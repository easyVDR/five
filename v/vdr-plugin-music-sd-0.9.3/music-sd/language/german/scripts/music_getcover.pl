#!/usr/bin/perl -X

# EDIT only! path2java


#$ENV{PATH}="/bin:/sbin:/usr/bin:/usr/local/bin:/opt/blackdown-jre-1.4.2.03/bin";


$artist    = "$ARGV[0]";
$album     = "$ARGV[1]";
$coverdir  = "$ARGV[2]";
$tmpdir    = "$ARGV[3]";
$maxdl     = "$ARGV[4]";
$basedir   = "$ARGV[5]";
$filename  = "$ARGV[6]";
$logger    = "$tmpdir/cover.log";
$CMD       = "";

print "\n
----------------------------------------------------------------------------------------------\n
NAME:
          getcover.pl - Perl Script to download coverpictures
\n       
SYNOPSIS:       
          getcover.pl 'artist' 'album' 'coverdir' 'tempdir' 'maxdownloads' 'basedir' 'filename'
\n
DESCRIPTION:
          This Perl script makes it a lot easier to fill your
	  musiccollection with coverpictures.
\n
OPTIONS:
          artist       = Artist of track
	  album        = Name of album
          coverdir     = Path to artists cover directory
	  tempdir      = Path to this script (getcover.pl)
	  maxdownloads = How much coverpicture do you want ?
	  basedir      = Path to track
	  filename     = Full path to track without suffix
\n
----------------------------------------------------------------------------------------------\n
\n";

# START.......

system("echo 'Cover download started !\n' > $logger");

# DON'T CHANGE ANYTHING HERE
system("echo 'Artist   : $artist
Album    : $album
Coverdir : $coverdir
Tempdir  : $tmpdir
MaxDL    : $maxdl
Logfile  : $logger
\n
<value> ARTIST    =$artist
<value> ALBUM     =$album
<value> COVERDIR  =$coverdir
<value> BASEDIR   =$basedir
<value> FILENAME  =$filename
\n' >> $logger");




if(!$artist) {
    system("echo '\nArtist empty. Give up !' >> $logger");
    die;
}    


if(-r "$tmpdir/cover_0.jpg") {
    system("echo '\nEntferne vorhandene Bilddateien\n' >> $logger");
    system("rm -f $tmpdir/*.jpg");
}

$CMD="$tmpdir/music_getcover.py 1 3 $tmpdir/ \"$artist\" \"$album\" \"$tmpdir/cover\"";
system("echo 'Command: $CMD' >> $logger");

system("echo '\n\n====================== Coverdownload start here =====================\n'");
system($CMD);
system("echo '\n====================== Coverdownload end here   =====================\n\n'");


if(-r "$tmpdir/cover_0.jpg") {
    system("echo '\nDownload von Cover erfolgreich !\n' >> $logger");
    }
else {
    system("echo '\nERROR: Download von Cover NICHT erfolgreich !\n' >> $logger");
}

system("cat '$logger'");
    