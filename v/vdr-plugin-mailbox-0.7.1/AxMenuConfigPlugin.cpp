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
 *   $Date: 2014-10-26 15:41:24 +0100 (So, 26 Okt 2014) $
 */
//=============================================================================

//=============================================================================
//     includes
//=============================================================================

//----- multiple defs of swap() workaround ------------------------------------
#include "AxPluginGlobals.h"

//----- qt --------------------------------------------------------------------
//----- CORBA -----------------------------------------------------------------
//----- ICE -------------------------------------------------------------------
//----- C++ -------------------------------------------------------------------
#include <algorithm>

//----- C ---------------------------------------------------------------------
//----- vdr -------------------------------------------------------------------
#include <vdr/tools.h>
#include <vdr/i18n.h>
#include <vdr/interface.h>
#include <vdr/menuitems.h>

//----- AxLib -----------------------------------------------------------------
#include <Ax/Tools/Trace.h>
// #include <Ax/Tools/TraceOn.h>
#include <Ax/Tools/String.h>
#include <Ax/Mail/MailBox.h>

//----- local plugin ----------------------------------------------------------
#include "AxPluginSetup.h"
#include "AxMenuConfigMailBox.h"
#include "AxMenuConfigCommon.h"
#include "AxMenuConfigPlugin.h"
#include "AxPluginMailBox.h"

//----- locals ----------------------------------------------------------------

//=============================================================================
//     using namespaces
//=============================================================================
using namespace std;
using namespace Ax;

//=============================================================================
//     class AxMailBoxSetupItem
//=============================================================================
class AxMailBoxSetupItem : public cOsdItem
{
  typedef cOsdItem PARENT;

  public:
    //-------------------------------------------------------------------------
    //     AxMailBoxSetupItem()
    //-------------------------------------------------------------------------
    AxMailBoxSetupItem(Ax::Mail::MailBoxSetup *aMBSetup);

    //-------------------------------------------------------------------------
    //     ~AxMailBoxSetupItem()
    //-------------------------------------------------------------------------
    virtual ~AxMailBoxSetupItem();

    //-------------------------------------------------------------------------
    //     getMBSetup()
    //-------------------------------------------------------------------------
    Ax::Mail::MailBoxSetup *getMBSetup();

    //-------------------------------------------------------------------------
    //     Set()
    //-------------------------------------------------------------------------
    virtual void Set(void);

  private:
    //-------------------------------------------------------------------------
    //     attributes
    //-------------------------------------------------------------------------
    Ax::Mail::MailBoxSetup *myMBSetup;
}; // class AxMailBoxSetupItem

//-----------------------------------------------------------------------------
//     AxMailBoxSetupItem::AxMailBoxSetupItem()
//-----------------------------------------------------------------------------
AxMailBoxSetupItem::AxMailBoxSetupItem(Ax::Mail::MailBoxSetup *aMBSetup)
                  : myMBSetup(aMBSetup)
{
} // AxMailBoxSetupItem::AxMailBoxSetupItem()

//-----------------------------------------------------------------------------
//     AxMailBoxSetupItem::~AxMailBoxSetupItem()
//-----------------------------------------------------------------------------
AxMailBoxSetupItem::~AxMailBoxSetupItem()
{
} // AxMailBoxSetupItem::~AxMailBoxSetupItem()

//-----------------------------------------------------------------------------
//     AxMailBoxSetupItem::getMBSetup()
//-----------------------------------------------------------------------------
Ax::Mail::MailBoxSetup *AxMailBoxSetupItem::getMBSetup()
{
  return myMBSetup;
} // AxMailBoxSetupItem::getMBSetup()

//-----------------------------------------------------------------------------
//     AxMBSetupItem::Set()
//-----------------------------------------------------------------------------
void AxMailBoxSetupItem::Set()
{
  SetText(myMBSetup->getAccountName().c_str());
} // AxMailBoxSetupItem::Set()

//=============================================================================
//     class AxMenuConfigPlugin
//=============================================================================

//-----------------------------------------------------------------------------
//     AxMenuConfigPlugin::AxMenuConfigPlugin()
//-----------------------------------------------------------------------------
AxMenuConfigPlugin::AxMenuConfigPlugin(AxPluginMailBox *thePlugin)
                  : PARENT  (tr("Plugin settings"))
                  , myPlugin(thePlugin)
{
#if VDRVERSNUM >= 10734
  SetMenuCategory(mcPluginSetup);
#endif 
  
  //----- create actions -------------------------------------------------
  addAction(new Vdr::ActionCallback<PARENT>( this
                                           , A_COMMON_EDIT
                                           , tr("ConfigPlugin.Common$Edit"  )
                                           , "CommonEdit"
                                           , tr("Edit general settings")
                                           ));

  addAction(new Vdr::ActionCallback<PARENT>( this
                                           , A_MAILBOX_NEW
                                           , tr("ConfigPlugin.MailAccount$New")
                                           , "MailBoxNew"
                                           , tr("Create a new mail account")
                                           ));

  addAction(new Vdr::ActionCallback<PARENT>( this
                                           , A_MAILBOX_DUPLICATE
                                           , tr("ConfigPlugin.MailAccount$Duplicate")
                                           , "MailBoxDuplicate"
                                           , tr("Duplicate selected mail account")
                                           ));

  addAction(new Vdr::ActionCallback<PARENT>( this
                                           , A_MAILBOX_EDIT
                                           , tr("ConfigPlugin.MailAccount$Edit")
                                           , "MailBoxEdit"
                                           , tr("Edit selected mail account")
                                           ));


  addAction(new Vdr::ActionCallback<PARENT>( this
                                           , A_MAILBOX_DELETE
                                           , tr("ConfigPlugin.MailAccount$Delete")
                                           , "MailBoxDelete"
                                           , tr("Delete selected mail account")
                                           ));

  addAction(new Vdr::ActionCallback<PARENT>( this
                                           , A_MAILBOX_MARK
                                           , tr("ConfigPlugin.MailAccount$Mark")
                                           , "MailBoxMark"
                                           , tr("Move the selected account to another position")
                                           ));

  //----- setup items ----------------------------------------------------
  setup();

  //----- assign actions to keys -----------------------------------------
  updateActions();

} // AxMenuConfigPlugin::AxMenuConfigPlugin()

//-----------------------------------------------------------------------------
//     AxMenuConfigPlugin::~AxMenuConfigPlugin()
//-----------------------------------------------------------------------------
AxMenuConfigPlugin::~AxMenuConfigPlugin()
{
  myPlugin->SetupMenuActionFinished();
} // AxMenuConfigPlugin::~AxMenuConfigPlugin()

//-------------------------------------------------------------------------
//     AxMenuConfigPlugin::setup()
//-------------------------------------------------------------------------
void AxMenuConfigPlugin::setup()
{
  int                     aCurrentIdx   = 0;
  Ax::Mail::MailBoxSetup *aCurrentSetup = 0;

  if (Count() > 0 && getCurrentMailBoxItem() != 0)
  {
    aCurrentSetup = getCurrentMailBoxItem()->getMBSetup();
    aCurrentIdx   = -1;
  } // if

  //----- remove the old items -------------------------------------------
  Clear();

  //----- Common settings ------------------------------------------------
  Add(new cOsdItem(tr("General settings")), aCurrentIdx == 0);

  //----- mail accounts --------------------------------------------------
  addSeparator(tr("ConfigPlugin$Mail accounts"));

  for (Mail::MailBoxSetupCltn::iterator anIter  = myPlugin->getSetup().Accounts.begin();
                                        anIter != myPlugin->getSetup().Accounts.end();
                                      ++anIter)
  {
    AxMailBoxSetupItem *anItem = new AxMailBoxSetupItem(*anIter);
    Add(anItem, *anIter == aCurrentSetup);
    anItem->Set();
  } // for
} // AxMenuConfigPlugin::setup()

//-------------------------------------------------------------------------
//     AxMenuConfigPlugin::getCurrentMailBoxItem()
//-------------------------------------------------------------------------
AxMailBoxSetupItem *AxMenuConfigPlugin::getCurrentMailBoxItem()
{
  return dynamic_cast<AxMailBoxSetupItem *>(getCurrentItem());
} // AxMenuConfigPlugin::getCurrentMailBoxItem()

