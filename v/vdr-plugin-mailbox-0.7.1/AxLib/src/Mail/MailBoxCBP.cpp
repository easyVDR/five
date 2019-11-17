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
//----- AxLib -----------------------------------------------------------------
#include <Ax/Tools/Macros.h>
#include <Ax/Tools/Trace.h>
// #include <Ax/Tools/TraceOn.h>
#include <Ax/Mail/MailBox.h>
#include <Ax/Mail/MailBoxSetup.h>

//----- vdr -------------------------------------------------------------------
//----- local plugin ----------------------------------------------------------
//----- locals ----------------------------------------------------------------
#include "MailBoxCBP.h"

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
//     class MailBoxCBP
//=============================================================================

//-------------------------------------------------------------------------
//     MailBoxCBP::MailBoxCBP()
//-------------------------------------------------------------------------
MailBoxCBP::MailBoxCBP( LogHandler        &theLogHandler
                      , MailBox           *theMailBox
                      , const std::string &theName
                      , CallBackProcessor *theParent
                      )
          : PARENT   (theLogHandler, theMailBox, theName, theParent)
{
  wsdebug(("> MailBoxCBP::MailBoxCBP() MailBox: %s", theMailBox ? theMailBox->getSetup().getMailBoxString().c_str(): "NULL"));
  wsdebug(("< MailBoxCBP::MailBoxCBP() MailBox: %s", theMailBox ? theMailBox->getSetup().getMailBoxString().c_str(): "NULL"));
} // MailBoxCBP::MailBoxCBP()

//-------------------------------------------------------------------------
//     MailBoxCBP::~MailBoxCBP()
//-------------------------------------------------------------------------
MailBoxCBP::~MailBoxCBP()
{
  wsdebug(("> MailBoxCBP::~MailBoxCBP()"));
  wsdebug(("< MailBoxCBP::~MailBoxCBP()"));
} // MailBoxCBP::~MailBoxCBP()

//-------------------------------------------------------------------------
//     MailBoxCBP::process_mm_login()
//-------------------------------------------------------------------------
bool MailBoxCBP::process_mm_login(NETMBX *theMailBox, char *theUser, char *thePassword, long theTrials)
{
  wsdebug(("> MailBoxCBP::process_mm_login() MailBox: %s", getMailBox() ? getMailBox()->getSetup().getMailBoxString().c_str() : "NULL"));

  AX_UNUSED_ARG(theMailBox);
  AX_UNUSED_ARG(theTrials );

  if (getMailBox())
  {
    strcpy(theUser    , getMailBox()->getSetup().getUserName().c_str());
    strcpy(thePassword, getMailBox()->getSetup().getPassword().c_str());
  }
  else
  {
    wserror(("MailBoxCBP::process_mm_login() no MailBox present!"));
  } // if
  wsdebug(("< MailBoxCBP::process_mm_login() MailBox: %s", getMailBox() ? getMailBox()->getSetup().getMailBoxString().c_str() : "NULL"));
  return T;
} // MailBoxCBP::process_mm_login()

//=============================================================================
//     namespaces
//=============================================================================
} // Mail
} // Ax
