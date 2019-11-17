#ifndef __Ax_Mail_Request_H__
#define __Ax_Mail_Request_H__
//=============================================================================
/*
 * See the file README in the main directory for a description of
 * this software, copyright information, and how to reach the author.
 *
 * Author: Alex
 * Date:   27.12.2004
 *
 * Last modfied:
 *   $Author: alex $
 *   $Date: 2004-11-08 22:04:16 +0100 (Mo, 08 Nov 2004) $
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
#include <list>

//----- C ---------------------------------------------------------------------
//----- AxLib -----------------------------------------------------------------
#include <Ax/Mail/Globals.h>

//----- vdr -------------------------------------------------------------------
//----- c-client --------------------------------------------------------------
//----- other includes --------------------------------------------------------
//----- local -----------------------------------------------------------------
#include "CallBackProcessor.h"

//=============================================================================
//     namespaces
//=============================================================================
namespace Ax   {
namespace Mail {

//=============================================================================
//     forward declarations
//=============================================================================
class MailBox;

//=============================================================================
//     class Request
//=============================================================================
/** A Request initiates one or more commands to a MailBox, processes the
 *  results.and stores data internally.
 *
 *  A Request (as a CallBackProcessor) is a direct child (in the sense of
 *  object parent-child relationship and not in the sense of inheritance)
 *  of the CallBackProcessor of the given MailBox. Callbacks
 *  not processed by any of the classes derived from Request are
 *  forwarded to the CallBackProcessor of the MailBox.
 *
 *  Derived classes should overwrite execInternal() to implement their
 *  function and return true if the request was successful.
 *
 *  Users of Request classes call the method exec(), which registers
 *  the instance at the CallBackDispatcher as active, call the method
 *  execIternal() and deregisters from the CallBackDispatcher.
 *
 */
class Request : public CallBackProcessor
{
    //-------------------------------------------------------------------------
    //     typedefs
    //-------------------------------------------------------------------------
    typedef CallBackProcessor PARENT;

  public:
    //-------------------------------------------------------------------------
    //     Request()
    //-------------------------------------------------------------------------
    /** Constructor
     */
    Request( MailBox           *theMailBox
           , const std::string &theName
           );

    //-------------------------------------------------------------------------
    //     ~Request()
    //-------------------------------------------------------------------------
    /** Destructor
     */
    virtual ~Request();

    //-------------------------------------------------------------------------
    //     exec()
    //-------------------------------------------------------------------------
    /** Register at the CallBackDispatcher as active using a CallBackLock,
     *  call execInternal() and deregister from the CallBackDispatcher.
     */
    bool exec();

  protected:
    //-------------------------------------------------------------------------
    //     execInternal()
    //-------------------------------------------------------------------------
    /** derived classes implement this method to do the real function: call
     *  any c-client-function.
     */
    virtual bool execInternal() = 0;

}; // class Request

#if 0
//=============================================================================
//     class RequestLock
//=============================================================================
/** A 'mutation' of 'normal Requestes: The real function isn't implemented
 *  in the execInternal method. Instead instances of this class request
 *  the lock during the complete lifetime of the instance.
 */
class RequestLock : public Request
{
    //-------------------------------------------------------------------------
    //     typedefs
    //-------------------------------------------------------------------------
    typedef Request PARENT;

  public:
    //-------------------------------------------------------------------------
    //     RequestLock()
    //-------------------------------------------------------------------------
    /** Constructor
     */
    RequestLock( MailBox           *theMailBox
               , const std::string &theName
               );

    //-------------------------------------------------------------------------
    //     ~RequestLock()
    //-------------------------------------------------------------------------
    /** Destructor
     */
    virtual ~RequestLock();

  protected:
    //-------------------------------------------------------------------------
    //     execInternal()
    //-------------------------------------------------------------------------
    /** empty dummy method
     */
    virtual bool execInternal();

  private:
    //-------------------------------------------------------------------------
    //     attributes
    //-------------------------------------------------------------------------
    CallBackLock myLock;

}; // class RequestLock
#endif
//=============================================================================
//     class RequestOpen
//=============================================================================
/** Open a Mail-Account
 *
 *  If a login is necessary, the process_mm_login()-callback is handled
 *  in the MailBoxCBP.
 *
 *  calls: mail_open()
 *
 */