//-------------------------------------------------------------------------
//     AxMenuConfigPlugin::processAction()
//-------------------------------------------------------------------------
eOSState AxMenuConfigPlugin::processAction(const Ax::Vdr::Action &theAction, eOSState theState)
{
  switch (theAction.getID())
  {
    //---------------------------------------------------------------------
    case A_COMMON_EDIT:
    {
      theState = AddSubMenu(new AxMenuConfigCommon(myPlugin));
      break;
    }
    //---------------------------------------------------------------------
    case A_MAILBOX_NEW:
    {
      Mail::MailBoxSetup *aMBSetup = new Mail::MailBoxSetup(tr("ConfigPlugin$New account"));

      myPlugin->getSetup().Accounts.push_back(aMBSetup);

      saveAccounts();

      AxMailBoxSetupItem *aNewItem = new AxMailBoxSetupItem(aMBSetup);
      Add(aNewItem);
      SetCurrent(aNewItem);

      cOsdMenu *anOsdMenu = new AxMenuConfigMailBox(myPlugin, *aMBSetup);

      theState = AddSubMenu(anOsdMenu);
      break;
    }
    //---------------------------------------------------------------------
    case A_MAILBOX_EDIT:
    {
      AxMailBoxSetupItem *aMBItem = getCurrentMailBoxItem();
      if (aMBItem)
      {
        theState = AddSubMenu(new AxMenuConfigMailBox(myPlugin, *aMBItem->getMBSetup()));
      } // if
      break;
    }
    //---------------------------------------------------------------------
    case A_MAILBOX_DELETE:
    {
      AxMailBoxSetupItem *aMBItem = getCurrentMailBoxItem();

      if (aMBItem)
      {
        std::string aMsg = Ax::Tools::String::sprintf( tr("Do you really want to delete account %s?")
                                                     , aMBItem->getMBSetup()->getAccountName().c_str()
                                                     );

        if (Interface->Confirm(aMsg.c_str()))
        {
          int anIndex = Current() - 2; // Offset is 2 because of Common-Settings and separator

          std::rotate( myPlugin->getSetup().Accounts.begin() + anIndex     // begin
                     , myPlugin->getSetup().Accounts.begin() + anIndex + 1 // new begin
                     , myPlugin->getSetup().Accounts.end()                 // end
                     );
          myPlugin->getSetup().Accounts.resize(myPlugin->getSetup().Accounts.size() - 1);
          delete aMBItem->getMBSetup();
          Del(Current());

          saveAccounts();

          updateActions();
          Display();
        } // if
      } // if

      theState = osContinue;
      break;
    }
    //---------------------------------------------------------------------
    case A_MAILBOX_DUPLICATE:
    {
      AxMailBoxSetupItem *aMBItem = getCurrentMailBoxItem();

      if (aMBItem)
      {
        Mail::MailBoxSetup *aMBSetup = new Mail::MailBoxSetup(*aMBItem->getMBSetup());

        myPlugin->getSetup().Accounts.push_back(aMBSetup);

        saveAccounts();

        AxMailBoxSetupItem *aNewItem = new AxMailBoxSetupItem(aMBSetup);
        Add(aNewItem);
        SetCurrent(aNewItem);

        cOsdMenu *anOsdMenu = new AxMenuConfigMailBox(myPlugin, *aMBSetup);

        theState = AddSubMenu(anOsdMenu);
      } // if
      break;
    }
    //---------------------------------------------------------------------
    case A_MAILBOX_MARK:
    {
      if (getCurrentMailBoxItem())
      {
        Mark();
      } // if
      break;
    }
    //---------------------------------------------------------------------
    default:
    {
      theState = PARENT::processAction(theAction, theState);
      break;
    }
  } // switch

  return theState;
} // AxMenuConfigPlugin::processAction()

//-------------------------------------------------------------------------
//     AxMenuConfigPlugin::informSubOSDClosed()
//-------------------------------------------------------------------------
void AxMenuConfigPlugin::informSubOSDClosed(const Ax::Vdr::Action &theAction, eKeys theKey)
{
  updateActions();
} // AxMenuConfigPlugin::informSubOSDClosed()

//-------------------------------------------------------------------------
//     AxMenuConfigPlugin::informCurrentChanged()
//-------------------------------------------------------------------------
void AxMenuConfigPlugin::informCurrentChanged(int)
{
  updateActions();
} // AxMenuConfigPlugin::informCurrentChanged()

