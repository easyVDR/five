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

//----- AxLib -----------------------------------------------------------------
#include <Ax/Tools/Trace.h>
// #include <Ax/Tools/TraceOn.h>
#include <Ax/Tools/String.h>
#include <Ax/Mail/MailFolder.h>
#include <Ax/Mail/MailBox.h>
#include <Ax/Mail/MailBoxSetup.h>
#include <Ax/Mail/MailBoxMgr.h>
#include <Ax/Mail/Mail.h>

//----- vdr -------------------------------------------------------------------
//----- c-client --------------------------------------------------------------
//----- local -----------------------------------------------------------------
#include "Request.h"
#include "Internal.h"
#include "MailBoxInternal.h"
#include "MsgCache.h"

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
//     class MailFolder
//=============================================================================

//-------------------------------------------------------------------------
//     MailFolder::MailFolder()
//-------------------------------------------------------------------------
MailFolder::MailFolder(       MailBox     *theMailBox
                      , const std::string &theFolderPath
                      ,       MailFolder  *theParentFolder
                      ,       long         theAttr
                      )
          : myMailBox          (theMailBox     )
          , myParentFolder     (theParentFolder)
          , myFolderPath       (theFolderPath  )
          , myIsOpen           (false)
          , myHasStatus        (false)
          , myCountMails       (0)
          , myCountUnseen      (0)
          , myCountRecent      (0)
          , myUIDNext          (0)
          , myUIDValidity      (0)
          , myAttributes       (theAttr)
{
#if 0
  wsdebug(("> MailFolder::MailFolder() MailBox: '%s'  Folder: '%s'"
         , myMailBox->getAccountName().c_str()
         , myFolderPath.c_str()
         ));
#endif

#if 0
  wsdebug(("< MailFolder::MailFolder() MailBox: '%s'  Folder: '%s'"
         , myMailBox->getAccountName().c_str()
         , myFolderPath.c_str()
         ));
#endif
} // MailFolder::MailFolder()

//-------------------------------------------------------------------------
//     MailFolder::~MailFolder()
//-------------------------------------------------------------------------
MailFolder::~MailFolder()
{
#if 0
  wsdebug(("> MailFolder::~MailFolder() MailBox: '%s'  Folder: '%s'"
         , myMailBox->getAccountName().c_str()
         , myFolderPath.c_str()
         ));
#endif

  clear();

#if 0
  wsdebug(("< MailFolder::~MailFolder() MailBox: '%s'  Folder: '%s'"
         , myMailBox->getAccountName().c_str()
         , myFolderPath.c_str()
         ));
#endif
} // MailFolder::~MailFolder()

//-------------------------------------------------------------------------
//     MailFolder::clear()
//-------------------------------------------------------------------------
void MailFolder::clear()
{
  wsdebug(("> MailFolder::clear() %s", getMailBoxString().c_str()));
  myIsOpen      = false;
  myHasStatus   = false;
#if 1
  myCountMails  =  0;
  myCountRecent =  0;
  myCountUnseen =  0;
  myUIDNext     =  0;
  myUIDValidity =  0;
#endif

  //----- removing mails in thisski  folder -----------------------------------
  wsdebug(("  MailFolder::clear() %s  myMails  : %d", getMailBoxString().c_str(), myMails.size()));
  for (MailVector::size_type i = 0; i < myMails.size(); ++i)
  {
    if (myMails[i] != 0)
    {
      wsdebug(("  MailFolder::clear() deleting mail %d", i));
      delete myMails[i];
      myMails[i] = 0;
    } // if
  } // for
  myMails.clear();

  //----- removing sub folders --------------------------------------------
  wsdebug(("  MailFolder::clear() %s  myFolders: %d", getMailBoxString().c_str(), mySubFolders.size()));
  for (MailFolderCltn::iterator anIter  = mySubFolders.begin();
                                anIter != mySubFolders.end();
                              ++anIter)
  {
    MailFolder *aSubFolder = *anIter;

    // if the folder is the current folder of the mailbox,
    // then the folder isn't destroyed but removed from the tree
    if (aSubFolder == getMailBox()->getCurrentFolder())
    {
      wsdebug(("  MailFolder::clear() folder %s is the current folder -> removing parent pointer"
             , aSubFolder->getFolderPath().c_str()
             ));
      aSubFolder->setParentFolder(0);
      aSubFolder->clear();
    }
    else
    {
      wsdebug(("  MailFolder::clear() deleting folder: %s", aSubFolder->getFolderPath().c_str()));
      delete aSubFolder;
    } // if
  } // while
  mySubFolders.clear();

  wsdebug(("< MailFolder::clear() %s", getMailBoxString().c_str()));
} // MailFolder::clear();

