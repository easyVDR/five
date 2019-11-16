#!/usr/bin/perl
#
#############################################################################
#
#
my $Version     = "0.3.0-beta6";
#
my $Date        = "2008-06-10";
#
# Author: Mike Constabel Portal: <vejoun @ vdrportal>
#                         eMail: <vejoun @ toppoint . de>
#
# See "VDRSeriesTimer.pl --help" for help.
# See "VDRSeriesTimer.pl --man" for documentation.
#
#############################################################################

use strict;
use warnings;

BEGIN {
  my($error, @Modules, $mod);
  @Modules = qw(Eplists Getopt::Long Config::General Data::Dumper Fcntl Pod::Usage File::Basename locale POSIX Locale::TextDomain Locale::Messages Text::LevenshteinXS);
  for $mod (@Modules) {
    if (eval "require $mod") {
      if ( $mod eq "Config::General" ) {
        $mod->import("ParseConfig");
      } elsif ( $mod eq "Fcntl" ) { 
        $mod->import(":DEFAULT", ":flock");
      } elsif ( $mod eq "Text::LevenshteinXS" ) {
        $mod->import("distance");
      } elsif ( $mod eq "Locale::TextDomain" ) {
        $mod->import('eplists', "/usr/share/locale", "/usr/local/share/locale");
#      } elsif ( $mod eq "Locale::Messages" ) {
#        $mod->import("LC_MESSAGES");
      } elsif ( $mod eq "POSIX" ) { 
        $mod->import("locale_h", "setlocale");
      } elsif ( $mod eq "Eplists" ) { 
        $mod->import("Eplists_Version", "Eplists_Date", "Eplists_Update", "Eplists_Search");
      } else {
        $mod->import;
      }
    } else {
      $error = 1;
      print STDERR "Module $mod not found, please install $mod\n";
      print STDERR "On Debian:\naptitude install perl-modules libconfig-general-perl libintl-perl\nperl -MCPAN -e 'install Text::LevenshteinXS'\n";
    }
  }  
  exit 1 if $error;
}
                                                                                                                                                  
setlocale (LC_MESSAGES, '');

################################
# evaluate commandline options #
################################
my %Config;

Getopt::Long::Configure ("bundling_override");
GetOptions (\%Config,	'Title|t=s', 'Subtitle|s=s', 'EpisodeOverAll|Episode|e:i', 'Summary:s', 'Category:s', 'Genre:s', 'Date:s', 'Time:s',
                        'Channel:i', 'TimeT:i', 'Length:i', 'Verbose|v:+', 'UseShortTitle|short!', 'DryRun|dry-run|dry',
                        'Format|f=s', 'FormatPrefix|fp=s', 'FormatSpacer|fs=s', 'help|h|?', 'man', 'ListsDir|ld=s', 'version|V',
                        'PrintLists|Lists|l', 'PrintOptions|options', 'upgrade', 'forceupgrade',
                      ) or pod2usage(2);

if ( exists $Config{version} ) {
  print "$0 v$Version $Date (Eplists.pm v".Eplists_Version()." ".Eplists_Date().")\n";
  exit 0;
}
pod2usage(1)					if ( $Config{help} );

pod2usage(-exitstatus => 0, -verbose => 2)	if ( $Config{man} );

my $Result_ref = Eplists_Search( \%Config );

foreach (@{$$Result_ref{StdErr}}) {
  print "$_\n";
}

if ( $Config{Verbose} && $Config{Verbose} >= 4 ) {
  print "\n----------------------------------------------------------------------------------\n\n";
  print Dumper($Result_ref);
  print "\n----------------------------------------------------------------------------------\n\n";
}

print $$Result_ref{Result}."\n";


#open FILE, ">wishlist.txt";
#foreach my $key ( sort keys %{$$Result_ref{WishList}} ) {
#  print FILE $key, "\t", $$Result_ref{WishList}{$key}{count}, "\n";
#}
#close FILE;


############################
# Inline man page and help #
############################

__END__

=head1 NAME

VDRSeriesTimer.pl - Script to find season and episode number with the help
                    of episode lists

=head1 SYNOPSIS

