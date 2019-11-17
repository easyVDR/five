/*
 * dvb_wrapper.c: wirbelscan - A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * wirbelscan-0.0.8
 *
 * $Id$ v20120525
 */

#include <linux/dvb/frontend.h>
#include <linux/dvb/version.h>
#include <vdr/config.h>
#include <vdr/dvbdevice.h>
#include <vdr/channels.h>
#include <vdr/timers.h>
#include <vdr/diseqc.h>
#include "common.h"
#include "dvb_wrapper.h"
#include <ctype.h>

#if (DVB_API_VERSION < 5)
  #error "YOUR LINUX DVB HEADERS/DRIVER ARE TOO OLD; PLEASE UPGRADE"
#endif

fe_code_rate_t CableSatCodeRates(int cr) {
  switch(cr) {
    case 1  : return FEC_1_2;
    case 2  : return FEC_2_3;
    case 3  : return FEC_3_4;
    case 4  : return FEC_5_6;
    case 5  : return FEC_7_8;
    case 6  : return FEC_8_9;
    case 7  : return FEC_3_5;
    case 8  : return FEC_4_5;
    case 9  : return FEC_9_10;
    case 15 : return FEC_NONE;
    default : dlog(0, "%s, unknown coderate %u", __FUNCTION__, cr);
              return FEC_AUTO;
    }
}

fe_modulation_t CableModulations(int cm) {
  switch(cm) {
    case 1   : return QAM_16;
    case 2   : return QAM_32;
    case 3   : return QAM_64;
    case 4   : return QAM_128;
    case 5   : return QAM_256;
    default  : dlog(0, "%s, unknown modulation %u", __FUNCTION__, cm);
               return QAM_AUTO;
    }
}

fe_polarization_t SatPolarizations(int sp) {
  switch(sp) {
    case 0  : return POLARIZATION_HORIZONTAL;
    case 1  : return POLARIZATION_VERTICAL;
    case 2  : return POLARIZATION_CIRCULAR_LEFT;
    case 3  : return POLARIZATION_CIRCULAR_RIGHT;
    default : dlog(0, "%s, unknown polarization %u", __FUNCTION__, sp);
              return POLARIZATION_HORIZONTAL;
    }
}

char GetVDRPolarizationFromDVB(uint8_t Polarization) {
 switch (Polarization) {
   case POLARIZATION_HORIZONTAL:     return 'h';
   case POLARIZATION_VERTICAL:       return 'v';
   case POLARIZATION_CIRCULAR_LEFT:  return 'l';
   case POLARIZATION_CIRCULAR_RIGHT: return 'r';
   default: dlog(0, "%s, unknown polarization %d", __FUNCTION__, Polarization);
            return 'h';
   }
}

fe_modulation_t SatModulationTypes(int mt) {
  switch(mt) {
    case 1   : return QPSK;
    case 3   : return QAM_16;
    case 2   : return PSK_8;
    default  : dlog(0, "%s, unknown modulation type %u", __FUNCTION__, mt);
               return QPSK;
    }
}

fe_rolloff_t SatRollOffs(int ro) {
  switch(ro) {
    case 0   : return ROLLOFF_35;
    case 1   : return ROLLOFF_25;
    case 2   : return ROLLOFF_20;
    default  : dlog(0, "%s, unknown rolloff %u", __FUNCTION__, ro);
               return ROLLOFF_35;
    }
}

int TerrBandwidths(int tb) {
  switch(tb) {
    case 0   : return 8000000;
    case 1   : return 7000000;
    case 2   : return 6000000;
    case 3   : return 5000000;
    default  : dlog(0, "%s, unknown bandwidth %u", __FUNCTION__, tb);
               return 8000000;
    }
}

fe_modulation_t TerrConstellations(int tc) {
  switch(tc) {
    case 0   : return QPSK;
    case 1   : return QAM_16;
    case 2   : return QAM_64;
    default  : dlog(0, "%s, unknown constellation %u", __FUNCTION__, tc);
               return QAM_AUTO;
    }
}

fe_hierarchy_t TerrHierarchies(int th) {
  switch(th & 0x3) {
    case 0   : return HIERARCHY_NONE;
    case 1   : return HIERARCHY_1;
    case 2   : return HIERARCHY_2;
    case 3   : return HIERARCHY_4;
    default  : dlog(0, "%s, unknown hierarchy %u", __FUNCTION__, th);
               return HIERARCHY_AUTO;
    }
}

