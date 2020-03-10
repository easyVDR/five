/*
 * Remote Control plugin for the Video Disk Recorder
 *
 * remote.h: input/lirc/tty remote control
 *
 * Copyright (C) 2002-2012 Oliver Endriss <o.endriss@gmx.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * Or, point your browser to http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 */


#ifndef __PLUGIN_REMOTE_H
#define __PLUGIN_REMOTE_H

#include <vdr/remote.h>
#include <vdr/thread.h>
#include "ttystatus.h"

//#define DEBUG

#ifdef DEBUG
#define DSYSLOG(x...)    dsyslog(x)
#else
#define DSYSLOG(x...)
#endif

#if VDRVERSNUM <= 10306
#define MSG_ERROR(x)	Interface->Error(x)
#define MSG_INFO(x)	Interface->Info(x)
#else
#define MSG_ERROR(x)	Skins.Message(mtError,x)
#define MSG_INFO(x)	Skins.Message(mtInfo,x)
#endif
				
// display error message with parameters on OSD
#define EOSD(fmt,parms...)     {  char msg[132]; \
                                  snprintf(msg, sizeof msg, fmt, parms); \
                                  MSG_ERROR(msg); \
                                  MSG_ERROR(msg); /* repeat once */ }

#if APIVERSNUM <= 10404
#define uint64_t	uint64
#endif



/*****************************************************************************/
class cRemoteGeneric : protected cRemote, protected cThread
/*****************************************************************************/
{
protected:
  static const uint64_t INVALID_KEY = (uint64_t) -1;
  int fh;
  char *device;
  int polldelay;
  int repeatdelay;
  int repeatfreq;
  int repeattimeout;
  cRemoteGeneric(const char *name, int f, char *d);
  virtual ~cRemoteGeneric();
  virtual uint64_t getKey(void) = 0;
  virtual bool keyPressed(uint64_t code) = 0;
  virtual bool Put(uint64_t Code, bool Repeat = false, bool Release = false);
  virtual void Action(void);
};



/*****************************************************************************/
class cRemoteDevInput : protected cRemoteGeneric
/*****************************************************************************/
{
private:
  bool testMode;
  uint64_t testKey;
  bool loadKeymap(const char *devname, uint32_t options);
protected:
  virtual uint64_t getKey(void);
  virtual bool keyPressed(uint64_t code);
public:
  cRemoteDevInput(const char *name, int f, char *d);
  virtual bool Initialize(void);
};



#ifdef REMOTE_FEATURE_LIRCOLD
/*****************************************************************************/
class cRemoteDevLirc : protected cRemoteGeneric
/*****************************************************************************/
{
protected:
  virtual uint64_t getKey(void);
  virtual bool keyPressed(uint64_t code);
public:
  cRemoteDevLirc(const char *name, int f, char *d)
    :cRemoteGeneric(name, f, d) { Start(); }
};
#endif



/*****************************************************************************/
class cRemoteDevTty : protected cRemoteGeneric
/*****************************************************************************/
{
private:
  struct termios tm;
protected:
  virtual uint64_t getKey(void);
  virtual bool keyPressed(uint64_t code);
  virtual bool Put(uint64_t Code, bool Repeat = false, bool Release = false);
public:
  cRemoteDevTty(const char *name, int f, char *d);
  virtual ~cRemoteDevTty(); 
};

/*****************************************************************************/
class cRemoteDevTtyWithOsd : protected cRemoteDevTty, protected cTtyStatus
/*****************************************************************************/
{
public:
  cRemoteDevTtyWithOsd(const char *name, int f, char *d)
    :cRemoteDevTty(name, f, d), cTtyStatus(f) {}
};

#endif // __PLUGIN_REMOTE_H
