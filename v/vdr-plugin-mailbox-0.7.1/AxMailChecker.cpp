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
//----- C ---------------------------------------------------------------------
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//----- vdr -------------------------------------------------------------------
#include <vdr/plugin.h>
#include <vdr/skins.h>
#include <vdr/remote.h>

//----- AxLib -----------------------------------------------------------------
#include <Ax/Tools/Macros.h>
#include <Ax/Tools/Trace.h>
// #include <Ax/Tools/TraceOn.h>
#include <Ax/Tools/Globals.h>
#include <Ax/Tools/String.h>
#include <Ax/Mail/MailFolder.h>
#include <Ax/Mail/MailBoxMgr.h>
#include <Ax/Mail/MailBox.h>
#include <Ax/Mail/MailBoxSetup.h>

//----- local -----------------------------------------------------------------
#include "AxMailChecker.h"
#include "AxPluginSetup.h"
#include "AxPluginMailBox.h"

//----- locals ----------------------------------------------------------------

//=============================================================================
//     using namespaces
//=============================================================================
using namespace std;
using namespace Ax;

//=============================================================================
//     consts
//=============================================================================

//=============================================================================
//     class AxMailChecker
//=============================================================================

//-------------------------------------------------------------------------
//     AxMailChecker::AxMailChecker()
//-------------------------------------------------------------------------
AxMailChecker::AxMailChecker(AxPluginMailBox *thePlugin)
             : PARENT("mail-checker")
             , myPlugin        (thePlugin)
             , myOSDMessageTime(3) // to have an initial value
             , myPluginSettings(thePlugin->getSetup().Settings)
             , myState         (MC_STOP)
{
  wsdebug(("> AxMailChecker::AxMailChecker()"));
  wsdebug(("< AxMailChecker::AxMailChecker()"));
} // AxMailChecker::AxMailChecker()

//-------------------------------------------------------------------------
//     AxMailChecker::~AxMailChecker()
//-------------------------------------------------------------------------
AxMailChecker::~AxMailChecker()
{
  wsdebug(("> AxMailChecker::~AxMailChecker()"));
  wsdebug(("< AxMailChecker::~AxMailChecker()"));
} // AxMailChecker::~AxMailChecker()

//-------------------------------------------------------------------------
//     AxMailChecker::getState()
//-------------------------------------------------------------------------
AxMailChecker::MCState AxMailChecker::getState()
{
  MCState aState = MC_STOP;
  {
    cMutexLock aLock(&myStateMutex);
    aState = myState;
  }
  return aState;
} // AxMailChecker::getState()

//-------------------------------------------------------------------------
//     AxMailChecker::setState()
//-------------------------------------------------------------------------
void AxMailChecker::setState(AxMailChecker::MCState theState)
{
  cMutexLock aLock(&myStateMutex);
  myState = theState;
} // AxMailChecker::setState()

//-------------------------------------------------------------------------
//     AxMailChecker::startChecking()
//-------------------------------------------------------------------------
bool AxMailChecker::startChecking()
{
  bool fOK = false;

  if (isActive())
  {
    wswarn(("AxMailChecker::startChecking() thread is already executing!"));
    fOK = true;
  }
  else if (myPluginSettings.BGCheckDelay <= 0)
  {
    wsinfo(("AxMailChecker::startChecking() BGCheckDelay == 0 -> thread not started!"));
  }
  else if (myPlugin->getMailBoxCltn().size() == 0)
  {
    wsinfo(("AxMailChecker::startChecking() no mail-accounts defined -> thread not started!"));
  }
  else
  {
    myOSDMessageTime = Setup.OSDMessageTime; // use the settings of vdr
    fOK = start();
    if (!fOK)
    {
      wserror(("unable to start mail-checker-thread!"));
    } // if
  } // if
  return fOK;
} // AxMailChecker::startChecking()

