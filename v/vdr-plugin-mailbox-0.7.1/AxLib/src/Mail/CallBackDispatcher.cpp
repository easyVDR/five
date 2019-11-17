//=============================================================================
/*
 * See the file README in the main directory for a description of
 * this software, copyright information, and how to reach the author.
 *
 * Author: Alex
 * Date:   27.12.2004
 *
 * Last modfied:
 *   $Author: alex $
 *   $Date: 2004-11-08 22:04:16 +0100 (Mo, 08 Nov 2004) $
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
#include <typeinfo>

//----- C ---------------------------------------------------------------------
//----- AxLib -----------------------------------------------------------------
#include <Ax/Tools/Trace.h>
// #include <Ax/Tools/TraceOn.h>
#include <Ax/Tools/Macros.h>
#include <Ax/Tools/String.h>
#include <Ax/Mail/MailBox.h>
#include <Ax/Mail/MailBoxMgr.h>

//----- vdr -------------------------------------------------------------------
//----- local plugin ----------------------------------------------------------
//----- locals ----------------------------------------------------------------
#include "CallBackDispatcher.h"
#include "Request.h"

//=============================================================================
//     defines
//=============================================================================

//=============================================================================
//     using namespaces
//=============================================================================
using namespace std;

//=============================================================================
//     namespaces
//=============================================================================
namespace Ax   {
namespace Mail {

//=============================================================================
//     class CallBackDispatcher
//=============================================================================

//-------------------------------------------------------------------------
//     CallBackDispatcher::CallBackDispatcher()
//-------------------------------------------------------------------------
CallBackDispatcher::CallBackDispatcher()
                  : myActiveProcessor(0)
{
  wsdebug(("> CallBackDispatcher::CallBackDispatcher()"));
  wsdebug(("< CallBackDispatcher::CallBackDispatcher()"));
} // CallBackDispatcher::CallBackDispatcher()

//-------------------------------------------------------------------------
//     CallBackDispatcher::~CallBackDispatcher()
//-------------------------------------------------------------------------
CallBackDispatcher::~CallBackDispatcher()
{
  wsdebug(("> CallBackDispatcher::~CallBackDispatcher()"));
  wsdebug(("< CallBackDispatcher::~CallBackDispatcher()"));
} // CallBackDispatcher::~CallBackDispatcher()

//-------------------------------------------------------------------------
//     CallBackDispatcher::setActiveProcessor()
//-------------------------------------------------------------------------
void CallBackDispatcher::setActiveProcessor(CallBackProcessor *theCBP, bool fActive)
{
#if 0
  wsdebug(("  CallBackDispatcher::setActiveProcessor() %d %p %s"
         , fActive
         , theCBP
         , theCBP ? theCBP->getName().c_str() : "NULL"
         ));
#endif
  if (fActive)
  {
    if (myActiveProcessor)
    {
      wserror(("  CallBackDispatcher::setActiveProcessor() already have an active processor: %s"
             , theCBP->getName().c_str()
             ));
    } // if
    myActiveProcessor = theCBP;
  }
  else
  {
    if (myActiveProcessor != theCBP)
    {
      wserror(("  CallBackDispatcher::setActiveProcessor() current active processor %s != the given %s"
             , myActiveProcessor->getName().c_str(), theCBP->getName().c_str()
             ));
    } // if
    myActiveProcessor = 0;
  } // if
} // CallBackDispatcher::setActiveProcessor()

//-------------------------------------------------------------------------
//     CallBackDispatcher::getProcessor()
//-------------------------------------------------------------------------
CallBackProcessor *CallBackDispatcher::getProcessor(const MAILSTREAM *theStream)
{
  wsdebug(("> CallBackDispatcher::getProcessor() stream 0x%p", theStream));
  AX_UNUSED_ARG(theStream);
  CallBackProcessor *aCBProcessor = 0;

  //----- first check the active processor
  if (myActiveProcessor)
  {
    if (theStream != 0 && myActiveProcessor->getStream() != 0 && theStream != myActiveProcessor->getStream())
    {
      wsdebug(("  CallBackDispatcher::getProcessor() RESULT: STREAM-CONFLICT!"));
    }
    else
    {
      aCBProcessor = myActiveProcessor;
      wsdebug(("  CallBackDispatcher::getProcessor() RESULT: using my active processor!"));
    } // if
  }

  if (aCBProcessor == 0)
  {
    aCBProcessor = MailBoxMgr::instance()->getCBP();
    wsdebug(("  CallBackDispatcher::getProcessor() RESULT: using final processor!"));
  } // if

  if (aCBProcessor == 0)
  {
    // this should not happen as MailBoxMgr has always a CBP
    wserror(("  CallBackDispatcher::getProcessor() no CallBackProcessor found -> returning 0 -> crash pending!"));
  } // if

  wsdebug(("< CallBackDispatcher::getProcessor() stream 0x%p = %s", theStream, aCBProcessor ? aCBProcessor->getName().c_str() : "NULL"));
  return aCBProcessor;
} // CallBackDispatcher::getProcessor()

//-------------------------------------------------------------------------
//     CallBackDispatcher::showActiveProcessor()
//-------------------------------------------------------------------------
void CallBackDispatcher::showActiveProcessor(CallBackProcessor *theProcessor, const std::string &thePrefix) const
{
  if (myActiveProcessor)
  {
    theProcessor->log( LL_WARN
                     , Ax::Tools::String::sprintf( "%s: active processor: %s (%s)"
                                                 , thePrefix.c_str()
                                                 , myActiveProcessor->getName().c_str()
                                                 , typeid(myActiveProcessor).name()
                                                 )
                     );
  }
  else
  {
    theProcessor->log( LL_WARN
                     , Ax::Tools::String::sprintf( "%s: no active processor"
                                                 , thePrefix.c_str()
                                                 )
                     );
  } // if
} // CallBackDispatcher::showActiveProcessor()

//=========================================================================
//     process c-client callbacks
//=========================================================================
void CallBackDispatcher::process_mm_flags(const MAILSTREAM *theStream, unsigned long theMsgNum)
{
  CallBackProcessor *aProcessor = getProcessor(theStream);

  aProcessor->log( LL_CALLBACK
                 , Ax::Tools::String::sprintf("mm_flags: MsgNum: %ld", theMsgNum)
                 );

  bool fProcessed = aProcessor->process_mm_flags(theMsgNum);

  if (!fProcessed)
  {
    aProcessor->log( LL_WARN
                   , Ax::Tools::String::sprintf("mm_flags NOT PROCESSED: MsgNum: %ld", theMsgNum)
                   );
    showActiveProcessor(aProcessor, "mm_flags");
  } // if
} // CallBackDispatcher::process_mm_flags()

//-------------------------------------------------------------------------
void CallBackDispatcher::process_mm_status(const MAILSTREAM *theStream, const std::string &theMailBoxString, const MAILSTATUS *theStatus)
{
  CallBackProcessor *aProcessor = getProcessor(theStream);

  aProcessor->log( LL_CALLBACK
                 , Ax::Tools::String::sprintf("mm_status: mailbox: '%s'  total: %lu  unseen: %lu  recent: %lu  uidnext: %lu  uidvalidity: %lu"
                                             , theMailBoxString.c_str()
                                             , theStatus->messages
                                             , theStatus->unseen
                                             , theStatus->recent
                                             , theStatus->uidnext
                                             , theStatus->uidvalidity
                                             )
                 );

  bool fProcessed = aProcessor->process_mm_status(theMailBoxString, theStatus);

  if (!fProcessed)
  {
    aProcessor->log( LL_WARN
                   , Ax::Tools::String::sprintf( "mm_status NOT PROCESSED: mailbox: '%s'  total: %lu  unseen: %lu  recent: %lu  uidnext: %lu  uidvalidity: %lu"
                                               , theMailBoxString.c_str()
                                               , theStatus->messages
                                               , theStatus->unseen
                                               , theStatus->recent
                                               , theStatus->uidnext
                                               , theStatus->uidvalidity
                                               )
                   );
    showActiveProcessor(aProcessor, "mm_status");
  } // if
} // CallBackDispatcher::process_mm_status()

//-------------------------------------------------------------------------
void CallBackDispatcher::process_mm_searched(const MAILSTREAM *theStream, unsigned long theMsgNum)
{
  CallBackProcessor *aProcessor = getProcessor(theStream);

  aProcessor->log( LL_CALLBACK
                 , Ax::Tools::String::sprintf("mm_searched: MsgNum: %lu", theMsgNum)
                 );

  bool fProcessed = aProcessor->process_mm_searched(theMsgNum);

  if (!fProcessed)
  {
    aProcessor->log( LL_WARN
                   , Ax::Tools::String::sprintf("mm_searched NOT PROCESSED: MsgNum: %lu", theMsgNum)
                   );
    showActiveProcessor(aProcessor, "mm_searched");
  } // if
} // CallBackDispatcher::process_mm_searched()

//-------------------------------------------------------------------------
void CallBackDispatcher::process_mm_exists(const MAILSTREAM *theStream, unsigned long theMsgNum)
{
  CallBackProcessor *aProcessor = getProcessor(theStream);

  aProcessor->log( LL_CALLBACK
                 , Ax::Tools::String::sprintf("mm_exists: 0x%p  MsgNum: %lu", theStream, theMsgNum)
                 );

  bool fProcessed = aProcessor->process_mm_exists(theMsgNum);

  if (!fProcessed)
  {
    aProcessor->log( LL_WARN
                   , Ax::Tools::String::sprintf("mm_exists NOT PROCESSED: 0x%p  MsgNum: %lu", theStream, theMsgNum)
                   );
    showActiveProcessor(aProcessor, "mm_exists");
  } // if
} // CallBackDispatcher::process_mm_exists()

//-------------------------------------------------------------------------
void CallBackDispatcher::process_mm_expunged(const MAILSTREAM *theStream, unsigned long theMsgNum)
{
  CallBackProcessor *aProcessor = getProcessor(theStream);

  aProcessor->log( LL_CALLBACK
                 , Ax::Tools::String::sprintf("mm_expunged: 0x%p  MsgNum: %lu", theStream, theMsgNum)
                 );

  bool fProcessed = aProcessor->process_mm_expunged(theMsgNum);

  if (!fProcessed)
  {
    aProcessor->log( LL_WARN
                   , Ax::Tools::String::sprintf("mm_expunged NOT PROCESSED: 0x%p  MsgNum: %lu", theStream, theMsgNum)
                   );
    showActiveProcessor(aProcessor, "mm_expunged");
  } // if
} // CallBackDispatcher::process_mm_expunged()

//-------------------------------------------------------------------------
void CallBackDispatcher::process_mm_list(const MAILSTREAM *theStream, char theDelim, const std::string &theMailBoxString, long theAttrib)
{
  CallBackProcessor *aProcessor = getProcessor(theStream);

  aProcessor->log( LL_CALLBACK
                 , Ax::Tools::String::sprintf( "mm_list: delimiter: '%c' = %d  attr: %04lX  name: '%s'"
                                             , theDelim, int(theDelim), theAttrib, theMailBoxString.c_str()
                                             )
                 );

  bool fProcessed = aProcessor->process_mm_list(theDelim, theMailBoxString, theAttrib);

  if (!fProcessed)
  {
    aProcessor->log( LL_WARN
                   , Ax::Tools::String::sprintf( "mm_list NOT PROCESSED: delimiter: '%c' = %d  attr: %04lX  name: '%s'"
                                               , theDelim, int(theDelim), theAttrib, theMailBoxString.c_str()
                                               )
                   );
    showActiveProcessor(aProcessor, "mm_list");
  } // if
} // CallBackDispatcher::process_mm_list()

//-------------------------------------------------------------------------
void CallBackDispatcher::process_mm_lsub(const MAILSTREAM *theStream, char theDelim, const std::string &theMailBoxString, long theAttrib)
{
  CallBackProcessor *aProcessor = getProcessor(theStream);

  aProcessor->log( LL_CALLBACK
                 , Ax::Tools::String::sprintf( "mm_lsub: delimiter: '%c' = %d  attr: %04lX  name: '%s'"
                                             , theDelim, int(theDelim), theAttrib, theMailBoxString.c_str()
                                             )
                 );

  bool fProcessed = aProcessor->process_mm_lsub(theDelim, theMailBoxString, theAttrib);

  if (!fProcessed)
  {
    aProcessor->log( LL_WARN
                   , Ax::Tools::String::sprintf( "mm_lsub NOT PROCESSED: delimiter: '%c' = %d  attr: %04lX  name: '%s'"
                                               , theDelim, int(theDelim), theAttrib, theMailBoxString.c_str()
                                               )
                   );
    showActiveProcessor(aProcessor, "mm_lsub");
  } // if
} // CallBackDispatcher::process_mm_lsub()

//-------------------------------------------------------------------------
void CallBackDispatcher::process_mm_notify(const MAILSTREAM *theStream, const std::string &theMessage, long theErrorFlag)
{
  CallBackProcessor *aProcessor = getProcessor(theStream);
  bool fProcessed =  aProcessor->process_mm_notify(theMessage, theErrorFlag);

  if (!fProcessed)
  {
    aProcessor->log( LL_WARN
                   , Ax::Tools::String::sprintf("mm_notify NOT PROCESSED: %ld: '%s'", theErrorFlag, theMessage.c_str())
                   );
    showActiveProcessor(aProcessor, "mm_notify");
  } // if
} // CallBackDispatcher::process_mm_notify()

//-------------------------------------------------------------------------
void CallBackDispatcher::process_mm_log(const std::string &theMessage, long theErrorFlag)
{
  CallBackProcessor *aProcessor = getProcessor(0);
  bool fProcessed =  aProcessor->process_mm_log(theMessage, theErrorFlag);

  if (!fProcessed)
  {
    aProcessor->log( LL_WARN
                   , Ax::Tools::String::sprintf("mm_log NOT PROCESSED: %ld: '%s'", theErrorFlag, theMessage.c_str())
                   );
    showActiveProcessor(aProcessor, "mm_log");
  } // if
} // CallBackDispatcher::process_mm_log()

//-------------------------------------------------------------------------
void CallBackDispatcher::process_mm_dlog(const std::string &theMessage)
{
  CallBackProcessor *aProcessor = getProcessor(0);
  bool fProcessed =  aProcessor->process_mm_dlog(theMessage);

  if (!fProcessed)
  {
    aProcessor->log( LL_WARN
                   , Ax::Tools::String::sprintf("mm_dlog NOT PROCESSED: '%s'", theMessage.c_str())
                   );
    showActiveProcessor(aProcessor, "mm_dlog");
  } // if
} // CallBackDispatcher::proceprocess_mm_dlog()

//-------------------------------------------------------------------------
void CallBackDispatcher::process_mm_fatal(const std::string &theMessage)
{
  CallBackProcessor *aProcessor = getProcessor(0);

  aProcessor->log( LL_CALLBACK
                 , Ax::Tools::String::sprintf("mm_fatal: msg: %s", theMessage.c_str())
                 );

  bool fProcessed = aProcessor->process_mm_fatal(theMessage);

  if (!fProcessed)
  {
    aProcessor->log( LL_WARN
                   , Ax::Tools::String::sprintf("mm_fatal NOT PROCESSED: msg: '%s'", theMessage.c_str())
                   );
    showActiveProcessor(aProcessor, "mm_fatal");
  } // if
} // CallBackDispatcher::process_mm_fatal()

//-------------------------------------------------------------------------
void CallBackDispatcher::process_mm_login(NETMBX *theMailBox, char *theUser, char *thePassword, long theTrials)
{
  CallBackProcessor *aProcessor = getProcessor(0);

  aProcessor->log( LL_CALLBACK
                 , Ax::Tools::String::sprintf( "mm_login: host: '%s'  user: '%s'  mailbox: '%s'  service: '%s'  port: %lu  trials: %ld"
                                             , theMailBox->host
                                             , theMailBox->user
                                             , theMailBox->mailbox
                                             , theMailBox->service
                                             , theMailBox->port
                                             , theTrials
                                             )
                 );

  bool fProcessed = aProcessor->process_mm_login(theMailBox, theUser, thePassword, theTrials);

  if (!fProcessed)
  {
    aProcessor->log( LL_WARN
                   , Ax::Tools::String::sprintf( "mm_login NOT PROCESSED: host: '%s'  user: '%s'  mailbox: '%s'  service: '%s'  port: %lu  trials: %ld"
                                               , theMailBox->host
                                               , theMailBox->user
                                               , theMailBox->mailbox
                                               , theMailBox->service
                                               , theMailBox->port
                                               , theTrials
                                               )
                   );
    showActiveProcessor(aProcessor, "mm_login");
  } // if
} // CallBackDispatcher::process_mm_login()

//-------------------------------------------------------------------------
void CallBackDispatcher::process_mm_critical(const MAILSTREAM *theStream)
{
  CallBackProcessor *aProcessor = getProcessor(theStream);

  aProcessor->log( LL_CALLBACK
                 , Ax::Tools::String::sprintf("mm_critical: stream: 0x%p", theStream)
                 );

  bool fProcessed = aProcessor->process_mm_critical();

  if (!fProcessed)
  {
    aProcessor->log( LL_WARN
                   , Ax::Tools::String::sprintf("mm_critical NOT PROCESSED: stream: 0x%p", theStream)
                   );
    showActiveProcessor(aProcessor, "mm_critical");
  } // if
} // CallBackDispatcher::process_mm_critical()

//-------------------------------------------------------------------------
void CallBackDispatcher::process_mm_nocritical(const MAILSTREAM *theStream)
{
  CallBackProcessor *aProcessor = getProcessor(theStream);

  aProcessor->log( LL_CALLBACK
                 , Ax::Tools::String::sprintf("mm_nocritical: stream: 0x%p", theStream)
                 );

  bool fProcessed = aProcessor->process_mm_nocritical();

  if (!fProcessed)
  {
    aProcessor->log( LL_WARN
                   , Ax::Tools::String::sprintf("mm_nocritical NOT PROCESSED: stream: 0x%p", theStream)
                   );
    showActiveProcessor(aProcessor, "mm_nocritical");
  } // if
} // CallBackDispatcher::process_mm_nocritical()

//-------------------------------------------------------------------------
long CallBackDispatcher::process_mm_diskerror(const MAILSTREAM *theStream, unsigned long theOSErrorCode, unsigned long theIsSerious)
{
  CallBackProcessor *aProcessor = getProcessor(theStream);

  aProcessor->log( LL_CALLBACK
                 , Ax::Tools::String::sprintf( "mm_diskerror: stream: 0x%p  errcode: %ld  serious: %ld"
                                             , theStream, theOSErrorCode, theIsSerious
                                             )
                 );

  bool fProcessed = aProcessor->process_mm_diskerror(theOSErrorCode, theIsSerious);

  if (!fProcessed)
  {
    aProcessor->log( LL_WARN
                   , Ax::Tools::String::sprintf("mm_diskerror NOT PROCESSED: stream: 0x%p  errcode: %ld  serious: %ld"
                                               , theStream, theOSErrorCode, theIsSerious
                                               )
                   );
    showActiveProcessor(aProcessor, "mm_diskerror");
  } // if

  return T;
} // CallBackDispatcher::process_mm_diskerror()
//-------------------------------------------------------------------------

//=============================================================================
//     namespaces
//=============================================================================
} // Mail
} // Ax

//=============================================================================
//     c-client callbacks
//=============================================================================
#define CBD ::Ax::Mail::MailBoxMgr::instance()->getCBD()

#include <Ax/Tools/TraceOff.h>

//-----------------------------------------------------------------------------
void mm_flags(MAILSTREAM *stream, unsigned long number)
{
  wsdebug(("> mm_flags: 0x%p  msgnum: %ld", stream, number));
  CBD->process_mm_flags(stream, number);
  wsdebug(("< mm_flags: 0x%p  msgnum: %ld", stream, number));
} // mm_flags()
//-----------------------------------------------------------------------------
void mm_status(MAILSTREAM *stream, char *mailbox, MAILSTATUS *status)
{
  wsdebug(("> mm_status: 0x%p  mailbox: %s", stream, mailbox));
  CBD->process_mm_status(stream, std::string(mailbox), status);
  wsdebug(("< mm_status: 0x%p  mailbox: %s", stream, mailbox));
} // mm_status()
//-----------------------------------------------------------------------------
void mm_searched(MAILSTREAM *stream, unsigned long msgno)
{
  wsdebug(("> mm_searched: 0x%p  msgnum: %ld", stream, msgno));
  CBD->process_mm_searched(stream, msgno);
  wsdebug(("< mm_searched: 0x%p  msgnum: %ld", stream, msgno));
} // mm_searched()
//-----------------------------------------------------------------------------
void mm_exists(MAILSTREAM *stream, unsigned long number)
{
  wsdebug(("> mm_exists: 0x%p  msgnum: %ld", stream, number));
  CBD->process_mm_exists(stream, number);
  wsdebug(("< mm_exists: 0x%p  msgnum: %ld", stream, number));
} // mm_exists()
//-----------------------------------------------------------------------------
void mm_expunged(MAILSTREAM *stream, unsigned long number)
{
  wsdebug(("> mm_expunged: 0x%p  msgnum: %ld", stream, number));
  CBD->process_mm_expunged(stream, number);
  wsdebug(("< mm_expunged: 0x%p  msgnum: %ld", stream, number));
} // mm_expunged()
//-----------------------------------------------------------------------------
void mm_list(MAILSTREAM *stream, int delimiter, char *name, long attributes)
{
  wsdebug(("> mm_list: 0x%p  delim: %d  attr: %04X name: %s", stream, delimiter, attributes, name));
  CBD->process_mm_list(stream, delimiter, std::string(name), attributes);
  wsdebug(("< mm_list: 0x%p  delim: %d  attr: %04X name: %s", stream, delimiter, attributes, name));
} // mm_list()
//-----------------------------------------------------------------------------
void mm_lsub(MAILSTREAM *stream, int delimiter, char *name, long attributes)
{
  wsdebug(("> mm_lsub: 0x%p  delim: %d  attr: %04X name: %s", stream, delimiter, attributes, name));
  CBD->process_mm_lsub(stream, delimiter, name, attributes);
  wsdebug(("< mm_lsub: 0x%p  delim: %d  attr: %04X name: %s", stream, delimiter, attributes, name));
} // mm_lsub()
//-----------------------------------------------------------------------------
void mm_notify(MAILSTREAM *stream, char *string, long errflg)
{
  wsdebug(("> mm_notify: 0x%p  err: %ld  str: %s", stream, errflg, string));
  CBD->process_mm_notify(stream, std::string(string), errflg);
  wsdebug(("< mm_notify: 0x%p  err: %ld  str: %s", stream, errflg, string));
} // mm_notify()
//-----------------------------------------------------------------------------
void mm_log(char *string, long errflg)
{
  wsdebug(("> mm_log: err: %ld  str: %s", errflg, string));
  CBD->process_mm_log(std::string(string), errflg);
  wsdebug(("< mm_log: err: %ld  str: %s", errflg, string));
} // mm_log()
//-----------------------------------------------------------------------------
void mm_dlog(char *string)
{
  wsdebug(("> mm_dlog: str: '%s'", string));
  CBD->process_mm_dlog(std::string(string));
  wsdebug(("< mm_dlog: str: '%s'", string));
} // mm_dlog()
//-----------------------------------------------------------------------------
void mm_login(NETMBX *mb, char *username, char *password, long trial)
{
  wsdebug(("> mm_login:"));
  CBD->process_mm_login(mb, username, password, trial);
  wsdebug(("< mm_login:"));
} // mm_login
//-----------------------------------------------------------------------------
void mm_critical(MAILSTREAM *stream)
{
  wsdebug(("> mm_critical: stream: 0x%p", stream));
  CBD->process_mm_critical(stream);
  wsdebug(("< mm_critical: stream: 0x%p", stream));
} // mm_critical()
//-----------------------------------------------------------------------------
void mm_nocritical(MAILSTREAM *stream)
{
  wsdebug(("> mm_nocritical: stream: 0x%p", stream));
  CBD->process_mm_nocritical(stream);
  wsdebug(("< mm_nocritical: stream: 0x%p", stream));
} // mm_nocritical()
//-----------------------------------------------------------------------------
long mm_diskerror(MAILSTREAM *stream, long errcode, long serious)
{
  wsdebug(("> mm_diskerror: stream: 0x%p  errcode: %ld  serious: %d", stream, errcode, serious));
  return CBD->process_mm_diskerror(stream, errcode, serious);
  wsdebug(("< mm_diskerror: stream: 0x%p  errcode: %ld  serious: %d", stream, errcode, serious));
} // mm_diskerror()
//-----------------------------------------------------------------------------
void mm_fatal(char *string)
{
  wsdebug(("> mm_fatal: str: '%s'", string));
  CBD->process_mm_fatal(std::string(string));
  wsdebug(("< mm_fatal: str: '%s'", string));
} // mm_fatal()

