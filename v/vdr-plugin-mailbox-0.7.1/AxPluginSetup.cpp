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

//----- multiple defs of swap() workaround ------------------------------------
#include "AxPluginGlobals.h"

//----- qt --------------------------------------------------------------------
//----- CORBA -----------------------------------------------------------------
//----- ICE -------------------------------------------------------------------
//----- C++ -------------------------------------------------------------------
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

//----- C ---------------------------------------------------------------------
//----- vdr -------------------------------------------------------------------
//----- AxLib -----------------------------------------------------------------
#include <Ax/Tools/Trace.h>
// #include <Ax/Tools/TraceOn.h>
#include <Ax/Tools/Globals.h>
#include <Ax/Tools/String.h>
#include <Ax/Mail/MailBoxSetup.h>
#include <Ax/Mail/Tools.h>

//----- vdr -------------------------------------------------------------------
#include <vdr/config.h>
#include <vdr/plugin.h>
#include <vdr/config.h>

//----- local -----------------------------------------------------------------
#include "AxPluginSetup.h"

//----- locals ----------------------------------------------------------------

//=============================================================================
//     using namespaces
//=============================================================================
using namespace std;
using namespace Ax;

//=============================================================================
//     consts
//=============================================================================
const std::string AccountsFileName = std::string("accounts.conf");

//=============================================================================
//     static
//=============================================================================

//=============================================================================
//     class AxPluginSettings
//=============================================================================

//-------------------------------------------------------------------------
//     AxPluginSettings::AxPluginSettings()
//-------------------------------------------------------------------------
AxPluginSettings::AxPluginSettings()
{
  clear();
} // AxPluginSettings::AxPluginSettings()

//-------------------------------------------------------------------------
//     AxPluginSettings::clear()
//-------------------------------------------------------------------------
void AxPluginSettings::clear()
{
  BGCheckDelay     = 0;
  BGCheckDelayMax  = 35;

  MLV_SortOrder    = 0;
  MLV_MaxMails     = 100;
  MV_ViewMode      = 2;

  ConnTimeOut      = 0;

  MailInfoCmd      = "";
  MailConvCmd      = "";
  MailSrcFile      = Ax::Tools::String::sprintf("/tmp/vdr-mail-%05ld.html", Ax::Tools::getPID());
  MailDstFile      = Ax::Tools::String::sprintf("/tmp/vdr-mail-%05ld.txt" , Ax::Tools::getPID());

} // AxPluginSettings::clear()

//-------------------------------------------------------------------------
//     AxPluginSettings::parseSetup()
//-------------------------------------------------------------------------
bool AxPluginSettings::parseSetup(const char *theName, const char *theValue)
{
  bool fConsumed = true;

  if      (!strcasecmp(theName, "BGCheckDelay" ))
  {
    BGCheckDelay  = atoi(theValue);
    if (BGCheckDelay > BGCheckDelayMax)
    {
      wswarn(("mailbox: configuration value BGCheckDelay (%d) > maximum (%d) -> setting to %d"
            , BGCheckDelay, BGCheckDelayMax, BGCheckDelayMax
            ));
      BGCheckDelay  = BGCheckDelayMax;
    } // if
  }
  else if (!strcasecmp(theName, "MLV_SortOrder")) MLV_SortOrder = atoi(theValue);
  else if (!strcasecmp(theName, "MLV_MaxMails" )) MLV_MaxMails  = atoi(theValue);
  else if (!strcasecmp(theName, "MV_ViewMode"  )) MV_ViewMode   = atoi(theValue);
  else if (!strcasecmp(theName, "ConnTimeOut"  )) ConnTimeOut   = atoi(theValue);
  else if (!strcasecmp(theName, "DebugCClient" )) ; // ignore deprecated config entry
  else if (!strcasecmp(theName, "Timeout"      )) ; // ignore deprecated config entry
  else fConsumed = false;

return fConsumed;
} // AxPluginSettings::parseSetup()

