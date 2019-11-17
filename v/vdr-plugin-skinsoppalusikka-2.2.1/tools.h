/*
 * tools.h: The 'soppalusikka' VDR skin
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __SKINSOPPALUSIKKA_TOOLS_H
#define __SKINSOPPALUSIKKA_TOOLS_H

#include <vdr/tools.h>

char *strcatrealloc(char *destP, const char *srcP);
char *strncatrealloc(char *destP, const char *srcP, size_t lenP);
char *parseaux(char *strP);
bool ischaracters(const char *strP, const char *maskP);
bool ischaracter(const char chP, const char *maskP);

#endif // __SKINSOPPALUSIKKA_TOOLS_H
