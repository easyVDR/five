#!/usr/bin/perl
#
# svdrpsend-ng.pl
#
# Extended version to communicate with eplist server
# at eplists.constabel.net port 2006
#
# This version should be fully compatible with the original one.
#
my $Version		= "0.3.0-beta4";
my $Date		= "2008-06-10";
my $ClientInfo		= "svdrpsend-ng.pl v$Version $Date";
#
# Mike Constabel <eplists (at) constabel dot net>
#

###########
# Options #
###########

my $UseGzip		= 1;
my $UseSSL		= 1;

############
# /Options #
############

require 5.8.0;  #maybe

use strict 'refs';
use warnings;

BEGIN {
  my($error, @Modules, $mod);
  @Modules = qw(Unicode::String FindBin Getopt::Long Pod::Usage EplistsClient Locale::TextDomain Locale::Messages POSIX);
  for $mod (@Modules) {
    if (eval "require $mod") {
      if ( $mod eq "Locale::TextDomain" ) {
        $mod->import('eplists', "/usr/share/locale", "/usr/local/share/locale");
#      } elsif ( $mod eq "Locale::Messages" ) {
#        $mod->import("LC_MESSAGES");
      } elsif ( $mod eq "FindBin" ) {
        $mod->import;
        push @INC, "$FindBin::Bin";
      } elsif ( $mod eq "POSIX" ) {
        $mod->import("locale_h", "setlocale");
      } else {
        $mod->import;
      }
    } else {
      $error = 1;
      print STDERR "Module $mod not found, please install $mod\n";
      print STDERR "On Debian:\naptitude install perl-modules libunicode-string-perl libintl-perl\n";
    }
  }
  exit 1 if $error;
}

setlocale (LC_MESSAGES, '');

###########
# Getopts #
###########
my $locale = (setlocale(LC_ALL) =~ /UTF/i ? "utf8" : "latin1");

my %Config = (	Host		=> "localhost",
                Port		=> 2001,
                Username	=> "",
                Password	=> "",
                FileOrDir	=> "",
                Data		=> "",
                Clean		=> 0,
                Deref		=> 0,
                UserEncoding	=> $locale,
                Help		=> 0,
                Version		=> 0,
                UseGzip		=> $UseGzip,
                UseSSL		=> $UseSSL
            );

Getopt::Long::Configure ("bundling_override");
GetOptions (\%Config,	'Host|d:s', 'Port|p:s', 'Username|User|u:s', 'Password|Pass|s:s', 'FileOrDir|Output|o:s', 'Data|i:s', 'Clean|c', 'Deref|L',
                        'UserEncoding|e:s', 'help|h|?', 'man', 'version|V',
                      ) or pod2usage(2);

pod2usage(-exitstatus => 0, -verbose => 3)	if ( $Config{help} || $Config{man} );

if ( $Config{version} ) {
  print $ClientInfo . " (". EplistsClient_Version() . ")" . "\n";
  exit 0;
}

my $Cmd  = "@ARGV";

unless ( $Cmd ) {
  print __"Error", ": ", __"Missing command", "\n\n";
  pod2usage(1);
}

if ($Config{UserEncoding} =~ /^(iso-8859-1|latin1|utf8|utf-8-strict|utf-8)$/i) {
  $Config{UserEncoding} = $1;
  $Config{UserEncoding} = "latin1"  if $Config{UserEncoding} =~ /(iso-8859-1|latin1)/i;
  $Config{UserEncoding} = "utf8"    if $Config{UserEncoding} =~ /(utf8|utf-8-strict|utf-8)/i;
} else {
  pod2usage(1);
}

if ( $Config{UserEncoding} eq "latin1" ) {
  Unicode::String->stringify_as( 'utf8' ); # utf8 already is the default
  $Cmd = Unicode::String::latin1( $Cmd );
}

EplistsClient_Execute (	{	Host		=> $Config{Host},
                                Port		=> $Config{Port},
                                User		=> $Config{Username},
                                Pass		=> $Config{Password},
                                UserEncoding	=> $Config{UserEncoding},
                                FileOrDir	=> $Config{FileOrDir},
                                InputFile	=> $Config{Data},
                                Clean		=> $Config{Clean},
                                Deref		=> $Config{Deref},
                                UseGzip		=> ${UseGzip},
                                UseSSL		=> $Config{UseSSL}
                        },
                        $Cmd
                        ) || exit 1;
                                                                                                                                                                                                                                                                                

__END__

=head1 NAME

svdrpsend-ng.pl - This script is compatible with the original one delivered with vdr.

=head1 SYNOPSIS

 svdrpsend-ng.pl [options] command

 Help options:

   --help | -h | -?             brief help message
   --man                        full documentation

 Options:

         -d hostname          destination hostname (default: localhost)
         -p port              SVDRP port number (default: 2001)

         -o filename or dir   write output to file or dir, useful with -c
                            
         -i filename          read data from file (e.g. for PUT/CHECK an eplist or PUTE)
         -c                   clean, only print data, no codes
         -L                   dereference links
         -V                   print version

 Special options for eplist server:

         -u username          username to authorize for writing at eplist server
         -s password          password to authorize for writing at eplist server

         * You need an account at http://eplists.constabel.net/eplists/eplist.cgi *

         -e charset           data encoding, use utf8 or latin1

=head1 Examples for vdr:

  svdrpsend-ng.pl -d video -p 2001 -c -o ZDF.epg LSTE 2
  
    Writes the EPG of channel 2 into file ZDF.epg without the svdrp codes.

  svdrpsend-ng.pl -d video -p 2001 -i ZDF.epg PUTE

    Send vdr the data back.

 Examples for eplists:

  svdrpsend-ng.pl -d eplists.constabel.net -p 2006 LHELP

    Print long help.

  svdrpsend-ng.pl -d eplists.constabel.net -p 2006 -u demo -s 1234 [options] command
  
    Authorize first, then send the command

  svdrpsend-ng.pl -d eplists.constabel.net -p 2006 GET Monc   

    Get the episodes from Monk and print them on screen.

  svdrpsend-ng.pl -d eplists.constabel.net -p 2006 -c -o MonkTest.txt GET Monc

    Get the episodes from Monk and write them without svdrp codes to file MonkTest.txt.

  svdrpsend-ng.pl -d eplists.constabel.net -p 2006 -c -o /tmp/ GET Monc

   Get the episodes from Monk and write them without svdrp codes to file /tmp/Monk.episodes

  svdrpsend-ng.pl -d eplists.constabel.net -p 2006 -c -o /tmp/ GET all

   Get all lists and write them in files under /tmp/

=head1 COPYRIGHT and LICENSE

Copyright (c) 2007 Mike Constabel

L<http://www.constabel.net/vdr/>

This  is free software.  You may redistribute copies of it under the terms of the GNU General Public License <http://www.gnu.org/licenses/gpl.html>.
There is NO WARRANTY, to the extent permitted by law.

=cut

