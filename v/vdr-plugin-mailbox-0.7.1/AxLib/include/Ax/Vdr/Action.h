#ifndef __Ax_Vdr_Action_H__
#define __Ax_Vdr_Action_H__
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
 *   $Date: 2013-03-20 19:53:42 +0100 (Mi, 20 Mär 2013) $
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
//----- Vdr -------------------------------------------------------------------
#include <vdr/osdbase.h>

//----- AxLib -----------------------------------------------------------------
//----- local -----------------------------------------------------------------
//----- other includes --------------------------------------------------------

//=============================================================================
//     namespaces
//=============================================================================
namespace Ax  {
namespace Vdr {

//=============================================================================
//     class Action
//=============================================================================
/** (main description still missing)
 *
 *  An Action has following attributes/states which affect wether an action
 *  can be executed or not:
 *
 *  - Supported: is true, if an action is possible at all in the
 *               current configuration of the application.
 *
 *  - Enabled  : is true, if the action is executable in the current situation
 *               of an application.
 *
 *  - Locked   : the a lock-code is set, the action can't be run, without
 *               unlocking it before. After the Action is run() once,
 *               and after the execution it is locked again until it is
 *               unlocked again.
 *
 *  An Action which isn't supported in the current configuration will never
 *  return true when asked if enabled - even if setEnabled(true) was called
 *  before.
 *  This construct allows the programmer to
 *  - create all action-instances which are implemented in the application
 *    at the start of the application.
 *  - decide at the beginning which actions are supported in the current
 *    configuration
 *  - enable/disable actions depending on the current state of the application
 *    regardless, if some of these aren't supported at all
 *
 */
class Action
{
  public:
    //-------------------------------------------------------------------------
    //     Action()
    //-------------------------------------------------------------------------
    /** Constructor
     */
    Action( int                theID
          , const std::string &theName
          , const std::string &theDesc
          );

    //-------------------------------------------------------------------------
    //     ~Action()
    //-------------------------------------------------------------------------
    /** Destructor
     */
    virtual ~Action();

    //-------------------------------------------------------------------------
    //     getID()
    //-------------------------------------------------------------------------
    int getID() const  { return myID; }

    //-------------------------------------------------------------------------
    //    getName()
    //-------------------------------------------------------------------------
    std::string getName() const { return myName; }

    //-------------------------------------------------------------------------
    //    getDesc()
    //-------------------------------------------------------------------------
    std::string getDesc() const { return myDesc; }

    //-------------------------------------------------------------------------
    //    setDesc()
    //-------------------------------------------------------------------------
    void setDesc(const std::string &theDesc) { myDesc = theDesc; }

    //-------------------------------------------------------------------------
    //     precheck()
    //-------------------------------------------------------------------------
    /** This method is called, before the action is executed.
     *
     *  If this method returns false, the action will not be executed.
     *
     *  prechcheck() is called before the unlocking is done.
     *
     *  This method should not be overwritten and therefore isn't virtual.
     *  To implement your own action overwrite precheckInternal() instead.
     */
    bool precheck(eOSState theStateBefore);

    //-------------------------------------------------------------------------
    //     run()
    //-------------------------------------------------------------------------
    /** execute the action after checking whether this is enabled and not locked
      *
      *  This method should not be overwritten and therefore isn't virtual.
      *  To implement your own action overwrite runInternal() instead.
      */
    eOSState run(eOSState theStateBefore);

    //-------------------------------------------------------------------------
    //     isSupported()
    //-------------------------------------------------------------------------
    /** see documentation of this class
     */
    bool isSupported() const;

    //-------------------------------------------------------------------------
    //     setSupported()
    //-------------------------------------------------------------------------
    /** see documentation of this class
     */
    void setSupported(bool fSupported);

    //-------------------------------------------------------------------------
    //     isEnabled()
    //-------------------------------------------------------------------------
    /** see documentation of this class
     */
    bool isEnabled() const;

    //-------------------------------------------------------------------------
    //     setEnabled()
    //-------------------------------------------------------------------------
    /** see documentation of this class
     */
    void setEnabled(bool fEnabled);

    //-------------------------------------------------------------------------
    //     isLocked()
    //-------------------------------------------------------------------------
    /** see documentation of this class
     */
    bool isLocked() const;

    //-------------------------------------------------------------------------
    //     setLocked()
    //-------------------------------------------------------------------------
    /** see documentation of this class
     */
    void setLocked(bool fLocked);

    //-------------------------------------------------------------------------
    //     setLockCode()
    //-------------------------------------------------------------------------
    /** see documentation of this class
     */
    void setLockCode(const std::string &theLockCode);

    //-------------------------------------------------------------------------
    //     getLockCode()
    //-------------------------------------------------------------------------
    /** see documentation of this class
     */
    std::string getLockCode() const;

