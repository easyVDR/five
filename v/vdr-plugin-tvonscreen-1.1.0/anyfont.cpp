/*
 * anyfont.c: Internationalization
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include "anyfont.h"
#include "magazine.h"

anyFont::anyFont(cOsd *_osd,int fheight,int transparent)
{
    osd=_osd;
    Font = cFont::CreateFont(Setup.FontOsd, fheight);
    if (!Font || !Font->Height())
        Font = cFont::GetFont(fontSml);
    trans=transparent;
}

int anyFont::Height(void)
{
    return Font->Height();
}

int anyFont::Width(const char *txt)
{
    return Font->Width(txt);
}

int anyFont::Width(char c)
{
    return Font->Width(c);
}

int anyFont::Text(int x, int y, const char *txt, tColor fg, tColor bg)
{
    osd->DrawText(x, y, txt, fg, trans ? clrTransparent : bg, Font);
    return x += Font->Width(txt);
}

int anyFont::Text(int x, int y, int w, int h, const char *txt, tColor fg, tColor bg)
{
    if (txt==NULL)
        return x;
    char *memtxt=strdup(txt);
    char *mytxt=memtxt;
    char *t2=mytxt;
    char c;
    int x0=0;
    int y0=0;
//mzlog(0,"'%s'",t2);
    while (mytxt && *mytxt)
    {
        switch (*mytxt)
        {
        case ' ':
        case '\t':
        case '\n':
            c=*(mytxt);
            *(mytxt)=0;
            if (x0+Width(t2)>w && x0>0)
            {
                x0=0;
                y0+=Height();
            }
            if (y0<h*Height())
                x0+=Text(x+x0,y+y0,t2,fg,bg)   -x-x0+Width(c);
            *(mytxt)=c;
            t2=mytxt+1;
            if (c=='\t')
            {
                if (x0==0)
                {
                    x+=20;
                    w-=20;
                }
                else
                    x0=((x0+20)/20)*20;
            }
            else if (c=='\n')
            {
                x0=0;
                y0+=Height();
            }
            break;
        case '-':
        case '.':
            c=*(mytxt+1);
            *(mytxt+1)=0;
            if (x0+Width(t2)>w && x0>0)
            {
                x0=0;
                y0+=Height();
            }
            if (y0<h*Height())
                x0+=Text(x+x0,y+y0,t2,fg,bg)   -x-x0;
            *(mytxt+1)=c;
            t2=mytxt+1;
            break;
        }
        mytxt++;
    }
    if (t2!=mytxt && *t2!=0)
    {
        if (x0+Width(t2)>w && x0>0)
        {
            x0=0;
            y0+=Height();
        }
        if (y0<h*Height())
            x0+=Text(x+x0,y+y0,t2,fg,bg);
    }
    free(memtxt);
    return y0/Height()+1;
}

int anyFont::TextHeight(int w, const char *txt)
{
    if (txt==NULL)
        return 0;
    char *memtxt=strdup(txt);
    char *mytxt=memtxt;
    char *t2=mytxt;
    char c;
    int x0=0;
    int y0=0;
    while (mytxt && *mytxt)
    {
        switch (*mytxt)
        {
        case ' ':
        case '\t':
        case '\n':
            c=*(mytxt);
            *(mytxt)=0;
            if (x0+Width(t2)>w && x0>0)
            {
                x0=0;
                y0+=Height();
            }
            x0+=Width(t2)+Width(c);
            *(mytxt)=c;
            t2=mytxt+1;
            if (c=='\t')
            {
                if (x0==0)
                    w-=20;
                else
                    x0=((x0+20)/20)*20;
            }
            else if (c=='\n')
            {
                x0=0;
                y0+=Height();
            }
            break;
        case '-':
        case '.':
            c=*(mytxt+1);
            *(mytxt+1)=0;
            if (x0+Width(t2)>w && x0>0)
            {
                x0=0;
                y0+=Height();
            }
            x0+=Width(t2);
            *(mytxt+1)=c;
            t2=mytxt+1;
            break;
        }
        mytxt++;
    }
    if (t2!=mytxt && *t2!=0)
    {
        if (x0+Width(t2)>w && x0>0)
        {
            x0=0;
            y0+=Height();
        }
    }
    free(memtxt);
    return y0/Height()+1;
}