/*
 * section_lock.h:
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 *
 */

#ifndef __SECTION_LOCK_H
#define __SECTION_LOCK_H

#include <vdr/thread.h>

class cSectionLock {
  private:
    cMutex& mutex;
  public:
    cSectionLock(cMutex& Mutex) : mutex(Mutex) { mutex.Lock(); };
    ~cSectionLock() { mutex.Unlock(); };
};

#endif // __SECTION_LOCK_H
