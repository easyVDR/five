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
//----- C ---------------------------------------------------------------------
#include <stdio.h>

//----- AxLib -----------------------------------------------------------------
#include <Ax/Tools/Globals.h>
#include <Ax/Tools/Trace.h>
// #include <Ax/Tools/TraceOn.h>

//----- vdr -------------------------------------------------------------------
//----- local plugin ----------------------------------------------------------
//----- locals ----------------------------------------------------------------
#include "c-client-header.h"
#include "CallBackProcessor.h"
#include "CallBackDispatcher.h"
#include "MailBoxInternal.h"

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
//     class CallBackLock
//=============================================================================

//-------------------------------------------------------------------------
//     CallBackLock::CallBackLock()
//-------------------------------------------------------------------------
CallBackLock::CallBackLock(CallBackProcessor *theCallBackProcessor)
            : myCallBackProcessor(theCallBackProcessor)
{
//  wsdebug(("> CallBackLock::CallBackLock() getting lock %s", myCallBackProcessor->getName().c_str()));
  MailBoxMgr::instance()->getCBD()->setActiveProcessor(myCallBackProcessor, true);
//  wsdebug(("< CallBackLock::CallBackLock() getting lock %s", myCallBackProcessor->getName().c_str()));
} // CallBackLock::CallBackLock()

//-------------------------------------------------------------------------
//     CallBackLock::~CallBackLock()
//-------------------------------------------------------------------------
CallBackLock::~CallBackLock()
{
//  wsdebug(("> CallBackLock::~CallBackLock() getting lock %s", myCallBackProcessor->getName().c_str()));
  MailBoxMgr::instance()->getCBD()->setActiveProcessor(myCallBackProcessor, false);
//  wsdebug(("< CallBackLock::~CallBackLock() getting lock %s", myCallBackProcessor->getName().c_str()));
} // CallBackLock::~CallBackLock()

//=============================================================================
//     class CallBackProcessor
//=============================================================================

//-------------------------------------------------------------------------
//     CallBackProcessor::CallBackProcessor()
//-------------------------------------------------------------------------
CallBackProcessor::CallBackProcessor( MailBox           *theMailBox
                                    , const std::string &theName
                                    , CallBackProcessor *theParent
                                    )
                 : myMailBox(theMailBox)
                 , myParent (theParent)
                 , myName   (theName)
                 , myIsOK   (true)
{
} // CallBackProcessor::CallBackProcessor()

//-------------------------------------------------------------------------
//     CallBackProcessor::~CallBackProcessor()
//-------------------------------------------------------------------------
CallBackProcessor::~CallBackProcessor()
{
} // CallBackProcessor::~CallBackProcessor()

//-------------------------------------------------------------------------
//     CallBackProcessor::getParent()
//-------------------------------------------------------------------------
CallBackProcessor *CallBackProcessor::getParent() const
{
  return myParent;
} // CallBackProcessor::getParent()

//-------------------------------------------------------------------------
//     CallBackProcessor::getStream()
//-------------------------------------------------------------------------
MAILSTREAM *CallBackProcessor::getStream() const
{
  if (myMailBox != 0)
  {
    return myMailBox->getInternal().getStream();
  } // if
  return 0;
} // CallBackProcessor::getStream()

//---------------------------------------------------------------------
//     CallBackProcessor::log()
//---------------------------------------------------------------------
bool CallBackProcessor::log(LogLevel theLevel, const std::string &theMsg)
{
  if (myParent) return myParent->log(theLevel, theMsg);
  return false;
} // CallBackProcessor::log()

//-------------------------------------------------------------------------
//     CallBackProcessor::setIsOK()
//-------------------------------------------------------------------------
void CallBackProcessor::setIsOK(bool fOK)
{
  myIsOK = fOK;
} // CallBackProcessor::setIsOK()

