//=============================================================================
/*
 * See the file README in the main directory for a description of
 * this software, copyright information, and how to reach the author.
 *
 * Author: alex
 * Date:   10.12.2004
 *
 * Last modfied:
 *   $Author: alex $
 *   $Date: 2004-12-01 21:55:20 +0100 (Mi, 01 Dez 2004) $
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
#include <vdr/i18n.h>
#include <vdr/menuitems.h>

//----- AxLib -----------------------------------------------------------------
#include <Ax/Tools/Trace.h>
// #include <Ax/Tools/TraceOn.h>
#include <Ax/Tools/String.h>
#include <Ax/Mail/MailBox.h>
#include <Ax/Mail/MailFolder.h>

//----- local plugin ----------------------------------------------------------
#include "AxMenuFolders.h"
#include "AxMenuLogView.h"

//----- locals ----------------------------------------------------------------

//=============================================================================
//     using namespaces
//=============================================================================
using namespace Ax;

//=============================================================================
//     typedefs
//=============================================================================

//=============================================================================
//     class AxFolderItem
//=============================================================================
class AxFolderItem : public cOsdItem
{
  typedef cOsdItem PARENT;

  public:
    //-------------------------------------------------------------------------
    //     AxFolderItem()
    //-------------------------------------------------------------------------
    AxFolderItem( AxMenuFolders    *theOSDMenu
                , Mail::MailFolder *theFolder
                , int               theLevel = 0
                , eOSState          State    = osUnknown
                );

    //-------------------------------------------------------------------------
    //     ~AxFolderItem()
    //-------------------------------------------------------------------------
    virtual ~AxFolderItem();

    //-------------------------------------------------------------------------
    //     Set()
    //-------------------------------------------------------------------------
    virtual void Set(void);

    //-------------------------------------------------------------------------
    //     getMailFolder()
    //-------------------------------------------------------------------------
    Mail::MailFolder *getFolder() const;

    //-------------------------------------------------------------------------
    //     isOpen()
    //-------------------------------------------------------------------------
    bool isOpen() const;

    //-------------------------------------------------------------------------
    //     setOpen()
    //-------------------------------------------------------------------------
    void setOpen(bool fOpen);

    //-------------------------------------------------------------------------
    //     clearChildCltn()
    //-------------------------------------------------------------------------
    /** Hack: clear the collection of child-items but doesn't delete the
     *        child items.
     *
     *        This method is called for every AxFolderItem in the destructor
     *        of AxMenuFolders to speedup deleting the items
     */
    void clearChildCltn();

  private:
    //-------------------------------------------------------------------------
    //     delSubItems()
    //-------------------------------------------------------------------------
    void delSubItems();

    //-------------------------------------------------------------------------
    //     attributes
    //-------------------------------------------------------------------------
    AxMenuFolders            *myOSDMenu;
    Mail::MailFolder         *myFolder;
    int                       myLevel;
    bool                      myIsOpen;
    std::list<AxFolderItem *> mySubItems;

}; // class AxFolderItem

//-----------------------------------------------------------------------------
//     AxFolderItem::AxFolderItem()
//-----------------------------------------------------------------------------
AxFolderItem::AxFolderItem(AxMenuFolders *theOSDMenu, Mail::MailFolder *theFolder, int theLevel, eOSState State)
            : PARENT   (State)
            , myOSDMenu(theOSDMenu)
            , myFolder (theFolder)
            , myLevel  (theLevel)
            , myIsOpen (false)
{
  Set();
} // AxFolderItem::AxFolderItem()

//-----------------------------------------------------------------------------
//     AxFolderItem::~AxFolderItem()
//-----------------------------------------------------------------------------
AxFolderItem::~AxFolderItem()
{
} // AxFolderItem::~AxFolderItem()

//-----------------------------------------------------------------------------
//     AxFolderItem::getFolder()
//-----------------------------------------------------------------------------
Mail::MailFolder *AxFolderItem::getFolder() const
{
  return myFolder;
} // AxFolderItem::getFolder()

