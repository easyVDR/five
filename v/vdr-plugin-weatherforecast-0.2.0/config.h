#ifndef __WEATHERFORECAST_CONFIG_H
#define __WEATHERFORECAST_CONFIG_H

#include <string>
#include <vdr/plugin.h>

using namespace std;

class cWeatherforecastConfig {
private:
public:
    cWeatherforecastConfig();
    ~cWeatherforecastConfig();
    bool SetupParse(const char *Name, const char *Value);
    int hoursToUpdate;
    int userHoursToUpdate;
    string city;
    float lat;
    float lon;
    string userApiKey;
};
#endif //__WEATHERFORECAST_CONFIG_H
