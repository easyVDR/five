/*
 * general_remote.h:
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 *
 */

#ifndef __GENERAL_REMOTE_H
#define __GENERAL_REMOTE_H


//----------------------------- cGeneralRemote --------------------------------

#include <vdr/remote.h>

class cGeneralRemote : public cRemote {
 public:
  cGeneralRemote(const char *Name) : cRemote(Name) {};
  bool Put(const char *Code, bool Repeat=false, bool Release=false) 
    { return cRemote::Put(Code, Repeat, Release); };
};


#endif