//-----------------------------------------------------------------------------
//     AxFolderItem::Set()
//-----------------------------------------------------------------------------
void AxFolderItem::Set(void)
{
  const int NUMCHRS = 4;

  bool fIsRootFolder = myFolder == myFolder->getMailBox()->getRootFolder();

  std::string aFolderName("");
  if (fIsRootFolder) aFolderName = "[ROOT]";
  else               aFolderName = myFolder->getFolderName();

  std::string aFolderAttrStr = ("");

  std::string aSymbolStr("");
  std::string aSelectStr("");

  if       (!myFolder->isNode()) aSymbolStr = "";
  else if  (isOpen())            aSymbolStr = "[-]";
  else                           aSymbolStr = "[+]";

  if (myFolder->canSelect())     aSelectStr = "S";
  else                           aFolderName = std::string("<") + aFolderName + std::string(">");

  std::string aLine  = aSymbolStr
//                     + std::string("\t") + aSelectStr
                     + std::string("\t") + std::string(NUMCHRS * myLevel, ' ') + aFolderName
                     + aFolderAttrStr
                     ;

  SetText(aLine.c_str());
} // AxFolderItem::Set()

//-------------------------------------------------------------------------
//     AxFolderItem::isOpen()
//-------------------------------------------------------------------------
bool AxFolderItem::isOpen() const
{
  return myIsOpen;
} // AxFolderItem::isOpen()

//-------------------------------------------------------------------------
//     AxFolderItem::setOpen()
//-------------------------------------------------------------------------
void AxFolderItem::setOpen(bool fOpen)
{
  if (myIsOpen == fOpen)
  {
    return;
  } // if

  myIsOpen = fOpen;
  Set(); // update the text of myself
  if (fOpen)
  {
    myOSDMenu->showStatusMsg(tr("getting folder contents..."));
    myOSDMenu->Display();
    if (!myFolder->requestSubFolders())
    {
      myOSDMenu->showStatusMsg(tr("unable to get sub-folders"));
      myOSDMenu->Display();
    }
    else
    {
      AxFolderItem *aPrevItem = this;

      for (Ax::Mail::MailFolderCltn::const_iterator aFolderIter  = myFolder->getSubFolders().begin();
                                                    aFolderIter != myFolder->getSubFolders().end();
                                                  ++aFolderIter)
      {
        Mail::MailFolder *aFolder = *aFolderIter;
        AxFolderItem     *anItem  = new AxFolderItem(myOSDMenu, aFolder, myLevel + 1);

        mySubItems.push_back(anItem);
        myOSDMenu->Add(anItem, false, aPrevItem);

        aPrevItem = anItem;
      } // for
    } // if
    myOSDMenu->clearStatusMsg();
  }
  else
  {
    delSubItems();
  } // if
} // AxFolderItem::setOpen()

//-------------------------------------------------------------------------
//     AxFolderItem::delSubItems()
//-------------------------------------------------------------------------
void AxFolderItem::delSubItems()
{
  for (std::list<AxFolderItem *>::iterator anIter =  mySubItems.begin();
                                           anIter != mySubItems.end();
                                         ++anIter)
  {
    AxFolderItem *aSubItem = *anIter;

    aSubItem->delSubItems();
    myOSDMenu->delItem(aSubItem);
  } // for

  mySubItems.clear();
} // AxFolderItem::delSubItems()

//-------------------------------------------------------------------------
//     AxFolderItem::clearChildCltn()
//-------------------------------------------------------------------------
void AxFolderItem::clearChildCltn()
{
  mySubItems.clear();
} // AxFolderItem::clearChildCltn()

//=============================================================================
//     class AxMenuFolders
//=============================================================================

