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
 *   $Date: 2013-03-22 10:13:28 +0100 (Fr, 22 Mär 2013) $
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
#include <getopt.h>

//----- vdr -------------------------------------------------------------------
#include <vdr/plugin.h>
#include <vdr/interface.h>

//----- AxLib -----------------------------------------------------------------
#include <Ax/Tools/Trace.h>
// #include <Ax/Tools/TraceOn.h>
#include <Ax/Tools/Globals.h>
#include <Ax/Mail/MailBoxMgr.h>
#include <Ax/Mail/MailBox.h>
#include <Ax/Mail/MailBoxSetup.h>

//----- local -----------------------------------------------------------------
#include "i18n.h"
#include "AxPluginMailBox.h"
#include "AxMenuConfigPlugin.h"
#include "AxMenuMailBoxView.h"
#include "AxMenuMailListView.h"
#include "AxPluginSetup.h"
#include "AxVdrTools.h"
#include "AxMailBoxServiceTypes.h"

//----- local -----------------------------------------------------------------
//----- c-client --------------------------------------------------------------

//=============================================================================
//     VDRPLUGINCREATOR
//=============================================================================
/// Macro for plugin entry point
VDRPLUGINCREATOR(AxPluginMailBox);

//=============================================================================
//     using namespaces
//=============================================================================
using namespace Ax;

//=============================================================================
//     defines
//=============================================================================
static const char *VERSION        = "0.7.1";
static const char *DESCRIPTION    = trNOOP("Display emails of IMAP/POP3 accounts");

// Dummy strings as placeholders for strings from AxVDR
//
// AxLib/inlucde/Ax/VDR/MenuHelpKeys.cpp
static const char *Dummy_01 = trNOOP("Key mapping");
static const char *Dummy_02 = trNOOP("Activate");
static const char *Dummy_03 = trNOOP("Activate the selected action");
static const char *Dummy_04 = trNOOP("Ignore this key");
static const char *Dummy_05 = trNOOP("Drop changes & go back");
static const char *Dummy_06 = trNOOP("Show help for usable keys");

// AxLib/src/Ax/VDR/MenuHelpCode.cpp
static const char *Dummy_07 = trNOOP("MenuCode$Restricted area");
static const char *Dummy_08 = trNOOP("MenuCode$Access code");
static const char *Dummy_09 = trNOOP("MenuCode$Code invalid!");

// ???
static const char *Dummy_10 = trNOOP("Log c-client-callbacks");


//=============================================================================
//     globals
//=============================================================================

//=============================================================================
//     class PluginMail
//=============================================================================

//-----------------------------------------------------------------------------
//     PluginMail::AxPluginMailBox()
//-----------------------------------------------------------------------------
AxPluginMailBox::AxPluginMailBox()
               : mySetup         ()
               , myMailChecker   (this)
               , myInitialMailBox(0)
               , myTotalNewMails (0)
               , myMainMenuEntryAppendix("")
{
#if HAVE_AXLIB
  Ax::Tools::Trace::setLogFunc(syslog_with_tid);
#endif
  AX_UNUSED_ARG(Dummy_01);
  AX_UNUSED_ARG(Dummy_02);
  AX_UNUSED_ARG(Dummy_03);
  AX_UNUSED_ARG(Dummy_04);
  AX_UNUSED_ARG(Dummy_05);
  AX_UNUSED_ARG(Dummy_06);
  AX_UNUSED_ARG(Dummy_07);
  AX_UNUSED_ARG(Dummy_08);
  AX_UNUSED_ARG(Dummy_09);
  AX_UNUSED_ARG(Dummy_10);
  wsdebug(("> AxPluginMailBox::AxPluginMailBox()"));

  // Initialize any member varaiables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!

  memset(myMainMenuEntry, 0, MAIN_MENU_ENTRY_LEN);

  wsdebug(("< AxPluginMailBox::AxPluginMailBox()"));
} // AxPluginMailBox::AxPluginMailBox()

//-----------------------------------------------------------------------------
//     AxPluginMailBox::~AxPluginMailBox()
//-----------------------------------------------------------------------------
AxPluginMailBox::~AxPluginMailBox()
{
  wsdebug(("> AxPluginMailBox::~AxPluginMailBox()"));

  if (myMailChecker.Active())
  {
    wserror(("AxPluginMailBox::~AxPluginMailBox() mail-checker-thread still active, this should not happen!"));
    myMailChecker.stopChecking();
  } // if

  wsdebug(("  AxPluginMailBox::~AxPluginMailBox() terminating MailBoxMgr..."));
  Mail::MailBoxMgr::terminate();

  wsdebug(("  AxPluginMailBox::~AxPluginMailBox() cleaning up mySetup..."));
  mySetup.clear();

  wsdebug(("< AxPluginMailBox::~AxPluginMailBox()"));
} // AxPluginMailBox::~AxPluginMailBox()

