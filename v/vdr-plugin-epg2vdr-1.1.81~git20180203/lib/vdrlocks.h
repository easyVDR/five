/*
 * vdrlocks.h
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __VDRLOCKS_H
#  define __VDRLOCKS_H

#  ifdef VDR_PLUGIN

#  include <vdr/config.h>
#  include <vdr/tools.h>

/*
 * It must always be done in the sequence:
 *    Timers -> Channels -> Recordings -> Schedules
*/

//***************************************************************************
// Timer List Lock Macros
//***************************************************************************

#if defined (APIVERSNUM) && (APIVERSNUM >= 20301)
#  define GET_TIMERS_READ(name) USE_LIST_LOCK_READ(Timers);  \
   const cTimers* name = Timers;
#else
#  define GET_TIMERS_READ(name) cTimers* name = &Timers;
#endif

#if defined (APIVERSNUM) && (APIVERSNUM >= 20301)
#  define GET_TIMERS_WRITE(name) USE_LIST_LOCK_WRITE(Timers);   \
   cTimers* name = Timers;
#else
#  define GET_TIMERS_WRITE(name) cTimers* name = &Timers;
#endif

//***************************************************************************
// Channel List Lock Macros
//***************************************************************************

#if defined (APIVERSNUM) && (APIVERSNUM >= 20301)
#  define GET_CHANNELS_READ(name) USE_LIST_LOCK_READ(Channels);  \
   const cChannels* name = Channels;
#else
#  define GET_CHANNELS_READ(name) cChannels* name = &Channels;
#endif

#if defined (APIVERSNUM) && (APIVERSNUM >= 20301)
#  define GET_CHANNELS_WRITE(name) USE_LIST_LOCK_WRITE(Channels);   \
   cChannels* name = Channels;
#else
#  define GET_CHANNELS_WRITE(name) cChannels* name = &Channels;
#endif

//***************************************************************************
// Recording List Lock Macros
//***************************************************************************

#if defined (APIVERSNUM) && (APIVERSNUM >= 20301)
#  define GET_RECORDINGS_READ(name) USE_LIST_LOCK_READ(Recordings);  \
   const cRecordings* name = Recordings;
#else
#  define GET_RECORDINGS_READ(name) cRecordings* name = &Recordings;
#endif

#if defined (APIVERSNUM) && (APIVERSNUM >= 20301)
#  define GET_RECORDINGS_WRITE(name) USE_LIST_LOCK_WRITE(Recordings);   \
   cRecordings* name = Recordings;
#else
#  define GET_RECORDINGS_WRITE(name) cRecordings* name = &Recordings;
#endif

//***************************************************************************
#  endif // VDR_PLUGIN

#endif //___VDRLOCKS_H
