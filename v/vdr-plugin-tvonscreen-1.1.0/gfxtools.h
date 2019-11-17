/*
 * gfxtools.h
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef GFXTOOLS_H
#define GFXTOOLS_H

#include <vdr/plugin.h>
#include "anyfont.h"

typedef cOsd areaT;
typedef int winhandleT;

bool DrawXpm(const char *FileName,areaT *drawable,int x0,int y0,winhandleT winhand=0,bool blackwhite=false);
bool DrawXpm(char *Xpm[], areaT *drawable,int x0,int y0,winhandleT winhand=0,bool blackwhite=false);

#endif
