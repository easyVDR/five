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
 *   $Date: 2014-10-26 15:41:24 +0100 (So, 26 Okt 2014) $
 */
//=============================================================================

//=============================================================================
//     includes
//=============================================================================

//----- multiple defs of swap() workaround ------------------------------------
#include "AxPluginGlobals.h"

//----- qt --------------------------------------------------------------------
//----- CORBA -----------------------------------------------------------------
//----- ICE -------------------------------------------------------------------
//----- C++ -------------------------------------------------------------------
//----- C ---------------------------------------------------------------------
//----- vdr -------------------------------------------------------------------
#include <vdr/tools.h>
#include <vdr/i18n.h>
#include <vdr/menuitems.h>

//----- AxLib -----------------------------------------------------------------
#include <Ax/Tools/Trace.h>
// #include <Ax/Tools/TraceOn.h>
#include <Ax/Mail/MailBoxMgr.h>

//----- local plugin ----------------------------------------------------------
#include "AxMenuConfigCommon.h"
#include "AxPluginMailBox.h"
#include "AxVdrTools.h"
#include "AxMailWrapper.h"

//----- locals ----------------------------------------------------------------

//=============================================================================
//     using namespaces
//=============================================================================
using namespace Ax;

//-----------------------------------------------------------------------------
//     AxMenuConfigCommon::AxMenuConfigCommon()
//-----------------------------------------------------------------------------
AxMenuConfigCommon::AxMenuConfigCommon(AxPluginMailBox *thePlugin)
                  : PARENT( tr("General settings")
                          , tr("General settings")
                          , 29
                          )
                  , myPlugin       (thePlugin)
                  , myTmpSettings  (thePlugin->getSetup().Settings)
                  , myIndexViewMode(-1)
{
#if VDRVERSNUM >= 10734
  SetMenuCategory(mcPluginSetup);
#endif 

  //----- General settings ----------------------------------------------
  addSeparator(tr("General settings"));
  Add(new cMenuEditIntItem(tr("Check every (minutes)"       ), &myTmpSettings.BGCheckDelay,  0, myTmpSettings.BGCheckDelayMax));
  Add(new cMenuEditIntItem(tr("Connection timeout (seconds)"), &myTmpSettings.ConnTimeOut ,  0,120));

  //----- Mail-List-View ------------------------------------------------
  addSeparator(tr("Display: List of Mails"));

  Add(new cMenuEditStraItem(tr("Sort order"                 ), &myTmpSettings.MLV_SortOrder, AxVdrTools::SO_COUNT, AxVdrTools::SortOrderStrings));
  Add(new cMenuEditIntItem (tr("Maximum number of mails"    ), &myTmpSettings.MLV_MaxMails , 0));

  //----- Mail-View -----------------------------------------------------
  addSeparator(tr("Display: Single mail"));
  myIndexViewMode = Count();
  Add(new cMenuEditStraItem(tr("Word-wrap"                  ), &myTmpSettings.MV_ViewMode  , AxMailWrapper::WM_COUNT, AxVdrTools::WrapModeStrings));

  //----- create actions ------------------------------------------------
  addAction(new Vdr::ActionCallback<PARENT>( this
                                           , A_SAVE_AND_CLOSE
                                           , 0
                                           , "SaveAndClose"
                                           , tr("Save the current settings and go back")
                                           ));

  //----- assign actions to keys ----------------------------------------
#if 0
  setKeyAction(kBack, A_GO_BACK       );
#endif
  setKeyAction(kOk  , A_SAVE_AND_CLOSE);

  setButtonActions( 0
                 // red             , green            , yellow           , blue
                  , A_NONE          , A_NONE           , A_NONE           , A_NONE
                  );

  //----- select the first item (after the separator) -------------------
  SetCurrent(Get(1));
  updateColorButtons();

} // AxMenuConfigCommon::AxMenuConfigCommon()

//-----------------------------------------------------------------------------
//     AxMenuConfigCommon::~AxMenuConfigCommon()
//-----------------------------------------------------------------------------
AxMenuConfigCommon::~AxMenuConfigCommon()
{
} // AxMenuConfigCommon::~AxMenuConfigCommon()

//-------------------------------------------------------------------------
//     AxMenuConfigCommon::processAction()
//-------------------------------------------------------------------------
eOSState AxMenuConfigCommon::processAction(const Ax::Vdr::Action &theAction, eOSState theState)
{
  switch (theAction.getID())
  {
    case A_SAVE_AND_CLOSE: //----- save the settings and close this -------
    {
      myPlugin->getSetup().Settings = myTmpSettings;

      myPlugin->getSetup().Settings.storeSetup(myPlugin);

      Ax::Mail::MailBoxMgr::instance()->setTimeOut(myPlugin->getSetup().Settings.ConnTimeOut );

      theState = osBack;
      break;
    }
    default: //----- give parents a chance --------------------------------
    {
      theState = PARENT::processAction(theAction, theState);
      break;
    }
  } // switch
  return theState;
} // AxMenuConfigCommon::processAction()

//-------------------------------------------------------------------------
//     AxMenuConfigCommon::informSubOSDClosed()
//-------------------------------------------------------------------------
void AxMenuConfigCommon::informSubOSDClosed(const Ax::Vdr::Action &theAction, eKeys theKey)
{
  Get(Current())->Set();
} // AxMenuConfigCommon::informSubOSDClosed()

//-------------------------------------------------------------------------
//     AxMenuConfigCommon::informCurrentChanged()
//-------------------------------------------------------------------------
void AxMenuConfigCommon::informCurrentChanged(int theOldCurrent)
{
  if (Current() == myIndexViewMode)
  {
    if (DisplayMenu()->GetTextAreaFont(false) == 0 || DisplayMenu()->GetTextAreaWidth() == 0)
    {
      showStatusMsg(tr("hint: 2..4 is not supported with the current skin"), 0);
    } // if
  }
  else
  {
    clearStatusMsg();
  } // if
} // AxMenuConfigCommon::updateActions()

