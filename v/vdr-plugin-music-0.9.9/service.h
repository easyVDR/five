/*
 * MP3/MPlayer plugin to VDR (C++)
 *
 * (C) 2001-2006 Stefan Huelswitt <s.huelswitt@gmx.de>
 *
 * This code is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * Or, point your browser to http://www.gnu.org/copyleft/gpl.html
 */

#ifndef ___SERVICE_H
#define ___SERVICE_H

// GraphTFT stuff
struct MusicService_Covername_1_0 {
 const char *name;
};

struct MusicServicePlaylist_1_0
{
   int index;
   int count;
   const char* item;
};

struct MusicServicePlayerInfo_1_0
{
   const char* filename;
   const char* artist;
   const char* album;
   const char* genre;
   const char* comment;
   int year;
   double frequence;
   int bitrate;
   const char* smode;
   int index;           // current index in tracklist
   int count;           // total items in tracklist
   const char* status;  // player status  
   const char* currentTrack;
   bool loop;
   bool shuffle;
   bool shutdown;
   bool recording;
   int rating;   
};

struct MusicServiceHelpButtons_1_0
{
   const char* red;
   const char* green;
   const char* yellow;
   const char* blue;
};

struct MusicServiceInfo_1_0
{
   const char* info;
};
// GraphTFT stuff ends here


struct MP3ServiceData {
  int result;
  union {
    const char *filename;
    } data;
  };

struct MPlayerServiceData {
  int result;
  union {
    const char *filename;
    } data;
  };

#endif //___SERVICE_H