//-----------------------------------------------------------------------------
//     AxMenuFolders::AxMenuFolders()
//-----------------------------------------------------------------------------
AxMenuFolders::AxMenuFolders(Ax::Mail::MailFolder *&theBaseFolder)
             : PARENT( tr("Folder selection")
                     , tr("Folder selection")
                     , 3 // 3 chars for [+]/[-]
                     )
             , myMailBox(theBaseFolder->getMailBox())
             , myFolder (theBaseFolder)
{
#if VDRVERSNUM >= 10734
  SetMenuCategory(mcPluginSetup);
#endif 
  
  //----- create actions ------------------------------------------------------
  addAction(new Vdr::ActionCallback<PARENT>( this
                                           , A_CHECK_FOLDER
                                           , tr("Test")
                                           , "CheckFolder"
                                           , tr("Check if the folder is accessible")
                                           ));

  addAction(new Vdr::ActionCallback<PARENT>( this
                                           , A_ACCEPT_FOLDER
                                           , tr("Accept")
                                           , "AcceptFolder"
                                           , tr("Select the current folder and close")
                                           ));

  addAction(new Vdr::ActionCallback<PARENT>( this
                                           , A_REJECT_FOLDER
                                           , tr("Abort")
                                           , "RejectFolder"
                                           , tr("Abort selection and close")
                                           ));

  addAction(new Vdr::ActionCallback<PARENT>( this
                                           , A_OPEN_FOLDER
                                           , tr("Open")
                                           , "OpenFolder"
                                           , tr("Open the current folder")
                                           ));

  addAction(new Vdr::ActionCallback<PARENT>( this
                                           , A_CLOSE_FOLDER
                                           , tr("Close")
                                           , "CloseFolder"
                                           , tr("Close the current folder")
                                           ));

  addAction(new Vdr::ActionCallback<PARENT>( this
                                           , A_SHOW_LOG
                                           , tr("Log")
                                           , "ShowLog"
                                           , tr("Show the communication log")
                                           ));

  //----- initially disable all actions ---------------------------------------
  setActionEnabled(A_CHECK_FOLDER , false);
  setActionEnabled(A_ACCEPT_FOLDER, false);
  setActionEnabled(A_REJECT_FOLDER, false);
  setActionEnabled(A_OPEN_FOLDER  , false);
  setActionEnabled(A_CLOSE_FOLDER , false);
  setActionEnabled(A_SHOW_LOG     , false);

  //----- assign actions to keys ----------------------------------------------
  setKeyAction(kBack, A_REJECT_FOLDER);
  setKeyAction(kOk  , A_IGNORE       );

  setButtonActions( 0
                 // red             , green          , yellow           , blue
                  , A_ACCEPT_FOLDER , A_IGNORE       , A_SHOW_LOG       , A_CHECK_FOLDER
                  );

  //----- create title line ---------------------------------------------------
  setup();
  clearStatusMsg();

} // AxMenuFolders::AxMenuFolders()

//-----------------------------------------------------------------------------
//     AxMenuFolders::~AxMenuFolders()
//-----------------------------------------------------------------------------
AxMenuFolders::~AxMenuFolders()
{
  for (int i = 0; i < Count(); ++i)
  {
    AxFolderItem *anItem = dynamic_cast<AxFolderItem *>(Get(i));
    if (anItem) anItem->clearChildCltn();
  } // for

} // AxMenuFolders::~AxMenuFolders()

//-------------------------------------------------------------------------
//     AxMenuFolders::getCurrentFolder()
//-------------------------------------------------------------------------
Ax::Mail::MailFolder *AxMenuFolders::getCurrentFolder()
{
  Mail::MailFolder *aFolder = 0;

  if (const_cast<AxMenuFolders *>(this)->Current() >= 0)
  {
    AxFolderItem *aFolderItem = dynamic_cast<AxFolderItem *>(Get(Current()));

    if (aFolderItem)
    {
      aFolder = aFolderItem->getFolder();
    } // if
  } // if

  return aFolder;
} // AxMenuFolders::getCurrentFolder()

