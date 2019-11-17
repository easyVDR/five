#ifndef __Ax_Mail_Mail_H__
#define __Ax_Mail_Mail_H__
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
//----- C ---------------------------------------------------------------------
//----- AxLib -----------------------------------------------------------------
#include <Ax/Mail/Globals.h>

//----- vdr -------------------------------------------------------------------
//----- local -----------------------------------------------------------------

//=============================================================================
//     namespaces
//=============================================================================
namespace Ax   {
namespace Mail {

//=============================================================================
//     forward declarations
//=============================================================================
class MsgCache;

//=============================================================================
//     class Mail
//=============================================================================
/** An instance of Mail represents one mail in a MailBox.
 *
 *  Mails are composed of a tree of one or more instances of MailPart.
 *  The method getRootPart() returns the top-most part of a mail.
 */
class Mail
{
    //-------------------------------------------------------------------------
    //     friends
    //-------------------------------------------------------------------------
    /** only MailFolders are able to create mails to keep internal dependencies
     *  consistent. Therefore MailFolder is friend and the constructors is private.
     */
    friend class MailFolder;

  private:
    //-------------------------------------------------------------------------
    //     Mail()
    //-------------------------------------------------------------------------
    /** Constructor
     *
     */
    Mail(MailFolder *theFolder, const MsgCache &theMsgCache);

    //-------------------------------------------------------------------------
    //     ~Mail()
    //-------------------------------------------------------------------------
    /** Destructor
     */
    ~Mail();

  public:
    //-------------------------------------------------------------------------
    //     getFolder()
    //-------------------------------------------------------------------------
    /** returns the MailFolder to which this Mail belongs to.
     *
     *  @returns the MailFolderFolder to which this Mail belongs to.
     */
    MailFolder *getFolder() const;

    //-------------------------------------------------------------------------
    //     getMailBox()
    //-------------------------------------------------------------------------
    /** returns the MailBox to which this Mail belongs to.
     *
     *  @returns the MailBox to which this Mail belongs to.
     */
    MailBox *getMailBox() const;

    //-------------------------------------------------------------------------
    //     getMsgCache()
    //-------------------------------------------------------------------------
    const MsgCache *getMsgCache() const { return myMsgCache; }

    //-------------------------------------------------------------------------
    //     getRootPart()
    //-------------------------------------------------------------------------
    /** returns topmost MailPart of this Mail.
     *
     *  @returns topmost MailPart of this Mail.
     */
    MailPart *getRootPart();

    //-------------------------------------------------------------------------
    //     requestHeader()
    //-------------------------------------------------------------------------
    /** request date, from and subject from c-client.
     *
     *  @returns true if successful
     */
    bool requestHeader();

    //-------------------------------------------------------------------------
    //     getMsgNo()
    //-------------------------------------------------------------------------
    /** Returns the number (MsgNo) of this mail.
     *
     *  Attention: the MsgNo may change during lifetime of this message
     *  when other mails are deleted from the mailbox. If the MsgNo
     *  becomes 0 the message has been expunged from the mailbox.
     *
     *  @returns the number (MsgNo) of this mail.
     */
    long getMsgNo() const;

    //-------------------------------------------------------------------------
    //     hasValidFlags()
    //-------------------------------------------------------------------------
    /** Returns whether the states of the getIsRecent(), getIsSeen(),
     *  getIsFlagged() and getIsAnswered() provide valid information.
     */
    bool hasValidFlags() const;

    //-------------------------------------------------------------------------
    //     getIsRecent()
    //-------------------------------------------------------------------------
    /** Returns whether this mail has the \c \\Recent flag set.
     *
     *  @returns whether this mail has the \c \\Recent flag set.
     */
    bool getIsRecent() const;

    //-------------------------------------------------------------------------
    //     getIsSeen()
    //-------------------------------------------------------------------------
    /** Returns whether this mail has the \c \\Seen flag set.
     *
     *  @returns whether this mail has the \c \\Seen flag set.
     */
    bool getIsSeen() const;

    //-------------------------------------------------------------------------
    //     getIsFlagged()
    //-------------------------------------------------------------------------
    /** Returns whether this mail has the \c \\Flagged flag set.
     *
     *  @returns whether this mail has the \c \\Flagged flag set.
     */
    bool getIsFlagged() const;

