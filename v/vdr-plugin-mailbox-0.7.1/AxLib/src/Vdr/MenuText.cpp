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
 *   $Date: 2014-10-26 15:39:47 +0100 (So, 26 Okt 2014) $
 */
//=============================================================================

//=============================================================================
//     includes
//=============================================================================

//----- qt --------------------------------------------------------------------
//----- CORBA -----------------------------------------------------------------
//----- ICE -------------------------------------------------------------------
//----- C++ -------------------------------------------------------------------
#include <list>
//----- C ---------------------------------------------------------------------
//----- Vdr -------------------------------------------------------------------
#include <vdr/i18n.h>
#include <vdr/menuitems.h>

//----- AxLib -----------------------------------------------------------------
#include <Ax/Tools/Trace.h>
// #include <Ax/Tools/TraceOn.h>

//----- local plugin ----------------------------------------------------------
#include <Ax/Vdr/Tools.h>
#include <Ax/Vdr/MenuText.h>

//=============================================================================
//     namespaces
//=============================================================================
namespace Ax  {
namespace Vdr {

//=============================================================================
//     class MenuText
//=============================================================================

//-----------------------------------------------------------------------------
//     MenuText::MenuText()
//-----------------------------------------------------------------------------
MenuText::MenuText(const std::string &theInstName, const std::string &theTitle, const std::string theText)
        : PARENT(theInstName, theTitle)
        , myText(theText)
        , fMonospaced(false)
{
  wsdebug(("> MenuText::MenuText()"));
#if VDRVERSNUM >= 10734
  SetMenuCategory(mcText);
#endif 
  wsdebug(("< MenuText::MenuText()"));
} // MenuText::MenuText()

//-----------------------------------------------------------------------------
//     MenuText::~MenuText()
//-----------------------------------------------------------------------------
MenuText::~MenuText()
{
  wsdebug(("> MenuText::MenuText()"));
  wsdebug(("< MenuText::~MenuText()"));
} // MenuText::~MenuText()

//-------------------------------------------------------------------------
//     MenuText::Display()
//-------------------------------------------------------------------------
void MenuText::Display(void)
{
  wsdebug(("> MenuText::Display()"));

  PARENT::Display();

  if (!myText.empty())
  {
    DisplayMenu()->SetText(myText.c_str(), fMonospaced);
  } // if

  wsdebug(("< MenuText::Display()"));
} // MenuText::Display()

//-------------------------------------------------------------------------
//     MenuText::ProcessKey()
//-------------------------------------------------------------------------
eOSState MenuText::ProcessKey(eKeys theKey)
{
  //-----------------------------------------------------------------------
  // scroll the text-area
  //
  // ('borrowed from Vdr/menu.c:  eOSState cMenuText::ProcessKey(eKeys Key)
  //
  if (!HasSubMenu())
  {
    switch (int(theKey))
    {
      case kUp|k_Repeat:
      case kUp:
      case kDown|k_Repeat:
      case kDown:
      case kLeft|k_Repeat:
      case kLeft:
      case kRight|k_Repeat:
      case kRight:
      {
        DisplayMenu()->Scroll( NORMALKEY(theKey) == kUp   || NORMALKEY(theKey) == kLeft   // up?
                             , NORMALKEY(theKey) == kLeft || NORMALKEY(theKey) == kRight  // page?
                             );
        return osContinue;
        break;
      }
      default:
      {
        break;
      }
    } // switch
  } // if

  //-----------------------------------------------------------------------
  eOSState aState = PARENT::ProcessKey(theKey);

  return aState;
} // MenuText::ProcessKey()

//-------------------------------------------------------------------------
//     MenuText::setText()
//-------------------------------------------------------------------------
void MenuText::setText(const std::string &theText)
{
  myText = theText;
} // MenuText::setText()

//-------------------------------------------------------------------------
//     MenuText::setMonospaced()
//-------------------------------------------------------------------------
void MenuText::setMonospaced(bool theUseMonospaced)
{
  fMonospaced = theUseMonospaced;
} // MenuText::setMonospaced()

//=============================================================================
//     namespaces
//=============================================================================
} // Vdr
} // Ax

