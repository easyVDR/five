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
 *   $Date: 2013-03-20 19:53:42 +0100 (Mi, 20 Mär 2013) $
 */
//=============================================================================

//=============================================================================
//     includes
//=============================================================================

//----- qt --------------------------------------------------------------------
//----- CORBA -----------------------------------------------------------------
//----- ICE -------------------------------------------------------------------
//----- C++ -------------------------------------------------------------------
#include <string>

//----- C ---------------------------------------------------------------------
//----- Vdr -------------------------------------------------------------------
#include <vdr/tools.h>
#include <vdr/i18n.h>
#include <vdr/menuitems.h>
#include <vdr/device.h>
#include <vdr/channels.h>

//----- AxLib -----------------------------------------------------------------
#include <Ax/Tools/Trace.h>
// #include <Ax/Tools/TraceOn.h>

#include <Ax/Vdr/Tools.h>

//----- local plugin ----------------------------------------------------------
//----- locals ----------------------------------------------------------------


//=============================================================================
//     namespaces
//=============================================================================
namespace Ax  {
namespace Vdr {

//=============================================================================
//     class Tools
//=============================================================================

//-------------------------------------------------------------------------
//     Tools::getOSStateStr()
//-------------------------------------------------------------------------
const char *Tools::getOSStateStr(eOSState theState)
{
  switch (theState)
  {
    case osUnknown   : return "osUnknown";
    case osContinue  : return "osContinue";
    case osSchedule  : return "osSchedule";
    case osChannels  : return "osChannels";
    case osTimers    : return "osTimers";
    case osRecordings: return "osRecordings";
    case osPlugin    : return "osPlugin";
    case osSetup     : return "osSetup";
    case osCommands  : return "osCommands";
    case osPause     : return "osPause";
    case osRecord    : return "osRecord";
    case osReplay    : return "osReplay";
    case osStopRecord: return "osStopRecord";
    case osStopReplay: return "osStopReplay";
    case osCancelEdit: return "osCancelEdit";
    case osSwitchDvb : return "osSwitchDvb";
    case osBack      : return "osBack";
    case osEnd       : return "osEnd";
    case os_User     : return "os_User";
    case osUser1     : return "osUser1";
    case osUser2     : return "osUser2";
    case osUser3     : return "osUser3";
    case osUser4     : return "osUser4";
    case osUser5     : return "osUser5";
    case osUser6     : return "osUser6";
    case osUser7     : return "osUser7";
    case osUser8     : return "osUser8";
    case osUser9     : return "osUser9";
    case osUser10    : return "osUser10";
    default: break;
  } // switch
  return "NO_ENUM";
} // Tools::getOSStateStr()

//-------------------------------------------------------------------------
//     Tools::getKeyStr()
//-------------------------------------------------------------------------
std::string Tools::getKeyStr(eKeys theKey)
{
  std::string aStr("(UNKNWON)");

  const char *aKeyStr = cKey::ToString(eKeys(theKey & ~ k_Flags));
  if (aKeyStr)
  {
    aStr = std::string(aKeyStr);
    if ((theKey & k_Repeat ) != 0) aStr += " | Rep";
    if ((theKey & k_Release) != 0) aStr += " | Rel";
  } // if

  return aStr;
} // Tools::getKeyStr()

#if defined (APIVERSNUM) && (APIVERSNUM < 20300)
//-------------------------------------------------------------------------
//     Tools::dumpDevices()
//-------------------------------------------------------------------------
void Tools::dumpDevices()
{
  wsinfo(("========================================================================"));
  wsinfo(("NumDevices: %d", cDevice::NumDevices()));

  for (int i = 0; i < cDevice::NumDevices(); ++i)
  {
    dumpDevice(cDevice::GetDevice(i));
  } // for
  wsinfo(("========================================================================"));

} // Tools::dumpDevices()

//-------------------------------------------------------------------------
//     Tools::dumpDevice()
//-------------------------------------------------------------------------
void Tools::dumpDevice(const cDevice *theDevice)
{
  if (theDevice == 0)
  {
    wsinfo(("Device [    ]    == NULL"));
  }
  else
  {
    wsinfo(("Device [%4d] %c%c = %p  CardIndex: %d  IsPrimary: %d  HasDecoder: %d"
          , theDevice->DeviceNumber()
          , theDevice == cDevice::PrimaryDevice() ? 'P' : ' '
          , theDevice == cDevice::ActualDevice () ? 'A' : ' '
          , theDevice
          , theDevice->CardIndex()
          , theDevice->IsPrimaryDevice()
          , theDevice->HasDecoder()
          ));
  } // if
} // Tools::dumpDevice()

//-------------------------------------------------------------------------
//     Tools::dumpChannels()
//-------------------------------------------------------------------------
void Tools::dumpChannels()
{
  wsinfo(("========================================================================"));
  wsinfo(("Channels: MaxNumber: %d", Channels.MaxNumber()));

  for (cChannel *aChannel = Channels.First();
                 aChannel;
                 aChannel = dynamic_cast<cChannel *>(aChannel->Next()))
  {
    if (aChannel != Channels.First()) wsinfo((" "));
    dumpChannel(aChannel);
  } // while
  wsinfo(("========================================================================"));

} // Tools::dumpChannels()

//-------------------------------------------------------------------------
//     Tools::dumpChannel()
//-------------------------------------------------------------------------
void Tools::dumpChannel(const cChannel *theChannel)
{
  if (theChannel == 0)
  {
    wsinfo(("Channel[    ]    == NULL"));
  }
  else
  {
    wsinfo(("Channel[%4d] = %p  %s"
          ,  theChannel->Number()
          ,  theChannel
          , *theChannel->ToText()
          ));

    wsinfo(("  Name      : '%s'", theChannel->Name()));
    wsinfo(("  ShortName : '%s'", theChannel->ShortName()));
    wsinfo(("  Provider  : '%s'", theChannel->Provider()));
    wsinfo(("  PortalName: '%s'", theChannel->PortalName()));
    wsinfo(("  Frequency : %5d   Transponder: %5d", theChannel->Frequency(), theChannel->Transponder()));
    wsinfo(("  Source    : %5d  Srate: %5d  Vpid: %5d  Ppid: %5d  Tpid: %5d  Nid: %d  Tid: %d  Sid: %d  Rid: %d  Par: %s"
          , theChannel->Source()
          , theChannel->Srate()
          , theChannel->Vpid()
          , theChannel->Ppid()
          , theChannel->Tpid()
          , theChannel->Nid()
          , theChannel->Tid()
          , theChannel->Sid()
          , theChannel->Rid()
          , theChannel->Parameters()
          ));

#if 0
  int Inversion(void) const { return inversion; }
  int Bandwidth(void) const { return bandwidth; }
  int CoderateH(void) const { return coderateH; }
  int CoderateL(void) const { return coderateL; }
  int Modulation(void) const { return modulation; }
  int Transmission(void) const { return transmission; }
  int Guard(void) const { return guard; }
  int Hierarchy(void) const { return hierarchy; }
  bool IsCable(void) const { return cSource::IsCable(source); }
  bool IsSat(void) const { return cSource::IsSat(source); }
  bool IsTerr(void) const { return cSource::IsTerr(source); }
#endif

#if 0
    wsinfo(("Channel[%2d] %c%c = %p  CardIndex: %d  IsPrimary: %d  HasDecoder: %d"
          , theDevice->DeviceNumber()
          , theDevice == cDevice::PrimaryDevice() ? 'P' : ' '
          , theDevice == cDevice::ActualDevice () ? 'A' : ' '
          , theDevice
          , theDevice->CardIndex()
          , theDevice->IsPrimaryDevice()
          , theDevice->HasDecoder()
          ));
#endif
  } // if
} // Tools::dumpChannel()
#endif

//=============================================================================
//     namespaces
//=============================================================================
} // Vdr
} // Ax