//-------------------------------------------------------------------------
//     MailFolder::getMailBox()
//-------------------------------------------------------------------------
MailBox *MailFolder::getMailBox() const
{
  return myMailBox;
} // MailFolder::getMailBox()

//-------------------------------------------------------------------------
//     MailFolder::getParentFolder()
//-------------------------------------------------------------------------
MailFolder *MailFolder::getParentFolder() const
{
  return myParentFolder;
} // MailFolder::getParentFolder()

//-------------------------------------------------------------------------
//     MailFolder::setParentFolder()
//-------------------------------------------------------------------------
void MailFolder::setParentFolder(MailFolder *theParentFolder)
{
  myParentFolder = theParentFolder;
} // MailFolder::setParentFolder()

//-------------------------------------------------------------------------
//     MailFolder::getMailBoxString()
//-------------------------------------------------------------------------
const std::string MailFolder::getMailBoxString() const
{
  return getMailBox()->getSetup().getAccountString() + getFolderPath();
} // MailFolder::getMailBoxString()

//-------------------------------------------------------------------------
//     MailFolder::getFolderPath()
//-------------------------------------------------------------------------
const std::string &MailFolder::getFolderPath() const
{
  return myFolderPath;
} // MailFolder::getFolderPath()

//-------------------------------------------------------------------------
//     MailFolder::getFolderName()
//-------------------------------------------------------------------------
std::string MailFolder::getFolderName() const
{
  std::string aName = getFolderPath();

  if ( getParentFolder() != 0            // if no parent present, return my full name
    && getMailBox()->hasDelimiter()
     )
  {
    std::string::size_type aPos = aName.find_last_of(getMailBox()->getDelimiter());
    if (aPos != std::string::npos)
    {
      aName = aName.substr(aPos + 1);
    } // if
  } // if

  return aName;
} // MailFolder::getFolderName()

//-------------------------------------------------------------------------
//     MailFolder::getSetup()
//-------------------------------------------------------------------------
const MailBoxSetup &MailFolder::getSetup() const
{
  return getMailBox()->getSetup();
} // MailFolder::getSetup()

//-------------------------------------------------------------------------
//     binary predicare to compare MailFolder-instances by name
//-------------------------------------------------------------------------
bool MailFolderNameLessThan(const MailFolder *f1, const MailFolder *f2)
{
  return f1->getFolderPath() < f2->getFolderPath();
} // MailFolderNameLessThan()

//=========================================================================
//     commands to the mail server
//=========================================================================

