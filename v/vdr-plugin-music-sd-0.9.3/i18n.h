/*
 * i18n.h: 'Music-SD' plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __MUSIC-SD_I18N_H
#define __MUSIC-SD_I18N_H

#include "common.h"

#include <vdr/i18n.h>

#if VDRVERSNUM < 10507
extern const tI18nPhrase Phrases[];
#define trNOOP(s) (s)
#endif

#if APIVERSNUM < 10509
#define trVDR(s) tr(s)
#endif

#endif // __MUSIC-SD_I18N_H
// vim:et:sw=2:ts=2:
