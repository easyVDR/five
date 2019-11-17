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
#include <ctype.h>

//----- AxLib -----------------------------------------------------------------
#include <Ax/Tools/Trace.h>
// #include <Ax/Tools/TraceOn.h>
#include <Ax/Tools/Globals.h>
#include <Ax/Mail/MailFolder.h>
#include <Ax/Mail/MailBoxMgr.h>
#include <Ax/Mail/MailBox.h>

//----- vdr -------------------------------------------------------------------
//----- c-client --------------------------------------------------------------
//----- local -----------------------------------------------------------------
#include "MailBoxInternal.h"
#include "MailBoxCBP.h"
#include "Request.h"

//=============================================================================
//     namespaces
//=============================================================================
namespace Ax   {
namespace Mail {

//=============================================================================
//     class MailBoxInternal
//=============================================================================

//-------------------------------------------------------------------------
//     MailBoxInternal::MailBoxInternal()
//-------------------------------------------------------------------------
MailBoxInternal::MailBoxInternal(MailBox *theMailBox, const MailBoxSetup &theSetup)
               : myMailBox      (theMailBox)
               , mySetup        (theSetup)
               , myLogHandler   ("MailBox")
               , myStream       (0)
               , myCBP          (0)
               , myRootFolder   (0)
               , myCurrentFolder(0)
               , myDelimiter      ('\0')
               , myDelimRequFailed(false)
{
  myLogHandler.setMaxSize(theSetup.getClientOptDbg() == CC_DBG_YES ? 100 : 1);

  myCurrentFolder = new MailFolder(myMailBox, mySetup.getFolderPath(), 0);

  wsdebug(("  MailBoxInternal::MailBoxInternal() FolderPath  '%s'", theSetup.getFolderPath().c_str()));

  if (mySetup.getFolderPath() == "")
  {
    wsdebug(("  MailBoxInternal::MailBoxInternal() CurrentFolder == RootFolder!"));
    myRootFolder = myCurrentFolder;
  }
  else
  {
    // RootFolder not created because not needed (up right now)
  } // if

  myCBP = new MailBoxCBP( myLogHandler
                        , myMailBox
                        , "MailBoxProcessor"
                        , MBM->getCBP()
                        );

} // MailBoxInternal::MailBoxInternal()

//-------------------------------------------------------------------------
//     MailBoxInternal::~MailBoxInternal()
//-------------------------------------------------------------------------
MailBoxInternal::~MailBoxInternal()
{
  if (myCurrentFolder && myCurrentFolder->isOpen())
  {
    wsdebug(("  MailBoxInternal::~MailBoxInternal() myCurrentFolder->isOpen() -> closing..."));
    myCurrentFolder->close();
  } // if

  bool fDeleteMyCurrent = myCurrentFolder && myCurrentFolder != myRootFolder;

  if (myRootFolder)
  {
    // delete complete folder hierarchy
    // note: myCurrentFolder will not get destroyed here
    wsdebug(("  MailBoxInternal::~MailBoxInternal() deleting root folder..."));
    MailFolder *aFolder = myRootFolder;
    myRootFolder = 0;
    delete aFolder;
  } // if

  if (fDeleteMyCurrent)
  {
    wsdebug(("  MailBoxInternal::~MailBoxInternal() deleting current folder"));
    MailFolder *aFolder = myCurrentFolder;
    myCurrentFolder = 0;
    delete aFolder;
  } // if

  wsdebug(("  MailBoxInternal::~MailBoxInternal() deleting my callback-processor..."));
  Ax::Tools::DeleteNull(myCBP);

} // MailBoxInternal::~MailBoxInternal()

//-------------------------------------------------------------------------
//     MailBoxInternal::getSetup()
//-------------------------------------------------------------------------
const MailBoxSetup &MailBoxInternal::getSetup() const
{
  return mySetup;
} // MailBoxInternal::getSetup()

//---------------------------------------------------------------------
//     MailBoxInternal::getLogHandler()
//---------------------------------------------------------------------
LogHandler &MailBoxInternal::getLogHandler()
{
  return myLogHandler;
} // MailBoxInternal::getLogHandler()

//-------------------------------------------------------------------------
//     MailBoxInternal::getStream()
//-------------------------------------------------------------------------
MAILSTREAM *MailBoxInternal::getStream() const
{
  return myStream;
} // MailBoxInternal::getStream()

//-------------------------------------------------------------------------
//     MailBoxInternal::setStream()
//-------------------------------------------------------------------------
void MailBoxInternal::setStream(MAILSTREAM *theStream)
{
  myStream = theStream;
} // MailBoxInternal::setStream()

//-------------------------------------------------------------------------
//     MailBoxInternal::getCBP()
//-------------------------------------------------------------------------
CallBackProcessor *MailBoxInternal::getCBP() const
{
  return myCBP;
} // MailBoxInternal::getCBP()

//-------------------------------------------------------------------------
//     MailBoxInternal::getRootFolder()
//-------------------------------------------------------------------------
MailFolder *MailBoxInternal::getRootFolder() const
{
  if (myRootFolder == 0)
  {
    myRootFolder = new MailFolder(myMailBox, "", 0);
  } // if

  return myRootFolder;
} // MailBoxInternal::getRootFolder()

//-------------------------------------------------------------------------
//     MailBoxInternal::getCurrentFolder()
//-------------------------------------------------------------------------
MailFolder *MailBoxInternal::getCurrentFolder() const
{
  return myCurrentFolder;
} // MailBoxInternal::getCurrentFolder()

//-------------------------------------------------------------------------
//     MailBoxInternal::setCurrentFolder()
//-------------------------------------------------------------------------
void MailBoxInternal::setCurrentFolder(MailFolder *theFolder)
{
  if (theFolder == 0)
  {
    wswarn(("  MailBoxInternal::setCurrentFolder() theFolder == 0 -> setting to root folder!"));
    theFolder = myRootFolder;
  } // if

  if (theFolder != myCurrentFolder)
  {
    if (myCurrentFolder)
    {
      if (myCurrentFolder->getParentFolder() == 0 && myCurrentFolder != myRootFolder)
      {
        wsdebug(("  MailBoxInternal::setCuttentFolder() deleting current folder because not in tree!!!!"));
        MailFolder *aFolder = myCurrentFolder;
        myCurrentFolder = 0;
        delete aFolder;
      }
      else if (myCurrentFolder->isOpen())
      {
        wsdebug(("  MailBoxInternal::setCurrentFolder() old: %s -> setting to isOpen == FALSE !!!!!!"
               , myCurrentFolder->getMailBoxString().c_str()
               ));
        myCurrentFolder->setIsOpen(false);
      } // if
    } // if
    myCurrentFolder = theFolder;
  } // if
} // MailBoxInternal::setCurrentFolder()

//-------------------------------------------------------------------------
//     MailBoxInternal::getDelimiter()
//-------------------------------------------------------------------------
char MailBoxInternal::getDelimiter()
{
  if ( myDelimiter == '\0'
    && myMailBox->supportsFolders()
    && !myDelimRequFailed
     )
  {
    std::string anAccountStr = getSetup().getAccountString();

    wsdebug(("  MailBoxInternal::getDelimiter()  MailBoxString: '%s'", getSetup().getMailBoxString().c_str()));
    wsdebug(("  MailBoxInternal::getDelimiter()  AccountString: '%s'", getSetup().getAccountString().c_str()));
    wsdebug(("  MailBoxInternal::getDelimiter()  FolderPath   : '%s'", getSetup().getFolderPath   ().c_str()));

    wsinfo(("requesting delimiter for '%s'...", anAccountStr.c_str()));

    RequestList aReq( anAccountStr
                    , ""
                    , myMailBox
                    , getSetup().getAccountString() + std::string(" -> MailBox::getDelimiter()")
                    );

    bool fOK = aReq.exec();
    if (!fOK)
    {
      myDelimRequFailed = true;
      wserror(("unable to request delimiter for '%s'", getSetup().getMailBoxString().c_str()));
    }
    else
    {
      myDelimiter = aReq.getDelimiter();

      // implicitely create the root-folder and assign the attributes to it
      if (aReq.getFolderCltn().size() > 0)
      {
        getRootFolder()->setAttributes(aReq.getFolderCltn().begin()->Attrib);
      } // if

      wsinfo(("delimiter for '%s' is %d = '%c'"
            , anAccountStr.c_str()
            , getDelimiter()
            , isascii(getDelimiter()) ? getDelimiter() : '\0'
            ));
    } // if

  } // if

  return myDelimiter;
} // MailBoxInternal::getDelimiter()

//-------------------------------------------------------------------------
//     MailBoxInternal::hasDelimiter()
//-------------------------------------------------------------------------
bool MailBoxInternal::hasDelimiter() const
{
  return myDelimiter != '\0';
} // MailBoxInternal::getDelimiter()

//=============================================================================
//     namespaces
//=============================================================================
} // Mail
} // Ax
