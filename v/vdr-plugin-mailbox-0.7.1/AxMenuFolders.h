#ifndef __AxMenuFolders_H__
#define __AxMenuFolders_H__
//=============================================================================
/*
 * See the file README in the main directory for a description of
 * this software, copyright information, and how to reach the author.
 *
 * Author: Alex
 * Date:   10.12.2004
 *
 * Last modfied:
 *   $Author: alex $
 *   $Date: 2004-09-21 23:21:04 +0200 (Di, 21 Sep 2004) $
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
//----- AxLib -----------------------------------------------------------------
#include <Ax/Vdr/OsdMenu.h>

//----- local -----------------------------------------------------------------
//----- other includes --------------------------------------------------------

//=============================================================================
//     forward declarations
//=============================================================================
namespace Ax   {
namespace Mail {
  class MailFolder;
}
}

class AxFolderItem;

//=============================================================================
//     class AxMenuFolders
//=============================================================================
/** Menu to select a mail account and to call the mail-list-view.
 *
 *  If the menu is closed (with osBack) the given folder (theBaseFolder -> myFolder)
 *  either points:
 *
 *  - to the selected folder (accepted)
 *  or
 *  - to 0 (rejected)
 *
 */
class AxMenuFolders : public Ax::Vdr::OsdMenu
{
    //-------------------------------------------------------------------------
    //     typedefs
    //-------------------------------------------------------------------------
    typedef Ax::Vdr::OsdMenu  PARENT;
    typedef AxMenuFolders     MYSELF;

  protected:
    //-------------------------------------------------------------------------
    //     enums
    //-------------------------------------------------------------------------
    enum AxEAction
    {
      A_FIRST           = PARENT::A_LAST
    , A_ACCEPT_FOLDER   = A_FIRST        ///< accept the current folder and close
    , A_REJECT_FOLDER                    ///< reject the selection and close
    , A_CHECK_FOLDER                     ///< check if the current folder is accessible
    , A_OPEN_FOLDER                      ///< open the current folder
    , A_CLOSE_FOLDER                     ///< close the current folder
    , A_SHOW_LOG                         ///< show the log
    , A_LAST
    };

  public:
    //-------------------------------------------------------------------------
    //     AxMenuFolders()
    //-------------------------------------------------------------------------
    /** Constructor
     *
     *  Show all folders of the mailbox assigned to theBaseFolder and
     *  preselect theBaseFolder
     *
     */
    AxMenuFolders(Ax::Mail::MailFolder *&theBaseFolder);

    //-------------------------------------------------------------------------
    //     ~AxMenuFolders()
    //-------------------------------------------------------------------------
    /** Destructor
     */
    virtual ~AxMenuFolders();

    //-------------------------------------------------------------------------
    //     MenuKind()
    //-------------------------------------------------------------------------
    virtual const char* MenuKind() { return "MenuMailboxFolders"; }

    //-------------------------------------------------------------------------
    //     processAction()
    //-------------------------------------------------------------------------
    /** Process the action with the given ActionCode
     */
    virtual eOSState processAction(const Ax::Vdr::Action &theAction, eOSState = osUnknown);

    //-------------------------------------------------------------------------
    //     delItem()
    //-------------------------------------------------------------------------
    bool delItem(cOsdItem *theItem);

    //-------------------------------------------------------------------------
    //     getFolderItem()
    //-------------------------------------------------------------------------
    AxFolderItem *getFolderItem(std::string theFolderPath);

  protected:
    //-------------------------------------------------------------------------
    //     informSubOSDClosed()
    //-------------------------------------------------------------------------
    /** This methods is called from within ProcessKey() whenever a sub-dialog
     *  is closed
     */
    virtual void informSubOSDClosed(const Ax::Vdr::Action &theAction, eKeys theKey);

    //-------------------------------------------------------------------------
    //     informCurrentChanged()
    //-------------------------------------------------------------------------
    virtual void informCurrentChanged(int theOldCurrent);

  private:
    //-------------------------------------------------------------------------
    //     getCurrentFolder()
    //-------------------------------------------------------------------------
    /** returns the currently selected folder or 0 if no folder selected.
     */
    Ax::Mail::MailFolder *getCurrentFolder();

    //-------------------------------------------------------------------------
    //     updateActions()
    //-------------------------------------------------------------------------
    /** Update the enabled state of the actions depending on the current folder
     */
    void updateActions();

    //-------------------------------------------------------------------------
    //     setup()
    //-------------------------------------------------------------------------
    /** Clear all folder items and create items for all folders below
     *  (and not including) the root-folder of the mailbox
     */
    bool setup();

    //-------------------------------------------------------------------------
    //     checkCurrentFolder()
    //-------------------------------------------------------------------------
    /** Check if the current folder is accessible
     *
     *  If the current folder isn't accessible an error-message is displayed
     *
     *  @param   fShowStatus show success-status if accessible
     *  @returns true if the current folder is accessible
     */
    bool checkCurrentFolder(bool fShowStatus);

    //-------------------------------------------------------------------------
    //     attributes
    //-------------------------------------------------------------------------
    Ax::Mail::MailBox     *myMailBox;
    Ax::Mail::MailFolder *&myFolder;

}; // class AxMenuFolders

#endif

