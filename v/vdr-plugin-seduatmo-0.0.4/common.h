/*
 * common.h: EPG2VDR plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __COMMON_H
#define __COMMON_H

//***************************************************************************
// 
//***************************************************************************

enum Misc
{
   success = 0,
   done    = success,
   fail    = -1,
   ignore  = -2,
   na      = -1,
   yes     = 1,
   on      = 1,
   off     = 0,
   no      = 0,
   TB      = 1
};

//***************************************************************************
// Misc ..
//***************************************************************************

int minMax(int x, int min, int max);
int getrand(int min, int max);
double min(double a, double b);
double max(double a, double b);

//***************************************************************************
// Time
//***************************************************************************

typedef unsigned long long MsTime;

MsTime msNow();

//***************************************************************************
// Tell
//***************************************************************************

void tell(int eloquence, const char* format, ...);
int error(const char* format, ...);

//***************************************************************************
#endif //___COMMON_H
