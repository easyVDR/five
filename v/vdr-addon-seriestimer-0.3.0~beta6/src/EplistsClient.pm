#!/usr/bin/perl
#
# EplistsClient.pm, modul for svdrpsend-ng.pl
#
# Extended version to communicate with eplist server
# at eplists.constabel.net port 2006
#
# Mike Constabel <eplists (at) constabel dot net>
#

package EplistsClient;

require 5.8.0;  #maybe

my $VERSION	= "0.0.7";
my $DATE	= "2008-06-10";

###########
# Options #
###########

$DisableSSL	= 0;
$DisableGzip	= 0;

############
# /Options #
############

use strict 'refs';
use warnings;
use Exporter;

BEGIN {
  my($error, @Modules, $mod, $found);
  $found = 0;
  @Modules = qw(Unicode::String Net::hostent File::Basename Data::Dumper Pod::Usage locale POSIX Cwd Locale::TextDomain Locale::Messages);
  for $mod (@Modules) {
    if (eval "require $mod") {
      if ( $mod eq "Cwd" ) {
        $mod->import("abs_path");
      } elsif ( $mod eq "Fcntl" ) {
        $mod->import(":DEFAULT", ":flock");
      } elsif ( $mod eq "POSIX" ) {
        $mod->import("locale_h");
      } elsif ( $mod eq "Locale::TextDomain" ) {
        $mod->import('eplists', "/usr/share/locale", "/usr/local/share/locale");
#      } elsif ( $mod eq "Locale::Messages" ) {
#        $mod->import("LC_MESSAGES");
      } else {
        $mod->import;
      }
    } else {
      $error = 1;
      print STDERR "Module $mod not found, please install $mod\n";
      print STDERR "On Debian:\naptitude install perl-modules libintl-perl libunicode-string-perl\n";
    }
  }
  exit 1 if $error;
}

setlocale (LC_MESSAGES, '');

###########
# Options #
###########
my $Timeout		= 120; # max. seconds to wait for response

# Don't change! #
my $UseGzip		= 0;
my $UseSSL		= 0;
my $Debug		= 0;
my $EpisodesListsServer	= 0;
my $ClientInfo		= "EplistsClient.pm v$VERSION $DATE (ACCEPT_ENCODING: plain)";
my $EplistSuffix	= ".episodes";
my $DefaultEncoding	= "utf8";
my $Socket;
my $OutputHandle;
my ($Host, $Port, $User, $Pass, $FileOrDir, $InputFile, $Clean, $Deref, $Help, $Ver);

@ISA = qw(Exporter);
@EXPORT = qw( EplistsClient_Version EplistsClient_Date EplistsClient_Execute EplistsClient_Usage);
@EXPORT_OK = qw( );

sub EplistsClient_Version {
  return "EplistsClient.pm v$VERSION $DATE";
}

my $UserEncoding = $DefaultEncoding;

################
# Load Modules #
################
if (eval "require IO::Uncompress::Gunzip") {
  #print STDERR "Found and using IO::Uncompress::Gunzip\n";
  import IO::Uncompress::Gunzip;
  $UseGzip = 1;
} else {
  print STDERR __("Module")." IO::Uncompress::Gunzip ".__("not found").", ".__("please install it or edit EplistsClient.pm and set")." \$DisableGzip = 1;\n" unless $DisableGzip;
  print STDERR "On Debian:\nperl -MCPAN -e 'install IO::Uncompress::Gunzip'\n";
}
if (! eval "require IO::Socket::SSL") {
  print STDERR __("Module")." IO::Socket::SSL ".__("not found").", ".__("try to load")." IO::Socket::INET (".__("without")." SSL!)\n" unless $DisableSSL;
  print STDERR "On Debian:\naptitude install libio-socket-ssl-perl\n";
  if (! eval "require IO::Socket::INET") {
    print STDERR __("Module")." IO::Socket::INET ".__("not found").", ".__("please install")." IO::Socket::SSL (".__("or")." IO::Socket::INET)\n";
    exit 1;
  } else {
    import IO::Socket::INET;
    print STDERR __("Using")." IO::Socket::INET (".__("without")." SSL!). ".__("To disable this warning install")." IO::Socket::SSL ".__("or")." ".__("edit EplistsClient.pm and set")." \$DisableSSL = 1;\n" unless $DisableSSL;
  }
} else {
  #print STDERR "Found and using IO::Socket::SSL\n";
  import IO::Socket::SSL;
  $UseSSL = 1;
}

