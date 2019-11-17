#ifndef __AxMenuConfigMailBox_H__
#define __AxMenuConfigMailBox_H__
//=============================================================================
/*
 * See the file README in the main directory for a description of
 * this software, copyright information, and how to reach the author.
 *
 * Author: alex
 * Date:   17.11.2003
 *
 * Last modfied:
 *   $Author: alex $
 *   $Date: 2013-11-02 09:26:18 +0100 (Sa, 02 Nov 2013) $
 */
//=============================================================================

//=============================================================================
//     includes
//=============================================================================

//----- qt --------------------------------------------------------------------
//----- CORBA -----------------------------------------------------------------
//----- C++ -------------------------------------------------------------------
//----- C ---------------------------------------------------------------------
//----- vdr -------------------------------------------------------------------
#include <vdr/osd.h>

//----- AxLib -----------------------------------------------------------------
#include <Ax/Mail/MailBoxSetup.h>
#include <Ax/Vdr/OsdMenu.h>

//----- local -----------------------------------------------------------------

//----- other includes --------------------------------------------------------

//=============================================================================
//     forward declarations
//=============================================================================
class AxPluginMailBox;
class AxPluginSetup;

//=============================================================================
//     class AxMenuConfigMailBox
//=============================================================================
/** Menu to setup one mail account
 */
class AxMenuConfigMailBox : public Ax::Vdr::OsdMenu
{
    //-------------------------------------------------------------------------
    //     typedefs
    //-------------------------------------------------------------------------
    typedef Ax::Vdr::OsdMenu    PARENT;
    typedef AxMenuConfigMailBox MYSELF;

  protected:
    //-------------------------------------------------------------------------
    //     enums
    //-------------------------------------------------------------------------
    enum AxEAction
    {
      A_FIRST           = PARENT::A_LAST
    , A_SAVE_AND_CLOSE  = A_FIRST         ///< save the current settings and close
    , A_CHECK_ACCOUNT                     ///< check if the current config is accessible
    , A_SELECT_FOLDER                     ///< open the menu to select a folder
    , A_SHOW_LOG                          ///< show the log
    , A_LAST
    };

  public:
    //-------------------------------------------------------------------------
    //     AxMenuConfigMailBox()
    //-------------------------------------------------------------------------
    /** Constructor
     */
    AxMenuConfigMailBox(AxPluginMailBox *thePlugin, Ax::Mail::MailBoxSetup &theMailBoxSetup);

    //-------------------------------------------------------------------------
    //     ~AxMenuConfigMailBox()
    //-------------------------------------------------------------------------
    /** Destructor
     */
    virtual ~AxMenuConfigMailBox();

    //-------------------------------------------------------------------------
    //     MenuKind()
    //-------------------------------------------------------------------------
    virtual const char* MenuKind() { return "MenuMailboxConfig"; }

    //-------------------------------------------------------------------------
    //     ProcessKey()
    //-------------------------------------------------------------------------
    /** Process key stroke: create appropriate osditems if
     *  if relevant settings were changed
     */
    virtual eOSState ProcessKey(eKeys Key);

    //-------------------------------------------------------------------------
    //     processAction()
    //-------------------------------------------------------------------------
    /** Process the given action
     */
    virtual eOSState processAction(const Ax::Vdr::Action &theAction, eOSState = osUnknown);

  protected:
    //-------------------------------------------------------------------------
    //     informSubOSDClosed()
    //-------------------------------------------------------------------------
    virtual void informSubOSDClosed(const Ax::Vdr::Action &theAction, eKeys theKey);

    //-------------------------------------------------------------------------
    //     informCurrentChanged()
    //-------------------------------------------------------------------------
    virtual void informCurrentChanged(int theOldCurrent);

  private:
    //-------------------------------------------------------------------------
    //     checkAccount()
    //-------------------------------------------------------------------------
    /** check if the current settings represent a selectable mail account
     *
     *  @returns true if the current settings represent a usable mailbox
     */
    bool checkAccount();

    //-------------------------------------------------------------------------
    //     showLog()
    //-------------------------------------------------------------------------
    /** Show logs of c-client library
     */
    eOSState showLog();

    //-------------------------------------------------------------------------
    //     selectFolder()
    //-------------------------------------------------------------------------
    /** Open a sub-dialog to select a folder
     */
    eOSState selectFolder();

    //-------------------------------------------------------------------------
    //     setupItems()
    //-------------------------------------------------------------------------
    /** Create configuration items when osd is created and if some
     *  configuration options are changed which affect the number/type of
     *  possible options.
     */
    void setupItems();

    //-------------------------------------------------------------------------
    //     showMailBoxStr()
    //-------------------------------------------------------------------------
    /** Show the mailbox-string according to the current settings
     */
    void showMailBoxStr();

    //-------------------------------------------------------------------------
    //     convSetup2Dlg()
    //-------------------------------------------------------------------------
    /** convert the given MailBoxSetup to the attributes of this class
     */
    void convSetup2Dlg(const Ax::Mail::MailBoxSetup  &theSetup);

    //-------------------------------------------------------------------------
    //     convDlg2Setup()
    //-------------------------------------------------------------------------
    /** convert the settings of this class to the given MailBoxSetup
     */
    void convDlg2Setup(Ax::Mail::MailBoxSetup  &theSetup);

    //-------------------------------------------------------------------------
    //     attributes
    //-------------------------------------------------------------------------
    AxPluginMailBox        *myPlugin;
    Ax::Mail::MailBoxSetup &myMailBoxSetup;

    /** used during check for accessiblity and during folder selection
     */
    Ax::Mail::MailBox      *myTmpMailBox;

    /** only used for folder selection
     */
    Ax::Mail::MailFolder   *mySelectedFolder;

    //-------------------------------------------------------------------------
    //     editable attributes
    //-------------------------------------------------------------------------
    char myAccountName  [128];
    char myMailBoxString[128];
    char myHostName     [ 64];
    char myUserName     [ 64];
    char myPassword     [ 64];
    char myAccessCode   [ 10];
    char myFolder       [128];

    int myAccountType    ;
    int myAccessMode     ;

    int myBGCheck        ; // BG_CHK_ON = 0x08    ///< do do background check
    int myBGCheck_ExtApp ; // BG_EXT_ON = 0x01    ///< do start external app
    int myBGCheck_OsdMsg ; // BG_MSG_ON = 0x02    ///< do display a message
    int myBGCheck_Menu   ; // BG_MEN_ON = 0x04    ///< do display in main menu
    int myBGCheck_SIF    ; // BG_SIF_ON = 0x10    ///< do affect service-interface

    int myFAutoMarkSeen  ;
    int myFExpungeOnClose;
    int myClientOptSec   ;
    int myClientOptRSH   ;
    int myClientOptSSL   ;
    int myClientOptTLS   ;
    int myClientOptCert  ;
    int myClientOptDbg   ;

    int myIndexFirstCClient;
    int myIndexFolder      ;

    //-------------------------------------------------------------------------
    //     possible characters in edit items
    //-------------------------------------------------------------------------
    static const char *MailBoxNameChars;
    static const char *HostNameChars;
    static const char *UserNameChars;
    static const char *PaswordChars;
    static const char *MailBoxChars;
    static const char *AccessCodeChars;

}; // class AxMenuConfigMailBox

#endif