fe_code_rate_t TerrCodeRates(int cr) {
  switch(cr) {
    case 0  : return FEC_1_2;
    case 1  : return FEC_2_3;
    case 2  : return FEC_3_4;
    case 3  : return FEC_5_6;
    case 4  : return FEC_7_8;
    default : dlog(0, "%s, unknown coderate %u", __FUNCTION__, cr);
              return FEC_AUTO;
    }
}

fe_guard_interval_t TerrGuardIntervals(int gi) {
  switch(gi) {
    case 0  : return GUARD_INTERVAL_1_32;
    case 1  : return GUARD_INTERVAL_1_16;
    case 2  : return GUARD_INTERVAL_1_8;
    case 3  : return GUARD_INTERVAL_1_4;
    default : dlog(0, "%s, unknown guard interval %u", __FUNCTION__, gi);
              return GUARD_INTERVAL_AUTO;
    }
}

fe_transmit_mode_t TerrTransmissionModes(int tm) {
  switch(tm) {
    case 0  : return TRANSMISSION_MODE_2K;
    case 1  : return TRANSMISSION_MODE_8K;
    case 2  : return TRANSMISSION_MODE_4K;
    default : dlog(0, "%s, unknown transm mode %u", __FUNCTION__, tm);
              return TRANSMISSION_MODE_AUTO;
    }
}

int T2Bandwidths(int tb) {
  switch(tb) {
    case 0 ... 3 : return TerrBandwidths(tb);
    case 4   : return 10000000;
    case 5   : return 1712000;
    default  : dlog(0, "%s, unknown bandwidth %u", __FUNCTION__, tb);
               return 8000000;
    }
}

fe_guard_interval_t T2GuardIntervals  (int gi){
  switch(gi) {
    case 0 ... 3  : return TerrGuardIntervals(gi);
    case 4  : return GUARD_INTERVAL_1_128;
    case 5  : return GUARD_INTERVAL_19_128;
    case 6  : return GUARD_INTERVAL_19_256;
    default : dlog(0, "%s, unknown guard interval %u", __FUNCTION__, gi);
              return GUARD_INTERVAL_AUTO;
    }
}
 
fe_transmit_mode_t T2TransmissionModes(int tm){
  switch(tm) {
    case 0 ... 2  : return TerrTransmissionModes(tm);
    case 3  : return TRANSMISSION_MODE_1K;
    case 4  : return TRANSMISSION_MODE_16K;
    case 5  : return TRANSMISSION_MODE_32K;
    default : dlog(0, "%s, unknown transm mode %u", __FUNCTION__, tm);
              return TRANSMISSION_MODE_AUTO;
    }
}

cString ParamsToString(char Type, char Polarization, int Inversion, int Bandwidth, int CoderateH, int CoderateL, int Modulation, int System, int Transmission, int Guard, int Hierarchy, int RollOff) {
 cDvbTransponderParameters * p = new cDvbTransponderParameters;
 p->SetPolarization(Polarization);
 p->SetInversion(Inversion);
 p->SetBandwidth(Bandwidth);
 p->SetCoderateH(CoderateH);
 p->SetCoderateL(CoderateL);
 p->SetModulation(Modulation);
 p->SetSystem(System);
 p->SetTransmission(Transmission);
 p->SetGuard(Guard);
 p->SetHierarchy(Hierarchy);
 p->SetRollOff(RollOff);
 return p->ToString(Type);
}


cDiseqc *GetDiseqc(int Source, int Frequency, char Polarization) {
  for (cDiseqc *p = Diseqcs.First(); p; p = Diseqcs.Next(p)) {
      if (p->Source() == Source && p->Slof() > Frequency && p->Polarization() == toupper(Polarization))
        return p;
      }
  return NULL;
}

bool ValidSatfreq(int f, const cChannel * Channel) {
  bool r = true;

  while (f > 999999) f /= 1000; 
  if (Setup.DiSEqC) {
     cDvbTransponderParameters * p = new cDvbTransponderParameters(Channel->Parameters());
     cDiseqc *diseqc = GetDiseqc(Channel->Source(), Channel->Frequency(), p->Polarization());
     free(p);
     if (diseqc)
        f -= diseqc->Lof();
     else {
        dlog(0, "no diseqc settings for %s", *PrintTransponder(Channel));
        return false;
        }
     }
  else {
     if (f < Setup.LnbSLOF)
        f -= Setup.LnbFrequLo;
     else
        f -= Setup.LnbFrequHi;
     }

  r = ((f >= 950) && (f <= 2150));
  if (!r)
     dlog(0, "transponder %s (freq %d -> out of tuning range)", *PrintTransponder(Channel), f);
  return r;
}

