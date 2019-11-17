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
//----- C ---------------------------------------------------------------------
//----- vdr -------------------------------------------------------------------
#include <vdr/tools.h>
#include <vdr/i18n.h>
#include <vdr/menuitems.h>

//----- AxLib -----------------------------------------------------------------
#include <Ax/Tools/Trace.h>
// #include <Ax/Tools/TraceOn.h>
#include <Ax/Tools/String.h>
#include <Ax/Mail/Mail.h>
#include <Ax/Mail/MailFolder.h>
#include <Ax/Mail/MailBox.h>
#include <Ax/Mail/MailBoxMgr.h>
#include <Ax/Mail/Tools.h>
#include <Ax/Vdr/Tools.h>

//----- local plugin ----------------------------------------------------------
#include "AxMenuMailView.h"
#include "AxMenuMailListView.h"
#include "AxVdrTools.h"
#include "AxPluginMailBox.h"
#include "AxPluginSetup.h"

//----- locals ----------------------------------------------------------------

#define SHOW_UID 0

//=============================================================================
//     using namespaces
//=============================================================================
using namespace std;
using namespace Ax;

//=============================================================================
//     constants
//=============================================================================
#define ColWithFlags     5
#define ColWithSubject  28

//=============================================================================
//     class AxMailItem
//=============================================================================
class AxMailItem : public cOsdItem
{
  typedef cOsdItem PARENT;

  public:
    //-------------------------------------------------------------------------
    //     AxMailItem()
    //-------------------------------------------------------------------------
    AxMailItem(Mail::Mail *theMail, eOSState theState = osUnknown);

    //-------------------------------------------------------------------------
    //     AxMailItem()
    //-------------------------------------------------------------------------
    AxMailItem(Mail::Mail *theMail, const char *theText, eOSState theState = osUnknown);

    //-------------------------------------------------------------------------
    //     ~AxMailItem()
    //-------------------------------------------------------------------------
    virtual ~AxMailItem();

    //-------------------------------------------------------------------------
    //     Set()
    //-------------------------------------------------------------------------
    virtual void Set(void);

    //-------------------------------------------------------------------------
    //     getMail()
    //-------------------------------------------------------------------------
    Mail::Mail *getMail();

  private:
    //-------------------------------------------------------------------------
    //     createCacheValues()
    //-------------------------------------------------------------------------
    void createCacheValues();

    //-------------------------------------------------------------------------
    //     attributes
    //-------------------------------------------------------------------------
    Mail::Mail *myMail;

    // cache these values as they won't change
    std::string mySubject;
    std::string myFrom   ;
}; // class AxMailItem

//-----------------------------------------------------------------------------
//     AxMailItem::AxMailItem()
//-----------------------------------------------------------------------------
AxMailItem::AxMailItem(Mail::Mail *theMail, eOSState theState)
          : PARENT   (theState)
          , myMail   (theMail)
{
  wsdebug(("> AxMailItem::AxMailItem() Mail: %p  State: %d", theMail, theState));
  createCacheValues();
  wsdebug(("< AxMailItem::AxMailItem() Mail: %p  State: %d", theMail, theState));
} // AxMailItem::AxMailItem()

//-----------------------------------------------------------------------------
//     AxMailItem::AxMailItem()
//-----------------------------------------------------------------------------
AxMailItem::AxMailItem(Mail::Mail *theMail, const char *theText, eOSState theState)
          : PARENT(theText, theState)
          , myMail(theMail)
{
  wsdebug(("> AxMailItem::AxMailItem() Mail: %p  State: %d  Text: %p", theMail, theState, theText));
  createCacheValues();
  wsdebug(("< AxMailItem::AxMailItem() Mail: %p  State: %d  Text: %p", theMail, theState, theText));
} // AxMailItem::AxMailItem()

//-----------------------------------------------------------------------------
//     AxMailItem::~AxMailItem()
//-----------------------------------------------------------------------------
AxMailItem::~AxMailItem()
{
  wsdebug(("> AxMailItem::~AxMailItem()"));
  wsdebug(("< AxMailItem::~AxMailItem()"));
} // AxMailItem::~AxMailItem()

