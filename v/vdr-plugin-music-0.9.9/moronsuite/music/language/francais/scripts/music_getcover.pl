#!/usr/bin/perl -X

# EDIT only! path2java


$ENV{PATH}="/bin:/sbin:/usr/bin:/usr/local/bin:/opt/blackdown-jre-1.4.2.03/bin";


$artist    = "$ARGV[0]";
$album     = "\L$ARGV[1]";
$coverdir  = "$ARGV[2]";
$tmpdir    = "$ARGV[3]";
$maxdl     = "$ARGV[4]";
$basedir   = "$ARGV[5]";
$filename  = "$ARGV[6]";
$logger    = "$tmpdir/cover.log";
$path2java = "/opt/blackdown-jre-1.4.2.03/bin";


print "\n
----------------------------------------------------------------------------------------------\n
NAME:
          getcover.pl - Perl Script to download coverpictures
	  from amazons webservice.
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
Javapath : $path2java
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

system("echo 'Executing: $path2java/java -cp jdom.jar:coverdownload.jar:. cover.cover \"$artist\" \"$album\" \"cover\" $maxdl' >> $logger");
system("cd $tmpdir; $path2java/java -cp jdom.jar:coverdownload.jar:. cover.cover \"$artist\"  \"$album\" \"cover\" $maxdl >/dev/null");

if(-r "$tmpdir/cover_0.jpg") {
    system("echo '\nDownload von Cover erfolgreich !\n' >> $logger");

#    $artist =~ s/\//-/g;
#    $artist =~ s/\'/ /g;
#    $artist =~ s/\"/ /g;

#    if(-r "$coverdir/$artist.jpg") {
#      system("echo 'Entferne altes Cover\n' >> $logger");
#      system("rm -f \"$coverdir/$artist.jpg\"");
#    }


#    $cmd = "cp -f \"$tmpdir/cover_1.jpg\" \"$coverdir/$artist.jpg\"";

#    system("echo 'Kopiere: $cmd\n' >> $logger");
#    system("$cmd");

#    if(-r "$coverdir/$artist.jpg") {
#        system("echo '\nCover erfolgreich kopiert !\n' >> $logger");
#        }
#      else {
#        system("echo '\nERROR: Cover wurde NICHT kopiert !\n' >> $logger");
#      }

    }
else {
    system("echo '\nERROR: Download von Cover NICHT erfolgreich !\n' >> $logger");
}

system("cat '$logger'");
    