//-------------------------------------------------------------------------
//     AxMenuFolders::delItem()
//-------------------------------------------------------------------------
bool AxMenuFolders::delItem(cOsdItem *theItem)
{
  bool fDeleted = false;

  // not very nice and not performant, but I didn't
  // find a method to remove a cOsdItem from a
  // cOsdMenu by a pointer to an item and I don't
  // think that simply deleting theItem will work
  // because destroying the item directly doesn't
  // keep constistency within cOsdMenu
  if (theItem)
  {
    for (int i = 0; i < Count(); ++i)
    {
      if (Get(i) == theItem)
      {
        fDeleted = true;
        Del(i);
      } // if
    } // for
  } // if

  return fDeleted;
} // AxMenuFolders::delItem()

//-------------------------------------------------------------------------
//     AxMenuFolders::getFolderItem()
//-------------------------------------------------------------------------
AxFolderItem *AxMenuFolders::getFolderItem(std::string theFolderPath)
{
  AxFolderItem *anItem = 0;

  // I know, not very nice and not performant, but it's sufficient ;-)
  for (int i = 0; i < Count(); ++i)
  {
    AxFolderItem *aTmpItem = dynamic_cast<AxFolderItem *>(Get(i));
    if (aTmpItem && aTmpItem->getFolder()->getFolderPath() == theFolderPath)
    {
      anItem = aTmpItem;
      break;
    } // if
  } // for

  return anItem;
} // AxMenuFolders::getFolderItem()

//-------------------------------------------------------------------------
//     AxMenuFolders::processAction()
//-------------------------------------------------------------------------
eOSState AxMenuFolders::processAction(const Ax::Vdr::Action &theAction, eOSState theState)
{
  switch (theAction.getID())
  {
    //---------------------------------------------------------------------
    case A_ACCEPT_FOLDER:
    {
      if (checkCurrentFolder(false))
      {
        myFolder = getCurrentFolder();
        theState = osBack;
      }
      else
      {
        theState = osContinue;
      } // if
      break;
    }
    //---------------------------------------------------------------------
    case A_REJECT_FOLDER:
    {
      theState = osBack;
      myFolder = 0;
      break;
    }
    //---------------------------------------------------------------------
    case A_CHECK_FOLDER:
    {
      checkCurrentFolder(true);
      theState = osContinue;
      break;
    }
    //---------------------------------------------------------------------
    case A_OPEN_FOLDER :
    case A_CLOSE_FOLDER:
    {
      AxFolderItem *aFolderItem = dynamic_cast<AxFolderItem *>(Get(Current()));
      if (aFolderItem)
      {
        aFolderItem->setOpen(theAction.getID() == A_OPEN_FOLDER);
        informCurrentChanged(0); // update actions, title, status
        Display();
      } // if

      theState = osContinue;
      break;
    }
    //---------------------------------------------------------------------
    case A_SHOW_LOG:
    {
      Mail::MailFolder *aFolder = getCurrentFolder();
      if (aFolder)
      {
        theState = AddSubMenu(new AxMenuLogView(myMailBox->getLogHandler()));
      } // if
      break;
    }
    //---------------------------------------------------------------------
    default: //----- give parents a chance --------------------------------
    {
      theState = PARENT::processAction(theAction, theState);
      break;
    }
  } // switch

  return theState;
} // AxMenuFolders::processAction()

