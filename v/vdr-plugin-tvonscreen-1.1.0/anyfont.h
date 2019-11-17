/*
 * anyfont.h: Internationalization
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef ANYFONT_H
#define ANYFONT_H

#include <vdr/plugin.h>

class anyFont
{
    cOsd *osd;
    const cFont *Font;
    int trans;

public:
    anyFont(cOsd *o,int fheight,int transparent=0);
    int Height(void);
    int Width(const char *txt);
    int Width(char c);
    int Text(int x, int y, const char *txt, tColor fg, tColor bg=clrTransparent);
    int Text(int x, int y, int w, int h, const char *txt, tColor fg, tColor bg=clrTransparent);
    int TextHeight(int w, const char *txt);
};

#endif
