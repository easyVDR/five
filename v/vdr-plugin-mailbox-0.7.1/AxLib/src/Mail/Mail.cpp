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
//----- AxLib -----------------------------------------------------------------
#include <Ax/Tools/Trace.h>
// #include <Ax/Tools/TraceOn.h>
#include <Ax/Mail/Mail.h>
#include <Ax/Mail/MailPart.h>
#include <Ax/Mail/MailBox.h>
#include <Ax/Mail/MailFolder.h>
#include <Ax/Mail/MailBoxMgr.h>

//----- vdr -------------------------------------------------------------------
//----- local plugin ----------------------------------------------------------
//----- locals ----------------------------------------------------------------
#include "Internal.h"
#include "MsgBody.h"
#include "MsgCache.h"
#include "Request.h"

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
//     class Mail
//=============================================================================

//-------------------------------------------------------------------------
//     Mail::Mail()
//-------------------------------------------------------------------------
Mail::Mail(MailFolder *theMailFolder, const MsgCache &theMsgCache)
      : myMailFolder (theMailFolder)
      , myRootPart   (0)
      , myMsgCache   (new MsgCache(theMsgCache))
      , myUID        (0)
      , mySubject    ("")
      , myFrom       ("")
      , myDate       ("")
{
  wsdebug(("> Mail::Mail() MsgCache: %p  MsgNo: %d  LockCount: %d"
         , *myMsgCache
         , !myMsgCache->isNull() ? int((*myMsgCache)->msgno    ) : -1
         , !myMsgCache->isNull() ? int((*myMsgCache)->lockcount) : -1
         ));

  wsdebug(("< Mail::Mail() MsgCache: %p  MsgNo: %d  LockCount: %d"
         , *myMsgCache
         , !myMsgCache->isNull() ? int((*myMsgCache)->msgno    ) : -1
         , !myMsgCache->isNull() ? int((*myMsgCache)->lockcount) : -1
         ));
} // Mail::Mail()

//-------------------------------------------------------------------------
//     Mail::~Mail()
//-------------------------------------------------------------------------
Mail::~Mail()
{
  wsdebug(("> Mail::~Mail() MsgCache: %p  MsgNo: %d  LockCount: %d"
         , *myMsgCache
         , !myMsgCache->isNull() ? int((*myMsgCache)->msgno    ) : -1
         , !myMsgCache->isNull() ? int((*myMsgCache)->lockcount) : -1
         ));

  delete myMsgCache; myMsgCache = 0;
  delete myRootPart; myRootPart = 0;

  wsdebug(("< Mail::~Mail()"));
} // Mail::~Mail()

//-------------------------------------------------------------------------
//     Mail::getFolder()
//-------------------------------------------------------------------------
MailFolder *Mail::getFolder() const
{
  return myMailFolder;
} // Mail::getFolder()

//-------------------------------------------------------------------------
//     Mail::getMailBox()
//-------------------------------------------------------------------------
MailBox *Mail::getMailBox() const
{
  return getFolder()->getMailBox();
} // Mail::getMailBox()

//-------------------------------------------------------------------------
//     Mail::requestHeader()
//-------------------------------------------------------------------------
bool Mail::requestHeader()
{
  if (myMsgCache->isNull())
  {
    wswarn(("- Mail::requestHeader() myMsgCache == 0"));
    return false;
  } // if

  wsdebug(("> Mail::requestHeader() MsgNo: %ld", getMsgNo()));
  bool fOK = true;

  //----- request 'Date' -----
  if (fOK && myDate.length() == 0)
  {
    RequestFetchStructure aReqStructure(getMsgNo(), getMailBox(), "FetchStructure");

    fOK = aReqStructure.exec();
    if (!fOK)
    {
      wswarn(("- Mail::requestHeader() MsgNo: %ld: error getting structure", getMsgNo()));
    }
    else if (aReqStructure.getEnvelope() && aReqStructure.getEnvelope()->date)
    {
      myDate = std::string((const char *)(aReqStructure.getEnvelope()->date));
    } // if
  } // if

  //----- request 'From' line -----
  if (fOK && myFrom.length() == 0)
  {
    RequestMailFrom aReq(this);
    fOK = aReq.exec();
    if (!fOK)
    {
      wswarn(("- Mail::requestHeader() MsgNo: %ld: error getting 'From'", getMsgNo()));
      myFrom = "";
    }
    else
    {
      myFrom = aReq.getFrom();
    } // if
  } // if

  //----- request 'Subject' line -----
  fOK = mySubject.length() == 0;
  if (fOK)
  {
    RequestMailSubject aReq(this);
    fOK = aReq.exec();
    if (!fOK)
    {
      wswarn(("- Mail::getSubject() MsgNo: %ld: error getting 'Subject'", getMsgNo()));
    }
    else
    {
      mySubject = aReq.getSubject();
    } // if
  } // if

  wsdebug(("< Mail::requestHeader() MsgNo: %ld  fOK: %d", getMsgNo(), fOK));
  return fOK;
} // Mail::requestHeader()

