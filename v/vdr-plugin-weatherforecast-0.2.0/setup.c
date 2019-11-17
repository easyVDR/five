#include "setup.h"
#include "tools/jsonhelpers.h"

cWeatherforecastSetup::cWeatherforecastSetup() {
    strn0cpy(city, weatherConfig.city.c_str(), sizeof(city));
    string strLat = FloatToString(weatherConfig.lat);
    string strLon = FloatToString(weatherConfig.lon);
    strn0cpy(lat, strLat.c_str(), sizeof(lat));
    strn0cpy(lon, strLon.c_str(), sizeof(lon));
    strn0cpy(userApiKey, weatherConfig.userApiKey.c_str(), sizeof(userApiKey));
    userHoursToUpdate = weatherConfig.userHoursToUpdate;
    Setup();
}

cWeatherforecastSetup::~cWeatherforecastSetup() {
}


void cWeatherforecastSetup::Setup(void) {
    int current = Current();
    Clear();

    Add(new cMenuEditStrItem(tr("City"), city, sizeof(city), trVDR(FileNameChars)));
    Add(new cMenuEditStrItem(tr("Latitude"), lat, sizeof(lat), trVDR(FileNameChars)));
    Add(new cMenuEditStrItem(tr("Longitude"), lon, sizeof(lon), trVDR(FileNameChars)));
    Add(new cMenuEditStrItem(tr("API Key"), userApiKey, sizeof(userApiKey), trVDR(FileNameChars)));

    if (strlen(userApiKey) > 0) {
        Add(new cMenuEditIntItem(tr("Update Frequency in hours"), &userHoursToUpdate, 1, 24));
    }

    SetCurrent(Get(current));
    Display();
}

eOSState cWeatherforecastSetup::ProcessKey(eKeys Key) {
    eOSState state = cMenuSetupPage::ProcessKey(Key);
    return state;
}

void cWeatherforecastSetup::Store(void) {
    SetupStore("city", city);
    SetupStore("lat", lat);
    SetupStore("lon", lon);
    SetupStore("userapikey", userApiKey);
    SetupStore("updatefreq", userHoursToUpdate);

    weatherConfig.city = city;
    weatherConfig.lat = atod(lat);
    weatherConfig.lon = atod(lon);
    weatherConfig.userApiKey = userApiKey;
    weatherConfig.userHoursToUpdate = userHoursToUpdate;
}