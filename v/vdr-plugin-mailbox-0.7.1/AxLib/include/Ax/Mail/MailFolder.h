#ifndef __Ax_Mail_MailFolder_H__
#define __Ax_Mail_MailFolder_H__
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
 *   $Revision: 1.1 $
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
class MailBox;
class MailBoxSetup;
class Mail;

//=============================================================================
//     class MailFolder
//=============================================================================
/** Instances of this class represent a folder within a MailBox
 *
 *  MailFolders can contain other MailFolders and Mails
 *
 */
class MailFolder
{
    //-------------------------------------------------------------------------
    //     friends
    //-------------------------------------------------------------------------
    /** only MailBoxInternal is able to create folders to keep internal
     *  dependencies consistent. Therefore MailBoxInternal is friend and
     *  constructors are private.
     */
    friend class MailBoxInternal;

  private:
    //-------------------------------------------------------------------------
    //     avoid copy-construction / assignment
    //-------------------------------------------------------------------------
    MailFolder(const MailFolder &);
    MailFolder &operator=(const MailFolder &);

    //-------------------------------------------------------------------------
    //     MailFolder()
    //-------------------------------------------------------------------------
    /** Constructor
     */
    MailFolder(       MailBox     *theMailBox
              , const std::string &theFolderPath
              ,       MailFolder  *theParentFolder
              ,       long         theAttributes = -1
              );

    //-------------------------------------------------------------------------
    //     ~MailFolder()
    //-------------------------------------------------------------------------
    /** Destructor
     *
     *  Any child parts contained within this part will be destryed.
     */
    ~MailFolder();

    //-------------------------------------------------------------------------
    //     clear()
    //-------------------------------------------------------------------------
    /** called from constructors and destructors to release internal
     *  memory and set attributes to defined values
     */
    void clear();

  public:
    //-------------------------------------------------------------------------
    //     getMailBox()
    //-------------------------------------------------------------------------
    /** returns a pointer to the MailBox this MailFolder belongs to
     */
    MailBox *getMailBox() const;

    //-------------------------------------------------------------------------
    //     getParentFolder()
    //-------------------------------------------------------------------------
    /** returns a pointer to the parent folder
     */
    MailFolder *getParentFolder() const;

  private:
    //-------------------------------------------------------------------------
    //     setParentFolder()
    //-------------------------------------------------------------------------
    /** Assign the the parent folder to this
     */
    void setParentFolder(MailFolder *theParentFolder);

  public:
    //-------------------------------------------------------------------------
    //     getMailBoxString()
    //-------------------------------------------------------------------------
    /** Returns the complete MailBox-string for this folder
     *
     *  @returns the complete MailBox-string for this folder
     */
    const std::string getMailBoxString() const;

    //-------------------------------------------------------------------------
    //     getFolderPath()
    //-------------------------------------------------------------------------
    /** Returns the folder name with complete path, e.g. 'INBOX.Lists.VDR'
     *
     *  @returns the folder name
     */
    const std::string &getFolderPath() const;

    //-------------------------------------------------------------------------
    //     getFolderName()
    //-------------------------------------------------------------------------
    /** Returns the folder name (only the rightmost part), e.g. if the
     *  FolderPath is 'INBOX.Lists.VDR' then this method returns 'VDR'
     *
     *  @returns the folder name
     */
    std::string getFolderName() const;

    //-------------------------------------------------------------------------
    //     getSetup()
    //-------------------------------------------------------------------------
    /** Convenience method: returns the MailBoxSetup for the MailBox this
     *  MailFolder belongs to.
     */
    const MailBoxSetup &getSetup() const;

    //=========================================================================
    //     commands to the mail server
    //=========================================================================

    //-------------------------------------------------------------------------
    //     requestStatus()
    //-------------------------------------------------------------------------
    /** Initiates a status-request to get information about the mailbox.
     *
     *  This method must be called before any calls to getCountMails(),
     *  getCountUnseen() and getCountRecent().
     *
     */
    bool requestStatus();

    //-------------------------------------------------------------------------
    //     requestSubFolders()
    //-------------------------------------------------------------------------
    /** Query the direct subfolders of this from the mail account
     */
    bool requestSubFolders();