//-------------------------------------------------------------------------
//     Mail::getRootPart()
//-------------------------------------------------------------------------
MailPart *Mail::getRootPart()
{
  if (myRootPart == 0)
  {
    wsinfo(("retrieving mail-structure %ld from mailbox '%s'..."
          , getMsgNo()
          , getFolder()->getMailBoxString().c_str()
          ));

    wsdebug(("> Mail::getRootPart() MsgNo: %ld", getMsgNo()));

    RequestFetchStructure aReq(getMsgNo(), getMailBox(), getFolder()->getMailBoxString() + " -> FetchStructure()");
    if (aReq.exec())
    {
      myRootPart = new MailPart(  this
                               ,  0        // no parent
                               ,  MsgBody(aReq.getBody())
                               , (aReq.getBody()->type == TYPEMULTIPART) ? "" : "1"
                               );

      wsinfo(("retrieving mail-structure %ld from mailbox '%s'...OK"
            , getMsgNo(), getFolder()->getMailBoxString().c_str()
            ));
    }
    else
    {
      wserror(("retrieving mail-structure %ld from mailbox '%s'...FAILED"
            , getMsgNo(), getFolder()->getMailBoxString().c_str()
            ));
    } // if

    wsdebug(("< Mail::getRootPart() MsgNo: %ld", getMsgNo()));
  } // if
  return myRootPart;
} // Mail::getRootPart()

//-------------------------------------------------------------------------
//     Mail::getMsgNo()
//-------------------------------------------------------------------------
long Mail::getMsgNo() const
{
  return !myMsgCache->isNull() ? (*myMsgCache)->msgno : 0;
} // Mail::getMsgNo()

//-------------------------------------------------------------------------
//     Mail::hasValidFlags()
//-------------------------------------------------------------------------
bool Mail::hasValidFlags() const
{
  return !myMsgCache->isNull() ? (*myMsgCache)->valid : false;
} // Mail::hasValidFlags()

//-------------------------------------------------------------------------
//     Mail::getIsRecent()
//-------------------------------------------------------------------------
bool Mail::getIsRecent() const
{
  return !myMsgCache->isNull() ? (*myMsgCache)->recent : false;
} // Mail::getIsRecent()

//-------------------------------------------------------------------------
//     Mail::getIsSeen()
//-------------------------------------------------------------------------
bool Mail::getIsSeen() const
{
  return !myMsgCache->isNull() ? (*myMsgCache)->seen : false;
} // Mail::getIsSeen()

//-------------------------------------------------------------------------
//     Mail::getIsFlagged()
//-------------------------------------------------------------------------
bool Mail::getIsFlagged() const
{
  return !myMsgCache->isNull() ? (*myMsgCache)->flagged : false;
} // Mail::getIsFlagged()

//-------------------------------------------------------------------------
//     Mail::getIsAnswered()
//-------------------------------------------------------------------------
bool Mail::getIsAnswered() const
{
  return !myMsgCache->isNull() ? (*myMsgCache)->answered : false;
} // Mail::getIsAnswered()

//-------------------------------------------------------------------------
//     Mail::getIsDeleted()
//-------------------------------------------------------------------------
bool Mail::getIsDeleted() const
{
  return !myMsgCache->isNull() ? (*myMsgCache)->deleted : false;
} // Mail::getIsDeleted()