class RequestOpen : public Request
{
    //-------------------------------------------------------------------------
    //     typedefs
    //-------------------------------------------------------------------------
    typedef Request PARENT;

  public:
    //-------------------------------------------------------------------------
    //     RequestOpen()
    //-------------------------------------------------------------------------
    /** Constructor
     */
    RequestOpen( const std::string &theMailBoxString
               , MailBox           *theMailBox
               , const std::string &theName
               );

    //-------------------------------------------------------------------------
    //     ~RequestOpen()
    //-------------------------------------------------------------------------
    /** Destructor
     */
    virtual ~RequestOpen();

    //-------------------------------------------------------------------------
    //     process_mm_exists()
    //-------------------------------------------------------------------------
    virtual bool process_mm_exists(unsigned long number);

  protected:
    //-------------------------------------------------------------------------
    //     execInternal()
    //-------------------------------------------------------------------------
    virtual bool execInternal();

  private:
    //-------------------------------------------------------------------------
    //     attributes
    //-------------------------------------------------------------------------
    char         *myMailBoxString;
    unsigned long myNumMails;

}; // class RequestOpen

//=============================================================================
//     class RequestClose
//=============================================================================
/** Closes the connection to the given mail account
 *
 *  calls: mail_close_full()
 *
 */
class RequestClose : public Request
{
    //-------------------------------------------------------------------------
    //     typedefs
    //-------------------------------------------------------------------------
    typedef Request PARENT;

  public:
    //-------------------------------------------------------------------------
    //     RequestClose()
    //-------------------------------------------------------------------------
    /** Constructor
     */
    RequestClose( MailBox           *theMailBox
                , const std::string &theName
                );

    //-------------------------------------------------------------------------
    //     ~RequestClose()
    //-------------------------------------------------------------------------
    /** Destructor
     */
    virtual ~RequestClose();

  protected:
    //-------------------------------------------------------------------------
    //     execInternal()
    //-------------------------------------------------------------------------
    virtual bool execInternal();

}; // class RequestClose

//=============================================================================
//     class RequestStatus
//=============================================================================
/** Request information about the given mailbox/folder
 *
 *  calls: mail_status()
 *
 */
class RequestStatus : public Request
{
    //-------------------------------------------------------------------------
    //     typedefs
    //-------------------------------------------------------------------------
    typedef Request PARENT;

  public:
    //-------------------------------------------------------------------------
    //     RequestStatus()
    //-------------------------------------------------------------------------
    /** Constructor
     */
    RequestStatus( const std::string &theMailBoxString
                 , MailBox           *theMailBox
                 , const std::string &theName
                 );

    //-------------------------------------------------------------------------
    //     ~RequestStatus()
    //-------------------------------------------------------------------------
    /** Destructor
     */
    virtual ~RequestStatus();

  protected:
    //-------------------------------------------------------------------------
    //     execInternal()
    //-------------------------------------------------------------------------
    virtual bool execInternal();

  public:
    //-------------------------------------------------------------------------
    //     c-client callbacks
    //-------------------------------------------------------------------------
    virtual bool process_mm_status(const std::string &theMailBoxString, const MAILSTATUS *theStatus);

    //-------------------------------------------------------------------------
    //     results
    //-------------------------------------------------------------------------
    const MAILSTATUS &getStatus() const { return myStatus; }

  private:
    //-------------------------------------------------------------------------
    //     attributes
    //-------------------------------------------------------------------------
    char         *myMailBoxString;
    std::string   myUser;
    std::string   myPass;

    bool          myHasStatus;
    MAILSTATUS    myStatus;

}; // class RequestStatus

//=============================================================================
//     class FolderInfo
//=============================================================================
class FolderInfo
{
  public:
    FolderInfo(const std::string &theName, long theAttrib)
      : Name  (theName), Attrib(theAttrib)
    { }

    std::string Name;
    long        Attrib;

}; // FolderInfo

typedef std::vector<FolderInfo> FolderInfoCltn;

//=============================================================================
//     class RequestList
//=============================================================================
/** Retrieve a list of folders
 *
 *  calls: mail_list()
 */