//-------------------------------------------------------------------------
//     AxMenuFolders::updateActions()
//-------------------------------------------------------------------------
void AxMenuFolders::updateActions()
{
  cOsdItem         *anItem      = Current() >= 0 ? Get(Current()) : 0;
  AxFolderItem     *aFolderItem = Current() >= 0 ? dynamic_cast<AxFolderItem *>(anItem) : 0;
  Mail::MailFolder *aFolder     = aFolderItem ? aFolderItem->getFolder() : 0;

  if (aFolder && aFolderItem)
  {
    setActionEnabled(A_CHECK_FOLDER , aFolder->canSelect());
    setActionEnabled(A_ACCEPT_FOLDER, aFolder->canSelect());
    setActionEnabled(A_REJECT_FOLDER, true );
    setActionEnabled(A_OPEN_FOLDER  , aFolder->isNode() && !aFolderItem->isOpen());
    setActionEnabled(A_CLOSE_FOLDER , aFolder->isNode() &&  aFolderItem->isOpen());
    setActionEnabled(A_SHOW_LOG     , !myMailBox->getLogHandler().isEmpty());

    if (aFolder->isNode())
    {
      if (aFolderItem->isOpen())
      {
        setKeyAction(kOk    , A_CLOSE_FOLDER);
        setKeyAction(kGreen , A_CLOSE_FOLDER);
      }
      else
      {
        setKeyAction(kOk    , A_OPEN_FOLDER);
        setKeyAction(kGreen , A_OPEN_FOLDER);
      } // if
    }
    else
    {
        setKeyAction(kOk    , A_IGNORE);
        setKeyAction(kGreen , A_IGNORE);
    } // if
  }
  else
  {
    setActionEnabled(A_CHECK_FOLDER , false);
    setActionEnabled(A_ACCEPT_FOLDER, false);
    setActionEnabled(A_REJECT_FOLDER, true );
    setActionEnabled(A_OPEN_FOLDER  , false);
    setActionEnabled(A_CLOSE_FOLDER , false);
    setActionEnabled(A_SHOW_LOG     , false);

    setKeyAction(kOk    , A_IGNORE);
    setKeyAction(kGreen , A_IGNORE);
  } // if

} // AxMenuFolders::updateActions()

//-------------------------------------------------------------------------
//     AxMenuFolders::setup()
//-------------------------------------------------------------------------
bool AxMenuFolders::setup()
{
  //----- clear all items -------------------------------------------------
  Clear();
  showStatusMsg( Ax::Tools::String::sprintf( "%s ..."
                                           , tr("Getting folder hierarchy")
                                           )
              , 0
              );

  // enforce that the osd is displayed now
  Display();

  //----- request delimiter -----------------------------------------------
  bool fOK = true;

  if (fOK)
  {
    fOK = myMailBox->getDelimiter() != '\0';
    if (!fOK)
    {
      clearStatusMsg();
      Skins.Message( mtError
                   , Ax::Tools::String::sprintf( "%s: %s"
                                               , tr("Error")
                                               , tr("unable to request delimiter, see log")
                                               ).c_str()
                  );
    } // if
  } // if

  //----- request top level folders ---------------------------------------
  if (fOK)
  {
    fOK = myMailBox->getRootFolder()->requestSubFolders();
    if (!fOK)
    {
      std::string aMsg = Ax::Tools::String::sprintf( "%s: %s"
                                                  , tr("Error")
                                                  , tr("unable to request folders, see log")
                                                  );
      Skins.Message(mtError, aMsg.c_str());
    } // if
  } // if

  //----- creating top level items ----------------------------------------
  std::map<std::string, AxFolderItem *> aFolderMap;
  if (fOK)
  {
    Ax::Mail::MailFolder *aParentFolder = myMailBox->getRootFolder();
    AxFolderItem *aPrevItem = 0;

    for (Ax::Mail::MailFolderCltn::const_iterator aFolderIter  = aParentFolder->getSubFolders().begin();
                                                  aFolderIter != aParentFolder->getSubFolders().end();
                                                ++aFolderIter)
    {
      Mail::MailFolder *aFolder = *aFolderIter;
      AxFolderItem     *anItem  = new AxFolderItem(this, aFolder, 0);

      aFolderMap[aFolder->getFolderPath()] = anItem;
      Add(anItem, false, aPrevItem);

      aPrevItem = anItem;
    } // for
  } // if

  //----- setting up tree for current folder ------------------------------
  if (fOK)
  {
    Tools::StringVector aFolderVec = Tools::String::split(myFolder->getFolderPath(), myMailBox->getDelimiter(), false);

    std::string   aCurrentPath("");
    AxFolderItem *aCurrentItem = 0;

    for (unsigned int i = 0; i < aFolderVec.size(); ++i)
    {
      if (aCurrentPath.length() > 0)
      {
        aCurrentPath += std::string(1, myMailBox->getDelimiter());
      } // if
      aCurrentPath += aFolderVec[i];

      aCurrentItem = getFolderItem(aCurrentPath);

      if (aCurrentItem)
      {
        if (aCurrentItem->getFolder()->getFolderPath() == myFolder->getFolderPath())
        {
          SetCurrent(aCurrentItem);
          break;
        }
        else if (aCurrentItem->getFolder()->isNode())
        {
          aCurrentItem->setOpen(true);
        } // if
      }
      else
      {
        // not found
        break;
      } // if
    } // for
  } // if

  if (Current() == -1 && Count() > 0)
  {
    SetCurrent(Get(0));
  } // if

  // update actions, status
  informCurrentChanged(0);
  Display();

  return fOK;
} // AxMenuFolders::setup()

