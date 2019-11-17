/*
 * serial plugin for VDR
 *
 * Copyright (C) 2003 Andreas Regel <andreas.regel@gmx.de>
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 *
 */

#include <vdr/plugin.h>
#include <vdr/remote.h>
#include <vdr/thread.h>
#include <vdr/status.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <getopt.h>

#define LEDREC TIOCM_RTS
#define LED2 TIOCM_SR
#define MAXCYCLE 10
#define MAXDELAY 7

static const char *VERSION        = "0.0.6d";
static const char *DESCRIPTION    = "Frontpanel plugin for the serial line";
//static const char *MAINMENUENTRY  = NULL;

static const char *SerPort[] = { "", "/dev/ttyS0", "/dev/ttyS1", "/dev/ttyS2", "/dev/ttyS3" };

// Setup Menu
struct sSerialSetup {
  int iFlash;
  int Port;
  int kpDelay;
  int iInstDev;
};

sSerialSetup SerialSetup;

class cMenuSetupSerial : public cMenuSetupPage {
private:
  sSerialSetup newSerialSetup;
protected:
  virtual void Store(void);
public:
  cMenuSetupSerial(void);
};

cMenuSetupSerial::cMenuSetupSerial(void) {
  newSerialSetup.iFlash = SerialSetup.iFlash;
  newSerialSetup.Port = SerialSetup.Port;
  newSerialSetup.iInstDev = SerialSetup.iInstDev;
  newSerialSetup.kpDelay = SerialSetup.kpDelay;
  Add(new cMenuEditIntItem( tr("Flashrate"), &newSerialSetup.iFlash, 1, MAXCYCLE));
  Add(new cMenuEditIntItem( tr("Serial Port (COM)"), &newSerialSetup.Port, 1, 4));
  Add(new cMenuEditIntItem( tr("Installed Devices"), &newSerialSetup.iInstDev, 1, MAXDEVICES));
  Add(new cMenuEditIntItem( tr("Key press delay"), &newSerialSetup.kpDelay, 0, MAXDELAY));
}

void cMenuSetupSerial::Store(void) {
  SetupStore("Flashrate", SerialSetup.iFlash = newSerialSetup.iFlash);
  SetupStore("Port", SerialSetup.Port = newSerialSetup.Port);
  SetupStore("InstalledDevices", SerialSetup.iInstDev = newSerialSetup.iInstDev);
  SetupStore("KeyPressDelay", SerialSetup.kpDelay = newSerialSetup.kpDelay);
}

class cSerialStatus : public cStatus {
private:
  int iLedStatus[MAXCYCLE-1];
  int iNumDevices;

protected:
#if VDRVERSNUM < 10338
  virtual void Recording(const cDevice *Device, const char *Name);
#else
  virtual void Recording(const cDevice *Device, const char *Name, const char *FileName, bool On);
#endif

public:
  int GetLedStatus(int i);
  cSerialStatus(void);
  int iCardIsRecording[MAXDEVICES-1];
};

cSerialStatus::cSerialStatus(void) {
  for(int i=0; i<MAXCYCLE; i++)    iLedStatus[i] = 0;
  for(int i=0; i<MAXDEVICES; i++)  iCardIsRecording[i] = 0;
  iNumDevices = cDevice::NumDevices();
  dsyslog("SERIAL: iNumDevices = %d", iNumDevices);
  dsyslog("SERIAL: VERSION 0.0.6d");
}

int cSerialStatus::GetLedStatus(int j) {
  int iOccupiedDevices = 0;
  int i;

  for(i=0;i<iNumDevices;i++) {
    if (iCardIsRecording[i]> 0) iOccupiedDevices++;
  }

  if(iNumDevices = (iOccupiedDevices > (SerialSetup.iInstDev-1))) {               // Blinken
//  if(iNumDevices = (iOccupiedDevices > 3)) {              // Blinken
//  if(iNumDevices == iOccupiedDevices) {                   // Blinken
    for(i=0;i<MAXCYCLE;i++) {
      if (i<SerialSetup.iFlash) iLedStatus[i] |=  LEDREC;  // Bit setzen
      else  iLedStatus[i] &= ~LEDREC;                      // Bit loeschen
    }
  } else if(iOccupiedDevices > 0) {                        // Dauerleuchten
    for(i=0;i<MAXCYCLE;i++) iLedStatus[i] |=  LEDREC;      // Bit setzen
  } else {                                                 // LED aus
    for(i=0;i<MAXCYCLE;i++) iLedStatus[i] &= ~LEDREC;      // Bit loeschen
  }
  return iLedStatus[j % MAXCYCLE];
}

