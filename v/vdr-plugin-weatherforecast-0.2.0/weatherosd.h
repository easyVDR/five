#ifndef __WEATHEROSD_H
#define __WEATHEROSD_H

#include <vdr/osdbase.h>
#include <string>
#include <libskindesignerapi/skindesignerapi.h>
#include <libskindesignerapi/skindesignerosdbase.h>
#include "libforecastio/forecastio.h"
#include "definitions.h"

class cWeatherOsd : public skindesignerapi::cSkindesignerOsdMenu {
private:
    bool isDetailedView;
    int lastRootMenuElement;
    cForecastIO *forecastIO;
    void SetRootMenu(void);
    void SetDetailViewCurrent(void);
    void SetDetailViewHourly(void);
    void SetDetailViewDaily(void);
public:
    cWeatherOsd(cForecastIO *forecastIO, skindesignerapi::cPluginStructure *plugStruct);
    virtual ~cWeatherOsd();
    virtual eOSState ProcessKey(eKeys key);
    static void DefineTokens(eMenus menu, skindesignerapi::cTokenContainer *tk);
};

#endif //__WEATHEROSD_H