//-------------------------------------------------------------------------
//     MailFolder::requestStatus()
//-------------------------------------------------------------------------
bool MailFolder::requestStatus()
{
  wsdebug(("> MailFolder::requestStatus()"));
  std::string aMailBoxString = getMailBoxString();

  wsinfo(("requesting status for '%s'...", aMailBoxString.c_str()));

  myCountMails  = 0;
  myCountUnseen = 0;
  myCountRecent = 0;
  myUIDNext     = 0;
  myUIDValidity = 0;

  //----- request status --------------------------------------------------
  RequestStatus aReq( aMailBoxString
                    , getMailBox()
                    , getMailBox()->getSetup().getAccountString() + " -> MailFolder::requestStatus()"
                    );

  myHasStatus = aReq.exec();
  if (!myHasStatus)
  {
    wserror(("unable to request status for '%s': %s"
           , aMailBoxString.c_str()
           , getMailBox()->getLogHandler().getLastImportantMsg().c_str()
           ));
  }
  else
  {
    myCountMails  = (aReq.getStatus().flags & SA_MESSAGES   ) ? aReq.getStatus().messages    : 0;
    myCountUnseen = (aReq.getStatus().flags & SA_UNSEEN     ) ? aReq.getStatus().unseen      : 0;
    myCountRecent = (aReq.getStatus().flags & SA_RECENT     ) ? aReq.getStatus().recent      : 0;
    myUIDNext     = (aReq.getStatus().flags & SA_UIDNEXT    ) ? aReq.getStatus().uidnext     : 0;
    myUIDValidity = (aReq.getStatus().flags & SA_UIDVALIDITY) ? aReq.getStatus().uidvalidity : 0;

    getMailBox()->getInternal().setCurrentFolder(this);
  } // if

  //----- create/delete mail instances ------------------------------------
  if (myMails.size() != myCountMails)
  {
    int anIndex = 0;;

    // delete supernumerous mails
    for (anIndex = myCountMails; anIndex < int(myMails.size()); ++anIndex)
    {
//      wsdebug(("  MailFolder::processMailStatus() deleting supernumerous mail %d", anIndex));
      Mail *aMail = myMails[anIndex];
      myMails[anIndex] = 0;
      delete aMail;
    } // for

    anIndex = myMails.size();
    myMails.resize(myCountMails);

    // create new mails
    for (; anIndex < int(myMails.size()); ++anIndex)
    {
//      wsdebug(("  MailFolder::processMailStatus() setting mail %d to 0", anIndex));
      myMails[anIndex] = 0; // new Mail(this);
    } // for
  } // if

  wsinfo(("requesting status for '%s'...%s", aMailBoxString.c_str(), myHasStatus ? "OK" : "FAILED"));
  wsdebug(("< MailFolder::requestStatus() HasStatus: %d", myHasStatus));
  return myHasStatus;
} // MailFolder::requestStatus()

