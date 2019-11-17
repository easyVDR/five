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
#include <Ax/Tools/Globals.h>
#include <Ax/Mail/MailFolder.h>
#include <Ax/Mail/MailBox.h>
#include <Ax/Mail/MailBoxMgr.h>

//----- vdr -------------------------------------------------------------------
//----- c-client --------------------------------------------------------------
//----- local -----------------------------------------------------------------
#include "MailBoxInternal.h"
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
//     class MailBox
//=============================================================================

//-----------------------------------------------------------------------------
//     MailBox::MailBox()
//-----------------------------------------------------------------------------
MailBox::MailBox(const MailBoxSetup &theSetup)
       : myInternal         (new MailBoxInternal(this, theSetup))
{
  wsdebug(("> MailBox::MailBox() AccountStr: '%s'", theSetup.getAccountString().c_str()));

  if (theSetup.getMailBoxString() == "")
  {
    theSetup.dump();
    wswarn(("MailBox::MailBox() MailBoxString is empty, THIS IS NOT ALLOWED!"));
  } // if

  wsdebug(("< MailBox::MailBox() AccountStr: '%s'", theSetup.getAccountString().c_str()));
} // MailBox::MailBox()

//-----------------------------------------------------------------------------
//     MailBox::~MailBox()
//-----------------------------------------------------------------------------
MailBox::~MailBox()
{
  wsdebug(("> MailBox::~MailBox()"));

  wsdebug(("  MailBox::~MailBox() deleting my private data..."));
  delete myInternal;
  myInternal = 0;

  wsdebug(("< MailBox::~MailBox()"));
} // MailBox::~MailBox()

//-------------------------------------------------------------------------
//     MailBox::getInternal()
//-------------------------------------------------------------------------
MailBoxInternal &MailBox::getInternal() const
{
  return *myInternal;
} // MailBox::getInternal()

//-------------------------------------------------------------------------
//     MailBox::getSetup()
//-------------------------------------------------------------------------
const MailBoxSetup &MailBox::getSetup() const
{
  return getInternal().getSetup();
} // MailBox::getSetup()

//---------------------------------------------------------------------
//     MailBox::getLogHandler()
//---------------------------------------------------------------------
LogHandler &MailBox::getLogHandler() const
{
  return getInternal().getLogHandler();
} // MailBox::getLogHandler()

//---------------------------------------------------------------------
//     MailBox::getLastImportantMsg()
//---------------------------------------------------------------------
std::string MailBox::getLastImportantMsg() const
{
  return getLogHandler().getLastImportantMsg();
} // MailBox::getLastImportantMsg()

//-------------------------------------------------------------------------
//     MailBox::getRootFolder()
//-------------------------------------------------------------------------
MailFolder *MailBox::getRootFolder() const
{
  return getInternal().getRootFolder();
} // MailBox::getRootFolder()

//-------------------------------------------------------------------------
//     MailBox::getCurrentFolder()
//-------------------------------------------------------------------------
MailFolder *MailBox::getCurrentFolder() const
{
  return getInternal().getCurrentFolder();
} // MailBox::getCurrentFolder()

//-------------------------------------------------------------------------
//     MailBox::isConnected()
//-------------------------------------------------------------------------
bool MailBox::isConnected() const
{
  return getInternal().getStream() != NIL;
} // MailBox::isConnected()

//-------------------------------------------------------------------------
//     MailBox::supportsFolders()
//-------------------------------------------------------------------------
bool MailBox::supportsFolders() const
{
  return getSetup().supportsOption(CCO_FOLDER);
} // MailBox::supportsFolders()

//-------------------------------------------------------------------------
//     MailBox::isIMAP()
//-------------------------------------------------------------------------
bool MailBox::isIMAP() const
{
  return getSetup().getAccountType() == AT_IMAP;
} // MailBox::isIMAP()

//-------------------------------------------------------------------------
//     MailBox::isPOP3()
//-------------------------------------------------------------------------
bool MailBox::isPOP3() const
{
  return getSetup().getAccountType() == AT_POP3;
} // MailBox::isPOP3()

//-------------------------------------------------------------------------
//     MailBox::isNNTP()
//-------------------------------------------------------------------------
bool MailBox::isNNTP() const
{
  return getSetup().getAccountType() == AT_NNTP;
} // MailBox::isNNTP()

//-------------------------------------------------------------------------
//     MailBox::getAccountName()
//-------------------------------------------------------------------------
const std::string &MailBox::getAccountName() const
{
  return getSetup().getAccountName();
} // MailBox::getAccountName()

//-------------------------------------------------------------------------
//     MailBox::getDelimiter()
//-------------------------------------------------------------------------
char MailBox::getDelimiter() const
{
  return getInternal().getDelimiter();
} // MailBox::getDelimiter()

//-------------------------------------------------------------------------
//     MailBox::hasDelimiter()
//-------------------------------------------------------------------------
bool MailBox::hasDelimiter() const
{
  return getInternal().hasDelimiter();
} // MailBox::getDelimiter()

//=============================================================================
//     namespaces
//=============================================================================
} // Mail
} // Ax