########
# Subs #
########
sub Print {
  my ($msg, $t, $handle, $clean) = @_;
  if ( $clean && ! $t && defined $msg ) {
    if ( $msg =~ /^(?:21[49](?: |-)|215-|216-|250(?: |-)|301 |451 |5[05]0(?: |-)|900(?: |-))(.*)$/ ) {
      print $handle $1."\n" if $handle;
      print STDOUT $1."\n" if ! $handle;
    }
  } elsif ( $t && defined $msg ) {
    print STDERR $msg."\n";
  } else {
    print $handle $msg if $handle && defined $msg;
    print STDOUT $msg if ! $handle && defined $msg;
  }
}

sub Send
{
  my $cmd = shift || Error("no command to send");
  print $Socket "$cmd\r\n";
  return Receive();
}

sub Receive
{
  my ($buffer, $cbuffer, $line);

  while (defined( $line = <$Socket> )) {
    chomp($line);
    my ($code, $sep, $data) = SplitLine($line);
    my @buffer;

    if ( $code == 888 && $UseGzip && $data =~ /^(\d+) bytes gzip compressed data \(\d+\) follows/ ) {
      my $zip_bytes	= $1;
      my $unzip_bytes	= $2;
      Print($line."\n", 0, $OutputHandle, $Clean);
      read  $Socket, $cbuffer, $zip_bytes;
      my $z = new IO::Uncompress::Gunzip \$cbuffer or die "IO::Uncompress::Gunzip ".__("failed")."\n";
      my $status = $z->read(\$buffer, $unzip_bytes);
      $z->close();
      @buffer = split("\n", $buffer);
    } else {
      push @buffer, $line;
    }

    foreach (@buffer) {
      Print($_."\n", 0, $OutputHandle, $Clean);
    }

    last if substr($line, 3, 1) ne "-";
  }
  my ($code, $sep, $data) = SplitLine($line);
  return ($code, $sep, $data);
}

sub Error
{
  print STDERR "@_\n";
  close($Socket);
  exit 0;
}

sub SplitLine($) {
  my ($line) = @_;
  return ($1, $2, $3) if ( $line && $line =~ /^([0-9]{3})([- ])(.*)$/ );
  Error(sprintf(__("Unidentified Line").": %s", ($line) ? $line : "undef"));
}

sub OpenOutput {
  my ($file) = @_;
  close $OutputHandle if $OutputHandle;
  open ($OutputHandle, ">", $file);
}

sub ResetOutput {
  close($OutputHandle);
}

sub CreateLink {
  my ($target, $linkname) = @_;
  symlink $target, $linkname || die $!;
}

sub SetOutput {
  my ($filename, $linktarget) = @_;
  if ( ! $linktarget ) {
    if ( $FileOrDir && -d $FileOrDir && $filename ) {
      OpenOutput($filename.$EplistSuffix);
    } elsif ( $FileOrDir && ! -d $FileOrDir && $filename ) {
      OpenOutput($filename);
    }
  } else {
    if ( $FileOrDir && -d $FileOrDir && $filename && $linktarget ) {
      if ( ! $Deref ) {
        CreateLink($linktarget.$EplistSuffix, $filename.$EplistSuffix);
        OpenOutput($linktarget.$EplistSuffix);
      } else {
        OpenOutput($filename.$EplistSuffix);
      }
    }
  }
}