//-------------------------------------------------------------------------
//     MailFolder::requestSubFolders()
//-------------------------------------------------------------------------
bool MailFolder::requestSubFolders()
{
  if (!getSetup().supportsOption(CCO_FOLDER))
  {
    wswarn(("MailBox '%s' doesn't support folders!", getMailBoxString().c_str()));
    return false;
  } // if

  char aDelim = getMailBox()->getDelimiter();
  if (aDelim == '\0')
  {
    return false;
  } // if

//  wsdebug((">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"));
  wsdebug(("> MailFolder::requestSubFolders() %s  myFolders: %d", getMailBoxString().c_str(), mySubFolders.size()));
  bool fOK = true;


  //----- build a map of name <-> folders -------------------------------------
  std::map<std::string, MailFolder *> aFolderMap;

  wsdebug(("%08X  = this       %-20s  '%s'"
        , int(this)
        , getFolderName().c_str()
        , getFolderPath().c_str()
        ));

  if (mySubFolders.size() > 0)
  {
    wsdebug(("  MailFolder::requestSubFolders()  building folder map..."));
    wsdebug(("-----"));
    for (MailFolderCltn::const_iterator anIter  = mySubFolders.begin();
                                        anIter != mySubFolders.end();
                                      ++anIter)
    {
      MailFolder *aFolder = *anIter;
      wsdebug(("%08X  = aFolderMap[%-20s] '%s'"
            , int(aFolder)
            , aFolder->getFolderName().c_str()
            , aFolder->getFolderPath().c_str()
            ));

      aFolderMap[aFolder->getFolderPath()] = aFolder;
    } // for
    wsdebug(("-----"));
  } // if

  //----- request all folders -------------------------------------------------

  std::string anAccountStr   = getSetup().getAccountString();
  std::string aReference     = anAccountStr
                             + ( getFolderPath().length() > 0
                               ? getFolderPath() + std::string(1, getMailBox()->getDelimiter())
                               : std::string("")
                               )
                             ;
  std::string aPattern("%");

  wsdebug(("  MailFolder::requestSubFolders()  CurrentFolder: '%s'", getMailBox()->getCurrentFolder()->getFolderPath().c_str()));

  wsdebug(("  MailFolder::requestSubFolders()  AccountString: '%s'", anAccountStr    .c_str()));
  wsdebug(("  MailFolder::requestSubFolders()  FolderPath   : '%s'", getFolderPath() .c_str()));
  wsdebug(("  MailFolder::requestSubFolders()  FolderName   : '%s'", getFolderName() .c_str()));
  wsdebug(("  MailFolder::requestSubFolders()  Reference    : '%s'", aReference.c_str()));
  wsdebug(("  MailFolder::requestSubFolders()  Pattern      : '%s'", aPattern  .c_str()));

  wsinfo(("sending list('%s','%s') request ...", aReference.c_str(), aPattern.c_str()));

  RequestList aReq(aReference, aPattern, getMailBox(), getMailBoxString() + std::string(" -> MailFolder::reqestSubFolders()"));
  fOK = aReq.exec();
  if (!fOK)
  {
    wserror(("Unable to execute list-request for MailBox: '%s'!", getMailBoxString().c_str()));
  }
  else
  {
    for (FolderInfoCltn::const_iterator anIter  = aReq.getFolderCltn().begin();
                                        anIter != aReq.getFolderCltn().end();
                                      ++anIter)
    {
      const FolderInfo &aFI = *anIter;
      std::string       aSubFolderPath = aFI.Name;

      wsdebug(("- FullName  : %s", aSubFolderPath.c_str()));
      wsdebug(("- Current   : %s", getMailBox()->getCurrentFolder()->getFolderPath().c_str()));

      if ( aSubFolderPath.length() < anAccountStr.length()
        || aSubFolderPath.substr(0,  anAccountStr.length()) != anAccountStr
        )
      {
        wswarn(("MailBox::setupFolderTree() unable to parse '%s'", aSubFolderPath.c_str()));
        continue;
      } // if

      aSubFolderPath = aSubFolderPath.substr(anAccountStr.length());

      wsdebug(("- FolderPath: %s", aSubFolderPath.c_str()));
      wsdebug(("- Equal     : %d", getMailBox()->getCurrentFolder()->getFolderPath() == aSubFolderPath));

      // do we already have this sub folder?
      MailFolder *aSubFolder = aFolderMap[aSubFolderPath];

      // no? then: is the current folder perhaps the searched folder?
      if (aSubFolder == 0 && getMailBox()->getCurrentFolder() && aSubFolderPath == getMailBox()->getCurrentFolder()->getFolderPath())
      {
        aSubFolder = getMailBox()->getCurrentFolder();

        wsdebug(("%08X  Attr: %08lX  Folder is CURRENT FOLDER '%s'", int(aSubFolder), aFI.Attrib, aSubFolderPath.c_str()));

        aSubFolder->setParentFolder(this);   // integrate the current folder into the folder tree
        mySubFolders.push_back(aSubFolder);  // add the current folder to my collection of sub folders

        aFolderMap[aSubFolderPath] = aSubFolder;
      } // if

      // now: if we already have the folder -> update the attributes
      //      else create a new folder
      if (aSubFolder)
      {
        wsdebug(("%08X  Attr: %08lX  Folder already present: '%s'", int(aSubFolder), aFI.Attrib, aSubFolderPath.c_str()));
        aSubFolder->setAttributes(aFI.Attrib);
      }
      else
      {
        wsdebug(("%08X  Attr: %08lX  Folder creating...      '%s'", 0, aFI.Attrib, aSubFolderPath.c_str()));
        aSubFolder = new MailFolder(getMailBox(), aSubFolderPath, this, aFI.Attrib);
        mySubFolders.push_back(aSubFolder);
        aFolderMap[aSubFolderPath] = aSubFolder;
      } // if
    } // for

    // resort my collections of sub-folders
    std::sort(mySubFolders.begin(), mySubFolders.end(), MailFolderNameLessThan);
  } // if

  wsdebug(("< MailFolder::requestSubFolders() %s  myFolders: %d", getMailBoxString().c_str(), mySubFolders.size()));
//  wsdebug(("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"));
  return fOK;
} // MailFolder::requestSubFolders()

//-------------------------------------------------------------------------
//     MailFolder::open()
//-------------------------------------------------------------------------
bool MailFolder::open()
{
  std::string aMailBoxString = getMailBoxString();
  if (isOpen())
  {
    wswarn(("Folder %s is already open!", aMailBoxString.c_str()));
    return true;
  } // if

  wsdebug(("> MailFolder::open() %s  isOpen: %d", aMailBoxString.c_str(), isOpen()));

  //----- open the mailbox ------------------------------------------------
  RequestOpen aReq( aMailBoxString
                  , getMailBox()
                  , getMailBox()->getSetup().getAccountString() + " -> MailFolder::open()"
                  );
  myIsOpen = aReq.exec();
  if (!myIsOpen)
  {
    wserror(("opening mailbox '%s'...FAILED: %s", aMailBoxString.c_str(), getMailBox()->getLastImportantMsg().c_str()));
  }
  else
  {
    getMailBox()->getInternal().setCurrentFolder(this);
  } // if

  wsdebug(("< MailFolder::open() %s  isOpen: %d  ok: %d", aMailBoxString.c_str(), isOpen(), myIsOpen));
  return myIsOpen;
} // MailFolder::open()

