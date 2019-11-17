#!/bin/sh
# =================================================================
#  converts the given srcfile in HTML-format to text format dstfile
# -----------------------------------------------------------------
# Parameters:
# 
#    -i <name of source file>       -> SRCFILE
#    -o <name of destination file>  -> DSTFILE
#    -s <charset of html-file>      -> SRCCHARSET
#    -d <charset of vdr-osd>        -> DSTCHARSET
#    -w <width in characters>       -> WIDTH
#
# =================================================================
# CONVERTER=w3m
# CONVERTER=h2t
CONVERTER=lnx

CMDNAME=`basename $0`
LOGCMD="logger -t $CMDNAME"
# LOGCMD="echo"

# -----------------------------------------------------------------
#      default values
# -----------------------------------------------------------------
SRCFILE=""
DSTFILE=""
CONTENT_TYPE="html"
CHARSET="iso-8859-1"
WIDTH=50

# -----------------------------------------------------------------
#      show values
# -----------------------------------------------------------------
function showValues
{
  $LOGCMD "Parameter:"
  $LOGCMD "src-file:     $SRCFILE"
  $LOGCMD "src-Charset:  $SRCCHARSET"
  $LOGCMD "dst-file:     $DSTFILE"
  $LOGCMD "dst-Charset:  $DSTCHARSET"
  $LOGCMD "width:        $WIDTH"
}

# -----------------------------------------------------------------
#      process arguments
# -----------------------------------------------------------------
while getopts "i:o:c:s:d:w:" opt; do
  case $opt in
    i) SRCFILE=$OPTARG;;
    o) DSTFILE=$OPTARG;;
    s) SRCCHARSET=$OPTARG;;
    d) DSTCHARSET=$OPTARG;;
    w) WIDTH=$OPTARG;;
  esac
done
shift $(($OPTIND - 1))

if test "$SRCFILE" = ""; then
  $LOGCMD "source file missing (parameter -i)"
  exit 1
fi

if test "$DSTFILE" = ""; then
  $LOGCMD "desitnation file missing (parameter -o)"
  exit 1
fi

if test "$SRCCHARSET" = ""; then
  $LOGCMD "source charset missing (parameter -s)"
  exit 1
fi

if test "$DSTCHARSET" = ""; then
  $LOGCMD "destination charset missing (parameter -d)"
  exit 1
fi

if test "$WIDTH" = ""; then
  $LOGCMD "width missing (parameter -w)"
  exit 1
fi

showValues

# -----------------------------------------------------------------
#      convert
# -----------------------------------------------------------------
case $CONVERTER in
  w3m)
    $LOGCMD "using w3m..."
    w3m -cols $WIDTH -I $SRCCHARSET -O $DSTCHARSET -S -dump $SRCFILE >$DSTFILE
    ;;

  h2t)
    # by default, html2text uses ISO 8859-1
    # -> if VDR needs UTF-8, we have to convert the text afterwards
    case $CHARSET in
      UTF-8)
        $LOGCMD "using html2text/iconv..."
        html2text -width $WIDTH -nobs -style compact $SRCFILE | iconv -f $SRCCHARSET -t $DSTCHARSET > $DSTFILE
        ;;

      *)
        $LOGCMD "using html2text..."
        html2text -width $WIDTH -nobs -style pretty $SRCFILE > $DSTFILE
        ;;
    esac
    ;;

  lnx)
    $LOGCMD "using lynx..."
    lynx -dump -nomargins -nonumbers -center -width $WIDTH -assume_charset=$SRCCHARSET -display_charset=$DSTCHARSET $SRCFILE > $DSTFILE
    ;;

  *)
    $LOGCMD "converter not set!"
    ;;
esac
EXTRET=$?

# -----------------------------------------------------------------
$LOGCMD "exiting with $EXTRET"
exit $EXTRET