    //-------------------------------------------------------------------------
    //     open()
    //-------------------------------------------------------------------------
    /** Opens the connection to the mailbox.
     *
     *  This method must be called, before getMail() may be called.
     */
    bool open();

    //-------------------------------------------------------------------------
    //     isOpen()
    //-------------------------------------------------------------------------
    /** Returns true if the connection to the mailbox is established.
     */
    bool isOpen() const;

    //-------------------------------------------------------------------------
    //     setIsOpen()
    //-------------------------------------------------------------------------
    /** sets this folder to the open or not-open state
     *
     *  this is called from MailBox if the current folder is changed
     */
    void setIsOpen(bool fIsOpen);

    //-------------------------------------------------------------------------
    //     close()
    //-------------------------------------------------------------------------
    /** Close the connection to the mailbox
     *
     *  This also resets the connection to the server
     *  -> MailBox->stream() is 0 after this
     */
    void close();

    //-------------------------------------------------------------------------
    //     getSubFolders()
    //-------------------------------------------------------------------------
    const MailFolderCltn &getSubFolders() const { return mySubFolders; }

    //-------------------------------------------------------------------------
    //     getSubFolderByName()
    //-------------------------------------------------------------------------
    MailFolder *getSubFolderByName(const std::string &theName) const;

    //-------------------------------------------------------------------------
    //     hasStatus()
    //-------------------------------------------------------------------------
    /** Returns if we have the state of the mailbox either by a call
     *  to requestState() or by a call to open().
     *
     *  @returns true if the last requestStatus() or open() succeeded.
     */
    bool hasStatus() const;

    //-------------------------------------------------------------------------
    //     supportsFlag()
    //-------------------------------------------------------------------------
    /** Returns whether the given flag can be set permanently in the current
     *  situation.
     *
     *  @param theFlag flag to set.
     */
    bool supportsFlag(MailFlag theFlag) const;

    //-------------------------------------------------------------------------
    //     getCountMails()
    //-------------------------------------------------------------------------
    /** Returns the total number of mails (only valid if hasStatus() == true)
     *
     *  @returns the total number of mails.
     */
    unsigned long getCountMails() const;

    //-------------------------------------------------------------------------
    //     getCountUnseen()
    //-------------------------------------------------------------------------
    /** Returns the number of unseen (not \\Seen) mails (only valid if hasStatus() == true)
     *
     *  @returns the number of unseen (not \\Seen) mails.
     */
    unsigned long getCountUnseen() const;

    //-------------------------------------------------------------------------
    //     getCountRecent()
    //-------------------------------------------------------------------------
    /** Returns the number of recent (\\Recent) mails (only valid if hasStatus() == true)
     *
     *  @returns the number of recent (\\Recent) mails.
     */
    unsigned long getCountRecent() const;

    //-------------------------------------------------------------------------
    //     getUIDNext()
    //-------------------------------------------------------------------------
    /** Returns...
     */
    unsigned long getUIDNext() const;

    //=========================================================================
    //     mail access methods
    //=========================================================================

    //-------------------------------------------------------------------------
    //     getMail()
    //-------------------------------------------------------------------------
    /** Returns the instance with the given mail number.
     *
     *  The mail number has to be in the range of 1..getCountMails().
     *
     *  @param   theMailNum is the number of the mail [1..getCountMails()]
     *  @returns a pointer to the Mail-instance with the given
     *           mail number or 0 if theMailNum is out of range or if the
     *           mailbox was not able to fetch the mail.
     */
    Mail *getMail(long theMsgNo);

    //-------------------------------------------------------------------------
    //     getNext()
    //-------------------------------------------------------------------------
    /** Returns the next mail after the given mail or 0 if not possible.
     *
     *  Returns 0 if the mailbox doesn't contain any mails, the given Mail
     *  isn't within this mailbox or the given mail is the last mail in the
     *  mailbox.
     *
     *  Returns the first mail if the given mail is 0 and there is at least
     *  one mail in the mailbox.
     *
     *  @returns the next mail after the given or the first mail if
     *           the given mail is 0.
     */
    Mail *getNext(Mail *theMail = 0);

