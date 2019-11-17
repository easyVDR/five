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
#include <Ax/Tools/Globals.h>
#include <Ax/Tools/String.h>
#include <Ax/Mail/MailFolder.h>
#include <Ax/Mail/MailBox.h>

//----- local plugin ----------------------------------------------------------
#include "AxMenuConfigMailBox.h"
#include "AxVdrTools.h"
#include "AxPluginSetup.h"
#include "AxPluginMailBox.h"
#include "AxMenuFolders.h"
#include "AxMenuLogView.h"

//----- locals ----------------------------------------------------------------

//=============================================================================
//     using namespaces
//=============================================================================
using namespace Ax;

//=============================================================================
//     static / globals
//=============================================================================
const char *AxMenuConfigMailBox::MailBoxNameChars = " abcdefghijklmnopqrstuvwxyz0123456789-.#@~{}[]()_/";
const char *AxMenuConfigMailBox::HostNameChars    = "abcdefghijklmnopqrstuvwxyz0123456789-._~:";
const char *AxMenuConfigMailBox::UserNameChars    = "abcdefghijklmnopqrstuvwxyz0123456789-+.,:;?!$&#@~{}[]()_/";
const char *AxMenuConfigMailBox::PaswordChars     = "abcdefghijklmnopqrstuvwxyz0123456789-+.,:;?!$&#@~{}[]()_/";
const char *AxMenuConfigMailBox::MailBoxChars     = "abcdefghijklmnopqrstuvwxyz0123456789-.#@~{}[]_/";
const char *AxMenuConfigMailBox::AccessCodeChars  = "0123456789";

//-----------------------------------------------------------------------------
//     AxMenuConfigMailBox::AxMenuConfigMailBox()
//-----------------------------------------------------------------------------
AxMenuConfigMailBox::AxMenuConfigMailBox(AxPluginMailBox *myPlugin, Ax::Mail::MailBoxSetup &theMailBoxSetup)
                   : PARENT( tr("Account settings")
                           , tr("Account settings")
                           , 20
                           )
                   , myPlugin           (myPlugin       )
                   , myMailBoxSetup     (theMailBoxSetup)
                   , myTmpMailBox       ( 0)
                   , mySelectedFolder   ( 0)
                   , myIndexFirstCClient( 0)
                   , myIndexFolder      (-1)
{
#if VDRVERSNUM >= 10734
  SetMenuCategory(mcPluginSetup);
#endif 
  
  convSetup2Dlg(myMailBoxSetup);

  //----- create actions ------------------------------------------------------
  addAction(new Vdr::ActionCallback<PARENT>( this
                                           , A_SAVE_AND_CLOSE
                                           , 0
                                           , "SaveAndClose"
                                           , tr("Save the current settings and go back")
                                           ));

  addAction(new Vdr::ActionCallback<PARENT>( this
                                           , A_CHECK_ACCOUNT
                                           , tr("Test")
                                           , "CheckAccount"
                                           , tr("Check if the mail account is accessible")
                                           ));

  addAction(new Vdr::ActionCallback<PARENT>( this
                                           , A_SHOW_LOG
                                           , tr("Log")
                                           , "ShowLog"
                                           , tr("Show the communication log")
                                           ));
  setActionEnabled(A_SHOW_LOG, myTmpMailBox && !myTmpMailBox->getLogHandler().isEmpty());

  addAction(new Vdr::ActionCallback<PARENT>( this
                                           , A_SELECT_FOLDER
                                           , tr("Select")
                                           , "SelectFolder"
                                           , tr("Select a folder")
                                           ));
  setActionEnabled(A_SELECT_FOLDER, false);

  //----- assign actions to keys ----------------------------------------------
  setKeyAction(kOk    , A_SAVE_AND_CLOSE);

  setKeyAction(kRed   , A_SELECT_FOLDER );
  setKeyAction(kYellow, A_SHOW_LOG      );
  setKeyAction(kBlue  , A_CHECK_ACCOUNT );

  setupItems();

} // AxMenuConfigMailBox::AxMenuConfigMailBox()