//-------------------------------------------------------------------------
//     AxPluginSettings::storeSetup()
//-------------------------------------------------------------------------
void AxPluginSettings::storeSetup(cPlugin *thePlugin)
{
  thePlugin->SetupStore("BGCheckDelay"  , BGCheckDelay );
  thePlugin->SetupStore("MLV_SortOrder" , MLV_SortOrder);
  thePlugin->SetupStore("MLV_MaxMails"  , MLV_MaxMails );
  thePlugin->SetupStore("MV_ViewMode"   , MV_ViewMode  );
  thePlugin->SetupStore("ConnTimeOut"   , ConnTimeOut  );
} // AxPluginSettings::save()

//=============================================================================
//     class AxPluginSetup
//=============================================================================

//-------------------------------------------------------------------------
//     AxPluginSetup::AxPluginSetup()
//-------------------------------------------------------------------------
AxPluginSetup::AxPluginSetup()
{
} // AxPluginSetup::AxPluginSeSortOrdertup()

//-------------------------------------------------------------------------
//     AxPluginSetup::AxPluginSetup()
//-------------------------------------------------------------------------
AxPluginSetup::AxPluginSetup(const AxPluginSetup &that)
{
  copyFrom(that);
} // AxPluginSetup::AxPluginSetup()

//-------------------------------------------------------------------------
//     AxPluginSetup::operator=()
//-------------------------------------------------------------------------
AxPluginSetup &AxPluginSetup::operator=(const AxPluginSetup &that)
{
  if (this != &that)
  {
    copyFrom(that);
  } // if

  return *this;
} // AxPluginSetup::operator=()

//-------------------------------------------------------------------------
//     AxPluginSetup::~AxPluginSetup()
//-------------------------------------------------------------------------
AxPluginSetup::~AxPluginSetup()
{
  clear();
} // AxPluginSetup::~AxPluginSetup()

//-------------------------------------------------------------------------
//     AxPluginSetup::copyFrom()
//-------------------------------------------------------------------------
void AxPluginSetup::copyFrom(const AxPluginSetup &that)
{
  clear();

  Settings = that.Settings;

  for (Ax::Mail::MailBoxSetupCltn::const_iterator anIter  = that.Accounts.begin();
                                                  anIter != that.Accounts.end();
                                                ++anIter)
  {
    Accounts.push_back(new Mail::MailBoxSetup(**anIter));
  } // for

} // AxPluginSetup::copyFrom()

//-------------------------------------------------------------------------
//     AxPluginSetup::clear()
//-------------------------------------------------------------------------
void AxPluginSetup::clear()
{
  Settings.clear();
  clearAccounts();
} // AxPluginSetup::clear()

//-------------------------------------------------------------------------
//     AxPluginSetup::clearAccounts()
//-------------------------------------------------------------------------
void AxPluginSetup::clearAccounts()
{
  for (Ax::Mail::MailBoxSetupCltn::iterator anIter  = Accounts.begin();
                                            anIter != Accounts.end();
                                          ++anIter)
  {
    delete *anIter;
  } // for
  Accounts.clear();
} // AxPluginSetup::clearAccounts()

//-------------------------------------------------------------------------
//     AxPluginSetup::readKeyVal()
//-------------------------------------------------------------------------
bool AxPluginSetup::readKeyVal(const::std::string theLine, std::string &theKey, std::string &theVal)
{
  std::string::size_type aPosSep = theLine.find_first_of('=');

  if ( aPosSep > 0                     // - not at the first pos
    && aPosSep < theLine.length() - 1  // - not at the last  pos
     )
  {
    theKey = Ax::Tools::String::stripWhiteSpaces(theLine.substr(0, aPosSep));
    theVal = Ax::Tools::String::stripWhiteSpaces(theLine.substr(aPosSep + 1));
  }
  else
  {
    theKey = "";
    theVal = "";
  } // if

  wsdebug(("  AxPluginSetup::readKeyVal() Key: '%s'  Val: '%s'", theKey.c_str(), theVal.c_str()));

  return theKey.length() > 0;
} // AxPluginSetup::readKeyVal()

