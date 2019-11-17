/*
 * common.c: wirbelscan - A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */


/*
   Generic functions which will be used in the whole plugin.
*/

#include <stdarg.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <vdr/plugin.h>
#include <vdr/device.h>
#include <vdr/config.h>
#include "common.h"
#include "menusetup.h"

// plugin setup data
cMySetup::cMySetup(void) {
  logFile         = STDOUT;         /* log errors/messages to stdout */
  DVB_Type        = SCAN_TERRESTRIAL;
  verbosity       = 3;              /* default to messages           */
  CountryIndex    = 82;             /* default to Germany            */
  SatIndex        = 67;             /* default to Astra 19.2         */
  enable_s2       = 1;
  DVBC_Symbolrate = 0;              /* default to AUTO               */
  DVBC_QAM        = 0;              /* default to AUTO               */
  scanflags       = SCAN_TV | SCAN_RADIO | SCAN_FTA | SCAN_SCRAMBLED | SCAN_HD;
  initsystems     = false;
  enable_pvrinput = false;
}

void cMySetup::InitSystems(void) {
  int cardnr = 0;
  cDevice * d;

  memset(&systems[0], 0, sizeof(systems));

  while ((d = cDevice::GetDevice(cardnr++))) {
        if (d->ProvidesSource(cSource::stTerr))  systems[SCAN_TERRESTRIAL   ] = 1;
        if (d->ProvidesSource(cSource::stCable)) systems[SCAN_CABLE         ] = 1;
        if (d->ProvidesSource(cSource::stSat))   systems[SCAN_SATELLITE     ] = 1;
        if (d->ProvidesSource(cSource::stAtsc))  systems[SCAN_TERRCABLE_ATSC] = 1;
        }

  if (wSetup.enable_pvrinput && cPluginManager::GetPlugin("pvrinput")) {
     systems[SCAN_PVRINPUT   ] = 1;
     systems[SCAN_PVRINPUT_FM] = 1;
     }

  if (DVB_Type >= SCAN_NO_DEVICE || ! systems[DVB_Type]) {
     for (DVB_Type = scantype_t(SCAN_UNDEFINED + 1); DVB_Type < SCAN_NO_DEVICE; DVB_Type = scantype_t(DVB_Type + 1)) {
         if (systems[DVB_Type])
            break;
         }
     }
  initsystems = true;
}

cMySetup wSetup;                   

void dlog(const int level, const char *fmt, ...) {
  char t[BUFSIZ] = { 0 };
  va_list ap;
  time_t  now;
  if (wSetup.verbosity >= level){
    va_start(ap, fmt);
    switch (wSetup.logFile) {
      case STDOUT : time(&now);
                    vsnprintf(t + 9, sizeof t - 9, fmt, ap);
                    strftime(t, sizeof t, "%H:%M:%S", localtime(&now));
                    t[8] = ' ';
                    printf("\r%s\033[K\r\n", t);
                    fflush(stdout);
                    break;
      case SYSLOG : vsnprintf(t, sizeof t, fmt, ap);
                    syslog(LOG_DEBUG, "%s", t);
                    break;
      default:      printf("WARNING: setting logFile to %d\n",
                          (wSetup.logFile = STDOUT));
      }
    va_end(ap);
    if (MenuScanning)
      MenuScanning->AddLogMsg(t);
    }
}

void hexdump(const char * intro, const unsigned char * buf, int len) {
  int  i, j;
  char sbuf[17] = { 0 };

  if (wSetup.verbosity < 3) {
    return;
    }

  if (! buf) {
    dlog(0, "BUG: %s was called with buf = NULL", __FUNCTION__);
    return;
    }

  printf("\t===================== %s ", intro);
  for (i = strlen(intro) + 1; i < 50; i++)
    printf("=");
  printf("\n");
  printf("\tlen = %d\n", len);
  for (i = 0; i < len; i++) {
    if ((i % 16) == 0) {
      printf("%s0x%.2X: ", i ? "\n\t" : "\t", (i / 16) * 16);
      }
    printf("%.2X ", (uint8_t) *(buf + i));
    sbuf[i % 16] = *(buf + i);
    if (((i + 1) % 16) == 0) {
      // remove non-printable chars
      for (j = 0; j < 16; j++)
        if (!((sbuf[j] > 31) && (sbuf[j] < 127))) {
          sbuf[j] = ' ';
          }

      printf(": %s", sbuf);
      memset(&sbuf, 0, 17);
      }
    }
  if (len % 16) {
    for (i = 0; i < (len % 16); i++)
      if (!((sbuf[i] > 31) && (sbuf[i] < 127))) {
        sbuf[i] = ' ';
        }
    for (i = (len % 16); i < 16; i++)
      printf("   ");
    printf(": %s", sbuf);
    }
  printf("\n");
  printf("\t========================================================================\n");
}