//-----------------------------------------------------------------------------
//     AxMenuConfigMailBox::~AxMenuConfigMailBox()
//-----------------------------------------------------------------------------
AxMenuConfigMailBox::~AxMenuConfigMailBox()
{
  Ax::Tools::DeleteNull(myTmpMailBox);
} // AxMenuConfigMailBox::~AxMenuConfigMailBox()

//-------------------------------------------------------------------------
//     AxMenuConfigMailBox::ProcessKey()
//-------------------------------------------------------------------------
eOSState AxMenuConfigMailBox::ProcessKey(eKeys theKey)
{
  int  aPrevAccountType  = myAccountType;
  int  aPrevAccessMode   = myAccessMode ;
  int  aPrevBGCheck      = myBGCheck    ;

  int aPrevClientOptSec  = myClientOptSec ;
  int aPrevClientOptRSH  = myClientOptRSH ;
  int aPrevClientOptSSL  = myClientOptSSL ;
  int aPrevClientOptTLS  = myClientOptTLS ;
  int aPrevClientOptCert = myClientOptCert;
  int aPrevClientOptDbg  = myClientOptDbg ;

  eOSState aState = PARENT::ProcessKey(theKey);

  if (theKey != kNone && !HasSubMenu())
  {
    bool fShowMailBoxStr = false;
    if ( aPrevAccountType != myAccountType
      || aPrevAccessMode  != myAccessMode
      || aPrevBGCheck     != myBGCheck
       )
    {
      setupItems();
      fShowMailBoxStr = true;
    } // if

    if ( fShowMailBoxStr
      || aPrevClientOptSec  != myClientOptSec
      || aPrevClientOptRSH  != myClientOptRSH
      || aPrevClientOptSSL  != myClientOptSSL
      || aPrevClientOptTLS  != myClientOptTLS
      || aPrevClientOptCert != myClientOptCert
      || aPrevClientOptDbg  != myClientOptDbg
       )
    {
      fShowMailBoxStr = true;
      showMailBoxStr();
    } // if

    if (fShowMailBoxStr)
    {
      Display();
    } // if
  } // if

  return aState;
} // AxMenuConfigMailBox::ProcessKey()