//-------------------------------------------------------------------------
//     AxPluginSetup::loadAccounts()
//-------------------------------------------------------------------------
bool AxPluginSetup::loadAccounts(const std::string &theConfigFolder)
{
  bool fSaveAccounts = false;

  std::string aFileName = std::string(theConfigFolder)
                        + std::string("/")
                        + AccountsFileName
                        ;
  wsdebug(("> AxPluginSetup::loadAccounts() FileName: '%s'", aFileName.c_str()));

  bool fOK = false;
  try
  {
    std::ifstream aFile(aFileName.c_str());
    if (!aFile)
    {
      throw std::string("unable to open file for reading");
    } // if

    Ax::Mail::MailBoxSetupCltn aSetupCltn;
    Ax::Mail::MailBoxSetup     aSetup;

    int aLineNum = 0;
    std::string aLine;

    while (std::getline(aFile, aLine).good())
    {
      ++aLineNum;
      if (aLine.length() == 0 || aLine.find_first_of('#') == 1)
      {
        continue; // skip empty lines or lines beginning with #
      } // if

      std::string aKey, aVal;
      bool fConsumed = true;
      if (aLine.find("[Account]") == 0)
      {
        if (aSetup.isValid())
        {
          aSetupCltn.push_back(new Ax::Mail::MailBoxSetup(aSetup));
        } // if
        aSetup.init();
      }
      else if (readKeyVal(aLine, aKey, aVal))
      {
        if      (aKey == "AccountName"    ) aSetup.setAccountName   (aVal);
        else if (aKey == "MailBox"        ) aSetup.setMailBoxString (aVal);
        else if (aKey == "LoginName"      ) aSetup.setUserName      (aVal);
        else if (aKey == "AccessCode"     ) aSetup.setAccessCode    (aVal);
        else if (aKey == "AccessMode"     ) aSetup.setAccessMode    (atoi(aVal.c_str()));
        else if (aKey == "AutoMarkSeen"   ) aSetup.setAutoMarkSeen  (atoi(aVal.c_str()));
        else if (aKey == "Expunge"        ) aSetup.setExpungeOnClose(atoi(aVal.c_str()));
        else if (aKey == "BackgroundCheck") aSetup.setBGCheckMode   (atoi(aVal.c_str()));
        else if (aKey == "LoginPass"      )
        {
          // if the password wasn't already read -> use the plain-text password
          if (aSetup.getPassword().length() == 0)
          {
            aSetup.setPassword(aVal);
          } // if

          // save the accounts if at least one plain-text password was found
          fSaveAccounts = true;
        }
        else if (aKey == "LoginPassEnc")
        {
          std::string aPwEnc(aVal);

          if ( aPwEnc.length() >= 4
            && aPwEnc.substr(0,2) == std::string("AX")
            && aPwEnc.substr(aPwEnc.length() - 2, 2) == std::string("M=")
             )
          {
            aPwEnc = aPwEnc.substr(2, aPwEnc.length() - 4);
          } // if

          std::string aPwDec;
          if (aPwEnc.length() > 0 && Ax::Mail::Tools::decodeBase64(aPwEnc, aPwDec))
          {
            aSetup.setPassword(aPwDec);
          }
          else
          {
            wswarn(("unable to decode password in line %d in %s", aLineNum, aFileName.c_str()));
          } // if
        }
        else
        {
          fConsumed = false;
        }
      }
      else
      {
        fConsumed = false;
      } // if

      if (!fConsumed)
      {
        wswarn(("unknown line %d '%s' in %s", aLineNum, aLine.c_str(), aFileName.c_str()));
      } // if
    } // while

    if (aSetup.isValid())
    {
      aSetupCltn.push_back(new Ax::Mail::MailBoxSetup(aSetup));
    } // if

    Accounts = aSetupCltn;
    fOK = true;
  }
  catch (const std::string &aMsg)
  {
    wserror(("error while reading %s: %s", aFileName.c_str(), aMsg.c_str()));
  }
  catch (...)
  {
    wserror(("error while reading %s", aFileName.c_str()));
  } // catch

  if (fSaveAccounts)
  {
    wsinfo(("saving accounts to obscure passwords..."));
    saveAccounts(theConfigFolder);
  } // if

  wsdebug(("< AxPluginSetup::loadAccounts() FileName: '%s'", aFileName.c_str()));
  return fOK;
} // AxPluginSetup::loadAccounts()

