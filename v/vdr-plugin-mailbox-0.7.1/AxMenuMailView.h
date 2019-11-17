#ifndef __AxMenuMailView_H__
#define __AxMenuMailView_H__
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
 *   $Date: 2013-03-20 20:33:18 +0100 (Mi, 20 Mär 2013) $
 */
//=============================================================================

//=============================================================================
//     includes
//=============================================================================

//----- qt --------------------------------------------------------------------
//----- CORBA -----------------------------------------------------------------
//----- C++ -------------------------------------------------------------------
#include <string>

//----- C ---------------------------------------------------------------------
//----- vdr -------------------------------------------------------------------
#include <vdr/osd.h>
#include <vdr/menuitems.h>

//----- AxLib -----------------------------------------------------------------
#include <Ax/Mail/Globals.h>

//----- local -----------------------------------------------------------------
#include "AxMenuMailViewBase.h"
#include "AxMailWrapper.h"

//=============================================================================
//     forward declarations
//=============================================================================
class AxMenuMailListView;
class AxPluginMailBox;
class AxOsdItem;

//=============================================================================
//     class AxMenuMailView
//=============================================================================
/** Menu to display one mail.
 */
class AxMenuMailView : public AxMenuMailViewBase
{
    //-------------------------------------------------------------------------
    //     typedefs
    //-------------------------------------------------------------------------
    typedef AxMenuMailViewBase PARENT;
    typedef AxMenuMailView     MYSELF;

  protected:
    //-------------------------------------------------------------------------
    //     enums
    //-------------------------------------------------------------------------
    enum AxEAction
    {
      A_FIRST          = PARENT::A_LAST
    , A_SHOW_PAGE_JUMP = A_FIRST
    , A_SHOW_PAGE_MARK
    , A_SWITCH_WRAP
    , A_SHOW_CALIB
    , A_SHOW_ALL_PARTS
    , A_LAST
    };

    //-------------------------------------------------------------------------
    //     enum AxPage
    //-------------------------------------------------------------------------
    enum AxPage
    {
      PAGE_JUMP
    , PAGE_MARK
    }; // enum AxPage

  public:
    //-------------------------------------------------------------------------
    //     AxMenuMailView()
    //-------------------------------------------------------------------------
    /** Constructor
     */
    AxMenuMailView( AxPluginMailBox      *thePlugin
                  , Ax::Mail::MailFolder *theFolder
                  , Ax::Mail::Mail       *theMail
                  , AxMenuMailViewBase   *theMailViewParent
                  );

    //-------------------------------------------------------------------------
    //     ~AxMenuMailView()
    //-------------------------------------------------------------------------
    /** Destructor
     */
    virtual ~AxMenuMailView();

    //-------------------------------------------------------------------------
    //     processAction()
    //-------------------------------------------------------------------------
    virtual eOSState processAction(const Ax::Vdr::Action &theAction, eOSState = osUnknown);

    //-------------------------------------------------------------------------
    //     ProcessKey()
    //-------------------------------------------------------------------------
    /** Process key press events
     */
    virtual eOSState ProcessKey(eKeys Key);

    //-------------------------------------------------------------------------
    //     setCurrentMail()
    //-------------------------------------------------------------------------
    virtual void setCurrentMail(Ax::Mail::Mail *theMail);

    //-------------------------------------------------------------------------
    //     Display()
    //-------------------------------------------------------------------------
    virtual void Display(void);

    //-------------------------------------------------------------------------
    //     updateActions()
    //-------------------------------------------------------------------------
    virtual void updateActions();

  protected:
    //-------------------------------------------------------------------------
    //     updateWithCurrentMail()
    //-------------------------------------------------------------------------
    /** update the menuitems to display the current mail
     */
    void updateWithCurrentMail();

    //-------------------------------------------------------------------------
    //     informSubOSDClosed()
    //-------------------------------------------------------------------------
    virtual void informSubOSDClosed(const Ax::Vdr::Action &theAction, eKeys theKey);

  private:
    //-------------------------------------------------------------------------
    //     attributes
    //-------------------------------------------------------------------------
    AxMailWrapper          *myMailWrapper;
    std::string             myBodyText;
    AxMailWrapper::WrapMode myWrapMode;
    bool                    myUseFixedFont;
    bool                    myShowAllParts;

}; // class AxMenuMailView

#endif

