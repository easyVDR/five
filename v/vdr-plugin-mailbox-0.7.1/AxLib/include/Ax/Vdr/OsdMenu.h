#ifndef __Ax_Vdr_OsdMenu_H__
#define __Ax_Vdr_OsdMenu_H__
//=============================================================================
/*
 * See the file README in the main directory for a description of
 * this software, copyright information, and how to reach the author.
 *
 * Author: alex
 * Date:   03.06.2004
 *
 * Last modfied:
 *   $Author: alex $
 *   $Date: 2014-10-26 11:19:44 +0100 (So, 26 Okt 2014) $
 */
//=============================================================================

//=============================================================================
//     includes
//=============================================================================

//----- qt --------------------------------------------------------------------
//----- CORBA -----------------------------------------------------------------
//----- C++ -------------------------------------------------------------------
#include <map>
#include <string>

//----- C ---------------------------------------------------------------------
//----- Vdr -------------------------------------------------------------------
#include <vdr/osd.h>
#include <vdr/menuitems.h>

//----- AxLib -----------------------------------------------------------------
#include <Ax/Vdr/Action.h>

//----- local -----------------------------------------------------------------
//----- other includes --------------------------------------------------------

//=============================================================================
//     namespaces
//=============================================================================
namespace Ax  {
namespace Vdr {

//=============================================================================
//     forward declarations
//=============================================================================
class ButtonActions;

//=============================================================================
//     defines
//=============================================================================
#define Key_HelpKeys k0

//=============================================================================
//     class OsdMenuBase
//=============================================================================
/**
 *   I would prefer if cPlugin::SetupMenu() would return a
 *   'normal' cOsdMenu, then this whole class as template wouldn't
 *   be necessary.
 */
template <typename TT>
class OsdMenuBase : public TT
{
  private:
    //=========================================================================
    //     typedefs
    //=========================================================================
    typedef TT          PARENT;
    typedef OsdMenuBase MYSELF;

  public:
    //-------------------------------------------------------------------------
    typedef std::map<int, Action *       > ID2ActionMap ;
    typedef std::map<int, Action *       > Key2ActionMap;
    typedef std::map<int, ButtonActions *> ButtonPage2Actions;

  protected:
    //-------------------------------------------------------------------------
    //     enums
    //-------------------------------------------------------------------------
    enum AxEAction
    {
      A_FIRST
    , A_NONE     = A_FIRST  /// default behaviour (event may be distributed to parent menus)
    , A_IGNORE              /// ignore the event  (returns osContinue)
    , A_GO_BACK             /// go back           (returns osBack)
    , A_HELP_KEYS           /// display key-mapping on osd
    , A_LAST
    };

  public:
    //-------------------------------------------------------------------------
    //     OsdMenuBase()
    //-------------------------------------------------------------------------
    /** Constructor
    */
    OsdMenuBase( const std::string &theMenuName
               , const std::string &theTitle
               , int c0 = 0, int c1 = 0, int c2 = 0, int c3 = 0, int c4 = 0
               );

    //-------------------------------------------------------------------------
    //     OsdMenuBase()
    //-------------------------------------------------------------------------
    /** Constructor
    */
    OsdMenuBase(const std::string &theInstName);

  private:
    //-------------------------------------------------------------------------
    //     init()
    //-------------------------------------------------------------------------
    void init();

  public:
    //-------------------------------------------------------------------------
    //     ~OsdMenuBase()
    //-------------------------------------------------------------------------
    /** Destructor
    */
    virtual ~OsdMenuBase();

    //-------------------------------------------------------------------------
    //     getName()
    //-------------------------------------------------------------------------
    virtual std::string getName() const { return myMenuName; }

    //-------------------------------------------------------------------------
    //     getCurrentItem()
    //-------------------------------------------------------------------------
    virtual cOsdItem *getCurrentItem();

    //-------------------------------------------------------------------------
    //     Display()
    //-------------------------------------------------------------------------
    virtual void Display(void);

    //-------------------------------------------------------------------------
    //     ProcessKey()
    //-------------------------------------------------------------------------
    /** Process key press events
     */
    virtual eOSState ProcessKey(eKeys Key);

    //-------------------------------------------------------------------------
    //     processAction()
    //-------------------------------------------------------------------------
    /** Process the action with the given ActionCode
     */
    virtual eOSState processAction(const Ax::Vdr::Action &theAction, eOSState theState = osUnknown);

    //-------------------------------------------------------------------------
    //     debugProcessKey()
    //-------------------------------------------------------------------------
    /** This methods is called from within ProcessKey() whenever the call
     *  to PARENT::ProcessKey() changes the current menuitem
     */
    virtual void debugProcessKey(char c, eKeys theKey, eOSState theState, const std::string &theMsg = "");

    //-------------------------------------------------------------------------
    //     addAction()
    //-------------------------------------------------------------------------
    /** add the given action the the internal collection of actions and
     *  give ownership (and responsiblity for destruction) to this
     *
     *  If there was already an action with the given id, this previous
     *  action will be deleted.
     */
    void addAction(Action *theAction);