//-----------------------------------------------------------------------------
//     AxPluginMailBox::MainMenuEntry()
//-----------------------------------------------------------------------------
const char *AxPluginMailBox::MainMenuEntry(void)
{
  copyMainMenuEntry();

  return myMainMenuEntry;
} // AxPluginMailBox::MainMenuEntry()

//-----------------------------------------------------------------------------
//     AxPluginMailBox::Version()
//-----------------------------------------------------------------------------
const char *AxPluginMailBox::Version(void)
{
  return VERSION;
} // AxPluginMailBox::Version()

//-----------------------------------------------------------------------------
//     AxPluginMailBox::Description()
//-----------------------------------------------------------------------------
const char *AxPluginMailBox::Description(void)
{
  return tr(DESCRIPTION);
} // AxPluginMailBox::Description()

//-----------------------------------------------------------------------------
//     AxPluginMailBox::CommandLineHelp()
//-----------------------------------------------------------------------------
const char *AxPluginMailBox::CommandLineHelp(void)
{
  wsdebug(("  AxPluginMailBox::CommandLineHelp()"));

  return "  -m CMD, --mailcmd=CMD   CMD is called when the number of new mails changed\n"
         "  -c CMD, --convcmd=CMD   CMD is called to convert mail-parts from html to text\n"
         "  -i MAX, --interval=MAX  MAX the maximum interval for the background check (minutes)\n"
         "  -d LVL, --debug=LVL     LVL is a debug value (numeric value)\n"
         ;
} // AxPluginMailBox::CommandLineHelp()

//-----------------------------------------------------------------------------
//     AxPluginMailBox::ProcessArgs()
//-----------------------------------------------------------------------------
bool AxPluginMailBox::ProcessArgs(int argc, char *argv[])
{
  static struct option long_options[] =
  {
    { "mailcmd",      required_argument, NULL, 'm' },
    { "convcmd",      required_argument, NULL, 'c' },
    { "interval",     required_argument, NULL, 'i' },
    { "debug"  ,      required_argument, NULL, 'd' },
    { NULL }
  };

  int c;
  while ((c = getopt_long(argc, argv, "m:c:i:d:", long_options, NULL)) != -1)
  {
    switch (c)
    {
      case 'm':
      {
        mySetup.Settings.MailInfoCmd = std::string(optarg);
        break;
      }
      case 'c':
      {
        mySetup.Settings.MailConvCmd = std::string(optarg);
        break;
      }
      case 'i':
      {
        mySetup.Settings.BGCheckDelayMax = atoi(optarg);
        break;
      }
      case 'd':
      {
        long aDebugFlag = atoi(optarg);
        wsinfo(("mailbox-plugin: Debug-Value: %08lX", aDebugFlag));
        if ((aDebugFlag & 0x01) != 0) Ax::Vdr::Thread::setDebugFlag(true);
        break;
      }
      default:
      {
        return false;
      }
    } // switch
  } // while

  wsdebug(("  AxPluginMailBox::ProcessArgs() MaxBGCheckDelay: %d", mySetup.Settings.BGCheckDelayMax ));
  wsdebug(("  AxPluginMailBox::ProcessArgs() MailInfoCmd: '%s'", mySetup.Settings.MailInfoCmd.c_str()));
  wsdebug(("< AxPluginMailBox::ProcessArgs() MailConvCmd: '%s'", mySetup.Settings.MailConvCmd.c_str()));
  return true;
} // AxPluginMailBox::ProcessArgs()

//-----------------------------------------------------------------------------
//     AxPluginMailBox::Start()
//-----------------------------------------------------------------------------
bool AxPluginMailBox::Start(void)
{
  wsdebug(("> AxPluginMailBox::Start()"));
  // Start any background activities the plugin shall perform.

  Ax::Mail::MailBoxMgr::instance();

#if APIVERSNUM < 10507
  // register i18n phrases
  RegisterI18n(MailboxPhrases);
#endif

  setMainMenuAppendix(); // i18n now initialized

  // load the account definitions
  if (!mySetup.loadAccounts(ConfigDirectory(Name())))
  {
    // error message already in loadAccounts()
  } // if

  createMailBoxes();
  Ax::Mail::MailBoxMgr::instance()->setTimeOut(getSetup().Settings.ConnTimeOut );

  if (!myMailChecker.startChecking())
  {
    // error message already in startChecking()
  } // if

  wsdebug(("< AxPluginMailBox::Start()"));
  return true;
} // AxPluginMailBox::Start()