VDRSeriesTimer.pl ( -c=<> | -ec=<> ) --t=<> (--sub=<> | --ep=<>) [options...]

 Help options:

   --help | -h | -?		brief help message
   --man			full documentation

 Needed options:

     At least one of them:

   --title | -t			series title
   --subtitle | -s 		series subtitle
   --lists | -l			lists episodes files
   --options			print options/variables and exit

 Optional options:

   --summary			series summary

   --episode | -ep		overall episode number

   --formatprefix | -fp		First part of the output string, default: "%D~%T~"

   --format | -f		Output format, default: "%sx%e - %n. %S"

   --formatspacer | -fs		How to seperate the subtitles if there is more than one,
                                default: " / "

                                  %T - Title
                                  %S - Subtitle
                                  %s - Season number
                                  %e - Episode number
                                  %n - Episode number over all
                                  %D - Directory
                                  %C - Category
                                  %G - Genre

                                Example:
                                  Smallville~05x10 - 098. Lex gegen Jonathan

   --category			category
   --genre			genre
   --date			date
   --time			time
   --timet			timestamp
   --channel			channel number

   --useshorttitle | -short	use / don't use the SHORT keyword in episode list, defaults to "use"
   --nouseshorttitle | -noshort

   --verbose | -v		verbosity, increasing by using it more than one times, up to three times
   --dry-run | -dry		don't change EPG via SVDRP or any file
   --version | -V		show version number

=head1 OPTIONS

=over 8

=item B<-help>

Print a brief help message and exits.

=item B<-man>

Prints the manual page and exits.

=back

=head1 DESCRIPTION

B<VDRSeriesTimer.pl> is called out of the epgsearchuservars.conf with some parameters.

It needs VDR > 1.4.0 and epgsearch > 0.9.19.

This allows you to configure the timer which is set by an series-searchtimer.

Normally, if the EPG contains something like this:

 Title:    The Simpsons
 Subtitle: My Big Fat Geek Wedding

you will get an Timer like this

 The Simpsons~My Big Fat Geek Wedding

With this Script you can get this

 The Simpsons~15x17 - 330. My Big Fat Geek Wedding

or this

 The Simpsons~15.17 My Big Fat Geek Wedding

or this

 The Simpsons~S15E17 My Big Fat Geek Wedding

The Script recognize also some multi epiosodes with the EPG-format which RTL II uses in germany.

With

 Title:    Andromeda
 Subtitle: 15. Episode 1 / 16. Episode 2

you get

 Andromeda~01x15 - 15. Episode 1 / 01x16 - 16. Episode 2

=head1 CONTENT

 1. Installation
 1.1 --format
 2. Episodes file
 2.x Keywords
 3. VDRSeriesTimer.conf - The configuration file
 4. "Per series" configuration file
 5. Examples
 6. How to get episode files
 7. COPYRIGHT and LICENSE

=head1 1. Installation

Copy the Script VDRSeriesTimer.pl somewhere in the $PATH.

Then edit F<epgsearchuservars> and add one of the following:

A) If you've extended EPG (tvmovie, hörzu, etc.):

 %Season%=system(/usr/local/bin/VDRSeriesTimer.pl,--title %Title% --subtitle %Subtitle% --episode %Episode% --category %Category% --genre %Genre% --date %date% --time %time% --channel %chnr% --timet %time_lng%)

 %DateVar%=%time_w% %date% %time% 
 %SeriesSD%=%Subtitle% ? %Subtitle% : %DateVar%
 %SeriesVar3%=Series~%Title%~%Episodes%. %SeriesSD%
 %SeriesVar4%=Series~%Title%~%SeriesSD% 
 %SeriesVar1%=%Episode% ? %SeriesVar3% : %SeriesVar4%

 %Series%=%Season% ? %Season% : %SeriesVar1%

B) If you dont't have extended EPG with category, genre etc., but the episode number, you can use a shorter version.

 %Season%=system(/usr/local/bin/VDRSeriesTimer.pl,--title %Title% --subtitle %Subtitle% --episode %Episode% --channel %chnr% --timet %time_lng%)
 
 %DateVar%=%time_w% %date% %time%
 %SeriesSD%=%Subtitle% ? %Subtitle% : %DateVar%
 %SeriesVar3%=Series~%Title%~%Episodes%. %SeriesSD%
 %SeriesVar4%=Series~%Title%~%SeriesSD% 
 %SeriesVar1%=%Episode% ? %SeriesVar3% : %SeriesVar4%

 %Series%=%Season% ? %Season% : %SeriesVar1%

