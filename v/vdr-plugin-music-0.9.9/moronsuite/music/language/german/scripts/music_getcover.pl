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
$tempdir   = "$tmpdir/music_cover";
$nodata    = "$tmpdir/music_nodata";
$logger    = "$tempdir/cover.log";
$CMD       = "";
$home= $ENV{"HOME"};
$chooserdir= "$home/.coverchooser/Temp";


print "\n
----------------------------------------------------------------------------------------------\n
NAME:
          getcover.pl - Perl Script to download coverpictures
\n       
SYNOPSIS:       
          getcover.pl 'artist' 'album' 'coverdir' 'tmpdir' 'maxdl' 'basedir' 'filename'
\n
DESCRIPTION:
          This Perl script makes it a lot easier to fill your
	  musiccollection with coverpictures.
\n
OPTIONS:
          artist       = Artist of track
	  album        = Name of album
          coverdir     = Path to artists cover directory
	  tmpdir       = Path where to save downloaded images
	  maxdl        = How much coverpicture do you want ?
	  basedir      = Path to track
	  filename     = Full path to track without suffix
\n
----------------------------------------------------------------------------------------------\n
\n";

# START.......

system("echo 'Cover download started !\n' > $logger");

# DON'T CHANGE ANYTHING HERE
system("echo 'Artist     : $artist
Album      : $album
Coverdir   : $coverdir
Tmpdir     : $tmpdir
Tempdir    : $tempdir
Nodata     : $nodata
MaxDL      : $maxdl
Logfile    : $logger
Chooserdir : $chooserdir
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


#if(-r "$tempdir/cover_0.jpg") {
#    system("echo '\nEntferne vorhandene Bilddateien\n' >> $logger");
#    system("rm -f $tempdir/*.jpg");
#    system("rm -f $tempdir/*.png");
#}

if(-r "$tempdir/cover.log") {
    system("echo '\nEntferne vorhandene Bilddateien\n' >> $logger");
    unlink glob("$tempdir/*.jpg");
    unlink glob("$tempdir/*.png");
    unlink glob("$tempdir/*.gif");
    }
else {
    system("echo '\n$tempdir/cover.log nicht lesbar (ABBRUCH)\n' >> $logger");
    system("cat '$logger'");
    die
}

$CMD="coverchooser --artist=\"$artist\" --album=\"$album\" --path=\"$tempdir\" --all --ordered --module=discogs --module=lastfm --module=google --timeout=5 --verbose";
system("echo 'Command: $CMD\n\n' >> $logger");

system("echo '\n\n====================== Coverdownload start here =====================\n'");
system($CMD);
system("echo '\n====================== Coverdownload end here   =====================\n\n'");



$result=0;


@files = <$chooserdir/*.jpg>;
foreach $file (@files) {
    if (-f $file) {
        system("echo 'Kopiere Datei: $file nach $tempdir' >> $logger");
        system("cp -f $file $tempdir");
        $result=1;
        break;
    }
}

@files = <$chooserdir/*.png>;
foreach $file (@files) {
    if (-f $file) {
        system("cp -f $file $tempdir");
        $result=1;
        break;
    }
}

@files = <$chooserdir/*.gif>;
foreach $file (@files) {
    if (-f $file) {
        system("cp -f $file $tempdir");
        $result=1;
        break;
    }
}


#system("cp -f $chooserdir/*.jpg $tempdir");
#system("cp -f $chooserdir/*.png $tempdir");



#@files = <$chooserdir/*.jpg>;
#foreach $file (@files) {
#    if (-f $file) {
#        print "\nThis is a file: " . $file;
#        $result=1;
#    }
#}



#@files = <$chooserdir/*.jpg>;
#foreach $file (@files) {
#    if (-f $file) {
#        $result=1;
#        break;
#    }
#}

#@files = <$chooserdir/*.png>;
#foreach $file (@files) {
#    if (-f $file) {
#        $result=1;
#        break;
#    }
#}

#@files = <$chooserdir/*.gif>;
#foreach $file (@files) {
#    if (-f $file) {
#        $result=1;
#        break;
#    }
#}



print "\nresult: $result \n\n";

if($result>0) {
    system("echo '\nDownload von Cover erfolgreich !\n' >> $logger");
    system("/usr/bin/svdrpsend PLUG coverviewer PLAYDIR $tempdir");
    }
else {
    system("echo '\nERROR: Download von Cover NICHT erfolgreich !\n' >> $logger");
    system("/usr/bin/svdrpsend PLUG coverviewer PLAYDIR $nodata");
}

system("cat '$logger'");
