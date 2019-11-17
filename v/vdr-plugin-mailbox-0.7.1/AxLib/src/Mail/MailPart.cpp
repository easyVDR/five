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
 *   $Date: 2013-03-20 22:41:44 +0100 (Mi, 20 Mär 2013) $
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
#include <ctype.h>
#include <stdio.h>

//----- AxLib -----------------------------------------------------------------
#include <Ax/Tools/Trace.h>
// #include <Ax/Tools/TraceOn.h>
#include <Ax/Tools/String.h>
#include <Ax/Mail/Mail.h>
#include <Ax/Mail/MailFolder.h>
#include <Ax/Mail/MailPart.h>
#include <Ax/Mail/MailBox.h>
#include <Ax/Mail/MailBoxMgr.h>
#include <Ax/Mail/Tools.h>

//----- vdr -------------------------------------------------------------------
//----- local plugin ----------------------------------------------------------
//----- locals ----------------------------------------------------------------
#include "MsgBody.h"
#include "Request.h"

//=============================================================================
//     namespaces
//=============================================================================
namespace Ax   {
namespace Mail {

//=============================================================================
//     class MailPart
//=============================================================================

//-------------------------------------------------------------------------
//     MailPart::MailPart()
//-------------------------------------------------------------------------
MailPart::MailPart( const Mail        *theMail
                  , const MailPart    *theParent
                  , const MsgBody     &theBody
                  , const std::string &theSectionName
                  )
          : myMail       (theMail   )
          , myParent     (theParent )
          , myBody       (new MsgBody(theBody))
          , mySectionName(theSectionName)
{
  wsdebug(("> MailPart::MailPart() Section: %-5s MultiPart: %d Parent: %p"
         , mySectionName.c_str()
         , isMultiPart()
         , myParent
         ));

  wsdebug(("  MailPart::MailPart() Type: %d  id: %s  desc: %s"
         , getType()
         , myBody->get() ? myBody->get()->id          : "(nil)"
         , myBody->get() ? myBody->get()->description : "(nil)"
         ));

  wsdebug(("  MailPart::MailPart() Type: %s  body: %p  subtype: %s"
         , getTypeStr().c_str()
         , myBody->get()
         , myBody->get() ? (myBody->get()->subtype ? myBody->get()->subtype : "(nil)") : ""
         ));

  //----- check for sub-parts and create them too -----
  if (isMultiPart())
  {
    //----- multipart message -----
    int j = 1;
    for (PART *aPart  = (*myBody)->nested.part;
               aPart != 0;
               aPart  = aPart->next, ++j)
    {
      wsdebug(("  MailPart::MailPart() adding nested part..."));

      std::string aSubSecName = mySectionName;
      if (!aSubSecName.empty())
      {
        aSubSecName += std::string(".");
      } // if
      char aName[20];
      sprintf(aName, "%d", j);
      aSubSecName += std::string(aName);

      mySubParts.push_back(new MailPart(myMail, this, &aPart->body, aSubSecName));
    } // for
  }
  else if ( ((*myBody)->type == TYPEMESSAGE)
         && !strcmp ((*myBody)->subtype, "RFC822")
         && ((*myBody)->nested.msg->body != 0)
          )
  {
    wsdebug(("  MailPart::MailPart() adding nested mail..."));
    //----- encapsulated message? -----
    BODY *aBody = (*myBody)->nested.msg->body;

    std::string aSubSecName = mySectionName;

    if (aBody->type != TYPEMULTIPART)
    {
      if (!aSubSecName.empty())
      {
        aSubSecName += std::string(".");
      } // if
      aSubSecName += std::string("1");
    } // if

    mySubParts.push_back( new MailPart( myMail
                                      , this
                                      , aBody
                                      , aSubSecName
                                      )
                        );
  } // if

  wsdebug(("< MailPart::MailPart() Section: %-5s MultiPart: %d Parent: %p  SubParts: %d"
         , mySectionName.c_str()
         , isMultiPart()
         , myParent
         , mySubParts.size()
         ));
} // MailPart::MailPart()

//-------------------------------------------------------------------------
//     MailPart::~MailPart()
//-------------------------------------------------------------------------
MailPart::~MailPart()
{
  wsdebug(("> MailPart::~MailPart() Section: %-5s MultiPart: %d Parent: %p  SubParts: %d"
         , mySectionName.c_str()
         , isMultiPart()
         , myParent
         , mySubParts.size()
         ));
  for (MailPartVector::iterator anIter  = mySubParts.begin();
                                anIter != mySubParts.end();
                              ++anIter)
  {
    delete *anIter;
    *anIter = 0;
  } // while
  mySubParts.clear();

  delete myBody;

  wsdebug(("< MailPart::~MailPart() Section: %-5s MultiPart: %d Parent: %p  SubParts: %d"
         , mySectionName.c_str()
         , isMultiPart()
         , myParent
         , mySubParts.size()
         ));
} // MailPart::~MailPart()

//-------------------------------------------------------------------------
//     MailPart::getMail()
//-------------------------------------------------------------------------
const Mail *MailPart::getMail() const
{
  return myMail;
} // MailPart::getMail()

//-------------------------------------------------------------------------
//     MailPart::getParent()
//-------------------------------------------------------------------------
const MailPart *MailPart::getParent() const
{
  return myParent;
} // MailPart::getParent()

//-------------------------------------------------------------------------
//     MailPart::getSubParts()
//-------------------------------------------------------------------------
const MailPartVector &MailPart::getSubParts() const
{
  return mySubParts;
} // MailPart::getSubParts()

//-------------------------------------------------------------------------
//     MailPart::getType()
//-------------------------------------------------------------------------
EBodyType MailPart::getType() const
{
  switch ((*myBody)->type)
  {
    case TYPETEXT       : return BT_TEXT;
    case TYPEMULTIPART  : return BT_MULTIPART;
    case TYPEMESSAGE    : return BT_MESSAGE;
    case TYPEAPPLICATION: return BT_APPLICATION;
    case TYPEAUDIO      : return BT_AUDIO;
    case TYPEIMAGE      : return BT_IMAGE;
    case TYPEVIDEO      : return BT_VIDEO;
    case TYPEMODEL      : return BT_MODEL;
    case TYPEOTHER      : return BT_OTHER;
    case TYPEMAX        : return BT_MAX;
  } // switch

  return BT_UNKNOWN;
} // MailPart::getType()

//-------------------------------------------------------------------------
//     MailPart::getEnc()
//-------------------------------------------------------------------------
EBodyEncoding MailPart::getEnc() const
{
  switch ((*myBody)->encoding)
  {
    case ENC7BIT           : return BE_7BIT;
    case ENC8BIT           : return BE_8BIT;
    case ENCBINARY         : return BE_BINARY;
    case ENCBASE64         : return BE_BASE64;
    case ENCQUOTEDPRINTABLE: return BE_QUOTEDPRINTABLE;
    case ENCOTHER          : return BE_OTHER;
    case ENCMAX            : return BE_MAX;
  } // switch

  return BE_UNDEFINED;
} // MailPart::getEnc()

//-------------------------------------------------------------------------
//     MailPart::getCountLines()
//-------------------------------------------------------------------------
unsigned long MailPart::getCountLines() const
{
  return (*myBody)->size.lines;
} // MailPart::getCountLines()

//-------------------------------------------------------------------------
//     MailPart::getCountBytes()
//-------------------------------------------------------------------------
unsigned long MailPart::getCountBytes() const
{
  return (*myBody)->size.bytes;
} // MailPart::getCountBytes()

//-------------------------------------------------------------------------
//     MailPart::getTypeStr()
//-------------------------------------------------------------------------
std::string MailPart::getTypeStr() const
{
  if ((*myBody)->type < TYPEMAX)
  {
    return body_types[(*myBody)->type];
  } // if
  return Ax::Tools::String::sprintf("Unknown_%d", (*myBody)->type);
} // MailPart::getTypeStr()

//-------------------------------------------------------------------------
//     MailPart::getEncStr()
//-------------------------------------------------------------------------
std::string MailPart::getEncStr() const
{
  return body_encodings[(*myBody)->encoding];
} // MailPart::getEncStr()

//-------------------------------------------------------------------------
//     MailPart::getSubType()
//-------------------------------------------------------------------------
std::string MailPart::getSubType() const
{
  return (*myBody)->subtype ? (*myBody)->subtype : "" ;
} // MailPart::getSubType()

//-------------------------------------------------------------------------
//     MailPart::getID()
//-------------------------------------------------------------------------
std::string MailPart::getID() const
{
  return (*myBody)->id ? (*myBody)->id : "" ;
} // MailPart::getID()

//-------------------------------------------------------------------------
//     MailPart::getDesc()
//-------------------------------------------------------------------------
std::string MailPart::getDesc() const
{
  return (*myBody)->description ? (*myBody)->description : "" ;
} // getDesc()

//-------------------------------------------------------------------------
//     MailPart::getMD5()
//-------------------------------------------------------------------------
std::string MailPart::getMD5() const
{
  return (*myBody)->md5 ? (*myBody)->md5 : "" ;
} // MailPart::getMD5()

//-------------------------------------------------------------------------
//     MailPart::getPartName()
//-------------------------------------------------------------------------
std::string MailPart::getPartName() const
{
  return mySectionName;
} // MailPart::getPartName()

//-------------------------------------------------------------------------
//     MailPart::isMultiPart()
//-------------------------------------------------------------------------
bool MailPart::isMultiPart() const
{
  return ((*myBody)->type == TYPEMULTIPART);
} // MailPart::isMultiPart()

//-------------------------------------------------------------------------
//     MailPart::getParams()
//-------------------------------------------------------------------------
String2StringMap MailPart::getParams() const
{
  String2StringMap aMap;
  PARAMETER       *aPar = (*myBody)->parameter;

  if (aPar != 0)
  {
    do
    {
      aMap[aPar->attribute] = aPar->value ? aPar->value : "";
    }  while ((aPar = aPar->next) != 0);
  } // if

  return aMap;
} // MailPart::getParams()

//-------------------------------------------------------------------------
//     MailPart::getParam()
//-------------------------------------------------------------------------
std::string MailPart::getParam(std::string theParam, bool *theFFound) const
{
  bool        fFound = false;
  std::string aValue("");
  PARAMETER  *aPar   = (*myBody)->parameter;

  if (aPar != 0 && !theParam.empty())
  {
    std::transform(theParam.begin(), theParam.end(), theParam.begin(), toupper);

    do
    {
      std::string aParam(aPar->attribute);
      std::transform(aParam.begin(), aParam.end(), aParam.begin(), toupper);

      fFound = (aParam == theParam);
      if (fFound)
      {
        aValue = aPar->value ? std::string(aPar->value) : std::string("");
        std::transform(aValue.begin(), aValue.end(), aValue.begin(), toupper);
      } // if
    } while ((aPar = aPar->next) != 0 && !fFound);
  } // if

#if 0
  if (!fFound)
  {
    wswarn(("unable to find param '%s' in mail part!", theParam.c_str()));
  } // if
#endif

  if (theFFound)
  {
    *theFFound = fFound;
  } // if

  return aValue;
} // MailPart::getParam()

//-------------------------------------------------------------------------
//     MailPart::getBodyText()
//-------------------------------------------------------------------------
std::string MailPart::getBodyText() const
{
  wsinfo(("retrieving part %s of mail %ld in mailbox '%s'..."
        , getPartName().c_str()
        , myMail->getMsgNo()
        , myMail->getFolder()->getMailBoxString().c_str()
        ));

  RequestBodyFull aReq(const_cast<Mail *>(getMail()), getPartName());

  std::string aBodyText("");
  if (!aReq.exec())
  {
    wserror(("retrieving part %s of mail %ld in mailbox '%s'...FAILED"
           , getPartName().c_str(), myMail->getMsgNo()
           , myMail->getFolder()->getMailBoxString().c_str()
           ));
  }
  else
  {
    if (!myBody->isNull() && aReq.getBodyLen() != (*myBody)->size.bytes)
    {
      wswarn(("part %s of mail %ld in mailbox '%s' invalid sizes: expected: %ld != %ld received (strlen: %ld)"
            , getPartName().c_str()
            , myMail->getMsgNo()
            , myMail->getFolder()->getMailBoxString().c_str()
            , (*myBody)->size.bytes
            , aReq.getBodyLen()
            , long(aReq.getBodyText().length())
            ));
    } // if

    aBodyText = aReq.getBodyText();

    switch (getEnc())
    {
      case BE_QUOTEDPRINTABLE:
      {
        if (!Tools::decodeQuoted(aBodyText, aBodyText))
        {
          wswarn(("decoding of 'quoted-printable' failed for part %s in mail %ld in mailbox '%s'"
                , getPartName().c_str()
                , myMail->getMsgNo()
                , myMail->getFolder()->getMailBoxString().c_str()
                ));
        } // if
        break;
      }

      case BE_BASE64:
      {
        if (!Tools::decodeBase64(aBodyText, aBodyText))
        {
          wswarn(("decoding of 'base64' failed for part %s in mail %ld in mailbox '%s'"
                , getPartName().c_str()
                , myMail->getMsgNo()
                , myMail->getFolder()->getMailBoxString().c_str()
                ));
        } // if
        break;
      }

      default:
      {
        // can't handle the other encodings now
        break;
      }
    } // switch

    wsinfo(("retrieving part %s of mail %ld in mailbox '%s'...OK"
          , getPartName().c_str()
          , myMail->getMsgNo()
          , myMail->getFolder()->getMailBoxString().c_str()
          ));
  } // if

  return aBodyText;
} // MailPart::getBodyText()

//=============================================================================
//     namespaces
//=============================================================================
} // Mail
} // Ax