C) If you don't have an episode number in EPG:

 %Season%=system(/usr/local/bin/VDRSeriesTimer.pl,--title %Title% --subtitle %Subtitle% --channel %chnr% --timet %time_lng%)
 
 %DateVar%=%time_w% %date% %time%
 %SeriesSD%=%Subtitle% ? %Subtitle% : %DateVar%
 %SeriesVar1%=Series~%Title%~%SeriesSD% 

 %Series%=%Season% ? %Season% : %SeriesVar1%

In this short version, the script always tries to find an matching subtitle in the episodes list.

Then use in the searchtimer the variable %Series% as Path.

The episode files must have as filename the title of the series with the suffix I<.episodes>.

So for the series I<The Simpsons>, it searches for I<The Simpsons.episodes>.

The Script allows per default a difference of 20 percent of the title, subtitle and the filename.

=head2 1.1 --format

With I<--formatprefix <string>>, I<--format <string>> and I<--formatspacer <string>> you can define how the
output should look. The default is:

 "%D~%T~"         for --formatprefix <string>
 "%sx%e - %n. %S" for --format <string>
 " / "            for --formatspacer <string>

Call "VDRSeriesTimer.pl -help" to see what each variable means.

You can also change how many zeroes will be prefixed.

 "%s" results in "02"
 
 "%5s" results in "00002"

This can be used with %s, %e and %n. Don't try this with %D, %T, %S, %C and %G.

=head1 2. Episodes file

The file contains the episodes in the following format. Each field is seperated by one tab.

 #Season	Episode Number	Subtitle
 01	1	1	Season one, Episode one
 01	2	2	Season one, Episode two
 01	3	3	Season one, Episode three
 02	1	4	Sesaon two, Episode one
 02	2	5	Season two, Episode two
 ...


At the beginning of the file you can use some special keywords.
Each keyword must begin with "# ".

Take a look on some of the "ready to use" lists, Webaddress at the end of this file.

=head2 2.x Keywords

Description of the available keywords.

=head2 2.1 B<SEASONLIST ... /SEASONLIST>

If you don't have a complete episode list but your EPG contains the episode number,
you can define which season contains which episodes. Then this script can calculate
the needed data. Episodes, which are not in this file will be added.

 # SEASONLIST
 # 1	1	22
 # 2	23	44
 # 3	45	66
 # /SEASONLIST

The format is <Season> TAB <first episode in season> TAB <last episode in season>

=head2 2.5 B<COMPLETE>

 # COMPLETE

The script assume that the episode list is complete, nothing will be changed in the file.

=head2 2.7 B<SHORT <text>>

This text will be used as series title. So you can use this

 # SHORT DS9

to get "DS9", if the title in the EPG is "Deep Space Nine"

You can use your own short title by using a "per series" configuration file. See chapter 4.

=head1 3. VDRSeriesTimer.conf - The configuration file

If the configuration file .VDRSeriesTimer.conf in the users does not
exist, an file with default values named .VDRSeriesTimer.conf will be created in your
current $HOME.

Thes settings are:

B<ListsDir>          = ""

Path to the episodes list.

B<SeriesConfsDir>          = ""

Path where to find the "per series" configuration.

B<UseShortTitle>     = yes

Replace the title with the short title found in the "per series" configuration file or
the .episodes file.

B<Protected>         = no

Don't change any file.

B<FormatPrefix>      = "%D~%T~"

B<Format>            = "%sx%e - %n. %S"

B<FormatSpacer>      = " / "

Output format.

B<Fuzzy>             = 0.20

Fuzzy matching in percent.
0.00 means must match 100%.
1.00 means must match 0%.

Useful is something between 0.00 and 0.25.

B<SendEMail>         = no

Send an e-mail if something gets wrong, eg. an epiosde list cannot be found.

B<EMailTo>           = ""

E-mail recipient.

B<EMailSubject>      = "Season data missing"

Subject.

B<ChangeEPG>         = no

Add the season data to the epg summary. The script does this via svdrp.