//-------------------------------------------------------------------------
//     AxMenuConfigMailBox::processAction()
//-------------------------------------------------------------------------
eOSState AxMenuConfigMailBox::processAction(const Ax::Vdr::Action &theAction, eOSState theState)
{
  switch (theAction.getID())
  {
    //---------------------------------------------------------------------
    case A_SAVE_AND_CLOSE:
    {
      convDlg2Setup(myMailBoxSetup);
      if (myPlugin->getSetup().saveAccounts(cPlugin::ConfigDirectory(myPlugin->Name())))
      {
        theState = osBack;
      }
      else
      {
        Skins.Message(mtError, tr("Unable to write to account definition file!"));
        theState = osContinue;
      } // if
      break;
    }
    //---------------------------------------------------------------------
    case A_CHECK_ACCOUNT:
    {
      checkAccount();
      theState = osContinue;
      break;
    }
    //---------------------------------------------------------------------
    case A_SHOW_LOG:
    {
      theState = showLog();
      break;
    }
    //---------------------------------------------------------------------
    case A_SELECT_FOLDER:
    {
      theState = selectFolder();
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
} // AxMenuConfigMailBox::processAction()

//-------------------------------------------------------------------------
//     AxMenuConfigMailBox::informSubOSDClosed()
//-------------------------------------------------------------------------
void AxMenuConfigMailBox::informSubOSDClosed(const Ax::Vdr::Action &theAction, eKeys theKey)
{
  switch (theAction.getID())
  {
    case A_SELECT_FOLDER:
    {
      if (mySelectedFolder != 0)
      {
        strn0cpy(myFolder, mySelectedFolder->getFolderPath().c_str(), sizeof(myFolder) - 1);
        mySelectedFolder = 0;
        Get(Current())->Set();
        DisplayCurrent(true);
      }
      else
      {
        // SELECTION ABORTED
      } // if
      break;
    }
    case A_SHOW_LOG:
    {
      break;
    }
  } // switch

  setActionEnabled(A_SHOW_LOG, myTmpMailBox && !myTmpMailBox->getLogHandler().isEmpty());
} // AxMenuConfigMailBox::informSubOSDClosed()

//-------------------------------------------------------------------------
//     AxMenuConfigMailBox::informCurrentChanged()
//-------------------------------------------------------------------------
void AxMenuConfigMailBox::informCurrentChanged(int theOldCurrent)
{
  //----- enable / disable actions ---------------------------------------
  if (Current() == myIndexFolder)
  {
    bool fEnableFolder = Current() == myIndexFolder
                      && Mail::MailBoxSetup::supportsOption(myAccountType , Mail::CCO_FOLDER)
                      ;
    setActionEnabled(A_SELECT_FOLDER, fEnableFolder);
  }
  else if (Current() != myIndexFolder && theOldCurrent == myIndexFolder)
  {
    setActionEnabled(A_SELECT_FOLDER, false);
  } // if

  showMailBoxStr();
} // AxMenuConfigMailBox::updateActions()

//-------------------------------------------------------------------------
//     AxMenuConfigMailBox::setupItems()
//-------------------------------------------------------------------------
void AxMenuConfigMailBox::setupItems()
{
  int aCurrent = Current() <= 0 ? 1 : Current();

  Clear();

  //----- VDR settings ---------------------------------------------------
  addSeparator(tr("VDR settings"));
  Add(new cMenuEditStrItem (tr("Account name"         ),  myAccountName     , sizeof(myAccountName) - 1, MailBoxNameChars));
  Add(new cMenuEditStraItem(tr("Restrict access"      ), &myAccessMode      , Mail::AM_COUNT           , AxVdrTools::AccessModeNames));
  if (myAccessMode != Mail::AM_NONE)
  {
    Add(new cMenuEditStrItem (tr("- Access code"      ),  myAccessCode      , sizeof(myAccessCode ) - 1, AccessCodeChars ));
  } // if
  Add(new cMenuEditBoolItem(tr("Auto mark seen"       ), &myFAutoMarkSeen  ));
  Add(new cMenuEditBoolItem(tr("Cleanup on close"     ), &myFExpungeOnClose));

  if (myPlugin->getSetup().Settings.BGCheckDelay > 0)
  {
    Add(new cMenuEditBoolItem(tr("Background check"   ), &myBGCheck));
    if (myBGCheck)
    {
      Add(new cMenuEditBoolItem(tr("- Status message"    ), &myBGCheck_OsdMsg));
      Add(new cMenuEditBoolItem(tr("- Main menu entry"   ), &myBGCheck_Menu  ));
      Add(new cMenuEditBoolItem(tr("- External command"  ), &myBGCheck_ExtApp));
      Add(new cMenuEditBoolItem(tr("- Internal interface"), &myBGCheck_SIF   ));
    } // if
  } // if

  //----- Account settings -----------------------------------------------
  addSeparator(tr("Account settings"));
  myIndexFolder       = -1;
  myIndexFirstCClient = Count();

  Add(new cMenuEditStraItem(tr("Account type"           ), &myAccountType     , Mail::AT_COUNT             , AxVdrTools::AccountTypeNames));

  if (myAccountType == Mail::AT_USER)
  {
    Add(new cMenuEditStrItem(tr("Config$c-client mailbox string"),  myMailBoxString   , sizeof(myMailBoxString) - 1, MailBoxChars    ));
  } // if

  if (Mail::MailBoxSetup::supportsOption(myAccountType , Mail::CCO_HOSTNAME))
  {
    Add(new cMenuEditStrItem (tr("Host-name/IP"         ),  myHostName        , sizeof(myHostName ) - 1    , HostNameChars   ));
  } // if

  if (Mail::MailBoxSetup::supportsOption(myAccountType , Mail::CCO_USERNAME))
  {
    Add(new cMenuEditStrItem (tr("User-name"            ),  myUserName        , sizeof(myUserName ) - 1    , UserNameChars   ));
  } // if

  if (Mail::MailBoxSetup::supportsOption(myAccountType , Mail::CCO_PASSWORD))
  {
    Add(new cMenuEditStrItem (tr("Password"             ),  myPassword        , sizeof(myPassword ) - 1    , PaswordChars    ));
  } // if

  if (Mail::MailBoxSetup::supportsOption(myAccountType , Mail::CCO_FOLDER))
  {
    myIndexFolder = Count();
    Add(new cMenuEditStrItem (tr("Folder"               ),  myFolder         , sizeof(myFolder    ) - 1    , MailBoxChars    ));
  } // if

  if (Mail::MailBoxSetup::supportsOption(myAccountType , Mail::CCO_SEC))
  {
    Add(new cMenuEditStraItem(tr("Secure"               ), &myClientOptSec   , Mail::CC_SEC_COUNT , AxVdrTools::CClientOptSecNames ));
  } // if

  if (Mail::MailBoxSetup::supportsOption(myAccountType , Mail::CCO_RSH))
  {
    Add(new cMenuEditStraItem(tr("Use RSH"              ), &myClientOptRSH   , Mail::CC_RSH_COUNT , AxVdrTools::CClientOptRSHNames ));
  } // if

  if (Mail::MailBoxSetup::supportsOption(myAccountType , Mail::CCO_SSL))
  {
    Add(new cMenuEditStraItem(tr("Use SSL"              ), &myClientOptSSL   , Mail::CC_SSL_COUNT , AxVdrTools::CClientOptSSLNames ));
  } // if

  if (Mail::MailBoxSetup::supportsOption(myAccountType , Mail::CCO_TLS))
  {
    Add(new cMenuEditStraItem(tr("Start TLS"            ), &myClientOptTLS   , Mail::CC_TLS_COUNT , AxVdrTools::CClientOptTLSNames ));
  } // if

  if (Mail::MailBoxSetup::supportsOption(myAccountType , Mail::CCO_CERT))
  {
    Add(new cMenuEditStraItem(tr("Validate certificates"), &myClientOptCert  , Mail::CC_CERT_COUNT, AxVdrTools::CClientOptCertNames));
  } // if

  if (Mail::MailBoxSetup::supportsOption(myAccountType , Mail::CCO_DBG))
  {
    Add(new cMenuEditStraItem(tr("Debug"                ), &myClientOptDbg   , Mail::CC_DBG_COUNT , AxVdrTools::CClientOptDbgNames ));
  } // if

  //----- select old current and display ---------------------------------
  SetCurrent(Get(aCurrent));

} // AxMenuConfigMailBox::setupItems()

//-------------------------------------------------------------------------
//     AxMenuConfigMailBox::showMailBoxStr()
//-------------------------------------------------------------------------
void AxMenuConfigMailBox::showMailBoxStr()
{
  if (Current() >= myIndexFirstCClient)
  {
    Ax::Mail::MailBoxSetup aTmpSetup;
    convDlg2Setup(aTmpSetup);

    showStatusMsg(aTmpSetup.getMailBoxString().c_str(), 0);
  }
  else
  {
    clearStatusMsg();
  } // if
} // AxMenuConfigMailBox::showMailBoxStr()

//-------------------------------------------------------------------------
//     AxMenuConfigMailBox::checkAccount()
//-------------------------------------------------------------------------
bool AxMenuConfigMailBox::checkAccount()
{
  //----- show "checking mailbox <mailboxname>..."
  std::string aMsg = Ax::Tools::String::sprintf( "%s ..."
                                               , tr("Checking mailbox")
                                               );
  showStatusMsg(aMsg.c_str(), 0);

  //----- create old temp mailbox
  Ax::Tools::DeleteNull(myTmpMailBox);

  //----- create new temp mailbox
  Mail::MailBoxSetup aSetup;
  convDlg2Setup(aSetup);

  // alwas produce debug output when checking an account
  // aSetup.setClientOptDbg(Mail::CC_DBG_YES);

  myTmpMailBox = new Mail::MailBox(aSetup);

  bool fHasStatus = myTmpMailBox->getCurrentFolder()->requestStatus();

  setActionEnabled(A_SHOW_LOG, myTmpMailBox && !myTmpMailBox->getLogHandler().isEmpty());

  if (fHasStatus)
  {
    showStatusMsg(Ax::Tools::String::sprintf( tr("Access successful, %lu mails, %lu new")
                                            , myTmpMailBox->getCurrentFolder()->getCountMails()
                                            , myTmpMailBox->getCurrentFolder()->getCountUnseen()
                                            )
                 );
  }
  else
  {
    clearStatusMsg();
    Skins.Message( mtError
                 , Ax::Tools::String::sprintf( "%s"
                                             , myTmpMailBox->getLogHandler().getLastImportantMsg(tr("failed -> see log")).c_str()
                                             ).c_str()
                 );
  } // if

  return fHasStatus;
} // AxMenuConfigMailBox::checkAccount()

//-------------------------------------------------------------------------
//     AxMenuConfigMailBox::showLog()
//-------------------------------------------------------------------------
eOSState AxMenuConfigMailBox::showLog()
{
  eOSState aState = osContinue;
  if (!myTmpMailBox)
  {
    wswarn(("  AxMenuConfigMailBox::showLog() myTmpMailBox == 0"));
  }
  else if (myTmpMailBox->getLogHandler().isEmpty())
  {
    wswarn(("  AxMenuConfigMailBox::showLog() myTmpMailBox->getLogHandler().isEmpty()"));
  }
  else
  {
    aState = AddSubMenu(new AxMenuLogView(myTmpMailBox->getLogHandler()));
  }  // if

  return aState;
} // AxMenuConfigMailBox::showLog()

//-------------------------------------------------------------------------
//     AxMenuConfigMailBox::selectFolder()
//-------------------------------------------------------------------------
eOSState AxMenuConfigMailBox::selectFolder()
{
  eOSState aState = osContinue;

  //----- create old temp mailbox
  Ax::Tools::DeleteNull(myTmpMailBox);

  //----- create new temp mailbox
  Mail::MailBoxSetup aSetup;
  convDlg2Setup(aSetup);

  // alwas produce debug output when checking folders
  // aSetup.setClientOptDbg(Mail::CC_DBG_YES);

  myTmpMailBox     = new Mail::MailBox(aSetup);
  mySelectedFolder = myTmpMailBox->getCurrentFolder();

  aState = AddSubMenu(new AxMenuFolders(mySelectedFolder));

  return aState;
} // AxMenuConfigMailBox::selectFolder()

//-------------------------------------------------------------------------
//     AxMenuConfigMailBox::convSetup2Dlg()
//-------------------------------------------------------------------------
void AxMenuConfigMailBox::convSetup2Dlg(const Ax::Mail::MailBoxSetup  &theSetup)
{
  strn0cpy(myAccountName  , theSetup.getAccountName  ().c_str(), sizeof(myAccountName  ));
  strn0cpy(myMailBoxString, theSetup.getMailBoxString().c_str(), sizeof(myMailBoxString));
  strn0cpy(myHostName     , theSetup.getHostName     ().c_str(), sizeof(myHostName     ));
  strn0cpy(myUserName     , theSetup.getUserName     ().c_str(), sizeof(myUserName     ));
  strn0cpy(myPassword     , theSetup.getPassword     ().c_str(), sizeof(myPassword     ));
  strn0cpy(myAccessCode   , theSetup.getAccessCode   ().c_str(), sizeof(myAccessCode   ));
  strn0cpy(myFolder       , theSetup.getFolderPath   ().c_str(), sizeof(myFolder       ));

  myAccountType    = theSetup.getAccountType   ();
  myAccessMode     = theSetup.getAccessMode    ();
  int aBgMode      = theSetup.getBGCheckMode   ();

  myBGCheck        = (aBgMode & Mail::BG_CHK_ON) != 0 ? 1 : 0;
  myBGCheck_ExtApp = (aBgMode & Mail::BG_EXT_ON) != 0 ? 1 : 0;
  myBGCheck_OsdMsg = (aBgMode & Mail::BG_MSG_ON) != 0 ? 1 : 0;
  myBGCheck_Menu   = (aBgMode & Mail::BG_MEN_ON) != 0 ? 1 : 0;
  myBGCheck_SIF    = (aBgMode & Mail::BG_SIF_ON) != 0 ? 1 : 0;

  myClientOptSec   = theSetup.getClientOptSec  ();
  myClientOptRSH   = theSetup.getClientOptRSH  ();
  myClientOptSSL   = theSetup.getClientOptSSL  ();
  myClientOptTLS   = theSetup.getClientOptTLS  ();
  myClientOptCert  = theSetup.getClientOptCert ();
  myClientOptDbg   = theSetup.getClientOptDbg  ();

  myFAutoMarkSeen  = theSetup.getAutoMarkSeen  ();
  myFExpungeOnClose= theSetup.getExpungeOnClose();

} // AxMenuConfigMailBox::convSetup2Dlg()

//-------------------------------------------------------------------------
//     AxMenuConfigMailBox::convDlg2Setup()
//-------------------------------------------------------------------------
void AxMenuConfigMailBox::convDlg2Setup(Ax::Mail::MailBoxSetup  &theSetup)
{
  theSetup.init();

  theSetup.setAccountName     (myAccountName    );
  theSetup.setAccountType     (myAccountType    );

  if (myAccountType == Mail::AT_USER)
  {
    theSetup.setMailBoxString (myMailBoxString  );
  }
  else
  {
    theSetup.setHostName      (myHostName       );
    theSetup.setFolderPath    (myFolder         );

    theSetup.setClientOptSec  (myClientOptSec   );
    theSetup.setClientOptRSH  (myClientOptRSH   );
    theSetup.setClientOptSSL  (myClientOptSSL   );
    theSetup.setClientOptTLS  (myClientOptTLS   );
    theSetup.setClientOptCert (myClientOptCert  );
    theSetup.setClientOptDbg  (myClientOptDbg   );
  } // if

  theSetup.setUserName        (myUserName       );
  theSetup.setPassword        (myPassword       );
  theSetup.setAccessMode      (myAccessMode     );
  theSetup.setAccessCode      (myAccessCode     );

  theSetup.setBGCheckMode     ( (myBGCheck        ? Mail::BG_CHK_ON : Mail::BG_CHK_OFF)
                              | (myBGCheck_ExtApp ? Mail::BG_EXT_ON : Mail::BG_EXT_OFF)
                              | (myBGCheck_OsdMsg ? Mail::BG_MSG_ON : Mail::BG_MSG_OFF)
                              | (myBGCheck_Menu   ? Mail::BG_MEN_ON : Mail::BG_MEN_OFF)
                              | (myBGCheck_SIF    ? Mail::BG_SIF_ON : Mail::BG_SIF_OFF)
                              );

  theSetup.setAutoMarkSeen    (myFAutoMarkSeen  );
  theSetup.setExpungeOnClose  (myFExpungeOnClose);

} // AxMenuConfigMailBox::convSetup2Dlg()

