//=============================================================================
/*
 * See the file README in the main directory for a description of
 * this software, copyright information, and how to reach the author.
 *
 * Author: Alex
 * Date:   13.09.2005
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
//----- C ---------------------------------------------------------------------
//----- AxLib -----------------------------------------------------------------
#include <Ax/Tools/Globals.h>
#include <Ax/Tools/Trace.h>
// #include <Ax/Tools/TraceOn.h>
#include <Ax/Mail/MailBox.h>
#include <Ax/Mail/MailBoxMgr.h>
#include <Ax/Mail/MailBoxSetup.h>

//----- vdr -------------------------------------------------------------------
//----- local plugin ----------------------------------------------------------
//----- locals ----------------------------------------------------------------
#include "LogHandlerCBP.h"

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
//     class LogHandlerCBP
//=============================================================================

//-------------------------------------------------------------------------
//     LogHandlerCBP::LogHandlerCBP()
//-------------------------------------------------------------------------
LogHandlerCBP::LogHandlerCBP( LogHandler        &theLogHandler
                            , MailBox           *theMailBox
                            , const std::string &theName
                            , CallBackProcessor *theParent
                            )
             : PARENT      (theMailBox, theName, theParent)
             , myLogHandler(theLogHandler)
{
} // LogHandlerCBP::LogHandlerCBP()

//-------------------------------------------------------------------------
//     LogHandlerCBP::~LogHandlerCBP()
//-------------------------------------------------------------------------
LogHandlerCBP::~LogHandlerCBP()
{
} // LogHandlerCBP::~CallBackProcessor()

//---------------------------------------------------------------------
//     LogHandlerCBP::log()
//---------------------------------------------------------------------
bool LogHandlerCBP::log(LogLevel theLevel, const std::string &theMsg)
{
  if ( getMailBox() == 0
    || (theLevel != LL_DEBUG && theLevel != LL_CALLBACK)
    || getMailBox()->getSetup().getClientOptDbg() == CC_DBG_YES
     )
  {
    myLogHandler.log(theLevel, theMsg);
  } // if

  return true;
} // LogHandlerCBP::log()

//=========================================================================
//     process c-client callbacks
//=========================================================================

//-------------------------------------------------------------------------
//     LogHandlerCBP::process_mm_notify()
//-------------------------------------------------------------------------
bool LogHandlerCBP::process_mm_notify(const std::string &theMsg, long theErrorFlag)
{
  log(LogHandler::ErrFlag2LogLevel(theErrorFlag), theMsg);
  return true;
} // LogHandlerCBP::process_mm_notify()

//-------------------------------------------------------------------------
//     LogHandlerCBP::process_mm_log()
//-------------------------------------------------------------------------
bool LogHandlerCBP::process_mm_log(const std::string &theMsg, long theErrorFlag)
{
  log(LogHandler::ErrFlag2LogLevel(theErrorFlag), theMsg);
  return true;
} // LogHandlerCBP::process_mm_log()

//-------------------------------------------------------------------------
//     LogHandlerCBP::process_mm_dlog()
//-------------------------------------------------------------------------
bool LogHandlerCBP::process_mm_dlog(const std::string &theMsg)
{
  log(LL_DEBUG, theMsg);
  return true;
} // LogHandlerCBP::process_mm_dlog()

//-------------------------------------------------------------------------
//     LogHandlerCBP::process_mm_fatal()
//-------------------------------------------------------------------------
bool LogHandlerCBP::process_mm_fatal(const std::string &theMsg)
{
  log(LL_FATAL, theMsg);
  return true;
} // LogHandlerCBP::process_mm_fatal()

//=============================================================================
//     namespaces
//=============================================================================
} // Mail
} // Ax