    //-------------------------------------------------------------------------
    //     getAction()
    //-------------------------------------------------------------------------
    /** returns the action with the given id or 0 of no action with the id
     *  is present.
     */
    Action *getAction(int theID) const;

    //-------------------------------------------------------------------------
    //     getActions()
    //-------------------------------------------------------------------------
    const ID2ActionMap &getActions() const { return myActions; }

    //-------------------------------------------------------------------------
    //     setActionSupported()
    //-------------------------------------------------------------------------
    void setActionSupported(int theID, bool fSupported);

    //-------------------------------------------------------------------------
    //     isActionSupported()
    //-------------------------------------------------------------------------
    bool isActionSupported(int theID) const;

    //-------------------------------------------------------------------------
    //     setActionEnabled()
    //-------------------------------------------------------------------------
    void setActionEnabled(int theID, bool fEnabled);

    //-------------------------------------------------------------------------
    //     isActionEnabled()
    //-------------------------------------------------------------------------
    bool isActionEnabled(int theID) const;

    //-------------------------------------------------------------------------
    //     setActionLocked()
    //-------------------------------------------------------------------------
    void setActionLocked(int theID, bool fLocked);

    //-------------------------------------------------------------------------
    //     setActionLockCode()
    //-------------------------------------------------------------------------
    void setActionLockCode(int theID, const std::string &theLockCode);

    //-------------------------------------------------------------------------
    //     setUpdateColorButtons()
    //-------------------------------------------------------------------------
    void setUpdateColorButtons(bool fSet = true);

    //-------------------------------------------------------------------------
    //     setKeyAction()
    //-------------------------------------------------------------------------
    /** Assign an action to a key.
    */
    Action *setKeyAction(int theKey, int theID);

    //-------------------------------------------------------------------------
    //     selectButtonPage()
    //-------------------------------------------------------------------------
    /** Setup a page of color buttons
    */
    void selectButtonPage(int thePageNum);

    //-------------------------------------------------------------------------
    //     getButtonPage()
    //-------------------------------------------------------------------------
    /** returns the currently selected button page
    */
    int getButtonPage() const { return myButtonPage; }

    //-------------------------------------------------------------------------
    //     setButtonTexts()
    //-------------------------------------------------------------------------
    /** Setup a page of color buttons
     */
    void setButtonTexts( const std::string &theTextRed
                       , const std::string &theTextGreen
                       , const std::string &theTextYellow
                       , const std::string &theTextBlue
                       );

    //-------------------------------------------------------------------------
    //     setButtonActions()
    //-------------------------------------------------------------------------
    /** Setup a page of color buttons
     */
    void setButtonActions( int thePageNum
                         , int theActionRed          = A_NONE
                         , int theActionGreen        = A_NONE
                         , int theActionYellow       = A_NONE
                         , int theActionBlue         = A_NONE
                         , int theActionRedRep       = A_NONE
                         , int theActionGreenRep     = A_NONE
                         , int theActionYellowRep    = A_NONE
                         , int theActionBlueRep      = A_NONE
                         );

    //-------------------------------------------------------------------------
    //     getKeyAction()
    //-------------------------------------------------------------------------
    /** returns the action for the given key
     */
    Action *getKeyAction(int theKey) const;

    //-------------------------------------------------------------------------
    //     getActionKey()
    //-------------------------------------------------------------------------
    /** returns the key for the given action or kNone if no key assigned
     */
    int getActionKey(Action *theAction) const;

    //-------------------------------------------------------------------------
    //     showStatusMsg()
    //-------------------------------------------------------------------------
    /** Displays a status message for the given time and then deletes the
     *  status line.
     *
     *  This message differs from the Vdr Skins.Message-method:
     *  - Skins.Message(mtStatus, ...) doesn't delete the message after timeout
     *  - Skins.Message(mtInfo,...)    consumes the next key as confirmation
     *
     *  @param theSecs time to display the message, -1 for Vdr-settings, 0 for ever
     */
    void showStatusMsg(const std::string &theMsg = std::string(""), int theSecs = -1);

    //-------------------------------------------------------------------------
    //     clearStatusMsg()
    //-------------------------------------------------------------------------
    /** clears the status message
     */
    void clearStatusMsg();

    //-------------------------------------------------------------------------
    //     addSubMenu()
    //-------------------------------------------------------------------------
    /** use my naming conventions (methods start with a small char)
     */
    eOSState addSubMenu(cOsdMenu *theSubMenu)
    {
      return PARENT::AddSubMenu(theSubMenu);
    }

  protected:
    //-------------------------------------------------------------------------
    //     updateColorButtons()
    //-------------------------------------------------------------------------
    /** call this method, if you changed the enabled/disabled state of
     *  an action assigned to a color button or if you assigned another
     *  action to a color button with setKeyAction
     */
    void updateColorButtons();