class RequestList : public Request
{
    //-------------------------------------------------------------------------
    //     typedefs
    //-------------------------------------------------------------------------
    typedef Request PARENT;

  public:
    //-------------------------------------------------------------------------
    //     RequestList()
    //-------------------------------------------------------------------------
    /** Constructor
     */
    RequestList( const std::string &theReference
               , const std::string &thePattern
               , MailBox           *theMailBox
               , const std::string &theName
               );

    //-------------------------------------------------------------------------
    //     ~RequestList()
    //-------------------------------------------------------------------------
    /** Destructor
     */
    virtual ~RequestList();

  protected:
    //-------------------------------------------------------------------------
    //     execInternal()
    //-------------------------------------------------------------------------
    virtual bool execInternal();

  public:
    //-------------------------------------------------------------------------
    //     c-client callbacks
    //-------------------------------------------------------------------------
    virtual bool process_mm_list(char theDelim, const std::string &theMailBoxString, long theAttrib);

    //-------------------------------------------------------------------------
    //     results
    //-------------------------------------------------------------------------
    char                  getDelimiter () const { return myDelimiter ; }
    const FolderInfoCltn &getFolderCltn() const { return myFolderCltn; }

  private:
    //-------------------------------------------------------------------------
    //     attributes
    //-------------------------------------------------------------------------
    char          *myReference ;
    char          *myPattern   ;
    char           myDelimiter ;
    FolderInfoCltn myFolderCltn;

}; // class RequestList

//=============================================================================
//     class RequestFetchStructure
//=============================================================================
/** Fetch the structure of a Mail
 *
 *  calls: mail_fetchstructure()
 *
 *  According to the documentation of c-client this method should be called
 *  before RequestFetchMsgCache.
 *
 */
class RequestFetchStructure : public Request
{
    //-------------------------------------------------------------------------
    //     typedefs
    //-------------------------------------------------------------------------
    typedef Request PARENT;

  public:
    //-------------------------------------------------------------------------
    //     RequestFetchStructure()
    //-------------------------------------------------------------------------
    /** Constructor
     */
    RequestFetchStructure( unsigned long      theMsgNo
                         , MailBox           *theMailBox
                         , const std::string &theName
                         );

    //-------------------------------------------------------------------------
    //     ~RequestFetchStructure()
    //-------------------------------------------------------------------------
    /** Destructor
     */
    virtual ~RequestFetchStructure();

    //-------------------------------------------------------------------------
    //     getEnvelope()
    //-------------------------------------------------------------------------
    ENVELOPE *getEnvelope() { return myEnvelope; }

    //-------------------------------------------------------------------------
    //     getBody()
    //-------------------------------------------------------------------------
    BODY *getBody() { return myBody; }

  protected:
    //-------------------------------------------------------------------------
    //     execInternal()
    //-------------------------------------------------------------------------
    virtual bool execInternal();

  public:
    //=========================================================================
    //     process c-client callbacks
    //=========================================================================
    virtual bool process_mm_flags     (unsigned long theMsgNo);

  private:
    //-------------------------------------------------------------------------
    //     attributes
    //-------------------------------------------------------------------------
    unsigned long myMsgNo;
    ENVELOPE     *myEnvelope;
    BODY         *myBody;

}; // class RequestFetchStructure

//=============================================================================
//     class RequestFetchMsgCache
//=============================================================================
/** Fetch the MESSAGECACHE of a Mail
 *
 *  calls: mail_elt()
 *
 *  According to the documentation of c-client RequestFetchStructure should
 *  be called before this.
 *
 */
class RequestFetchMsgCache : public Request
{
    //-------------------------------------------------------------------------
    //     typedefs
    //-------------------------------------------------------------------------
    typedef Request PARENT;

  public:
    //-------------------------------------------------------------------------
    //     RequestFetchMsgCache()
    //-------------------------------------------------------------------------
    /** Constructor
     */
    RequestFetchMsgCache( long               theMsgNo
                        , MailBox           *theMailBox
                        , const std::string &theName
                        );

    //-------------------------------------------------------------------------
    //     ~RequestFetchMsgCache()
    //-------------------------------------------------------------------------
    /** Destructor
     */
    virtual ~RequestFetchMsgCache();