void SetPids(cChannel * channel, int Vpid, int Ppid, int Vtype, int *Apids, int *Atypes, char ALangs[][MAXLANGCODE2], int *Dpids, int *Dtypes, char DLangs[][MAXLANGCODE2], int *Spids, char SLangs[][MAXLANGCODE2], int Tpid) {
 channel->SetPids(Vpid, Ppid,
                  Vtype, /* vdr 1.7.1 added Vtype */
                  Apids,
                  Atypes,
                  ALangs, Dpids,
                  Dtypes,
                  DLangs,
                  Spids, SLangs,
                  Tpid);
}
 
int GetVDRInversionFromDVB(fe_spectral_inversion_t Inversion) {
 switch (Inversion) {
   case INVERSION_OFF:  return 0;
   case INVERSION_ON:   return 1;
   case INVERSION_AUTO: return 999;
   default:             dlog(0, "%s, unknown inversion %d",
                         __FUNCTION__, Inversion);
                        return 999;
   } 
}

int GetVDRBandwidthFromDVB(int Bandwidth) {
 switch (Bandwidth) {
   case 1712000:             return 1712;
   case 5000000:             return 5;
   case 6000000:             return 6;
   case 7000000:             return 7;
   case 8000000:             return 8;
   case 10000000:            return 10;
   default:                  dlog(0, "%s, unknown Bandwidth %d", __FUNCTION__, Bandwidth);
                             return 999;
   } 
}

int GetVDRCoderateFromDVB(fe_code_rate_t Coderate) {
 switch (Coderate) {
   case FEC_NONE: return   0;
   case FEC_1_2:  return  12;
   case FEC_2_3:  return  23;
   case FEC_3_4:  return  34;
   case FEC_4_5:  return  45;
   case FEC_5_6:  return  56;
   case FEC_6_7:  return  67;
   case FEC_7_8:  return  78;
   case FEC_8_9:  return  89;
   case FEC_AUTO: return 999;
   case FEC_3_5:  return  35;
   case FEC_9_10: return 910;
   default:       dlog(0, "%s, unknown Coderate %d", __FUNCTION__, Coderate);
                  return 999;
   } 
}

int GetVDRModulationtypeFromDVB(int Modulationtype) {
 switch (Modulationtype) {
   case QPSK:     return    2;
   case QAM_16:   return   16;
   case QAM_32:   return   32;
   case QAM_64:   return   64;
   case QAM_128:  return  128;
   case QAM_256:  return  256;
   case QAM_AUTO: return  999;
   case VSB_8:    return   10;
   case VSB_16:   return   11;
   case PSK_8:    return    5;
   case APSK_16:  return    6;
   case APSK_32:  return    7;
   case DQPSK:    return   12;
   default:       dlog(0, "%s, unknown Modulationtype %d",
                   __FUNCTION__, Modulationtype);
                  return  999;
   }
}

int GetVDRSystemFromDVB(fe_delivery_system_t System) {
 switch (System) {
   case SYS_DVBS2:
   case SYS_DVBT2: return 1;
   default:        return 0;
   }
}



int GetVDRTransmissionModeFromDVB(fe_transmit_mode_t TransmissionMode) {
 switch (TransmissionMode) {
   case TRANSMISSION_MODE_1K:   return   1;
   case TRANSMISSION_MODE_2K:   return   2;
   case TRANSMISSION_MODE_4K:   return   4;
   case TRANSMISSION_MODE_8K:   return   8;
   case TRANSMISSION_MODE_16K:  return   16;
   case TRANSMISSION_MODE_32K:  return   32;
   case TRANSMISSION_MODE_AUTO: return  999;
   default:                     dlog(0, "%s, unknown transmissionmode %d",
                                __FUNCTION__, TransmissionMode);
                                return 999;
   }
}

int GetVDRGuardFromDVB(fe_guard_interval_t Guard) {
 switch (Guard) {
   case GUARD_INTERVAL_1_4:    return   4;
   case GUARD_INTERVAL_1_8:    return   8;
   case GUARD_INTERVAL_1_16:   return  16;
   case GUARD_INTERVAL_1_32:   return  32;
   case GUARD_INTERVAL_1_128:  return  128;
   case GUARD_INTERVAL_19_128: return  19128;
   case GUARD_INTERVAL_19_256: return  19256;
   case GUARD_INTERVAL_AUTO:  return 999;
   default:                  dlog(0, "%s, unknown guard %d",
                             __FUNCTION__, Guard);
                             return 999;
   }
}

