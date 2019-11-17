#ifndef __Ax_Mail_MailBoxSetup_H__
#define __Ax_Mail_MailBoxSetup_H__
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
 *   $Revision: 1.11 $
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
#include <Ax/Mail/Globals.h>

//----- local -----------------------------------------------------------------
//----- vdr -------------------------------------------------------------------
//----- c-client --------------------------------------------------------------
//----- other includes --------------------------------------------------------

//=============================================================================
//     namespaces
//=============================================================================
namespace Ax   {
namespace Mail {

//=========================================================================
//     class MailBoxSetup
//=========================================================================
/** Small class to hold the settings for one mailbox.
*/
class MailBoxSetup
{
  public:
    //-------------------------------------------------------------------------
    //     MailBoxSetup()
    //-------------------------------------------------------------------------
    /** Constructor
     */
    MailBoxSetup(std::string theAccountName = "");

    //-------------------------------------------------------------------------
    //     ~MailBoxSetup()
    //-------------------------------------------------------------------------
    /** Destructor
     */
    ~MailBoxSetup();

    //-------------------------------------------------------------------------
    //     init()
    //-------------------------------------------------------------------------
    void init();

    //-------------------------------------------------------------------------
    //     isValid()
    //-------------------------------------------------------------------------
    bool isValid() const;

    //-------------------------------------------------------------------------
    //     dump()
    //-------------------------------------------------------------------------
    void dump() const;

    //-------------------------------------------------------------------------
    //     setMailBoxString()
    //-------------------------------------------------------------------------
    /**
     * @returns true if the string was correctly parsed
     */
    bool setMailBoxString(const std::string &theMailBoxString);

    //-------------------------------------------------------------------------
    //     getMailBoxString()
    //-------------------------------------------------------------------------
    std::string getMailBoxString() const;

    //-------------------------------------------------------------------------
    //     getAccountString()
    //-------------------------------------------------------------------------
    /** returns the remote account specification string (the part
     *  between (and including) '{' ... '}'
     *
     *  (currently) supported only for imap accounts
     */
    const std::string &getAccountString() const;

    //-------------------------------------------------------------------------
    //     some getter and setter methods
    //-------------------------------------------------------------------------
    const std::string &getAccountName()  const { return myAccountName ; }
    const std::string &getFolderPath()   const { return myFolderPath  ; }
    const std::string &getUserName()     const { return myUserName    ; }
    const std::string &getPassword()     const { return myPassword    ; }
    const std::string &getAccessCode()   const { return myAccessCode  ; }
    const std::string &getHostName()     const { return myHostName    ; }

    void setAccountName (const std::string &theAccountName );
    void setFolderPath  (const std::string &theFolderPath  );
    void setAccessCode  (const std::string &theAccessCode  );
    void setUserName    (const std::string &theUserName    );
    void setPassword    (const std::string &thePassword    );
    void setHostName    (const std::string &theHostName    );

    //-------------------------------------------------------------------------
    //     get a c-client-option
    //-------------------------------------------------------------------------
    int getAccountType  ()   const { return myAccountType  ; }
    int getAccessMode   ()   const { return myAccessMode   ; }
    int getBGCheckMode  ()   const { return myBGCheckMode  ; }
    int getClientOptSec ()   const { return myClientOptSec ; }
    int getClientOptRSH ()   const { return myClientOptRSH ; }
    int getClientOptSSL ()   const { return myClientOptSSL ; }
    int getClientOptTLS ()   const { return myClientOptTLS ; }
    int getClientOptCert()   const { return myClientOptCert; }
    int getClientOptDbg ()   const { return myClientOptDbg ; }

    //-------------------------------------------------------------------------
    //     set a c-client-option
    //-------------------------------------------------------------------------
    void setAccountType   (int thAccountType   );
    void setAccessMode    (int theAccessMode   );
    void setBGCheckMode   (int theBGCheckMode  );

    void setClientOptSec  (int theClientOptSec );
    void setClientOptRSH  (int theClientOptRSH );
    void setClientOptSSL  (int theClientOptSSL );
    void setClientOptTLS  (int theClientOptTLS );
    void setClientOptCert (int theClientOptCert);
    void setClientOptDbg  (int theClientOptDbg );

    //-------------------------------------------------------------------------
    //     client-application specific flags
    //-------------------------------------------------------------------------
    bool getAutoMarkSeen  () const     { return myFAutoMarkSeen   != 0; }
    bool getExpungeOnClose() const     { return myFExpungeOnClose != 0; }

    void setAutoMarkSeen  (bool fSet)  { myFAutoMarkSeen   = fSet ? 1 : 0; }
    void setExpungeOnClose(bool fSet)  { myFExpungeOnClose = fSet ? 1 : 0; }

    //-------------------------------------------------------------------------
    //     some helper methods to build and split the mailbox string
    //-------------------------------------------------------------------------

    //-------------------------------------------------------------------------
    //     supportsOption()
    //-------------------------------------------------------------------------
    /** Checks whether the given account type supports the given option
     */
    bool        supportsOption     (CCOption theOption) const { return supportsOption     (getAccountType(), theOption); }
    std::string getAccountTypeStr  ()                   const { return getAccountTypeStr  (getAccountType  ());  }
    std::string getClientOptSecStr ()                   const { return getClientOptSecStr (getClientOptSec ());  }
    std::string getClientOptRSHStr ()                   const { return getClientOptRSHStr (getClientOptRSH ()) ; }
    std::string getClientOptSSLStr ()                   const { return getClientOptSSLStr (getClientOptSSL ());  }
    std::string getClientOptTLSStr ()                   const { return getClientOptTLSStr (getClientOptTLS ());  }
    std::string getClientOptCertStr()                   const { return getClientOptCertStr(getClientOptCert());  }
    std::string getClientOptDbgStr ()                   const { return getClientOptDbgStr (getClientOptDbg ());  }

    static bool        supportsOption     (int theType, CCOption theOption);
    static std::string getAccountTypeStr  (int theOpt);
    static std::string getClientOptSecStr (int theOpt);
    static std::string getClientOptRSHStr (int theOpt);
    static std::string getClientOptSSLStr (int theOpt);
    static std::string getClientOptTLSStr (int theOpt);
    static std::string getClientOptCertStr(int theOpt);
    static std::string getClientOptDbgStr (int theOpt);

  private:
    //-------------------------------------------------------------------------
    //     updateAccountString()
    //-------------------------------------------------------------------------
    void updateAccountString() const;

    //-------------------------------------------------------------------------
    //     initFromMailBoxString()
    //-------------------------------------------------------------------------
    bool initFromMailBoxString(std::string theMailBoxString);

    //-------------------------------------------------------------------------
    //     attributes
    //-------------------------------------------------------------------------
    std::string myAccountName;
    std::string myFolderPath;

    std::string myUserName;
    std::string myPassword;
    std::string myHostName;
    std::string myAccessCode;

    /** myAccountString is lazy updated when used (and dirty) by (the const)
     *  methods getAccountString() or indirectly by getMailBoxString().
     */
    mutable bool          myIsDirty;
    mutable std::string   myAccountString;

    int myAccountType    ;
    int myAccessMode     ;
    int myBGCheckMode    ;
    int myClientOptSec   ;
    int myClientOptRSH   ;
    int myClientOptSSL   ;
    int myClientOptTLS   ;
    int myClientOptCert  ;
    int myClientOptDbg   ;
    int myFAutoMarkSeen  ;
    int myFExpungeOnClose;
}; // class MailBoxSetup

//=============================================================================
//     namespaces
//=============================================================================
} // Mail
} // Ax

#endif

