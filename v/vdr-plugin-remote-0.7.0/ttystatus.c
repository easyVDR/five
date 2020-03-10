/*
 * Remote Control plugin for the Video Disk Recorder
 *
 * ttystatus.c: tty osd emulation
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


#include <vdr/plugin.h>
#include <vdr/status.h>
#include <wchar.h>
#include "ttystatus.h"


void cTtyStatus::dprintf(const char *Text)
{
    char *text = strdup(Text);

    if (text)
    {
        for (int i = 0; i < (int)strlen(text); i++)
        {
            switch(text[i])
            {
                case '\t':
                    text[i] = '|';
                    break;
            }
        }
        printf("%s: '%s'\n", __func__, text);
        free(text);
    }
}


void cTtyStatus::display(const char *buf, const bool fillEOL, const int limit)
{
    int n = strlen(buf);
    int pos = 0;
    int t = 0;
    mbstate_t ps;
    static const char blanks[] = "                    ";

    memset(&ps, 0, sizeof ps);

    for (int i = 0; i < n && pos < limit; )
    {
        switch (buf[i])
        {
            case '\t':
                do 
                {
                    write(fd, blanks, 1);
                    pos++;
                }
                while (t < numTabs && pos <= tabPos[t] + t+1);
                if (t < numTabs)
                    t++;
                i++;
                break;

            case '\n':
                write(fd, "\r\n", 2);
                i++;
                break;

            default:
#if 1
                // first calculate number of bytes, then perform a single 'write'
                const char *p = buf+i;
                int l = 0;
                while (*p != '\t' && *p != '\n' && pos < limit)
                {
                    int l2 = mbrlen(p, MB_CUR_MAX, &ps);
                    if (l2 == 0)
                        break;
                    if (l2 < 0)
                        l2 = 1;
                    p += l2;
                    l += l2;
                    pos++;
                }
#else
                // simple but suboptimal: one 'write' for each character
                int l = mbrlen(buf+i, MB_CUR_MAX, &ps);
                if (l < 0)
                    l = 1;
                pos++;
#endif
                write(fd, buf+i, l);
                i += l;
                break;
        }
    }

    while (fillEOL && pos < limit)
    {
        n = min(limit-pos, (int)sizeof(blanks)-1);
        write(fd, blanks, n);
        pos += n;
    }
}


void cTtyStatus::set_color(int col)
{
    switch(col)
    {
        case WHITE_BLACK:
            display("\e[1m\e[37m\e[40m");
	    break;

        case YELLOW_BLACK:
            display("\e[1m\e[33m\e[40m");
	    break;

        case CYAN_BLACK:
            display("\e[1m\e[36m\e[40m");
	    break;

        case BLACK_CYAN:
            display("\e[0m\e[30m\e[46m");
	    break;

        case BLACK_RED:
            display("\e[0m\e[30m\e[41m");
	    break;

        case BLACK_GREEN:
            display("\e[0m\e[30m\e[42m");
            break;

        case BLACK_YELLOW:
            display("\e[0m\e[30m\e[43m");
            break;

        case WHITE_BLUE:
            display("\e[1m\e[37m\e[44m");
            break;
    }
}		  


#if VDRVERSNUM <= 10337
void cTtyStatus::Recording(const cDevice *Device, const char *Name)
{
    const char *FileName = NULL;
    bool On = (Name != NULL);
#else
void cTtyStatus::Recording(const cDevice *Device,
                           const char *Name, const char *FileName, bool On)
{
#endif
    const char *startStop = On ? "start" : "stop";
    const char *s = Name;

    if (!s)
        s = FileName;
    if (!s)
        s = "...";

    set_pos(24, 0);
    set_color(BLACK_GREEN);
    print("  Card %d: %s recording '%s'%-60s",
          Device->CardIndex()+1, startStop, s, "");
    refresh();
    set_pos(2, 0);
}


#if VDRVERSNUM <= 10337
void cTtyStatus::Replaying(const cControl *Control, const char *Name)
{
    const char *FileName = NULL;
    bool On = (Name != NULL);
#else
void cTtyStatus::Replaying(const cControl *Control,
                           const char *Name, const char *FileName, bool On)
{
#endif
    const char *startStop = On ? "Start" : "Stop";
    const char *s = Name;

    if (!s)
        s = FileName;
    if (!s)
        s = "...";

    set_pos(24, 0);
    set_color(BLACK_GREEN);
    print("  %s replay '%s'%-70s", startStop, s, "");
    refresh();
    set_pos(2, 0);
}


void cTtyStatus::SetVolume(int Volume, bool Absolute)
{
    static int lastVolume;

    // dsyslog("%s: vol %d abs %d", __FUNCTION__, Volume, Absolute);

    set_pos(24, 0);
    set_color(BLACK_GREEN);
#if APIVERSNUM < 10402
    Absolute = true;
#endif
    if (Absolute)
        lastVolume = Volume;
    else
        lastVolume += Volume;
    print("     Set volume %d %-70s", lastVolume, "");
    refresh();
    set_pos(2, 0);
}


void cTtyStatus::OsdClear(void)
{
    clear_screen();
    refresh();
}


void cTtyStatus::OsdTitle(const char *Title)
{
    // dsyslog("%s: '%s'", __FUNCTION__, Title);

    clear_screen();
    set_color(BLACK_CYAN);
    set_pos(0, 0);
    display("", true, 5);
    display(Title, true, 75);
    set_color(WHITE_BLACK);
    refresh();
    set_pos(2, 0);
}


void cTtyStatus::OsdStatusMessage(const char *Message)
{
    set_pos(23,0);
    if (Message)
    {
        set_color(BLACK_YELLOW);
        display("", true, 5);
        display(Message, true, 75);
    }
    else
    {
        set_color(WHITE_BLACK);
        display("", true);
    }
    refresh();
    set_pos(2, 0);
}


void cTtyStatus::OsdHelpKeys(const char *Red, const char *Green,
                             const char *Yellow, const char *Blue)
{
    set_color(Red ? BLACK_RED : WHITE_BLACK);
    set_pos(24, 0);
    display("", true, 5);
    display(Red ? Red : "", true, 15);

    set_color(Green ? BLACK_GREEN: WHITE_BLACK);
    set_pos(24, 20);
    display("", true, 5);
    display(Green ? Green : "", true, 15);

    set_color(Yellow ? BLACK_YELLOW : WHITE_BLACK);
    set_pos(24, 40);
    display("", true, 5);
    display(Yellow ? Yellow : "", true, 15);

    set_color(Blue ? WHITE_BLUE : WHITE_BLACK);
    set_pos(24, 60);
    display("", true, 5);
    display(Blue ? Blue : "", true, 15);

    refresh();
    set_pos(2, 0);
}


void cTtyStatus::OsdItem(const char *Text, int Index)
{
    // dsyslog("%s[%4d]: '%s'", __FUNCTION__, Index, Text);

    if (numEntries <= Index)
    {
        int oldNumEntries = numEntries;
        numEntries = Index + 100;
        lineBuf = (char **) realloc((void *)lineBuf, numEntries * sizeof(char *));
        if (lineBuf == NULL)
        {
            numEntries = 0;
            return;
        }
        for (int i = oldNumEntries; i < numEntries; i++)
            lineBuf[i] = NULL;
    }

    if (Text && 0 <= Index && Index < numEntries)
    {
        if (lineBuf[Index] != NULL)
            free(lineBuf[Index]);
        lineBuf[Index] = strdup(Text);
        lastIndex = Index;
    }
}


void cTtyStatus::OsdCurrentItem(const char *Text)
{
    int i;
    int first, last;
  
    // dsyslog("%s: '%s'", __FUNCTION__, Text);

    if (numEntries > 0 && Text)
    {
        for (i = 0; i <= lastIndex; i++)
        {
            if (lineBuf[i] == NULL)
                continue;
            if (strcmp(Text, lineBuf[i]) == 0)
                break;
        }

        if (i <= lastIndex)
        {
            // found
            currIndex = i;
        }
        else
        {
            // not found, item changed
            if (lineBuf[currIndex] != NULL)
                free(lineBuf[currIndex]);
            lineBuf[currIndex] = strdup(Text);
        }

        first = min(currIndex-10,lastIndex-20);
        first = max(first,0);
        last = min(first+20,lastIndex);

        // Update tab positions in window
        memset(tabPos, 0, sizeof tabPos);
        numTabs = 0;
        for (i = first; i <= last; i++)
        {
            int n = strlen(lineBuf[i]);
            int t = 0;
            for (int j = 0; j < n; j++)
            {
                if (lineBuf[i][j] == '\t' && t < maxTabs)
                {
                    tabPos[t] = max(tabPos[t], j);
                    t++;
                }
            }
            numTabs = max(numTabs, t);
        }
#if 0
	for (i = 0; i < numTabs; i++)
	    printf("tab %d at position %d\n", i, tabPos[i]);
#endif

        set_color(WHITE_BLACK);
        for (i = first; i <= last; i++)
        {
            if (i == currIndex)
                set_color(BLACK_CYAN);
            set_pos(2+i-first, 0);
            display(lineBuf[i], true);
            if (i == currIndex)
                set_color(WHITE_BLACK);
        }
        set_color(WHITE_BLACK);
        refresh();
    }
}


//
// Note: Used for event decription, recording info etc.
//
void cTtyStatus::OsdTextItem(const char *Text, bool Scroll)
{
    // dsyslog("%s: '%s' scroll %d", __FUNCTION__, Text, Scroll);

    if (Text)
    {
        set_color(CYAN_BLACK);
        display(Text, false, 0x7fff);
        display("\n\n");
        refresh();
    }
    else
    {
        set_pos(2, 0);
    }
}


void cTtyStatus::OsdChannel(const char *Text)
{
    clear_screen();
    set_color(WHITE_BLACK);
    set_pos(19, 0);
    display(Text);
    refresh();
}


void cTtyStatus::OsdProgramme(time_t PresentTime, const char *PresentTitle,
                      const char *PresentSubtitle, time_t FollowingTime,
                      const char *FollowingTitle, const char *FollowingSubtitle)
{
    char buffer[25];
    struct tm tm_r;
    int line = 20;

    strftime(buffer, sizeof(buffer), "%R", localtime_r(&PresentTime, &tm_r));
    if (PresentTitle)
    {
        set_color(YELLOW_BLACK);
        set_pos(line++, 0);
        display(buffer, true, 6);
        set_color(CYAN_BLACK);
        display(PresentTitle);
    }
    if (PresentSubtitle)
    {
        if (strlen(PresentSubtitle))
        {
            set_pos(line++, 6);
            display(PresentSubtitle);
        }
    }

    strftime(buffer, sizeof(buffer), "%R", localtime_r(&FollowingTime, &tm_r));
    if (FollowingTitle)
    {
        set_color(YELLOW_BLACK);
        set_pos(line++, 0);
        display(buffer, true, 6);
        set_color(CYAN_BLACK);
        display(FollowingTitle);
    }
    if (FollowingSubtitle)
    {
        set_pos(line++, 6);
        display(FollowingSubtitle);
    }

    refresh();
}


cTtyStatus::cTtyStatus(int f)
{
    // initialize variables
    lineBuf = NULL;
    numEntries = 0;
    lastIndex = 0;
    currIndex = 0;
    fd = f;
}


cTtyStatus::~cTtyStatus(void)
{
    if (lineBuf && numEntries > 0)
    {
        for (int i = 0; i < numEntries; i++)
        {
            if (lineBuf[i])
                free(lineBuf[i]);
        }
        free(lineBuf);
    }
}
