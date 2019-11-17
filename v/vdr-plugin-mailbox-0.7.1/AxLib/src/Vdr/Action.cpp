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
 *   $Date: 2013-03-20 19:53:42 +0100 (Mi, 20 Mär 2013) $
 */
//=============================================================================

#define MEASURE_TIME 0

//=============================================================================
//     includes
//=============================================================================

//----- qt --------------------------------------------------------------------
//----- CORBA -----------------------------------------------------------------
//----- ICE -------------------------------------------------------------------
//----- C++ -------------------------------------------------------------------
//----- C ---------------------------------------------------------------------
#if MEASURE_TIME
#include <sys/time.h>
#endif

//----- Vdr -------------------------------------------------------------------
//----- AxLib -----------------------------------------------------------------
#include <Ax/Tools/Trace.h>
// #include <Ax/Tools/TraceOn.h>
#include <Ax/Tools/Macros.h>

#include "Ax/Vdr/Action.h"
#include "Ax/Vdr/Tools.h"

//----- locals ----------------------------------------------------------------

//=============================================================================
//     namespaces
//=============================================================================
namespace Ax  {
namespace Vdr {

//=============================================================================
//     class Action
//=============================================================================

//-------------------------------------------------------------------------
//     Action::Action()
//-------------------------------------------------------------------------
Action::Action(int theID, const std::string &theName, const std::string &theDesc)
      : myID         (theID  )
      , myName       (theName)
      , myDesc       (theDesc)
      , myLockCode   (""     )
      , myIsLocked   (false  )
      , myIsEnabled  (true   )
      , myIsSupported(true   )
{
  wsdebug(("> Action::Action() %d: %s", myID, myName.c_str()));
  wsdebug(("< Action::Action() %d: %s", myID, myName.c_str()));
} // Action::Action()

//-------------------------------------------------------------------------
//     Action::~Action()
//-------------------------------------------------------------------------
Action::~Action()
{
  wsdebug(("> Action::~Action() %d: %s", myID, myName.c_str()));
  wsdebug(("< Action::~Action() %d: %s", myID, myName.c_str()));
} //  Action::~Action()

//-------------------------------------------------------------------------
//     Action::precheck()
//-------------------------------------------------------------------------
bool Action::precheck(eOSState theStateBefore)
{
  wsdebug(("> Action::precheck() %s  StateBefore: %-10s"
         , myName.c_str()
         , Ax::Vdr::Tools::getOSStateStr(theStateBefore)
         ));

  bool fOK = precheckInternal(theStateBefore);
  wsdebug(("> Action::precheck() %s  fOK: %d", myName.c_str(),fOK));
  return fOK;
} // Action::precheck()

//-------------------------------------------------------------------------
//     Action::run()
//-------------------------------------------------------------------------
eOSState Action::run(eOSState theStateBefore)
{
  wsdebug(("> Action::run() %s  StateBefore: %-10s Enabled: %d  Locked: %d"
         , myName.c_str()
         , Ax::Vdr::Tools::getOSStateStr(theStateBefore)
         , isEnabled(), isLocked()
         ));
  eOSState aState = osUnknown;

  if (!isSupported())
  {
    wswarn(("  Action::run() %s  IS NOT SUPPORTED", myName.c_str()));
  }
  else if (!isEnabled())
  {
    wswarn(("  Action::run() %s  IS NOT ENABLED", myName.c_str()));
  }
  else if (isLocked())
  {
    wswarn(("  Action::run() %s  IS LOCKED", myName.c_str()));
  }
  else
  {
    wsdebug(("  Action::run() %s  running action...", myName.c_str()));

#if MEASURE_TIME
    timeval tv1;
    gettimeofday(&tv1, NULL);
#endif

    aState = runInternal(theStateBefore);

#if MEASURE_TIME
    timeval tv2;
    gettimeofday(&tv2, NULL);

    wsinfo(("  Action::run() %ld ms  for '%s'"
          , ((long(tv2.tv_sec) - long(tv1.tv_sec)) * 1000000 + (long(tv2.tv_usec) - long(tv1.tv_usec)))
            / 1000 /* us -> ms */
          , myName.c_str()
          ));
#endif

    wsdebug(("  Action::run() %s  running action...returned: %s"
           , myName.c_str()
           , Ax::Vdr::Tools::getOSStateStr(aState)
           ));

    if (getLockCode().length() > 0)
    {
      wsdebug(("  Action::run() %s  locking again after execution", myName.c_str()));
      setLocked(true);
    } // if

    wsdebug(("  Action::run() %s  running action... returned: %s", myName.c_str(), Ax::Vdr::Tools::getOSStateStr(aState)));
  } // if

  if (aState == osUnknown || aState == theStateBefore)
  {
    wsdebug(("  Action::run() %s  StateAfter: %s  Before: %s  setting to osContinue"
          , myName.c_str()
          , Ax::Vdr::Tools::getOSStateStr(aState)
          , Ax::Vdr::Tools::getOSStateStr(theStateBefore)
          ));
    aState = osContinue;
  } // if

  wsdebug(("< Action::run() %s  StateAfter : %s", myName.c_str(), Ax::Vdr::Tools::getOSStateStr(aState)));
  return aState;
} //  Action::run()

//-------------------------------------------------------------------------
//     Action::isSupported()
//-------------------------------------------------------------------------
bool Action::isSupported() const
{
  return myIsSupported;
} // Action::isSupported)

//-------------------------------------------------------------------------
//     Action::setSupported()
//-------------------------------------------------------------------------
void Action::setSupported(bool fSupported)
{
  myIsSupported = fSupported;
} // Action::setSupported()

//-------------------------------------------------------------------------
//     Action::isEnabled()
//-------------------------------------------------------------------------
bool Action::isEnabled() const
{
  return myIsEnabled && isSupported();
} // Action::isEnabled()

//-------------------------------------------------------------------------
//     Action::setEnabled()
//-------------------------------------------------------------------------
void Action::setEnabled(bool fEnabled)
{
  if (isEnabled() != fEnabled)
  {
    wsdebug(("  Action::setEnabled() %s  setting fEnabled to %d", myName.c_str(), fEnabled));
    myIsEnabled = fEnabled;
  } // if
} // if

//-------------------------------------------------------------------------
//     Action::isLocked()
//-------------------------------------------------------------------------
bool Action::isLocked() const
{
  return myIsLocked;
} // Action::isLocked()

//-------------------------------------------------------------------------
//     Action::setLocked()
//-------------------------------------------------------------------------
void Action::setLocked(bool fLocked)
{
  if (isLocked() != fLocked)
  {
    wsdebug(("  Action::setLocked() %s  setting fLocked to %d", myName.c_str(), fLocked));
    myIsLocked = fLocked;
  } // if
} // Action::isLocked()

//-------------------------------------------------------------------------
//     Action::getLockCode()
//-------------------------------------------------------------------------
std::string Action::getLockCode() const
{
  return myLockCode;
} // Action::getLockCode()

//-------------------------------------------------------------------------
//     Action::setLockCode()
//-------------------------------------------------------------------------
void Action::setLockCode(const std::string &theLockCode)
{
  wsdebug(("  Action::setLockCode() %s  Len: %d  Code: %s",  myName.c_str(), theLockCode.length(), theLockCode.c_str()));
  myLockCode = theLockCode;
  myIsLocked = theLockCode.length() > 0;
} // Action::setLockCode()

//-------------------------------------------------------------------------
//     Action::getButtonText()
//-------------------------------------------------------------------------
const char *Action::getButtonText() const
{
  if (isEnabled())
  {
    return getButtonTextInternal();
  } //
  return 0;
} // Action::getButtonText()

//-------------------------------------------------------------------------
//     Action::getButtonTextInternal()
//-------------------------------------------------------------------------
const char *Action::getButtonTextInternal() const
{
  return tr("BASE");
} // Action::getButtonTextInternal()

//-------------------------------------------------------------------------
//    Action::precheckInternal()
//-------------------------------------------------------------------------
bool Action::precheckInternal(eOSState theStateBefore)
{
  AX_UNUSED_ARG(theStateBefore);
  return true;
} // Action::precheckInternal()

//=============================================================================
//     class ActionReturnConstState
//=============================================================================

//-------------------------------------------------------------------------
//    ActionReturnConstState::ActionReturnConstState()
//-------------------------------------------------------------------------
ActionReturnConstState::ActionReturnConstState( eOSState           theState
                                              , int                theID
                                              , const char        *theButtonText
                                              , const std::string &theName
                                              , const std::string &theDesc
                                              )
                        : PARENT(theID, theName, theDesc)
                        , myButtonText(theButtonText)
                        , myState     (theState)
{
} // ActionReturnConstState::ActionReturnConstState()

//-------------------------------------------------------------------------
//    ActionReturnConstState::~ActionReturnConstState()
//-------------------------------------------------------------------------
ActionReturnConstState::~ActionReturnConstState()
{
} // ActionReturnConstState::~ActionReturnConstState()

//-------------------------------------------------------------------------
//     ActionReturnConstState::getButtonTextInternal()
//-------------------------------------------------------------------------
const char *ActionReturnConstState::getButtonTextInternal() const
{
  return myButtonText;
} // ActionReturnConstState::getButtonTextInternal()

//-------------------------------------------------------------------------
//     ActionReturnConstState::runInternal()
//-------------------------------------------------------------------------
eOSState ActionReturnConstState::runInternal(eOSState theStateBefore)
{
  AX_UNUSED_ARG(theStateBefore);
  return myState;
} // ActionReturnConstState::runInternal()

//=============================================================================
//     namespaces
//=============================================================================
} // Vdr
} // Ax