int GetVDRHierarchyFromDVB(fe_hierarchy_t Hierarchy) {
 switch (Hierarchy) {
   case HIERARCHY_NONE: return   0;
   case HIERARCHY_1:    return   1;
   case HIERARCHY_2:    return   2;
   case HIERARCHY_4:    return   4;
   case HIERARCHY_AUTO: return 999;
   default:             dlog(0, "%s, unknown hierarchy %d",
                        __FUNCTION__, Hierarchy);
                        return 999;
   }
}


int GetVDRRollOffFromDVB(fe_rolloff_t RollOff) {
 switch (RollOff) {
   case ROLLOFF_AUTO:    return 35;
   case ROLLOFF_20:      return 20;
   case ROLLOFF_25:      return 25;
   case ROLLOFF_35:      return 35;
   default:              dlog(0, "%s, unknown rolloff %d",
                         __FUNCTION__, RollOff);
                         return 999;
   }
}


cString PrintTransponder(const cChannel * Transponder) {
  cString buffer;
  int Frequency = Transponder->Frequency();
  if (Frequency < 1000)    Frequency *= 1000;
  if (Frequency > 999999)  Frequency /= 1000;
    switch (Transponder->Source()) {
        case cSource::stCable:
            {
            cDvbTransponderParameters * p = new cDvbTransponderParameters(Transponder->Parameters());
            buffer = cString::sprintf("DVB-C %.3fMHz M%d SR%d",
                 Frequency/1000.0,
                 GetVDRModulationtypeFromDVB((fe_modulation_t) p->Modulation()),
                 Transponder->Srate());
            }
            break;
        case cSource::stTerr:
            {
            cDvbTransponderParameters * p = new cDvbTransponderParameters(Transponder->Parameters());
            buffer = cString::sprintf("%s %.3fMHz M%dI%dB%dC%dD%dT%dG%dY%dS%d",
                 p->System()?"DVB-T2":"DVB-T",
                 Frequency/1000.0, GetVDRModulationtypeFromDVB((fe_modulation_t) p->Modulation()),
                 GetVDRInversionFromDVB((fe_spectral_inversion_t) p->Inversion()),
                 GetVDRBandwidthFromDVB(p->Bandwidth()),
                 GetVDRCoderateFromDVB((fe_code_rate_t) p->CoderateH()),
                 GetVDRCoderateFromDVB((fe_code_rate_t) p->CoderateL()),
                 GetVDRTransmissionModeFromDVB((fe_transmit_mode_t) p->Transmission()),
                 GetVDRGuardFromDVB((fe_guard_interval_t)p->Guard()),
                 GetVDRHierarchyFromDVB((fe_hierarchy_t) p->Hierarchy()),
                 p->System());
            }
            break;
        case cSource::stAtsc:
            {
            cDvbTransponderParameters * p = new cDvbTransponderParameters(Transponder->Parameters());
            buffer = cString::sprintf("ATSC %.3fMHz M%d",
                 Frequency/1000.0, GetVDRModulationtypeFromDVB((fe_modulation_t) p->Modulation()));
            }
            break;
        default:
          if (cSource::IsSat(Transponder->Source())) {
            cDvbTransponderParameters * p = new cDvbTransponderParameters(Transponder->Parameters());
            buffer = cString::sprintf("%s %d %cM%dC%dO%dS%d SR%d",
                 p->System()?"DVB-S2":"DVB-S",
                 Transponder->Frequency(),
                 p->Polarization(),
                 GetVDRModulationtypeFromDVB((fe_modulation_t) p->Modulation()),
                 GetVDRCoderateFromDVB((fe_code_rate_t) p->CoderateH()),
                 GetVDRRollOffFromDVB((fe_rolloff_t) p->RollOff()),
                 p->System(),                    
                 Transponder->Srate());
            }
          else {
            buffer = cString::sprintf("PVRINPUT %.2fMHz %s",
                 Frequency/1000.0,
                 Transponder->Parameters());
            }
        }
    return buffer;
}


cString PrintChannel(const cChannel * Channel) {
  cString buffer = cString::sprintf("%s", *Channel->ToText());
  buffer.Truncate(-1); //remove "\n"
  return buffer;
} 

