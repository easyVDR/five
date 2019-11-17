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
 *   $Date: 2014-10-26 11:19:44 +0100 (So, 26 Okt 2014) $
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
#include <algorithm>
#include <iterator>
#include <list>
#include <string>

//----- C ---------------------------------------------------------------------
#if MEASURE_TIME
#include <sys/time.h>
#endif

//----- Vdr -------------------------------------------------------------------
//----- AxLib -----------------------------------------------------------------
#ifdef TRACE_WAS_ON
#undef TRACE_WAS_ON
#endif

#if TRACE_IS_ON
#define OSD_MENU_TRACE_WAS_ON 1
#else
#define OSD_MENU_TRACE_WAS_ON 0
#endif

#include <Ax/Tools/Trace.h>

#if 0
#include <Ax/Tools/TraceOn.h>
#else
#include <Ax/Tools/TraceOff.h>
#endif
#include <Ax/Tools/Globals.h>
#include <Ax/Tools/Macros.h>
#include <Ax/Vdr/MenuCode.h>
#include <Ax/Vdr/OsdMenu.h>
#include <Ax/Vdr/Tools.h>
#include <Ax/Vdr/Action.h>
#include <Ax/Vdr/MenuHelpKeys.h>

//----- locals ----------------------------------------------------------------

//=============================================================================
//     using namespaces
//=============================================================================
using namespace std;

//=============================================================================
//     defines
//=============================================================================
/** following define is taken from Vdr.c
 */
#define MENUTIMEOUT 120

