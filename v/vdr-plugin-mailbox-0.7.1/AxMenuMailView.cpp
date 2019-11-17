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
#include <string>

//----- C ---------------------------------------------------------------------
//----- vdr -------------------------------------------------------------------
#include <vdr/tools.h>
#include <vdr/font.h>
#include <vdr/i18n.h>
#include <vdr/menuitems.h>
#include <vdr/remote.h>
#include <vdr/status.h>

//----- AxLib -----------------------------------------------------------------
#include <Ax/Tools/Trace.h>
// #include <Ax/Tools/TraceOn.h>
#include <Ax/Tools/Globals.h>
#include <Ax/Tools/String.h>
#include <Ax/Mail/Mail.h>
#include <Ax/Mail/MailFolder.h>
#include <Ax/Mail/MailPart.h>
#include <Ax/Mail/MailBox.h>
#include <Ax/Mail/Tools.h>
#include <Ax/Vdr/Tools.h>

//----- local plugin ----------------------------------------------------------
#include "AxPluginMailBox.h"
#include "AxPluginSetup.h"
#include "AxMenuMailView.h"
#include "AxMenuMailListView.h"
#include "AxVdrTools.h"
#include "AxMailWrapper.h"

//=============================================================================
//     using namespaces
//=============================================================================
using namespace std;
using namespace Ax;

//=============================================================================
//     class AxMenuMailView
//=============================================================================

//-----------------------------------------------------------------------------
//     AxMenuMailView::AxMenuMailView()
//-----------------------------------------------------------------------------
AxMenuMailView::AxMenuMailView( AxPluginMailBox      *thePlugin
                              , Ax::Mail::MailFolder *theFolder
                              , Ax::Mail::Mail       *theMail
                              , AxMenuMailViewBase   *theMailViewParent
                              )
               : PARENT( thePlugin
                       , theFolder
                       , theMail
                       , theMailViewParent
                       , tr("Mail-View")
                       , ""
                       , 9
                       )
               , myMailWrapper (0)
               , myBodyText    ("")
               , myWrapMode    (AxMailWrapper::WrapMode(thePlugin->getSetup().Settings.MV_ViewMode))
               , myUseFixedFont(false)
               , myShowAllParts(false)
{
#if VDRVERSNUM >= 10734
  SetMenuCategory(mcText);
#endif 
  
  //----- create actions ------------------------------------------------------
  addAction(new Vdr::ActionActivateButtonPage<PARENT>
                                           ( this
                                           , A_SHOW_PAGE_JUMP
                                           , tr("Back..." )
                                           , PAGE_JUMP
                                           , "ShowPageJump"
                                           , tr("Use color keys for movement")
                                           ));

  addAction(new Vdr::ActionActivateButtonPage<PARENT>
                                           ( this
                                           , A_SHOW_PAGE_MARK
                                           , tr("Flags...")
                                           , PAGE_MARK
                                           , "ShowPageMark"
                                           , tr("Use color keys to toggle flags")
                                           ));

  addAction(new Vdr::ActionCallback<PARENT>( this
                                           , A_SWITCH_WRAP
                                           , tr("Action$Wrap mode")
                                           , "SwitchWrap"
                                           , tr("Switch word-wrapping mode")
                                           ));

  addAction(new Vdr::ActionCallback<PARENT>( this
                                           , A_SHOW_ALL_PARTS
                                           , tr("Action$Toggle parts")
                                           , "ToggleAllParts"
                                           , tr("Show all parts")
                                           ));

  bool fConvHtml = thePlugin->getSetup().Settings.MailConvCmd.length() > 0
                && thePlugin->getSetup().Settings.MailSrcFile.length() > 0
                && thePlugin->getSetup().Settings.MailDstFile.length() > 0
                 ;

  setActionSupported(A_SHOW_ALL_PARTS, fConvHtml);

  //----- query width and font for text area ----------------------------------
  const cFont *aFont  = 0;
  int          aWidth = 0;

  if (myWrapMode > AxMailWrapper::WM_NONE)
  {
    aFont  = DisplayMenu()->GetTextAreaFont(myUseFixedFont);
    aWidth = DisplayMenu()->GetTextAreaWidth();

    if (aFont == 0 || aWidth == 0)
    {
      wsinfo(("unable to query osd-properties for the current skin '%s'"
            , Skins.Current()->Name()
            ));
      wsinfo(("word-wrapping-mode %s is only supported for the two standard skins and Elchi"
            , AxVdrTools::WrapModeStrings[myWrapMode]
            ));

      myWrapMode = AxMailWrapper::WM_NONE;

      wsinfo(("using wrapping-mode %s for now"
            , AxVdrTools::WrapModeStrings[myWrapMode]
            ));

      setActionSupported(A_SWITCH_WRAP, false);
    } // if
  } // if

  myMailWrapper = new AxMailWrapper(getPlugin(), aFont, aWidth);

  //----- show the mail -------------------------------------------------------
  updateWithCurrentMail();

  //----- assign actions to keys ----------------------------------------------
  setKeyAction(k1               , A_TOGGLE_SEEN     );
  setKeyAction(k2               , A_TOGGLE_FLAGGED  );
  setKeyAction(k3               , A_TOGGLE_DELETED  );

  setKeyAction(k5                , A_SWITCH_WRAP    );

  // jump to previous mail
  setKeyAction(kFastRew          , A_JUMP_2_PREV    );
  setKeyAction(kFastRew| k_Repeat, A_JUMP_2_PREV    );
  setKeyAction(k4                , A_JUMP_2_PREV    );
  setKeyAction(k4      | k_Repeat, A_JUMP_2_PREV    );

  // jump to next mail
  setKeyAction(kFastFwd          , A_JUMP_2_NEXT    );
  setKeyAction(kFastFwd| k_Repeat, A_JUMP_2_NEXT    );
  setKeyAction(k6                , A_JUMP_2_NEXT    );
  setKeyAction(k6      | k_Repeat, A_JUMP_2_NEXT    );

  // jump to previous unread mail
  setKeyAction(k7                , A_JUMP_2_PREV_NEW);
  setKeyAction(k7      | k_Repeat, A_JUMP_2_PREV_NEW);

  // toggle display of all parts
  setKeyAction(k8                , A_SHOW_ALL_PARTS );

  // jump to next unread mail
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
                  );

  setActionSupported(A_TOGGLE_FLAGGED, getFolder()->supportsFlag(Mail::MF_FLAGGED));
  setActionSupported(A_TOGGLE_SEEN   , getFolder()->supportsFlag(Mail::MF_SEEN   ));
  setActionSupported(A_TOGGLE_DELETED, getFolder()->supportsFlag(Mail::MF_DELETED));

  if (getButtonPage() != PAGE_JUMP) selectButtonPage(PAGE_JUMP);

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

} // AxMenuMailView::AxMenuMailView()

