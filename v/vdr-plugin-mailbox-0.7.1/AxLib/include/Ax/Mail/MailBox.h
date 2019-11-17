#ifndef __Ax_Mail_MailBox_H__
#define __Ax_Mail_MailBox_H__
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
 *   $Date: 2013-03-20 20:09:21 +0100 (Mi, 20 Mär 2013) $
 *   $Revision: 1.9 $
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
#include <Ax/Mail/Globals.h>
#include <Ax/Mail/MailBoxMgr.h>
#include <Ax/Mail/MailFolder.h>

//----- vdr -------------------------------------------------------------------
//----- c-client --------------------------------------------------------------
//----- local -----------------------------------------------------------------

//=============================================================================
//     namespaces
//=============================================================================
namespace Ax   {
namespace Mail {

//=============================================================================
//     forward declarations
//=============================================================================
class MailFolder;
class MailBoxMgr;
class MailBoxSetup;
class CallBackProcessor;
class MailBoxCBP;
class MailBoxInternal;
class LogHandler;

//=============================================================================
//     class MailBox
//=============================================================================
/** An instance of MailBox is used to access a mail account and to retrieve
 *  mails from it.
 */
class MailBox
{
  public:
    //-------------------------------------------------------------------------
    //     MailBox()
    //-------------------------------------------------------------------------
    /** Constructor
     *
     *  Note: MailBox doesn't copy the contents of theSetup, instead it holds
     *  a pointer the the given setup. Therefore theSetup must exists as long
     *  as an instance of MailBox exist.
     *
     *  \todo think about the memory managmnt of theSetup.
     *
     *  @param theSetup contains the settings for a mailbox.
     */
    MailBox(const MailBoxSetup &theSetup);

    //-------------------------------------------------------------------------
    //     ~MailBox()
    //-------------------------------------------------------------------------
    /** Destructor
     *
     *  Destroys all instances of Mail and closes the connection if isConnected().
     */
    virtual ~MailBox();

    //-------------------------------------------------------------------------
    //     getInternal()
    //-------------------------------------------------------------------------
    /** returns a reference to MailBox-internal data
     *
     *  using the pimpl-idiom here
     */
    MailBoxInternal &getInternal() const;

    //-------------------------------------------------------------------------
    //     getSetup()
    //-------------------------------------------------------------------------
    /** returns a pointer of the setup of this mailbox.
     *
     *  @returns a pointer of the setup of this mailbox.
     */
    const MailBoxSetup &getSetup() const;

    //---------------------------------------------------------------------
    //     getLogHandler()
    //---------------------------------------------------------------------
    /** returns the internal log-handler of this MailBox
     */
    LogHandler &getLogHandler() const;

    //---------------------------------------------------------------------
    //     getLastImportantMsg()
    //---------------------------------------------------------------------
    /** returns the last 'important' message from the internal LogHandler
     */
    std::string getLastImportantMsg() const;

    //-------------------------------------------------------------------------
    //     getRootFolder()
    //-------------------------------------------------------------------------
    /** Returns the topmost folder
     *
     *  Attention: this method will always return NULL until setupFolderTree()
     *             has been called.
     */
    MailFolder *getRootFolder() const;

    //-------------------------------------------------------------------------
    //     getCurrentFolder()
    //-------------------------------------------------------------------------
    /** returns the current folder of this
     */
    MailFolder *getCurrentFolder() const;

    //-------------------------------------------------------------------------
    //     supportsFolders()
    //-------------------------------------------------------------------------
    /** Returns true if this mailbox supports folder hierarchies.
     *
     *  Attention: Don't call setupFolderTree() or other folder-browsing
     *             methods if supportsFolders() returns false.
     */
    bool supportsFolders() const;

    //-------------------------------------------------------------------------
    //     some small helper methods
    //-------------------------------------------------------------------------
    bool isIMAP() const;
    bool isPOP3() const;
    bool isNNTP() const;

    //-------------------------------------------------------------------------
    //     getAccountName()
    //-------------------------------------------------------------------------
    /** Returns the symbolic name of this mailbox (from the setup).
     */
    const std::string &getAccountName() const;

    //-------------------------------------------------------------------------
    //     isConnected()
    //-------------------------------------------------------------------------
    /** Returns true if the connection to the mailbox is established (the
     *  stream is not NULL)
     */
    bool isConnected() const;

    //-------------------------------------------------------------------------
    //     getDelimiter()
    //-------------------------------------------------------------------------
    /** returns the hierarchy delimiter character
     *
     *  note: if the MailBox doesn't support folders (supportsFolders() == false)
     *        or if the call isn't able to query the delimiter this method
     *        returns '\0'
     *  note: only the first call this method requests the delimiter from the
     *        mail account. If this call fails no further attempts are made.
     */
    char getDelimiter() const;

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
    /** private implementation data (pimpl-idiom)
     */
    MailBoxInternal   *myInternal;

}; // class MailBox

//=============================================================================
//     namespaces
//=============================================================================
} // Mail
} // Ax

#endif