//-------------------------------------------------------------------------
//     MailFolder::isOpen()
//-------------------------------------------------------------------------
bool MailFolder::isOpen() const
{
  bool fIsOpen = myIsOpen
              && getMailBox()->getCurrentFolder() == this
              && getMailBox()->isConnected()
               ;
//  wsdebug(("  MailFolder::isOpen() isOpen: %d  %s  ", fIsOpen, getMailBoxString().c_str()));
  return fIsOpen;
} // MailFolder::isOpen()

//-------------------------------------------------------------------------
//     MailFolder::setIsOpen()
//-------------------------------------------------------------------------
void MailFolder::setIsOpen(bool fIsOpen)
{
  if (fIsOpen != myIsOpen)
  {
    wsdebug(("- MailFolder::setIsOpen() isOpen: %d  %s", fIsOpen, getMailBoxString().c_str()));
    myIsOpen = fIsOpen;
  } // if
} // MailFolder::setIsOpen()

//-------------------------------------------------------------------------
//     MailFolder::close()
//-------------------------------------------------------------------------
void MailFolder::close()
{
  if (!isOpen())
  {
    wswarn(("Unable to close folder %s (not open)", getMailBoxString().c_str()));
    return;
  } // if

  wsdebug(("> MailFolder::close() %s  isOpen: %d", getMailBoxString().c_str(), isOpen()));

  RequestClose aReq( myMailBox
                   , getMailBox()->getSetup().getAccountString() + " -> MailFolder::close()"
                   );
  aReq.exec();
  myIsOpen = false;

  wsinfo(("closing mailbox '%s'...OK", getMailBoxString().c_str()));

  wsdebug(("  MailFolder::close() clearing..."));
  clear();

  wsdebug(("< MailFolder::close() %s  isOpen: %d", getMailBoxString().c_str(), isOpen()));
} // MailFolder::close()

//-------------------------------------------------------------------------
//     MailFolder::getSubFolderByName()
//-------------------------------------------------------------------------
MailFolder *MailFolder::getSubFolderByName(const std::string &theName) const
{
  MailFolder *aFolder = 0;

  for (MailFolderCltn::const_iterator anIter  = mySubFolders.begin();
                                      anIter != mySubFolders.end();
                                    ++anIter)
  {
    if ((*anIter)->getFolderName() == theName)
    {
      aFolder = *anIter;
      break;
    } // if
  } // for

  return aFolder;
} // MailFolder::getSubFolderByName()

//-------------------------------------------------------------------------
//     MailFolder::hasStatus()
//-------------------------------------------------------------------------
bool MailFolder::hasStatus() const
{
  return myHasStatus;
} // MailFolder::hasStatus()

//-------------------------------------------------------------------------
//     MailFolder::supportsFlag()
//-------------------------------------------------------------------------
bool MailFolder::supportsFlag(MailFlag theFlag) const
{
  bool fSupported = false;

  std::string aFlagName = "";

  if (getMailBox() && getMailBox()->getInternal().getStream())
  {
    MAILSTREAM *s = getMailBox()->getInternal().getStream();

    switch (theFlag)
    {
      case Ax::Mail::MF_SEEN    : fSupported = s->perm_seen    ; aFlagName = "\\seen"    ; break;
      case Ax::Mail::MF_FLAGGED : fSupported = s->perm_flagged ; aFlagName = "\\flagged" ; break;
      case Ax::Mail::MF_DELETED : fSupported = s->perm_deleted ; aFlagName = "\\deleted" ; break;
      case Ax::Mail::MF_ANSWERED: fSupported = s->perm_answered; aFlagName = "\\answered"; break;
    } // switch
  } // if

  if (aFlagName.length() > 0)
  {
    wsdebug(("Supports-Flag: %d  %d  %s", theFlag, fSupported, aFlagName.c_str()));
  } // if

  return fSupported;
} // MailFolder::supportsFlag()

