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
 *   $Date: 2014-10-26 15:39:47 +0100 (So, 26 Okt 2014) $
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

//----- C ---------------------------------------------------------------------
//----- Vdr -------------------------------------------------------------------
#include <vdr/i18n.h>
#include <vdr/menuitems.h>

//----- AxLib -----------------------------------------------------------------
#include <Ax/Tools/Trace.h>
// #include <Ax/Tools/TraceOn.h>

//----- local plugin ----------------------------------------------------------
#include <Ax/Vdr/OsdMenu.h>
#include <Ax/Vdr/MenuCode.h>
#include <Ax/Vdr/Tools.h>
#include <Ax/Vdr/Action.h>

//=============================================================================
//     namespaces
//=============================================================================
namespace Ax  {
namespace Vdr {

//=============================================================================
//     class MenuCode
//=============================================================================

//-----------------------------------------------------------------------------
//     MenuCode::MenuCode()
//-----------------------------------------------------------------------------
MenuCode::MenuCode(Ax::Vdr::Action *theAction)
        : PARENT( tr("MenuCode$Restricted area")
                , tr("MenuCode$Restricted area")
                , strlen(tr("MenuCode$Access code")) + 1
                )
        , myAction  (theAction)
        , myActCode (0)
        , myCodeItem(0)
{
  wsdebug(("> MenuCode::MenuCode() Code: '%s'", myAction->getLockCode().c_str()));
#if VDRVERSNUM >= 10734
  SetMenuCategory(mcPlugin);
#endif 

  //----- init attributes -------------------------------------------------
  myActCode = (char *)calloc(myAction->getLockCode().length() + 1, sizeof(char));

  //----- create some empty lines------------------------------------------
  for (int i = 1; i <= std::max(DisplayMenu()->MaxItems() / 2 - 1, 0); ++i)
  {
    cOsdItem *anItem = new cOsdItem("");
    anItem->SetSelectable(false);
    Add(anItem);
  } // for

  //----- create the access-code item -------------------------------------
  myCodeItem = new cMenuEditNumItem(tr("MenuCode$Access code"), myActCode, myAction->getLockCode().length(), true);
  Add(myCodeItem);
  SetCurrent(myCodeItem);

  //----- ensure no actions are on the numeric keys -----------------------
  setKeyAction(kOk  , A_NONE);
  setKeyAction(kBack, A_NONE);
  for (int aKey = k0; aKey <= k9; ++aKey)
  {
    setKeyAction(aKey, A_NONE);
   } // if

  wsdebug(("< MenuCode::MenuCode() Count: %d", Count()));
} // MenuCode::MenuCode()

//-----------------------------------------------------------------------------
//     MenuCode::~MenuCode()
//-----------------------------------------------------------------------------
MenuCode::~MenuCode()
{
  wsdebug(("> MenuCode::MenuCode()"));
  free(myActCode);
  wsdebug(("< MenuCode::~MenuCode()"));
} // MenuCode::~MenuCode()

#define DEBUG_PROCESS_KEY 0

//-------------------------------------------------------------------------
//     MenuCode::ProcessKey()
//-------------------------------------------------------------------------
eOSState MenuCode::ProcessKey(eKeys theKey)
{
#if DEBUG_PROCESS_KEY
  if (theKey != kNone)
  {
    wsdebug(("> MenuCode::ProcessKey()      Key: %-10s  Cur: %3d  HasSub: %d  parent processing..."
           , Ax::Vdr::Tools::getKeyStr(theKey).c_str()
           , Current()
           , HasSubMenu()
           ));
  } // if
#endif

  eOSState aState = PARENT::ProcessKey(theKey);

#if DEBUG_PROCESS_KEY
  if (theKey != kNone)
  {
    wsdebug(("  MenuCode::ProcessKey()      Key: %-10s  Cur: %3d  HasSub: %d  State: %-10s  "
           , Ax::Vdr::Tools::getKeyStr(theKey).c_str()
           , Current()
           , HasSubMenu()
           , Ax::Vdr::Tools::getOSStateStr(aState)
           ));
  } // if
#endif

  //----- select action... ----------------------------------------------------
  switch (theKey)
  {
    case kBack:
    {
      aState = osBack;
      break;
    }

    //----- check the code and quit if ok or display error msg ----
    case kOk:
    {
      bool fCodeOk = strcmp(myActCode, myAction->getLockCode().c_str()) == 0;

      wsdebug(("  MenuCode::ProcessKey() CodeOK: %d  ReqCode: '%s'  ActCode: '%s'"
             , fCodeOk, myAction->getLockCode().c_str(), myActCode
             ));

      if (fCodeOk)
      {
        myAction->setLocked(false);
        aState = osBack;
      }
      else
      {
        memset(myActCode, 0, (myAction->getLockCode().length() + 1) * sizeof(char));
        showStatusMsg(tr("MenuCode$Code invalid!"));
        aState = osContinue;
      } // if
      break;
    }

    //----- check the code immediately and quit if ok -----
    case k0:
    case k1:
    case k2:
    case k3:
    case k4:
    case k5:
    case k6:
    case k7:
    case k8:
    case k9:
    {
      clearStatusMsg();
      // immediately close MenuCode if code is valid (without pressing the kOk)
      bool fCodeOk = strcmp(myActCode, myAction->getLockCode().c_str()) == 0;
      wsdebug(("  MenuCode::ProcessKey() CodeOK: %d  ReqCode: '%s'  ActCode: '%s'"
             , fCodeOk, myAction->getLockCode().c_str(), myActCode
             ));

      if (fCodeOk)
      {
        myAction->setLocked(false);
        aState = osBack;
      }
      myCodeItem->Set();
      Display();
      break;
    }

    default:
    {
#if 0
      aState = osContinue;
#endif
      break;
    }
  } // switch

#if DEBUG_PROCESS_KEY
  if (theKey != kNone)
  {
    wsdebug(("< MenuCode::ProcessKey()      Key: %-10s  Cur: %3d  HasSub: %d  State: %-10s  "
           , Ax::Vdr::Tools::getKeyStr(theKey).c_str()
           , Current()
           , HasSubMenu()
           , Ax::Vdr::Tools::getOSStateStr(aState)
           ));
  } // if
#endif
  return aState;
} // MenuCode::ProcessKey()

//=============================================================================
//     namespaces
//=============================================================================
} // Vdr
} // Ax
