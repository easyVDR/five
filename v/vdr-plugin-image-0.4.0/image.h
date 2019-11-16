/*
 * Image plugin to VDR (C++)
 *
 * (C) 2004-2011 Andreas Brachold    <vdr07 at deltab.de>
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 *
 */

#ifndef ___IMAGE_H
#define ___IMAGE_H

#include <vdr/config.h>
#include <vdr/osdbase.h>
#include <vdr/skins.h>
#include <string.h>

#ifdef APIVERSNUM
#if APIVERSNUM < 10347
    #error "For compiled are at the least VDR Plugin-API 1.3.47 required"
#endif
#endif

inline void OSD_InfoMsg(const char* sz) 
{	
  Skins.Message(mtInfo,sz);
  Skins.Flush();
}

inline void OSD_ErrorMsg(const char* sz) 
{	
  Skins.Message(mtError,sz);
  Skins.Flush();
}

inline void OSD_ErrorNumMsg(int err, const char* szDef) 
{	
    char szErr[128];
    int nErr = err;
    szErr[sizeof(szErr)-1] = '\0';
    if(0 != strerror_r(nErr,szErr,sizeof(szErr)-1)) {
        szErr[0] = '\0';
    } 
    OSD_ErrorMsg(szErr[0] != '\0'?szErr:szDef);
}

#endif				//___IMAGE_H