//=========================================================================
//     process c-client callbacks
//=========================================================================
bool CallBackProcessor::process_mm_flags(unsigned long theMsgNum)
{
  if (myParent) return myParent->process_mm_flags(theMsgNum);
  return false;
} // CallBackProcessor::process_mm_flags()
//-------------------------------------------------------------------------
bool CallBackProcessor::process_mm_status(const std::string &theMailBoxString, const MAILSTATUS *theStatus)
{
  if (myParent) return myParent->process_mm_status(theMailBoxString, theStatus);
  return false;
} // CallBackProcessor::process_mm_status()
//-------------------------------------------------------------------------
bool CallBackProcessor::process_mm_searched(unsigned long theMsgNum)
{
  if (myParent) return myParent->process_mm_searched(theMsgNum);
  return false;
} // CallBackProcessor::process_mm_searched()
//-------------------------------------------------------------------------
bool CallBackProcessor::process_mm_exists(unsigned long theMsgNum)
{
  if (myParent) return myParent->process_mm_exists(theMsgNum);
  return false;
} // CallBackProcessor::process_mm_exists()
//-------------------------------------------------------------------------
bool CallBackProcessor::process_mm_expunged(unsigned long theMsgNum)
{
  if (myParent) return myParent->process_mm_expunged(theMsgNum);
  return false;
} // CallBackProcessor::process_mm_expunged()
//-------------------------------------------------------------------------
bool CallBackProcessor::process_mm_list(char theDelim, const std::string &theMailBoxString, long theAttrib)
{
  if (myParent) return myParent->process_mm_list(theDelim, theMailBoxString, theAttrib);
  return false;
} // CallBackProcessor::process_mm_list()
//-------------------------------------------------------------------------
bool CallBackProcessor::process_mm_lsub(char theDelim, const std::string &theMailBoxString, long theAttrib)
{
  if (myParent) return myParent->process_mm_lsub(theDelim, theMailBoxString, theAttrib);
  return false;
} // CallBackProcessor::process_mm_lsub()
//-------------------------------------------------------------------------
bool CallBackProcessor::process_mm_notify(const std::string &theMsg, long theErrorFlag)
{
  switch (theErrorFlag)
  {
    case WARN  :                 break;
    case ERROR : setIsOK(false); break;
  } // switch

  if (myParent) return myParent->process_mm_notify(theMsg, theErrorFlag);
  return false;
} // CallBackProcessor::process_mm_notify()
//-------------------------------------------------------------------------
bool CallBackProcessor::process_mm_log(const std::string &theMsg, long theErrorFlag)
{
  switch (theErrorFlag)
  {
    case NIL   :                 break;  // normal operation
    case PARSE :                 break;  // RFC822 parse error
    case WARN  :                 break;  // warning event -> display to user
    case ERROR : setIsOK(false); break;
  } // switch

  if (myParent) return myParent->process_mm_log(theMsg, theErrorFlag);
  return false;
} // CallBackProcessor::process_mm_log()
//-------------------------------------------------------------------------
bool CallBackProcessor::process_mm_dlog(const std::string &theMsg)
{
  if (myParent) return myParent->process_mm_dlog(theMsg);
  return false;
} // CallBackProcessor::proceprocess_mm_dlog()
//-------------------------------------------------------------------------
bool CallBackProcessor::process_mm_fatal(const std::string &theMsg)
{
  setIsOK(false);

  if (myParent) return myParent->process_mm_fatal(theMsg);
  return false;
} // CallBackProcessor::process_mm_fatal()
//-------------------------------------------------------------------------
bool CallBackProcessor::process_mm_login(NETMBX *theMailBox, char *theUser, char *thePassword, long theTrials)
{
  if (myParent) return myParent->process_mm_login(theMailBox, theUser, thePassword, theTrials);
  return false;
} // CallBackProcessor::process_mm_login()
//-------------------------------------------------------------------------
bool CallBackProcessor::process_mm_critical()
{
  if (myParent) return myParent->process_mm_critical();
  return false;
} // CallBackProcessor::process_mm_critical()
//-------------------------------------------------------------------------
bool CallBackProcessor::process_mm_nocritical()
{
  if (myParent) return myParent->process_mm_nocritical();
  return false;
} // CallBackProcessor::process_mm_nocritical()
//-------------------------------------------------------------------------
bool CallBackProcessor::process_mm_diskerror(unsigned long theOSErrorCode, unsigned long theIsSerious)
{
  setIsOK(false);

  if (myParent) return myParent->process_mm_diskerror(theOSErrorCode, theIsSerious);
  return false;
} // CallBackProcessor::process_mm_diskerror()

//=============================================================================
//     namespaces
//=============================================================================
} // Mail
} // Ax