int IOCTL(int fd, int cmd, void * data) {
  int retry;
    
  for (retry=10;retry>=0;) {
    if (ioctl(fd, cmd, data) != 0) {
      /* :-( */
      if (retry){
        usleep(10000); /* 10msec */
        retry--;
        continue;
        }
      return -1;       /* :'-((  */
      }
    else
      return 0;        /* :-)    */
    }
  return 0;
}


#include <sys/stat.h> 

bool FileExists(const char * aFile) {
  struct stat Stat; 

  if (! stat(aFile,&Stat))
     return true; 
  return false; 
}

/******************************************************************************
 * translate linuxtv inversions to VDR inversion identifiers. 
 *
 *****************************************************************************/

const char * vdr_inversion_name(int inversion) {
    switch(inversion) {
           case INVERSION_OFF: return "0";
           case INVERSION_ON:  return "1";
           default:            return "999";
           }
}

/******************************************************************************
 * translate linuxtv forw err corr to VDR fec identifiers. 
 *
 *****************************************************************************/

const char * vdr_fec_name(int fec) {
    switch(fec) {
           case FEC_NONE:      return "0";
           case FEC_1_2:       return "12";
           case FEC_2_3:       return "23";
           case FEC_3_4:       return "34";
           case FEC_3_5:       return "35";
           case FEC_4_5:       return "45";
           case FEC_5_6:       return "56";
           case FEC_6_7:       return "67";
           case FEC_7_8:       return "78";
           case FEC_8_9:       return "89";
           case FEC_9_10:      return "910";
           default:            return "999";
           }
}

/******************************************************************************
 * translate linuxtv modulation types to VDR > 1.7.0 identifiers. 
 *
 *****************************************************************************/

const char * vdr_modulation_name(int modulation) {
    switch(modulation) {    
           case QAM_16:        return "16"; 
           case QAM_32:        return "32"; 
           case QAM_64:        return "64"; 
           case QAM_128:       return "128"; 
           case QAM_256:       return "256";
           #ifdef QAM_512
           // not defined in Linux DVB API
           case QAM_512:       return "512";
           #endif
           #ifdef QAM_1024
           // not defined in Linux DVB API
           case QAM_1024:      return "1024";
           #endif
           #ifdef QAM_2048
           // not defined in Linux DVB API
           case QAM_2048:      return "2048";
           #endif
           #ifdef QAM_4096
           // not defined in Linux DVB API
           case QAM_4096:      return "4096";
           #endif 
           case QAM_AUTO:      return "998";
           case QPSK:          return "2";
           case PSK_8:         return "5";
           case APSK_16:       return "6";
           case APSK_32:       return "7";
           case VSB_8:         return "10";
           case VSB_16:        return "11";
           case DQPSK:         return "12";
           default:            return "999";
           }
}

/******************************************************************************
 * translate linuxtv bandwidth values to VDR identifiers. 
 *
 *****************************************************************************/
#define kHz (1000)
#define MHz (1000 * kHz)

const char * vdr_bandwidth_name (int bandwidth) {
  switch(bandwidth) {
     case 1712000:   return "1712";
     case (5 * MHz): return "5";
     case (6 * MHz): return "6";             
     case (7 * MHz): return "7";
     case (8 * MHz): return "8";
     case (10* MHz): return "10";
     default:        return "999";
     }                         
}
                                       
/******************************************************************************
 * translate linuxtv tm values to VDR identifiers. 
 *
 *****************************************************************************/

const char * vdr_transmission_mode_name (int transmission_mode) {
    switch(transmission_mode) {
           case TRANSMISSION_MODE_1K:  return "1";              
           case TRANSMISSION_MODE_2K:  return "2";
           case TRANSMISSION_MODE_4K:  return "4";
           case TRANSMISSION_MODE_8K:  return "8";
           case TRANSMISSION_MODE_16K: return "16";
           case TRANSMISSION_MODE_32K: return "32";
           default:                    return "999";
           }                         
}  

