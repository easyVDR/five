//=============================================================================
/*
 * See the file README in the main directory for a description of
 * this software, copyright information, and how to reach the author.
 *
 * Author: alex
 * Date:   13.09.2005
 *
 * Last modfied:
 *   $Author: alex $
 *   $Date: 2005-07-10 18:58:38 +0200 (So, 10 Jul 2005) $
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
#include <Ax/Tools/Trace.h>
// #include <Ax/Tools/TraceOn.h>
#include <Ax/Mail/LogHandler.h>

//----- vdr -------------------------------------------------------------------
//----- local -----------------------------------------------------------------
#include "c-client-header.h"

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
//     class LogRecord
//=============================================================================

//---------------------------------------------------------------------
//     LogRecord::LogRecord()
//---------------------------------------------------------------------
LogRecord::LogRecord(const std::string theMsg, LogLevel theLevel)
         : myLevel(theLevel)
         , myMsg  (theMsg)
{
} // LogRecord::LogRecord()

//---------------------------------------------------------------------
//     LogRecord::~LogRecord()
//---------------------------------------------------------------------
LogRecord::~LogRecord()
{
} // LogRecord::~LogRecord()

//---------------------------------------------------------------------
//     LogRecord::tmsg()
//---------------------------------------------------------------------
std::string LogRecord::tmsg() const
{
  return std::string("[") + LogHandler::Level2Char(level()) + std::string("] ") + msg();
} // LogRecord::tmsg()

//=============================================================================
//     class LogHandler
//=============================================================================

//---------------------------------------------------------------------
//     LogHandler::LogHandler()
//---------------------------------------------------------------------
LogHandler::LogHandler(const std::string &theName)
          : myName(theName)
          , myMaxSize(1)
{
  wsdebug(("- LogHandler::LogHandler() %p", this));
} // LogHandler::LogHandler()

//---------------------------------------------------------------------
//     LogHandler::~LogHandler()
//---------------------------------------------------------------------
LogHandler::~LogHandler()
{
  wsdebug(("- LogHandler::~LogHandler() %p", this));
} // LogHandler::~LogHandler()

//-------------------------------------------------------------------------
//     LogHandler::ErrFlag2LogLevel()
//-------------------------------------------------------------------------
LogLevel LogHandler::ErrFlag2LogLevel(long theErrorFlag)
{
  LogLevel aLevel = LL_UNKNOWN;

  switch (theErrorFlag)
  {
    case NIL     : aLevel = LL_INFO   ; break;  // mm_log no error
    case WARN    : aLevel = LL_WARN   ; break;  // mm_log warning type
    case ERROR   : aLevel = LL_ERROR  ; break;  // mm_log error type
    case PARSE   : aLevel = LL_WARN   ; break;  // mm_log parse error type
    case BYE     : aLevel = LL_BYE    ; break;  // mm_notify stream dying
    case TCPDEBUG: aLevel = LL_INFO   ; break;  // mm_log TCP debug babble
    default      : aLevel = LL_UNKNOWN; break;
  } // switch

  return aLevel;
} // LogHandler::ErrFlag2LogLevel()

//---------------------------------------------------------------------
//     LogHandler::Level2Char()
//---------------------------------------------------------------------
std::string LogHandler::Level2Char(LogLevel theLevel)
{
  std::string aStr = "?";

  switch (theLevel)
  {
    case LL_INVALID : aStr = "V"; break;
    case LL_FATAL   : aStr = "F"; break;
    case LL_ERROR   : aStr = "E"; break;
    case LL_WARN    : aStr = "W"; break;
    case LL_INFO    : aStr = "I"; break;
    case LL_DEBUG   : aStr = "D"; break;
    case LL_BYE     : aStr = "B"; break;
    case LL_CALLBACK: aStr = "C"; break;
    case LL_UNKNOWN : aStr = "U"; break;
  } // switch

  return aStr;
} // LogHandler::Level2Char()

//---------------------------------------------------------------------
//     LogHandler::getRecords()
//---------------------------------------------------------------------
const LogCltn &LogHandler::getRecords() const
{
  return myLogCltn;
} // LogHandler::getRecords()

//---------------------------------------------------------------------
//     LogHandler::getLastMsg()
//---------------------------------------------------------------------
std::string LogHandler::getLastMsg(LogLevel theLevel) const
{
  std::string aMsg = "";
  for (LogCltn::const_reverse_iterator anIter  = myLogCltn.rbegin();
                                       anIter != myLogCltn.rend();
                                       anIter++)
  {
    if (anIter->level() == theLevel)
    {
      aMsg = anIter->msg();
      break;
    } // if
  } // for

  return aMsg;
} // LogHandler::getLastMsg()

//---------------------------------------------------------------------
//     LogHandler::getLastImportantMsg()
//---------------------------------------------------------------------
std::string LogHandler::getLastImportantMsg(const std::string &theMsgIfNone) const
{
  std::string aMsg = "";

  if (myLogCltn.size() > 0)
  {
    for (int i = LL_FATAL; i <= LL_UNKNOWN; ++i)
    {
      aMsg = getLastMsg(LogLevel(i));
      if (!aMsg.empty())
      {
        break;
      } // if
    } // for
  } // if

  if (aMsg.empty()) aMsg = theMsgIfNone;

  return aMsg;
} // LogHandler::getLastImportantMsg()

//---------------------------------------------------------------------
//     LogHandler::isEmpty()
//---------------------------------------------------------------------
bool LogHandler::isEmpty() const
{
  wsdebug(("- LogHandler::isEmpty() %p size: %d", this, myLogCltn.size()));
  return myLogCltn.size() == 0;
} // LogHandler::isEmpty()

//---------------------------------------------------------------------
//     LogHandler::clear()
//---------------------------------------------------------------------
void LogHandler::clear()
{
  wsdebug(("- LogHandler::clear()   %p size: %d", this, myLogCltn.size()));
  myLogCltn.clear();
} // LogHandler::clear()

//---------------------------------------------------------------------
//     LogHandler::setMaxSize()
//---------------------------------------------------------------------
void LogHandler::setMaxSize(LogCltn::size_type theMaxSize)
{
  // store at least one LogRecord
  if (theMaxSize < 1) theMaxSize = 1;

  if ( theMaxSize       != myMaxSize
    || myLogCltn.size() != myMaxSize
     )
  {
    myMaxSize = theMaxSize;

    if (myMaxSize == 0)
    {
      myLogCltn.clear();
    }
    else
    {
      while (myLogCltn.size() > myMaxSize)
      {
        myLogCltn.pop_front();
      } // while
    } // if
  } // if
} // LogHandler::setMaxSize()

//---------------------------------------------------------------------
//     LogHandler::log()
//---------------------------------------------------------------------
void LogHandler::log(LogLevel theLevel, const std::string &theMsg)
{
  LogRecord aLogRecord(theMsg, theLevel);

  if (myMaxSize == 1)
  {
    // store the last most important mail
    if (myLogCltn.size() > 0 && myLogCltn.front().level() >= theLevel)
    {
      myLogCltn.clear();
    } // if

    if (myLogCltn.size() == 0)
    {
      myLogCltn.push_back(aLogRecord);
    } // if
  }
  else
  {
    myLogCltn.push_back(aLogRecord);
    setMaxSize(myMaxSize);
  } // if

  switch (theLevel)
  {
    case LL_FATAL   : /* F */ wserror(("FATAL %s: %s", myName.c_str(), aLogRecord.msg().c_str())); break;
    case LL_ERROR   : /* E */ wserror(("ERROR %s: %s", myName.c_str(), aLogRecord.msg().c_str())); break;
    case LL_WARN    : /* W */ wswarn (("WARN  %s: %s", myName.c_str(), aLogRecord.msg().c_str())); break;
    case LL_BYE     : /* B */ wsinfo (("BYE   %s: %s", myName.c_str(), aLogRecord.msg().c_str())); break;
    case LL_INFO    : /* I */ wsinfo (("INFO  %s: %s", myName.c_str(), aLogRecord.msg().c_str())); break;
    case LL_DEBUG   : /* D */ wsinfo (("DEBUG %s: %s", myName.c_str(), aLogRecord.msg().c_str())); break;
    case LL_UNKNOWN : /* U */ wsinfo (("UNKWN %s: %s", myName.c_str(), aLogRecord.msg().c_str())); break;
    case LL_CALLBACK: /* C */ wsinfo (("CLLBK %s: %s", myName.c_str(), aLogRecord.msg().c_str())); break;
    case LL_INVALID : /* N */ wsinfo (("BYE   %s: %s", myName.c_str(), aLogRecord.msg().c_str())); break;
    default         : /* ? */ wsinfo (("%3d   %s: %s", theLevel, myName.c_str(), aLogRecord.msg().c_str())); break;
  } // switch
} // LogHandler::log()


//=============================================================================
//     namespaces
//=============================================================================
} // Mail
} // Ax