//-------------------------------------------------------------------------
//     AxMailChecker::stopChecking()
//-------------------------------------------------------------------------
void AxMailChecker::stopChecking()
{
  if (isActive())
  {
    wsdebug(("> AxMailChecker::stopChecking() Active: %d", int(isActive())));

    setStopRequested();

    { // request the lock
      // if currently a mailbox is checked, this will block until finished
      // so this should block until the thread has finished querying
      // the current account and is asking whether to stop or continue
      wsdebug(("  AxMailChecker::stopChecking() waiting until thread isn't busy..."));
      cMutexLock aLock(&myCheckMutex);
      wsdebug(("  AxMailChecker::stopChecking() waiting until thread isn't busy...done"));
    } // release the lock

    // wait at most as long as vdr displays a message
    // before killing the thread
    int aWaitSecs = Setup.OSDMessageTime + 1;
    wsdebug(("  AxMailChecker::stopChecking() calling stop(%d)...", aWaitSecs));
    stop(aWaitSecs);
    wsdebug(("  AxMailChecker::stopChecking() calling stop(%d)...done", aWaitSecs));

    setState(MC_STOP);

    wsdebug(("< AxMailChecker::stopChecking() Active: %d", int(isActive())));
  }
  else
  {
    wsdebug(("- AxMailChecker::stopChecking() mail-checker-thread is not active!"));
  } // if
} // AxMailChecker::stopChecking()

//-------------------------------------------------------------------------
//     AxMailChecker::Action()
//-------------------------------------------------------------------------
void AxMailChecker::Action()
{
  bool fFirst     = true;         // is this the first check
  int  aTimeOutMS = 30000;

  wsdebug(("%s: myCheckDelay: %d -> next check in %d secs"
        , getName().c_str()
        , myPluginSettings.BGCheckDelay
        , aTimeOutMS / 1000
        ));

  myAskAgain.clear();

  setState(MC_SLEEP);

  // ensure there's no old event pending
  waitEvent(1);

  //----- loop until aborted -----
  while (waitEvent(aTimeOutMS))
  {
    std::string   anAppendix("");
    unsigned long aSumUnread = 0;
    unsigned long aSumSIF    = 0;
    bool          fAbort     = false;

    //----- iterate over Mail-Accounts -----
    for (Mail::MailBoxList::const_iterator anIter  = myPlugin->getMailBoxCltn().begin();
                       !stopRequested() && anIter != myPlugin->getMailBoxCltn().end();
                                         ++anIter)
    {
      Mail::MailBox *aMB = *anIter;

      if ((aMB->getSetup().getBGCheckMode() & Mail::BG_CHK_MASK) == Mail::BG_CHK_ON)
      {
        if (checkMailBox(aMB->getCurrentFolder(), fFirst, fAbort))
        {
          if ((aMB->getSetup().getBGCheckMode() & Mail::BG_MEN_MASK) == Mail::BG_MEN_ON)
          {
            aSumUnread += aMB->getCurrentFolder()->getCountUnseen();

            if (!anAppendix.empty())
            {
              anAppendix = anAppendix + std::string("/");
            } // if
            anAppendix = anAppendix + std::string(itoa(aMB->getCurrentFolder()->getCountUnseen()));
          } // if

          if ((aMB->getSetup().getBGCheckMode() & Mail::BG_SIF_MASK) == Mail::BG_SIF_ON)
          {
            aSumSIF += aMB->getCurrentFolder()->getCountUnseen();
          } // if
        } // if

        if (fAbort)
        {
          wsinfo(("%s: stop requested within loop to activate main menu -> exiting", getName().c_str()));
          break;
        } // if

        // wait for a short time without lock
        if (!stopRequested()) usleep(10000);
      } // if
    } // for

    if (fAbort || stopRequested())
    {
      wsinfo(("%s: stop requested within loop -> exiting", getName().c_str()));
      break;
    } // if

    // show appendix to main-menu-entry only if there's at least one unread mail
    if (aSumUnread == 0)
    {
      anAppendix = "";
    } // if

    myPlugin->setMainMenuAppendix(aSumSIF, anAppendix);

    fFirst     = false;
    aTimeOutMS = myPluginSettings.BGCheckDelay * 60 * 1000;

    wsdebug(("%s: myCheckDelay: %d -> next check in %d secs"
          , getName().c_str()
          , myPluginSettings.BGCheckDelay
          , aTimeOutMS / 1000
          ));
  } // while

  // remove all osd-messages of this thread
  Skins.QueueMessage(mtInfo, 0);

  setState(MC_STOP);

} // AxMailChecker::Action()