    //-------------------------------------------------------------------------
    //     getBody()
    //-------------------------------------------------------------------------
    MESSAGECACHE *getMsgCache() { return myMsgCache; }

  protected:
    //-------------------------------------------------------------------------
    //     execInternal()
    //-------------------------------------------------------------------------
    virtual bool execInternal();

  private:
    //-------------------------------------------------------------------------
    //     attributes
    //-------------------------------------------------------------------------
    long          myMsgNo;
    MESSAGECACHE *myMsgCache;

}; // class RequestFetchMsgCache

//=============================================================================
//     class RequestMailBase
//=============================================================================
/** Requests on Mails
 */
class RequestMailBase : public Request
{
    //-------------------------------------------------------------------------
    //     typedefs
    //-------------------------------------------------------------------------
    typedef Request PARENT;

  public:
    //-------------------------------------------------------------------------
    //     RequestMailBase()
    //-------------------------------------------------------------------------
    /** Constructor
     */
    RequestMailBase( Mail              *theMail
                   , const std::string &theName
                   );

    //-------------------------------------------------------------------------
    //     ~RequestMailBase()
    //-------------------------------------------------------------------------
    /** Destructor
     */
    virtual ~RequestMailBase();

    //-------------------------------------------------------------------------
    //     getMail()
    //-------------------------------------------------------------------------
    Mail *getMail() const { return myMail; }

    //-------------------------------------------------------------------------
    //     getMsgNo()
    //-------------------------------------------------------------------------
    virtual long getMsgNo() const;

  private:
    //-------------------------------------------------------------------------
    //     attributes
    //-------------------------------------------------------------------------
    Mail *myMail;

}; // class RequestMailBase

//=============================================================================
//     class RequestMailFlagBase
//=============================================================================
/** Requests on Mails
 */
class RequestMailFlagBase : public RequestMailBase
{
    //-------------------------------------------------------------------------
    //     typedefs
    //-------------------------------------------------------------------------
    typedef RequestMailBase PARENT;

  public:
    //-------------------------------------------------------------------------
    //     RequestMailFlagBase()
    //-------------------------------------------------------------------------
    /** Constructor
     */
    RequestMailFlagBase( Mail              *theMail
                       , MailFlag           theFlag
                       , const std::string &theName
                       );

    //-------------------------------------------------------------------------
    //     ~RequestMailFlagBase()
    //-------------------------------------------------------------------------
    /** Destructor
     */
    virtual ~RequestMailFlagBase();

  public:
    //=========================================================================
    //     process c-client callbacks
    //=========================================================================
    virtual bool process_mm_flags     (unsigned long theMsgNo);

  protected:
    //-------------------------------------------------------------------------
    //     attributes
    //-------------------------------------------------------------------------
    MailFlag myFlag;
    bool     fOK;

}; // class RequestMailFlagBase

//=============================================================================
//     class RequestMailFlagSet
//=============================================================================
/** Clears the given flag at the given mail
 *
 *  calls: mail_setflag_full()
 */
class RequestMailFlagSet : public RequestMailFlagBase
{
    //-------------------------------------------------------------------------
    //     typedefs
    //-------------------------------------------------------------------------
    typedef RequestMailFlagBase PARENT;

  public:
    //-------------------------------------------------------------------------
    //     RequestMailFlagSet()
    //-------------------------------------------------------------------------
    /** Constructor
     */
    RequestMailFlagSet( Mail              *theMail
                      , MailFlag           theFlag
                      );

    //-------------------------------------------------------------------------
    //     ~RequestMailFlagSet()
    //-------------------------------------------------------------------------
    /** Destructor
     */
    virtual ~RequestMailFlagSet();

  protected:
    //-------------------------------------------------------------------------
    //     execInternal()
    //-------------------------------------------------------------------------
    virtual bool execInternal();

}; // class RequestMailFlagSet

//=============================================================================
//     class RequestMailFlagClear
//=============================================================================
/** Clears the given flag at the given mail
 *
 *  calls: mail_clearflag_full()
 */