//-------------------------------------------------------------------------
//     AxMenuFolders::checkCurrentFolder()
//-------------------------------------------------------------------------
bool AxMenuFolders::checkCurrentFolder(bool fShowStatus)
{
  Mail::MailFolder *aFolder = getCurrentFolder();

  if (aFolder == 0)
  {
    return false;
  } // if

  bool fUsable = false;

  myMailBox->getLogHandler().clear();

  //----- show "checking MailFolder <MailFoldername>..." -----
  std::string aMsg = Ax::Tools::String::sprintf( "%s %s..."
                                                , tr("Checking folder")
                                                , aFolder->getFolderName().c_str()
                                                );

  //----- only update the status-bar -----
  showStatusMsg(aMsg, 0);
//    Display();

  if (aFolder->requestStatus())
  {
    if (fShowStatus)
    {
      showStatusMsg(Ax::Tools::String::sprintf( tr("Access successful, %lu mails, %lu new")
                                              , aFolder->getCountMails()
                                              , aFolder->getCountUnseen()
                                              ));
    } // if
    fUsable = true;
  }
  else
  {
    Skins.Message( mtError
                 , Ax::Tools::String::sprintf( "%s: %s"
                                             , tr("Error")
                                             , myMailBox->getLastImportantMsg().c_str()
                                             ).c_str()
                 );
  } // if
  updateActions();

  return fUsable;
} // AxMenuFolders::checkCurrentFolder()

//-------------------------------------------------------------------------
//     AxMenuFolders::informSubOSDClosed()
//-------------------------------------------------------------------------
void AxMenuFolders::informSubOSDClosed(const Ax::Vdr::Action &theAction, eKeys theKey)
{
//  updateActions();
  informCurrentChanged(Current());
} // AxMenuFolders::informSubOSDClosed()

//-------------------------------------------------------------------------
//     AxMenuFolders::informCurrentChanged()
//-------------------------------------------------------------------------
void AxMenuFolders::informCurrentChanged(int theOldCurrent)
{
  updateActions();

  cOsdItem         *anItem      = Current() >= 0 ? Get(Current()) : 0;
  AxFolderItem     *aFolderItem = Current() >= 0 ? dynamic_cast<AxFolderItem *>(anItem) : 0;
  Mail::MailFolder *aFolder     = aFolderItem ? aFolderItem->getFolder() : 0;

  if (aFolder && aFolderItem)
  {
    if (aFolder->canSelect())
    {
      showStatusMsg(aFolder->getFolderPath(), 0);
    }
    else
    {
      showStatusMsg(tr("not selectable"), 0);
    } // if
  }
  else
  {
    clearStatusMsg();
  } // if
} // AxMenuFolders::updateActions()