//-----------------------------------------------------------------------------
//     AxMailItem::createCacheValues()
//-----------------------------------------------------------------------------
void AxMailItem::createCacheValues()
{
  wsdebug(("> AxMailItem::createCacheValues() Mail: %p", myMail));
  if (mySubject.empty())
  {
    if (!Ax::Mail::Tools::decodeEncodedWords(myMail->getSubject(), mySubject, AxVdrTools::getVdrCharSet(), true))
    {
      wswarn(("AxMailItem::AxMailItem() unable to decode subject line '%s'", mySubject.c_str()));
    } // if
  } // if

  wsdebug(("  AxMailItem::createCacheValues() Mail: %p", myMail));

  if (myFrom.empty())
  {
    if (!Ax::Mail::Tools::decodeEncodedWords(myMail->getFrom(), myFrom, AxVdrTools::getVdrCharSet(), true))
    {
      wswarn(("AxMailItem::AxMailItem() unable to decode from line '%s'", myFrom.c_str()));
    } // if
  } // if
  wsdebug(("< AxMailItem::createCacheValues() Mail: %p", myMail));
} // AxMailItem::createCacheValues()

//-----------------------------------------------------------------------------
//     AxMailItem::getMail()
//-----------------------------------------------------------------------------
Mail::Mail *AxMailItem::getMail()
{
  return myMail;
} // AxMailItem::getMail()

//-----------------------------------------------------------------------------
//     AxMailItem::Set()
//-----------------------------------------------------------------------------
void AxMailItem::Set(void)
{
  // the %80s avoids that parts of the subject are displayed behind
  // the sender, if the subject is very long and the sender is
  // very short.
#if SHOW_UID
  std::string aLine = Ax::Tools::String::sprintf( "%4d\t %s\t  %-80s"
                                                , myMail->getUID()
                                                , mySubject.c_str()
                                                , myFrom   .c_str()
                                                );
#else
  std::string aLine = Ax::Tools::String::sprintf( "%s\t %s\t  %-80s"
                                                , AxMenuMailListView::getFlagsStr(myMail).c_str()
                                                , mySubject.c_str()
                                                , myFrom   .c_str()
                                                );
#endif

  SetText(aLine.c_str());
} // AxMailItem::Set()

//=============================================================================
//     class AxMenuMailListView
//=============================================================================

