#include "config.h"

cWeatherforecastConfig::cWeatherforecastConfig() {
    //number of hours to wait till next update from forecast.io is fetched 
    hoursToUpdate = 20;
    //if own API Key is set, user can configure its own udate frequence
    userHoursToUpdate = 0;
    //city to display in menus
    city = "";
    //latitude and longitude of location for forecast.io query
    lat = 0.0;
    lon = 0.0;
    //User can configure individual API Key
    userApiKey = "";
}

cWeatherforecastConfig::~cWeatherforecastConfig() {
}

bool cWeatherforecastConfig::SetupParse(const char *Name, const char *Value) {
    if      (!strcasecmp(Name, "city"))           city = Value;
    else if (!strcasecmp(Name, "lat"))            lat = atod(Value);
    else if (!strcasecmp(Name, "lon"))            lon = atod(Value);
    else if (!strcasecmp(Name, "userapikey"))     userApiKey = Value;
    else if (!strcasecmp(Name, "updatefreq"))     userHoursToUpdate = atoi(Value);
    else return false;
    return true;
}
