#ifndef __AxMenuConfigCommon_H__
#define __AxMenuConfigCommon_H__
//=============================================================================
/*
 * See the file README in the main directory for a description of
 * this software, copyright information, and how to reach the author.
 *
 * Author: alex
 * Date:   18.12.2003
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
#include <Ax/Vdr/OsdMenu.h>

//----- local -----------------------------------------------------------------
#include "AxPluginSetup.h"

//----- other includes --------------------------------------------------------

//=============================================================================
//     forward declarations
//=============================================================================
class AxPluginMailBox;

//=============================================================================
//     class AxMenuConfigCommon
//=============================================================================
/** Setup menu for common settings of the plugin
 */
class AxMenuConfigCommon : public Ax::Vdr::OsdMenu
{
    typedef Ax::Vdr::OsdMenu   PARENT;
    typedef AxMenuConfigCommon MYSELF;

  protected:
    //-------------------------------------------------------------------------
    //     enums
    //-------------------------------------------------------------------------
    enum AxEAction
    {
      A_FIRST            = PARENT::A_LAST
    , A_SAVE_AND_CLOSE   = A_FIRST
    , A_LAST
    };

  public:
    //-------------------------------------------------------------------------
    //     AxMenuConfigCommon()
    //-------------------------------------------------------------------------
    /** Constructor
     */
    AxMenuConfigCommon(AxPluginMailBox *thePlugin);

    //-------------------------------------------------------------------------
    //     ~AxMenuConfigCommon()
    //-------------------------------------------------------------------------
    /** Destructor
     */
    virtual ~AxMenuConfigCommon();

    //-------------------------------------------------------------------------
    //     MenuKind()
    //-------------------------------------------------------------------------
    virtual const char* MenuKind() { return "MenuMailboxConfig"; }

    //-------------------------------------------------------------------------
    //     processAction()
    //-------------------------------------------------------------------------
    /** Process the action with the given ActionCode
     */
    virtual eOSState processAction(const Ax::Vdr::Action &theAction, eOSState = osUnknown);

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
    //     attributes
    //-------------------------------------------------------------------------
    AxPluginMailBox  *myPlugin;
    AxPluginSettings  myTmpSettings;

    // indices of MenuItems
    int               myIndexViewMode;

}; // class AxMenuConfigCommon

#endif