//-----------------------------------------------------------------------------
//     AxMenuMailListView::AxMenuMailListView()
//-----------------------------------------------------------------------------
AxMenuMailListView::AxMenuMailListView( AxPluginMailBox      *thePlugin
                                      , Ax::Mail::MailFolder *theFolder
                                      , bool                  theIsMainMenuAction
                                      )
               : PARENT( thePlugin
                       , theFolder
                       , 0                     // theParentMailView
                       , 0                     // theMail (no current mail yet)
                       , tr("MailList-View")
                       , ""
                       , ColWithFlags
                       , ColWithSubject
                       )
               , myIsMainMenuAction(theIsMainMenuAction)
{
#if VDRVERSNUM >= 10734
  SetMenuCategory(mcPlugin);
#endif 
  
  bool fOK = true;

  //----- update title bar ----------------------------------------------------
  updateTitle();

  //----- show "checking mailbox <mailboxname>..." ----------------------------
  Display();
  showStatusMsg(tr("Getting mails..."), 0);

  //----- open the mailbox ----------------------------------------------------
  if (fOK)
  {
    fOK = theFolder->open();
    if (!fOK)
    {
      showStatusMsg(tr("Unable to open mail account!"), 0);
    } // if
  } // if

  //----- request the current status ------------------------------------------
  if (fOK)
  {
    fOK = theFolder->requestStatus();
    if (!fOK)
    {
      showStatusMsg(tr("Unable to request status!"), 0);
    } // if
  } // if

  //----- create actions ------------------------------------------------------
  addAction(new Vdr::ActionCallback<PARENT>( this
                                           , A_OPEN_CURRENT
                                           , 0
                                           , "OpenCurrentMail"
                                           , tr("Open the selected mail")
                                           ));

  addAction(new Vdr::ActionActivateButtonPage<PARENT>
                                           ( this
                                           , A_SHOW_PAGE_JUMP
                                           , tr("Back..." )
                                           , PAGE_JUMP
                                           , "ShowPageJump"
                                           , tr("Use color keys for movement"   )
                                           ));

  addAction(new Vdr::ActionActivateButtonPage<PARENT>
                                           ( this
                                           , A_SHOW_PAGE_MARK
                                           , tr("Flags...")
                                           , PAGE_MARK
                                           , "ShowPageMark"
                                           , tr("Use color keys to toggle flags")
                                           ));

  //----- show the mails ------------------------------------------------------
  if (fOK)
  {
    showMails();
  } // if

  //----- assign actions to keys ----------------------------------------------
  setKeyAction(kOk              , A_OPEN_CURRENT    );

  setKeyAction(k1               , A_TOGGLE_SEEN     );
  setKeyAction(k2               , A_TOGGLE_FLAGGED  );
  setKeyAction(k3               , A_TOGGLE_DELETED  );

  setKeyAction(kFastRew          , A_JUMP_2_PREV    );
  setKeyAction(kFastRew| k_Repeat, A_JUMP_2_PREV    );
  setKeyAction(k4                , A_JUMP_2_PREV    );
  setKeyAction(k4      | k_Repeat, A_JUMP_2_PREV    );

  setKeyAction(k7                , A_JUMP_2_PREV_NEW);
  setKeyAction(k7      | k_Repeat, A_JUMP_2_PREV_NEW);

  // jump to next mail
  setKeyAction(kFastFwd          , A_JUMP_2_NEXT    );
  setKeyAction(kFastFwd| k_Repeat, A_JUMP_2_NEXT    );
  setKeyAction(k6                , A_JUMP_2_NEXT    );
  setKeyAction(k6      | k_Repeat, A_JUMP_2_NEXT    );

  setKeyAction(k9                , A_JUMP_2_NEXT_NEW);
  setKeyAction(k9      | k_Repeat, A_JUMP_2_NEXT_NEW);


  setButtonActions( PAGE_JUMP
                 // red             , green            , yellow           , blue
                  , A_NONE          , A_JUMP_2_PREV_NEW, A_JUMP_2_NEXT_NEW, A_SHOW_PAGE_MARK
                  , A_NONE          , A_JUMP_2_PREV_NEW, A_JUMP_2_NEXT_NEW, A_NONE
                  );

  setButtonActions( PAGE_MARK
                 // red             , green            , yellow           , blue
                  , A_TOGGLE_SEEN   , A_TOGGLE_FLAGGED , A_TOGGLE_DELETED , A_SHOW_PAGE_JUMP
                  , A_NONE          , A_NONE           , A_NONE           , A_NONE
                  );

  if (getButtonPage() != PAGE_JUMP) selectButtonPage(PAGE_JUMP);

  setActionSupported(A_TOGGLE_FLAGGED, getFolder()->supportsFlag(Mail::MF_FLAGGED));
  setActionSupported(A_TOGGLE_SEEN   , getFolder()->supportsFlag(Mail::MF_SEEN   ));
  setActionSupported(A_TOGGLE_DELETED, getFolder()->supportsFlag(Mail::MF_DELETED));

  // if we can't toggle any flags -> disable switching pages
  if ( !isActionSupported(A_TOGGLE_FLAGGED)
    && !isActionSupported(A_TOGGLE_SEEN   )
    && !isActionSupported(A_TOGGLE_DELETED)
     )
  {
    // setting the deleted-flag makes only sense if
    // - we have an IMAP-account (which stores the deleted flag)
    // - ExpungeOnClose is enabled
    setActionSupported(A_SHOW_PAGE_MARK, false);
    setActionSupported(A_SHOW_PAGE_JUMP, false);
  } // if

  //----- select te first mail ------------------------------------------------
  if (Count() > 0)
  {
    SetCurrent(Get(0));
    AxMailItem *anItem = dynamic_cast<AxMailItem *>(Get(Current()));
    if (anItem)
    {
      setCurrentMail(anItem->getMail());
    } // if
  } // if

} // AxMenuMailListView::AxMenuMailListView()