//void cSerialStatus::Recording(const cDevice *Device, const char *Name) {
void cSerialStatus::Recording(const cDevice *Device, const char *Name, const char *FileName, bool On) {
  int iCardIndex = Device->CardIndex();
  iNumDevices = Device->NumDevices();

  if(iCardIndex < MAXDEVICES) {
    if(Name && Name[0]) iCardIsRecording[iCardIndex]++;
    else                iCardIsRecording[iCardIndex]--;
  }
}

class cSerialRemote : public cRemote, private cThread
{
private:
  int fd;
  int iCycle;
  virtual void Action(void);
  virtual void SetState(void);
  cSerialStatus *cSSstat;

public:
  cSerialRemote(cSerialStatus *stat);
  virtual ~cSerialRemote(void);
  int Open(const char *device, cSerialStatus *stat);
  void Close(int fd);
};

cSerialRemote::cSerialRemote(cSerialStatus *stat)
  :cRemote("Serial")
{
  fd = Open(SerPort[SerialSetup.Port], stat);
}

cSerialRemote::~cSerialRemote()
{
  int state = 0;
  ioctl(fd, TIOCMSET, &state);
  Cancel();
}

void cSerialRemote::SetState(void)
{
  int iStateWithDTR = 0;

  iCycle = (iCycle + 1) % MAXCYCLE;
  iStateWithDTR = cSSstat->GetLedStatus(iCycle);
  iStateWithDTR |= TIOCM_DTR;
  ioctl(fd, TIOCMSET, &iStateWithDTR);
}

void cSerialRemote::Action(void)
{
  dsyslog("Serial remote control thread started (pid=%d)", getpid());

  int state = 0;
  int button = 0;
  int xbutton = 0;
  int dl = 0;
  char str[32];

  while(fd >= 0)
  {
    ioctl(fd, TIOCMGET, &state);
    xbutton = button;
    button = 0;
    if(state & TIOCM_CTS) button += 1;
    if(state & TIOCM_DSR) button += 2;
    if(state & TIOCM_RNG) button += 4;
    if(state & TIOCM_CAR) button += 8;

    if ((xbutton != button) && !((button == 0) || (xbutton == 0))) button = xbutton;

    if((xbutton != button) || ((dl >= SerialSetup.kpDelay) && (SerialSetup.kpDelay != MAXDELAY)))
      dl = 0;
    else {
      if (dl > MAXDELAY) dl = MAXDELAY; //um überlauf zu vermeiden
      dl++;
    }

    if((button > 0) && (dl == 0))
    {
      sprintf(str, "Button%d", button);
      Put(str);
    }
    SetState();
    usleep(100000);
  }
  dsyslog("Serial remote control thread ended (pid=%d)", getpid());
}

int cSerialRemote::Open(const char *device, cSerialStatus *stat)
{
  int fd = 0;
  int state = 0;
  cSSstat = stat;
  iCycle = 0;

  fd = open(device , O_RDONLY | O_NDELAY);
  if(fd >= 0)
  {
      /* Set DTR to high */
    state |= TIOCM_DTR;
    ioctl(fd, TIOCMSET, &state);
    Start();
    return fd;
  }
  return -1;
}

void cSerialRemote::Close(int fd)
{
  if(fd >= 0)
    close(fd);
}

class cPluginSerial : public cPlugin
{
private:
  // Add any member variables or functions you may need here.
  cSerialRemote *ser;
  cSerialStatus *stat;
public:
  cPluginSerial(void);
  virtual ~cPluginSerial();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return DESCRIPTION; }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Start(void);
  virtual void Housekeeping(void);
  virtual const char *MainMenuEntry(void) { return NULL; }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
};

cPluginSerial::cPluginSerial(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
}

cPluginSerial::~cPluginSerial()
{
  // Clean up after yourself!
}

const char *cPluginSerial::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginSerial::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginSerial::Start(void)
{
  // Start any background activities the plugin shall perform.
  stat = new cSerialStatus();
  ser  = new cSerialRemote(stat);
  return true;
}

void cPluginSerial::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

cOsdObject *cPluginSerial::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  return NULL;
}

cMenuSetupPage *cPluginSerial::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return new cMenuSetupSerial;
}

bool cPluginSerial::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  if (!strcasecmp(Name,"Flashrate"))
    SerialSetup.iFlash = atoi(Value);
  else if (!strcasecmp(Name,"Port"))
    SerialSetup.Port = atoi(Value);
  else if (!strcasecmp(Name,"KeyPressDelay"))
    SerialSetup.kpDelay = atoi(Value);
  else if (!strcasecmp(Name,"InstalledDevices"))
    SerialSetup.iInstDev = atoi(Value);
  else
    return false;

  return true;
}





VDRPLUGINCREATOR(cPluginSerial); // Don't touch this!
