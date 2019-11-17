//=============================================================================
/*
 * See the file README in the main directory for a description of
 * this software, copyright information, and how to reach the author.
 *
 * Author: alex
 * Date:   19.06.2004
 *
 * Last modfied:
 *   $Author: alex $
 *   $Date: 2013-03-20 20:33:18 +0100 (Mi, 20 Mär 2013) $
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
#include <string>

//----- C ---------------------------------------------------------------------
//----- vdr -------------------------------------------------------------------
#include <vdr/tools.h>
#include <vdr/font.h>
#include <vdr/i18n.h>
#include <vdr/menuitems.h>
#include <vdr/remote.h>

//----- AxLib -----------------------------------------------------------------
#include <Ax/Tools/Trace.h>
// #include <Ax/Tools/TraceOn.h>
#include <Ax/Mail/Mail.h>
#include <Ax/Mail/MailFolder.h>
#include <Ax/Mail/MailPart.h>
#include <Ax/Mail/MailBox.h>
#include <Ax/Vdr/Tools.h>

//----- local plugin ----------------------------------------------------------
#include "AxPluginMailBox.h"
#include "AxPluginSetup.h"
#include "AxMenuMailViewBase.h"

//=============================================================================
//     using namespaces
//=============================================================================
using namespace std;
using namespace Ax;

//=============================================================================
//     class AxMenuMailViewBase::AxActionToggleFlag
//=============================================================================

//-------------------------------------------------------------------------
//     AxActionToggleFlag::AxActionToggleFlag()
//-------------------------------------------------------------------------
AxMenuMailViewBase::AxActionToggleFlag::AxActionToggleFlag( AxMenuMailViewBase *theMailView
                                                          , int                 theID
                                                          , Ax::Mail::MailFlag  theFlag
                                                          , const char         *theButtonTextOn
                                                          , const char         *theButtonTextOff
                                                          , const std::string  &theName
                                                          , const std::string  &theDesc
                                                          )
  : PARENT         (theID, theName, theDesc)
  , myMailView     (theMailView)
  , myFlag         (theFlag)
  , myButtonTextOn (theButtonTextOn )
  , myButtonTextOff(theButtonTextOff)
{
} // AxActionToggleFlag::AxActionToggleFlag()

//-------------------------------------------------------------------------
//    AxActionToggleFlag::~AxActionToggleFlag()
//-------------------------------------------------------------------------
AxMenuMailViewBase::AxActionToggleFlag::~AxActionToggleFlag()
{
} // AxActionToggleFlag::~AxActionToggleFlag()

//------------------------------------------------------------------------
//    AxActionToggleFlag::getButtonTextInternal()
//------------------------------------------------------------------------
const char *AxMenuMailViewBase::AxActionToggleFlag::getButtonTextInternal() const
{
  Ax::Mail::Mail *aMail = myMailView->getCurrentMail();
  if (aMail)
  {
    if (aMail->getFlag(myFlag)) return myButtonTextOn ;
    else                        return myButtonTextOff;
  } // if
  return 0;
} // AxActionToggleFlag::getButtonTextInternal()

//-------------------------------------------------------------------------
//     AxActionToggleFlag::runInternal()
//-------------------------------------------------------------------------
eOSState AxMenuMailViewBase::AxActionToggleFlag::runInternal(eOSState theStateBefore)
{
  Ax::Mail::Mail *aMail = myMailView->getCurrentMail();
  if (aMail)
  {
    if (aMail->getFlag(myFlag)) aMail->clearFlag(myFlag);
    else                        aMail->setFlag  (myFlag);

    if (myMailView->Get(myMailView->Current())) myMailView->Get(myMailView->Current())->Set();

#if 0
#if !STAY_ON_MARK_PAGE
    AxAction *anAction = myMailView->getAction(A_SWITCH_BUTTONS);
    if (anAction) anAction->run(theStateBefore);
#endif
#endif
    myMailView->setUpdateColorButtons();
    myMailView->updateActions();
  } // if

  return osContinue;
} // AxActionToggleFlag::runInternal()

//=============================================================================
//     class AxMenuMailViewBase
//=============================================================================

//-----------------------------------------------------------------------------
//     AxMenuMailViewBase::AxMenuMailViewBase()
//-----------------------------------------------------------------------------
AxMenuMailViewBase::AxMenuMailViewBase( AxPluginMailBox      *thePlugin
                                      , Ax::Mail::MailFolder *theFolder
                                      , Ax::Mail::Mail       *theMail
                                      , AxMenuMailViewBase   *theParentMailView
                                      , std::string           theInstName
                                      , const char           *theTitle
                                      , int c0, int c1, int c2, int c3, int c4
                                      )
               : PARENT   (theInstName, theTitle, c0, c1, c2, c3, c4)
               , myPlugin (thePlugin )
               , myFolder (theFolder )
               , myMail   (theMail   )
               , myPrevNew(0)
               , myNextNew(0)
               , myPrev   (0)
               , myNext   (0)
               , myParent (theParentMailView)
{
  //----- create actions ------------------------------------------------------
  addAction(new Vdr::ActionCallback<PARENT>( this
                                           , A_JUMP_2_PREV
                                           , 0
                                           , "Jump2Prev"
                                           , tr("Jump to the previous mail")
                                           ));

  addAction(new Vdr::ActionCallback<PARENT>( this
                                           , A_JUMP_2_NEXT
                                           , 0
                                           , "Jump2Next"
                                           , tr("Jump to the next mail")
                                           ));

  addAction(new Vdr::ActionCallback<PARENT>( this
                                           , A_JUMP_2_PREV_NEW
                                           , tr("<< New")
                                           , "Jump2PrevNew"
                                           , tr("Jump to the previous unread mail")
                                           ));

  addAction(new Vdr::ActionCallback<PARENT>( this
                                           , A_JUMP_2_NEXT_NEW
                                           , tr("New >>")
                                           , "Jump2NextNew"
                                           , tr("Jump to the next unread mail")
                                           ));

  addAction(new AxActionToggleFlag         ( this
                                           , A_TOGGLE_FLAGGED
                                           , Mail::MF_FLAGGED
                                           , tr("ButtonFlag$Not Flagged")
                                           , tr("ButtonFlag$Flagged"    )
                                           , "ToggleFlagged"
                                           , tr("Toggle flagged state")
                                           ));

  addAction(new AxActionToggleFlag         ( this
                                           , A_TOGGLE_SEEN
                                           , Mail::MF_SEEN
                                           , tr("ButtonFlag$New")
                                           , tr("ButtonFlag$Seen")
                                           , "ToggleSeen"
                                           , tr("Toggle seen state")
                                           ));

  addAction(new AxActionToggleFlag         ( this
                                           , A_TOGGLE_DELETED
                                           , Mail::MF_DELETED
                                           , tr("ButtonFlag$Undelete")
                                           , tr("ButtonFlag$Delete"  )
                                           , "ToggleDeleted"
                                           , tr("Toggle deleted state")
                                           ));

#if 0
  // setting the deleted-flag makes only sense if
  // - we have an IMAP-account (which stores the deleted flag)
  // - ExpungeOnClose is enabled
  setActionSupported( A_TOGGLE_DELETED
                    ,  myFolder->getMailBox()->isIMAP()
                    || myFolder->getMailBox()->getSetup().getExpungeOnClose()
                    );
#endif

} // AxMenuMailViewBase::AxMenuMailViewBase()

//-----------------------------------------------------------------------------
//     AxMenuMailViewBase::~AxMenuMailViewBase()
//-----------------------------------------------------------------------------
AxMenuMailViewBase::~AxMenuMailViewBase()
{
} // AxMenuMailViewBase::~AxMenuMailViewBase()

//-------------------------------------------------------------------------
//     AxMenuMailViewBase::processAction()
//-------------------------------------------------------------------------
eOSState AxMenuMailViewBase::processAction(const Ax::Vdr::Action &theAction, eOSState theState)
{
  switch (theAction.getID())
  {
    case A_JUMP_2_PREV:
    {
      if (myPrev && myPrev != myMail)
      {
        setCurrentMail(myPrev);
      } // if
      break;
    }
    case A_JUMP_2_NEXT:
    {
      if (myNext && myNext != myMail)
      {
        setCurrentMail(myNext);
      } // if
      break;
    }
    case A_JUMP_2_PREV_NEW:
    {
      if (myPrevNew && myPrevNew != myMail)
      {
        setCurrentMail(myPrevNew);
      } // if
      break;
    }
    case A_JUMP_2_NEXT_NEW:
    {
      if (myNextNew && myNextNew != myMail)
      {
        setCurrentMail(myNextNew);
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
} // AxMenuMailViewBase::processAction()

//-------------------------------------------------------------------------
//     AxMenuMailViewBase::updateActions()
//-------------------------------------------------------------------------
void AxMenuMailViewBase::updateActions()
{
  if (myMail)
  {
    myFolder->getSurroundings(myMail, &myPrev, &myNext, &myPrevNew, &myNextNew);
  }
  else
  {
    myPrev = myNext = myPrevNew = myNextNew = 0;
  } // if

  setActionEnabled(A_TOGGLE_FLAGGED , myMail    != 0);
  setActionEnabled(A_TOGGLE_SEEN    , myMail    != 0);
  setActionEnabled(A_TOGGLE_DELETED , myMail    != 0);

  setActionEnabled(A_JUMP_2_PREV    , myPrev    != 0);
  setActionEnabled(A_JUMP_2_NEXT    , myNext    != 0);
  setActionEnabled(A_JUMP_2_PREV_NEW, myPrevNew != 0);
  setActionEnabled(A_JUMP_2_NEXT_NEW, myNextNew != 0);

} // AxMenuMailViewBase::updateActions()

//-------------------------------------------------------------------------
//     AxMenuMailViewBase::setCurrentMail()
//-------------------------------------------------------------------------
void AxMenuMailViewBase::setCurrentMail(Ax::Mail::Mail *theMail)
{
  // update the color buttons if the current mail != the prev. mail and flags changed
  // or if current or previous mail is 0
  if (myMail != theMail)
  {
    if (  myMail
     &&  theMail
     && (  myMail->getFlag(Mail::MF_SEEN    ) != theMail->getFlag(Mail::MF_SEEN    )
        || myMail->getFlag(Mail::MF_FLAGGED ) != theMail->getFlag(Mail::MF_FLAGGED )
        || myMail->getFlag(Mail::MF_DELETED ) != theMail->getFlag(Mail::MF_DELETED )
        || myMail->getFlag(Mail::MF_ANSWERED) != theMail->getFlag(Mail::MF_ANSWERED)
        )
      )
    {
      setUpdateColorButtons();
    } // if
    else if (myMail == 0 || theMail == 0)
    {
      setUpdateColorButtons();
    } // if
  } // if

  myMail = theMail;

  if (!HasSubMenu())
  {
    updateWithCurrentMail();
  } // if

  if (myParent)
  {
    myParent->setCurrentMail(myMail);
  } // if

} // AxMenuMailViewBase::setCurrentMail()

//-------------------------------------------------------------------------
//     AxMenuMailViewBase::getMailBox()
//-------------------------------------------------------------------------
Ax::Mail::MailBox *AxMenuMailViewBase::getMailBox() const
{
  return myFolder->getMailBox();
} // AxMenuMailViewBase::getMailBox()

//-------------------------------------------------------------------------
//     AxMenuMailViewBase::getFolder()
//-------------------------------------------------------------------------
Ax::Mail::MailFolder *AxMenuMailViewBase::getFolder() const
{
  return myFolder;
} // AxMenuMailViewBase::getFolder()

//-------------------------------------------------------------------------
//     AxMenuMailViewBase::getCurrentMail()
//-------------------------------------------------------------------------
Ax::Mail::Mail *AxMenuMailViewBase::getCurrentMail()
{
  return myMail;
} // AxMenuMailViewBase::getCurrentMail()

//-------------------------------------------------------------------------
//     AxMenuMailViewBase::informSubOSDClosed()
//-------------------------------------------------------------------------
void AxMenuMailViewBase::informSubOSDClosed(const Ax::Vdr::Action &theAction, eKeys theKey)
{
  PARENT::informSubOSDClosed(theAction, theKey);

  updateWithCurrentMail();

} // AxMenuMailViewBase::informSubOSDClosed()

//-------------------------------------------------------------------------
//     AxMenuMailViewBase::getFlagsStr()
//-------------------------------------------------------------------------
std::string AxMenuMailViewBase::getFlagsStr(const Ax::Mail::Mail *theMail)
{
  if (!theMail->hasValidFlags())
  {
    return std::string("");
  } // if

  return std::string(theMail->getIsSeen    ()  ? tr("MailFlag_Seen_On$_"    ) : tr("MailFlag_Seen_Off$N"    ))
       + std::string(theMail->getIsFlagged ()  ? tr("MailFlag_Flagged_On$F" ) : tr("MailFlag_Flagged_Off$_" ))
       + std::string(theMail->getIsDeleted ()  ? tr("MailFlag_Deleted_On$D" ) : tr("MailFlag_Deleted_Off$_" ))
       + std::string(theMail->getIsAnswered()  ? tr("MailFlag_Answered_On$A") : tr("MailFlag_Answered_Off$_"))
       ;
} // AxMenuMailViewBase::getFlagsStr()
