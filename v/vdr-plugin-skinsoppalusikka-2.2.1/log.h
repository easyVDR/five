/*
 * log.h: 'Soppalusikka' skin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __SOPPALUSIKKA_LOG_H
#define __SOPPALUSIKKA_LOG_H

#include "config.h"

#define error(x...)   esyslog("SOPPALUSIKKA-ERROR: " x)
#define info(x...)    isyslog("SOPPALUSIKKA: " x)
// 0x0001: Generic call stack
#define debug1(x...)  void( SoppalusikkaConfig.IsTraceMode(cSoppalusikkaConfig::eTraceModeDebug1)  ? dsyslog("SOPPALUSIKKA1: " x)  : void() )
// 0x0002: Logo cache
#define debug2(x...)  void( SoppalusikkaConfig.IsTraceMode(cSoppalusikkaConfig::eTraceModeDebug2)  ? dsyslog("SOPPALUSIKKA2: " x)  : void() )
// 0x0004: TBD
#define debug3(x...)  void( SoppalusikkaConfig.IsTraceMode(cSoppalusikkaConfig::eTraceModeDebug3)  ? dsyslog("SOPPALUSIKKA3: " x)  : void() )
// 0x0008: TBD
#define debug4(x...)  void( SoppalusikkaConfig.IsTraceMode(cSoppalusikkaConfig::eTraceModeDebug4)  ? dsyslog("SOPPALUSIKKA4: " x)  : void() )
// 0x0010: TBD
#define debug5(x...)  void( SoppalusikkaConfig.IsTraceMode(cSoppalusikkaConfig::eTraceModeDebug5)  ? dsyslog("SOPPALUSIKKA5: " x)  : void() )
// 0x0020: TBD
#define debug6(x...)  void( SoppalusikkaConfig.IsTraceMode(cSoppalusikkaConfig::eTraceModeDebug6)  ? dsyslog("SOPPALUSIKKA6: " x)  : void() )
// 0x0040: TBD
#define debug7(x...)  void( SoppalusikkaConfig.IsTraceMode(cSoppalusikkaConfig::eTraceModeDebug7)  ? dsyslog("SOPPALUSIKKA7: " x)  : void() )
// 0x0080: TBD
#define debug8(x...)  void( SoppalusikkaConfig.IsTraceMode(cSoppalusikkaConfig::eTraceModeDebug8)  ? dsyslog("SOPPALUSIKKA8: " x)  : void() )
// 0x0100: TBD
#define debug9(x...)  void( SoppalusikkaConfig.IsTraceMode(cSoppalusikkaConfig::eTraceModeDebug9)  ? dsyslog("SOPPALUSIKKA9: " x)  : void() )
// 0x0200: TBD
#define debug10(x...) void( SoppalusikkaConfig.IsTraceMode(cSoppalusikkaConfig::eTraceModeDebug10) ? dsyslog("SOPPALUSIKKA10: " x) : void() )
// 0x0400: TBD
#define debug11(x...) void( SoppalusikkaConfig.IsTraceMode(cSoppalusikkaConfig::eTraceModeDebug11) ? dsyslog("SOPPALUSIKKA11: " x) : void() )
// 0x0800: TBD
#define debug12(x...) void( SoppalusikkaConfig.IsTraceMode(cSoppalusikkaConfig::eTraceModeDebug12) ? dsyslog("SOPPALUSIKKA12: " x) : void() )
// 0x1000: TBD
#define debug13(x...) void( SoppalusikkaConfig.IsTraceMode(cSoppalusikkaConfig::eTraceModeDebug13) ? dsyslog("SOPPALUSIKKA13: " x) : void() )
// 0x2000: TBD
#define debug14(x...) void( SoppalusikkaConfig.IsTraceMode(cSoppalusikkaConfig::eTraceModeDebug14) ? dsyslog("SOPPALUSIKKA14: " x) : void() )
// 0x4000: TBD
#define debug15(x...) void( SoppalusikkaConfig.IsTraceMode(cSoppalusikkaConfig::eTraceModeDebug15) ? dsyslog("SOPPALUSIKKA15: " x) : void() )
// 0x8000; Extra call stack
#define debug16(x...) void( SoppalusikkaConfig.IsTraceMode(cSoppalusikkaConfig::eTraceModeDebug16) ? dsyslog("SOPPALUSIKKA16: " x) : void() )

#endif // __SOPPALUSIKKA_LOG_H
