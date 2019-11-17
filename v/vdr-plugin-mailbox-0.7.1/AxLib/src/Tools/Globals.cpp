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
 *   $Date: 2013-03-20 19:53:42 +0100 (Mi, 20 Mär 2013) $
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

#ifdef WIN32
#include <windows.h>
#elif defined ANDROID
#include <QThread>
#include <QProcess>
#else
#include <sys/types.h>
#include <unistd.h>
#include <linux/unistd.h>
#include <errno.h>
#endif

//----- AxLib -----------------------------------------------------------------
#include <Ax/Tools/Trace.h>
// #include <Ax/Tools/TraceOn.h>
#include <Ax/Tools/Globals.h>

//----- local -----------------------------------------------------------------

//=============================================================================
//     namespaces
//=============================================================================
namespace Ax    {
namespace Tools {

//---------------------------------------------------------------------
//     getPID()
//---------------------------------------------------------------------
long getPID()
{
#ifdef WIN32
  return GetCurrentProcessId();
#elif defined ANDROID
  return 0; // QProcess::pid();
#else
  return getpid();
#endif
} // getTID()

//---------------------------------------------------------------------
//     getTID()
//---------------------------------------------------------------------
long getTID()
{
#ifdef WIN32
  return long(GetCurrentThreadId());
#elif defined ANDROID
  return long(QThread::currentThreadId());
#else
  return syscall(__NR_gettid);
#endif
} // getTID()

//=============================================================================
//     namespaces
//=============================================================================
} // Tools
} // Ax
