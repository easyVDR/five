#ifndef __VIEWELEMENTSCOMMON_H
#define __VIEWELEMENTSCOMMON_H

#include <vdr/menu.h>
#include "viewelement.h"

/******************************************************************
* cVeDateTime
******************************************************************/
class cVeDateTime : public cViewElement {
private:
    int lastMinute;
public:
    cVeDateTime(void);
    virtual ~cVeDateTime(void);
    void Close(void);
    void SetTokenContainer(void);
    bool Parse(bool forced = false);
};
/******************************************************************
* cVeTime
******************************************************************/
class cVeTime : public cViewElement {
private:
    int lastSecond;
public:
    cVeTime(void);
    virtual ~cVeTime(void);
    void Close(void);
    void SetTokenContainer(void);
    bool Parse(bool forced = false);
};
/******************************************************************
* cVeMessage
******************************************************************/
class cVeMessage : public cViewElement {
private:
    bool changed;
    eMessageType type;
    char *text;
public:
    cVeMessage(void);
    virtual ~cVeMessage(void);
    void SetTokenContainer(void);
    void Set(eMessageType type, const char *text);
    bool Parse(bool forced = false);
};
/******************************************************************
* cVeDevices
******************************************************************/
class cVeDevices : public cViewElement {
private:
    bool light;
    time_t lastRefresh;
    vector<int> devices;
    bool initial;
    int devicesIndex;
    cMutex mutexDevices;
    int numDevices;
    int* lastSignalStrength;
    int* lastSignalQuality;
    bool* recDevices;
    void Init(void);
public:
    cVeDevices(void);
    virtual ~cVeDevices(void);
    void Close(void);
    void SetTokenContainer(void);
    bool Parse(bool forced = false);
};
/******************************************************************
* cVeCurrentWeather
******************************************************************/
class cVeCurrentWeather : public cViewElement {
private:
public:
    cVeCurrentWeather(void);
    virtual ~cVeCurrentWeather(void);
    void SetTokenContainer(void);
    bool Parse(bool forced = false);
};
/******************************************************************
* cVeCustomTokens
******************************************************************/
class cVeCustomTokens : public cViewElement {
private:
public:
    cVeCustomTokens(void);
    virtual ~cVeCustomTokens(void);
    void Reset(void);
    void SetTokenContainer(void);
    bool Parse(bool forced = false);
};
/******************************************************************
* cVeVolume
******************************************************************/
class cVeVolume : public cViewElement {
private:
    int current;
    int total;
    bool mute;
    bool changed;
public:
    cVeVolume(void);
    virtual ~cVeVolume(void);
    void SetTokenContainer(void);
    void Set(int current, int total, bool mute);
    bool Parse(bool forced = false);
};
#endif //__VIEWELEMENTSCOMMON_H