//-------------------------------------------------------------------------
//     MailFolder::getCountMails()
//-------------------------------------------------------------------------
unsigned long MailFolder::getCountMails() const
{
  return myCountMails;
} // MailFolder::getCountMails()

//-------------------------------------------------------------------------
//     MailFolder::getCountUnseen()
//-------------------------------------------------------------------------
unsigned long MailFolder::getCountUnseen() const
{
  return myCountUnseen;
} // MailFolder::getCountUnseen()

//-------------------------------------------------------------------------
//     MailFolder::getCountRecent()
//-------------------------------------------------------------------------
unsigned long MailFolder::getCountRecent() const
{
  return myCountRecent;
} // MailFolder::getCountRecent()

//-------------------------------------------------------------------------
//     MailFolder::getUIDNext()
//-------------------------------------------------------------------------
unsigned long MailFolder::getUIDNext() const
{
  return myUIDNext;
} // MailFolder::getUIDNext()

//=========================================================================
//     mail access methods
//=========================================================================

//-------------------------------------------------------------------------
//     MailFolder::getMail()
//-------------------------------------------------------------------------
Mail *MailFolder::getMail(long theMsgNo)
{
  wsdebug(("> MailFolder::getMail() MsgNo: %ld", theMsgNo));

  long aMailIdx = theMsgNo - 1;

  if (aMailIdx < 0 || aMailIdx >= long(myMails.size()))
  {
    wserror(("mail number %ld not in [1..%ld] for mailbox '%s'...", theMsgNo, long(myMails.size()), getMailBoxString().c_str()));
    return 0;
  } // if

  Mail *aMail = myMails[aMailIdx];
  if (aMail == 0)
  {
    RequestFetchStructure aReqStructure(theMsgNo, getMailBox(), "FetchStructure");
    if (!aReqStructure.exec())
    {
      wserror(("getting structure mail %ld from %s...FAILED: %s", theMsgNo, getMailBoxString().c_str(), getMailBox()->getLastImportantMsg().c_str()));
    }
    else
    {
      RequestFetchMsgCache aReqMsgCache(theMsgNo, getMailBox(), "FetchMsgCache");
      if (!aReqMsgCache.exec())
      {
        wserror(("getting message-cache for mail %ld from %s...FAILED: %s", theMsgNo, getMailBoxString().c_str(), getMailBox()->getLastImportantMsg().c_str()));
      }
      else
      {
        aMail = new Mail(this, MsgCache(aReqMsgCache.getMsgCache()));
        aMail->requestHeader();
        myMails[aMailIdx] = aMail;
      }  // if
    } // if
  } // if

  wsdebug(("< MailFolder::getMail() Mail  : 0x%08X", int(aMail)));

  return aMail;
} // MailFolder::getMail()

//-------------------------------------------------------------------------
//     MailFolder::getNext()
//-------------------------------------------------------------------------
Mail *MailFolder::getNext(Mail *theMail)
{
  Mail *aMail = 0;

  if (getCountMails() == 0)
  {
    // nothing to do: return 0;
  }
  else if (getCountMails() == 1)
  {
    if (theMail == 0)
    {
      aMail = *(myMails.begin());
    }
    else
    {
      // nothing to do: return 0;
    } // if
  }
  else
  {
    MailVector::iterator anIter = std::find(myMails.begin(), myMails.end(), theMail);
    if (anIter != myMails.end())
    {
      anIter++;
      if (anIter != myMails.end())
      {
        aMail = *anIter;
      }
      else
      {
        // nothing to do: return 0;
      } // if
    }
    else
    {
      aMail = *(myMails.begin());
    } // if
  } // if

  return aMail;
} // MailFolder::getNext()

//-------------------------------------------------------------------------
//     MailFolder::getPrev()
//-------------------------------------------------------------------------
Mail *MailFolder::getPrev(Mail *theMail)
{
  Mail *aMail = 0;

  if (getCountMails() == 0)
  {
    // nothing to do: return 0;
  }
  else if (getCountMails() == 1)
  {
    if (theMail == 0)
    {
      aMail = *(myMails.begin());
    }
    else
    {
      // nothing to do: return 0;
    } // if
  }
  else
  {
    MailVector::reverse_iterator anIter = std::find(myMails.rbegin(), myMails.rend(), theMail);
    if (anIter != myMails.rend())
    {
      anIter++;
      if (anIter != myMails.rend())
      {
        aMail = *anIter;
      }
      else
      {
        // nothing to do: return 0;
      } // if
    }
    else
    {
      aMail = *(myMails.rbegin());
    } // if
  } // if

  return aMail;
} // MailFolder::getPrev()

