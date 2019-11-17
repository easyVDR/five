//=============================================================================
/*
 * See the file README in the main directory for a description of
 * this software, copyright information, and how to reach the author.
 *
 * Author: alex
 * Date:   07.09.2003
 *
 * Last modfied:
 *   $Author: alex $
 *   $Date: 2013-04-27 12:05:08 +0200 (Sa, 27 Apr 2013) $
 */
//=============================================================================

//=============================================================================
//     includes
//=============================================================================

//----- qt --------------------------------------------------------------------
//----- CORBA -----------------------------------------------------------------
//----- C++ -------------------------------------------------------------------
//----- C ---------------------------------------------------------------------
#include <stdio.h>
#include <stdarg.h>

#if 0
#ifndef WIN32
#include <sys/types.h>
#include <unistd.h>
#include <linux/unistd.h>
#include <errno.h>
#endif
#endif

//----- AxLib -----------------------------------------------------------------
#include <Ax/Tools/Trace.h>
#include <Ax/Tools/Globals.h>

//----- local -----------------------------------------------------------------

//=============================================================================
//     locals
//=============================================================================
#ifndef WIN32

//=============================================================================
//     LogFunctionPtr
//=============================================================================
AxTSysLogFunc AxSysLog = 0; //syslog;

//=============================================================================
//     Helper to enforce initialisation of AxSysLog
//=============================================================================
class AxSetLog
{
  public:
    AxSetLog();
};

AxSetLog::AxSetLog()
{
  Ax::Tools::Trace::setLogFunc(syslog);
} // AxSetLog::AxSetLog()

static AxSetLog SetLogHelper;


//=============================================================================
//     AxLogFunc()
//=============================================================================
void AxLogFunc(int theLevel, const char *theFormat, ...)
{
  const int MAXLOGLEN = 1024;

  va_list ap;
  char szBuf[MAXLOGLEN];
  snprintf( szBuf
          , sizeof(szBuf)
          , "[%ld:%ld] %s"
          , Ax::Tools::getPID()
          , Ax::Tools::getTID()
          , theFormat
          );
  va_start(ap, theFormat);
  vsyslog(theLevel, szBuf, ap);
  va_end(ap);
} // AxLogFunc()

//=============================================================================
//     AxTraceLogStdErr()
//=============================================================================
void AxTraceLogStdErr(int theLevel, const char *theFormat, ...)
{
  FILE *aFH = stderr;
  if (theLevel == LOG_INFO || theLevel == LOG_DEBUG)
  {
    aFH = stdout;
  } // if
  
  const int MAXLOGLEN = 1024;

  va_list ap;
  char szBuf[MAXLOGLEN];
  snprintf( szBuf
          , sizeof(szBuf)
          , "[%ld:%ld] %s\n"
          , Ax::Tools::getPID()
          , Ax::Tools::getTID()
          , theFormat
          );
  va_start(ap, theFormat);
  vfprintf(aFH, szBuf, ap);
  va_end(ap);
} // AxTraceLogStrdErr()

//=============================================================================
//     namespaces
//=============================================================================
namespace Ax    {
namespace Tools {
namespace Trace {

//---------------------------------------------------------------------
//     setLogFunc()
//---------------------------------------------------------------------
void setLogFunc(AxTSysLogFunc theLogFunc)
{
  if (theLogFunc != 0)
  {
    AxSysLog = theLogFunc;
  }
  else
  {
    AxSysLog = syslog;
  } // if

#if 0
  if (AxSysLog == syslog)
  {
    (*AxSysLog)(LOG_DEBUG, "Setting log to syslog()");
  }
  else
  {
    (*AxSysLog)(LOG_DEBUG, "Setting log to given function");
  } // if
#endif
} // setLogFunc()

//=============================================================================
//     namespaces
//=============================================================================
} // Trace
} // Tools
} // Ax
#endif
