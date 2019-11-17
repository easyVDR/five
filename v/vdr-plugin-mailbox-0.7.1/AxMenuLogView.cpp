//=============================================================================
/*
 * See the file README in the main directory for a description of
 * this software, copyright information, and how to reach the author.
 *
 * Author: alex
 * Date:   13.09.2005
 *
 * Last modfied:
 *   $Author: alex $
 *   $Date: 2005-09-12 19:09:16 +0200 (Mo, 12 Sep 2005) $
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
#include <string>

//----- C ---------------------------------------------------------------------
//----- vdr -------------------------------------------------------------------
#include <vdr/tools.h>
#include <vdr/font.h>
#include <vdr/i18n.h>
#include <vdr/menuitems.h>
#include <vdr/remote.h>

//----- AxLib -----------------------------------------------------------------
#include <Ax/Tools/Trace.h>
// #include <Ax/Tools/TraceOn.h>
#include <Ax/Tools/Globals.h>

//----- local plugin ----------------------------------------------------------
#include "AxMenuLogView.h"

//=============================================================================
//     using namespaces
//=============================================================================
using namespace std;
using namespace Ax;

//=============================================================================
//     class AxMenuLogView
//=============================================================================

//-----------------------------------------------------------------------------
//     AxMenuLogView::AxMenuLogView()
//-----------------------------------------------------------------------------
AxMenuLogView::AxMenuLogView(Ax::Mail::LogHandler &theLogHandler)
             : PARENT( tr("Log-view")
                     , tr("Log-view")
                     )
             , myLogHandler(theLogHandler)
{
  //----- create actions ------------------------------------------------------
  addAction(new Vdr::ActionCallback<PARENT>( this
                                           , A_CLEAR_LOG
                                           , tr("Clear")
                                           , "ClearLog"
                                           , tr("Clear log and close")
                                           ));

  //----- assign actions to keys ----------------------------------------------
  setKeyAction(kRed, A_CLEAR_LOG);

  //----- fill log text -------------------------------------------------------
  std::string aLogText = "";
  Mail::LogCltn::const_iterator aEnd = myLogHandler.getRecords().end();
  for (Mail::LogCltn::const_iterator anIter  = myLogHandler.getRecords().begin();
                                     anIter != aEnd;
                                   ++anIter)
  {
    aLogText += anIter->tmsg() + std::string("\n");
  } // for

  setText(aLogText);

  if (myLogHandler.isEmpty())
  {
    showStatusMsg(tr("No log available"), 0);
  } // if

} // AxMenuLogView::AxMenuLogView()

//-----------------------------------------------------------------------------
//     AxMenuLogView::~AxMenuLogView()
//-----------------------------------------------------------------------------
AxMenuLogView::~AxMenuLogView()
{
} // AxMenuLogView::~AxMenuLogView()

//-------------------------------------------------------------------------
//     AxMenuLogView::processAction()
//-------------------------------------------------------------------------
eOSState AxMenuLogView::processAction(const Ax::Vdr::Action &theAction, eOSState theState)
{
  switch (theAction.getID())
  {
    case A_CLEAR_LOG:
    {
      myLogHandler.clear();
      theState = osBack;  // kein log mehr vorhanden -> close
      break;
    }
    default: //----- give parents a chance --------------------------------
    {
      theState = PARENT::processAction(theAction, theState);
      break;
    }
  } // switch
  return theState;
} // AxMenuLogView::processAction()

