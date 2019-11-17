#ifndef __Ax_Mail_CallBackDispatcher_H__
#define __Ax_Mail_CallBackDispatcher_H__
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
#include <Ax/Tools/Globals.h>
#include <Ax/Mail/Globals.h>
#include <Ax/Mail/MailBox.h>

//----- vdr -------------------------------------------------------------------
//----- c-client --------------------------------------------------------------
//----- other includes --------------------------------------------------------
//----- local -----------------------------------------------------------------
#include "CallBackProcessor.h"

//=============================================================================
//     namespaces
//=============================================================================
namespace Ax   {
namespace Mail {

//=============================================================================
//     class CallBackDispatcher
//=============================================================================
/** The singleton CallBackDispatcher is responsible for all instances of MailBox.
 *
 *  The main purpose of this class is to demultiplex answers of \c c-client /
 *  from the mail-server and to forward them to the associated instance
 *  of MailBox.
 */
class CallBackDispatcher
{
    //-------------------------------------------------------------------------
    //     friends
    //-------------------------------------------------------------------------
    friend class CallBackLock;

  public:
    //-------------------------------------------------------------------------
    //     CallBackDispatcher()
    //-------------------------------------------------------------------------
    /** Constructor
    */
    CallBackDispatcher();

    //-------------------------------------------------------------------------
    //     ~CallBackDispatcher()
    //-------------------------------------------------------------------------
    /** Destructor
     */
    ~CallBackDispatcher();

    //=========================================================================
    //     process c-client callbacks
    //=========================================================================
    // don't overwrite one of these process_*-methods,
    // instead the int_*-methods should be overwritten if necessary
    //-------------------------------------------------------------------------
    void process_mm_flags     (const MAILSTREAM *theStream, unsigned long theMsgNum);
    void process_mm_status    (const MAILSTREAM *theStream, const std::string &theMailBoxString, const MAILSTATUS *theStatus);
    void process_mm_searched  (const MAILSTREAM *theStream, unsigned long theMsgNum);
    void process_mm_exists    (const MAILSTREAM *theStream, unsigned long theMsgNum);
    void process_mm_expunged  (const MAILSTREAM *theStream, unsigned long theMsgNum);
    void process_mm_list      (const MAILSTREAM *theStream, char theDelim, const std::string &theMailBoxString, long theAttrib);
    void process_mm_lsub      (const MAILSTREAM *theStream, char theDelim, const std::string &theMailBoxString, long theAttrib);
    void process_mm_notify    (const MAILSTREAM *theStream, const std::string &theMessage, long theErrorFlag);

    void process_mm_log       (const std::string &theMessage, long theErrorFlag);
    void process_mm_dlog      (const std::string &theMessage);
    void process_mm_fatal     (const std::string &theMessage);

    void process_mm_login     (NETMBX *theMailBox, char *theUser, char *thePassword, long theTrials);

    void process_mm_critical  (const MAILSTREAM *theStream);
    void process_mm_nocritical(const MAILSTREAM *theStream);

    long process_mm_diskerror (const MAILSTREAM *theStream, unsigned long theOSErrorCode, unsigned long theIsSerious);

  private:
    //-------------------------------------------------------------------------
    //     setActiveProcessor()
    //-------------------------------------------------------------------------
    void setActiveProcessor(CallBackProcessor *theCBP, bool fActive);

    //-------------------------------------------------------------------------
    //     getProcessor()
    //-------------------------------------------------------------------------
    CallBackProcessor *getProcessor(const MAILSTREAM *theStream);

    //-------------------------------------------------------------------------
    //     showActiveProcessor()
    //-------------------------------------------------------------------------
    void showActiveProcessor(CallBackProcessor *theProcessor, const std::string &thePrefix) const;

    //-------------------------------------------------------------------------
    //     attributes
    //-------------------------------------------------------------------------
    CallBackProcessor *myActiveProcessor;

}; // class CallBackDispatcher

//=============================================================================
//     namespaces
//=============================================================================
} // Mail
} // Ax

#endif

