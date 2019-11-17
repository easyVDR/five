//=============================================================================
/*
 * See the file README in the main directory for a description of
 * this software, copyright information, and how to reach the author.
 *
 * Author: alex
 * Date:   16.09.2004
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
//----- C ---------------------------------------------------------------------
//----- Vdr -------------------------------------------------------------------
#include <vdr/i18n.h>
#include <vdr/remote.h>

//----- AxLib -----------------------------------------------------------------
#include <Ax/Tools/String.h>
#include <Ax/Vdr/MenuHelpKeys.h>

//----- local plugin ----------------------------------------------------------

//=============================================================================
//     namespaces
//=============================================================================
namespace Ax  {
namespace Vdr {

//=============================================================================
//     class MenuHelpItem
//=============================================================================
class MenuHelpItem : public cOsdItem
{
  typedef cOsdItem PARENT;

  public:
    //-------------------------------------------------------------------------
    //     MenuHelpItem()
    //-------------------------------------------------------------------------
    MenuHelpItem(Ax::Vdr::Action *theAction, const std::string &theText)
      : PARENT(theText.c_str())
      , myAction(theAction)
    {}

    //-------------------------------------------------------------------------
    //     ~MenuHelpItem()
    //-------------------------------------------------------------------------
    virtual ~MenuHelpItem()
    {}

    //-------------------------------------------------------------------------
    //     getAction()
    //-------------------------------------------------------------------------
    Ax::Vdr::Action *getAction() const { return myAction; }

  private:
    //-------------------------------------------------------------------------
    //     attributes
    //-------------------------------------------------------------------------
    Ax::Vdr::Action *myAction;

}; // class MenuHelpItem

//=============================================================================
//     class MenuHelpKeys
//=============================================================================

//-----------------------------------------------------------------------------
//     MenuHelpKeys::MenuHelpKeys()
//-----------------------------------------------------------------------------
template <typename TT>
Ax::Vdr::MenuHelpKeys<TT>::MenuHelpKeys(const Ax::Vdr::OsdMenuBase<TT> *theDocuMenu)
                         : PARENT( tr("Key mapping")
                                 , std::string(std::string(tr("Key mapping")) + std::string(": ") + (theDocuMenu ? theDocuMenu->getName() : std::string("")))
                                 , 8
                                 )
                         , myDocuMenu(theDocuMenu)
{
#if VDRVERSNUM >= 10734
  SetMenuCategory(mcPlugin);
#endif 
  
  //----- assign actions --------------------------------------------------
  addAction(new Vdr::ActionCallback<MYSELF>( this
                                           , A_ACTIVATE_KEY
                                           , tr("Activate")
                                           , "Activate"
                                           , tr("Activate the selected action")
                                           ));
#if 0
  setKeyAction(kBack       , A_GO_BACK     );
#endif
  setKeyAction(kOk         , A_ACTIVATE_KEY);
  setKeyAction(kRed        , A_ACTIVATE_KEY);

  setActionSupported(A_HELP_KEYS, false); // disable help-Action in help-Display

  //----- show key mappings -----------------------------------------------
  for (eKeys aKey = kUp; aKey < kNone; aKey = eKeys(aKey + 1))
  {
    Action *anAction = myDocuMenu->getKeyAction(aKey);

    if ( anAction
      && anAction->isSupported()
      && anAction->getDesc().length() > 0
#if 1 // don't show help for some internal actions
      && anAction->getID() != A_NONE
      && anAction->getID() != A_IGNORE
      && anAction->getID() != A_HELP_KEYS
#endif
       )
    {
      std::string aStr = Ax::Tools::String::sprintf( "%s\t%s"
#if APIVERSNUM < 10509
                                                   , (aKey < k0 || aKey > k9)
                                                   ? tr(Ax::Tools::String::sprintf("Key$%s", cKey::ToString(aKey)).c_str())
                                                   : cKey::ToString(aKey)
#else
                                                   , cKey::ToString(aKey, true)
#endif                                                     
                                                   , anAction->getDesc().c_str()
                                                   );

      cOsdItem *anItem = new MenuHelpItem(anAction, aStr);
      if (!anAction->isEnabled())
      {
        anItem->SetSelectable(false);
      } // if
      Add(anItem);
    } // if
  } // for

#if 0 // disabled because we can't activate them from here (right now)
  //----- show all actions without keys -----
  for (OsdMenu::ID2ActionMap::const_iterator anIter  = myDocuMenu->getActions().begin();
                                             anIter != myDocuMenu->getActions().end();
                                           ++anIter)
  {
    Action *anAction = anIter->second;
    int     aKey     = myDocuMenu->getActionKey(anAction);

    if ( anAction
      && anAction->isSupported()
      && anAction->getDesc().length() > 0
      && aKey == kNone
#if 1 // don't show help for some internal actions
      && anAction->getID() != A_NONE
      && anAction->getID() != A_IGNORE
      && anAction->getID() != A_HELP_KEYS
#endif
       )
    {
      std::string aStr = Ax::Tools::String::sprintf( "\t%s"
                                                  , anAction->getDesc().c_str()
                                                  );

      cOsdItem *anItem = new MenuHelpItem(anAction, aStr);
      if ( !anAction->isEnabled()
#if 1 // can't activate actions without keys
        && true
#endif
         )
      {
        anItem->SetSelectable(false);
      } // if
      Add(anItem);
    } // if
  } // for
#endif

} // MenuHelpKeys::MenuHelpKeys()

//-------------------------------------------------------------------------
//     MenuHelpKeys::~MenuHelpKeys()
//-------------------------------------------------------------------------
template <typename TT>
Ax::Vdr::MenuHelpKeys<TT>::~MenuHelpKeys()
{
} // MenuHelpKeys::~MenuHelpKeys()

//-------------------------------------------------------------------------
//     MenuHelpKeys::processAction()
//-------------------------------------------------------------------------
template <typename TT>
eOSState Ax::Vdr::MenuHelpKeys<TT>::processAction(const Ax::Vdr::Action &theAction, eOSState theState)
{
  switch(theAction.getID())
  {
    case A_ACTIVATE_KEY: //----- activate action for key ------------------
    {
      theState = osContinue;

      MenuHelpItem *anItem = dynamic_cast<MenuHelpItem *>(getCurrentItem());

      if (anItem && anItem->getAction() && anItem->getAction()->isEnabled())
      {
        int aKey = myDocuMenu->getActionKey(anItem->getAction());
        if (kUp <= aKey && aKey < kNone)
        {
          cRemote::Put(eKeys(aKey));
          theState = osBack;
        } // if
      } // if
      break;
    }
    default: //----- give parents a chance --------------------------------
    {
      theState = PARENT::processAction(theAction, theState);
      break;
    }
  } // switch

  return theState;
} // MenuHelpKeys::processAction()

//=============================================================================
//     namespaces
//=============================================================================
} // Vdr
} // Ax