    //-------------------------------------------------------------------------
    //     addSeparator()
    //-------------------------------------------------------------------------
    /** Adds a not selectable separator-line to the osd
     */
    void addSeparator(std::string theText);

    //-------------------------------------------------------------------------
    //     informSubOSDOpened()
    //-------------------------------------------------------------------------
    /** This methods is called from within ProcessKey() whenever a sub-osd
     * is closed
     */
    virtual void informSubOSDOpened(const Ax::Vdr::Action &theAction, eKeys theKey);

    //-------------------------------------------------------------------------
    //     informSubOSDClosed()
    //-------------------------------------------------------------------------
    /** This methods is called from within ProcessKey() whenever a sub-osd
     * is closed
     */
    virtual void informSubOSDClosed(const Ax::Vdr::Action &theAction, eKeys theKey);

    //-------------------------------------------------------------------------
    //     informCurrentChanged()
    //-------------------------------------------------------------------------
    /** This methods is called from within ProcessKey() whenever the call
     *  to PARENT::ProcessKey() changes the current menuitem
     */
    virtual void informCurrentChanged(int theOldCurrent);

  private:
    //-------------------------------------------------------------------------
    //     isButtonAction()
    //-------------------------------------------------------------------------
    bool isButtonAction(int theActionID) const;

    //-------------------------------------------------------------------------
    //     drawStatusMsg()
    //-------------------------------------------------------------------------
    void drawStatusMsg();

    //-------------------------------------------------------------------------
    //     attributes
    //-------------------------------------------------------------------------
    std::string        myMenuName;

    /** If the current action to exectue is locked, the current action
     *  is stored in this member and the osdmenu to enter the unlock
     *  code is displayed.
     */
    Action            *myPendingAction;

    /** If the execution of an Action opens a submenu, this member holds
     *  a pointe to the action which created that submenu.
     */
    Action            *mySubAction;

    ID2ActionMap       myActions;
    Key2ActionMap      myKeyActions;
    int                myButtonPage;
    ButtonPage2Actions myButtonActions;

    // cache the button texts
    // (Vdr doesn't copy the values)
    std::string        myTextRed;
    std::string        myTextGreen;
    std::string        myTextYellow;
    std::string        myTextBlue;

    bool               myUpdateColorButtons;

    //-------------------------------------------------------------------------
    //     timed status message
    //-------------------------------------------------------------------------
    std::string        myStatusMsg;
    std::string        myStatusMsgLast;
    cTimeMs            myStatusTimeout;

}; // typename OsdMenuBase

//=========================================================================
//     ButtonActions
//=========================================================================
class ButtonActions
{
  public:
    ButtonActions( int theActionRed
                 , int theActionGreen
                 , int theActionYellow
                 , int theActionBlue
                 , int theActionRedRep
                 , int theActionGreenRep
                 , int theActionYellowRep
                 , int theActionBlueRep
                 )
      : ActionRed      (theActionRed)
      , ActionGreen    (theActionGreen)
      , ActionYellow   (theActionYellow)
      , ActionBlue     (theActionBlue)
      , ActionRedRep   (theActionRedRep)
      , ActionGreenRep (theActionGreenRep)
      , ActionYellowRep(theActionYellowRep)
      , ActionBlueRep  (theActionBlueRep)
    {
    }

    int ActionRed      ;
    int ActionGreen    ;
    int ActionYellow   ;
    int ActionBlue     ;
    int ActionRedRep   ;
    int ActionGreenRep ;
    int ActionYellowRep;
    int ActionBlueRep  ;
}; // struct ButtonActions

//=============================================================================
//     class ActionActivateButtonPage
//=============================================================================
template <typename TT>
class ActionActivateButtonPage : public Action
{
    //-------------------------------------------------------------------------
    typedef Action PARENT;

  public:
    //-------------------------------------------------------------------------
    ActionActivateButtonPage( TT                *theOsdMenu
                            , int                theID
                            , const char        *theButtonText
                            , int                thePageNum
                            , const std::string &theName // = "ActivateButtonPage"
                            , const std::string &theDesc // = ""
                            );

    //-------------------------------------------------------------------------
    virtual ~ActionActivateButtonPage();

  protected:
    //-------------------------------------------------------------------------
    virtual const char *getButtonTextInternal() const;

    //-------------------------------------------------------------------------
    virtual eOSState runInternal(eOSState theStateBefore);

  private:
    //-------------------------------------------------------------------------
    TT          *myOsdMenu   ;
    const char  *myButtonText;
    int          myPageNum   ;

}; // class ActionActivateButtonPage

//=============================================================================
//     classes
//=============================================================================
typedef OsdMenuBase<cOsdMenu      > OsdMenu     ;
typedef OsdMenuBase<cMenuSetupPage> OsdSetupMenu;

//=============================================================================
//     namespaces
//=============================================================================
} // Vdr
} // Ax

//=============================================================================
//     read the implementation of the template class
//=============================================================================
#include <Ax/Vdr/OsdMenu.cpp>

#endif