//=============================================================================
//     namespaces
//=============================================================================
namespace Ax  {
namespace Vdr {

//=============================================================================
//     class OsdMenuBase
//=============================================================================

//-----------------------------------------------------------------------------
//     OsdMenuBase::OsdMenuBase()
//-----------------------------------------------------------------------------
template <typename TT>
OsdMenuBase<TT>::OsdMenuBase( const std::string &theMenuName
                            , const std::string &theTitle
                            , int c0, int c1, int c2, int c3, int c4
                            )
                 : PARENT(theTitle.c_str(), c0, c1, c2, c3, c4)
                 , myMenuName     (theMenuName)
                 , myPendingAction(0)
                 , mySubAction    (0)
                 , myButtonPage   (-1)
                 , myUpdateColorButtons(false)
{
  wsdebug(("> OsdMenuBase<%s>::OsdMenuBase()", myMenuName.c_str()));

  init();

  wsdebug(("< OsdMenuBase<%s>::OsdMenuBase()", myMenuName.c_str()));
} // OsdMenuBase::OsdMenuBase()

//-----------------------------------------------------------------------------
//     OsdMenuBase::OsdMenuBase()
//-----------------------------------------------------------------------------
template <typename TT>
OsdMenuBase<TT>::OsdMenuBase(const std::string &theInstName)
                 : PARENT()
                 , myMenuName     (theInstName)
                 , myPendingAction(0)
                 , mySubAction    (0)
                 , myButtonPage   (-1)
                 , myUpdateColorButtons(false)
{
  wsdebug(("> OsdMenuBase<%s>::OsdMenuBase()", myMenuName.c_str()));

  init();

  wsdebug(("< OsdMenuBase<%s>::OsdMenuBase()", myMenuName.c_str()));
} // OsdMenuBase::OsdMenuBase()

//-------------------------------------------------------------------------
//     OsdMenuBase::init()
//-------------------------------------------------------------------------
template <typename TT>
void OsdMenuBase<TT>::init()
{
  addAction(new ActionReturnConstState(osContinue, A_IGNORE , NULL, "IgnoreKey", tr("Ignore this key"       )));
  addAction(new ActionReturnConstState(osBack    , A_GO_BACK, NULL, "GoBack"   , tr("Drop changes & go back")));

  addAction(new ActionCallback<MYSELF>( this
                                      , A_HELP_KEYS
                                      , tr("Key mapping")
                                      , "HelpKeys"
                                      , tr("Show help for usable keys")
                                      ));
  setKeyAction(Key_HelpKeys, A_HELP_KEYS);
#if VdrVERSNUM >= 10513
  setKeyAction(kInfo       , A_HELP_KEYS);
#endif
} // OsdMenuBase::init()

//-----------------------------------------------------------------------------
//     OsdMenuBase::~OsdMenuBase()
//-----------------------------------------------------------------------------
template <typename TT>
OsdMenuBase<TT>::~OsdMenuBase()
{
  wsdebug(("> OsdMenuBase<%s>::~OsdMenuBase()", myMenuName.c_str()));

  //----- delete all button pages -----
  {
    for(ButtonPage2Actions::iterator anIter  = myButtonActions.begin();
                                     anIter != myButtonActions.end();
                                   ++anIter)
    {
      DELETENULL(anIter->second);
    } // for
  }

  //----- delete all actions -----
  {
    for (ID2ActionMap::iterator anIter  = myActions.begin();
                                anIter != myActions.end();
                              ++anIter)
    {
#if 0
      wsdebug(("  OsdMenuBase<%s>::~OsdMenuBase() Deleting action %p %s"
            , myMenuName.c_str()
            , anIter->second
            , anIter->second ? anIter->second->getName().c_str() : "NULL"
            ));
#endif
      DELETENULL(anIter->second);
    } // for
  }
  wsdebug(("< OsdMenuBase<%s>::~OsdMenuBase()", myMenuName.c_str()));
} // OsdMenuBase::~OsdMenuBase()

//-------------------------------------------------------------------------
//     OsdMenuBase::getCurrentItem()
//-------------------------------------------------------------------------
template <typename TT>
cOsdItem *OsdMenuBase<TT>::getCurrentItem()
{
  return TT::Current() >= 0 ? TT::Get(TT::Current()) : 0;
} // OsdMenuBase::getCurrentItem()

//-------------------------------------------------------------------------
//     Display()
//-------------------------------------------------------------------------
template <typename TT>
void OsdMenuBase<TT>::Display(void)
{
  wsdebug(("> OsdMenuBase<%s>::Display()", myMenuName.c_str()));

  wsdebug(("  OsdMenuBase<%s>::Display() calling updateColorButtons()...", myMenuName.c_str()));
  updateColorButtons();

  wsdebug(("  OsdMenuBase<%s>::Display() calling PARENT::Display()...", myMenuName.c_str()));
  PARENT::Display();

#if 0
  wsdebug(("  OsdMenuBase<%s>::Display() calling drawStatusMsg()...", myMenuName.c_str()));
  drawStatusMsg();
#endif
  wsdebug(("< OsdMenuBase<%s>::Display()", myMenuName.c_str()));
} // OsdMenuBase::Display()

//-------------------------------------------------------------------------
//     debugProcessKey()
//-------------------------------------------------------------------------
template <typename TT>
void OsdMenuBase<TT>::debugProcessKey(char c, eKeys theKey, eOSState theState, const std::string &theMsg)
{
  AX_UNUSED_ARG(c);
  AX_UNUSED_ARG(theKey);
  AX_UNUSED_ARG(theState);
  AX_UNUSED_ARG(theMsg);
#if 1
  if (theKey != kNone)
  {
#if 1
    if (!TT::HasSubMenu())
#endif
    {
      wsdebug(("%c OsdMenuBase<%s>::ProcessKey() Key: %-10s  Cur: %3d  HasSub: %d  State: %-10s  %s"
            , c
            , myMenuName.c_str()
            , Ax::Vdr::Tools::getKeyStr(theKey).c_str()
            , TT::Current()
            , TT::HasSubMenu()
            , Ax::Vdr::Tools::getOSStateStr(theState)
            , theMsg.c_str()
            ));
    } // if
  } // if
#endif
} // OsdMenuBase::debugProcessKey()

//-------------------------------------------------------------------------
//     OsdMenuBase::ProcessKey()
//-------------------------------------------------------------------------
template <typename TT>
eOSState OsdMenuBase<TT>::ProcessKey(eKeys theKey)
{
  debugProcessKey('>', theKey, osUnknown);

  //----- clear timed status msg ------------------------------------------
  if (!TT::HasSubMenu())
  {
    if (!myStatusMsg.empty() && myStatusTimeout.TimedOut())
    {
      clearStatusMsg();
    } // if
  } // if

  //----- store current item and wether we have a submenu ----------------
  bool      fHadSubMenu    = TT::HasSubMenu();
  int       aOldCurrentIdx = TT::Current();
  cOsdItem *aOldCurrent    = getCurrentItem();

  //----- parent processing: process items or submenus -------------------
  eOSState aState = PARENT::ProcessKey(theKey);

  debugProcessKey(' ', theKey, aState, "after PARENT::processKey()");

  // Hack: cMenuSetupPage returns osBack for theKey == kOk and therefore
  //       the key seems to be processed and the menu will get closed.
  //       -> I don't want to close the setup-menu by kOk until I explicitely
  //          close the menu.
  if (theKey == kOk && aState == osBack && dynamic_cast<cMenuSetupPage *>(this) != 0)
  {
#if 0
    wswarn(("  OsdMenuBase<%s>::ProcessKey() HACK:cMenuSetupPage shouldn't close for key kOk!"
          , myMenuName.c_str()
          ));
#endif
    aState = osUnknown;

    debugProcessKey(' ', theKey, aState, "after cMenuSetupPage-Hack!");
  } // if

  //----- process only if no submenu present -----------------------------
  if (!TT::HasSubMenu())
  {
    //----- submenu disappeared --------------------------------------------
    if (fHadSubMenu)
    {
      if (mySubAction)
      {
        // Submenu was closed -> inform myself
        informSubOSDClosed(*mySubAction, theKey);
        mySubAction = 0;
      } // if

      if (myPendingAction && !myPendingAction->isLocked())
      {
        Action *aTmpAction = myPendingAction;
        myPendingAction = 0;

        aState = aTmpAction->run(aState);
        if (TT::HasSubMenu())
        {
          // Submenu was opened -> store initiating action and inform myself
          mySubAction = aTmpAction;
          informSubOSDOpened(*mySubAction, theKey);
        } // if
      }
      else
      {
        // fIgnore = true;
      } // if
    } // if

    debugProcessKey(' ', theKey, aState, "in the middle 2");

    //----- select action -------------------------------------------------------
    if (aState == osUnknown)
    {
      //----- execute action ----------------------------------------------------
      Action *anAction = this->getKeyAction(theKey);
      if ( anAction
        && anAction->isEnabled()
        && anAction->precheck(aState)
        )
      {
        if (anAction->isLocked())
        {
          // precheck ok, but Action is locked -> show MenuCode
          myPendingAction = anAction;

          cOsdMenu *anOsdMenu = new MenuCode(anAction);
          aState = TT::AddSubMenu(anOsdMenu);
        }
        else
        {
          // precheck ok and not locked -> execute Action
          aState = anAction->run(aState);
          if (TT::HasSubMenu())
          {
            // Submenu was opened -> store initiating action and inform myself
            mySubAction = anAction;
            informSubOSDOpened(*mySubAction, theKey);
          } // if
        } // if
      }
      else if (anAction && !anAction->isEnabled())
      {
        aState = osContinue;
      } // if
    } // if

    //----- no submenu, current changed -> inform myself ------------------------
    if (getCurrentItem() != aOldCurrent)
    {
#if MEASURE_TIME
      timeval tv1;
      gettimeofday(&tv1, NULL);
#endif

      informCurrentChanged(aOldCurrentIdx);

#if MEASURE_TIME
      timeval tv2;
      gettimeofday(&tv2, NULL);

      wsinfo(("  OsdMenuBase<%s>::ProcessKey() %ld ms for informCurrentChanged()"
            , myMenuName.c_str()
            , ((long(tv2.tv_sec) - long(tv1.tv_sec)) * 1000000 + (long(tv2.tv_usec) - long(tv1.tv_usec)))
              / 1000 /* us -> ms */
            ));
#endif
    } // if

#if 1
    // workaround the behaviour of Vdr not to remember the current
    // color-keys when an editing item is activated / not restoring
    // the previous color buttons
    if (theKey == kOk && aState == osContinue && !TT::HasSubMenu() && !fHadSubMenu)
    {
      debugProcessKey(' ', theKey, aState, "EDITING FINISHED!");
      setUpdateColorButtons();
    } // if
#endif
    if (myUpdateColorButtons)
    {
      updateColorButtons();
    } // if
  } // if (!HasSubMenu())

  debugProcessKey('<', theKey, aState);

  return aState;
} // OsdMenuBase::ProcessKey()

//-------------------------------------------------------------------------
//     OsdMenuBase::processAction()
//-------------------------------------------------------------------------
template <typename TT>
eOSState OsdMenuBase<TT>::processAction(const Ax::Vdr::Action &theAction, eOSState theState)
{
  wsdebug(("> OsdMenuBase<%s>::ProcessKey() Action: %2d State: %-10s  "
        , myMenuName.c_str()
        , theAction.getID()
        , Ax::Vdr::Tools::getOSStateStr(theState)
        ));

  switch (theAction.getID())
  {
    case A_HELP_KEYS:
    {
      theState = TT::AddSubMenu(new MenuHelpKeys<TT>(this));
      break;
    }
    default:
    {
    }
  } // switch

  wsdebug(("< OsdMenuBase<%s>::ProcessKey() Action: %2d State: %-10s  "
        , myMenuName.c_str()
        , theAction.getID()
        , Ax::Vdr::Tools::getOSStateStr(theState)
        ));
  return theState;
} // OsdMenuBase::processAction()

//-------------------------------------------------------------------------
//     OsdMenuBase::addAction()
//-------------------------------------------------------------------------
template <typename TT>
void OsdMenuBase<TT>::addAction(Action *theAction)
{
  if (theAction->getID() == A_NONE)
  {
    wswarn(("OsdMenuBase<%s>::addAction() unable to assign an action to ID: %d = NONE"
          , myMenuName.c_str()
          , theAction->getID()
          ));

    // because this method is declared to take ownership of theAction
    // and because we don't store theAction, we simply delete it.
    delete theAction;
    return;
  } // if

  Action *anOldAction = 0;

  //----- check if an action with the given ID is already present -----
  ID2ActionMap::iterator anActionIter = myActions.find(theAction->getID());
  if (anActionIter != myActions.end())
  {
    anOldAction = anActionIter->second;

    if (anOldAction != theAction)
    {
      //----- check whether the action is assigned to a key and remove the entry
      std::list<int> aKeyList;
      for (Key2ActionMap::const_iterator aKeyIter  = myKeyActions.begin();
                                         aKeyIter != myKeyActions.end();
                                       ++aKeyIter)
      {
        if (aKeyIter->second == anOldAction)
        {
          wsdebug(("  OsdMenuBase::addAction() OldAction is assigned to key %d", aKeyIter->first));
          aKeyList.push_back(aKeyIter->first);
        } // if
      } // for

      while (aKeyList.size() > 0)
      {
        myKeyActions.erase(aKeyList.front());
        aKeyList.pop_front();
      } // while

      delete anOldAction;
    } // if
  } // if

  //----- save the new action -----
  if (theAction && theAction != anOldAction)
  {
    myActions[theAction->getID()] = theAction;
  } // if

} // OsdMenuBase::addAction()

//-------------------------------------------------------------------------
//     OsdMenuBase::getAction()
//-------------------------------------------------------------------------
template <typename TT>
Action *OsdMenuBase<TT>::getAction(int theID) const
{
  Action *anAction = 0;

  if (theID != A_NONE)
  {
    ID2ActionMap::const_iterator anIter = myActions.find(theID);
    if (anIter != myActions.end())
    {
      anAction = anIter->second;
    }
    else
    {
      wswarn(("OsdMenuBase<%s>::getAction() action with ID %d not present!"
            , myMenuName.c_str()
            , theID
            ));
    } // if
  } // if

  return anAction;
} //  OsdMenuBase::getAction()

//-------------------------------------------------------------------------
//     OsdMenuBase::setActionSupported()
//-------------------------------------------------------------------------
template <typename TT>
void OsdMenuBase<TT>::setActionSupported(int theID, bool fSupported)
{
  ID2ActionMap::iterator anIter = myActions.find(theID);
  if (anIter != myActions.end())
  {
    if (anIter->second->isSupported() != fSupported)
    {
      anIter->second->setSupported(fSupported);
      if (isButtonAction(theID)) setUpdateColorButtons();
    } // if
  }
  else
  {
    wswarn(("OsdMenuBase<%s>::setActionSupported() action with ID %d not present!"
          , myMenuName.c_str()
          , theID
          ));
  } // if
} // OsdMenuBase::setActionSupported()

//-------------------------------------------------------------------------
//     OsdMenuBase::isActionSupported()
//-------------------------------------------------------------------------
template <typename TT>
bool OsdMenuBase<TT>::isActionSupported(int theID) const
{
  bool fFeasible = false;

  ID2ActionMap::const_iterator anIter = myActions.find(theID);
  if (anIter != myActions.end())
  {
    fFeasible = anIter->second->isSupported();
  } // if

  return fFeasible;
} // OsdMenuBase::isActionSupported()

//-------------------------------------------------------------------------
//     OsdMenuBase::setActionEnabled()
//-------------------------------------------------------------------------
template <typename TT>
void OsdMenuBase<TT>::setActionEnabled(int theID, bool fEnabled)
{
  ID2ActionMap::iterator anIter = myActions.find(theID);
  if (anIter != myActions.end())
  {
    Action *anAction         = anIter->second;
    bool    fIsButtonAction  = isButtonAction(theID);
    std::string aButtonText1 = fIsButtonAction && anAction->getButtonText()
                             ? std::string(anAction->getButtonText())
                             : std::string("");

    if (anAction->isEnabled() != fEnabled)
    {
      anAction->setEnabled(fEnabled);
    } // if

    std::string aButtonText2 = fIsButtonAction && anAction->getButtonText()
                             ? std::string(anAction->getButtonText())
                             : std::string("");

    if (fIsButtonAction && aButtonText1 != aButtonText2)
    {
      setUpdateColorButtons();
    } // if
  }
  else
  {
    wswarn(("OsdMenuBase<%s>::setActionEnabled() action with ID %d not present!"
          , myMenuName.c_str()
          , theID
          ));
  } // if
} // OsdMenuBase::setActionEnabled()

//-------------------------------------------------------------------------
//     OsdMenuBase::isActionEnabled()
//-------------------------------------------------------------------------
template <typename TT>
bool OsdMenuBase<TT>::isActionEnabled(int theID) const
{
  bool fEnabled = false;

  ID2ActionMap::const_iterator anIter = myActions.find(theID);
  if (anIter != myActions.end())
  {
    fEnabled = anIter->second->isEnabled();
  } // if

  return fEnabled;
} // OsdMenuBase::isActionEnabled(int theID)

//-------------------------------------------------------------------------
//     OsdMenuBase::setActionLocked()
//-------------------------------------------------------------------------
template <typename TT>
void OsdMenuBase<TT>::setActionLocked(int theID, bool fLocked)
{
  ID2ActionMap::iterator anIter = myActions.find(theID);
  if (anIter != myActions.end())
  {
    if (anIter->second->isLocked() != fLocked)
    {
      anIter->second->setLocked(fLocked);
    } // if
  }
  else
  {
    wswarn(("OsdMenuBase<%s>::setActionLocked() action with ID %d not present!"
          , myMenuName.c_str()
          , theID
          ));
  } // if
} // OsdMenuBase::setActionLocked()

//-------------------------------------------------------------------------
//     OsdMenuBase::setActionLockCode()
//-------------------------------------------------------------------------
template <typename TT>
void OsdMenuBase<TT>::setActionLockCode(int theID, const std::string &theLockCode)
{
  ID2ActionMap::iterator anIter = myActions.find(theID);
  if (anIter != myActions.end())
  {
    anIter->second->setLockCode(theLockCode);
  }
  else
  {
    wswarn(("OsdMenuBase<%s>::setActionLocked() action with ID %d not present!"
          , myMenuName.c_str()
          , theID
          ));
  } // if
} // OsdMenuBase::setActionLockCode()

//-------------------------------------------------------------------------
//     OsdMenuBase::setUpdateColorButtons()
//-------------------------------------------------------------------------
template <typename TT>
void OsdMenuBase<TT>::setUpdateColorButtons(bool fSet)
{
  if (myUpdateColorButtons != fSet)
  {
    myUpdateColorButtons = fSet;
    wsdebug(("OsdMenuBase<%s>::setActionLocked() setting updateColorButtons = %d"
           , myMenuName.c_str()
           , fSet
           ));
  } // if
} // OsdMenuBase::setUpdateColorButtons()

//-------------------------------------------------------------------------
//     OsdMenuBase::setKeyAction()
//-------------------------------------------------------------------------
template <typename TT>
Action *OsdMenuBase<TT>::setKeyAction(int theKey, int theID)
{
#if 0
  wsdebug(("> OsdMenuBase<%s>::setKeyAction() Key: %-10s  Action: %3d"
         , myMenuName.c_str()
         , Vdr::Tools::getKeyStr(eKeys(theKey)).c_str()
         , theID
         ));
#endif

  if ( theKey == kUp
    || theKey == kDown
    || theKey == kLeft
    || theKey == kRight
     )
  {
    wswarn(("! OsdMenuBase<%s>::setKeyAction() Key: %-13s  Action: %3d - NO GOOD IDEA TO ASSIGN ACTIONS TO CURSOR-KEYS!"
          , myMenuName.c_str()
          , Vdr::Tools::getKeyStr(eKeys(theKey)).c_str()
          , theID
          ));
  } // if

  Action *aNewAction  = getAction(theID);
  Action *anOldAction = 0;

  //----- get the old action -----
  Key2ActionMap::const_iterator anIter = myKeyActions.find(theKey);
  if (anIter != myKeyActions.end())
  {
    anOldAction = anIter->second;
  } // if

  if (aNewAction != anOldAction)
  {
    wsdebug(("  OsdMenuBase<%s>::setKeyAction() Key: %-13s  Action-OLD: %3d %s"
          , myMenuName.c_str()
          , Ax::Vdr::Tools::getKeyStr(eKeys(theKey)).c_str()
          , theID
          , anOldAction ? anOldAction->getName().c_str() : "(NULL)"
          ));
    wsdebug(("  OsdMenuBase<%s>::setKeyAction() Key: %-13s  Action-NEW: %3d %s"
          , myMenuName.c_str()
          , Ax::Vdr::Tools::getKeyStr(eKeys(theKey)).c_str()
          , theID
          , aNewAction ? aNewAction->getName().c_str() : "(NULL)"
          ));
  } // if

  //----- save the new action -----
  if (aNewAction)
  {
    myKeyActions[theKey] = aNewAction;

    if ( theKey == kRed    || theKey == (kRed    | k_Repeat)
      || theKey == kGreen  || theKey == (kGreen  | k_Repeat)
      || theKey == kYellow || theKey == (kYellow | k_Repeat)
      || theKey == kBlue   || theKey == (kBlue   | k_Repeat)
       )
    {
      setUpdateColorButtons();
    } // if
  }
  else
  {
    myKeyActions.erase(theKey);
  } // if

#if 0
  wsdebug(("< OsdMenuBase<%s>::setKeyAction() Key: %-10s  Action: %3d %s"
         , myMenuName.c_str()
         , Ax::Vdr::Tools::getKeyStr(eKeys(theKey)).c_str()
         , theID
         , ""
         ));
#endif
  return anOldAction;
} // OsdMenuBase::setKeyAction()

//-------------------------------------------------------------------------
//     OsdMenuBase::getKeyAction()
//-------------------------------------------------------------------------
template <typename TT>
Action *OsdMenuBase<TT>::getKeyAction(int theKey) const
{
  Action *anAction = 0;

  Key2ActionMap::const_iterator anIter = myKeyActions.find(theKey);
  if (anIter != myKeyActions.end())
  {
    anAction = anIter->second;
  } // if

  return anAction;
} // OsdMenuBase::getKeyAction()

//-------------------------------------------------------------------------
//     OsdMenuBase::getActionKey()
//-------------------------------------------------------------------------
template <typename TT>
int OsdMenuBase<TT>::getActionKey(Action *theAction) const
{
  int aKey = kNone;

  if (theAction != 0)
  {
    for (Key2ActionMap::const_iterator anIter  = myKeyActions.begin();
                                       anIter != myKeyActions.end();
                                     ++anIter)
    {
      if (anIter->second == theAction)
      {
        aKey = anIter->first;
        break;
      } // if
    } // for
  } // if

  return aKey;
} // OsdMenuBase::getActionKey()

//-------------------------------------------------------------------------
//     OsdMenuBase::addSeparator()
//-------------------------------------------------------------------------
template <typename TT>
void OsdMenuBase<TT>::addSeparator(std::string theText)
{
  std::string aLine = std::string(5, '-')
                    + std::string(1, ' ')
                    + theText
                    + std::string(1, ' ')
                    + std::string(140 - (theText.length() > 40 ? theText.length() : 40), '-')
                    ;

  cOsdItem *anItem = new cOsdItem(aLine.c_str());
  anItem->SetSelectable(false);

  TT::Add(anItem);
} // OsdMenuBase::addSeparator()

//-------------------------------------------------------------------------
//     OsdMenuBase::updateColorButtons()
//-------------------------------------------------------------------------
template <typename TT>
void OsdMenuBase<TT>::updateColorButtons()
{
  if (myUpdateColorButtons)
  {
    wsdebug(("  OsdMenuBase<%s>::updateColorButtons() updating color buttons...", myMenuName.c_str()));

    setUpdateColorButtons(false);

    Action *anActionRed    = getKeyAction(kRed   );
    Action *anActionGreen  = getKeyAction(kGreen );
    Action *anActionYellow = getKeyAction(kYellow);
    Action *anActionBlue   = getKeyAction(kBlue  );

    TT::SetHelp( anActionRed    ? anActionRed   ->getButtonText() : 0
               , anActionGreen  ? anActionGreen ->getButtonText() : 0
               , anActionYellow ? anActionYellow->getButtonText() : 0
               , anActionBlue   ? anActionBlue  ->getButtonText() : 0
               );
  } // if
} // OsdMenuBase<TT>::updateColorButtons()

//-------------------------------------------------------------------------
//     OsdMenuBase::selectButtonPage()
//-------------------------------------------------------------------------
template <typename TT>
void OsdMenuBase<TT>::selectButtonPage(int thePageNum)
{
  myButtonPage = thePageNum;

  ButtonPage2Actions::iterator anIter = myButtonActions.find(thePageNum);

  if (anIter != myButtonActions.end())
  {
    setKeyAction(kRed               , anIter->second->ActionRed      );
    setKeyAction(kRed     | k_Repeat, anIter->second->ActionRedRep   );
    setKeyAction(kGreen             , anIter->second->ActionGreen    );
    setKeyAction(kGreen   | k_Repeat, anIter->second->ActionGreenRep );
    setKeyAction(kYellow            , anIter->second->ActionYellow   );
    setKeyAction(kYellow  | k_Repeat, anIter->second->ActionYellowRep);
    setKeyAction(kBlue              , anIter->second->ActionBlue     );
    setKeyAction(kBlue    | k_Repeat, anIter->second->ActionBlueRep  );
  }
  else
  {
    wswarn(("OsdMenuBase<%s>::selectButtonPage() Page %d not present!"
          , myMenuName.c_str()
          , thePageNum
          ));

    setKeyAction(kRed               , A_NONE);
    setKeyAction(kRed     | k_Repeat, A_NONE);
    setKeyAction(kGreen             , A_NONE);
    setKeyAction(kGreen   | k_Repeat, A_NONE);
    setKeyAction(kYellow            , A_NONE);
    setKeyAction(kYellow  | k_Repeat, A_NONE);
    setKeyAction(kBlue              , A_NONE);
    setKeyAction(kBlue    | k_Repeat, A_NONE);
  } // if
} // OsdMenuBase::selectButtonPage()

//-------------------------------------------------------------------------
//     OsdMenuBase::setButtonTexts()
//-------------------------------------------------------------------------
template <typename TT>
void OsdMenuBase<TT>::setButtonTexts( const std::string &theTextRed
                                , const std::string &theTextGreen
                                , const std::string &theTextYellow
                                , const std::string &theTextBlue
                                )
{
  myTextRed    = theTextRed;
  myTextGreen  = theTextGreen;
  myTextYellow = theTextYellow;
  myTextBlue   = theTextBlue;

  TT::SetHelp( myTextRed    != "" ? myTextRed   .c_str() : 0
             , myTextGreen  != "" ? myTextGreen .c_str() : 0
             , myTextYellow != "" ? myTextYellow.c_str() : 0
             , myTextBlue   != "" ? myTextBlue  .c_str() : 0
             );
} // OsdMenuBase::setButtonTexts


//-------------------------------------------------------------------------
//     OsdMenuBase::setButtonActions()
//-------------------------------------------------------------------------
template <typename TT>
void OsdMenuBase<TT>::setButtonActions( int thePageNum
                                      , int theActionRed
                                      , int theActionGreen
                                      , int theActionYellow
                                      , int theActionBlue
                                      , int theActionRedRep
                                      , int theActionGreenRep
                                      , int theActionYellowRep
                                      , int theActionBlueRep
                                      )
{
  ButtonPage2Actions::iterator anIter = myButtonActions.find(thePageNum);

  if (anIter != myButtonActions.end())
  {
    anIter->second->ActionRed      = theActionRed;
    anIter->second->ActionRedRep   = theActionRedRep;
    anIter->second->ActionGreen    = theActionGreen;
    anIter->second->ActionGreenRep = theActionGreenRep;
    anIter->second->ActionYellow   = theActionYellow;
    anIter->second->ActionYellowRep= theActionYellowRep;
    anIter->second->ActionBlue     = theActionBlue;
    anIter->second->ActionBlueRep  = theActionBlueRep;
  }
  else
  {
    myButtonActions[thePageNum] = new ButtonActions( theActionRed
                                                   , theActionGreen
                                                   , theActionYellow
                                                   , theActionBlue
                                                   , theActionRedRep
                                                   , theActionGreenRep
                                                   , theActionYellowRep
                                                   , theActionBlueRep
                                                   );
  } // if

  if (myButtonPage == -1)
  {
    selectButtonPage(thePageNum);
  } // if
} // OsdMenuBase::setButtonActions()

//-------------------------------------------------------------------------
//     OsdMenuBase::informSubOSDOpened()
//-------------------------------------------------------------------------
template <typename TT>
void OsdMenuBase<TT>::informSubOSDOpened(const Ax::Vdr::Action &theAction, eKeys theKey)
{
  AX_UNUSED_ARG(theAction);
  AX_UNUSED_ARG(theKey);
  wsdebug(("- OsdMenuBase<%s>::informSubOSDOpened() Action: %s  Key: %2d %-20s  !!!!!"
         , myMenuName.c_str()
         , theAction.getName().c_str()
         , theKey
         , Ax::Vdr::Tools::getKeyStr(theKey).c_str()
         ));
} // OsdMenuBase::informSubOSDOpened()

//-------------------------------------------------------------------------
//     OsdMenuBase::informSubOSDClosed()
//-------------------------------------------------------------------------
template <typename TT>
void OsdMenuBase<TT>::informSubOSDClosed(const Ax::Vdr::Action &theAction, eKeys theKey)
{
  AX_UNUSED_ARG(theAction);
  AX_UNUSED_ARG(theKey);
  wsdebug(("- OsdMenuBase<%s>::informSubOSDClosed() Action: %s  Key: %2d %-20s  !!!!!"
         , myMenuName.c_str()
         , theAction.getName().c_str()
         , theKey
         , Ax::Vdr::Tools::getKeyStr(theKey).c_str()
         ));
} // OsdMenuBase::informSubOSDClosed()

//-------------------------------------------------------------------------
//     OsdMenuBase::informCurrentChanged()
//-------------------------------------------------------------------------
template <typename TT>
void OsdMenuBase<TT>::informCurrentChanged(int theOldCurrent)
{
  AX_UNUSED_ARG(theOldCurrent);
  wsdebug(("- OsdMenuBase<%s>::informCurrentChanged() Current: %d  OldCurrent: %d"
         , myMenuName.c_str()
         , TT::Current()
         , theOldCurrent
         ));
} // OsdMenuBase::informCurrentChanged()

//-------------------------------------------------------------------------
//     OsdMenuBase::showStatusMsg()
//-------------------------------------------------------------------------
template <typename TT>
void OsdMenuBase<TT>::showStatusMsg(const std::string &theMsg, int theSecs)
{
  wsdebug(("- OsdMenuBase<%s>::showStatusMsg() Secs: %3d  myMsg: '%s'  theMsg: '%s'"
         , myMenuName.c_str()
         , theSecs
         , myStatusMsg.c_str()
         , theMsg.c_str()
         ));

  if (theMsg.empty())
  {
    clearStatusMsg();
  }
  else
  {
    myStatusMsg = theMsg;

    if      (theSecs == 0) theSecs = MENUTIMEOUT;
    else if (theSecs <  0) theSecs = Setup.OSDMessageTime;

    myStatusTimeout.Set(theSecs * 1000);
    drawStatusMsg();
  } // if
} // OsdMenuBase::showStatusMsg()

//-------------------------------------------------------------------------
//     OsdMenuBase::clearStatusMsg()
//-------------------------------------------------------------------------
template <typename TT>
void OsdMenuBase<TT>::clearStatusMsg()
{
  if (!myStatusMsg.empty())
  {
    wsdebug(("- OsdMenuBase<%s>::clearStatusMsg() myMsg: '%s'"
          , myMenuName.c_str()
          , myStatusMsg.c_str()
          ));

    myStatusMsg = ""; // .clear();
    drawStatusMsg();
  } // if
} // OsdMenuBase::clearStatusMsg()

//-------------------------------------------------------------------------
//     OsdMenuBase::drawStatusMsg()
//-------------------------------------------------------------------------
template <typename TT>
void OsdMenuBase<TT>::drawStatusMsg()
{
  if (myStatusMsgLast != myStatusMsg)
  {
    if (myStatusMsg.empty())
    {
      wsdebug(("OsdMenuBase::drawStatusMsg() -> clearing message"));
      Skins.Message(mtStatus, 0);
    }
    else
    {
      wsdebug(("OsdMenuBase::drawStatusMsg() -> setting message: '%s'", myStatusMsg.c_str()));
      Skins.Message(mtStatus, myStatusMsg.c_str());
    } // if
    myStatusMsgLast = myStatusMsg;
  } // if
} // OsdMenuBase::drawStatusMsg()

//-------------------------------------------------------------------------
//     OsdMenuBase::isButtonAction()
//-------------------------------------------------------------------------
template <typename TT>
bool OsdMenuBase<TT>::isButtonAction(int theActionID) const
{
  Action *anAction = 0;

  return (((anAction = getKeyAction(kRed              )) != 0) && anAction->getID() == theActionID)
      || (((anAction = getKeyAction(kGreen            )) != 0) && anAction->getID() == theActionID)
      || (((anAction = getKeyAction(kYellow           )) != 0) && anAction->getID() == theActionID)
      || (((anAction = getKeyAction(kBlue             )) != 0) && anAction->getID() == theActionID)
      || (((anAction = getKeyAction(kRed    | k_Repeat)) != 0) && anAction->getID() == theActionID)
      || (((anAction = getKeyAction(kGreen  | k_Repeat)) != 0) && anAction->getID() == theActionID)
      || (((anAction = getKeyAction(kYellow | k_Repeat)) != 0) && anAction->getID() == theActionID)
      || (((anAction = getKeyAction(kBlue   | k_Repeat)) != 0) && anAction->getID() == theActionID)
       ;

} // OsdMenuBase::isButtonAction()


//=============================================================================
//     class ActionActivateButtonPage
//=============================================================================

//-----------------------------------------------------------------------------
//     ActionActivateButtonPage::ActionActivateButtonPage()
//-----------------------------------------------------------------------------
template <typename TT>
ActionActivateButtonPage<TT>::ActionActivateButtonPage( TT                *theOsdMenu
                                                      , int                theID
                                                      , const char        *theButtonText
                                                      , int                thePageNum
                                                      , const std::string &theName
                                                      , const std::string &theDesc
                                                      )
                            : PARENT      (theID, theName, theDesc)
                            , myOsdMenu   (theOsdMenu   )
                            , myButtonText(theButtonText)
                            , myPageNum   (thePageNum   )
{
} // ActionActivateButtonPage::ActionActivateButtonPage()

//-----------------------------------------------------------------------------
//     ActionActivateButtonPage::~ActionActivateButtonPage()
//-----------------------------------------------------------------------------
template <typename TT>
ActionActivateButtonPage<TT>::~ActionActivateButtonPage()
{
} // ActionActivateButtonPage::~ActionActivateButtonPage()

//-----------------------------------------------------------------------------
//     ActionActivateButtonPage::~ActionActivateButtonPage()
//-----------------------------------------------------------------------------
template <typename TT>
const char *ActionActivateButtonPage<TT>::getButtonTextInternal() const
{
  return myButtonText;
} // ActionActivateButtonPage::getButtonTextInternal()

//-----------------------------------------------------------------------------
//     ActionActivateButtonPage::runInternal()
//-----------------------------------------------------------------------------
template <typename TT>
eOSState ActionActivateButtonPage<TT>::runInternal(eOSState theStateBefore)
{
  myOsdMenu->selectButtonPage(myPageNum);
  return osContinue;
} // ActionActivateButtonPage::runInternal()

//=============================================================================
//     namespaces
//=============================================================================
} // Vdr
} // Ax

#if OSD_MENU_TRACE_WAS_ON
#include <Ax/Tools/TraceOn.h>
#else
#include <Ax/Tools/TraceOff.h>
#endif
#undef OSD_MENU_TRACE_WAS_ON