    //-------------------------------------------------------------------------
    //     getIsAnswered()
    //-------------------------------------------------------------------------
    /** Returns whether this mail has the \c \\Answered flag set.
     *
     *  @returns whether this mail has the \c \\Answered flag set.
     */
    bool getIsAnswered() const;

    //-------------------------------------------------------------------------
    //     getIsDeleted()
    //-------------------------------------------------------------------------
    /** Returns whether this mail has the \c \\Deleted flag set.
     *
     *  @returns whether this mail has the \c \\Deleted flag set.
     */
    bool getIsDeleted() const;

    //-------------------------------------------------------------------------
    //     getBodySize()
    //-------------------------------------------------------------------------
    /** Returns the size of the body of this mail in bytes.
     *
     *  @returns the size of the body of this mail in bytes.
     */
    long getBodySize() const;

    //-------------------------------------------------------------------------
    //     getUID()
    //-------------------------------------------------------------------------
    /** Returns the unique id of this mail
     *
     *  @returns the unique id of this mail
     */
    unsigned long getUID() const;

    //-------------------------------------------------------------------------
    //     getDate()
    //-------------------------------------------------------------------------
    /** Returns the date of the mail in the form:
     *  <tt>dd-mmm-yyyy hh:mm:ss +zzzz</tt>.
     *
     *  @returns the date of the mail.
     */
    const std::string &getDate() { return myDate; }

    //-------------------------------------------------------------------------
    //     getFrom()
    //-------------------------------------------------------------------------
    /** Returns the sender of the mail.
     *
     *  This method internally calls the method \c mail_fetchfrom of
     *  \ref c_client which works as follows:
     *
     *  "If the personal name of the first address in the envelope's from
     *   item is non-NIL, it is used; otherwise a string is created by
     *   appending the mailbox of the first address, an "@", and the host of
     *   the first address."
     *
     *  @returns the sender of the mail.
     */
    const std::string &getFrom() const { return myFrom; }

    //-------------------------------------------------------------------------
    //     getSubject()
    //-------------------------------------------------------------------------
    /** Returns the subject line of the mail.
     *
     *  @returns the subject line of the mail.
     */
    const std::string &getSubject() const { return mySubject; }

    //-------------------------------------------------------------------------
    //     getFullText()
    //-------------------------------------------------------------------------
    /** Returns the complete body-text of the mail in its orginal form.
     *
     *  @returns the complete body-text of the mail.
     */
    std::string getFullText() const;

    //-------------------------------------------------------------------------
    //     setFlag()
    //-------------------------------------------------------------------------
    /** Set the given flag.
     *
     *  @param theFlag flag to set.
     */
    void setFlag(MailFlag theFlag);

    //-------------------------------------------------------------------------
    //     clearFlag()
    //-------------------------------------------------------------------------
    /** Clears the given flag.
     *
     *  @param theFlag flag to clear.
     */
    void clearFlag(MailFlag theFlag);

    //-------------------------------------------------------------------------
    //     getFlag()
    //-------------------------------------------------------------------------
    /** Returns whether the given flag is set.
     *
     *  @param theFlag flag to check
     *  @returns the state of the given flag.
     */
    bool getFlag(MailFlag theFlag) const;

    //-------------------------------------------------------------------------
    //     getFlagStr()
    //-------------------------------------------------------------------------
    /** Returns the IMAP-string-representation of the given flag.
     *
     *  @returns the IMAP-string-representation of the given flag.
     */
    static std::string getFlagStr(MailFlag theFlag);

  private:
    //-------------------------------------------------------------------------
    //     attributes
    //-------------------------------------------------------------------------
    /** Pointer to the folder this mail belongs to.
     */
    MailFolder   *myMailFolder;

    /** The root part of this mail
     */
    MailPart     *myRootPart;

    /** Pointer to the \ref c_client message-cache.
     */
    MsgCache     *myMsgCache;

    /** the unique id of this msg
     */
    unsigned long myUID;

    /** cached values for faster access
     */
    std::string   mySubject;
    std::string   myFrom;
    std::string   myDate;

}; // class Mail

//=============================================================================
//     namespaces
//=============================================================================
} // Mail
} // Ax

#endif