//-------------------------------------------------------------------------
//     Mail::getBodySize()
//-------------------------------------------------------------------------
long Mail::getBodySize() const
{
  return !myMsgCache->isNull() ? (*myMsgCache)->rfc822_size: 0;
} // Mail::getBodySize()

//-------------------------------------------------------------------------
//     Mail::getUID()
//-------------------------------------------------------------------------
unsigned long Mail::getUID() const
{
  if (myMsgCache->isNull())
  {
    return 0;
  } // if

  RequestMailUID aReq(const_cast<Mail *>(this));
  aReq.exec();
  return aReq.getUID();
} // Mail::getUID()

//-------------------------------------------------------------------------
//     Mail::getFullText()
//-------------------------------------------------------------------------
std::string Mail::getFullText() const
{
  if (myMsgCache->isNull())
  {
    wswarn(("- Mail::getFullText() MsgNo: %3ld  MsgCache == 0 -> nothing to do", getMsgNo()));
    return "";
  } // if

  RequestFetchTextFull aReq(const_cast<Mail *>(this));
  aReq.exec();

  return aReq.getFullText();
} // Mail::getFullText()

//-------------------------------------------------------------------------
//     Mail::getFlagStr()
//-------------------------------------------------------------------------
std::string Mail::getFlagStr(MailFlag theFlag)
{
  switch (theFlag)
  {
    case Ax::Mail::MF_SEEN    : return "\\Seen"    ;
    case Ax::Mail::MF_FLAGGED : return "\\Flagged" ;
    case Ax::Mail::MF_DELETED : return "\\Deleted" ;
    case Ax::Mail::MF_ANSWERED: return "\\Answered";
  } // switch
  return "";
} // Mail::getFlagStr()

//-------------------------------------------------------------------------
//     Mail::setFlag()
//-------------------------------------------------------------------------
void Mail::setFlag(MailFlag theFlag)
{
  if (myMsgCache->isNull())
  {
    wswarn(("- Mail::setFlag() MsgNo: %3ld  MsgCache == 0 -> nothing to do", getMsgNo()));
    return;
  } // if

  wsdebug(("> Mail::setFlag() MsgNo: %3ld  %s", getMsgNo(), getFlagStr(theFlag).c_str()));

  RequestMailFlagSet aReq(this, theFlag);
  if (!aReq.exec())
  {
    wswarn(("- Mail::setFlag() MsgNo: %3ld: error setting flag %s", getMsgNo(), getFlagStr(theFlag).c_str()));
  } // if

  wsdebug(("< Mail::setFlag() MsgNo: %3ld  %s", getMsgNo(), getFlagStr(theFlag).c_str()));
} // Mail::setFlag()

//-------------------------------------------------------------------------
//     Mail::clearFlag()
//-------------------------------------------------------------------------
void Mail::clearFlag(MailFlag theFlag)
{
  if (myMsgCache->isNull())
  {
    wswarn(("- Mail::clearFlag() MsgNo: %3ld  MsgCache == 0 -> nothing to do", getMsgNo()));
    return;
  } // if

  wsdebug(("> Mail::clearFlag() MsgNo: %3ld  %s", getMsgNo(), getFlagStr(theFlag).c_str()));

  RequestMailFlagClear aReq(this, theFlag);
  if (!aReq.exec())
  {
    wswarn(("- Mail::clearFlag() MsgNo: %3ld: error clearing flag %s", getMsgNo(), getFlagStr(theFlag).c_str()));
  } // if

  wsdebug(("< Mail::clearFlag() MsgNo: %3ld  %s", getMsgNo(), getFlagStr(theFlag).c_str()));
} // Mail::clearFlag()

//-------------------------------------------------------------------------
//     Mail::getFlag
//-------------------------------------------------------------------------
bool Mail::getFlag(MailFlag theFlag) const
{
  bool fSet = false;
  switch (theFlag)
  {
    case MF_SEEN    : fSet = getIsSeen    (); break;
    case MF_FLAGGED : fSet = getIsFlagged (); break;
    case MF_DELETED : fSet = getIsDeleted (); break;
    case MF_ANSWERED: fSet = getIsAnswered(); break;
  } // switch

  return fSet;
} // Mail::getFlag

//=============================================================================
//     namespaces
//=============================================================================
} // Mail
} // Ax
