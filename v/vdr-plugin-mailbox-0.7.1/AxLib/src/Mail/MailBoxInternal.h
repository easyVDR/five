#ifndef __Ax_Mail_MailBoxInternal_H__
#define __Ax_Mail_MailBoxInternal_H__
//=============================================================================
/*
 * See the file README in the main directory for a description of
 * this software, copyright information, and how to reach the author.
 *
 * Author: alex
 * Date:   28.03.2006
 *
 * Last modfied:
 *   $Author: alex $
 *   $Date: 2013-03-20 19:53:42 +0100 (Mi, 20 Mär 2013) $
 */
//=============================================================================

//=============================================================================
//     includes
//=============================================================================

//----- qt --------------------------------------------------------------------
//----- CORBA -----------------------------------------------------------------
//----- ICE -------------------------------------------------------------------
//----- C++ -------------------------------------------------------------------
//----- C ---------------------------------------------------------------------
//----- AxLib -----------------------------------------------------------------
//----- vdr -------------------------------------------------------------------
//----- c-client --------------------------------------------------------------
#include <Ax/Mail/MailBoxSetup.h>
#include <Ax/Mail/MailFolder.h>
#include <Ax/Mail/LogHandler.h>

//----- local -----------------------------------------------------------------
#include "c-client-header.h"
// #include "Request.h"

//=============================================================================
//     namespaces
//=============================================================================
namespace Ax   {
namespace Mail {

//=============================================================================
//     forward declarations
//=============================================================================
class MailBox;
class MailFolder;
class MailBoxCBP;

//=============================================================================
//     class MailBoxInternal
//=============================================================================
/** An instance of this class is contained in every MailBox instance.
 *
 *  The only purpose of this class is to hide some methods from the users
 *  of Ax::Mail::MailBox which are only used internally and should'n get
 *  called from the outside to avoid internal inconsistencies.
 *
 */
class MailBoxInternal
{
  public:
    //-------------------------------------------------------------------------
    //     MailBoxInternal()
    //-------------------------------------------------------------------------
    /** Constructor
     */
    MailBoxInternal(MailBox *theMailBox, const MailBoxSetup &theSetup);

    //-------------------------------------------------------------------------
    //     MailBoxInternal()
    //-------------------------------------------------------------------------
    /** Destructor
     */
    ~MailBoxInternal();

    //-------------------------------------------------------------------------
    //     getSetup()
    //-------------------------------------------------------------------------
    /** Returns a pointer of the setup of the mailbox.
     *
     *  @returns a pointer of the setup of the mailbox.
     */
    const MailBoxSetup &getSetup() const;

    //---------------------------------------------------------------------
    //     getLogHandler()
    //---------------------------------------------------------------------
    /** returns the internal log-handler of this MailBox
     */
    LogHandler &getLogHandler();

    //-------------------------------------------------------------------------
    //     getStream()
    //-------------------------------------------------------------------------
    /** Returns a pointer to the \c c-client stream type used for the connection
     *  to the mailbox.
     *
     *  @returns a pointer to the \c c-client stream used for the connection.
     */
    MAILSTREAM *getStream() const;

  private:
    //-------------------------------------------------------------------------
    /** allow friends to call setStream()
     */
    friend class RequestOpen;
    friend class RequestClose;

    //-------------------------------------------------------------------------
    //     setStream()
    //-------------------------------------------------------------------------
    /** Sets the internal stream-handle
     *
     *  This is called from
     *  - RequestOpen ::execInternal()
     *  - RequestClose::execInternal()
     */
    void setStream(MAILSTREAM *theStream);

  public:
    //-------------------------------------------------------------------------
    //     getCBP()
    //-------------------------------------------------------------------------
    /** Returns the CallBackProcessor of thie MailBox
     */
    CallBackProcessor *getCBP() const;

  public:
    //-------------------------------------------------------------------------
    //     getRootFolder()
    //-------------------------------------------------------------------------
    /** Returns the topmost folder of the MailBox
     *
     *  If the root-folder wasn't instantiated before, it's implicitely
     *  created by this call.
     */
    MailFolder *getRootFolder() const;

    //-------------------------------------------------------------------------
    //     getCurrentFolder()
    //-------------------------------------------------------------------------
    /** Returns the current Folder of the MailBox
     *
     *  Attention: It isn't guaranteed that the currentFolder is already
     *             open.
     *
     */
    MailFolder *getCurrentFolder() const;

  private:
    //-------------------------------------------------------------------------
    /** allow friends to call setCurrentFolder()
     */
    friend bool MailFolder::requestStatus();
    friend bool MailFolder::open();

    //-------------------------------------------------------------------------
    //     setCurrentFolder()
    //-------------------------------------------------------------------------
    /** Sets the current folder of the MailAccount
     *
     *  The only way to change the current folder is a call to
     *  MailFolder::open() or by a call to MailFolder::requestStatus()
     */
    void setCurrentFolder(MailFolder *theFolder);

  public:
    //-------------------------------------------------------------------------
    //     getDelimiter()
    //-------------------------------------------------------------------------
    /** requests the hierary delimiter for mail accounts which support folders
     *  if not already present
     */
    char getDelimiter();

    //-------------------------------------------------------------------------
    //     hasDelimiter()
    //-------------------------------------------------------------------------
    /** returns true if the delimiter is already present but makes
     *  no attempts to get the delimiter if not already there
     */
    bool hasDelimiter() const;

  private:
    //-------------------------------------------------------------------------
    //     attributes
    //-------------------------------------------------------------------------
    /** reference to the MailBox-instance this instance belongs to
     */
    MailBox      *myMailBox;

    /** setup for the mailbox.
     */
    MailBoxSetup  mySetup;

    /** collect logs for this mail account
     */
    LogHandler    myLogHandler;

    /** Pointer to the \c c-client mailstream used for the connection to the
     *  mailbox.
     */
    MAILSTREAM   *myStream;

    /** process c-client callbacks
     */
    MailBoxCBP   *myCBP;

    /** the topmost folder of this mailbox
     *
     *  a folder is resonsible for the management (lifetime) for
     *  all of its subfolders except the current folder of the mailbox
     * 
     *  mutable because it's created/modified in getRootFolder()
     */
    mutable MailFolder *myRootFolder;

    /** the current folder
     *  (unlike the other subfolders, which are owned by their
     *  enclosing (parent) folder, this folder instance is managed
     *  by the MailBox instance
     */
    MailFolder   *myCurrentFolder;

    /** hierarchy delimiter
     */
    char  myDelimiter;

    /** flag: this is set to true by getDelimiter() if the delimiter
     *  isn't present and it wasn't possible to request a delimiter
     *  from the mail account.
     */
    bool  myDelimRequFailed;

}; // class MailBoxInternal

//=============================================================================
//     namespaces
//=============================================================================
} // Mail
} // Ax

#endif

