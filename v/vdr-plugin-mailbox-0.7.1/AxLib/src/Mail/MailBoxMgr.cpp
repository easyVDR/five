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
#include <Ax/Mail/MailBox.h>
#include <Ax/Mail/MailFolder.h>
#include <Ax/Mail/MailBoxMgr.h>

//----- vdr -------------------------------------------------------------------
//----- locals ----------------------------------------------------------------
#include "c-client-header.h"
#include "CallBackDispatcher.h"
#include "LogHandlerCBP.h"

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
//     externals
//=============================================================================

//-------------------------------------------------------------------------
//     dumpParameters()
//-------------------------------------------------------------------------
void dumpParameters()
{
  wsinfo(("----- > dumpParameters() ------------------------------------"));

  void *aRetVal = 0;

  aRetVal = mail_parameters(0, GET_OPENTIMEOUT , 0);  wsinfo(("c-client: OPENTIMEOUT : %ld", (long)aRetVal));
  aRetVal = mail_parameters(0, GET_READTIMEOUT , 0);  wsinfo(("c-client: READTIMEOUT : %ld", (long)aRetVal));
  aRetVal = mail_parameters(0, GET_WRITETIMEOUT, 0);  wsinfo(("c-client: WRITETIMEOUT: %ld", (long)aRetVal));
  aRetVal = mail_parameters(0, GET_CLOSETIMEOUT, 0);  wsinfo(("c-client: CLOSETIMEOUT: %ld", (long)aRetVal));

  wsdebug(("----- < dumpParameters() ------------------------------------"));
} // dumpParameters()

//=============================================================================
//     class MailBoxMgr
//=============================================================================

//-------------------------------------------------------------------------
//     globals
//-------------------------------------------------------------------------
MailBoxMgr *MailBoxMgr::myInstance             = 0;
bool        MailBoxMgr::myIsCClientInitialized = false;

//-------------------------------------------------------------------------
//     MailBoxMgr::MailBoxMgr()
//-------------------------------------------------------------------------
MailBoxMgr::MailBoxMgr()
          : myCallBackDispatcher(0)
          , myCallBackProcessor (0)
          , myLogHandler("FINAL")
{
  wsdebug(("> MailBoxMgr::MailBoxMgr()"));

  if (!myIsCClientInitialized)
  {
    // required for c-client-library: register mailbox drivers
#   include <linkage.c>
    myIsCClientInitialized = true;
  } // if

//  myLogHandler.setMaxSize(mySetup.getClientOptDbg() == CC_DBG_YES ? 100 : 1);

//  myCallBackDispatcher and myCallbackProcessor can't be created here!

  wsdebug(("< MailBoxMgr::MailBoxMgr()"));
} // MailBoxMgr::MailBoxMgr()

//-------------------------------------------------------------------------
//     MailBoxMgr::~MailBoxMgr()
//-------------------------------------------------------------------------
MailBoxMgr::~MailBoxMgr()
{
  wsdebug(("> MailBoxMgr::~MailBoxMgr()"));
  Ax::Tools::DeleteNull(myCallBackProcessor );
  Ax::Tools::DeleteNull(myCallBackDispatcher);
  wsdebug(("< MailBoxMgr::~MailBoxMgr()"));
} // MailBoxMgr::~MailBoxMgr()

//-------------------------------------------------------------------------
//     MailBoxMgr::instance()
//-------------------------------------------------------------------------
MailBoxMgr *MailBoxMgr::instance()
{
//  wsdebug(("> MailBoxMgr::instance() myInstance: %p", myInstance));
  if (myInstance == 0)
  {
    wsdebug(("> MailBoxMgr::instance() creating instance..."));
    myInstance = new MailBoxMgr();
    wsdebug(("< MailBoxMgr::instance() creating instance...done"));

    myInstance->myCallBackDispatcher = new CallBackDispatcher();
    myInstance->myCallBackProcessor  = new LogHandlerCBP( myInstance->myLogHandler
                                                        , 0 /* no MailBox */
                                                        , "MailBoxMgrCBP"
                                                        , 0 /* no parent */
                                                        );
  } // if

//  wsdebug(("< MailBoxMgr::instance() myInstance: %p", myInstance));
  return myInstance;
} // MailBoxMgr::instance()

//-------------------------------------------------------------------------
//     MailBoxMgr::terminate()
//-------------------------------------------------------------------------
void MailBoxMgr::terminate()
{
  wsdebug(("> MailBoxMgr::terminate() myInstance: %p", myInstance));

  if (myInstance)
  {
    //----- destroy myself ----------------
    wsdebug(("  MailBoxMgr::terminate() deleting myInstance..."));
    MailBoxMgr *aTmp = myInstance;
    myInstance = 0;
    delete aTmp;
  } // if

  wsdebug(("< MailBoxMgr::terminate() myInstance: %p", myInstance));
} // MailBoxMgr::terminate()

//---------------------------------------------------------------------
//     MailBoxMgr::setTimeOut()
//---------------------------------------------------------------------
void MailBoxMgr::setTimeOut(long theTimeOut)
{
  wsdebug(("> MailBoxMgr::setTimeOut()"));

//  dumpParameters();

  // be save against changes of the parameter type
  long aTimeOut = theTimeOut;

  mail_parameters(0, SET_OPENTIMEOUT , (void *)aTimeOut);
  mail_parameters(0, SET_READTIMEOUT , (void *)aTimeOut);
  mail_parameters(0, SET_WRITETIMEOUT, (void *)aTimeOut);
//  mail_parameters(0, SET_CLOSETIMEOUT, (void *)aTimeOut);

//  dumpParameters();

  wsdebug(("< MailBoxMgr::setTimeOut()"));
} // MailBoxMgr::setTimeOut()

//---------------------------------------------------------------------
//     MailBoxMgr::getCBD()
//---------------------------------------------------------------------
Ax::Mail::CallBackDispatcher *MailBoxMgr::getCBD() const
{
  return myCallBackDispatcher;
} // MailBoxMgr::getCBD()

//---------------------------------------------------------------------
//     MailBoxMgr::getCBP()
//---------------------------------------------------------------------
Ax::Mail::CallBackProcessor  *MailBoxMgr::getCBP()  const
{
  return myCallBackProcessor;
} // MailBoxMgr::getCBP()

//---------------------------------------------------------------------
//     MailBoxMgr::getLogHandler()
//---------------------------------------------------------------------
LogHandler &MailBoxMgr::getLogHandler()
{
  return myLogHandler;
} // MailBoxMgr::getLogHandler()


//=============================================================================
//     namespaces
//=============================================================================
} // Mail
} // Ax