//-------------------------------------------------------------------------
//     AxPluginSetup::saveAccounts()
//-------------------------------------------------------------------------
bool AxPluginSetup::saveAccounts(const std::string &theConfigFolder)
{
  std::string aFileName = std::string(theConfigFolder)
                        + std::string("/")
                        + AccountsFileName
                        ;

  wsdebug(("> AxPluginSetup::saveAccounts() FileName: '%s'", aFileName.c_str()));

  // save current umask
  mode_t anOldUMASK = umask(0x477);

  bool fOK = false;
  try
  {
    std::ofstream aFile(aFileName.c_str());
    if (!aFile)
    {
      throw std::string("unable to open file for writing");
    } // if

    for (Ax::Mail::MailBoxSetupCltn::const_iterator anIter  = Accounts.begin();
                                                    anIter != Accounts.end();
                                                  ++anIter)
    {
      const Mail::MailBoxSetup *aSetup = *anIter;

      aFile << "[Account]"                                           << std::endl;
      aFile << "  AccountName     = " << aSetup->getAccountName   () << std::endl;
      aFile << "  LoginName       = " << aSetup->getUserName      () << std::endl;

      if (aSetup->getPassword().length() > 0)
      {
        //
        // obscure the password in a format that isn't easy readable for humans
        //
        // the password is simply encoded in Base64 just like ncftp stores its bookmarks
        //
        // aFile << "  LoginPass       = " << aSetup->getPassword      () << std::endl;
        std::string aPwEnc;
        if (Ax::Mail::Tools::encodeBase64(aSetup->getPassword(), aPwEnc))
        {
          if (aPwEnc[aPwEnc.length() - 1] == '\n')
          {
            aPwEnc = aPwEnc.substr(0, aPwEnc.length() - 2);
          } // if

          aPwEnc = std::string("AX") + aPwEnc + std::string("M="); // obscure a little more ;-)
          aFile << "  LoginPassEnc    = " << aPwEnc << std::endl;
        }
        else
        {
          wswarn(("unable to encode password for account '%s'", aSetup->getAccountName().c_str()));
        } // if
      } // if

      aFile << "  AccessCode      = " << aSetup->getAccessCode    () << std::endl;
      aFile << "  AccessMode      = " << aSetup->getAccessMode    () << std::endl;
      aFile << "  AutoMarkSeen    = " << aSetup->getAutoMarkSeen  () << std::endl;
      aFile << "  Expunge         = " << aSetup->getExpungeOnClose() << std::endl;
      aFile << "  BackgroundCheck = " << aSetup->getBGCheckMode   () << std::endl;
      aFile << "  MailBox         = " << aSetup->getMailBoxString () << std::endl;
      aFile                                                          << std::endl;
    } // for

    fOK = true;
  }
  catch (const std::string &aMsg)
  {
    wserror(("error while writing %s: %s", aFileName.c_str(), aMsg.c_str()));
  }
  catch (...)
  {
    wserror(("error while writing %s", aFileName.c_str()));
  } // catch

  // restore previouse umask
  umask(anOldUMASK);

  wsdebug(("< AxPluginSetup::saveAccounts() FileName: '%s'", aFileName.c_str()));
  return fOK;
} // AxPluginSetup::saveAccounts()

