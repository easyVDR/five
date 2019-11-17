#!/bin/sh

#------------------------------------------------------------------
#     get params
#------------------------------------------------------------------
CMDNAME=`basename $0`
ACCOUNTNAME=$1
USERNAME=$2
# $3 is a dummy (will eventually become host-name)
# $4 is a dummy (will eventually become "POP3" | "IMAP")
COUNT_NEW=$5
COUNT_TOTAL=$6

#------------------------------------------------------------------
#     set some variables
#------------------------------------------------------------------
LOGCMD="logger -t $CMDNAME"
SVDRPSEND="/usr/bin/svdrpsend"

#------------------------------------------------------------------
#     log the parameters
#------------------------------------------------------------------
$LOGCMD "========================================================"
$LOGCMD "svdrpsend       : '$SVDRPSEND'"
$LOGCMD "AccountName     : '$ACCOUNTNAME'"
$LOGCMD "UserName        : '$USERNAME'"
$LOGCMD "New / Total     : $COUNT_NEW / $COUNT_TOTAL"
$LOGCMD "========================================================"

exit $EXTRET