//-----------------------------------------------------------------------------
//     AxMenuMailView::~AxMenuMailView()
//-----------------------------------------------------------------------------
AxMenuMailView::~AxMenuMailView()
{
  delete myMailWrapper;
} // AxMenuMailView::~AxMenuMailView()

//-------------------------------------------------------------------------
//     AxMenuMailView::processAction()
//-------------------------------------------------------------------------
eOSState AxMenuMailView::processAction(const Ax::Vdr::Action &theAction, eOSState theState)
{
  switch (theAction.getID())
  {
    case A_SWITCH_WRAP:
    {
      myWrapMode = AxMailWrapper::WrapMode(myWrapMode + 1);
      if (myWrapMode >= AxMailWrapper::WM_COUNT) myWrapMode = AxMailWrapper::WM_NONE;

      updateWithCurrentMail();
      showStatusMsg( ::Ax::Tools::String::sprintf( "%s: %s"
                                               , tr("Info$Wrap mode")
                                               , std::string(AxVdrTools::WrapModeStrings[myWrapMode]).c_str()
                                               )
                   );
      break;
    }
    case A_SHOW_ALL_PARTS:
    {
      myShowAllParts = !myShowAllParts;
      getAction(A_SHOW_ALL_PARTS)->setDesc(myShowAllParts ? tr("Show text parts") : tr("Show all parts"));
      updateWithCurrentMail();
      break;
    }
    default: //----- give parents a chance --------------------------------
    {
      theState = PARENT::processAction(theAction, theState);
      break;
    }
  } // switch
  return theState;
} // AxMenuMailView::processAction()

