//=============================================================================
/*
 * See the file README in the main directory for a description of
 * this software, copyright information, and how to reach the author.
 *
 * Author: alex
 * Date:   15.11.2003
 *
 * Last modfied:
 *   $Author: alex $
 *   $Date: 2013-03-20 19:53:42 +0100 (Mi, 20 Mär 2013) $
 */
//=============================================================================

//=============================================================================
//     includes
//=============================================================================

//----- qt --------------------------------------------------------------------
//----- CORBA -----------------------------------------------------------------
//----- ICE -------------------------------------------------------------------
//----- C++ -------------------------------------------------------------------
#include <algorithm>
#include <string>

//----- C ---------------------------------------------------------------------
#include <iconv.h>
#include <errno.h>

//----- Mail -------------------------------------------------------------------
//----- AxLib -----------------------------------------------------------------
#include <Ax/Tools/Trace.h>
// #include <Ax/Tools/TraceOn.h>
#include <Ax/Mail/Globals.h>

//----- local plugin ----------------------------------------------------------
//----- locals ----------------------------------------------------------------
#include "Internal.h"

//=============================================================================
//     using namespaces
//=============================================================================

//=============================================================================
//     namespaces
//=============================================================================
namespace Ax       {
namespace Mail     {
namespace Internal {

//-----------------------------------------------------------------------
//     dumpStream()
//-----------------------------------------------------------------------
void dumpStream(const MAILSTREAM *s)
{
  if (s == 0)
  {
    wsinfo(("- dumpStream() Stream == 0"));
    return;
  } // if

  wsinfo(("> dumpStream() Stream == %p", s));
  wsinfo(("  mailbox     : %s" , s->mailbox        ));
  wsinfo(("  use         : %d" , s->use            ));
  wsinfo(("  sequence    : %d" , s->sequence       ));
  wsinfo(("  rdonly      : %d" , s->rdonly         ));
  wsinfo(("  anonym.     : %d" , s->anonymous      ));
  wsinfo(("  halfopen    : %d" , s->halfopen       ));
  wsinfo(("  - seen      : %d" , s->perm_seen      ));
  wsinfo(("  - deleted   : %d" , s->perm_deleted   ));
  wsinfo(("  - flagged   : %d" , s->perm_flagged   ));
  wsinfo(("  - answered  : %d" , s->perm_answered  ));
  wsinfo(("  - draft     : %d" , s->perm_draft     ));
  wsinfo(("  kwd_create  : %d" , s->kwd_create     ));
  wsinfo(("  user_flags  : %lu", s->perm_user_flags));
  wsinfo(("  gensym      : %lu", s->gensym         ));
  wsinfo(("  nmsgs       : %lu", s->nmsgs          ));
  wsinfo(("  recent      : %lu", s->recent         ));
  wsinfo(("  uid_validity: %lu", s->uid_validity   ));
  wsinfo(("  uid_last    : %lu", s->uid_last       ));
  for (int i = 0; i < NUSERFLAGS; ++i)
  {
    if (s->user_flags[i] != 0)
      wsinfo(("  userflag[%2d] : '%s'", i, s->user_flags[i]));
  } // for
  wsinfo(("< dumpStream() Stream == %p", s));

} // dumpStream()

//-----------------------------------------------------------------------
//     dumpMsgCache()
//-----------------------------------------------------------------------
void dumpMsgCache(const MESSAGECACHE *s)
{
  if (s == 0)
  {
    wsinfo(("- dumpMsgCache() MsgCache == 0"));
    return;
  } // if

  wsinfo(("> dumpMsgCache() MsgCache == %p", s));
  wsinfo(("  msgno       : %lu", s->msgno          ));
//  wsinfo(("  uid         : %lu", s->uid            ));
  wsinfo(("  lockcount   : %u ", s->lockcount      ));
  wsinfo(("  hms +- ofs  : %02d:%02d:%02d %c %02d:%02d", s->hours, s->minutes, s->seconds, s->zoccident ? '-' : '+', s->zhours, s->zminutes));
  wsinfo(("  date        : %2d.%02d.%d", s->day, s->month, s->year + BASEYEAR));
  wsinfo(("  - seen      : %d" , s->seen           ));
  wsinfo(("  - deleted   : %d" , s->deleted        ));
  wsinfo(("  - flagged   : %d" , s->flagged        ));
  wsinfo(("  - answered  : %d" , s->answered       ));
  wsinfo(("  - draft     : %d" , s->draft          ));
  wsinfo(("  - valid     : %d %s", s->valid, s->valid ? "" : "!!!!!!!"));
  wsinfo(("  - recent    : %d" , s->recent         ));
  wsinfo(("  - searched  : %d" , s->searched       ));
  wsinfo(("  user-flags  : %08lX", s->user_flags    ));
  wsinfo(("  rfc822_size : %lu", s->rfc822_size    ));
  wsinfo(("< dumpMsgCache() MsgCache == %p", s));
} // dumpMsgCache()

//-----------------------------------------------------------------------
//     dumpMsgBody()
//-----------------------------------------------------------------------
void dumpMsgBody(const BODY *s)
{
  if (s == 0)
  {
    wsinfo(("- dumpMsgBody() Body == 0"));
    return;
  } // if

  wsinfo(("> dumpMsgBody() Body == %p", s));
#if 0
  wsinfo(("  msgno       : %lu", s->msgno          ));
//  wsinfo(("  uid         : %lu", s->uid            ));
  wsinfo(("  lockcount   : %u ", s->lockcount      ));
  wsinfo(("  hms +- ofs  : %02d:%02d:%02d %c %02d:%02d", s->hours, s->minutes, s->seconds, s->zoccident ? '-' : '+', s->zhours, s->zminutes));
  wsinfo(("  date        : %2d.%02d.%d", s->day, s->month, s->year + BASEYEAR));
  wsinfo(("  - seen      : %d" , s->seen           ));
  wsinfo(("  - deleted   : %d" , s->deleted        ));
  wsinfo(("  - flagged   : %d" , s->flagged        ));
  wsinfo(("  - answered  : %d" , s->answered       ));
  wsinfo(("  - draft     : %d" , s->draft          ));
  wsinfo(("  - valid     : %d %s", s->valid, s->valid ? "" : "!!!!!!!"));
  wsinfo(("  - recent    : %d" , s->recent         ));
  wsinfo(("  - searched  : %d" , s->searched       ));
  wsinfo(("  user-flags  : %08lX", s->user_flags    ));
  wsinfo(("  rfc822_size : %lu", s->rfc822_size    ));
#endif
  wsinfo(("< dumpMsgBody() MsgCache == %p", s));
} // dumpMsgBody()

//-----------------------------------------------------------------------
//     dumpEnvelope()
//-----------------------------------------------------------------------
void dumpEnvelope(const ENVELOPE *s)
{
  if (s == 0)
  {
    wsinfo(("- dumpEnvelope() Envelope == 0"));
    return;
  } // if

  wsinfo(("> dumpEnvelope() Envelope == %p", s));
  if (s->remail         ) wsinfo(("  remail      : %s",  s->remail         ));
  if (s->date           ) wsinfo(("  date        : %s",  s->date           ));
  if (s->subject        ) wsinfo(("  subject     : %s",  s->subject        ));
  if (s->in_reply_to    ) wsinfo(("  in_reply_to : %s",  s->in_reply_to    ));
  if (s->message_id     ) wsinfo(("  message_id  : %s",  s->message_id     ));
  if (s->newsgroups     ) wsinfo(("  newsgroups  : %s",  s->newsgroups     ));
  if (s->followup_to    ) wsinfo(("  followup_to : %s",  s->followup_to    ));
  if (s->references     ) wsinfo(("  references  : %s",  s->references     ));
  wsinfo(("< dumpEnvelope() Envelope == %p", s));
} // dumpEnvelope()

//=============================================================================
//     namespaces
//=============================================================================
} // Internal
} // Mail
} // Ax