//-----------------------------------------------------------------------------
//     AxMenuMailListView::~AxMenuMailListView()
//-----------------------------------------------------------------------------
AxMenuMailListView::~AxMenuMailListView()
{
#if 1
  getFolder()->close();
#else
  if (Mail::MailBoxMgr::instance()->hasMailBoxPtr(getMailBox()))
  {
    getMailBox()->close();
  }
  else
  {
    wsinfo(("AxMenuMailListView::~AxMenuMailListView() MailBox vanished, assuming it was destroyed before this!"));
  } // if
#endif

  if (myIsMainMenuAction)
  {
    //----- inform the plugin about the "end of osd"  -----
    getPlugin()->MainMenuActionFinished();
  } // if

} // AxMenuMailListView::~AxMenuMailListView()

//-------------------------------------------------------------------------
//     AxMenuMailListView::processAction()
//-------------------------------------------------------------------------
eOSState AxMenuMailListView::processAction(const Ax::Vdr::Action &theAction, eOSState theState)
{
  switch (theAction.getID())
  {
    case A_OPEN_CURRENT:
    {
      if (getCurrentMail())
      {
        cOsdMenu *anOsdMenu = new AxMenuMailView(getPlugin(), getFolder(), getCurrentMail(), this);
        theState = AddSubMenu(anOsdMenu);
      } // if
      break;
    }
    default: //----- give parents a chance --------------------------------
    {
      theState = PARENT::processAction(theAction, theState);
      break;
    }
  } // switch
  return theState;
} // AxMenuMailListView::processAction()

//-------------------------------------------------------------------------
//     AxMenuMailListView::setCurrentMail()
//-------------------------------------------------------------------------
void AxMenuMailListView::setCurrentMail(Ax::Mail::Mail *theMail)
{

  PARENT::setCurrentMail(theMail);

  if (theMail != 0)
  {
    AxMailItem *anItem = dynamic_cast<AxMailItem *>(Get(Current()));
    if (anItem && anItem->getMail() != theMail)
    {
      for (int i = 0; i < Count(); ++i)
      {
        AxMailItem *anItem = dynamic_cast<AxMailItem *>(Get(i));
        Mail::Mail *aMail  = anItem->getMail();

        if (aMail == theMail)
        {
          SetCurrent(anItem);
          break;
        } // if
      } // for
    } // if
#if 0
    wsdebug(("  AxMenuMailListView::setCurrentMail() calling Display()..."));
    Display();
    wsdebug(("  AxMenuMailListView::setCurrentMail() calling Display()...DONE"));
#endif
  } // if

} // AxMenuMailListView::setCurrentMail()

//-------------------------------------------------------------------------
//     AxMenuMailListView::updateActions()
//-------------------------------------------------------------------------
void AxMenuMailListView::updateActions()
{
  PARENT::updateActions();

  //----- refresh title -------
  updateTitle();

  //----- refresh display -----
  Display();

} // AxMenuMailListView::updateActions()

//-------------------------------------------------------------------------
//     AxMenuMailListView::informSubOSDClosed()
//-------------------------------------------------------------------------
void AxMenuMailListView::informSubOSDClosed(const Ax::Vdr::Action &theAction, eKeys theKey)
{
  PARENT::informSubOSDClosed(theAction, theKey);

  for (int i = 0; i < Count(); ++i)
  {
    Get(i)->Set();
  } // for

  updateActions();

} // AxMenuMailListView::informSubOSDClosed()

