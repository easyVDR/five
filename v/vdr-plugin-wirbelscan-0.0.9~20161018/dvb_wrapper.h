/*
 * dvb_wrapper.h: wirbelscan - A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * wirbelscan-0.0.5
 *
 * $Id$ v20101210
 */

/* ARGHS! I hate that unsolved API problems with DVB..
 * Three different API approaches and one additional
 * wrapper to the old one for vdr-1.7.x 
 */

#ifndef __WIRBELSCAN_DVB_WRAPPER_H_
#define __WIRBELSCAN_DVB_WRAPPER_H_

#include <linux/dvb/frontend.h>
#include <linux/dvb/version.h>
#include <vdr/dvbdevice.h>
#include <vdr/channels.h>
#include "extended_frontend.h"

//-------------------------------------------------------------------

fe_code_rate_t          CableSatCodeRates (int cr);

fe_modulation_t         SatModulationTypes(int mt);
fe_rolloff_t            SatRollOffs       (int ro);
fe_polarization_t       SatPolarizations  (int sp);

fe_modulation_t         CableModulations  (int cm);

int                     TerrBandwidths    (int tb);
fe_modulation_t         TerrConstellations(int tc);
fe_hierarchy_t          TerrHierarchies   (int th);
fe_code_rate_t          TerrCodeRates     (int cr);
fe_guard_interval_t     TerrGuardIntervals(int gi);
fe_transmit_mode_t      TerrTransmissionModes(int tm);

int                     T2Bandwidths      (int tb);
fe_guard_interval_t     T2GuardIntervals  (int gi);
fe_transmit_mode_t      T2TransmissionModes(int tm);


char GetVDRPolarizationFromDVB(uint8_t Polarization);


void SetPids(cChannel* channel, int Vpid, int Ppid, int Vtype, int *Apids, int *Atypes, char ALangs[][MAXLANGCODE2], int *Dpids, int *Dtypes, char DLangs[][MAXLANGCODE2], int *Spids, char SLangs[][MAXLANGCODE2], int Tpid);

// debug print
cString PrintTransponder(const cChannel* Transponder);
cString PrintChannel(const cChannel* Channel);
cString PrintDvbApi(void);
cString PrintDvbApiUsed(cDevice* dev);

cString ParamsToString(char Type, char Polarization, int Inversion, int Bandwidth, int CoderateH, int CoderateL, int Modulation, int System, int Transmission, int Guard, int Hierarchy, int RollOff);

// DVB frontend capabilities
unsigned int GetFrontendStatus(cDevice* dev);

bool GetTerrCapabilities (cDevice* dev, bool* CodeRate, bool* Modulation, bool* Inversion, bool* Bandwidth, bool* Hierarchy, bool* TransmissionMode, bool* GuardInterval, bool* DvbT2);
bool GetCableCapabilities(cDevice* dev, bool* CodeRate, bool* Modulation, bool* Inversion);
bool GetAtscCapabilities (cDevice* dev, bool* CodeRate, bool* Modulation, bool* Inversion, bool* VSB, bool* QAM);
bool GetSatCapabilities  (cDevice* dev, bool* CodeRate, bool* Modulation, bool* RollOff, bool* DvbS2); //DvbS2: true if supported.
bool ValidSatfreq        (int f, const cChannel* Channel);

// plugin && vdr capabilities/properties
bool IsPvrinput(const cChannel* Channel);
bool IsScantype(scantype_t type, const cChannel* c);
bool ActiveTimers(scantype_t type);
bool PendingTimers(scantype_t type, int Margin);

#endif
