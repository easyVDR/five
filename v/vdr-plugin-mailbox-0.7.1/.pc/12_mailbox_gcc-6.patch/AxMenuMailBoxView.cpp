//=============================================================================
/*
 * See the file README in the main directory for a description of
 * this software, copyright information, and how to reach the author.
 *
 * Author: alex
 * Date:   17.11.2003
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
//----- C ---------------------------------------------------------------------
//----- vdr -------------------------------------------------------------------
#include <vdr/tools.h>
#include <vdr/i18n.h>
#include <vdr/menuitems.h>

//----- AxLib -----------------------------------------------------------------
#include <Ax/Tools/Trace.h>
// #include <Ax/Tools/TraceOn.h>
#include <Ax/Tools/String.h>
#include <Ax/Mail/MailBox.h>
#include <Ax/Mail/MailFolder.h>
#include <Ax/Mail/MailBoxSetup.h>
#include <Ax/Vdr/Tools.h>

//----- local plugin ----------------------------------------------------------
#include "AxVdrTools.h"
#include "AxMenuMailBoxView.h"
#include "AxMenuMailListView.h"
#include "AxMenuLogView.h"
#include "AxPluginSetup.h"
#include "AxPluginMailBox.h"

//----- locals ----------------------------------------------------------------

//=============================================================================
//     using namespaces
//=============================================================================
using namespace std;
using namespace Ax;

//=============================================================================
//     class AxActionOpenMailBox
//=============================================================================
class AxActionOpenMailBox : public Ax::Vdr::Action
{
    //-------------------------------------------------------------------------
    typedef Ax::Vdr::Action PARENT;

  public:
    //------------------------------------------------------------------------
    AxActionOpenMailBox( AxMenuMailBoxView *theMailBoxView
                       , long               theID
                       , const std::string &theName
                       , const std::string &theDesc
                       );

    //------------------------------------------------------------------------
    virtual ~AxActionOpenMailBox();

  protected:
    //-------------------------------------------------------------------------
    virtual bool precheckInternal(eOSState theStateBefore);

    //------------------------------------------------------------------------
    virtual eOSState runInternal(eOSState theStateBefore);

    //------------------------------------------------------------------------
    virtual const char *getButtonTextInternal() const;

  private:
    //------------------------------------------------------------------------
    AxMenuMailBoxView *myMailBoxView;

}; // class AxOpenMailBox

//------------------------------------------------------------------------
//     AxActionOpenMailBox::AxActionOpenMailBox()
//------------------------------------------------------------------------
AxActionOpenMailBox::AxActionOpenMailBox( AxMenuMailBoxView *theMailBoxView
                                        , long               theID
                                        , const std::string &theName
                                        , const std::string &theDesc
                                        )
                   : PARENT(theID, theName, theDesc)
                   , myMailBoxView(theMailBoxView)
{
} // AxActionOpenMailBox()

//------------------------------------------------------------------------
//     AxActionOpenMailBox::~AxActionOpenMailBox()
//------------------------------------------------------------------------
AxActionOpenMailBox::~AxActionOpenMailBox()
{
} // ~AxActionOpenMailBox()

//-------------------------------------------------------------------------
//     AxActionOpenMailBox::precheckInternal()
//------------------------------------------------------------------------
bool AxActionOpenMailBox::precheckInternal(eOSState theStateBefore)
{
  bool fExec = false;

  Mail::MailBox *aMB = myMailBoxView->getCurrentMailBox();

  if (aMB)
  {
    if (!aMB->getCurrentFolder()->hasStatus())
    {
      myMailBoxView->showStatusMsg(aMB->getLastImportantMsg(), 0);
    }
    else if (aMB->getCurrentFolder()->getCountMails() == 0)
    {
      myMailBoxView->showStatusMsg(tr("Mailbox is empty"), 0);
    }
    else
    {
      fExec = true;
    } // if
  } // if

  return fExec;
} // precheckInternal()

//-------------------------------------------------------------------------
//     AxActionOpenMailBox::runInternal()
//------------------------------------------------------------------------
eOSState AxActionOpenMailBox::runInternal(eOSState theStateBefore)
{
  return myMailBoxView->doOpenCurrentMailBox();
} // runInternal()

//-------------------------------------------------------------------------
//     AxActionOpenMailBox::getButtonTextInternal()
//------------------------------------------------------------------------
const char *AxActionOpenMailBox::getButtonTextInternal() const
{
  return tr("Open");
} // getButtonTextInternal()

//=============================================================================
//     class AxMailBoxItem
//=============================================================================
class AxMailBoxItem : public cOsdItem
{
  typedef cOsdItem PARENT;

  public:
    //-------------------------------------------------------------------------
    //     AxMailBoxItem()
    //-------------------------------------------------------------------------
    AxMailBoxItem(Mail::MailBox *theMailBox, eOSState State = osUnknown);

    //-------------------------------------------------------------------------
    //     ~AxMailBoxItem()
    //-------------------------------------------------------------------------
    virtual ~AxMailBoxItem();

    //-------------------------------------------------------------------------
    //     Set()
    //-------------------------------------------------------------------------
    virtual void Set(void);

    //-------------------------------------------------------------------------
    //     getMailBox()
    //-------------------------------------------------------------------------
    Mail::MailBox *getMailBox();

  private:
    //-------------------------------------------------------------------------
    //     attributes
    //-------------------------------------------------------------------------
    Mail::MailBox *myMailBox;

}; // class AxMailBoxItem

//-----------------------------------------------------------------------------
//     AxMailBoxItem::AxMailBoxItem()
//-----------------------------------------------------------------------------
AxMailBoxItem::AxMailBoxItem(Mail::MailBox *theMailBox, eOSState State)
             : PARENT(State)
             , myMailBox(theMailBox)
{
} // AxMailBoxItem::AxMailBoxItem()

//-----------------------------------------------------------------------------
//     AxMailBoxItem::~AxMailBoxItem()
//-----------------------------------------------------------------------------
AxMailBoxItem::~AxMailBoxItem()
{
} // AxMailBoxItem::~AxMailBoxItem()

//-----------------------------------------------------------------------------
//     AxMailBoxItem::getMailBox()
//-----------------------------------------------------------------------------
Mail::MailBox *AxMailBoxItem::getMailBox()
{
  return myMailBox;
} // AxMailBoxItem::getMailBox()

//-----------------------------------------------------------------------------
//     AxMailBoxItem::Set()
//-----------------------------------------------------------------------------
void AxMailBoxItem::Set(void)
{
  std::string aLine("");

  if (getMailBox()->getCurrentFolder()->hasStatus())
  {
    aLine = Ax::Tools::String::sprintf( "%ld\t%ld\t%s"
                                      , getMailBox()->getCurrentFolder()->getCountUnseen()
                                      , getMailBox()->getCurrentFolder()->getCountMails()
                                      , getMailBox()->getAccountName().c_str()
                                      );
  }
  else
  {
    aLine = Ax::Tools::String::sprintf( "%s\t\t%s"
                                      , "???"
                                      , getMailBox()->getAccountName().c_str()
                                      );
  } // if

  SetText(aLine.c_str());
} // AxMailBoxItem::Set()

//=============================================================================
//     class AxMenuMailBoxView
//=============================================================================

//-----------------------------------------------------------------------------
//     AxMenuMailBoxView::AxMenuMailBoxView()
//-----------------------------------------------------------------------------
AxMenuMailBoxView::AxMenuMailBoxView(AxPluginMailBox *thePlugin)
                 : PARENT( tr("Mailbox-View")
                         , tr("Mail accounts")
                         , 6
                         , 7
                         )
                 , myPlugin(thePlugin)
{
#if VDRVERSNUM >= 10734
  SetMenuCategory(mcPlugin);
#endif 
  
  //----- create title line ---------------------------------------------------
  if (myPlugin->getSetup().Accounts.size() > 0)
  {
    //----- create actions ------------------------------------------------------
    addAction(new AxActionOpenMailBox        ( this
                                             , A_OPEN_MAILBOX
                                             , "OpenMailBox"
                                             , tr("Open the selected mailbox")
                                             ));

    addAction(new Vdr::ActionCallback<PARENT>( this
                                             , A_REFRESH_CURRENT
                                             , tr("Refresh current")
                                             , "RefreshCurrent"
                                             , tr("Check the selected mailbox again")
                                             ));

    addAction(new Vdr::ActionCallback<PARENT>( this
                                             , A_REFRESH_ALL
                                             , tr("Refresh all")
                                             , "RefreshAll"
                                             , tr("Check all mailboxes again")
                                             ));

  addAction(new Vdr::ActionCallback<PARENT>( this
                                           , A_SHOW_LOG
                                           , tr("Log")
                                           , "ShowLog"
                                           , tr("Show the communication log")
                                           ));

    //----- create header line ------------------------------------------------
    std::string aLine = Ax::Tools::String::sprintf( "%s ---\t%s ---\t%s------------------------------------------------------------------------"
                                                  , tr("MailBoxView$New")
                                                  , tr("MailBoxView$Total")
                                                  , tr("Mail account")
                                                  );
    cOsdItem *anItem = new cOsdItem(aLine.c_str());
    anItem->SetSelectable(false);
    Add(anItem);

    setActionEnabled(A_REFRESH_CURRENT, false);
    setActionEnabled(A_REFRESH_ALL    , false);
    setActionEnabled(A_OPEN_MAILBOX   , false);
    setActionEnabled(A_SHOW_LOG       , false);

    //----- assign actions to keys ----------------------------------------------
    setKeyAction(kOk               , A_OPEN_MAILBOX  );

    setButtonActions( 0
                   // red              , green           , yellow           , blue
                    , A_SHOW_LOG       , A_OPEN_MAILBOX  , A_REFRESH_CURRENT, A_REFRESH_ALL
                    );

    //----- create mailbox list items -----------------------------------------
    for (Mail::MailBoxList::const_iterator anIter  = myPlugin->getMailBoxCltn().begin();
                                           anIter != myPlugin->getMailBoxCltn().end();
                                         ++anIter)
    {
      Mail::MailBox *aMB = *anIter;

      //---- create menu-item and add to osd-menu -----
      AxMailBoxItem *anItem = new AxMailBoxItem(aMB);
      anItem->Set();
      Add(anItem);
    } // for

    // call Display to show the menu immediately
    Display();

    SetCurrent(Get(1));
    checkMailBoxes();
  }
  else
  {
    showStatusMsg(tr("No mail accounts defined!"), 0);
  } // if

} // AxMenuMailBoxView::AxMenuMailBoxView()

//-----------------------------------------------------------------------------
//     AxMenuMailBoxView::~AxMenuMailBoxView()
//-----------------------------------------------------------------------------
AxMenuMailBoxView::~AxMenuMailBoxView()
{
  //----- inform the plugin about the "end of osd"  -----
  myPlugin->MainMenuActionFinished();

} // AxMenuMailBoxView::~AxMenuMailBoxView()

//-------------------------------------------------------------------------
//     AxMenuMailBoxView::processAction()
//-------------------------------------------------------------------------
eOSState AxMenuMailBoxView::processAction(const Ax::Vdr::Action &theAction, eOSState theState)
{
  eOSState aState = osContinue;
  switch (theAction.getID())
  {
    case A_SHOW_LOG:
    {
      Mail::MailBox *aMB = getCurrentMailBox();
      if (aMB)
      {
        aState = AddSubMenu(new AxMenuLogView(aMB->getLogHandler()));
      } // if
      break;
    }
    case A_REFRESH_CURRENT:
    {
      if (Current() > 0)
      {
        checkMailBox(Current());
        updateActions();
      } // if
      break;
    }
    case A_REFRESH_ALL:
    {
      checkMailBoxes();
      break;
    }
    default: //----- give parents a chance --------------------------------
    {
      aState = PARENT::processAction(theAction, theState);
      break;
    }
  } // switch
  return aState;
} // AxMenuMailBoxView::processAction()

//-------------------------------------------------------------------------
//     AxMenuMailBoxView::getCurrentMailBox()
//-------------------------------------------------------------------------
Ax::Mail::MailBox *AxMenuMailBoxView::getCurrentMailBox() const
{
  Mail::MailBox *aMB = 0;

  if (const_cast<AxMenuMailBoxView *>(this)->Current() > 0)
  {
    AxMailBoxItem *aMailBoxItem = dynamic_cast<AxMailBoxItem *>(Get(const_cast<AxMenuMailBoxView *>(this)->Current()));

    if (aMailBoxItem)
    {
      aMB = aMailBoxItem->getMailBox();
    } // if
  } // if

  return aMB;
} // AxMenuMailBoxView::getCurrentMailBox()

//-------------------------------------------------------------------------
//     AxMenuMailBoxView::updateActions()
//-------------------------------------------------------------------------
void AxMenuMailBoxView::updateActions()
{
  Mail::MailBox *aMB = getCurrentMailBox();

  //----- update the enabled-state of the actions -----
  std::string aLockCode = "";
  if ( aMB
    && aMB->getSetup().getAccessMode() == Mail::AM_SETUP_READ
    && aMB->getSetup().getAccessCode().length() > 0
     )
  {
    aLockCode = aMB->getSetup().getAccessCode();
  } // if

  setActionLockCode(A_OPEN_MAILBOX, aLockCode);

  setActionEnabled(A_REFRESH_CURRENT, Current() > 0);
  setActionEnabled(A_REFRESH_ALL    , myPlugin->getMailBoxCltn().size() > 0);
  setActionEnabled(A_OPEN_MAILBOX   , aMB && aMB->getCurrentFolder()->getCountMails() > 0);
  setActionEnabled(A_SHOW_LOG       , aMB && aMB->getSetup().getClientOptDbg() == Mail::CC_DBG_YES
                                          && !aMB->getLogHandler().isEmpty());

  //----- display a status-message -----
  if (aMB && !aMB->getCurrentFolder()->hasStatus())
  {
    std::string aMsg = aMB->getLastImportantMsg();
    wsinfo(("%s: %s", aMB->getAccountName().c_str(), aMsg.c_str()));
    showStatusMsg(aMsg, 0);
  }
  else
  {
    clearStatusMsg();
  } // if

} // AxMenuMailBoxView::updateActions()

//-------------------------------------------------------------------------
//     AxMenuMailBoxView::checkMailBoxes()
//-------------------------------------------------------------------------
void AxMenuMailBoxView::checkMailBoxes()
{
//  DisplayCurrent(false);
  AxMailBoxItem *aMailBoxItem = dynamic_cast<AxMailBoxItem *>(Get(Current()));

  // counting starts with 1 as the first (0) is the header-line
  for (int i = 1; i < Count(); ++i)
  {
    checkMailBox(i);
  } // if

  clearStatusMsg();

  if (aMailBoxItem)
  {
//    SetCurrent(aMailBoxItem);
    DisplayCurrent(true);
  } // if
  updateActions();

} // AxMenuMailBoxView::checkMailBoxes()

//-------------------------------------------------------------------------
//     AxMenuMailBoxView::checkMailBox()
//-------------------------------------------------------------------------
void AxMenuMailBoxView::checkMailBox(int theIndex)
{
  if (theIndex > 0 && theIndex <= Count())
  {
    AxMailBoxItem *aMailBoxItem = dynamic_cast<AxMailBoxItem *>(Get(theIndex));

    if (aMailBoxItem)
    {
      Mail::MailBox *aMB = aMailBoxItem->getMailBox();

      aMB->getLogHandler().clear();

      //----- show "checking mailbox <mailboxname>..." -----
      std::string aMsg = Ax::Tools::String::sprintf( "%s %s..."
                                                   , tr("Checking mailbox")
                                                   , aMB->getSetup().getAccountName().c_str()
                                                   );
      showStatusMsg(aMsg.c_str(), 0);

      //----- update mailbox -----
      aMB->getCurrentFolder()->requestStatus();
      aMailBoxItem->Set();
      Display();
    } // if
  } // if
} // AxMenuMailBoxView::checkMailBox()

//-------------------------------------------------------------------------
//     AxMenuMailBoxView::doOpenCurrentMailBox()
//-------------------------------------------------------------------------
eOSState AxMenuMailBoxView::doOpenCurrentMailBox()
{
  eOSState aState = osUnknown;

  Mail::MailBox *aMB = getCurrentMailBox();
  if (aMB)
  {
    if (!aMB->getCurrentFolder()->hasStatus())
    {
      showStatusMsg(aMB->getLastImportantMsg());
    }
    else if (aMB->getCurrentFolder()->getCountMails() == 0)
    {
      showStatusMsg(tr("Mailbox is empty"));
    }
    else if (aMB->getCurrentFolder()->getCountMails() > 0)
    {
      cOsdMenu *anOsdMenu = new AxMenuMailListView(myPlugin, aMB->getCurrentFolder(), false);
      aState = AddSubMenu(anOsdMenu);
    } // if
  } // if

  return aState;
} // AxMenuMailBoxView::doOpenCurrentMailBox()

//-------------------------------------------------------------------------
//     AxMenuMailBoxView::informSubOSDClosed()
//-------------------------------------------------------------------------
void AxMenuMailBoxView::informSubOSDClosed(const Ax::Vdr::Action &theAction, eKeys theKey)
{
  PARENT::informSubOSDClosed(theAction, theKey);

  switch (theAction.getID())
  {
    case A_SHOW_LOG:
    {
      // if we returned from a log-view we don't want to check the current again
      break;
    }
    case A_OPEN_MAILBOX:
    {
      // if we returned from a MailBoxView, we want to check the current again
      // -> mail list view disappeared -> update current mailbox
      checkMailBox(Current());
      break;
    }
    default:
    {
    }
  } // switch

  updateActions();

  Get(Current())->Set();
  DisplayCurrent(true);

} // AxMenuMailBoxView::informSubOSDClosed()

//-------------------------------------------------------------------------
//     AxMenuMailBoxView::informCurrentChanged()
//-------------------------------------------------------------------------
void AxMenuMailBoxView::informCurrentChanged(int)
{
  updateActions();
} // AxMenuMailBoxView::updateActions()