cString PrintDvbApi(void) {
  cString buffer = cString::sprintf("compiled for DVB API %d.%d %s",
                DVB_API_VERSION,
                DVB_API_VERSION_MINOR,
                (DVB_API_VERSION < 5)?"*** warning: DVB-S2 support disabled ***":"");
  return buffer;
}

/* these structs should be used in PrintDvbApiUsed *only*
 */
typedef struct ___a_property {
    __u32 c;
    __u32 r0[3];
    union {__u32 d;struct {__u8 d[32];__u32 l;__u32 r1[3];void *r2;} b;} u;
    int result;
    } __attribute__ ((packed)) ___a_property_t;
typedef struct ___propertylist {__u32 count; ___a_property_t *properties; } ___propertylist_t;

cString PrintDvbApiUsed(cDevice * dev) {
  cString buffer = cString::sprintf("using DVB API 3.2");
  cDvbDevice* dvbdevice = (cDvbDevice*) dev;
  cString devstr = cString::sprintf("/dev/dvb/adapter%d/frontend%d",
                         dvbdevice->Adapter(),
                         dvbdevice->Frontend());
  int fe = open(*devstr, O_RDONLY | O_NONBLOCK);

  ___a_property_t p[1];
  ___propertylist_t pl;
  p[0].c = 35;
  pl.count = 1;
  pl.properties = p;
  if (ioctl(fe, _IOR('o', 83, ___propertylist_t), &pl)) {
    if (fe > -1) close(fe);
    return buffer;
    }
  close(fe);
  buffer = cString::sprintf("using DVB API %d.%d", p[0].u.d >> 8, p[0].u.d & 0xff);
  return buffer;
}

//bool SetSatTransponderDataFromVDR(cChannel * channel, int Source, int Frequency, eSatPolarizations Polarization, int Srate, eCableSatCodeRates CoderateH, eSatModulationTypes Modulation, eSatSystems System, eSatRollOffs RollOff) {
//  return SetSatTransponderDataFromDVB(channel, Source, Frequency, GetVDRPolarizationFromDVB(SatPolarizations(Polarization)), 
//                                      Srate, CableSatCodeRates(CoderateH), SatModulationTypes(Modulation), SatSystems(System), SatRollOffs(RollOff));
//}

//bool SetCableTransponderDataFromVDR(cChannel * channel, int Source, int Frequency, eCableModulations Modulation, int Srate, eCableSatCodeRates CoderateH, eCableTerrInversions Inversion) {
//  return SetCableTransponderDataFromDVB(channel, Source, Frequency, CableModulations(Modulation), Srate, CableSatCodeRates(CoderateH), CableTerrInversions(Inversion));
//}

//bool SetTerrTransponderDataFromVDR(cChannel * channel, int Source, int Frequency, eTerrBandwidths Bandwidth, eTerrConstellations Modulation, eTerrHierarchies Hierarchy, eTerrCodeRates CodeRateH, eTerrCodeRates CodeRateL, eTerrGuardIntervals Guard, eTerrTransmissionModes Transmission, eCableTerrInversions Inversion) {
//  return SetTerrTransponderDataFromDVB(channel, Source, Frequency, TerrBandwidths(Bandwidth), TerrConstellations(Modulation),
//                                      TerrHierarchies(Hierarchy), TerrCodeRates(CodeRateH), TerrCodeRates(CodeRateL),
//                                      TerrGuardIntervals(Guard), TerrTransmissionModes(Transmission), CableTerrInversions(Inversion));
//}


unsigned int GetFrontendStatus(cDevice * dev) {
  cDvbDevice* dvbdevice = (cDvbDevice*) dev;
  fe_status_t value;
  cString devstr = cString::sprintf("/dev/dvb/adapter%d/frontend%d",
                         dvbdevice->Adapter(),
                         dvbdevice->Frontend());

  int fe = open(*devstr, O_RDONLY | O_NONBLOCK);
  if (fe < 0) {
     dlog(0, "GetFrontendStatus(): could not open %s", *devstr);
     return 0;
     }
  if (IOCTL(fe, FE_READ_STATUS, &value) < 0) {
     close(fe);
     dlog(0, "GetFrontendStatus(): could not read %s", *devstr);
     return 0;
     }
  close(fe);
  return value;
}