    //-------------------------------------------------------------------------
    //     getButtonText()
    //-------------------------------------------------------------------------
    /** Returns the text for a color button for this action if enabled
     *  otherwise 0
     */
    const char *getButtonText() const;

  protected:
    //-------------------------------------------------------------------------
    //    precheckInternal()
    //-------------------------------------------------------------------------
    /** Overwrite this method to execute a check before the action is really
     *  executed.
     *
     *  \returns true, if the action should be executed
     */
    virtual bool precheckInternal(eOSState theStateBefore);

    //-------------------------------------------------------------------------
    //    runInternal()
    //-------------------------------------------------------------------------
    /** Overwrite this method to execute the real function of this action.
     */
    virtual eOSState runInternal(eOSState theStateBefore) = 0;

    //-------------------------------------------------------------------------
    //     getButtonText()
    //-------------------------------------------------------------------------
    /** Returns the text for a color button for this action
     *
     *  Attention: This pointer is passed to cOsdMenu::SetHelp() which
     *  stores the pointer. So the return value must point to a static
     *  character array.
     *
     *  The implementation in this base class returns a NULL pointer.
     */
    virtual const char *getButtonTextInternal() const;

  private:
    //-------------------------------------------------------------------------
    //    attributes
    //-------------------------------------------------------------------------
    int         myID         ;
    std::string myName       ;
    std::string myDesc       ;
    std::string myLockCode   ;
    bool        myIsLocked   ;
    bool        myIsEnabled  ;
    bool        myIsSupported;

}; // class Action

//=============================================================================
//     class ActionReturnConstState
//=============================================================================
/** Always retuns the constant eOSState given in the constructur
 */
class ActionReturnConstState : public Action
{
    //-------------------------------------------------------------------------
    //     typedefs
    //-------------------------------------------------------------------------
    typedef Action PARENT;

  public:
    //-------------------------------------------------------------------------
    //     ActionReturnConstState()
    //-------------------------------------------------------------------------
    ActionReturnConstState( eOSState           theState
                          , int                theID
                          , const char        *theButtonText
                          , const std::string &theName
                          , const std::string &theDesc
                          );

    //-------------------------------------------------------------------------
    //     ~ActionReturnConstState()
    //-------------------------------------------------------------------------
    virtual ~ActionReturnConstState();

  protected:
    //-------------------------------------------------------------------------
    //     getButtonTextInternal()
    //-------------------------------------------------------------------------
    virtual const char *getButtonTextInternal() const;

    //-------------------------------------------------------------------------
    //     runInternal()
    //-------------------------------------------------------------------------
    virtual eOSState runInternal(eOSState theStateBefore);

  private:
    //-------------------------------------------------------------------------
    //     attributes
    //-------------------------------------------------------------------------
    const char *myButtonText;
    eOSState    myState;

}; // class ActionReturnConstState

//=============================================================================
//     class ActionCallback
//=============================================================================
/** Simply calls the method <tt> processAction(const Ax::Vdr::Action &eOSState);</tt>
 *  with this as action.
 *
 *  This class can be used, if the action isn't very complex and therefore
 *  it would be an overkill to implement a separate class derived from Action.
 *
 */
template <class AXOSDMENU>
class ActionCallback : public Action
{
    //-------------------------------------------------------------------------
    //     typedefs
    //-------------------------------------------------------------------------
    typedef Action PARENT;

  public:
    //-------------------------------------------------------------------------
    //     ActionCallback()
    //-------------------------------------------------------------------------
    /** Constructor
    */
    ActionCallback( AXOSDMENU         *theOsdMenu
                  , int                theID
                  , const char        *theButtonText
                  , const std::string &theName
                  , const std::string &theDesc = ""
                  )
      : PARENT         (theID, theName, theDesc)
      , myOsdMenu      (theOsdMenu      )
      , myButtonText   (theButtonText   )
    {
    } // ActionCallback()

    //-------------------------------------------------------------------------
    //     ~ActionCallback()
    //-------------------------------------------------------------------------
    /** Destructor
    */
    virtual ~ActionCallback()
    {
    } // ~ActionCallback()

  protected:
    //-------------------------------------------------------------------------
    //    runInternal()
    //-------------------------------------------------------------------------
    virtual eOSState runInternal(eOSState theStateBefore)
    {
      return myOsdMenu->processAction(*this, theStateBefore);
    } // runInternal()

    //-------------------------------------------------------------------------
    //     getButtonTextInternal()
    //-------------------------------------------------------------------------
    virtual const char *getButtonTextInternal() const
    {
      return myButtonText;
    } // getButtonTextInternal()

  protected:
    //-------------------------------------------------------------------------
    //    attributes
    //-------------------------------------------------------------------------
    AXOSDMENU   *myOsdMenu;
    const char  *myButtonText;

}; // class ActionCallback

//=============================================================================
//     namespaces
//=============================================================================
} // Vdr
} // Ax

#endif
