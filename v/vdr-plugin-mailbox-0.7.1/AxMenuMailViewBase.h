#ifndef __AxMenuMailViewBase_H__
#define __AxMenuMailViewBase_H__
//=============================================================================
/*
 * See the file README in the main directory for a description of
 * this software, copyright information, and how to reach the author.
 *
 * Author: alex
 * Date:   19.06.2004
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
#include <string>

//----- C ---------------------------------------------------------------------
//----- vdr -------------------------------------------------------------------
#include <vdr/osd.h>
#include <vdr/menuitems.h>

//----- AxLib -----------------------------------------------------------------
#include <Ax/Mail/Globals.h>
#include <Ax/Vdr/OsdMenu.h>

//----- local -----------------------------------------------------------------

//=============================================================================
//     forward declarations
//=============================================================================
class AxMenuMailListView;
class AxPluginMailBox;
class AxOsdItem;

//=============================================================================
//     class AxMenuMailViewBase
//=============================================================================
/** Menu to display one mail.
 */
class AxMenuMailViewBase : public Ax::Vdr::OsdMenu
{
    //-------------------------------------------------------------------------
    //     typedefs
    //-------------------------------------------------------------------------
    typedef Ax::Vdr::OsdMenu   PARENT;
    typedef AxMenuMailViewBase MYSELF;

  protected:
    //=============================================================================
    //     class AxActionToggleFlag
    //=============================================================================
    class AxActionToggleFlag : public Ax::Vdr::Action
    {
        //-------------------------------------------------------------------------
        typedef Ax::Vdr::Action PARENT;

      public:
        //-------------------------------------------------------------------------
        AxActionToggleFlag( AxMenuMailViewBase *theMailView
                          , int                 theID
                          , Ax::Mail::MailFlag  theFlag
                          , const char         *theButtonTextOff
                          , const char         *theButtonTextOn
                          , const std::string  &theName
                          , const std::string  &theDesc
                          );

        //-------------------------------------------------------------------------
        virtual ~AxActionToggleFlag();

      protected:
        //------------------------------------------------------------------------
        virtual const char *getButtonTextInternal() const;

        //-------------------------------------------------------------------------
        virtual eOSState runInternal(eOSState theStateBefore);

      protected:
        //------------------------------------------------------------------------
        AxMenuMailViewBase *myMailView;
        Ax::Mail::MailFlag  myFlag;
        const char         *myButtonTextOn ;
        const char         *myButtonTextOff;

    }; // class AxActionToggleFlagged

    //-------------------------------------------------------------------------
    //     enums
    //-------------------------------------------------------------------------
    enum AxEAction
    {
      A_FIRST            = PARENT::A_LAST
    , A_UPDATE_ACTIONS   = A_FIRST
    , A_JUMP_2_PREV
    , A_JUMP_2_NEXT
    , A_JUMP_2_PREV_NEW
    , A_JUMP_2_NEXT_NEW
    , A_TOGGLE_FLAGGED
    , A_TOGGLE_SEEN
    , A_TOGGLE_DELETED
    , A_LAST
    };

  public:

    //-------------------------------------------------------------------------
    //     AxMenuMailViewBase()
    //-------------------------------------------------------------------------
    /** Constructor
     */
    AxMenuMailViewBase( AxPluginMailBox      *thePlugin
                      , Ax::Mail::MailFolder *theFolder
                      , Ax::Mail::Mail       *theMail
                      , AxMenuMailViewBase   *theParentMailView
                      , std::string           theInstName
                      , const char           *theTitle
                      , int c0 = 0, int c1 = 0, int c2 = 0, int c3 = 0, int c4 = 0
                      );

    //-------------------------------------------------------------------------
    //     ~AxMenuMailViewBase()
    //-------------------------------------------------------------------------
    /** Destructor
     */
    virtual ~AxMenuMailViewBase();

    //-------------------------------------------------------------------------
    //     MenuKind()
    //-------------------------------------------------------------------------
    virtual const char* MenuKind() { return "MenuMailboxView"; }

    //-------------------------------------------------------------------------
    //     processAction()
    //-------------------------------------------------------------------------
    virtual eOSState processAction(const Ax::Vdr::Action &theAction, eOSState = osUnknown);

    //-------------------------------------------------------------------------
    //     setCurrentMail()
    //-------------------------------------------------------------------------
    virtual void setCurrentMail(Ax::Mail::Mail *theMail);

    //-------------------------------------------------------------------------
    //     getCurrentMail()
    //-------------------------------------------------------------------------
    Ax::Mail::Mail *getCurrentMail();

    //-------------------------------------------------------------------------
    //     getMailBox()
    //-------------------------------------------------------------------------
    Ax::Mail::MailBox *getMailBox() const;

    //-------------------------------------------------------------------------
    //     getFolder()
    //-------------------------------------------------------------------------
    Ax::Mail::MailFolder *getFolder() const;

    //-------------------------------------------------------------------------
    //     AxPluginMailBox()
    //-------------------------------------------------------------------------
    AxPluginMailBox *getPlugin() const { return myPlugin; }

    //-------------------------------------------------------------------------
    //     updateActions()
    //-------------------------------------------------------------------------
    virtual void updateActions();

  protected:
    //-------------------------------------------------------------------------
    //     updateWithCurrentMail()
    //-------------------------------------------------------------------------
    virtual void updateWithCurrentMail() = 0;

    //-------------------------------------------------------------------------
    //     informSubOSDClosed()
    //-------------------------------------------------------------------------
    virtual void informSubOSDClosed(const Ax::Vdr::Action &theAction, eKeys theKey);

  public:
    //-------------------------------------------------------------------------
    //     getFlagsStr()
    //-------------------------------------------------------------------------
    /** Returns a displayable string of the flags of the mail.
      *
      *  The following characters are used to represent the flags:
      *
      *  \code
  aFlag[0] =  getIsSeen    ()  ? '_' : 'N';
  aFlag[1] =  getIsFlagged ()  ? 'F' : '_';
  aFlag[2] =  getIsAnswered()  ? 'A' : '_';
  aFlag[3] =  getIsDeleted ()  ? 'D' : '_';
      *  \endcode
      *
      *  @returns a displayable string of the flags of the mail.
      */
    static std::string getFlagsStr(const Ax::Mail::Mail *theMail);

  private:
    //-------------------------------------------------------------------------
    //     attributes
    //-------------------------------------------------------------------------
    AxPluginMailBox      *myPlugin ; ///< pointer to the plugin
    Ax::Mail::MailFolder *myFolder ; ///< pointer to the mail-folder
    Ax::Mail::Mail       *myMail   ; ///< the mail which is displayed
    Ax::Mail::Mail       *myPrevNew; ///< cache the prev new mail, updated in showMail()
    Ax::Mail::Mail       *myNextNew; ///< cache the next new mail, updated in showMail()
    Ax::Mail::Mail       *myPrev   ; ///< cache the prev mail    , updated in showMail()
    Ax::Mail::Mail       *myNext   ; ///< cache the next mail    , updated in showMail()

    AxMenuMailViewBase   *myParent ; ///< the 'parent'-view from which this was opened

}; // class AxMenuMailViewBase

#endif