//-------------------------------------------------------------------------
//     AxPluginMailBox::Stop()
//-------------------------------------------------------------------------
void AxPluginMailBox::Stop(void)
{
  wsdebug(("> AxPluginMailBox::Stop()"));

  wsdebug(("  AxPluginMailBox::Stop() terminating mail-checker-thread..."));
  myMailChecker.stopChecking();

  wsdebug(("< AxPluginMailBox::Stop()"));
} // AxPluginMailBox::Stop()

//-----------------------------------------------------------------------------
//     AxPluginMailBox::Housekeeping()
//-----------------------------------------------------------------------------
void AxPluginMailBox::Housekeeping(void)
{
//  wsdebug(("> AxPluginMailBox::Housekeeping()"));
//  wsdebug(("< AxPluginMailBox::Housekeeping()"));
} // AxPluginMailBox::Housekeeping()

//-----------------------------------------------------------------------------
//     AxPluginMailBox::MainMenuAction()
//-----------------------------------------------------------------------------
cOsdMenu *AxPluginMailBox::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  wsdebug(("> AxPluginMailBox::MainMenueAction()"));

  // restore MainMenuEntry
  // (unfortunately this doesn't work as vdr doesn't call
  //  MainMenuEntry() again when the osd-menu of a plugin
  //  is closed)
  setMainMenuAppendix();
  copyMainMenuEntry();

  cOsdMenu *anOsdMenu = 0;

  if (mySetup.Accounts.size() == 0)
  {
    wswarn(("AxPluginMailBox::MainMenuAction() No mail-accounts defined!"));
    Skins.Message(mtError, tr("No mail accounts defined!"));
  }
  else
  {
    // stop background processing
    AxMailChecker::MCState aState = myMailChecker.Active() ? myMailChecker.getState() : AxMailChecker::MC_STOP;
    wsdebug(("  MailBox::MainMenuAction() State: %d", aState));
    
    bool fOsdMsg = aState == AxMailChecker::MC_QUERY || aState == AxMailChecker::MC_ASK;
    if (fOsdMsg)
    {
      // mtStatus geht hier nicht!
      wsdebug(("  MailBox::MainMenuAction() OSD-Message start..."));
      Skins.Message(mtStatus, tr("Stopping background processing..."));
      wsdebug(("  MailBox::MainMenuAction() OSD-Message start...done"));
    } // if
    myMailChecker.stopChecking();
    if (fOsdMsg)
    {
      wsdebug(("  MailBox::MainMenuAction() OSD-Message clear..."));
      Skins.Message(mtStatus, 0);
      wsdebug(("  MailBox::MainMenuAction() OSD-Message clear...done"));
    } // if

    // initialize (translate) some strings in AxVdrTools
    AxVdrTools::initialize();

    Ax::Mail::MailBox *aMailBox = getInitialMailBox();

    if (aMailBox)
    {
      wsdebug(("  AxPluginMailBox::MainMenueAction() opening %s...", aMailBox->getAccountName().c_str()));
      anOsdMenu = new AxMenuMailListView(this, aMailBox->getCurrentFolder(), true);
      wsdebug(("  AxPluginMailBox::MainMenueAction() opening %s...done", aMailBox->getAccountName().c_str()));
    }
    else
    {
      wsdebug(("  AxPluginMailBox::MainMenueAction() opening MailBoxView..."));
      anOsdMenu = new AxMenuMailBoxView(this);
      wsdebug(("  AxPluginMailBox::MainMenueAction() opening MailBoxView...done"));
    } // if
  } // if

  wsdebug(("< AxPluginMailBox::MainMenueAction()"));

  return anOsdMenu;
} // AxPluginMailBox::MainMenuAction()

//-----------------------------------------------------------------------------
//     AxPluginMailBox::SetupMenu()
//-----------------------------------------------------------------------------
cMenuSetupPage *AxPluginMailBox::SetupMenu(void)
{
  wsdebug(("> AxPluginMailBox::SetupMenu()"));
  myMailChecker.stopChecking();

  // restore MainMenuEntry
  // (unfortunately this doesn't work as vdr doesn't call
  //  MainMenuEntry() again when the osd-menu of a plugin
  //  is closed)
  setMainMenuAppendix();
  copyMainMenuEntry();

  wsdebug(("  AxPluginMailBox::SetupMenu() destroying old mailboxes..."));
  destroyMailBoxes();

  // initialize (translate) some strings in AxVdrTools
  AxVdrTools::initialize();

  AxMenuConfigPlugin *aSetupPage = new AxMenuConfigPlugin(this);

  wsdebug(("< AxPluginMailBox::SetupMenu()"));
  return aSetupPage;
} // AxPluginMailBox::SetupMenu()

