//=============================================================================
/*
 * See the file README in the main directory for a description of
 * this software, copyright information, and how to reach the author.
 *
 * Author: alex
 * Date:   03.06.2004
 *
 * Last modfied:
 *   $Author: alex $
 *   $Date: 2005-09-13 11:03:31 +0200 (Di, 13 Sep 2005) $
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
//----- Vdr -------------------------------------------------------------------
//----- AxLib -----------------------------------------------------------------
#include <Ax/Tools/Trace.h>
// #include <Ax/Tools/TraceOn.h>
#include "Ax/Vdr/Thread.h"

//----- locals ----------------------------------------------------------------

//=============================================================================
//     namespaces
//=============================================================================
namespace Ax  {
namespace Vdr {

//=============================================================================
//     class Thread
//=============================================================================

//-------------------------------------------------------------------------
//     static
//-------------------------------------------------------------------------
bool Thread::myDebugFlag = false;

//-------------------------------------------------------------------------
//     Thread::Thread()
//-------------------------------------------------------------------------
Thread::Thread(const std::string &theDescription)
      : PARENT         (theDescription.c_str())
      , myName         (theDescription)
      , myStopRequested(false)
{
  wsdebug(("> Thread::Thread()"));
  wsdebug(("< Thread::Thread()"));
} // Thread::Thread()

//-------------------------------------------------------------------------
//     Thread::~Thread()
//-------------------------------------------------------------------------
Thread::~Thread()
{
  wsdebug(("> Thread::~Thread() Active: %d", isActive()));
  if (isActive())
  {
    if (myDebugFlag) wsinfo(("  Thread::~Thread() calling stop()..."));
    stop();
    if (myDebugFlag) wsinfo(("  Thread::~Thread() calling stop()...returned"));
  } // if
  wsdebug(("< Thread::~Thread() Active: %d", isActive()));
} // Thread::~Thread()

//-------------------------------------------------------------------------
//     Thread::getName()
//-------------------------------------------------------------------------
const std::string &Thread::getName() const
{
//  cMutexLock aLock(&myNameMutex);
  return myName;
} // Thread::getName()

//-------------------------------------------------------------------------
//     Thread::start()
//-------------------------------------------------------------------------
bool Thread::start()
{
  bool fRunning = true;

  wsdebug(("> Thread::start() Active: %d", isActive()));
  if (!isActive())
  {
    myStopRequested = false;
    if (myDebugFlag) wsinfo(("%s: starting thread...", getName().c_str()));
    fRunning = PARENT::Start();
    if (myDebugFlag) wsinfo(("%s: starting thread...%s", getName().c_str(), fRunning ? "OK" : "FAILED"));
  } // if
  wsdebug(("< Thread::start() Active: %d", isActive()));
  return fRunning;
} // Thread::start()

//-------------------------------------------------------------------------
//     Thread::stop()
//-------------------------------------------------------------------------
void Thread::stop(int theWaitSeconds)
{
  wsdebug(("> Thread::stop() Active: %d  WaitSecs: %d", isActive(), theWaitSeconds));
  if (isActive())
  {
    if (!stopRequested()) setStopRequested();

    if (myDebugFlag) wsinfo(("%s: cancelling and killing in %d secs...", getName().c_str(), theWaitSeconds));
    Cancel(theWaitSeconds);
    if (myDebugFlag) wsinfo(("%s: cancelling and killing in %d secs...done", getName().c_str(), theWaitSeconds));
  } // if
  wsdebug(("< Thread::stop() Active: %d  WaitSecs: %d", isActive(), theWaitSeconds));
} // Thread::stop()

//-------------------------------------------------------------------------
//     Thread::signalEvent()
//-------------------------------------------------------------------------
void Thread::signalEvent()
{
  if (isActive())
  {
    if (myDebugFlag) wsinfo(("  Thread::signalEvent() waking up thread..."));
    myWaitCondition.Signal();
  }
  else
  {
    if (myDebugFlag) wsinfo(("  Thread::signalEvent() Thread not active -> nothing to do!"));
  } // if
} // Thread::signalEvent()

//-------------------------------------------------------------------------
//     Thread::setStopRequested()
//-------------------------------------------------------------------------
void Thread::setStopRequested()
{
  if (isActive())
  {
    if (myDebugFlag) wsinfo(("%s: setting flag to stop the thread...", getName().c_str()));
//    {
      LOCK_THREAD;
      myStopRequested = true;
//    }
    if (myDebugFlag) wsinfo(("%s: signalling an event to the thread...", getName().c_str()));
    myWaitCondition.Signal();
    if (myDebugFlag) wsinfo(("%s: signalling an event to the thread...done", getName().c_str()));
 } // if
} // Thread::setStopRequested()

//-------------------------------------------------------------------------
//     Thread::stopRequested()
//-------------------------------------------------------------------------
bool Thread::stopRequested()
{
  LOCK_THREAD;
  return myStopRequested;
} // Thread::stopRequested()

//-------------------------------------------------------------------------
//     Thread::waitEvent()
//-------------------------------------------------------------------------
bool Thread::waitEvent(int theTimeOutMS, bool *fTimeOut)
{
  if (fTimeOut)
  {
    *fTimeOut = false;
  } // if

  bool fContinue = Running() && !stopRequested();

  if (!fContinue)
  {
    if (myDebugFlag) wsinfo(("%s: waitEvent: Running: %d  StopRequested: %d  fContinue: %d", getName().c_str(), Running(), stopRequested(), fContinue));
  }
  else
  {
    if (myDebugFlag) wsinfo(("%s: waiting for signal for %d ms...", getName().c_str(), theTimeOutMS));
    bool fSignal = myWaitCondition.Wait(theTimeOutMS);
    if (myDebugFlag) wsinfo(("%s: waiting for signal for %d ms...fSignal: %d", getName().c_str(), theTimeOutMS, fSignal));
    if (!fSignal && fTimeOut)
    {
      *fTimeOut = true;
    } // if

    fContinue = Running() && !stopRequested();
    if (!fContinue)
    {
      if (myDebugFlag) wsinfo(("%s: waitEvent: Running: %d  StopRequested: %d  fContinue: %d  Signal: %d"
                             , getName().c_str(), Running(), stopRequested(), fContinue, fSignal
                             ));
    } // if
  } // if

  return fContinue;
} // Thread::waitEvent()

//-------------------------------------------------------------------------
//     Thread::setDebugFlag()
//-------------------------------------------------------------------------
void Thread::setDebugFlag(bool fSet)
{
  myDebugFlag = fSet;
} // Thread::setDebugFlag()

//=============================================================================
//     namespaces
//=============================================================================
} // Vdr
} // Ax
