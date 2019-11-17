#ifndef __AxMailChecker_H__
#define __AxMailChecker_H__
//=============================================================================
/*
 * See the file README in the main directory for a description of
 * this software, copyright information, and how to reach the author.
 *
 * Author: alex
 * Date:   26.12.2003
 *
 * Last modfied:
 *   $Author: alex $
 *   $Date: 2013-03-20 20:33:18 +0100 (Mi, 20 Mär 2013) $
 */
//=============================================================================

//=============================================================================
//     includes
//=============================================================================

//----- qt --------------------------------------------------------------------
//----- CORBA -----------------------------------------------------------------
//----- C++ -------------------------------------------------------------------
#include <map>

//----- C ---------------------------------------------------------------------
//----- AxLib -----------------------------------------------------------------
#include <Ax/Mail/Globals.h>
#include <Ax/Vdr/Thread.h>

//----- local -----------------------------------------------------------------
//----- vdr -------------------------------------------------------------------

//----- local -----------------------------------------------------------------

//=============================================================================
//     forward declarations
//=============================================================================
class AxPluginSettings;
namespace Ax   {
namespace Mail {
  class MailBox;
}
}

//=============================================================================
//     forward declarations
//=============================================================================
class AxPluginMailBox;

//=============================================================================
//     class AxMailChecker
//=============================================================================
class AxMailChecker : public Ax::Vdr::Thread
{
    //-------------------------------------------------------------------------
    //     typedefs
    //-------------------------------------------------------------------------
    typedef Ax::Vdr::Thread PARENT;

  public:
    //-------------------------------------------------------------------------
    //     enum
    //-------------------------------------------------------------------------
    enum MCState
    {
      MC_STOP   ///< thread not active
    , MC_SLEEP  ///< sleeping between query-cycles
    , MC_QUERY  ///< requesting status of Mailbox
    , MC_ASK    ///< asking/showing status-message
    };

    //-------------------------------------------------------------------------
    //     AxMailChecker()
    //-------------------------------------------------------------------------
    AxMailChecker(AxPluginMailBox *thePlugin);

    //-------------------------------------------------------------------------
    //     ~AxMailChecker()
    //-------------------------------------------------------------------------
    ~AxMailChecker();

    //-------------------------------------------------------------------------
    //     startChecking()
    //-------------------------------------------------------------------------
    bool startChecking();

    //-------------------------------------------------------------------------
    //     stopChecking()
    //-------------------------------------------------------------------------
    void stopChecking();

    //-------------------------------------------------------------------------
    //     getState()
    //-------------------------------------------------------------------------
    MCState getState();

  protected:
    //-------------------------------------------------------------------------
    //     setState()
    //-------------------------------------------------------------------------
    void setState(MCState theState);

    //-------------------------------------------------------------------------
    //     Action()
    //-------------------------------------------------------------------------
    virtual void Action();

    //-------------------------------------------------------------------------
    //     checkMailBox()
    //-------------------------------------------------------------------------
    /** request the mutex and check one mailbox.
     *
     *  @returns true of checked ok
     */
    bool checkMailBox(Ax::Mail::MailFolder *theFolder, bool fForceStartExternal, bool &fAbortThread);

  public:
    //-------------------------------------------------------------------------
    //     attributes
    //-------------------------------------------------------------------------
    /** Plugin
     */
    AxPluginMailBox *myPlugin;

    /** time to display the osd-message "new mail in..."
     */
    int myOSDMessageTime;

    /** Plugin settings
     */
    const AxPluginSettings &myPluginSettings;

    /** Mutex is locked during a mail-check.
     *  So if main thread want's to set fExit, it first requests the mutex
     *  (which gives the thread the chance to finish the current mailbox)
     *  and sets fExit afterwards
     */
    cMutex myCheckMutex;

    /** Dictionary: which pointer has new mails and the user didn't press
     *  kOk or kBack.
     */
    std::map<Ax::Mail::MailFolder *, bool> myAskAgain;

    /** state of this thread and mutex to lock the member
     */
    cMutex  myStateMutex;
    MCState myState;

}; // class AxSettings

#endif