    //-------------------------------------------------------------------------
    //     getPrev()
    //-------------------------------------------------------------------------
    /** Returns the previous mail before the given mail or 0 if not possible.
     *
     *  Returns 0 if the mailbox doesn't contain any mails, the given Mail
     *  isn't within this mailbox or the given mail is the first mail in the
     *  mailbox.
     *
     *  Returns the last mail if the given mail is 0 and there is at least
     *  one mail in the mailbox.
     *
     *  @returns the previous mail before the given or the last mail if
     *           the given mail is 0.
     */
    Mail *getPrev(Mail *theMail = 0);

    //-------------------------------------------------------------------------
    //     getNextNew()
    //-------------------------------------------------------------------------
    /** Returns the next new mail after the given mail or 0 if not possible.
     *
     *  Returns 0 if the mailbox doesn't contain any new mails or there
     *  is no new mail after the given mail.
     *
     *  Returns the first new mail if the given mail is 0 or the given mail
     *  is not in the mailbox.
     *
     *  @returns the next new mail after the given or the first mail if
     *           the given mail is 0.
     */
    Mail *getNextNew(Mail *theMail = 0);

    //-------------------------------------------------------------------------
    //     getPrevNew()
    //-------------------------------------------------------------------------
    /** Returns the previous new mail before the given mail or 0 if not possible.
     *
     *  Returns 0 if the mailbox doesn't contain any new mails or there
     *  is no new mail before the given Mail.
     *
     *  Returns the last new mail if the given mail is 0 or the given mail
     *  is not in the mailbox.
     *
     *  @returns the previous new mail before the given or the last new mail if
     *           the given mail is 0.
     */
    Mail *getPrevNew(Mail *theMail = 0);

    //-------------------------------------------------------------------------
    //     getSurroundings()
    //-------------------------------------------------------------------------
    /** Returns the mails surrounding
     *
     */
    void getSurroundings( Mail * theMail    = 0
                        , Mail **thePrev    = 0
                        , Mail **theNext    = 0
                        , Mail **thePrevNew = 0
                        , Mail **theNextNew = 0
                        );

    //=========================================================================
    //     Atttributes of the folder
    //=========================================================================
  public:
    //-------------------------------------------------------------------------
    //     setAttributes()
    //-------------------------------------------------------------------------
    void setAttributes(long theAttr) { myAttributes = theAttr; }

    //-------------------------------------------------------------------------
    //     hasAttributes()
    //-------------------------------------------------------------------------
    bool hasAttributes() const { return myAttributes != -1; }

    //-------------------------------------------------------------------------
    //     hasAttributes()
    //-------------------------------------------------------------------------
    long getAttributes() const { return myAttributes; }

    //-------------------------------------------------------------------------
    //     canSelect()
    //-------------------------------------------------------------------------
    /** returns true if this folder probably contains mails
     */
    bool canSelect() const;

    //-------------------------------------------------------------------------
    //     isNode()
    //-------------------------------------------------------------------------
    /** returns true if this folder probably has sub-folders
     */
    bool isNode() const;

  private:
    //-------------------------------------------------------------------------
    //     attributes
    //-------------------------------------------------------------------------
    /** the mailbox this part belongs to
     */
    MailBox          *myMailBox;

    /** parent folder (if present)
     *
     *  this may be dynamically updated by getParentFolder()
     *  which is const and therfore myParentFolder must be mutable
     */
    mutable MailFolder *myParentFolder;

    /** the name of the folder
     */
    std::string       myFolderPath;

    /** true after a successful call to open and before
     *  for any other folder open() or requestStatus() was called
     */
    bool              myIsOpen;

    /** true if status of mailbox is known.
     */
    bool              myHasStatus;

    /** the total number of mails
     */
    unsigned long     myCountMails;

    /** the number of new (not \Seen) mails
     */
    unsigned long     myCountUnseen;

    /** the number of recent (\Recent) mails
     */
    unsigned long     myCountRecent;

    /** the next uid
     */
    unsigned long     myUIDNext;

    /** the uid valitity
     */
    unsigned long     myUIDValidity;

    /** a vector of pointers to Mail instances
     */
    MailVector        myMails;

    /** a vector of pointers to MailFolder instances
     */
    MailFolderCltn    mySubFolders;

    /** Attribtues of the folder
     */
    long myAttributes;

}; // class MailFolder

//=============================================================================
//     namespaces
//=============================================================================
} // Mail
} // Ax

#endif

