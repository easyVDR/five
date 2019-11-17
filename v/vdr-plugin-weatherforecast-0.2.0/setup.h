#ifndef __WEATHERFORECAST_SETUP_H
#define __WEATHERFORECAST_SETUP_H

#include "config.h"

extern cWeatherforecastConfig weatherConfig;
using namespace std;

class cWeatherforecastSetup : public cMenuSetupPage {
    public:
        cWeatherforecastSetup(void);
        virtual ~cWeatherforecastSetup();
        char city[100];
        char lat[10];
        char lon[10];
        char userApiKey[100];
        int userHoursToUpdate;
    private:
        cWeatherforecastConfig data;
        void Setup(void);
        virtual eOSState ProcessKey(eKeys Key);
        virtual void Store(void);
};
#endif //__WEATHERFORECAST_SETUP_H
