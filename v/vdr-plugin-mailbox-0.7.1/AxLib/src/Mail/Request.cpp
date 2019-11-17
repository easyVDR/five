//=============================================================================
/*
 * Description: This file contains the class Ax::Mail::Request
 *
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
#include <algorithm>

//----- C ---------------------------------------------------------------------
#include <stdio.h>

//----- AxLib -----------------------------------------------------------------
#include <Ax/Tools/Macros.h>
#include <Ax/Tools/Trace.h>
// #include <Ax/Tools/TraceOn.h>
#include <Ax/Tools/String.h>
#include <Ax/Mail/MailBoxMgr.h>
#include <Ax/Mail/MailBox.h>
#include <Ax/Mail/MailBoxSetup.h>
#include <Ax/Mail/MailFolder.h>
#include <Ax/Mail/Mail.h>

//----- vdr -------------------------------------------------------------------
//----- local plugin ----------------------------------------------------------
//----- locals ----------------------------------------------------------------
#include "Request.h"
#include "MailBoxInternal.h"

//=============================================================================
//     using namespaces
//=============================================================================
using namespace std;

//=============================================================================
//     namespaces
//=============================================================================
namespace Ax   {
namespace Mail {

//=============================================================================
//     class Request
//=============================================================================

//-------------------------------------------------------------------------
//     Request::Request()
//-------------------------------------------------------------------------
Request::Request(MailBox *theMailBox, const std::string &theName)
       : PARENT (theMailBox, theName, theMailBox ? theMailBox->getInternal().getCBP() : 0)
{
} // Request::Request()

//-------------------------------------------------------------------------
//     Request::~Request()
//-------------------------------------------------------------------------
Request::~Request()
{
} // Request::~Request()

//-------------------------------------------------------------------------
//     Request::exec()
//-------------------------------------------------------------------------
bool Request::exec()
{
#if 1
  log(LL_DEBUG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
  log(LL_DEBUG, Ax::Tools::String::sprintf( "> Request %s"
                                          , getName().c_str()
                                          ));
#endif

  setIsOK(true);
  bool fOK = true;
  { // lock pending request
    CallBackLock aLock(this);

    fOK = execInternal();
  } // unlock pending request

  setIsOK(isOK() && fOK);

  if (!isOK())
  {
    log(LL_WARN, Ax::Tools::String::sprintf("request failed: '%s'", getName().c_str()));
  } // if

#if 1
  log(LL_DEBUG, Ax::Tools::String::sprintf( "< Request %s  %s"
                                          , getName().c_str()
                                          , isOK() ? "SUCCESS" : "FAILED"
                                          ));
  log(LL_DEBUG, "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
#endif

  return isOK();
} // Request::exec()

#if 0
//=============================================================================
//     class RequestLock
//=============================================================================

//-------------------------------------------------------------------------
//     RequestLock::RequestLock()
//-------------------------------------------------------------------------
RequestLock::RequestLock( MailBox           *theMailBox
                        , const std::string &theName
                        )
           : PARENT(theMailBox, theName)
           , myLock(this)
{
} // RequestLock::RequestLock()

//-------------------------------------------------------------------------
//     RequestLock::~RequestLock()
//-------------------------------------------------------------------------
RequestLock::~RequestLock()
{
} // RequestLock::~RequestLock()

//-------------------------------------------------------------------------
//     RequestLock::execInternal()
//-------------------------------------------------------------------------
bool RequestLock::execInternal()
{
  return true;
} // RequestLock::execInternal()
#endif

//=============================================================================
//     class RequestOpen
//=============================================================================

//-------------------------------------------------------------------------
//     RequestOpen::RequestOpen()
//-------------------------------------------------------------------------
RequestOpen::RequestOpen( const std::string &theMailBoxString
                        , MailBox           *theMailBox
                        , const std::string &theName
                        )
           : PARENT         (theMailBox, theName)
           , myMailBoxString(0)
           , myNumMails     (0)
{
  myMailBoxString = strdup(theMailBoxString.c_str());
} // RequestOpen::RequestOpen()

//-------------------------------------------------------------------------
//     RequestOpen::~RequestOpen()
//-------------------------------------------------------------------------
RequestOpen::~RequestOpen()
{
  free(myMailBoxString);
} // RequestOpen::~RequestOpen()

//-------------------------------------------------------------------------
//     RequestOpen::execInternal()
//-------------------------------------------------------------------------
bool RequestOpen::execInternal()
{
  wsdebug(("> RequestOpen::execInternal() MailBoxString: %s", myMailBoxString));

  MAILSTREAM *aStreamBefore = getStream();
  MAILSTREAM *aStreamAfter  = mail_open(aStreamBefore, myMailBoxString, NIL);
  getMailBox()->getInternal().setStream(aStreamAfter);

  wsdebug(("< RequestOpen::execInternal() MailBoxString: %s  stream: 0x%p", myMailBoxString, getStream()));
  return getMailBox()->isConnected();
} // RequestOpen::execInternal()

//-------------------------------------------------------------------------
bool RequestOpen::process_mm_exists(unsigned long number)
{
  wsdebug(("- RequestOpen::process_mm_exists() theNumMails: %ld", number));
  myNumMails = number;
  return true;
} // RequestOpen::process_mm_exists()

//=============================================================================
//     class RequestClose
//=============================================================================

//-------------------------------------------------------------------------
//     RequestClose::RequestClose()
//-------------------------------------------------------------------------
RequestClose::RequestClose( MailBox           *theMailBox
                          , const std::string &theName
                          )
            : PARENT(theMailBox, theName)
{
} // RequestClose::RequestClose()

//-------------------------------------------------------------------------
//     RequestClose::~RequestClose()
//-------------------------------------------------------------------------
RequestClose::~RequestClose()
{
} // RequestClose::~RequestClose()

//-------------------------------------------------------------------------
//     RequestClose::execInternal()
//-------------------------------------------------------------------------
bool RequestClose::execInternal()
{
  wsdebug(("> RequestClose::execInternal() stream: 0x%p", getStream()));

  if (getStream())
  {
    MAILSTREAM *aStreamBefore = getStream();
    MAILSTREAM *aStreamAfter  = mail_close_full( aStreamBefore
                                               , getMailBox()->getSetup().getExpungeOnClose() ? CL_EXPUNGE : 0
                                               );
    getMailBox()->getInternal().setStream(aStreamAfter);

    wsdebug(("  RequestClose::execInternal() Stream before: %08X  after: %08X"
           , int(aStreamBefore), int(aStreamAfter)
           ));
  }
  else
  {
    wsdebug(("  RequestClose::execInternal() stream == 0 -> already close!"));
  } // if

  wsdebug(("< RequestClose::execInternal() stream: 0x%p", getStream()));
  return true;
} // RequestClose::execInternal()

//=============================================================================
//     class RequestStatus
//=============================================================================

//-------------------------------------------------------------------------
//     RequestStatus::RequestStatus()
//-------------------------------------------------------------------------
RequestStatus::RequestStatus( const std::string &theMailBoxString
                            , MailBox           *theMailBox
                            , const std::string &theName
                            )
           : PARENT         (theMailBox, theName)
           , myMailBoxString(0)
           , myHasStatus    (false  )
{
  myMailBoxString = strdup(theMailBoxString.c_str());

  myStatus.messages    = 0;
  myStatus.recent      = 0;
  myStatus.unseen      = 0;
  myStatus.uidnext     = 0;
  myStatus.uidvalidity = 0;
} // RequestStatus::RequestStatus()

//-------------------------------------------------------------------------
//     RequestStatus::~RequestStatus()
//-------------------------------------------------------------------------
RequestStatus::~RequestStatus()
{
  free(myMailBoxString);
} // RequestStatus::~RequestStatus()

//-------------------------------------------------------------------------
//     RequestStatus::execInternal()
//-------------------------------------------------------------------------
bool RequestStatus::execInternal()
{
  wsdebug(("> RequestStatus::execInternal() MailBoxString: %s", myMailBoxString));

  myHasStatus = false;
  bool fOK = mail_status( getStream()
                        , myMailBoxString
                        , SA_MESSAGES | SA_UNSEEN | SA_RECENT | SA_UIDNEXT | SA_UIDVALIDITY
                        ) == T
          && myHasStatus;


  wsdebug(("< RequestStatus::execInternal() MailBoxString: %s  isOK: %d", myMailBoxString, fOK));
  return fOK;
} // RequestStatus::execInternal()

//-------------------------------------------------------------------------
//     RequestStatus::process_mm_status()
//-------------------------------------------------------------------------
bool RequestStatus::process_mm_status(const std::string &theMailBoxString, const MAILSTATUS *theStatus)
{
  AX_UNUSED_ARG(theMailBoxString);

  myHasStatus = true;
  memcpy(&myStatus, theStatus, sizeof(MAILSTATUS));

  wsdebug(("- RequestStatus::process_mm_status(): mailbox: '%s'  total: %lu  unseen: %lu  recent: %lu  uidnext: %lu  uidvalidity: %lu"
         , theMailBoxString.c_str()
         , theStatus->messages
         , theStatus->unseen
         , theStatus->recent
         , theStatus->uidnext
         , theStatus->uidvalidity
         ));

  return true;
} // RequestStatus::process_mm_status()

//=============================================================================
//     class RequestList
//=============================================================================

//-------------------------------------------------------------------------
//     RequestList::RequestList()
//-------------------------------------------------------------------------
RequestList::RequestList( const std::string &theReference
                        , const std::string &thePattern
                        , MailBox           *theMailBox
                        , const std::string &theName
                        )
           : PARENT      (theMailBox, theName)
           , myReference (0)
           , myPattern   (0)
           , myDelimiter ('\0')
{
  myReference = strdup(theReference.c_str());
  myPattern   = strdup(thePattern  .c_str());
} // RequestList::RequestList()

//-------------------------------------------------------------------------
//     RequestList::~RequestList()
//-------------------------------------------------------------------------
RequestList::~RequestList()
{
  free(myReference);
  free(myPattern  );
} // RequestList::~RequestList()

//-------------------------------------------------------------------------
//     binary predicare to compare FolderInfo-instances
//-------------------------------------------------------------------------
bool FolderInfoLessThan(const FolderInfo &f1, const FolderInfo &f2)
{
  return f1.Name < f2.Name;
} // FolderInfoLessThan()

//-------------------------------------------------------------------------
//     RequestList::execInternal()
//-------------------------------------------------------------------------
bool RequestList::execInternal()
{
  wsdebug(("> RequestList::execInternal() Ref: '%s'  Pat: '%s'", myReference, myPattern));

  myFolderCltn.clear();

  mail_list(getStream(), myReference, myPattern);

  std::sort(myFolderCltn.begin(), myFolderCltn.end(), FolderInfoLessThan);

  wsdebug(("< RequestList::execInternal() Ref: '%s'  Pat: '%s'", myReference, myPattern));
  return true;
} // RequestList::execInternal()

//-------------------------------------------------------------------------
//     RequestList::process_mm_list()
//-------------------------------------------------------------------------
bool RequestList::process_mm_list(char theDelim, const std::string &theMailBoxString, long theAttrib)
{
  wsdebug(("- RequestList::process_mm_list(): theDelim: %c = %3d  Attr: %04lX  %s"
         , theDelim
         , theDelim
         , theAttrib
         , theMailBoxString.c_str()
         ));

  myDelimiter = theDelim;
  myFolderCltn.push_back(FolderInfo(theMailBoxString, theAttrib));
  return true;
} // RequestList::process_mm_list()

//=============================================================================
//     class RequestFetchStructure
//=============================================================================

//-------------------------------------------------------------------------
//     RequestFetchStructure::RequestFetchStructure()
//-------------------------------------------------------------------------
RequestFetchStructure::RequestFetchStructure( unsigned long      theMsgNo
                                            , MailBox           *theMailBox
                                            , const std::string &theName
                                            )
           : PARENT         (theMailBox, theName)
           , myMsgNo        (theMsgNo)
           , myEnvelope     (0)
           , myBody         (0)
{
} // RequestFetchStructure::RequestFetchStructure()

//-------------------------------------------------------------------------
//     RequestFetchStructure::~RequestFetchStructure()
//-------------------------------------------------------------------------
RequestFetchStructure::~RequestFetchStructure()
{
} // RequestFetchStructure::~RequestFetchStructure()

//-------------------------------------------------------------------------
//     RequestFetchStructure::execInternal()
//-------------------------------------------------------------------------
bool RequestFetchStructure::execInternal()
{
  wsdebug(("> RequestFetchStructure::execInternal() MsgNo: %ld", myMsgNo));

  myBody     = 0;
  myEnvelope = mail_fetchstructure(getStream(), myMsgNo, &myBody);

  wsdebug(("< RequestFetchStructure::execInternal() MsgNo: %ld", myMsgNo));
  return myBody != 0;
} // RequestFetchStructure::execInternal()

//-------------------------------------------------------------------------
//     RequestFetchStructure::process_mm_flags()
//-------------------------------------------------------------------------
bool RequestFetchStructure::process_mm_flags(unsigned long theMsgNum)
{
  bool fOK = theMsgNum == myMsgNo;

  wsdebug(("- RequestFetchStructure::process_mm_flags() MsgNum: %lu  fOK: %d", theMsgNum, fOK));
  return fOK;
} // RequestFetchStructure::process_mm_flags()

//=============================================================================
//     class RequestFetchMsgCache
//=============================================================================

//-------------------------------------------------------------------------
//     RequestFetchMsgCache::RequestFetchMsgCache()
//-------------------------------------------------------------------------
RequestFetchMsgCache::RequestFetchMsgCache( long               theMsgNo
                                          , MailBox           *theMailBox
                                          , const std::string &theName
                                          )
           : PARENT         (theMailBox, theName)
           , myMsgNo        (theMsgNo)
           , myMsgCache     (0)
{
} // RequestFetchMsgCache::RequestFetchMsgCache()

//-------------------------------------------------------------------------
//     RequestFetchMsgCache::~RequestFetchMsgCache()
//-------------------------------------------------------------------------
RequestFetchMsgCache::~RequestFetchMsgCache()
{
} // RequestFetchMsgCache::~RequestFetchMsgCache()

//-------------------------------------------------------------------------
//     RequestFetchMsgCache::execInternal()
//-------------------------------------------------------------------------
bool RequestFetchMsgCache::execInternal()
{
  wsdebug(("> RequestFetchMsgCache::execInternal() MsgNo: %ld", myMsgNo));

  myMsgCache = mail_elt(getStream(), myMsgNo);

  wsdebug(("< RequestFetchMsgCache::execInternal() MsgNo: %ld", myMsgNo));
  return myMsgCache != 0;
} // RequestFetchMsgCache::execInternal()

//=============================================================================
//     class RequestMailBase
//=============================================================================

//-------------------------------------------------------------------------
//     RequestMailBase::RequestMailBase()
//-------------------------------------------------------------------------
RequestMailBase::RequestMailBase( Mail              *theMail
                                , const std::string &theName
                                )
               : PARENT         (theMail->getMailBox(), theName)
               , myMail         (theMail)
{
} // RequestMailBase::RequestMailBase()

//-------------------------------------------------------------------------
//     RequestMailBase::~RequestMailBase()
//-------------------------------------------------------------------------
RequestMailBase::~RequestMailBase()
{
} // RequestMailBase::~RequestMailBase()

//-------------------------------------------------------------------------
//     RequestMailBase::getMsgNo()
//-------------------------------------------------------------------------
long RequestMailBase::getMsgNo() const
{
  return myMail ? myMail->getMsgNo() : 0;
} // RequestMailBase::getMsgNo()

//=============================================================================
//     class RequestMailFlagBase
//=============================================================================

//-------------------------------------------------------------------------
//     RequestMailFlagBase::RequestMailFlagBase()
//-------------------------------------------------------------------------
RequestMailFlagBase::RequestMailFlagBase( Mail              *theMail
                                      , MailFlag           theFlag
                                      , const std::string &theName
                                      )
                  : PARENT(theMail, theName)
                  , myFlag(theFlag)
                  , fOK   (false)
{
} // RequestMailFlagBase::RequestMailFlagBase()

//-------------------------------------------------------------------------
//     RequestMailFlagBase::~RequestMailFlagBase()
//-------------------------------------------------------------------------
RequestMailFlagBase::~RequestMailFlagBase()
{
} // RequestMailFlagBase::~RequestMailFlagBase()

//-------------------------------------------------------------------------
//     RequestMailFlagBase::process_mm_flags()
//-------------------------------------------------------------------------
bool RequestMailFlagBase::process_mm_flags(unsigned long theMsgNo)
{
  bool fOK = long(theMsgNo) == long(getMsgNo());

  wsdebug(("- RequestMailFlagBase::process_mm_flags() MsgNo: %lu  fOK: %d", theMsgNo, fOK));
  return fOK;
} // RequestMailFlagBase::process_mm_flags()

//=============================================================================
//     class RequestMailFlagSet
//=============================================================================

//-------------------------------------------------------------------------
//     RequestMailFlagSet::RequestMailFlagSet()
//-------------------------------------------------------------------------
RequestMailFlagSet::RequestMailFlagSet( Mail     *theMail
                                      , MailFlag  theFlag
                                      )
                  : PARENT(theMail, theFlag, "RequestMailFlagSet")
{
} // RequestMailFlagSet::RequestMailFlagSet()

//-------------------------------------------------------------------------
//     RequestMailFlagSet::~RequestMailFlagSet()
//-------------------------------------------------------------------------
RequestMailFlagSet::~RequestMailFlagSet()
{
} // RequestMailFlagSet::~RequestMailFlagSet()

//-------------------------------------------------------------------------
//     RequestMailFlagSet::execInternal()
//-------------------------------------------------------------------------
bool RequestMailFlagSet::execInternal()
{
  std::string aFlagStr = Mail::getFlagStr(myFlag);

  wsdebug(("> RequestMailFlagSet::execInternal() setting flag %s for mail %ld in mailbox '%s'"
         , aFlagStr.c_str(), getMsgNo(), getMail()->getFolder()->getMailBoxString().c_str()
         ));

  char szMailNum[20];
  sprintf(szMailNum, "%ld", getMsgNo());

  mail_setflag_full( getStream()
                   , szMailNum
                   , const_cast<char *>(aFlagStr.c_str())
                   , 0
                   );

  wsdebug(("< RequestMailFlagSet::execInternal() setting flag %s for mail %ld in mailbox '%s'"
         , aFlagStr.c_str(), getMsgNo(), getMail()->getFolder()->getMailBoxString().c_str()
         ));
  return fOK;
} // RequestMailFlagSet::execInternal()

//=============================================================================
//     class RequestMailFlagClear
//=============================================================================

//-------------------------------------------------------------------------
//     RequestMailFlagClear::RequestMailFlagClear()
//-------------------------------------------------------------------------
RequestMailFlagClear::RequestMailFlagClear( Mail     *theMail
                                          , MailFlag  theFlag
                                          )
                  : PARENT(theMail, theFlag, "RequestMailFlagClear")
{
} // RequestMailFlagClear::RequestMailFlagClear()

//-------------------------------------------------------------------------
//     RequestMailFlagClear::~RequestMailFlagClear()
//-------------------------------------------------------------------------
RequestMailFlagClear::~RequestMailFlagClear()
{
} // RequestMailFlagClear::~RequestMailFlagClear()

//-------------------------------------------------------------------------
//     RequestMailFlagClear::execInternal()
//-------------------------------------------------------------------------
bool RequestMailFlagClear::execInternal()
{
  std::string aFlagStr = Mail::getFlagStr(myFlag);

  wsdebug(("> RequestMailFlagClear::execInternal() clearing flag %s for mail %ld in mailbox '%s'"
         , aFlagStr.c_str(), getMsgNo(), getMail()->getFolder()->getMailBoxString().c_str()
         ));

  char szMailNum[20];
  sprintf(szMailNum, "%ld", getMsgNo());

  mail_clearflag_full( getStream()
                     , szMailNum
                     , const_cast<char *>(aFlagStr.c_str())
                     , 0
                     );

  wsdebug(("< RequestMailFlagClear::execInternal() clearing flag %s for mail %ld in mailbox '%s'"
         , aFlagStr.c_str(), getMsgNo(), getMail()->getFolder()->getMailBoxString().c_str()
         ));

  return fOK;
} // RequestMailFlagClear::execInternal()

//=============================================================================
//     class RequestMailSubject
//=============================================================================

//-------------------------------------------------------------------------
//     RequestMailSubject::RequestMailSubject()
//-------------------------------------------------------------------------
RequestMailSubject::RequestMailSubject(Mail *theMail)
                  : PARENT(theMail, "RequestMailSubject")
                  , mySubject("")
{
} // RequestMailSubject::RequestMailSubject()

//-------------------------------------------------------------------------
//     RequestMailSubject::~RequestMailSubject()
//-------------------------------------------------------------------------
RequestMailSubject::~RequestMailSubject()
{
} // RequestMailSubject::~RequestMailSubject()

//-------------------------------------------------------------------------
//     RequestMailSubject::execInternal()
//-------------------------------------------------------------------------
bool RequestMailSubject::execInternal()
{
  char szText[1024];
  mail_fetchsubject(szText, getStream(), getMsgNo(), sizeof(szText) - 1);
  szText[sizeof(szText) - 1] = '\0';

  mySubject = std::string((const char *)szText);

  return true;
} // RequestMailSubject::execInternal()

//=============================================================================
//     class RequestMailDate
//=============================================================================

//-------------------------------------------------------------------------
//     RequestMailDate::RequestMailDate()
//-------------------------------------------------------------------------
RequestMailDate::RequestMailDate(Mail *theMail)
               : PARENT(theMail, "RequestMailDate")
               , myDate("")
{
} // RequestMailDate::RequestMailDate()

//-------------------------------------------------------------------------
//     RequestMailDate::~RequestMailDate()
//-------------------------------------------------------------------------
RequestMailDate::~RequestMailDate()
{
} // RequestMailDate::~RequestMailDate()

//-------------------------------------------------------------------------
//     RequestMailDate::execInternal()
//-------------------------------------------------------------------------
bool RequestMailDate::execInternal()
{
  bool fOK = false;

  return fOK;
} // RequestMailDate::execInternal()

//=============================================================================
//     class RequestMailFrom
//=============================================================================

//-------------------------------------------------------------------------
//     RequestMailFrom::RequestMailFrom()
//-------------------------------------------------------------------------
RequestMailFrom::RequestMailFrom(Mail *theMail)
               : PARENT(theMail, "RequestMailFrom")
               , myFrom("")
{
} // RequestMailFrom::RequestMailFrom()

//-------------------------------------------------------------------------
//     RequestMailFrom::~RequestMailFrom()
//-------------------------------------------------------------------------
RequestMailFrom::~RequestMailFrom()
{
} // RequestMailFrom::~RequestMailFrom()

//-------------------------------------------------------------------------
//     RequestMailFrom::execInternal()
//-------------------------------------------------------------------------
bool RequestMailFrom::execInternal()
{
  char szText[256];
  mail_fetchfrom(szText, getStream(), getMsgNo(), sizeof(szText) - 1);

  // since c-client-libaray fills the string up to the length
  // with spaces we have to strip them away
  for ( char *p = szText + sizeof(szText) - 1;
        p > szText && (*p == ' ' || *p == '\0');
        p--)
  {
    *p = '\0';
  } // while

  myFrom = std::string((const char *)szText);

  return true;
} // RequestMailFrom::execInternal()

//=============================================================================
//     class RequestMailUID
//=============================================================================

//-------------------------------------------------------------------------
//     RequestMailUID::RequestMailUID()
//-------------------------------------------------------------------------
RequestMailUID::RequestMailUID(Mail *theMail)
                    : PARENT(theMail, "RequestMailUID")
                    , myUID(0)
{
} // RequestMailUID::RequestMailUID()

//-------------------------------------------------------------------------
//     RequestMailUID::~RequestMailUID()
//-------------------------------------------------------------------------
RequestMailUID::~RequestMailUID()
{
} // RequestMailUID::~RequestMailUID()

//-------------------------------------------------------------------------
//     RequestMailUID::execInternal()
//-------------------------------------------------------------------------
bool RequestMailUID::execInternal()
{
  myUID = mail_uid(getStream(), getMsgNo());

  return true;
} // RequestMailUID::execInternal()


//=============================================================================
//     class RequestFetchTextFull
//=============================================================================

//-------------------------------------------------------------------------
//     RequestFetchTextFull::RequestFetchTextFull()
//-------------------------------------------------------------------------
RequestFetchTextFull::RequestFetchTextFull(Mail *theMail)
                    : PARENT(theMail, "RequestFetchTextFull")
                    , myFullText("")
{
} // RequestFetchTextFull::RequestFetchTextFull()

//-------------------------------------------------------------------------
//     RequestFetchTextFull::~RequestFetchTextFull()
//-------------------------------------------------------------------------
RequestFetchTextFull::~RequestFetchTextFull()
{
} // RequestFetchTextFull::~RequestFetchTextFull()

//-------------------------------------------------------------------------
//     RequestFetchTextFull::execInternal()
//-------------------------------------------------------------------------
bool RequestFetchTextFull::execInternal()
{
  unsigned long aLen      = 0;
  long          aFlags    = (!getMailBox()->getSetup().getAutoMarkSeen()) ? FT_PEEK : 0;

  char *aMailBody = mail_fetchtext_full( getStream()
                                       , getMsgNo()
                                       , &aLen
                                       , aFlags
                                       );

  if (aMailBody)
  {
    myFullText = std::string((const char *)aMailBody);
  } // if

  return aMailBody != 0;
} // RequestFetchTextFull::execInternal()

//=============================================================================
//     class RequestBodyFull
//=============================================================================

//-------------------------------------------------------------------------
//     RequestBodyFull::RequestBodyFull()
//-------------------------------------------------------------------------
RequestBodyFull::RequestBodyFull(Mail *theMail, const std::string &thePartName)
               : PARENT(theMail, "RequestBodyFull")
               , myPartName(thePartName)
               , myBodyText("")
               , myBodyLen (0)
{
} // RequestBodyFull::RequestBodyFull()

//-------------------------------------------------------------------------
//     RequestBodyFull::~RequestBodyFull()
//-------------------------------------------------------------------------
RequestBodyFull::~RequestBodyFull()
{
} // RequestBodyFull::~RequestBodyFull()

//-------------------------------------------------------------------------
//     RequestBodyFull::execInternal()
//-------------------------------------------------------------------------
bool RequestBodyFull::execInternal()
{
  long  aFlags     = (!getMailBox()->getSetup().getAutoMarkSeen()) ? FT_PEEK : 0;
  char  *aPartName = 0;
  char  *aBodyText = 0;

  myBodyText = "";

  int aRet = asprintf(&aPartName, "%s", myPartName.c_str());
  if (aRet >= 0)
  {
    aBodyText = mail_fetchbody_full( getStream()
                                   , getMsgNo()
                                   , aPartName
                                   , &myBodyLen
                                   , aFlags
                                   );
    free(aPartName);

    if (aBodyText != 0)
    {
      myBodyText = std::string(aBodyText);
    } // if
  }
  else
  {
    wserror(("! RequestBodyFull::execInternal() error in asprintf!"));
  } // if

  return aBodyText != 0;
} // RequestBodyFull::execInternal()


//=============================================================================
//     namespaces
//=============================================================================
} // Mail
} // Ax