//-------------------------------------------------------------------------
//     AxPluginMailBox::MainMenuActionFinished()
//-------------------------------------------------------------------------
void AxPluginMailBox::MainMenuActionFinished()
{
  wsdebug(("> AxPluginMailBox::MainMenuActionFinished()"));

  if (!myMailChecker.startChecking())
  {
    // error message already in startChecking()
  } // if

  wsdebug(("< AxPluginMailBox::MainMenuActionFinished()"));
} // AxPluginMailBox::MainMenuActionFinished(void)

//-------------------------------------------------------------------------
//     AxPluginMailBox::SetupMenuActionFinished()
//-------------------------------------------------------------------------
void AxPluginMailBox::SetupMenuActionFinished()
{
  wsdebug(("> AxPluginMailBox::SetupMenuActionFinished()"));

  createMailBoxes();

  if (!myMailChecker.startChecking())
  {
    // error message already in startChecking()
  } // if

  wsdebug(("< AxPluginMailBox::SetupMenuActionFinished()"));
} // AxPluginMailBox::SetupMenuActionFinished()

//-------------------------------------------------------------------------
//     AxPluginMailBox::Service()
//-------------------------------------------------------------------------
bool AxPluginMailBox::Service(const char *Id, void *Data)
{
  bool fHandled = true;

  if (strcmp(Id, MailBox_HasNewMail_v1_0_NAME) == 0)
  {
    //---- MailBox_HasNewMail_v1_0_NAME -----------------------------------
    if (Data != 0)
    {
      bool *fHasNewMail = (bool *)Data;
      {
        cMutexLock aLock(&myMainMenuEntryAppendixMutex);
        *fHasNewMail = (myTotalNewMails != 0);
      }
      wsdebug(("- AxPluginMailBox::Service() Id: '%s' NumUnseen: %lu", Id, myTotalNewMails));
    }
    else
    {
      wsdebug(("- AxPluginMailBox::Service() Id: '%s' supported", Id));
    } // if
  }
  else if (strcmp(Id, MailBox_GetTotalUnseen_v1_0_NAME) == 0)
  {
    //---- MailBox_GetTotalUnseen_v1_0_NAME -------------------------------
    if (Data != 0)
    {
      unsigned long *aTotalUnseen = (unsigned long *)Data;
      {
        cMutexLock aLock(&myMainMenuEntryAppendixMutex);
        *aTotalUnseen = myTotalNewMails;
      }
      wsdebug(("- AxPluginMailBox::Service() Id: '%s' NumUnseen: %lu", Id, myTotalNewMails));
    }
    else
    {
      wsdebug(("- AxPluginMailBox::Service() Id: '%s' supported", Id));
    } // if
  }
  else
  {
    fHandled = false;
  } // if

  return fHandled;
} // AxPluginMailBox::Service()

#if 0
//-------------------------------------------------------------------------
//     AxPluginMailBox::MainThreadHook()
//-------------------------------------------------------------------------
void AxPluginMailBox::MainThreadHook(void)
{
} // AxPluginMailBox::MainThreadHook()

//-------------------------------------------------------------------------
//     AxPluginMailBox::Active()
//-------------------------------------------------------------------------
cString AxPluginMailBox::Active(void)
{
  return NULL;
} // AxPluginMailBox::Active()
#endif

//-------------------------------------------------------------------------
//     AxPluginMailBox::setInitialMailBox()
//-------------------------------------------------------------------------
void AxPluginMailBox::setInitialMailBox(Ax::Mail::MailBox *theMailBox)
{
  cMutexLock aLock(&myInitialMailBoxMutex);
  myInitialMailBox = theMailBox;
} // AxPluginMailBox::setInitialMailBox()

//-------------------------------------------------------------------------
//     AxPluginMailBox::getInitialMailBox()
//-------------------------------------------------------------------------
Ax::Mail::MailBox *AxPluginMailBox::getInitialMailBox()
{
  Ax::Mail::MailBox *aMailBox = 0;
  {
    cMutexLock aLock(&myInitialMailBoxMutex);
    aMailBox = myInitialMailBox;
    myInitialMailBox = 0;
  }
  return aMailBox;
} // AxPluginMailBox::getInitialMailBox()