//-------------------------------------------------------------------------
//     MailFolder::getNextNew()
//-------------------------------------------------------------------------
Mail *MailFolder::getNextNew(Mail *theMail)
{
  Mail *aMail = 0;

  if (getCountUnseen() == 0)
  {
    // nothing to do: return 0;
  }
  else if (getCountMails() == 0)
  {
    // nothing to do: return 0;
  }
  else if (getCountUnseen() == 1 && theMail && !theMail->getIsSeen())
  {
    // nothing to do: return 0;
  }
  else
  {
    //----- get an iterator for the given mail -----
    MailVector::iterator anIter = theMail
                                  ? std::find(myMails.begin(), myMails.end(), theMail)
                                  : myMails.begin()
                                  ;

    //----- if we found the given mal -> iterate one pos -----
    if (anIter != myMails.end())
    {
      anIter++;
    } // if

    //----- find the next mail which is not seen  -----
    for (; anIter != myMails.end() && *anIter && (*anIter)->getIsSeen(); ++anIter)
    {
      ;
    } // for

    if (anIter != myMails.end())
    {
      aMail = *anIter;
    } // if
  } // if
  return aMail;
} // MailFolder::getNextNew()

//-------------------------------------------------------------------------
//     MailFolder::getPrevNew()
//-------------------------------------------------------------------------
Mail *MailFolder::getPrevNew(Mail *theMail)
{
  Mail *aMail = 0;

  if (getCountUnseen() == 0)
  {
    // nothing to do: return 0;
  }
  else if (getCountMails() == 0)
  {
    // nothing to do: return 0;
  }
  else if (getCountUnseen() == 1 && theMail && !theMail->getIsSeen())
  {
    // nothing to do: return 0;
  }
  else
  {
    //----- get an iterator for the given mail -----
    MailVector::reverse_iterator anIter = theMail
                                          ? std::find(myMails.rbegin(), myMails.rend(), theMail)
                                          : myMails.rbegin()
                                          ;

    //----- if we found the given mal -> iterate one pos -----
    if (anIter != myMails.rend())
    {
      anIter++;
    } // if

    //----- find the next mail which is not seen  -----
    for (; anIter != myMails.rend() && *anIter && (*anIter)->getIsSeen(); ++anIter)
    {
      ;
    } // for

    if (anIter != myMails.rend())
    {
      aMail = *anIter;
    } // if
  } // if
  return aMail;
} // MailFolder::getPrevNew()