/******************************************************************************
 * translate linuxtv guard values to VDR identifiers. 
 *
 *****************************************************************************/

const char * vdr_guard_name (int guard_interval) {
    switch(guard_interval) {
           case GUARD_INTERVAL_1_4:    return "4";
           case GUARD_INTERVAL_1_8:    return "8";
           case GUARD_INTERVAL_1_16:   return "16";             
           case GUARD_INTERVAL_1_32:   return "32";
           case GUARD_INTERVAL_1_128:  return "128";
           case GUARD_INTERVAL_19_128: return "19128";
           case GUARD_INTERVAL_19_256: return "19256";
           default     :               return "999";
           }                         
}  

/******************************************************************************
 * translate linuxtv hierarchy values to VDR identifiers.
 * what about alpha? hm..
 *****************************************************************************/

const char * vdr_hierarchy_name (int hierarchy) {
    switch(hierarchy) {              
           case HIERARCHY_NONE: return "0";
           case HIERARCHY_1:    return "1";
           case HIERARCHY_2:    return "2";
           case HIERARCHY_4:    return "4";
           default:             return "999";
           }                         
}

/******************************************************************************
 * translate linuxtv rolloffs values to VDR identifiers. 
 *
 *****************************************************************************/

const char * vdr_rolloff_name(int rolloff) {
    switch(rolloff) {
           #if DVB_API_VERSION >= 5
           case ROLLOFF_20:    return "20";
           case ROLLOFF_25:    return "25";
           case ROLLOFF_35:    return "35";
           #endif              
           default:            return "35";
           }
}


const char * xine_transmission_mode_name (int transmission_mode) {
    switch(transmission_mode) {
           case TRANSMISSION_MODE_1K:  return "TRANSMISSION_MODE_1K";              
           case TRANSMISSION_MODE_2K:  return "TRANSMISSION_MODE_2K";
           case TRANSMISSION_MODE_4K:  return "TRANSMISSION_MODE_4K";
           case TRANSMISSION_MODE_8K:  return "TRANSMISSION_MODE_8K";
           case TRANSMISSION_MODE_16K: return "TRANSMISSION_MODE_16K";
           case TRANSMISSION_MODE_32K: return "TRANSMISSION_MODE_32K";
           default:                    return "TRANSMISSION_MODE_AUTO";
           }                         
}

int dvbc_modulation(int index) {
    switch(index) {
           case 0:             return QAM_64;
           case 1:             return QAM_256;
           case 2:             return QAM_128;
           default:            return QAM_AUTO;
           }
}

int dvbc_symbolrate(int index) {
    switch(index) {
           case 0:             return 6900000;
           case 1:             return 6875000;
           case 2:             return 6111000;
           case 3:             return 6250000;
           case 4:             return 6790000;
           case 5:             return 6811000;
           case 6:             return 5900000;
           case 7:             return 5000000;
           case 8:             return 3450000;
           case 9:             return 4000000;
           case 10:            return 6950000;
           case 11:            return 7000000;
           case 12:            return 6952000;
           case 13:            return 5156000;
           case 14:            return 5483000;
           default:            return 0;
           }
}

const char * xine_modulation_name(int modulation) {
    switch(modulation) {    
           case QPSK:          return "QPSK";
           case QAM_16:        return "QAM_16";
           case QAM_32:        return "QAM_32";
           case QAM_64:        return "QAM_64";
           case QAM_128:       return "QAM_128";
           case QAM_256:       return "QAM_256";
           case QAM_AUTO:      return "QAM_AUTO";
           case VSB_8:         return "VSB_8";
           case VSB_16:        return "VSB_16";
           case PSK_8:         return "PSK_8";
           case APSK_16:       return "APSK_16";
           case APSK_32:       return "APSK_32";
           case DQPSK:         return "DQPSK";
           default:            return "QAM_AUTO";
           }
}

const char * atsc_mod_to_txt ( int id ) {
    switch(id) {    
           case QAM_64:        return "QAM64";
           case QAM_256:       return "QAM256";
           case VSB_8:         return "8VSB";
           case VSB_16:        return "16VSB";
           default:            return "AUTO";
           }
}