//-------------------------------------------------------------------------
//     AxMenuMailListView::informCurrentChanged()
//-------------------------------------------------------------------------
void AxMenuMailListView::informCurrentChanged(int)
{
  Ax::Mail::Mail *aMail = (Current() >= 0) ? dynamic_cast<AxMailItem *>(Get(Current()))->getMail() : 0;
  setCurrentMail(aMail);
} // AxMenuMailListView::updateActions()

//-------------------------------------------------------------------------
//     AxMenuMailListView::updateWithCurrentMail()
//-------------------------------------------------------------------------
void AxMenuMailListView::updateWithCurrentMail()
{
  updateActions();
} // AxMenuMailListView::updateWithCurrentMail()

//-------------------------------------------------------------------------
//     AxMenuMailListView::updateTitle()
//-------------------------------------------------------------------------
void AxMenuMailListView::updateTitle()
{
  std::string aMsg  = Ax::Tools::String::sprintf( "%s [%ld/%ld]: %s: %ld"
                                                , getMailBox()     ? getMailBox()    ->getAccountName().c_str()      : "NULL"
                                                , getCurrentMail() ? getCurrentMail()->getMsgNo()       : 0l
                                                , getFolder()      ? getFolder()     ->getCountMails()  : 0l
                                                , tr("MailListView$New")
                                                , getFolder()      ? getFolder()     ->getCountUnseen() : 0l
                                                );

#if SHOW_UID
  aMsg += Ax::Tools::String::sprintf( "UIDNext: %lu", getFolder()->getUIDNext());
#endif

  // set the title in the osd (which will not be updated immediately)
  SetTitle(aMsg.c_str());
} // AxMenuMailListView::updateTitle()

//-------------------------------------------------------------------------
//     AxMenuMailListView::showMails()
//-------------------------------------------------------------------------
void AxMenuMailListView::showMails()
{
  if (getMailBox()->getCurrentFolder()->getCountMails() == 0)
  {
    wsdebug(("  AxMenuMailListView::showMails() %ld Mails", getFolder()->getCountMails()));
    return;
  } // if

  bool fOK    = true;
  long aStart = 0;
  long aEnd   = 0;
  long aInc   = 0;

  if (getPlugin()->getSetup().Settings.MLV_SortOrder == AxVdrTools::SO_ASC)
  {
    aStart = getPlugin()->getSetup().Settings.MLV_MaxMails == 0
           ? 1
           : ::std::max(1L, long(getFolder()->getCountMails()) - long(getPlugin()->getSetup().Settings.MLV_MaxMails));
    aEnd   = getFolder()->getCountMails();
    aInc   = 1;
  }
  else
  {
    aStart = getFolder()->getCountMails();
    aEnd   = getPlugin()->getSetup().Settings.MLV_MaxMails == 0
           ? 1
           : ::std::max(1L, long(getFolder()->getCountMails()) - long(getPlugin()->getSetup().Settings.MLV_MaxMails));
    aInc   = -1;
  } // if

  Mail::Mail *aFirstMail = 0;

  const int aDelta = 1;
  time_t    myMsgTime = time(0) + aDelta;

  for (long i = aStart; i != aEnd + aInc && fOK; i += aInc)
  {
    Mail::Mail *aMail = getFolder()->getMail(i);
    if (aMail == 0)
    {
      wserror(("  AxMenuMailListView::showMails() i: %3ld  unable to get AxMail!", i));
    }
    else
    {
      if (time(0) >= myMsgTime)
      {
        myMsgTime = time(0) + aDelta;

        std::string aMsg = Ax::Tools::String::sprintf( "%s %ld / %ld"
                                                     , tr("Getting mails...")
                                                     , (aStart < aEnd) ? i : (aStart - i + aEnd)
                                                     , getFolder()->getCountMails()
                                                     );
        showStatusMsg(aMsg.c_str(), 0);
        Display();
      } // if

      AxMailItem *anItem = new AxMailItem(aMail, "");
      anItem->Set();
      Add(anItem);

      if (aFirstMail == 0)
      {
        aFirstMail = aMail;
      } // if
    } // if
  } // for

  clearStatusMsg();

} // AxMenuMailListView::showMails()