unsigned int GetCapabilities(cDevice * dev) {
  cDvbDevice* dvbdevice = (cDvbDevice*) dev;
  struct dvb_frontend_info fe_info;
  cString devstr = cString::sprintf("/dev/dvb/adapter%d/frontend%d",
                         dvbdevice->Adapter(),
                         dvbdevice->Frontend());

  int fe = open(*devstr, O_RDONLY | O_NONBLOCK);
  if (fe < 0)
     return 0;

  if (IOCTL(fe, FE_GET_INFO, &fe_info) < 0) {
     close(fe);
     dlog(0, "GetCapabilities(): could not read %s", *devstr);
     return 0;
     }
  close(fe);
  return fe_info.caps;
}

bool GetTerrCapabilities (cDevice* dev, bool* CodeRate, bool* Modulation, bool* Inversion, bool* Bandwidth, bool* Hierarchy,
                          bool* TransmissionMode, bool* GuardInterval, bool* DvbT2) {
  unsigned int cap = GetCapabilities(dev);
  if (cap == 0)
     return false;
  *CodeRate         = cap & FE_CAN_FEC_AUTO;
  *Modulation       = cap & FE_CAN_QAM_AUTO;
  *Inversion        = cap & FE_CAN_INVERSION_AUTO; 
  *Bandwidth        = cap & FE_CAN_BANDWIDTH_AUTO;
  *Hierarchy        = cap & FE_CAN_HIERARCHY_AUTO;
  *TransmissionMode = cap & FE_CAN_GUARD_INTERVAL_AUTO;
  *GuardInterval    = cap & FE_CAN_TRANSMISSION_MODE_AUTO;
  *DvbT2            = cap & FE_CAN_2G_MODULATION;
  return true; 
}

bool GetCableCapabilities(cDevice * dev, bool *CodeRate, bool *Modulation, bool *Inversion) {
  int cap = GetCapabilities(dev);
  if (cap < 0)
     return false;
  *CodeRate         = cap & FE_CAN_FEC_AUTO;
  *Modulation       = cap & FE_CAN_QAM_AUTO;
  *Inversion        = cap & FE_CAN_INVERSION_AUTO;
  return true; 
}

bool GetAtscCapabilities (cDevice * dev, bool *CodeRate, bool *Modulation, bool *Inversion, bool *VSB, bool * QAM) {
  int cap = GetCapabilities(dev);
  if (cap < 0)
     return false;
  *CodeRate         = cap & FE_CAN_FEC_AUTO;
  *Modulation       = cap & FE_CAN_QAM_AUTO;
  *Inversion        = cap & FE_CAN_INVERSION_AUTO;
  *VSB              = cap & FE_CAN_8VSB;
  *QAM              = cap & FE_CAN_QAM_256;
  return true; 
}

bool GetSatCapabilities  (cDevice * dev, bool *CodeRate, bool *Modulation, bool *RollOff, bool *DvbS2) {
  int cap = GetCapabilities(dev);
  if (cap < 0)
     return false;
  *CodeRate         = cap & FE_CAN_FEC_AUTO;
  *Modulation       = cap & FE_CAN_QAM_AUTO;
  *RollOff          = cap & 0 /* there is no capability flag foreseen for rolloff auto? */ ;
  *DvbS2            = cap & FE_CAN_2G_MODULATION;
  return true; 
}

bool IsPvrinput(const cChannel * Channel) {
  return (Channel->IsSourceType('V'));
}

bool IsScantype(scantype_t type, const cChannel * c) {
  switch(type) {
        case SCAN_TERRESTRIAL:    return (c->IsTerr());
        case SCAN_CABLE:          return (c->IsCable());
        case SCAN_SATELLITE:      return (c->IsSat());
        case SCAN_PVRINPUT_FM: 
        case SCAN_PVRINPUT:       return (IsPvrinput(c));
        case SCAN_TERRCABLE_ATSC: return (c->IsAtsc());
        default:
                                  dlog(0, "%s(%d) UNKNOWN TYPE %d", __FUNCTION__, __LINE__, type);
                                  return false;
        }
}

bool ActiveTimers(scantype_t type) {
  for (cTimer * ti = Timers.First(); ti; ti = Timers.Next(ti))
      if (ti->Recording() && IsScantype(type, ti->Channel()))
         return true;

  return false;
}

bool PendingTimers(scantype_t type, int Margin) {
  time_t now = time(NULL);

  for (cTimer * ti = Timers.First(); ti; ti = Timers.Next(ti)) {
      ti->Matches();
      if (ti->HasFlags(tfActive) && (ti->StartTime() <= now + Margin) &&
           IsScantype(type, ti->Channel()))
         return true;
      }
  return false;
}
