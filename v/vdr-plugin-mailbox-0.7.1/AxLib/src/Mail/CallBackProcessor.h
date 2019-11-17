#ifndef __Ax_Mail_CallBackProcessor_H__
#define __Ax_Mail_CallBackProcessor_H__
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

//----- C ---------------------------------------------------------------------
//----- AxLib -----------------------------------------------------------------
#include <Ax/Mail/Globals.h>
#include <Ax/Mail/LogHandler.h>

//----- local -----------------------------------------------------------------
#include "c-client-header.h"

//----- vdr -------------------------------------------------------------------
//----- c-client --------------------------------------------------------------
//----- other includes --------------------------------------------------------

//=============================================================================
//     namespaces
//=============================================================================
namespace Ax   {
namespace Mail {

//=============================================================================
//     forward declarations
//=============================================================================
class CallBackProcessor;
class MailBox;

//=============================================================================
//     class CallBackLock
//=============================================================================
/** CallBackLock registers the given Mailbox as active / deactive at the manager
 */
class CallBackLock
{
  public:
    //-------------------------------------------------------------------------
    //     CallBackLock()
    //-------------------------------------------------------------------------
    /** Constructor
    */
    CallBackLock(CallBackProcessor *theCallBackProcessor);

    //-------------------------------------------------------------------------
    //     ~CallBackLock()
    //-------------------------------------------------------------------------
    /** Destructor
     */
    ~CallBackLock();

  private:
    //-------------------------------------------------------------------------
    //     attributes
    //-------------------------------------------------------------------------
    CallBackProcessor *myCallBackProcessor;

}; // class CallBackLock

//=============================================================================
//     class CallBackProcessor
//=============================================================================
/**
 *  Parent-Child-relationship of CallBackProcessor-instances form some
 *  kind of 'chain-of-responsibility': If a given instance of CallBackProcessor
 *  isn't able to process a request, the request is forwarded to its
 *  parent and so on. The parent of all instances of CallBackProcessor (even
 *  if not explicitly given) is the MailBoxMgr::instance()->getCallBackProcessor()
 *  which is a CallBackProcessorFinal.
 */
class CallBackProcessor
{
  public:
    //-------------------------------------------------------------------------
    //     CallBackProcessor()
    //-------------------------------------------------------------------------
    /** Constructor
     */
    CallBackProcessor( MailBox           *theMailBox
                     , const std::string &theName
                     , CallBackProcessor *theParent
                     );

    //-------------------------------------------------------------------------
    //     ~CallBackProcessor()
    //-------------------------------------------------------------------------
    /** Destructor
     */
    virtual ~CallBackProcessor();

    //-------------------------------------------------------------------------
    //     getName()
    //-------------------------------------------------------------------------
    const std::string &getName() const { return myName; }

    //-------------------------------------------------------------------------
    //     getParent()
    //-------------------------------------------------------------------------
    virtual CallBackProcessor *getParent() const;

    //-------------------------------------------------------------------------
    //     getMailBox()
    //-------------------------------------------------------------------------
    virtual MailBox *getMailBox() const { return myMailBox; }

    //-------------------------------------------------------------------------
    //     isOK()
    //-------------------------------------------------------------------------
    bool isOK() const { return myIsOK; }

    //-------------------------------------------------------------------------
    //     getStream()
    //-------------------------------------------------------------------------
    virtual MAILSTREAM *getStream() const;

    //---------------------------------------------------------------------
    //     log()
    //---------------------------------------------------------------------
    virtual bool log(LogLevel theLevel, const std::string &theMsg);

  protected:
    //-------------------------------------------------------------------------
    //     setIsOK()
    //-------------------------------------------------------------------------
    void setIsOK(bool fOK);

  public:
    //=========================================================================
    //     process c-client callbacks
    //=========================================================================
    virtual bool process_mm_flags     (unsigned long theMsgNum);
    virtual bool process_mm_status    (const std::string &theMailBoxString, const MAILSTATUS *theStatus);
    virtual bool process_mm_searched  (unsigned long theMsgNum);
    virtual bool process_mm_exists    (unsigned long theMsgNum);
    virtual bool process_mm_expunged  (unsigned long theMsgNum);
    virtual bool process_mm_list      (char theDelim, const std::string &theMailBoxString, long theAttrib);
    virtual bool process_mm_lsub      (char theDelim, const std::string &theMailBoxString, long theAttrib);

    virtual bool process_mm_notify    (const std::string &theMsg, long theErrorFlag);
    virtual bool process_mm_log       (const std::string &theMsg, long theErrorFlag);
    virtual bool process_mm_dlog      (const std::string &theMsg);
    virtual bool process_mm_fatal     (const std::string &theMsg);

    virtual bool process_mm_login     (NETMBX *theMailBox, char *theUser, char *thePassword, long theTrials);

    virtual bool process_mm_critical  ();
    virtual bool process_mm_nocritical();

    virtual bool process_mm_diskerror (unsigned long theOSErrorCode, unsigned long theIsSerious);

  private:
    //-------------------------------------------------------------------------
    //     attributes
    //-------------------------------------------------------------------------
    MailBox           *myMailBox;
    CallBackProcessor *myParent;
    std::string        myName;
    bool               myIsOK;

}; // class CallBackProcessor

//=============================================================================
//     namespaces
//=============================================================================
} // Mail
} // Ax

#endif