class RequestMailFlagClear : public RequestMailFlagBase
{
    //-------------------------------------------------------------------------
    //     typedefs
    //-------------------------------------------------------------------------
    typedef RequestMailFlagBase PARENT;

  public:
    //-------------------------------------------------------------------------
    //     RequestMailFlagClear()
    //-------------------------------------------------------------------------
    /** Constructor
     */
    RequestMailFlagClear( Mail              *theMail
                        , MailFlag           theFlag
                        );

    //-------------------------------------------------------------------------
    //     ~RequestMailFlagClear()
    //-------------------------------------------------------------------------
    /** Destructor
     */
    virtual ~RequestMailFlagClear();

  protected:
    //-------------------------------------------------------------------------
    //     execInternal()
    //-------------------------------------------------------------------------
    virtual bool execInternal();

}; // class RequestMailFlagClear

//=============================================================================
//     class RequestMailSubject
//=============================================================================
/** Requests subject of a Mail
 *
 *  calls: mail_fetchsubject()
 */
class RequestMailSubject : public RequestMailBase
{
    //-------------------------------------------------------------------------
    //     typedefs
    //-------------------------------------------------------------------------
    typedef RequestMailBase PARENT;

  public:
    //-------------------------------------------------------------------------
    //     RequestMailSubject()
    //-------------------------------------------------------------------------
    /** Constructor
     */
    RequestMailSubject(Mail *theMail);

    //-------------------------------------------------------------------------
    //     ~RequestMailSubject()
    //-------------------------------------------------------------------------
    /** Destructor
     */
    virtual ~RequestMailSubject();

    //-------------------------------------------------------------------------
    //     getSubject()
    //-------------------------------------------------------------------------
    const std::string &getSubject() const { return mySubject; }

  protected:
    //-------------------------------------------------------------------------
    //     execInternal()
    //-------------------------------------------------------------------------
    virtual bool execInternal();

  private:
    //-------------------------------------------------------------------------
    //     attributes
    //-------------------------------------------------------------------------
    std::string mySubject;

}; // class RequestMailSubject

//=============================================================================
//     class RequestMailDate
//=============================================================================
/** Requests Date of a Mail
 *
 *  calls: mail_fetchDate()
 */
class RequestMailDate : public RequestMailBase
{
    //-------------------------------------------------------------------------
    //     typedefs
    //-------------------------------------------------------------------------
    typedef RequestMailBase PARENT;

  public:
    //-------------------------------------------------------------------------
    //     RequestMailDate()
    //-------------------------------------------------------------------------
    /** Constructor
     */
    RequestMailDate(Mail *theMail);

    //-------------------------------------------------------------------------
    //     ~RequestMailDate()
    //-------------------------------------------------------------------------
    /** Destructor
     */
    virtual ~RequestMailDate();

    //-------------------------------------------------------------------------
    //     getDate()
    //-------------------------------------------------------------------------
    const std::string &getDate() const { return myDate; }

  protected:
    //-------------------------------------------------------------------------
    //     execInternal()
    //-------------------------------------------------------------------------
    virtual bool execInternal();

  private:
    //-------------------------------------------------------------------------
    //     attributes
    //-------------------------------------------------------------------------
    std::string myDate;

}; // class RequestMailDate

//=============================================================================
//     class RequestMailFrom
//=============================================================================
/** Requests From of a Mail
 *
 *  calls: mail_fetchFrom()
 */
class RequestMailFrom : public RequestMailBase
{
    //-------------------------------------------------------------------------
    //     typedefs
    //-------------------------------------------------------------------------
    typedef RequestMailBase PARENT;

  public:
    //-------------------------------------------------------------------------
    //     RequestMailFrom()
    //-------------------------------------------------------------------------
    /** Constructor
     */
    RequestMailFrom(Mail *theMail);

    //-------------------------------------------------------------------------
    //     ~RequestMailFrom()
    //-------------------------------------------------------------------------
    /** Destructor
     */
    virtual ~RequestMailFrom();

    //-------------------------------------------------------------------------
    //     getFrom()
    //-------------------------------------------------------------------------
    const std::string &getFrom() const { return myFrom; }

  protected:
    //-------------------------------------------------------------------------
    //     execInternal()
    //-------------------------------------------------------------------------
    virtual bool execInternal();

