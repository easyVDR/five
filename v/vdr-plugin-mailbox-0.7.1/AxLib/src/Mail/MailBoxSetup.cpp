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
#include <Ax/Tools/String.h>
#include <Ax/Mail/MailBoxSetup.h>
#include <Ax/Mail/MailBoxMgr.h>

//----- vdr -------------------------------------------------------------------
//----- local plugin ----------------------------------------------------------
//----- locals ----------------------------------------------------------------

//=============================================================================
//     namespaces
//=============================================================================
namespace Ax   {
namespace Mail {

//=============================================================================
//     class MailBoxSetup
//=============================================================================

//-------------------------------------------------------------------------
//     MailBoxSetup::MailBoxSetup()
//-------------------------------------------------------------------------
MailBoxSetup::MailBoxSetup(std::string theAccountName)
{
  wsdebug(("> MailBoxSetup::MailBoxSetup()"));
  init();
  myAccountName = theAccountName;
  wsdebug(("< MailBoxSetup::MailBoxSetup()"));
} // MailBoxSetup::MailBoxSetup()

//-------------------------------------------------------------------------
//     MailBoxSetup::~MailBoxSetup()
//-------------------------------------------------------------------------
MailBoxSetup::~MailBoxSetup()
{
  wsdebug(("> MailBoxSetup::~MailBoxSetup()"));
  wsdebug(("< MailBoxSetup::~MailBoxSetup()"));
} // MailBoxSetup::~MailBoxSetup()

//-------------------------------------------------------------------------
//     MailBoxSetup::init()
//-------------------------------------------------------------------------
void MailBoxSetup::init()
{
  myAccountName     = "";
  myFolderPath      = "";
  myUserName        = "";
  myPassword        = "";
  myHostName        = "";
  myAccessCode      = "";
  myIsDirty         = true;
  myAccountString   = "";
  myAccountType     = AT_IMAP     ;
  myAccessMode      = AM_NONE     ;
  myBGCheckMode     = BG_MODE_NONE;
  myClientOptSec    = CC_SEC_NONE ;
  myClientOptRSH    = CC_RSH_NO   ;
  myClientOptSSL    = CC_SSL_NONE ;
  myClientOptTLS    = CC_TLS_NO   ;
  myClientOptCert   = CC_CERT_NONE;
  myClientOptDbg    = CC_DBG_NONE ;
  myFAutoMarkSeen   = 0;
  myFExpungeOnClose = 0;
} // MailBoxSetup::init()

//-------------------------------------------------------------------------
//     MailBoxSetup::isValid()
//-------------------------------------------------------------------------
bool MailBoxSetup::isValid() const
{
  return !myAccountName     .empty()
      && (!myUserName       .empty() || myAccountType == AT_NNTP || myAccountType == AT_USER || myAccountType == AT_INBOX)
      && !getMailBoxString().empty()
       ;
} // MailBoxSetup::isValid()

//-------------------------------------------------------------------------
//     MailBoxSetup::dump()
//-------------------------------------------------------------------------
void MailBoxSetup::dump() const
{
  wsinfo(("dump of MailBoxSetup: %p", this));
  wsinfo(("  AccountName   : %s", myAccountName     .c_str()));
  wsinfo(("  FolderPath    : %s", myFolderPath      .c_str()));
  wsinfo(("  MailBoxString : %s", getMailBoxString().c_str()));
  wsinfo(("  UserName      : %s", myUserName        .c_str()));
  wsinfo(("  HostName      : %s", myHostName        .c_str()));
  wsinfo(("  AccessCode    : %s", myAccessCode      .c_str()));
  wsinfo(("  Type          : %d %s", myAccountType, getAccountTypeStr(myAccountType).c_str()));
} // MailBoxSetup::dump()

//-------------------------------------------------------------------------
//     MailBoxSetup::setMailBoxString()
//-------------------------------------------------------------------------
bool MailBoxSetup::setMailBoxString(const std::string &theMailBoxString)
{
  bool fParsedOK = initFromMailBoxString(theMailBoxString);

  if (!fParsedOK)
  {
    myAccountString = theMailBoxString;
    myFolderPath    = "";
    myHostName      = "";
    myIsDirty       = false;

    // reset the account type to USER and clear all the flags
    myAccountType   = AT_USER     ;

    myClientOptSec  = CC_SEC_NONE ;
    myClientOptRSH  = CC_RSH_NONE ;
    myClientOptSSL  = CC_SSL_NONE ;
    myClientOptTLS  = CC_TLS_NONE ;
    myClientOptCert = CC_CERT_NONE;
    myClientOptDbg  = CC_DBG_NONE ;
  } // if

  return fParsedOK;
} // MailBoxSetup::setMailBoxString()

//-------------------------------------------------------------------------
//     MailBoxSetup::getMailBoxString()
//-------------------------------------------------------------------------
std::string MailBoxSetup::getMailBoxString() const
{
  return getAccountString() + getFolderPath();
} // MailBoxSetup::getMailBoxString()

//-------------------------------------------------------------------------
//     MailBoxSetup::getAccountString()
//-------------------------------------------------------------------------
const std::string &MailBoxSetup::getAccountString() const
{
  if (myIsDirty)
  {
    updateAccountString();
  } // if
  return myAccountString;
} // MailBoxSetup::getAccountString()

//-------------------------------------------------------------------------
//     MailBoxSetup::setAccountType()
//-------------------------------------------------------------------------
void MailBoxSetup::setAccountType(int theAccountType)
{
  if (myAccountType != theAccountType)
  {
    myAccountType   = theAccountType;

    if (!supportsOption(CCO_HOSTNAME)) myHostName      = "";
    if (!supportsOption(CCO_FOLDER  )) myFolderPath    = "";
    if (!supportsOption(CCO_USERNAME)) myUserName      = "";
    if (!supportsOption(CCO_PASSWORD)) myPassword      = "";
    if (!supportsOption(CCO_SEC     )) myClientOptSec  = CC_SEC_NONE ;
    if (!supportsOption(CCO_RSH     )) myClientOptRSH  = CC_RSH_NONE ;
    if (!supportsOption(CCO_SSL     )) myClientOptSSL  = CC_SSL_NONE ;
    if (!supportsOption(CCO_TLS     )) myClientOptTLS  = CC_TLS_NONE ;
    if (!supportsOption(CCO_CERT    )) myClientOptCert = CC_CERT_NONE;
    if (!supportsOption(CCO_DBG     )) myClientOptCert = CC_DBG_NONE ;

    myIsDirty = true;
  } // if
} // MailBoxSetup::setAccountType()

//-------------------------------------------------------------------------
//     MailBoxSetup::setAccessMode()
//-------------------------------------------------------------------------
void MailBoxSetup::setAccessMode(int theAccessMode)
{
  if (myAccessMode != (theAccessMode & AM_MASK))
  {
    myAccessMode  = theAccessMode & AM_MASK;
    myIsDirty     = true;
  } // if
} // MailBoxSetup::setAccessMode()

//-------------------------------------------------------------------------
//     MailBoxSetup::setBGCheckMode()
//-------------------------------------------------------------------------
void MailBoxSetup::setBGCheckMode(int theBGCheckMode)
{
  if (myBGCheckMode != (theBGCheckMode & BG_MODE_MASK))
  {
    myBGCheckMode = theBGCheckMode & BG_MODE_MASK;
    myIsDirty     = true;
  } // if
} // MailBoxSetup::setBGCheckMode()

//-------------------------------------------------------------------------
//     MailBoxSetup::setClientOptSec()
//-------------------------------------------------------------------------
void MailBoxSetup::setClientOptSec(int theClientOptSec)
{
  if (supportsOption(CCO_SEC) && myClientOptSec != (theClientOptSec  & CC_SEC_MASK))
  {
    myClientOptSec  = theClientOptSec & CC_SEC_MASK;
    myIsDirty       = true;
  } // if
} // MailBoxSetup::setClientOptSec()

//-------------------------------------------------------------------------
//     MailBoxSetup::setClientOptRSH()
//-------------------------------------------------------------------------
void MailBoxSetup::setClientOptRSH(int theClientOptRSH)
{
  if (supportsOption(CCO_RSH ) && myClientOptRSH != (theClientOptRSH  & CC_RSH_MASK))
  {
    myClientOptRSH  = theClientOptRSH  & CC_RSH_MASK;
    myIsDirty       = true;
  } // if
} // MailBoxSetup::setClientOptRSH()

//-------------------------------------------------------------------------
//     MailBoxSetup::setClientOptSSL()
//-------------------------------------------------------------------------
void MailBoxSetup::setClientOptSSL(int theClientOptSSL)
{
  if (supportsOption(CCO_SSL) && myClientOptSSL != (theClientOptSSL & CC_SSL_MASK))
  {
    myClientOptSSL  = theClientOptSSL  & CC_SSL_MASK;
    myIsDirty       = true;
  } // if
} // MailBoxSetup::setClientOptSSL()

//-------------------------------------------------------------------------
//     MailBoxSetup::setClientOptTLS()
//-------------------------------------------------------------------------
void MailBoxSetup::setClientOptTLS(int theClientOptTLS)
{
  if (supportsOption(CCO_TLS) && myClientOptTLS != (theClientOptTLS & CC_TLS_MASK))
  {
    myClientOptTLS  = theClientOptTLS & CC_TLS_MASK;
    myIsDirty       = true;
  }
} // MailBoxSetup::setClientOptTLS()

//-------------------------------------------------------------------------
//     MailBoxSetup::setClientOptCert()
//-------------------------------------------------------------------------
void MailBoxSetup::setClientOptCert(int theClientOptCert)
{
  if (supportsOption(CCO_CERT) && myClientOptCert != (theClientOptCert & CC_CERT_MASK))
  {
    myClientOptCert = theClientOptCert & CC_CERT_MASK;
    myIsDirty       = true;
  } // if
} // MailBoxSetup::setClientOptCert()

//-------------------------------------------------------------------------
//     MailBoxSetup::setClientOptDbg()
//-------------------------------------------------------------------------
void MailBoxSetup::setClientOptDbg(int theClientOptDbg)
{
  if (supportsOption(CCO_DBG) && myClientOptDbg != (theClientOptDbg & CC_DBG_MASK))
  {
    myClientOptDbg  = theClientOptDbg & CC_DBG_MASK;
    myIsDirty       = true;
  } // if
} // MailBoxSetup::setClientOptDbg()

//-------------------------------------------------------------------------
//     MailBoxSetup::setAccountName()
//-------------------------------------------------------------------------
void MailBoxSetup::setAccountName(const std::string &theAccountName)
{
  myAccountName = theAccountName;
} // MailBoxSetup::setAccountName()

//-------------------------------------------------------------------------
//     MailBoxSetup::setFolderPath()
//-------------------------------------------------------------------------
void MailBoxSetup::setFolderPath(const std::string &theFolderPath)
{
  myFolderPath = theFolderPath;
} // MailBoxSetup::setFolderPath()

//-------------------------------------------------------------------------
//     MailBoxSetup::setAccessCode()
//-------------------------------------------------------------------------
void MailBoxSetup::setAccessCode(const std::string &theAccessCode)
{
  myAccessCode  = theAccessCode ;
} // MailBoxSetup::setAccessCode()

//-------------------------------------------------------------------------
//     MailBoxSetup::setUserName()
//-------------------------------------------------------------------------
void MailBoxSetup::setUserName(const std::string &theUserName)
{
  if (supportsOption(CCO_USERNAME))
  {
    myUserName = theUserName;
  } // if
} // MailBoxSetup::setUserName()

//-------------------------------------------------------------------------
//     MailBoxSetup::setPassword()
//-------------------------------------------------------------------------
void MailBoxSetup::setPassword(const std::string &thePassword)
{
  if (supportsOption(CCO_PASSWORD))
  {
    myPassword = thePassword;
  } // if
} // MailBoxSetup::setPassword()

//-------------------------------------------------------------------------
//     MailBoxSetup::setHostName()
//-------------------------------------------------------------------------
void MailBoxSetup::setHostName(const std::string &theHostName)
{
  if (supportsOption(CCO_HOSTNAME))
  {
    myHostName = theHostName;
    myIsDirty  = true;
  } // if
} // MailBoxSetup::setHostName()

//-------------------------------------------------------------------------
//     MailBoxSetup::supportsOption()
//-------------------------------------------------------------------------
bool MailBoxSetup::supportsOption(int theType, CCOption theOption)
{
  bool fSupported = false;

  switch (theType)
  {
    case AT_USER:
    {
      switch (theOption)
      {
        case CCO_HOSTNAME: fSupported = true ; break;
        case CCO_FOLDER  : fSupported = false; break;
        case CCO_USERNAME: fSupported = true ; break;
        case CCO_PASSWORD: fSupported = true ; break;
        case CCO_SEC     : fSupported = false; break;
        case CCO_RSH     : fSupported = false; break;
        case CCO_SSL     : fSupported = false; break;
        case CCO_TLS     : fSupported = false; break;
        case CCO_CERT    : fSupported = false; break;
        case CCO_DBG     : fSupported = false; break;
        default:                               break;
      } // switch
      break;
    }
    case AT_IMAP:
    {
      switch (theOption)
      {
        case CCO_HOSTNAME: fSupported = true ; break;
        case CCO_FOLDER  : fSupported = true ; break;
        case CCO_USERNAME: fSupported = true ; break;
        case CCO_PASSWORD: fSupported = true ; break;
        case CCO_SEC     : fSupported = true ; break;
        case CCO_RSH     : fSupported = true ; break;
        case CCO_SSL     : fSupported = true ; break;
        case CCO_TLS     : fSupported = true ; break;
        case CCO_CERT    : fSupported = true ; break;
        case CCO_DBG     : fSupported = true ; break;
        default:                               break;
      } // switch
      break;
    }
    case AT_POP3:
    {
      switch (theOption)
      {
        case CCO_HOSTNAME: fSupported = true ; break;
        case CCO_FOLDER  : fSupported = false; break;
        case CCO_USERNAME: fSupported = true ; break;
        case CCO_PASSWORD: fSupported = true ; break;
        case CCO_SEC     : fSupported = true ; break;
        case CCO_RSH     : fSupported = false; break;
        case CCO_SSL     : fSupported = true ; break;
        case CCO_TLS     : fSupported = true ; break;
        case CCO_CERT    : fSupported = true ; break;
        case CCO_DBG     : fSupported = true ; break;
        default:                               break;
      } // switch
      break;
    }
    case AT_NNTP:
    {
      switch (theOption)
      {
        case CCO_HOSTNAME: fSupported = true ; break;
        case CCO_FOLDER  : fSupported = true ; break;
        case CCO_USERNAME: fSupported = false; break;
        case CCO_PASSWORD: fSupported = false; break;
        case CCO_SEC     : fSupported = false; break;
        case CCO_RSH     : fSupported = false; break;
        case CCO_SSL     : fSupported = false; break;
        case CCO_TLS     : fSupported = false; break;
        case CCO_CERT    : fSupported = false; break;
        case CCO_DBG     : fSupported = true ; break;
        default:                               break;
      } // switch
      break;
    }
    case AT_INBOX:
    {
      switch (theOption)
      {
        case CCO_HOSTNAME: fSupported = false; break;
        case CCO_FOLDER  : fSupported = false; break;
        case CCO_USERNAME: fSupported = false; break;
        case CCO_PASSWORD: fSupported = false; break;
        case CCO_SEC     : fSupported = false; break;
        case CCO_RSH     : fSupported = false; break;
        case CCO_SSL     : fSupported = false; break;
        case CCO_TLS     : fSupported = false; break;
        case CCO_CERT    : fSupported = false; break;
        case CCO_DBG     : fSupported = false; break;
        default:                               break;
      } // switch
      break;
    }
    default:
    {
      break;
    }
  } // switch
#if 0
  wsdebug(("- MailBoxSetup::supportsOption() Type: %d  %s  Option: %d  Supp: %d"
         , theType
         , getAccountTypeStr(theType).c_str()
         , theOption
         , fSupported
         ));
#endif
  return fSupported;
} // MailBoxSetup::supportsOption()

#include <Ax/Tools/TraceOff.h>

//-------------------------------------------------------------------------
//     MailBoxSetup::getAccountTypeStr()
//-------------------------------------------------------------------------
std::string MailBoxSetup::getAccountTypeStr(int theOpt)
{
  switch (theOpt)
  {
    case AT_IMAP:  return "/imap";
    case AT_POP3:  return "/pop3";
    case AT_NNTP:  return "/nntp";
    case AT_INBOX: return "";
  } // switch
  return "";
} // MailBoxSetup::getAccountTypeStr()

//-------------------------------------------------------------------------
//     MailBoxSetup::getClientOptSecStr()
//-------------------------------------------------------------------------
std::string MailBoxSetup::getClientOptSecStr(int theOpt)
{
  switch (theOpt)
  {
    case CC_SEC_YES : return "/secure";
  } // switch
  return "";
} // MailBoxSetup::getClientOptSecStr()

//-------------------------------------------------------------------------
//     MailBoxSetup::getClientOptSecStr()
//-------------------------------------------------------------------------
std::string MailBoxSetup::getClientOptRSHStr(int theOpt)
{
  switch (theOpt)
  {
    case CC_RSH_NO  : return "/norsh";
  } // switch
  return "";
} // MailBoxSetup::getClientOptRSHStr()

//-------------------------------------------------------------------------
//     MailBoxSetup::getClientOptSecStr()
//-------------------------------------------------------------------------
std::string MailBoxSetup::getClientOptSSLStr(int theOpt)
{
  switch (theOpt)
  {
    case CC_SSL_YES : return "/ssl";
  } // switch
  return "";
} // MailBoxSetup::getClientOptSSLStr()

//-------------------------------------------------------------------------
//     MailBoxSetup::getClientOptTLSStr()
//-------------------------------------------------------------------------
std::string MailBoxSetup::getClientOptTLSStr(int theOpt)
{
  switch (theOpt)
  {
    case CC_TLS_YES : return "/tls";
    case CC_TLS_NO  : return "/notls";
  } // switch
  return "";
} // MailBoxSetup::getClientOptTLSStr()

//-------------------------------------------------------------------------
//     MailBoxSetup::getClientOptCertStr()
//-------------------------------------------------------------------------
std::string MailBoxSetup::getClientOptCertStr(int theOpt)
{
  switch (theOpt)
  {
    case CC_CERT_VALIDATE  : return "/validate-cert";
    case CC_CERT_NOVALIDATE: return "/novalidate-cert";
  } // switch
  return "";
} // MailBoxSetup::getClientOptCertStr()

//-------------------------------------------------------------------------
//     MailBoxSetup::getClientOptDbgStr()
//-------------------------------------------------------------------------
std::string MailBoxSetup::getClientOptDbgStr(int theOpt)
{
  switch (theOpt)
  {
    case CC_DBG_YES : return "/debug";
  } // switch
  return "";
} // MailBoxSetup::getClientOptDbgStr()

//-------------------------------------------------------------------------
//     MailBoxSetup::updateAccountString()
//-------------------------------------------------------------------------
void MailBoxSetup::updateAccountString() const
{
  switch (getAccountType())
  {
    case AT_IMAP:
    case AT_POP3:
    case AT_NNTP:
    {
      myAccountString = std::string("{")
                      + getHostName()
                      + getAccountTypeStr()
                      + getClientOptSecStr()
                      + getClientOptRSHStr()
                      + getClientOptSSLStr()
                      + getClientOptTLSStr()
                      + getClientOptCertStr()
                      + getClientOptDbgStr()
                      + std::string("}")
                      ;
      break;
    } // case

    case AT_INBOX:
    {
      myAccountString = std::string("INBOX");
      break;
    } // case
  } // switch

  myIsDirty = false;
} // MailBoxSetup::updateAccountString()

//-------------------------------------------------------------------------
//     MailBoxSetup::initFromMailBoxString()
//-------------------------------------------------------------------------
bool MailBoxSetup::initFromMailBoxString(std::string theMailBoxString)
{
  if (theMailBoxString.empty())
  {
    wswarn(("MailBoxSetup::initFromMailBoxString() mailbox-string is empty!"));
    return false;
  } // if

  if (theMailBoxString[0] == '#')
  {
    wsdebug(("MailBoxSetup::initFromMailBoxString() mailbox-string '%s' is a special name and therefore used unchanged!"
          , theMailBoxString.c_str()
          ));
    return false;
  }  // if

  if (theMailBoxString == std::string("INBOX"))
  {
    wsdebug(("MailBoxSetup::initFromMailBoxString() mailbox-string is INBOX"));
    setAccountType(AT_INBOX);
    return true;
  }  // if
  
  wsdebug(("> MailBoxSetup::initFromMailBoxString() MailBoxStr: '%s'", theMailBoxString.c_str()));
  // create a local copy to work with
  MailBoxSetup s(*this);

  std::string::size_type aPosFirst = theMailBoxString.find('{');
  std::string::size_type aPosLast  = theMailBoxString.find('}');

  wsdebug(("  MailBoxSetup::initFromMailBoxString() StrLen  : %d", theMailBoxString.length()));
  wsdebug(("  MailBoxSetup::initFromMailBoxString() PosFirst: %d", aPosFirst));
  wsdebug(("  MailBoxSetup::initFromMailBoxString() PosLast : %d", aPosLast ));

  if ( aPosFirst == std::string::npos
    || aPosLast  == std::string::npos
    || aPosFirst >  aPosLast
     )
  {
    wswarn(("MailBoxSetup::initFromMailBoxString() unable to parse mailbox-string '%s'", theMailBoxString.c_str()));
    wswarn(("MailBoxSetup::initFromMailBoxString() '{' and '}' not found!"));
    return false;
  } // if

  std::string aStrOpt(theMailBoxString, aPosFirst + 1, aPosLast - aPosFirst - 1);
  std::string aFolder(theMailBoxString, aPosLast  + 1);

  wsdebug(("  MailBoxSetup::initFromMailBoxString() StrOpt: '%s'", aStrOpt.c_str()));
  wsdebug(("  MailBoxSetup::initFromMailBoxString() Folder: '%s'", aFolder.c_str()));

  if (aStrOpt.empty())
  {
    wswarn(("MailBoxSetup::initFromMailBoxString() unable to parse mailbox-string '%s'", theMailBoxString.c_str()));
    wswarn(("MailBoxSetup::initFromMailBoxString() no characters between '{' and '}'"));
    return false;
  } // if

  if (aStrOpt[0] == '/')
  {
    wswarn(("MailBoxSetup::initFromMailBoxString() unable to parse mailbox-string '%s'", theMailBoxString.c_str()));
    wswarn(("MailBoxSetup::initFromMailBoxString() host-name missing!"));
    return false;
  } // if

  Tools::StringVector aVecOpt = Tools::String::split(aStrOpt, '/', false);

  if (aVecOpt.size() == 0)
  {
    wswarn(("MailBoxSetup::initFromMailBoxString() unable to parse mailbox-string '%s'", theMailBoxString.c_str()));
    wswarn(("MailBoxSetup::initFromMailBoxString() no strings between '{' and '}'"));
    return false;
  } // if

  std::string aHost = aVecOpt[0];

  wsdebug(("  MailBoxSetup::initFromMailBoxString() Host  : '%s'", aHost.c_str()));
  wsdebug(("  MailBoxSetup::initFromMailBoxString() Folder: '%s'", aFolder.c_str()));

  s.setAccountType(AT_USER);

  //----- first get type -----
  for (Tools::StringVector::size_type i = 1; i < aVecOpt.size(); ++i)
  {
    wsdebug(("  MailBoxSetup::initFromMailBoxString() Opt[%2d] '%s'", i, aVecOpt[i].c_str()));

    std::string anOptStr = std::string("/") + aVecOpt[i];

    if      (anOptStr == "/imap"                                ) { s.setAccountType  (AT_IMAP); aVecOpt[i] = ""; }
    else if (anOptStr == "/imap2"                               ) { s.setAccountType  (AT_IMAP); aVecOpt[i] = ""; }
    else if (anOptStr == "/imap2bis"                            ) { s.setAccountType  (AT_IMAP); aVecOpt[i] = ""; }
    else if (anOptStr == "/imap4"                               ) { s.setAccountType  (AT_IMAP); aVecOpt[i] = ""; }
    else if (anOptStr == "/imap4rev1"                           ) { s.setAccountType  (AT_IMAP); aVecOpt[i] = ""; }
    else if (anOptStr == "/pop3"                                ) { s.setAccountType  (AT_POP3); aVecOpt[i] = ""; }
    else if (anOptStr == "/nntp"                                ) { s.setAccountType  (AT_NNTP); aVecOpt[i] = ""; }
  } // for

  s.setHostName  (aHost  );
  s.setFolderPath(aFolder);

  bool fOK = true;
  for (Tools::StringVector::size_type i = 1; i < aVecOpt.size() && fOK; ++i)
  {
    if (aVecOpt[i] != "")
    {
      std::string anOptStr = std::string("/") + Tools::String::stripWhiteSpaces(aVecOpt[i]);

      if      (anOptStr == getClientOptSecStr (CC_SEC_YES        )) s.setClientOptSec (CC_SEC_YES);
      else if (anOptStr == getClientOptRSHStr (CC_RSH_NO         )) s.setClientOptRSH (CC_RSH_NO );
      else if (anOptStr == getClientOptSSLStr (CC_SSL_YES        )) s.setClientOptSSL (CC_SSL_YES);
      else if (anOptStr == getClientOptTLSStr (CC_TLS_YES        )) s.setClientOptTLS (CC_TLS_YES);
      else if (anOptStr == getClientOptTLSStr (CC_TLS_NO         )) s.setClientOptTLS (CC_TLS_NO );
      else if (anOptStr == getClientOptCertStr(CC_CERT_VALIDATE  )) s.setClientOptCert(CC_CERT_VALIDATE );
      else if (anOptStr == getClientOptCertStr(CC_CERT_NOVALIDATE)) s.setClientOptCert(CC_CERT_NOVALIDATE);
      else if (anOptStr == getClientOptDbgStr (CC_DBG_YES        )) s.setClientOptDbg (CC_DBG_YES);
      else
      {
        wswarn(("MailBoxSetup::initFromMailBoxString() option '%s' not supported in typed mailbox-settings!"
              , anOptStr.c_str()
              ));
        fOK = false;
      } //
    } // if
  } // for

  //----- now change this -----
  if (fOK)
  {
    *this = s;
  } // if

  wsdebug(("< MailBoxSetup::initFromMailBoxString() OK: %d  MailBoxStr: '%s'", fOK, getMailBoxString().c_str()));
  return fOK;
} // MailBoxSetup::createFromMailBoxString()

//=============================================================================
//     namespaces
//=============================================================================
} // Mail
} // Ax
