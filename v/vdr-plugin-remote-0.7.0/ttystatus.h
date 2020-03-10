/*
 * Remote Control plugin for the Video Disk Recorder
 *
 * ttystatus.h: tty osd emulation
 *
 * Copyright (C) 2002-2015 Oliver Endriss <o.endriss@gmx.de>
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


#ifndef __PLUGIN_TTYSTATUS_H
#define __PLUGIN_TTYSTATUS_H


#define WHITE_BLACK	1
#define YELLOW_BLACK	2
#define CYAN_BLACK	3
#define BLACK_CYAN	4
#define BLACK_RED	5
#define BLACK_GREEN	6
#define BLACK_YELLOW	7
#define WHITE_BLUE	8


#include <vdr/status.h>


#define clear_screen()   { set_color(WHITE_BLACK); display("\e[2J"); }
#define refresh()        
#define set_pos(y,x)     print("\e[%d;%dH", y+1, x+1);
#define print(fmt...)    { char buf[140]; snprintf(buf,sizeof buf,fmt); display(buf); }
  

/****************************************************************************/
class cTtyStatus : public cStatus {
/****************************************************************************/
private:
  int fd;
  char **lineBuf;
  int numEntries;
  int lastIndex;
  int currIndex;
  static const int maxTabs = 10;
  int numTabs;
  int tabPos[maxTabs];

protected:
  void dprintf(const char *Text);
  virtual void display(const char *buf, const bool fillEOL = false, const int limit = 80);
  virtual void set_color(int col);
#if VDRVERSNUM <= 10337
  virtual void Replaying(const cControl *Control, const char *Name);
  virtual void Recording(const cDevice *Device, const char *Name);
#else
  virtual void Recording(const cDevice *Device,
                         const char *Name, const char *FileName, bool On);
  virtual void Replaying(const cControl *Control,
                         const char *Name, const char *FileName, bool On);
#endif
  virtual void SetVolume(int Volume, bool Absolute);
  virtual void OsdClear(void);
  virtual void OsdTitle(const char *Title);
  virtual void OsdStatusMessage(const char *Message);
  virtual void OsdHelpKeys(const char *Red, const char *Green,
                           const char *Yellow, const char *Blue);
  virtual void OsdItem(const char *Text, int Index);
  virtual void OsdCurrentItem(const char *Text);
  virtual void OsdTextItem(const char *Text, bool Scroll);
  virtual void OsdChannel(const char *Text);
  virtual void OsdProgramme(time_t PresentTime, const char *PresentTitle,
                            const char *PresentSubtitle,
                            time_t FollowingTime, const char *FollowingTitle,
                            const char *FollowingSubtitle);

public:
  cTtyStatus(int fd);
  virtual ~cTtyStatus();
};


#endif // __PLUGIN_TTYSTATUS_H