  private:
    //-------------------------------------------------------------------------
    //     attributes
    //-------------------------------------------------------------------------
    std::string myFrom;

}; // class RequestMailFrom

//=============================================================================
//     class RequestMailUID
//=============================================================================
/** Requests the uid of the mail
 *
 *  calls: mail_fetchtext_full()
 */
class RequestMailUID : public RequestMailBase
{
    //-------------------------------------------------------------------------
    //     typedefs
    //-------------------------------------------------------------------------
    typedef RequestMailBase PARENT;

  public:
    //-------------------------------------------------------------------------
    //     RequestMailUID()
    //-------------------------------------------------------------------------
    /** Constructor
     */
    RequestMailUID(Mail *theMail);

    //-------------------------------------------------------------------------
    //     ~RequestMailUID()
    //-------------------------------------------------------------------------
    /** Destructor
     */
    virtual ~RequestMailUID();

    //-------------------------------------------------------------------------
    //     getUID()
    //-------------------------------------------------------------------------
    unsigned long getUID() const { return myUID; }

  protected:
    //-------------------------------------------------------------------------
    //     execInternal()
    //-------------------------------------------------------------------------
    virtual bool execInternal();

  private:
    //-------------------------------------------------------------------------
    //     attributes
    //-------------------------------------------------------------------------
    unsigned long myUID;

}; // class RequestMailUID

//=============================================================================
//     class RequestFetchTextFull
//=============================================================================
/** Requests the full mail text
 *
 *  calls: mail_fetchtext_full()
 */
class RequestFetchTextFull : public RequestMailBase
{
    //-------------------------------------------------------------------------
    //     typedefs
    //-------------------------------------------------------------------------
    typedef RequestMailBase PARENT;

  public:
    //-------------------------------------------------------------------------
    //     RequestFetchTextFull()
    //-------------------------------------------------------------------------
    /** Constructor
     */
    RequestFetchTextFull(Mail *theMail);

    //-------------------------------------------------------------------------
    //     ~RequestFetchTextFull()
    //-------------------------------------------------------------------------
    /** Destructor
     */
    virtual ~RequestFetchTextFull();

    //-------------------------------------------------------------------------
    //     getFullText()
    //-------------------------------------------------------------------------
    const std::string &getFullText() const { return myFullText; }

  protected:
    //-------------------------------------------------------------------------
    //     execInternal()
    //-------------------------------------------------------------------------
    virtual bool execInternal();

  private:
    //-------------------------------------------------------------------------
    //     attributes
    //-------------------------------------------------------------------------
    std::string myFullText;

}; // class RequestFetchTextFull

//=============================================================================
//     class RequestBodyFull
//=============================================================================
/** Requests the full body text of a mail part
 *
 *  calls: mail_fetchbody_full()
 */
class RequestBodyFull : public RequestMailBase
{
    //-------------------------------------------------------------------------
    //     typedefs
    //-------------------------------------------------------------------------
    typedef RequestMailBase PARENT;

  public:
    //-------------------------------------------------------------------------
    //     RequestBodyFull()
    //-------------------------------------------------------------------------
    /** Constructor
     */
    RequestBodyFull(Mail *theMail, const std::string &thePartName);

    //-------------------------------------------------------------------------
    //     ~RequestBodyFull()
    //-------------------------------------------------------------------------
    /** Destructor
     */
    virtual ~RequestBodyFull();

    //-------------------------------------------------------------------------
    //     getBodyText()
    //-------------------------------------------------------------------------
    const std::string &getBodyText() const { return myBodyText; }

    //-------------------------------------------------------------------------
    //     getBodyLen()
    //-------------------------------------------------------------------------
    unsigned long getBodyLen() const { return myBodyLen; }

  protected:
    //-------------------------------------------------------------------------
    //     execInternal()
    //-------------------------------------------------------------------------
    virtual bool execInternal();

  private:
    //-------------------------------------------------------------------------
    //     attributes
    //-------------------------------------------------------------------------
    std::string   myPartName;
    std::string   myBodyText;
    unsigned long myBodyLen;

}; // class RequestBodyFull

//=============================================================================
//     namespaces
//=============================================================================
} // Mail
} // Ax

#endif