//-------------------------------------------------------------------------
//     MailFolder::getSurroundings()
//-------------------------------------------------------------------------
void MailFolder::getSurroundings( Mail * theMail
                             , Mail **thePrev
                             , Mail **theNext
                             , Mail **thePrevNew
                             , Mail **theNextNew
                             )
{
  wsdebug(("> MailFolder::getSurroundings() Mail: %4ld", theMail ? theMail->getMsgNo() : -1));

  if (thePrev   ) *thePrev    = 0;
  if (theNext   ) *theNext    = 0;
  if (thePrevNew) *thePrevNew = 0;
  if (theNextNew) *theNextNew = 0;

  if (getCountMails() == 0)
  {
    return;
  } // if

  //----- get an iterator for the given mail -----
  MailVector::iterator aStartIter = theMail
                                  ? std::find(myMails.begin(), myMails.end(), theMail)
                                  : myMails.begin()
                                  ;

  //----- find the prev -----
  if (thePrev != 0 && aStartIter != myMails.begin())
  {
    *thePrev = *(aStartIter - 1);
  } // if

  //----- find the next -----
  if (theNext != 0 && aStartIter != myMails.end() && aStartIter + 1 != myMails.end())
  {
    *theNext = *(aStartIter + 1);
  } // if

  //----- find the prev new -----
  if (thePrevNew != 0 && getCountUnseen() != 0 && aStartIter != myMails.begin())
  {
    MailVector::iterator aPrevIter;

    for ( aPrevIter  = aStartIter - 1;
          aPrevIter != myMails.begin() && *aPrevIter && (*aPrevIter)->getIsSeen();
        --aPrevIter)
    {
    } // for

    // not aPrevIter points to a new mail or to the first mail
    if      (aPrevIter != myMails.begin()) *thePrevNew = *aPrevIter;
    else if (!(*aPrevIter)->getIsSeen())   *thePrevNew = *aPrevIter;
  } // if

  //----- find the next new -----
  if (theNextNew != 0 && getCountUnseen() != 0 && aStartIter != myMails.end())
  {
    MailVector::iterator aNextIter;

    for ( aNextIter  = aStartIter + 1;
          aNextIter != myMails.end() && *aNextIter && (*aNextIter)->getIsSeen();
        ++aNextIter)
    {
    } // for

    if (aNextIter != myMails.end()) *theNextNew = *aNextIter;
  } // if

  wsdebug(("< MailFolder::getSurroundings() Mail: %4ld  Prev: %4ld  Next: %4ld  PrevNew: %4ld  NextNew: %4ld"
         , theMail                  ? theMail      ->getMsgNo() : -1
         , thePrev    ? *thePrev    ? (*thePrev   )->getMsgNo() : -2 : -1
         , theNext    ? *theNext    ? (*theNext   )->getMsgNo() : -2 : -1
         , thePrevNew ? *thePrevNew ? (*thePrevNew)->getMsgNo() : -2 : -1
         , theNextNew ? *theNextNew ? (*theNextNew)->getMsgNo() : -2 : -1
         ));
} // MailFolder::getSurroundings()

//=========================================================================
//     Atttributes of the folder
//=========================================================================
/*
*
*  -----------------------
*  from internal.txt
*  -----------------------
*
*  LATT_NOINFERIORS it is not possible for there to be any
*                   hierarchy inferiors to this name (that is,
*                   this name followed by the hierarchy delimiter
*                   and additional name characters).
*
*  LATT_NOSELECT    this is not a mailbox name, just a hierarchy
*                   level, and it may not be opened by mail_open()
*
*  LATT_MARKED      this mailbox may have recent messages
*
*  LATT_UNMARKED    this mailbox does not have any recent messages
*
*  -----------------------
*  From c-client.h:
*  -----------------------
*  Bits for mm_list() and mm_lsub()
*
*  Note that (LATT_NOINFERIORS LATT_HASCHILDREN LATT_HASNOCHILDREN) and
* (LATT_NOSELECT LATT_MARKED LATT_UNMARKED) each have eight possible states,
* but only four of these are valid.  The other four are silly states which
* while invalid can unfortunately be expressed in the IMAP protocol.
*
* #define LATT_NOINFERIORS   (long) 0x01    terminal node in hierarchy
* #define LATT_HASCHILDREN   (long) 0x20    has selectable inferiors
* #define LATT_HASNOCHILDREN (long) 0x40    has no selectable inferiors
*
* #define LATT_NOSELECT      (long) 0x02    name can not be selected
* #define LATT_MARKED        (long) 0x04    changed since last accessed
* #define LATT_UNMARKED      (long) 0x08    accessed since last changed
*
* #define LATT_REFERRAL      (long) 0x10    name has referral to remote mailbox
*
*/

//-------------------------------------------------------------------------
//     MailFolder::canSelect()
//-------------------------------------------------------------------------
bool MailFolder::canSelect() const
{
  return getMailBox()->getRootFolder() == this
      || (hasAttributes() && ((myAttributes & LATT_NOSELECT) == 0))
       ;
} // MailFolder::canSelect()

//-------------------------------------------------------------------------
//     MailFolder::isNode()
//-------------------------------------------------------------------------
bool MailFolder::isNode() const
{
  return getMailBox()->getRootFolder() == this
      || (hasAttributes() && ((getAttributes() & LATT_HASCHILDREN) != 0))
      || (hasAttributes() && ((getAttributes() & LATT_NOSELECT   ) != 0))  // we assume a not selectable Folder has children
       ;
} // MailFolder::isNode()


//=============================================================================
//     namespaces
//=============================================================================
} // Mail
} // Ax
