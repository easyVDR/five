#ifndef __VIEWELEMENTSDT_H
#define __VIEWELEMENTSDT_H

#include <vdr/menu.h>
#include "viewelement.h"

/******************************************************************
* cVeDtBackground
******************************************************************/
class cVeDtBackground : public cViewElement {
private:
    int numTracks;
public:
    cVeDtBackground(void);
    virtual ~cVeDtBackground(void);
    void SetTokenContainer(void);
    void Set(int numTracks);
    bool Parse(bool forced = false);
};

/******************************************************************
* cVeDtHeader
******************************************************************/
class cVeDtHeader : public cViewElement {
private:
    char *title;
    int audioChannel;
    int numTracks;
    bool changed;
public:
    cVeDtHeader(void);
    virtual ~cVeDtHeader(void);
    void SetTokenContainer(void);
    void SetTitle(const char *title);
    void SetNumtracks(int numTracks);
    void SetAudiochannel(int audioChannel);
    bool Parse(bool forced = false);
};

#endif //__VIEWELEMENTSDT_H