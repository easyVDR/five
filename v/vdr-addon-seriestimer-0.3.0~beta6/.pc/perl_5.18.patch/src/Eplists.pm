#!/usr/bin/perl
#
# Eplists.pm, module for VDRSeriesTimer.pl
#
# Mike Constabel <eplists (at) constabel dot net>
#

package Eplists;

require 5.8.0;  #maybe

my $VERSION		= "0.1-beta12";
my $DATE		= "2008-06-10";

###########
# Options #
###########

my $DisableMailSend	= 0;

############
# /Options #
############

###########
# Modules #
###########
use warnings;
use strict;
use Exporter;

BEGIN {
  my($error, @Modules, $mod);
  @Modules = qw(Config::General Data::Dumper Fcntl Pod::Usage File::Basename locale POSIX Locale::TextDomain Locale::Messages Text::LevenshteinXS);
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

use vars qw( %Config %Default_and_User_Config %AllEpisodeLists %AllEpisodeData %AllConfLists %AllConfData %SeriesData %Global_Info @ISA @EXPORT @EXPORT_OK );

@ISA = qw(Exporter);
@EXPORT_OK = qw( Eplists_Version Eplists_Date Eplists_Update Eplists_Search %Config %AllEpisodeLists %AllEpisodeData %AllConfLists %AllConfData %SeriesData );

my $CanSendMail = 0;

if (eval "require Mail::Send") {
  import Mail::Send;
  $CanSendMail = 1;
} else {
  print STDERR __("Module")." Mail::Send ".__("not found").", ".__("please install it or edit Eplists.pm and set")." \$DisableMailSend = 1;\n" unless $DisableMailSend;
}

setlocale (LC_MESSAGES, '');

#############
# Variables #
#############
my $t_Time;
my $ModuleInitDone		= 0;

my %ConfigDefaults = (	GlobalConf		=> "/etc/eplists/vdrseriestimer.conf",
                        UserConf		=> "~/.eplists/vdrseriestimer.conf",
                        ListsDir		=> "~/.eplists/lists",
                        ListLanguage		=> "de,en",
                        ListName		=> "",
                        MissingFile		=> "",
                        SeriesConfsDir		=> "~/.eplists/series.conf",
                        UseShortTitle		=> 0,
                        Protected		=> 1,
                        Verbose			=> 0,
                        PrintOptions		=> 0,
                        FormatPrefix		=> "%D~%T~",
                        Format			=> "%sx%e - %n. %S",
                        FormatSpacer		=> " / ",
                        Fuzzy			=> 0.20,
                        SendEMail		=> 0,
                        EMailTo			=> "",
                        EMailSubject		=> "Season data missing",
                        ChangeEPG		=> 0,
                        KeepEPGSubtitle		=> 1,
                        KeepEPGTitle		=> 1,
                        VDRHost			=> "localhost",
                        SVDRPPort		=> 2001,
                        SVDRPTimeout		=> 30,
                        EplistServerName	=> "eplists.constabel.net",
                        EplistServerPort	=> "2006",
                        EplistServerTimeout	=> 60,
                        EplistServerSSL		=> 0,
                        Title			=> "",
                        ShortTitle		=> "",
                        Subtitle		=> "",
                        Summary			=> "",
                        EpisodeOverAll		=> 0,
                        Category		=> "",
                        Genre			=> "",
                        CompareSubtitle		=> 0,
                        CompareEpisode		=> 0,
                        Fallback		=> 1,
                        EpisodeIncrement	=> 0,
                        StartWithSeason		=> 0,
                        MultiRegex		=> '(?:\s*)(?:(\d+)\.\s+)?(.*?)\s*(?:\/|$)',
                        MultiDetection		=> 0,
                        DoubleIfLongerAs	=> 0,
                        Directory		=> "",
                        Date			=> "",
                        Time			=> "",
                        Channel			=> "",
                        TimeT			=> 0,
                        Length			=> 0,
                        DryRun			=> 0,
                        LogFile			=> "",
                        SeriesConfFile		=> "",
                        Bugfixes		=> 0,
                        BugfixesCode		=> 0
                     );

# Copy the defaults and change %Config later with data
%Config = %ConfigDefaults;
%Default_and_User_Config = %ConfigDefaults;

# Set default UserConf
$Config{UserConf} 	= (getpwuid($>))[7]."/.eplists/vdrseriestimer.conf";

my @Globals		= qw(ListsDir ListLanguage SeriesConfsDir UseShortTitle Protected FormatPrefix Format FormatSpacer Fuzzy SendEMail EMailTo EMailSubject ChangeEPG KeepEPGSubtitle VDRHost SVDRPPort SVDRPTimeout MultiRegex MultiDetection CompareSubtitle CompareEpisode Directory Category Genre Verbose LogFile DoubleIfLongerAs Fallback);
my @PerList		= qw(UseShortTitle ListLanguage ShortTitle Protected FormatPrefix Format FormatSpacer Fuzzy SendEMail ChangeEPG KeepEPGSubtitle MultiRegex MultiDetection CompareSubtitle CompareEpisode EpisodeIncrement Directory Category Genre Verbose LogFile DoubleIfLongerAs StartWithSeason Fallback);
my @Commandline		= qw(Title Subtitle EpisodeOverAll Summary Category Genre Date Time Channel TimeT Length DryRun ListsDir FormatPrefix Format FormatSpacer Verbose);
my @Numbers		= qw(Fuzzy SVDRPPort SVDRPTimeout EpisodeOverAll EpisodeIncrement Verbose DoubleIfLongerAs StartWithSeason);
my @Arrays		= qw(Subtitle EpisodeOverAll Season SeasonStart Extract);

my %Global_Tmp;
my %LangPart;

sub Init_Hashes {

  %SeriesData = ( Title		=> "",
                  ShortTitle	=> "",
                  Summary	=> "",
                  Subtitle	=> [()],
                  EpisodeOverAll=> [()],
                  Season	=> [()],
                  SeasonStart	=> [()],
                  Complete	=> 0,
                  Changes	=> 0,
                  Extract	=> [()],
                  Output	=> ""
                );

  %Global_Tmp = (	Unknown		=> 0,
                        Max		=> 0,
                        Lists		=> 0,
                        Links		=> 0,
                        StdErr		=> [()],
                        StdOut		=> [()],
                        Status		=> "",
                        Language	=> "de",
                        Translate	=> 0,
                  );

  $Config{ListLanguage} =~ s/\s//g;

  %LangPart = (  de	=> "Teil",
                 en	=> "Part",
               );

}

Init_Hashes();

sub Reset_Config {
  %Config = %Default_and_User_Config;
}
##################
# Main functions #
##################
sub Eplists_Init {
  if ( -e $Config{GlobalConf} ) {
    %Config = ParseConfig(-ConfigFile => $Config{GlobalConf}, -AutoTrue => 1, -MergeDuplicateOptions => 1, -LowerCaseNames => 0, -DefaultConfig => \%Config);
  }
  if ( -e (getpwuid($>))[7]."/.VDRSeriesTimer.conf" ) {
    %Config = ParseConfig(-ConfigFile => (getpwuid($>))[7]."/.VDRSeriesTimer.conf", -AutoTrue => 1, -MergeDuplicateOptions => 1, -LowerCaseNames => 0, -DefaultConfig => \%Config);
  }
  if ( $Config{UserConf} =~ /^~(.*)$/ ) {
    $Config{UserConf} = (getpwuid($>))[7].$1;
  }
  if ( -e $Config{UserConf} ) {
    %Config = ParseConfig(-ConfigFile => $Config{UserConf}, -AutoTrue => 1, -MergeDuplicateOptions => 1, -LowerCaseNames => 0, -DefaultConfig => \%Config);
  }

  $Config{ListsDir}		=~ s|/$||;
  $Config{SeriesConfsDir}	=~ s|/$||;

  if ( $Config{ListsDir} =~ /^~(.*)$/ ) {
    $Config{ListsDir} = (getpwuid($>))[7].$1;
  }
  if ( $Config{SeriesConfsDir} =~ /^~(.*)$/ ) {
    $Config{SeriesConfsDir} = (getpwuid($>))[7].$1;
  }
  if ( $Config{LogFile} =~ /^~(.*)$/ ) {
    $Config{LogFile} = (getpwuid($>))[7].$1;
  }

  while ( -l $Config{ListsDir} ) {
    $Config{ListsDir}		= readlink($Config{ListsDir});
  }
  while ( -l $Config{SeriesConfsDir} ) {
    $Config{SeriesConfsDir}	= readlink($Config{SeriesConfsDir});
  }

  $Config{ListsDir}		= $Config{ListsDir}."/"		if ( $Config{ListsDir} !~ m|/$| );
  $Config{SeriesConfsDir}	= $Config{SeriesConfsDir}."/"	if ( $Config{SeriesConfsDir} !~ m|/$| );

  %Default_and_User_Config = %Config;

  LoadAllListNames();
  LoadAllConfNames();
  LoadAllConf();

  $ModuleInitDone = 1;
}

sub Eplists_Search {

  my ($h_conf_ref) = @_;
  
  $t_Time = time();

  my $exit = 0;

  if ( defined $$h_conf_ref{UserConf} && $$h_conf_ref{UserConf} ) {
    if ( $$h_conf_ref{UserConf} =~ /^~(.*)$/ ) {
      $$h_conf_ref{UserConf} = (getpwuid($>))[7].$1; 
    }
    if ( -e $$h_conf_ref{UserConf} ) {
      $Config{UserConf} = $$h_conf_ref{UserConf};
    }
  }

  Eplists_Init() if ! $ModuleInitDone;

  foreach my $key ( keys %$h_conf_ref ) {
    $Config{$key} = $$h_conf_ref{$key} if ( $key ne "UserConf" );
  }

  epg_bugfixes();

  if ( ! ( ($Config{Title} && $Config{Subtitle}) || ($Config{Title} && $Config{EpisodeOverAll}) || ($Config{PrintLists} && $Config{ListsDir}) ) && ! exists $Config{PrintLists} && ! $Config{PrintOptions} ) {
    print_mess(__("Please provide Title and Subtitle or Title and EpisodeOverAll or PrintLists").".", 0, 1);
    my %Result = (  Result		=> "",
                    Title		=> $Config{Title},
                    Subtitle		=> $Config{Subtitle},
                    Bugfixes		=> 0,
                    BugfixesCode	=> $Config{BugfixesCode},
                    StdOut		=> $Global_Tmp{StdOut},
                    StdErr		=> $Global_Tmp{StdErr},
                    Stats		=> "",
                    Status		=> $Global_Tmp{Status},
                    MultiStatus		=> \$Global_Tmp{MultiStatus}
                  );
    Init_Hashes();
    Reset_Config();
    return \%Result;
  }

  SelectEpisodeList(); # Sets $Config{ListName}

  if ( ! $Config{PrintLists} && ! $Config{PrintOptions} && -d $Config{ListsDir} ) {
    if ( ! $Config{ListName} ) {
      $Global_Info{MissingEplists}{$Config{Title}}{count}++;
      $Global_Tmp{Status} = __"No matching list for this event found";
      $Global_Tmp{MultiStatus}[0] = $Global_Tmp{Status};
      my %Result = (  Result		=> "",
                      Title		=> $Config{Title},
                      Subtitle		=> $Config{Subtitle},
                      Bugfixes		=> 0,
                      BugfixesCode	=> $Config{BugfixesCode},
                      StdOut		=> $Global_Tmp{StdOut},
                      StdErr		=> $Global_Tmp{StdErr},
                      Stats		=> "",
                      Status		=> $Global_Tmp{Status},
                      MultiStatus	=> \$Global_Tmp{MultiStatus}
                    );
      Init_Hashes();
      Reset_Config();
      return \%Result;
    }
  }

  unless ( $Config{KeepEPGTitle} ) {
    my ($name) = $Config{ListName} =~ s/^(.*)\.[a-z][a-z]$//;
    $Config{Title} = $name;
  }

  if ( $Global_Tmp{WorkLanguage} =~ /\-/ ) {
    $Global_Tmp{Translate}++;
  }

  LoadListData($Config{ListName});
  InitSeriesData();
  SelectConf();

  # Read conf for the selected series
  if ( $Config{SeriesConfFile} && defined $AllConfLists{$Config{SeriesConfFile}} ) {
    %Config = ParseConfig(-ConfigFile => $AllConfLists{$Config{SeriesConfFile}}, -AutoTrue => 1, -MergeDuplicateOptions => 1, -LowerCaseNames => 0, -DefaultConfig => \%Config);
  }

  $LangPart{$Global_Tmp{Language}} = "en" if ! defined $LangPart{$Global_Tmp{Language}};

  PreWorker1();

  PrintOptionList();
  PrintEpisodeLists() if $Config{PrintLists};

  if ( $Config{PrintOptions} || $Config{PrintLists} || ! -d $Config{ListsDir} ) {
    print_mess(__("Please check ListsDir, directory doesn't exist")."!", 0, 1) if ! -d $Config{ListsDir};
    my %Result = (  Result		=> "",
                    Title		=> $Config{Title},
                    Subtitle		=> $Config{Subtitle},
                    Bugfixes		=> 0,
                    BugfixesCode	=> $Config{BugfixesCode},
                    StdOut		=> $Global_Tmp{StdOut},
                    StdErr		=> $Global_Tmp{StdErr},
                    Stats		=> "",
                    Status		=> $Global_Tmp{Status},
                    MultiStatus		=> \$Global_Tmp{MultiStatus}
                  );
    Init_Hashes();
    Reset_Config();
    return \%Result;
  }

  my $Result = MainWorker();
  Init_Hashes();
  Reset_Config();

  return $Result;
}

sub Eplists_Update {
}

##################
# Version / Date #
##################
sub Eplists_Version {
  return $VERSION;
}
sub Eplists_Date {
  return $DATE;
}

###############
# EPG Bufixes #
###############
sub epg_bugfixes {
  ##################################################################################
  #    1  Move stubtitle which is found in title to subtitle                       #
  #    2  If subtitle is found in title remove it from title                       #
  #    4  Remove episode number from title                                         #
  #    8  Remove episode number from subtitle                                      #
  #   16  Remove ", +Teil \d+" from title                                          #
  #   32  Remove " (\d+\/\d+\)" from title                                         #
  #   64  Remove "Folgen +(\d+) +- +(\d+)" from subtitle                           #
  #  128  If no subtitle but summary, take first sentence from summary as subtitle #
  #  256  Remove "(Subtitle, Part n)" from subtitle                                #
  ##################################################################################
  # 1
  if ( ! $Config{Subtitle} && $Config{Title} =~ /^(.*\w)\s*(:| -)\s+(\w.*?)$/ ) {
    $Config{Title}		= $1;
    $Config{Subtitle}		= $3;
    $Config{Bugfixes}++;
    $Config{BugfixesCode}	+= 1;
  # 2
  } elsif ( $Config{Subtitle} && $Config{Subtitle} =~ /^([\w ]{6,}).*?$/ ) {
    my $t = $1;
    if ( $Config{Title} =~ /(:| -) +($t).*$/ ) {
      $Config{Title}		=~ s/(:| -) +($t).*//;
      $Config{Bugfixes}++;
      $Config{BugfixesCode}	+= 2;
    }
  }
  # 4
  if ( $Config{EpisodeOverAll} && $Config{Title} =~ /\s+\((\d+)\)$/ ) {
    if ( $Config{EpisodeOverAll} == $1 ) {
      $Config{Title}		=~ s/\s+\(\d+\)$//;
      $Config{Bugfixes}++;
      $Config{BugfixesCode}	+= 4;
    }
  }
  # 8
  if ( $Config{Subtitle} && $Config{Subtitle} =~ /^(.*?),? (?:Folge|Part) \d+$/ ) {
    $Config{Subtitle}		= $1;
    $Config{Bugfixes}++;
    $Config{BugfixesCode}	+= 8;
  }
  # 16
  if ( $Config{Title} && $Config{Title} =~ /^(.*?), +(?:Teil|Part) \d+$/ ) {
    $Config{Title}		= $1;
    $Config{Bugfixes}++;
    $Config{BugfixesCode}	+= 16;
  }
  # 32
  if ( $Config{Title} && $Config{Title} =~ /^(.*?) +\((\d+)\/\d+\)$/ ) {
    $Config{Title}		= $1;
    $Config{EpisodeOverAll}	= $2;
    $Config{Bugfixes}++;
    $Config{BugfixesCode}	+= 32;
  }
  # 64
  if ( $Config{Subtitle} && $Config{Subtitle} =~ /Folgen +(\d+) +- +(\d+)$/ ) {
    if ( defined $1 && defined $2 ) {
      my ($t, $i);
      for ( $i=$1; $i <= $2; $i++ ) {
        $t .= sprintf("%i. n.n. / ", $i);
      }
      $t =~ s/ \/ $//;
      $Config{Subtitle}		= $t;
      $Config{Bugfixes}++;
      $Config{BugfixesCode}	+= 64;
    }
  }
  # 128
  if ( ! $Config{Subtitle} && $Config{Summary} =~ /^([\w\s]*?)\. / ) {
    $Config{Subtitle}		= $1;
    $Config{BugfixesCode}	+= 128;
  }
  # 256
  # Heimat (Home, Part 1) (1)
  if ( $Config{Subtitle} && $Config{Subtitle} =~ /^(.+?)\s+\(.+,\s+(?:Teil|Part)\s+\d+\)\s+\((\d+)\)$/ ) {
    $Config{Subtitle}		= $1 . ", " . $2;
    $Config{BugfixesCode}	+= 256;
  }  
}

###########################
# distance and GuessTitle #
###########################
sub StrDistance {
  my ($str1, $str2) = @_;
  return ( $str1 eq $str2 ) ? 0 : distance ($str1, $str2);
}

sub GuessTitle {
  my ($title, $h_ref, $ld_ref, $ldmax_ref) = @_;
  $$ld_ref = 255;
  my $found;
  $$ldmax_ref = int(length($title) * $Config{Fuzzy} + 0.5);
  if ( defined $$h_ref{$title} ) {
    $$ld_ref = 0;
    return $title
  }
  foreach my $key ( sort keys %$h_ref ) {
    my $ldtmp = StrDistance($title, $key);
    if ( ($ldtmp < $$ld_ref) && ($ldtmp <= $$ldmax_ref ) ) {
      $$ld_ref	= $ldtmp;
      $found	= $key;
      return $found if ! $$ld_ref;
    }
  }
  return $found ? $found : "";
}

######################################
# Subs for loading eplists and confs #
######################################
sub LoadAllListNames {
  if ( $Config{ListsDir} ) {
    my @EpisodeDir = glob $Config{ListsDir}."*episodes";
    foreach ( @EpisodeDir ) {
      my ($filename, $directories, $suffix) = fileparse($_, qr/\.episodes/);
      my $clean_Epsiode_list_name = $filename;
      if ( $clean_Epsiode_list_name !~ /\.[a-z][a-z]$/ ) {
        $clean_Epsiode_list_name .= ".de";
      }
      if ( not defined $AllEpisodeLists{$clean_Epsiode_list_name} ) {
        $AllEpisodeLists{$clean_Epsiode_list_name} = $_;
        $Global_Tmp{Lists}++;
        $Global_Tmp{Links}++ if ( -l $_ );
      }
    }
  }
}

# Load Files, listet in $a_list_ref into $a_data_ref
sub LoadListData {
  my ($name) = @_;
  return if defined $AllEpisodeData{$name};
  return if ! defined $AllEpisodeLists{$name};
  my @data;
  sysopen(FILE, $AllEpisodeLists{$name}, O_RDONLY) or die("Cannot open file ".$AllEpisodeLists{$name});
  flock(FILE, LOCK_SH);
  @data = <FILE>;
  close FILE;
  chomp(@data);

  if ( ! -l $AllEpisodeLists{$name} ) {
    $AllEpisodeData{$name}{Islink}       = 0;
    $AllEpisodeData{$name}{"Linktarget"} = "";
  } else {
    $AllEpisodeData{$name}{Islink}       = 1;
    my ($filename, $directories, $suffix)= fileparse(readlink($AllEpisodeLists{$name}), qr/\.episodes/);
    $AllEpisodeData{$name}{"Linktarget"} = $filename;
  }

  $AllEpisodeData{$name}{"data"}         = \@data; # Achtung, ref -> my

  foreach my $Line ( @{$AllEpisodeData{$name}{data}} ) {
    last if ($Line =~ /^\d/);
    if ( ! $AllEpisodeData{$name}{"Complete"} && $Line =~ /^#\s*COMPLETE/ ) {
      $AllEpisodeData{$name}{"Complete"} = 1;
      next;
    }
    if ( ! $Config{ShortTitle} && $Line =~ /^#(short|\s*SHORT(TITLE)?)\s\s*(.*?)\s*$/i ) {
      $AllEpisodeData{$name}{"ShortTitle"} = $3;
      $AllEpisodeData{$name}{"ShortTitle"} =~ tr/_/ /;
      next;
    }
    if ( $Line =~ /^#\s*SEASONLIST/ .. $Line =~ /^#\s*\/SEASONLIST/ ) {
      if ( $Line =~ /^#\s*(\d+)\s*\t\s*(\d+)\s*\t\s*(\d+)\s*$/ ) {
        my %Result = ( SeasonStart => $2, SeasonStop => $3 );
        $AllEpisodeData{$name}{SeasonList}{$1} = ( \%Result );
      }
    }
  }
  $AllEpisodeData{$name}{"Complete"}	= 0	if ! $AllEpisodeData{$name}{"Complete"};
  $AllEpisodeData{$name}{"ShortTitle"}	= ""	if ! $AllEpisodeData{$name}{"ShortTitle"};
}
   
sub PrintEpisodeLists {
  if ( $Config{ListsDir} && keys %AllEpisodeLists ) {
    # print lists found
    my $i;
    if ( $Config{PrintLists} ) {
      print_mess ( "List of episode lists found in <".$Config{ListsDir}.">:\n" );
      foreach my $key ( sort keys %AllEpisodeLists ) {
        print_mess ( "- ", 1 ) if ! exists $Config{PrintLists};
        if ( -l $AllEpisodeLists{$key} ) {
          my $linktarget = readlink($AllEpisodeLists{$key});
          $linktarget =~ s/\.episodes//;
          print_mess ( "*$key -> ".$linktarget );
          $i++;
        } else {
          print_mess ( " $key" );
        }
      }
      print_mess("\n". __("Lists found") .": ".( (keys %AllEpisodeLists) - $i)." ($i links)\n");
      return 1;
    }
    return 0;
  }
}

sub SelectEpisodeList {
  return if ( $Config{PrintLists} || $Config{PrintOptions} );
  if ( keys %AllEpisodeLists ) {
    # Select an episode list
    my ($ld, $ldmax, $found_title);
    my @languages = split(/,/, $Config{ListLanguage});
    my $tmp_ld = 255;
    foreach my $language ( @languages ) {
      my $lang = $language;
      $lang = $1 if ( ! $Global_Tmp{Translate} && $lang =~ /^([a-z][a-z])\-[a-z][a-z]$/ );
      $lang = $1 if (   $Global_Tmp{Translate} && $lang =~ /^[a-z][a-z]\-([a-z][a-z])$/ );
      $found_title = GuessTitle($Config{Title}.".".$lang, \%AllEpisodeLists, \$ld, \$ldmax);
      if ( $found_title && defined $AllEpisodeLists{$found_title} && $ld < $tmp_ld ) {
        $tmp_ld = $ld;
        $Config{ListName}		= $found_title;
        $Global_Tmp{WorkLanguage}	= $language;
        $Global_Tmp{Language}		= $lang;
        last if $ld == 0;
      }
    }
    if ( $Config{ListName} ) {
      print_mess (__("File")." <".$Config{ListName}."> ".__("is like title")." <".$Config{Title}.".".$Global_Tmp{Language}."> ".__("with a distance from")." $ld (max $ldmax),\n".__("using file")." <".$AllEpisodeLists{$Config{ListName}}.">\n\n");
      return $Config{ListName};
    } else {
      print_mess (__("No episode lists found for")." \"".$Config{Title}.".".$Global_Tmp{Language}."\"!\n\n");
      return 0;
    }

  }
}

sub LoadAllConfNames {
  if ( $Config{SeriesConfsDir} ) {
    my @SeriesConfDir = glob $Config{SeriesConfsDir}."*conf";
    foreach ( @SeriesConfDir ) {
      my ($filename, $directories, $suffix) = fileparse($_, qr/\.conf/);
      my $clean_Conf_name = $filename;
      if ( not defined $AllConfLists{$clean_Conf_name} ) {
        $AllConfLists{$clean_Conf_name} = $_;
      }
    }
  }
}

sub SelectConf {
  my ($filename, $directories, $suffix) = fileparse($Config{ListName}, qr/\.[^.]*/);
  my ($ld, $ldmax);
  my $title = GuessTitle($filename, \%AllConfLists, \$ld, \$ldmax);
  if ( $title && defined $AllConfLists{$title} ) {
    $Config{SeriesConfFile} = $title;
  }
}

# Load Files, listet in $a_list_ref into $a_data_ref
sub LoadAllConf {
  my %config;
  foreach my $name ( keys %AllConfLists ) {
    if ( ! -l $AllConfLists{$name} ) {
      %config = ParseConfig(-ConfigFile => $AllConfLists{$name}, -AutoTrue => 1, -MergeDuplicateOptions => 1, -LowerCaseNames => 0);
      $AllConfData{$name}{Islink}	= 0;
      $AllConfData{$name}{"Linktarget"}	= "";
    } else {
      $AllConfData{$name}{Islink}	= 1;
      $AllConfData{$name}{"Linktarget"}	= readlink($AllConfLists{$name});
    }
    $AllConfData{$name}{"data"}		= \%config; # Achtung, ref -> my
  }
}

#########################################
# First assignment of event information #
#########################################
sub InitSeriesData {
  $SeriesData{Title}			= $Config{Title};
  $SeriesData{Subtitle}[0]		= $Config{Subtitle};
  $SeriesData{EpisodeOverAll}[0]	= $Config{EpisodeOverAll};
  $SeriesData{Category}			= $Config{Category};
  $SeriesData{Genre}			= $Config{Genre};
}

sub PreWorker1 {
  #################################
  # Double the subtitle if needed #
  #################################
  sub DoubleSubtitle {
    if ( $Config{DoubleIfLongerAs} && $Config{Length} && $Config{Length} > $Config{DoubleIfLongerAs} * 60 ) {
      $Config{Subtitle} = "0. $Config{Subtitle} / 0. n.n.";
    }
  }

  DoubleSubtitle();

  ########################################################
  # detect multi episodes (like Stargate / 24 on RTL II) #
  ########################################################
  if ( $Config{Subtitle} ) {
    my $i = 0;
    if ( $Config{MultiDetection} && $Config{MultiRegex} ) {
      while ( $Config{Subtitle} =~ m/$Config{MultiRegex}/g && $2 ) {
        $SeriesData{EpisodeOverAll}[$i]	= $1 ? $1 : 0;
        $SeriesData{EpisodeOverAll}[$i] = $Config{EpisodeOverAll} + $i if ( $Config{DoubleIfLongerAs} && $Config{Length} && $Config{Length} > $Config{DoubleIfLongerAs} * 60 );
        $SeriesData{Subtitle}[$i]	= $2;
        $SeriesData{Season}[$i]		= 0;
        $SeriesData{SeasonStart}[$i]	= 0;
        $SeriesData{Extract}[$i]	= 0;
        $i++;
      }
    }
    if ( $i <= 1 ) {
      $SeriesData{EpisodeOverAll}[0]	= $Config{EpisodeOverAll};
      $SeriesData{Subtitle}[0]		= $Config{Subtitle};
      $SeriesData{Season}[0]		= 0;
      $SeriesData{SeasonStart}[0]	= 0;
      $SeriesData{Extract}[0]		= 0;
    }
  }

  ######################################
  # Increment EpisodeOverAll if needed #
  ######################################
  if ( $Config{EpisodeIncrement} ) {
    for (my $i = 0; $i < scalar @{$SeriesData{EpisodeOverAll}}; $i++) {
      $SeriesData{EpisodeOverAll}[$i]	+= $Config{EpisodeIncrement} if $SeriesData{EpisodeOverAll}[$i];
    }
  }

}

#####################################
# Generate option list and print it #
#####################################
sub PrintOptionList {
  if ( $Config{PrintOptions} || $Config{Verbose} ge 2 ) {
    print_mess("Hash \%Config:\n-------------\n", 1);
    my ($output, $spaces);
    foreach (sort keys %Config) {
      my $key	 = $_;
      my $value	 = $Config{$key};
      $value  	 =~ s/^0$/no/ if ! grep($_ eq $key, @Numbers);
      $value 	 =~ s/^1$/yes/ if ! grep($_ eq $key, @Numbers);
      $output	 = $key;
      $spaces	 = "";
      if ( grep($_ eq $key, @Globals) ) {
        $output	.= "¹";
        $spaces .= " ";
      }
      if ( grep($_ eq $key, @PerList) ) {
        $output	.= "²";
        $spaces .= " ";
      }
      if ( grep($_ eq $key, @Commandline) ) {
        $output	.= "³";
        $spaces .= " ";
      }
      print_mess(sprintf("%-20s%s => %s\n", $output, $spaces, $value), 1);
    }
    print_mess(sprintf("\n¹ ".__("Changeable in")." \"%s\"\n", $Config{UserConf}), 1);
    print_mess(sprintf("² ".__("Changeable in")." %s\n", ($Config{SeriesConfsDir} && $Config{ListName} )? "\"".$Config{SeriesConfsDir}.$Config{ListName}.".conf"."\"" : __("episodes list configuration file")), 1);
    print_mess("³ ".__("Changeable on commandline")."\n", 1);

    print_mess("\nHash \%SeriesData:\n-----------------\n", 1);
    foreach (sort keys %SeriesData) {
      my $key = $_;
      if ( grep($_ eq $key, @Arrays) ) {
        for (my $i = 0; $i < scalar @{$SeriesData{$key}}; $i++) {
          print_mess(sprintf ("%-20s => %s\n", $_."[$i]", $SeriesData{$key}[$i]), 1);
        }
        if ( ! scalar @{$SeriesData{$key}} ) {
          print_mess(sprintf ("%-20s => %s\n", $_."[0]", ""), 1);
        }
        next;
      }
      print_mess(sprintf ("%-20s => %s\n", $_, $SeriesData{$key}), 1);
    }

    print_mess("\nHash \%Global_Tmp:\n-----------------\n", 1);
    foreach (sort keys %Global_Tmp) {
      my $key = $_;
      if ( grep($_ eq $key, @Arrays) ) {
        for (my $i = 0; $i < scalar @{$Global_Tmp{$key}}; $i++) {
          print_mess(sprintf ("%-20s => %s\n", $_."[$i]", $Global_Tmp{$key}[$i]), 1);
        }
        if ( ! scalar @{$Global_Tmp{$key}} ) {
          print_mess(sprintf ("%-20s => %s\n", $_."[0]", ""), 1);
        }
        next;
      }
      print_mess(sprintf ("%-20s => %s\n", $_, $Global_Tmp{$key}), 1);
    }

    print_mess(sprintf ("\n"));
    return 1 if $Config{PrintOptions};
  }
}

sub MainWorker {
  ##################
  # Main Variables #
  ##################
  my $Line = "";
  my @field; 
  my %Result;
  my ($Seasonlist, $unknown) = qw(0 0);

# TODO ?
  #############################################################
  # If $File not exist print only the Path with clean $Title. #
  #############################################################

  ####################
  # Preselect action #
  ####################
  # always search episode if no episode number is known
  if (! $SeriesData{EpisodeOverAll}[0] && $SeriesData{Subtitle}[0] && ! $#{$SeriesData{EpisodeOverAll}}) {
    $Config{CompareSubtitle} = 1;
    print_mess (__("No episode number found, try searching by comparing subtitle").".\n\n");
  }

  # always search episode by subtitle if only episode number is known
  if ($SeriesData{EpisodeOverAll}[0] && ! $SeriesData{Subtitle}[0] && ! $#{$SeriesData{EpisodeOverAll}}) {
    $Config{CompareEpisode} = 1;
    print_mess (__("Only episode number found, try searching by comparing episode number").".\n\n");
  }

  ###############################
  # Set SHORTTITLE and COMPLETE #
  ###############################
  if ( $Config{ShortTitle} ) {
    $SeriesData{ShortTitle}	= $Config{ShortTitle}	if $Config{ShortTitle};
  } else {
    $SeriesData{ShortTitle}	= $AllEpisodeData{$Config{ListName}}{"ShortTitle"} if $AllEpisodeData{$Config{ListName}}{"ShortTitle"};
  }
  if ( $Config{Complete} ) {
    $SeriesData{Complete}	= $Config{Complete}	if $Config{Complete};
  } else {
    $SeriesData{Complete}	= $AllEpisodeData{$Config{ListName}}{Complete} if $AllEpisodeData{$Config{ListName}}{Complete};
  }

  for (my $i = 0; $i < scalar @{$SeriesData{EpisodeOverAll}}; $i++) {
    if ( $SeriesData{EpisodeOverAll}[$i] ) {
      my %Result = ();
      foreach (sort keys %{$AllEpisodeData{$Config{ListName}}{SeasonList}} ) {
        if (	$SeriesData{EpisodeOverAll}[$i] >= $AllEpisodeData{$Config{ListName}}{SeasonList}{$_}{SeasonStart}
             && $SeriesData{EpisodeOverAll}[$i] <= $AllEpisodeData{$Config{ListName}}{SeasonList}{$_}{SeasonStop} ) {

          %Result = ( Season => $_, SeasonStart => $AllEpisodeData{$Config{ListName}}{SeasonList}{$_}{SeasonStart} );
          last;
        }
      }
      if ( $Result{Season} && $Result{Season} < 10 ) {
        $Result{Season} = sprintf("%02i", $Result{Season} + 0);
      }
      $SeriesData{Season}[$i]		= $Result{Season};
      $SeriesData{SeasonStart}[$i]	= $Result{SeasonStart};
    }
  }

  # SearchSubtitleSub

  if ( $Config{CompareSubtitle} ) {
    for (my $i = 0; $i < scalar @{$SeriesData{EpisodeOverAll}}; $i++) {
      %Result = ();
      if ( $i >= 1 && $SeriesData{Subtitle}[$i] eq "n.n." ) {
        $SeriesData{EpisodeOverAll}[$i] = $SeriesData{EpisodeOverAll}[$i-1] + 1;
      } else {
        %Result = SearchSubtitleSub($SeriesData{Subtitle}[$i], $SeriesData{EpisodeOverAll}[$i]);
        if ( $Result{Season} && $Result{Season} =~ /^\d+$/ ) {
          $Result{Season} = sprintf("%02i", $Result{Season} + 0);
        }
        $SeriesData{Season}[$i]			= $Result{Season}		if $Result{Season};
        $SeriesData{Episode}[$i]		= $Result{Episode}		if $Result{Episode};
        $SeriesData{EpisodeOverAll}[$i]		= $Result{EpisodeOverAll}	if $Result{EpisodeOverAll};
        $SeriesData{Subtitle}[$i]		= $Result{Subtitle}		if $Result{Subtitle};
        $SeriesData{Extract}[$i]		= $Result{Extract}		if $Result{Extract};
        if ( ! $Result{Extract} && ! $Config{Fallback} ) {
          $SeriesData{Episode}[$i]		= 0;
          $SeriesData{EpisodeOverAll}[$i]	= 0;
          $SeriesData{Season}[$i]		= 0;
        }
      }
    }
    %Result = ();
  }

  # SearchEpisodeSub

  if ( $Config{CompareEpisode} ) {
    for (my $i = 0; $i < scalar @{$SeriesData{EpisodeOverAll}}; $i++) {
      if ( $SeriesData{EpisodeOverAll}[$i] ) {
        %Result = ();
        %Result = SearchEpisodeSub($SeriesData{EpisodeOverAll}[$i]);
        if ( $Result{Season} && $Result{Season} =~ /^\d+$/ ) {
          $Result{Season} = sprintf("%02i", $Result{Season} + 0);
        }
        $SeriesData{Season}[$i]			= $Result{Season}		if $Result{Season};
        $SeriesData{Episode}[$i]		= $Result{Episode}		if $Result{Episode};
        $SeriesData{EpisodeOverAll}[$i]		= $Result{EpisodeOverAll}	if $Result{EpisodeOverAll};
        $SeriesData{Subtitle}[$i]		= $Result{Subtitle}		if $Result{Subtitle};
        $SeriesData{Extract}[$i]		= $Result{Extract}		if $Result{Extract};
      }
    }
    %Result = ();
  }

  # UpdateEpisodeList

  if ( ! $Config{DryRun} ) {
    for (my $i = 0; $i < scalar @{$SeriesData{EpisodeOverAll}}; $i++) {
      if ( $SeriesData{Season}[$i] && $SeriesData{SeasonStart}[$i] && $SeriesData{EpisodeOverAll}[$i] && $SeriesData{Title} && $SeriesData{Subtitle}[$i] ) {
        my %DataTmp = (  Season		=> $SeriesData{Season}[$i],
                         SeasonStart	=> $SeriesData{SeasonStart}[$i],
                         EpisodeOverAll	=> $SeriesData{EpisodeOverAll}[$i],
                         Subtitle	=> $SeriesData{Subtitle}[$i],
                      );
        UpdateEpisodeList(\%DataTmp);
        undef(%DataTmp);
      }
    }
  }

  UpdateListFileOnDisk();

  CreateOutputString();

  return GenerateResult();

} # End MainWorker
  
########################################
# Search episode by comparing subtitle #
########################################
sub SearchSubtitleSub {
  my ($Subtitle, $EpisodeOverAll) = @_;
  my ($ldt, $ldtmp, $run) = qw(255 255 0);
  my ($st, $se, $EpSub) = ("", "", "");
  my %Result = ( run => 1, diff => 255 , Extract => 0 );
  my %LineField;
  #for ( my $k=0; $k <= $CleanSubRegex[$#CleanSubRegex][0]; $k++ ) {
  for ( my $k=0; $k <= 3; $k++ ) {
    $run++;
    my $SubtitleOrg	= $Subtitle;
    $Subtitle		= cleansub($Subtitle, $k) if $k;
    my $ldmax		= int(length($Subtitle) * $Config{Fuzzy} + 0.5);
    "1\t1\t1\tmuster" =~ /^\s*(\d+)\s*\t\s*(\d+)\s*\t\s*(\d+)\s*\t\s*(.*?)\s*(\t|$)/;
    foreach my $Line ( @{$AllEpisodeData{$Config{ListName}}{data}} ) {
      if ( $Line =~ // ) {
        %LineField = ( Season => $1, Episode => $2, EpisodeOverAll => $3, Subtitle => $4 );
        next	if ( $LineField{Season} < $Config{StartWithSeason} );
        $EpSub = $LineField{Subtitle};
        $EpSub = cleansub($LineField{Subtitle}, $k) if $k;
        if ( abs(length($Subtitle) - length($EpSub)) > $ldmax ) {
          $ldtmp = abs(length($Subtitle) - length($EpSub));
        } else {
          $ldtmp = StrDistance ($Subtitle, $EpSub);
        }
        $ldtmp += 1 if ( $EpisodeOverAll && ( $EpisodeOverAll != $LineField{Episode} && $EpisodeOverAll != $LineField{EpisodeOverAll} ) );
        if ( ($ldtmp < $Result{diff}) && ($ldtmp <= $ldmax + (($EpisodeOverAll && $EpisodeOverAll != $LineField{EpisodeOverAll}) ? 1 : 0)) ) {
          %Result	= (	run		=> $run,
                                diff		=> $ldtmp,
                                Season		=> $LineField{Season},
                                Episode		=> $LineField{Episode},
                                EpisodeOverAll	=> $LineField{EpisodeOverAll},
                                Subtitle	=> $Config{KeepEPGSubtitle} ? $SubtitleOrg : $LineField{Subtitle},
                                Extract		=> 1
                          );
          print_mess (__("Run")." $Result{run}: ".__("Found title")." \"$LineField{Subtitle}\", ".__("changed to")." \"$EpSub\",\n".
                      "       ".__("is like the given")." \"$SubtitleOrg\", ".__("changed to")." \"$Subtitle\",\n".
                      "       ".__("with a distance from")." $Result{diff} (max ".($ldmax + (($EpisodeOverAll && $EpisodeOverAll != $LineField{EpisodeOverAll}) ? 1 : 0))."), ".__("so using")."\n".
                      "       <$Result{Season} $Result{Episode} $Result{EpisodeOverAll} $Result{Subtitle}>\n\n");
          last unless $Result{diff};
        } elsif ( ($ldtmp < $Result{diff}) && ($ldtmp < $ldt) ) {
          $ldt = $ldtmp;
          $st = $LineField{Subtitle};
          $se = $EpSub;
        }
      }
    }
    print_mess (__("Run")." $Result{run}: ".__("Warning").": \"$st\", ".__("changed to")." \"$se\",\n".
                "       ".__("is like the given")." \"$SubtitleOrg\", ".__("changed to")." \"$Subtitle\",\n".
                "       ".__("with a distance from")." $ldt, ".__("but max distance is")." $ldmax!\n\n") if ! $Result{Extract};
    $Result{run}++;
  }
  return %Result;
}

###############################################
# Search subtitle by comparing EpisodeOverAll #
###############################################
sub SearchEpisodeSub {
  my ($EpisodeOverAll) = @_;
  my %LineField;
  my %Result = ( Extract => 0 );
  "1\t1\t1\tmuster" =~ /^\s*(\d+)\s*\t\s*(\d+)\s*\t\s*(\d+)\s*\t\s*(.*?)\s*(\t|$)/;
  foreach my $Line ( @{$AllEpisodeData{$Config{ListName}}{data}} ) {
    if ( $Line =~ // ) {
      %LineField = ( Season => $1, Episode => $2, EpisodeOverAll => $3, Subtitle => $4 );
      next if ( $LineField{EpisodeOverAll} != $EpisodeOverAll );
      %Result	= (	Season		=> $LineField{Season},
                        Episode		=> $LineField{Episode},
                        EpisodeOverAll	=> $LineField{EpisodeOverAll},
                        Subtitle	=> $LineField{Subtitle},
                        Extract		=> 1
                   );
      print_mess (__("Found title")." \"$LineField{EpisodeOverAll}\" ".__("is equal to the given")." \"$EpisodeOverAll\", ".__("so using")."\n".
                  "<$Result{Season} $Result{Episode} $Result{EpisodeOverAll} $Result{Subtitle}>\n\n");
      last;
    }
  }
  return %Result;
}

#######################################
# OFF
# Write new episode data if not there #
#######################################
sub UpdateEpisodeList {
  my ($hash_ref) = @_;
  my %Data = %$hash_ref;
  my %LineField;
  my ($found, $insert, $replace, $i) = qw(0 0 0 0);
  my ($ld, $ldtmp) = qw(255 255);
  my $SubtitleOrg = $Data{Subtitle};
  my $ldmax = int(length($SubtitleOrg) * $Config{Fuzzy} + 0.5);
  "1\t1\t1\tmuster" =~ /^\s*(\d+)\s*\t\s*(\d+)\s*\t\s*(\d+)\s*\t\s*(.*?)\s*(\t|$)/;
  foreach my $Line ( @{$AllEpisodeData{$Config{ListName}}{data}} ) {
    if ( $Line =~ // ) {
      %LineField = ( Season => $1, Episode => $2, EpisodeOverAll => $3, Subtitle => $4 );
      $ldtmp = StrDistance ($SubtitleOrg, $LineField{Subtitle});
      if ( ! $ld || ($ldtmp < $ld) && ($ldtmp <= $ldmax) ) {
        $ld = $ldtmp;
        $found = 1 if ( $Data{Season} == $LineField{Season} && $Data{EpisodeOverAll} == $LineField{EpisodeOverAll} );
        if ( $found ) {
          print_mess ("\"$LineField{Subtitle}\" ".__("is like")." \"$SubtitleOrg\" ".__("with a distance from")." $ld (max $ldmax), ".__("so using")." \"$LineField{Subtitle}\" ".__("as subtitle").".\n\n");
          $Data{Subtitle} = $LineField{Subtitle};
          last unless $ld;
        }
      }
      if ( ! $replace && $Data{Season} == $LineField{Season} && ($Data{EpisodeOverAll} - $Data{SeasonStart} + 1) == $LineField{Episode} && $LineField{Subtitle} eq 'n.n.' ) {
        $replace = $i;
      } elsif ( ! $replace && $Data{Season} > $LineField{Season} ) {
        $insert = $i + 1;
      } elsif ( ! $replace && $Data{Season} == $LineField{Season} && ($Data{EpisodeOverAll} - $Data{SeasonStart} + 1) > $LineField{Episode} ) {
        $insert = $i + 1;
      }
    }
    $i++;
  }
  $insert = $i if ! $insert;
  if ( ! $found && $insert && ! $replace && ($Data{EpisodeOverAll} - $Data{SeasonStart} + 1) > 0 && ${$AllEpisodeData{$Config{ListName}}{data}}[$insert] =~ /^\s*(\d+)\s*\t\s*(\d+)\s*\t\s*(\d+)\s*\t\s*n\.n\.\s*(\t|$)/ ) {
    splice (@{$AllEpisodeData{$Config{ListName}}{data}}, $insert, 0, sprintf("%02i\t%i\t%i\t%s", $Data{Season}, ($Data{EpisodeOverAll} - $Data{SeasonStart} + 1), $Data{EpisodeOverAll}, $Data{Subtitle}));
    $SeriesData{Changes}++;
  } elsif ( ! $found && $replace && ($Data{EpisodeOverAll} - $Data{SeasonStart} + 1) > 0 ) {
    splice (@{$AllEpisodeData{$Config{ListName}}{data}}, $replace, 1, sprintf("%02i\t%i\t%i\t%s", $Data{Season}, ($Data{EpisodeOverAll} - $Data{SeasonStart} + 1), $Data{EpisodeOverAll}, $Data{Subtitle}));
    $SeriesData{Changes}++;
  }
}

##################################################################
# Write new episodes in file if wanted and file is not protected #
##################################################################
sub UpdateListFileOnDisk {
  print_mess ("") if $Config{Verbose};
  my $file = $AllEpisodeLists{$Config{ListName}};
  my $target = $file;
  if ( $SeriesData{Changes} && ! $SeriesData{Complete} && ! $Config{Protected} ) {
    print_mess (__("Writing changed episodes list to file")." ".$file);
    sysopen(FILE, $file.'.$$$', O_RDWR | O_CREAT | O_TRUNC, 0644) or die(__("Cannot open file")." ".$file.'.$$$');
    flock(FILE, LOCK_SH);
    print FILE join("\n", @{$AllEpisodeData{$Config{ListName}}{data}}), "\n";
    close FILE;
    if ( $AllEpisodeData{$Config{ListName}}{"Islink"} ) {
      $target = $AllEpisodeLists{$AllEpisodeData{$Config{ListName}}{"Linktarget"}};
    }
    rename($file.'.$$$', $target) || die("Cannot rename ".$file.'$$$'." to ".$target.": ".$!);
  } elsif ( $SeriesData{Changes} && ( $SeriesData{Complete} || $Config{Protected} ) ) {
    print_mess (__("Cannot write changes of episodes list to file")." \"".$file."\". ".__("File is COMPLETE or PROTECTED").".");
  }
}

#######################
# Create ouput string #
#######################
sub CreateOutputString {
  my %OutputData	= ( Prefix	=> [()],
                            Data	=> [()]
                          );
                                      
  $OutputData{Prefix}[0] = $Config{FormatPrefix};

  for (my $i = 0; $i < scalar @{$SeriesData{EpisodeOverAll}}; $i++) {
    $OutputData{Data}[$i] = $Config{Format};
  }

  foreach my $key ( keys %OutputData ) {
    for (my $i = 0; $i < scalar @{$OutputData{$key}}; $i++) {
      my %FormatM = ( "s" => 2, "e" => 2, "n" => 3 );
      foreach("s", "e", "n") {
        $FormatM{$_} = $1 if ( $OutputData{$key}[$i] =~ s/%([0-9]+)$_/%$_/ );
      }

      $SeriesData{MultiSubtitle}[$i] = $SeriesData{Subtitle}[$i] ? $SeriesData{Subtitle}[$i] : "n.n.";

      my %FormatHash = (  '%T' => ( $Config{UseShortTitle} && $SeriesData{ShortTitle} ) ? $SeriesData{ShortTitle} : $SeriesData{Title},
                          '%S' => $SeriesData{Subtitle}[$i] ? $SeriesData{Subtitle}[$i] : "n.n.",
                          '%D' => $Config{Directory},
                          '%C' => $Config{Category},
                          '%G' => $Config{Genre},
                          '%s' => sprintf("%0".$FormatM{"s"}."i", $SeriesData{Season}[$i] ? $SeriesData{Season}[$i] : 0),
                          '%e' => sprintf("%0".$FormatM{"e"}."i", $SeriesData{Episode}[$i] ? $SeriesData{Episode}[$i] : ($SeriesData{SeasonStart}[$i] ? ($SeriesData{EpisodeOverAll}[$i] - $SeriesData{SeasonStart}[$i] + 1) : 0) ),
                          '%n' => sprintf("%0".$FormatM{"n"}."i", $SeriesData{EpisodeOverAll}[$i] ? $SeriesData{EpisodeOverAll}[$i] : 0)
                  );

      if ( ! $SeriesData{EpisodeOverAll}[$i] ) {
        $FormatHash{'%s'} = $SeriesData{Season}[$i] ? $SeriesData{Season}[$i] : "?";
        $FormatHash{'%e'} = $SeriesData{Episode}[$i] ? $SeriesData{Episode}[$i] : "?";
        $FormatHash{'%n'} = $SeriesData{EpisodeOverAll}[$i] ? $SeriesData{EpisodeOverAll}[$i] : "?";
        $Global_Tmp{Unknown} = 1;
        $Global_Tmp{Status}  = __"Cannot find episode title in episode list";
        $Global_Tmp{MultiStatus}[$i] = __"Cannot find episode title in episode list";
      }

      foreach ('%D', '%C', '%G', '%T', '%S', '%s', '%e', '%n') {
        $OutputData{$key}[$i] =~ s/$_/$FormatHash{$_}/ if ( $OutputData{$key}[$i]  =~ /$_/ );
        $OutputData{$key}[$i] =~ s/~{2,}/~/ if ($_ eq '%G');
      }

      $SeriesData{Output} .= $OutputData{$key}[$i];
      $SeriesData{Output} .= $Config{FormatSpacer}	if ( $key eq 'Data' && $i < (scalar @{$OutputData{$key}}) - 1 );
      
      $SeriesData{MultiOutput}[$i]  = $OutputData{$key}[$i];
    }
  }

  $SeriesData{Output} =~ s/^~+//;
}

###################
# Generate Result #
###################
sub GenerateResult {

  chomp $SeriesData{Output};

  $SeriesData{Output} =~ s/(?<!\\)\\n/\n/g;
  $SeriesData{Output} =~ s/\\\\n/\\n/g;

  my $stats; #  = "Script ".basename($0)." info: Start ".$t_Time."; End: ".time()."; Runs for ".(time() - $t_Time)."s; Changes count: ".$SeriesData{Changes}."; Title: <".$Config{Title}.">; Subtitle: <".$Config{Subtitle}.">\n";

  my %Result = ( Result		=> $SeriesData{Output},
                 MultiResult	=> \$SeriesData{MultiOutput},
                 Title		=> $Config{Title},
                 Subtitle	=> $Config{Subtitle},
                 MultiSubtitle	=> \$SeriesData{MultiSubtitle},
                 Bugfixes	=> $Config{Bugfixes},
                 BugfixesCode	=> $Config{BugfixesCode},
                 StdOut		=> $Global_Tmp{StdOut},
                 StdErr		=> $Global_Tmp{StdErr},
                 Stats		=> $stats,
                 Status		=> $Global_Tmp{Status},
                 MultiStatus	=> \$Global_Tmp{MultiStatus}
               );

  return \%Result;

}

########
#      #
########

# Store messages
sub print_mess {
  my ( $msg, $nl, $always, $onlylog ) = @_;
  chomp($msg);

  $Global_Tmp{StdErr}[$#{$Global_Tmp{StdErr}}+1] = $msg	if ( $Config{Verbose} || exists $Config{PrintLists} || $Config{PrintOptions} || $always );
}

# Clean subtitle
sub cleansub {
  my ($tmp, $level) = @_;
  if ( $level == 1 ) {
    $tmp =~ s/(\s+\-)?\s+\((\d)\)$/$2/;
    $tmp =~ s/(,?|\s\-)\s+$LangPart{$Global_Tmp{Language}} (\d)$/$2/;
    $tmp =~ s/(,?|\s\-)\s+$LangPart{$Global_Tmp{Language}} (\d) (?:von|of) \d$/$2/;
    $tmp =~ s/(,|\s-)?\s+\($LangPart{$Global_Tmp{Language}} (\d)\)$/$2/;
  }
  if ( $level == 2 ) {
    $tmp =~ s/^Tag \d+: //;
    $tmp =~ s/\s*Uhr\s*//g;
    $tmp =~ s/\s*-\s*/-/g;
  }
  if ( $level == 3 ) {
    $tmp =~ s/ \d$//;
    $tmp =~ s/\s*\(.*\)$//;
  }
  return $tmp;
}

# OFF
# Cleanup and print messages before exit
sub cleanup {
  my ($tmp, $str) = @_;
}

1;

=head1 NAME

Eplists - Eplists Module

=head1 SYNOPSIS

 #
 # the procedural way
 use Eplists qw(Eplists_Version Eplists_Date Eplists_Update Eplists_Search);

 my $Result_ref = Eplists_Search( {  Title           => $$title_ref,
                                     Subtitle        => $$subtitle_ref,
                                     EpisodeOverAll  => $episode,
                                     FormatPrefix    => "",
                                     Format          => "S%sE%eN%n",
                                     Verbose         => 0,
                                     Fallback        => 0,
                                     Protected       => 1
                                  }
                                );
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                

=cut
         
__END__