//-------------------------------------------------------------------------
//     AxMenuMailView::ProcessKey()
//-------------------------------------------------------------------------
eOSState AxMenuMailView::ProcessKey(eKeys theKey)
{
  //--------------------------------------------------------------------------------------------
  // scroll the text-area ('borrowed from vdr/menu.c:  eOSState cMenuText::ProcessKey(eKeys Key)
  if (!HasSubMenu())
  {
    switch (int(theKey))
    {
      case kUp|k_Repeat:
      case kUp:
      case kDown|k_Repeat:
      case kDown:
      case kLeft|k_Repeat:
      case kLeft:
      case kRight|k_Repeat:
      case kRight:
      {
        DisplayMenu()->Scroll( NORMALKEY(theKey) == kUp   || NORMALKEY(theKey) == kLeft   // up?
                             , NORMALKEY(theKey) == kLeft || NORMALKEY(theKey) == kRight  // page?
                             );
        cStatus::MsgOsdTextItem(NULL, NORMALKEY(theKey) == kUp || NORMALKEY(theKey) == kLeft);
        return osContinue;
        break;
      }
      default:
      {
        break;
      }
    } // switch
  } // if

  //--------------------------------------------------------------------------------------------
  eOSState aState = PARENT::ProcessKey(theKey);

  return aState;
} // AxMenuMailView::ProcessKey()

//-------------------------------------------------------------------------
//     AxMenuMailView::setCurrentMail()
//-------------------------------------------------------------------------
void AxMenuMailView::setCurrentMail(Ax::Mail::Mail *theMail)
{
  myShowAllParts = false;
  PARENT::setCurrentMail(theMail);
} // AxMenuMailView::setCurrentMail()

//-------------------------------------------------------------------------
//     AxMenuMailView::Display()
//-------------------------------------------------------------------------
void AxMenuMailView::Display(void)
{
  PARENT::Display();

  //----- display the mail text 
  DisplayMenu()->SetText(myBodyText.c_str(), myUseFixedFont);
   
  //----- forward the text to status receivers (e.g. for graphlcd/tft)
  if (!myBodyText.empty())
  {
    cStatus::MsgOsdTextItem(myBodyText.c_str());
  } // if

} // AxMenuMailView::Display()


//-------------------------------------------------------------------------
//     AxMenuMailView::updateActions()
//-------------------------------------------------------------------------
void AxMenuMailView::updateActions()
{
  PARENT::updateActions();

  //----- refresh title -------
  std::string aStr = Ax::Tools::String::sprintf( "%s: %ld / %ld [%s]"
                                               , getMailBox()     ? getMailBox()->getAccountName().c_str()                    : ""
                                               , getCurrentMail() ? getCurrentMail()->getMsgNo()                              : 0
                                               , getMailBox()     ? getFolder()     ->getCountMails()                         : 0
                                               , getCurrentMail() ? AxMenuMailListView::getFlagsStr(getCurrentMail()).c_str() : ""
                                               );
  SetTitle(aStr.c_str());

  //----- refresh display -----
  Display();

} // AxMenuMailView::updateActions()

//-------------------------------------------------------------------------
//     AxMenuMailView::updateWithCurrentMail()
//-------------------------------------------------------------------------
void AxMenuMailView::updateWithCurrentMail()
{
  myBodyText = "";

  Ax::Mail::Mail *aMail = getCurrentMail();
  if (getCurrentMail())
  {
    std::string aText;

    //----- Subject: --------------------------------------------------------
    if (!Ax::Mail::Tools::decodeEncodedWords(aMail->getSubject(), aText, AxVdrTools::getVdrCharSet(), true))
    {
      wswarn(("AxMenuMailView::updateWithCurrentMail() unable to decode subject line '%s'", aText.c_str()));
    } // if
    myBodyText += string(tr("Subject")) + ": " + aText + "\n";

    //----- From: ------------------------------------------------------------
    if (!Ax::Mail::Tools::decodeEncodedWords(aMail->getFrom(), aText, AxVdrTools::getVdrCharSet(), true))
    {
      wswarn(("AxMenuMailView::updateWithCurrentMail() unable to decode from line '%s'", aText.c_str()));
    } // if
    myBodyText += string(tr("From"   )) + ": " + aText + "\n";

    //----- Date: ------------------------------------------------------------
    myBodyText += string(tr("Date"   )) + ": " + aMail->getDate() + "\n";

    //----- Body: ------------------------------------------------------------
    myBodyText += std::string("\n")
               +  myMailWrapper->getMailText(aMail, myWrapMode, myShowAllParts);
  } // if

  //----- update title and buttons ------------------------------------------
  updateActions();

} // AxMenuMailView::updateWithCurrentMail()

//-------------------------------------------------------------------------
//     AxMenuMailView::informSubOSDClosed()
//-------------------------------------------------------------------------
void AxMenuMailView::informSubOSDClosed(const Ax::Vdr::Action &theAction, eKeys theKey)
{
  PARENT::informSubOSDClosed(theAction, theKey);

  updateWithCurrentMail();

} // AxMenuMailView::informSubOSDClosed()