B<KeepEPGSubtitle>   = yes

If the subtitle in the episode list differs from the one from EPG, don't change it
to the subtitle from list.

B<CompareSubtitle>   = no

If yes, search the list by comparing the subtitle to get the season and episode numbers. 

B<CompareEpisode>    = no

If yes, search the list by comparing the episode number to get the subtitle.

B<Fallback>          = yes

First, the script tries to compare Subtitle if CompareSubtitle is set.
If this fails, it fall back to use other information to guess the missing data.
If you don't want this, set this to no and the script returns
unknown (?x? - ?.) if the first try fails.

B<VDRHost>           = "localhost"
B<SVDRPPort>         = 2001
B<SVDRPTimeout>      = 30

SVDRP settings.

B<MultiRegex>        = "(?:\s*)(?:(\d+)\.\s+)?(.*?)\s*(?:\/|$)"

How to detect multi episodes like Andromeda or Stargate on german RTL II.
Fields are EpisodeNr, Subtitle.
Dont' change this if you don't know what you do. ;)

B<Verbose>           = 0

Verbosity, increasing by using it more than one times, up to three times

B<LogFile>           = ""

File to log to.

=head1 4. "Per series" configuration file

If you define the path B<SeriesConfsDir>, you can put there one configuration file per series.

File needs to be named as the episodes file, but with ".episodes" replaced with ".conf", for
example: B<Lost.conf>.

Settings from this file will overwrite the global ones from VDRSeriesTimer.conf.

Possible settings are:

B<UseShortTitle>     = no

B<Protected>         = yes

B<FormatPrefix>      = "%D~%T~"
B<Format>            = "%sx%e - %n. %S"
B<FormatSpacer>      = " / "

B<Fuzzy>             = 0.20

B<SendEMail>         = no

B<ChangeEPG>         = yes

B<KeepEPGSubtitle>   = yes

See previous chapter for description.

B<CompareSubtitle>   = no

If yes, search the list by comparing the subtitle to get the season and episode numbers. 

B<CompareEpisode>    = no

If yes, search the list by comparing the episode number to get the subtitle.

B<Fallback>          = yes

First, the script tries to compare Subtitle if CompareSubtitle is set.
If this fails, it fall back to use other information to guess the missing data.
If you don't want this, set this to no and the script returns
unknown (?x? - ?.) if the first try fails.

B<EpisodeIncrement>  = +0

If the episode number in the EPG isn't correct, you can increment or decrement the episode number.
For example, +5 or -5.

B<StartWithSeason>   = 0

Start the comaparing with this season number. Needed for "24", because there are the same subtitles in
each season.

B<MultiRegex>        = "(?:\s*)(?:(\d+)\.\s+)?(.*?)\s*(?:\/|$)"

How to detect multi episodes like Andromeda or Stargate on german RTL II.
Fields are EpisodeNr, Subtitle.
Dont' change this if you don't know what you do. ;)

B<Verbose>           = 0

Verbosity, increasing by using it more than one times, up to three times

B<LogFile>           = ""

File to log to.

=head1 5. Examples

 VDRSeriesTimer.pl --config /etc/vdr.conf/plugins/epgsearch/data/ --title 'Smallville' --subtitle 'Lex gegen Jonathan'

Results in:

 Smallville~05x10 - 098. Lex gegen Jonathan


 VDRSeriesTimer.pl --title 'Smallville' --subtitle 'Lex gegen Jonathan' --format "%T~%3s.%e (%4n) %S"

Results in:

 Smallville~005.10 (0098) Lex gegen Jonathan

=head1 6. How to get episode files

The episode files can be found there:

C<http://eplists.constabel.net/eplists/eplist.cgi>

If you like git, see there:

Overview:

C<http://eplists.constabel.net/git/gitweb.cgi?p=eplists.git;a=tree>

Download:

C<cg-clone http://eplists.constabel.net/git/eplists.git>

=head1 7. COPYRIGHT and LICENSE

Copyright (c) 2007 Mike Constabel

L<http://www.constabel.net/vdr/>

This  is free software.  You may redistribute copies of it under the terms of the GNU General Public License <http://www.gnu.org/licenses/gpl.html>.
There is NO WARRANTY, to the extent permitted by law.

=cut

