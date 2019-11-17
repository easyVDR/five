#ifndef __Ax_Mail_MailBoxMgr_H__
#define __Ax_Mail_MailBoxMgr_H__
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
 *   $Date: 2013-03-20 19:43:16 +0100 (Mi, 20 Mär 2013) $
 *   $Revision: 1.10 $
 */
//=============================================================================

//=============================================================================
//     includes
//=============================================================================

//----- qt --------------------------------------------------------------------
//----- CORBA -----------------------------------------------------------------
//----- ICE -------------------------------------------------------------------
//----- C++ -------------------------------------------------------------------
#include <string>

//----- C ---------------------------------------------------------------------
//----- AxLib -----------------------------------------------------------------
#include <Ax/Mail/Globals.h>
#include <Ax/Mail/LogHandler.h>

//----- local -----------------------------------------------------------------
//----- vdr -------------------------------------------------------------------
//----- c-client --------------------------------------------------------------
//----- other includes --------------------------------------------------------

//=============================================================================
//     defines
//=============================================================================
#define MBM ::Ax::Mail::MailBoxMgr::instance()

//=============================================================================
//     namespaces
//=============================================================================
namespace Ax   {
namespace Mail {

//=========================================================================
//     forward declarations
//=========================================================================
class CallBackDispatcher;
class CallBackProcessor;
class MailBoxMgrCBP;

//=========================================================================
//     class MailBoxMgr
//=========================================================================
/** The singleton MailBoxMgr is responsible for the initialization of
 *  the c-client library and to create and destroy some internal objects.
 *  Additionally this class is responsible for the logging of the
 *  communication beween client and server.
 */
class MailBoxMgr
{
  private:
    //---------------------------------------------------------------------
    //     MailBoxMgr()
    //---------------------------------------------------------------------
    /** Constructor
    */
    MailBoxMgr();

    //---------------------------------------------------------------------
    //     ~MailBoxMgr()
    //---------------------------------------------------------------------
    /** Destructor
     */
    ~MailBoxMgr();

  public:
    //---------------------------------------------------------------------
    //     instance()
    //---------------------------------------------------------------------
    /** Returns the only instance of this singleton.
     *
     */
    static MailBoxMgr *instance();

    //---------------------------------------------------------------------
    //     terminate()
    //---------------------------------------------------------------------
    /** cleanup remaining MailBox-instances and destroy myself.
     *
     *  this method must be called at the end of the application.
     */
    static void terminate();

    //---------------------------------------------------------------------
    //     setTimeOut()
    //---------------------------------------------------------------------
    void setTimeOut(long theTimeOut);

    //---------------------------------------------------------------------
    //     getCBD()
    //---------------------------------------------------------------------
    Ax::Mail::CallBackDispatcher *getCBD() const;

    //---------------------------------------------------------------------
    //     getCBP()
    //---------------------------------------------------------------------
    Ax::Mail::CallBackProcessor  *getCBP()  const;

    //---------------------------------------------------------------------
    //     getLogHandler()
    //---------------------------------------------------------------------
    LogHandler &getLogHandler();

  private:
    //---------------------------------------------------------------------
    //     attributes
    //---------------------------------------------------------------------
    /** singleton instance
     */
    static MailBoxMgr  *myInstance;

    /** flag: indicates whether c-client was already initialized my
     *        the running application
     */
    static bool         myIsCClientInitialized;

    /** dispatches callbacks from c-client to callback processors
     */
    CallBackDispatcher *myCallBackDispatcher;

    /** this callback processor gets all callbacks that weren't processed by
     *  a previous callback processor.
     */
    CallBackProcessor  *myCallBackProcessor;

    /** collect logs that didn't go to a specific mail-account
     */
    LogHandler          myLogHandler;

}; // class MailBoxMgr

//=============================================================================
//     namespaces
//=============================================================================
} // Mail
} // Ax

#endif