sub EplistsClient_Execute {
  my ($Config_ref, $Cmd) = @_;

  $Host		= $$Config_ref{Host}		|| "eplists.constabel.net";
  $Port		= $$Config_ref{Port}		|| 2006;
  $User		= $$Config_ref{User}		|| "";
  $Pass		= $$Config_ref{Pass}		|| "";
  $UserEncoding	= $$Config_ref{UserEncoding}	|| "utf8";
  $FileOrDir	= $$Config_ref{FileOrDir}	|| "";
  $InputFile	= $$Config_ref{InputFile}	|| "";
  $Clean	= $$Config_ref{Clean}		|| 0;
  $Deref	= $$Config_ref{Deref}		|| 0;
  if ( $UseSSL && defined $$Config_ref{UseSSL} ) {
    $UseSSL	= $$Config_ref{UseSSL};
  }
  if ( $UseGzip && defined $$Config_ref{UseGzip} ) {
    $UseGzip	= $$Config_ref{UseGzip};
  }

  $ClientInfo	=~ s/ACCEPT_ENCODING: plain/ACCEPT_ENCODING: gzip/ if $UseGzip;
  $ClientInfo	=~ s/\(ACCEPT_ENCODING/\(SSL, ACCEPT_ENCODING/ if $UseSSL;

  return 1 if ! $Cmd;

  ################################################
  # If output file defined, write STDOUT to file #
  ################################################
  if ( $FileOrDir && ! -d $FileOrDir ) {
    #open (STDOUT, ">$FileOrDir") or die $!;
    SetOutput($FileOrDir, 0);
  }
  if ( $FileOrDir && -d $FileOrDir ) {
    $FileOrDir = abs_path($FileOrDir);
    chdir $FileOrDir;
  }

  ##################################################
  # If input file is defined, read file into Array #
  ##################################################
  my @Data;
  my ($filename, $directories, $suffix);
  if ( $InputFile ) {
    if ( open (DATA, "<$InputFile") ) {
      @Data = <DATA>;
      close DATA;
      ($filename, $directories, $suffix) = fileparse($InputFile, qr/\.episodes/);
      if ( $Cmd =~ /^(PUT|CHECK) *$/i ) {
        if ( $UserEncoding eq "latin1" ) {
          Unicode::String->stringify_as( 'utf8' ); # utf8 already is the default
          $filename = Unicode::String::latin1( $filename );
        }
        $Cmd = "$1 " . $filename;
      }
    } else {
      Print(__("File")." $InputFile ".__("not found"), 1, $OutputHandle, $Clean);
      exit(1);
    }
  }

  #########################
  # Create the connection #
  #########################
  $SIG{ALRM} = sub { Error("timeout"); };
  alarm($Timeout);

  if ( $Port !~ /^\d+$/) {
    print STDERR __("Port isn't numeric")."!\n";
    return 1;
  }

  $Socket = new IO::Socket::INET->new(	PeerPort	=> $Port,
                                        Proto		=> 'tcp',
                                        PeerAddr	=> $Host
                                      );
  unless ( $Socket ) {
    print STDERR __("Cannot connect to")." $Host:$Port\n";
    exit 1;
  }

  $Socket->autoflush(1);

  select(STDERR); $|=1;
  select(STDOUT); $|=0;

  #########################################
  # Read first answer to identify service #
  #########################################
  while (defined( my $line = <$Socket> )) {
    chomp($line);
    my ($code, $sep, $data) = SplitLine($line);
    $EpisodesListsServer = 1 if ( index($data, "EpisodesListsServer") != -1 );
    Print($line."\n", 0, $OutputHandle, $Clean);
    last if ( $sep eq ' ' );
  }

  sub startssl {
      IO::Socket::SSL->start_SSL( $Socket, 
      SSL_verify_mode => NONE) or die $SSL_ERROR;
  }

  ##########################
  # If EpisodesListsServer #
  ##########################
  my @granted = (603," ",__"Please provide username and password");
  if ( $EpisodesListsServer ) {
    my @result = Send("helo ".$ClientInfo)	if $ClientInfo;
    return 1				if $ClientInfo && $result[0] =~ /^5/;
    if ( $UserEncoding !~ /^utf8$/ ) {
      my @result = Send("CHARSET " . $UserEncoding);
      return 1				if $result[0] =~ /^5/;
    }
    if ( $UseSSL ) {
      my @result = Send("STARTSSL");
      return 1				if $result[0] =~ /^5/;
      startssl();
    }
    if ( $Cmd =~ /^(?:PUT|DEL |LINK |REN )/i ) {
      my @result = Send("user ".$User)		if $User;
      return 1				if $User && $result[0] =~ /^5/;
      @granted = Send("pass ".$Pass)	if $Pass;
    }
  } else {
    $UseSSL	= 0;
    $UseGzip	= 0;
  }

  ######################################################
  # If command is an command which needs data input... #
  ######################################################
  if ( scalar @Data && $Cmd =~ /^(?:PUT |CHECK |PUTE)/i ) {
    if ($Cmd =~ /^PUT /i && $granted[0] == 600) {
      # Send the command
      print $Socket $Cmd."\n";
      my ($line, $code, $sep, $data);
      # Read returning data
      while (defined( my $line = <$Socket> )) {
        chomp($line);
        ($code, $sep, $data) = SplitLine($line);
        last;
      }
      # There is something wrong. Print it and go ahead
      if ( $code && $code =~ /^[0-9]{3}$/ && ($code == 451 || $code == 500) && $sep eq ' ' ) {
        Print($line."\n", 0, $OutputHandle, $Clean) if $line;
      }
      # Ok, send data from input file
      if ( $code && $code =~ /^[0-9]{3}$/ && $code == 354 && $sep eq ' ' ) {
        print $Socket @Data;
        print $Socket ".\n";
        Receive();
        Receive() if ( $Cmd !~ /^PUTE/i );
      }
    } elsif ($Cmd =~ /^(?:CHECK |PUTE)/i ) {
      # Send the command
      print $Socket $Cmd."\n";
      my ($line, $code, $sep, $data);
      # Read returning data
      while (defined( my $line = <$Socket> )) {
        chomp($line);
        ($code, $sep, $data) = SplitLine($line);
        last;
      }
      # There is something wrong. Print it and go ahead
      if ( $code == 451 && $sep eq ' ' ) {
        Print($line."\n", 0, $OutputHandle, $Clean);
      }
      # Ok, send data from input file
      if ( $code == 354 && $sep eq ' ' ) {
        print $Socket @Data;
        print $Socket ".\n";
        Receive();
        Receive() if ( $Cmd !~ /^PUTE/i );
      }
    } elsif ($Cmd =~ /^PUT /i && $granted[0] == 603) {
      Print($granted[0].$granted[1].$granted[2]."\n", 0, $OutputHandle, $Clean);
    }
    #######################################
    # Else only send the command w/o data #
    #######################################
  } elsif ( $Cmd =~ /^(PUT|CHECK|PUTE)/i ) {
    Print("Error, cannot use $1 w/o data, use -i with an filename", 1, $OutputHandle, $Clean);
    ##################################
    # Else T?GET which receives data #
    ##################################
  } elsif ( $Cmd =~ /^T?GET /i ) {
    print $Socket $Cmd."\n";
    my ($line, $filename, $linktarget, $code, $sep, $data);
    my @buffer;
    my $tmpline;
    GET: while (defined( my $line = <$Socket> )) {
      chomp($line);
      my ($buffer, $cbuffer);
      @buffer = ();
      ($code, $sep, $data) = SplitLine($line);
      if ( $code == 888 && $UseGzip && $data =~ /^(\d+) bytes gzip compressed data \(\d+\) follows/ ) {
        my $zip_bytes	= $1;
        my $unzip_bytes	= $2;
        Print($line."\n", 0, $OutputHandle, $Clean);
        read  $Socket, $cbuffer, $zip_bytes;
        my $z = new IO::Uncompress::Gunzip \$cbuffer or die "IO::Uncompress::Gunzip ".__("failed")."\n";
        my $status = $z->read(\$buffer, $unzip_bytes);
        $z->close();
        @buffer = split("\n", $buffer);
      } else {
        push @buffer, $line;
      }

      foreach (@buffer) {
        my $line = $_."\n";

        ($code, $sep, $data) = SplitLine($line);

        if ( $code == 550 && $sep eq ' ' ) {
          Print($line, 0, $OutputHandle, $Clean);
          last GET;
        } elsif ( $code == 217 && $sep eq ' ' ) {
          Print($line, 0, $OutputHandle, $Clean);
          last GET;
        } elsif ( $code == 218 && $sep eq '-' ) {
          $filename = $data;
          $tmpline = $line;
        } elsif ( $code == 218 && $sep eq ' ' ) {
          if ( $FileOrDir && -d $FileOrDir ) {
            if ( $data eq "not a link" ) {
              SetOutput($filename, 0);
            } else {
              SetOutput($filename, $data);
            }
          }
          Print($tmpline, 0, $OutputHandle, $Clean) if $tmpline;
          $tmpline = "" if $tmpline;
          Print($line, 0, $OutputHandle, $Clean);
        } elsif ( $code == 216 && $sep eq ' ' ) {
          Print($line, 0, $OutputHandle, $Clean);
        } elsif ( $code == 216 ) {
          Print($line, 0, $OutputHandle, $Clean);
        } elsif ( $code == 215 && $sep eq '-' ) {
          Print($line, 0, $OutputHandle, $Clean);
        } elsif ( $code == 215 && $sep eq ' ' ) {
          Print($line, 0, $OutputHandle, $Clean);
          last GET;
        }

      }
    }
  } else {
    ############################################
    # Else for DEL REN LINK check if access is #
    # granted or print error                   #
    ############################################
    if ($Cmd =~ /^(?:DEL|REN|LINK) /i && $granted[0] == 600) {
      Send($Cmd);
    } elsif ($Cmd =~ /^(?:DEL|REN|LINK) /i && $granted[0] == 603) {
      Print($granted[0].$granted[1].$granted[2]."\n", 0, $OutputHandle, $Clean);
    } else {
      Send($Cmd);
    }
  }
  Send("quit");

  ResetOutput() if $OutputHandle;

  ########
  # QUIT #
  ########

  $Socket->close(SSL_no_shutdown => 1)	if $UseSSL && $Socket;
  close $Socket				if ! $UseSSL && $Socket;

  close STDOUT	if $FileOrDir;

  return 0;
}

sub EplistsClient_Usage {

  my $Usage = qq{
Usage: $0 [options] command

This script is compatible with the original one delivered with vdr.

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

Examples for vdr:

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
   };

   return $Usage."\n";
}

1;

__END__