//-------------------------------------------------------------------------
//     AxPluginMailBox::createMailBoxes()
//-------------------------------------------------------------------------
void AxPluginMailBox::createMailBoxes()
{
  wsdebug(("> AxPluginMailBox::createMailBoxes()"));

  wsdebug(("  AxPluginMailBox::createMailBoxes() destroying old mailboxes..."));
  destroyMailBoxes();

  wsdebug(("  AxPluginMailBox::createMailBoxes() creating mailboxes..."));
  for (Mail::MailBoxSetupCltn::const_iterator anIter  = mySetup.Accounts.begin();
                                              anIter != mySetup.Accounts.end();
                                            ++anIter)
  {
    const Mail::MailBoxSetup *aMailBoxSetup = *anIter;

    wsdebug(("  AxPluginMailBox::createMailBoxes() creating MailBox: '%s' '%s'..."
           , aMailBoxSetup->getAccountName().c_str()
           , aMailBoxSetup->getMailBoxString().c_str()
           ));

    Mail::MailBox *aMailBox = new Mail::MailBox(*aMailBoxSetup);

    wsdebug(("  AxPluginMailBox::createMailBoxes() adding to collection..."));
    myMailBoxCltn.push_back(aMailBox);
  } // for

  wsdebug(("< AxPluginMailBox::createMailBoxes()"));
} // AxPluginMailBox::createMailBoxes()

//-------------------------------------------------------------------------
//     AxPluginMailBox::destroyMailBoxes()
//-------------------------------------------------------------------------
void AxPluginMailBox::destroyMailBoxes()
{
  wsdebug(("> AxPluginMailBox::destroyMailBoxes() Count: %d", myMailBoxCltn.size()));

  while (myMailBoxCltn.size() > 0)
  {
    Mail::MailBox *aMailBox = *myMailBoxCltn.begin();
    myMailBoxCltn.pop_front();
    Ax::Tools::DeleteNull(aMailBox);
  } // while

  wsdebug(("< AxPluginMailBox::destroyMailBoxes() Count: %d", myMailBoxCltn.size()));
} // AxPluginMailBox::destroyMailBoxes()

//-----------------------------------------------------------------------------
//     AxPluginMailBox::SetupParse()
//-----------------------------------------------------------------------------
bool AxPluginMailBox::SetupParse(const char *theName, const char *theValue)
{
  // Parse your own setup parameters and store their values.
  wsdebug(("> AxPluginMailBox::SetupParse() Name: '%s'  Value: '%s'"
         , theName
         , theValue ? theValue : "(NULL)"
         ));

  bool fConsumed = mySetup.Settings.parseSetup(theName, theValue);

  wsdebug(("< AxPluginMailBox::SetupParse() Name: '%s'  Value: '%s'"
         , theName
         , theValue ? theValue : "(NULL)"
         ));

  return fConsumed;
} // AxPluginMailBox::SetupParse()

//-------------------------------------------------------------------------
//     AxPluginMailBox::setMainMenuAppendix()
//-------------------------------------------------------------------------
void AxPluginMailBox::setMainMenuAppendix(unsigned long theTotalNewMails, const std::string &theMenuText)
{
  // lock myMainMenuEntryBuffer
  cMutexLock aLock(&myMainMenuEntryAppendixMutex);
  myTotalNewMails         = theTotalNewMails;
  myMainMenuEntryAppendix = theMenuText;
  // unlock myMainMenuEntryBuffer
} // AxPluginMailBox::setMainMenuAppendix()

//-------------------------------------------------------------------------
//     AxPluginMailBox::copyMainMenuEntry()
//-------------------------------------------------------------------------
void AxPluginMailBox::copyMainMenuEntry()
{
  std::string aMenuText = std::string(tr("Mailbox"));

  {
    // lock myMainMenuEntryBuffer
    cMutexLock aLock(&myMainMenuEntryAppendixMutex);

    if (!myMainMenuEntryAppendix.empty())
    {
      aMenuText = aMenuText
                + std::string(" (")
                + myMainMenuEntryAppendix
                + std::string(" ")
                + std::string(tr("MainMenuEntry$new"))
                + std::string(")")
                ;
    } // if
  }

  strn0cpy(myMainMenuEntry, aMenuText.c_str(), MAIN_MENU_ENTRY_LEN);
  // unlock myMainMenuEntryBuffer
} // AxPluginMailBox::copyMainMenuEntry()