//-------------------------------------------------------------------------
//     AxMenuConfigPlugin::updateActions()
//-------------------------------------------------------------------------
void AxMenuConfigPlugin::updateActions()
{
  AxMailBoxSetupItem *aMBItem   = getCurrentMailBoxItem();
  std::string         aLockCode = "";

  if ( aMBItem
    && ( aMBItem->getMBSetup()->getAccessMode() == Mail::AM_SETUP_READ
      || aMBItem->getMBSetup()->getAccessMode() == Mail::AM_SETUP
       )
    && !aMBItem->getMBSetup()->getAccessCode().empty()
     )
  {
    aLockCode = aMBItem->getMBSetup()->getAccessCode();
  } // if

  setActionLockCode(A_MAILBOX_EDIT  , aLockCode);
  setActionLockCode(A_MAILBOX_DELETE, aLockCode);

  setActionEnabled(A_COMMON_EDIT    , Current() == 0);
  setActionEnabled(A_MAILBOX_NEW    , true          );
  setActionEnabled(A_MAILBOX_EDIT   , aMBItem   != 0);
  setActionEnabled(A_MAILBOX_DELETE , aMBItem       );
  setActionEnabled(A_MAILBOX_MARK   , aMBItem   != 0 && myPlugin->getSetup().Accounts.size() >= 2);

  if (Current() == 0)
  {
    setKeyAction(kOk    , A_COMMON_EDIT      );

    setKeyAction(kRed   , A_NONE             );
    setKeyAction(kGreen , A_MAILBOX_NEW      );
    setKeyAction(kYellow, A_NONE             );
    setKeyAction(kBlue  , A_NONE             );
  }
  else if (aMBItem   != 0)
  {
    setKeyAction(kOk    , A_MAILBOX_EDIT     );

    setKeyAction(kRed   , A_MAILBOX_DUPLICATE);
    setKeyAction(kGreen , A_MAILBOX_NEW      );
    setKeyAction(kYellow, A_MAILBOX_DELETE   );
    setKeyAction(kBlue  , A_MAILBOX_MARK     );
  }
  else
  {
    setKeyAction(kOk    , A_NONE             );

    setKeyAction(kRed   , A_NONE             );
    setKeyAction(kGreen , A_NONE             );
    setKeyAction(kYellow, A_NONE             );
    setKeyAction(kBlue  , A_NONE             );
  } // if

} // AxMenuConfigPlugin::updateActions()

//-------------------------------------------------------------------------
//     AxMenuConfigPlugin::Move()
//-------------------------------------------------------------------------
void AxMenuConfigPlugin::Move(int From, int To)
{
  From -= 2; // Offset is 2 because of Common-Settings and separator
  To   -= 2; // Offset is 2 because of Common-Settings and separator

  // moved before separator? -> move at the beginning
  if (To < 0) To = 0;

  // nothing to do
  if (From == To)
  {
    return;
  } // if

  // invalid values (this should not happen)
  if (From < 0 || int(myPlugin->getSetup().Accounts.size()) <= From)
  {
    return;
  } // if

  // invalid values (this should not happen)
  if (To < 0 || int(myPlugin->getSetup().Accounts.size()) <= To)
  {
    return;
  } // if

  if (From < To)
  {
    std::rotate( myPlugin->getSetup().Accounts.begin() + From     // begin
               , myPlugin->getSetup().Accounts.begin() + From + 1 // new begin
               , myPlugin->getSetup().Accounts.begin() + To   + 1 // end
               );
  }
  else
  {
    std::rotate( myPlugin->getSetup().Accounts.begin() + To       // begin
               , myPlugin->getSetup().Accounts.begin() + From     // new begin
               , myPlugin->getSetup().Accounts.begin() + From + 1 // end
               );
  } // if

  saveAccounts();
  setup();
  SetCurrent(Get(To+2));
  Display();
} // AxMenuConfigPlugin::Move()

//-------------------------------------------------------------------------
//     AxMenuConfigPlugin::saveAccounts()
//-------------------------------------------------------------------------
void AxMenuConfigPlugin::saveAccounts(void)
{
  if (!myPlugin->getSetup().saveAccounts(cPlugin::ConfigDirectory(myPlugin->Name())))
  {
    Skins.Message(mtError, tr("Unable to write to account definition file!"));
  } // if
} // AxMenuConfigPlugin::saveAccounts()

//-------------------------------------------------------------------------
//     AxMenuConfigPlugin::Store()
//-------------------------------------------------------------------------
void AxMenuConfigPlugin::Store(void)
{
  // NOOP
} // AxMenuConfigPlugin::Store()