//-------------------------------------------------------------------------
//     AxMailChecker::checkMailBox()
//-------------------------------------------------------------------------
bool AxMailChecker::checkMailBox(Mail::MailFolder *theFolder, bool fForceStartCmd, bool &fAbortThread)
{
  fAbortThread = false;

  wsdebug(("%s: > checkMailBox() fForceStartCmd: %d", getName().c_str(), fForceStartCmd));
  wsdebug(("%s: checking for new mail in %s..."
         , getName().c_str()
         , theFolder->getMailBox()->getAccountName().c_str()
         ));

  setState(MC_QUERY);

  //----- set the lock and request the status ---------------------------------
  cMutexLock aLock(&myCheckMutex);

  unsigned long aCountUnseen = theFolder->getCountUnseen();
  unsigned long aCountMails  = theFolder->getCountMails ();
  unsigned long aUIDNext     = theFolder->getUIDNext    ();
  AX_UNUSED_ARG(aUIDNext);

  wsdebug(("%s: checking for new mail in %s before: total: %lu, unseen: %lu  UIDNext: %lu"
         , getName().c_str()
         , theFolder->getMailBox()->getAccountName().c_str()
         , theFolder->getCountMails()
         , theFolder->getCountUnseen()
         , theFolder->getUIDNext()
         ));

  bool fOK = theFolder->requestStatus();

  if (fOK)
  {
    wsdebug(("%s: checking for new mail in %s...done: total: %lu, unseen: %lu  UIDNext: %lu"
           , getName().c_str()
           , theFolder->getMailBox()->getAccountName().c_str()
           , theFolder->getCountMails()
           , theFolder->getCountUnseen()
           , theFolder->getUIDNext()
           ));
  }
  else
  {
    wsdebug(("%s: checking for new mail in %s...FAILED!"
           , getName().c_str()
           , theFolder->getMailBox()->getAccountName().c_str()
           ));
  } // if

  if (fOK && !stopRequested())
  {
    //----- start external command --------------------------------------------
    if ( !myPluginSettings.MailInfoCmd.empty()
      && (aCountUnseen != theFolder->getCountUnseen() || fForceStartCmd)
      && ((theFolder->getMailBox()->getSetup().getBGCheckMode() & Mail::BG_EXT_MASK) == Mail::BG_EXT_ON)
       )
    {
      std::string aCmd = Ax::Tools::String::sprintf( "%s \"%s\" \"%s\" dummy dummy %ld %ld %ld %ld"
                                                   , myPluginSettings.MailInfoCmd.c_str()
                                                   , theFolder->getMailBox()->getAccountName().c_str()
                                                   , theFolder->getMailBox()->getSetup().getUserName().c_str()
                                                   , theFolder->getCountUnseen()
                                                   , theFolder->getCountMails()
                                                   , aCountUnseen
                                                   , aCountMails
                                                   );

      wsdebug(("%s: executing '%s'", getName().c_str(), aCmd.c_str()));
      int aRet = SystemExec(aCmd.c_str());
      if (aRet != 0)
      {
        wserror(("%s: unable to start '%s' (ret: %d)", getName().c_str(), aCmd.c_str(), aRet));
      }
      else
      {
        wsinfo(("%s: Cmd: '%s' Ret: %d", getName().c_str(), aCmd.c_str(), aRet));
      } // if
    } // if

    //----- display message on OSD --------------------------------------------
    if ((theFolder->getMailBox()->getSetup().getBGCheckMode() & Mail::BG_MSG_MASK) == Mail::BG_MSG_ON)
    {
#if 0
      std::string aMsg;
      if (theFolder->getCountUnseen() == 1)
      {
        aMsg = Ax::Tools::String::sprintf( tr("New mail in %s")
                                        , theFolder->getMailBox()->getAccountName().c_str()
                                        );
      }
      else
      {
        aMsg = Ax::Tools::String::sprintf( tr("%ld new mails in %s")
                                        , theFolder->getCountUnseen()
                                        , theFolder->getMailBox()->getAccountName().c_str()
                                        );
      } // if

      // note: The 'asynchronous call to QueueMessage() with timeout
      //       doesn't deliver a state if the message was really displayed.
      //
      Skins.QueueMessage(mtInfo, aMsg.c_str());
#else
      bool fAskAgain = myAskAgain[theFolder];

      if (  theFolder->getCountUnseen() > 0
        && (theFolder->getCountUnseen() > aCountUnseen || fAskAgain)
         )
      {
        setState(MC_ASK);

        std::string aMsg;
        if (theFolder->getCountUnseen() == 1)
        {
          aMsg = Ax::Tools::String::sprintf( tr("New mail in %s, open?")
                                          , theFolder->getMailBox()->getAccountName().c_str()
                                          );
        }
        else
        {
          aMsg = Ax::Tools::String::sprintf( tr("%ld new mails in %s, open?")
                                          , theFolder->getCountUnseen()
                                          , theFolder->getMailBox()->getAccountName().c_str()
                                          );
        } // if

        // note: the 'synchronous' call to QueueMessage() whith timout
        //       is used here to get exact knowledge if the message
        //       was displayed and to receive if the user pressed a key.
        //
        //       max-time-for the call is the same as configured in VDR's Setup.OSDMessageTime
        //
        wsdebug(("%s: Skins.QueueMessage()...", getName().c_str()));
        int aKey = Skins.QueueMessage(mtInfo, aMsg.c_str(), myOSDMessageTime, myOSDMessageTime + 2);
        wsdebug(("%s: Skins.QueueMessage()...returned %d", getName().c_str(), aKey));

        fAskAgain = false;
        switch (aKey)
        {
          case -1:
          {
            wsdebug(("%s: -1 = nothing displayed"
                  , getName().c_str()
                  ));
            // the message wasn't displayed within the timeout
            // so we assume there was another OSD visible and
            // we try to show the message later again
            fAskAgain = true;
            break;
          }
          case kNone:
          {
            wsdebug(("%s: kNone pressed", getName().c_str()));
            // the message was displayed but the user didn't press a key
            // so we assume the user doesn't want be be disturbed by
            // displaying the message again
            break;
          }
          case kOk:
          {
            wsdebug(("%s: kOk pressed, calling plugin...", getName().c_str()));
            // the message was displayed and the user pressed OK
            // so we abort this thread, store the current folder
            // and activate the plugin (which displays the current folder)
            if (theFolder->getMailBox()->getSetup().getAccessMode() != Ax::Mail::AM_SETUP_READ)
            {
              fAbortThread = true;
              myPlugin->setInitialMailBox(theFolder->getMailBox());
              cRemote::CallPlugin("mailbox");
            } // if
            break;
          }
          case kBack:
          {
            wsdebug(("%s: kBack pressed", getName().c_str()));
            // the message was displayed and the user pressed kBack
            // so the message is considered as 'processed'
            break;
          }
          default:
          {
            wsdebug(("%s: unknown return code: %d", getName().c_str(), aKey));
            // unknown state -> consider the message as 'processed'
            break;
          }
        } // switch
      } // if
      myAskAgain[theFolder] = fAskAgain;
#endif
    } // if
  } // if

  setState(MC_SLEEP);

  wsdebug(("%s: < checkMailBox() fAbortThread: %d", getName().c_str(), fAbortThread));
  return fOK;
} // AxMailChecker::checkMailBox()
