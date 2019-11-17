#include <vdr/plugin.h>
#include <string>
#include <sstream>
#include <jansson.h>
#include "../config.h"
#include "../tools/curlfuncs.h"
#include "../tools/filesystem.h"
#include "forecastio.h"

using namespace std;
extern cWeatherforecastConfig weatherConfig;

cForecastIO::cForecastIO(string cacheDir) {
    forecastFile = *cString::sprintf("%s/%s", cacheDir.c_str(), "weather.json");
    cacheDurationDefault = 60 * 60 * weatherConfig.hoursToUpdate;
    apiKey = "9830052ef63efbec84ec0639e9a205d2";
    string osdLanguage = Setup.OSDLanguage;
    language = osdLanguage.substr(0,2);
    unit = "si";
    baseURL = "https://api.forecast.io/forecast";
    latitude = weatherConfig.lat;
    longitude = weatherConfig.lon;
    current = NULL;
    hourly = NULL;
    daily = NULL;
}

cForecastIO::~cForecastIO() {
    Clear();
}

/*****************************************************************
* PUBLIC FUNCTIONS
*****************************************************************/
void cForecastIO::Action(void) {
    loopActive = true;
    ReadForecastInitial();
    waitMutex.Lock();
    while (loopActive && Running()) {
        waitCondition.TimedWait(waitMutex, 1000 * 60);
        if (!loopActive)
            return;
        if (!CacheFileValid()) {
            ReadForecast();
        } 
    }
}

void cForecastIO::Stop() { 
    loopActive = false;
    waitCondition.Broadcast();
    Cancel(1);
    while (Active())
        cCondWait::SleepMs(10);
}

cForecast *cForecastIO::GetCurrentForecast(void) {
    time_t now = time(0);
    if (current && current->TimeMatch(now)) {
        return current;
    }
    if (hourly) {
        return hourly->GetCurrent();
    }
    return NULL;
}

bool cForecastIO::SetCurrentWeather(cServiceCurrentWeather *call) {
    cForecast *currentForecast = GetCurrentForecast();
    if (!currentForecast)
        return false;

    call->timeStamp = currentForecast->GetDateTimeString();
    call->temperature = currentForecast->GetTemperatureString();
    call->apparentTemperature = currentForecast->GetApparentTemperatureString();
    call->summary = currentForecast->GetSummary();
    call->icon = currentForecast->GetIcon();
    call->precipitationIntensity = currentForecast->GetPrecipIntensityString();
    call->precipitationProbability = currentForecast->GetPrecipProbabilityPercent();
    call->precipitationType = currentForecast->GetPercipType();
    call->humidity = currentForecast->GetHumidityPercent();
    call->windSpeed = currentForecast->GetWindSpeedString();
    call->windBearing = currentForecast->GetWindBearing();
    call->windBearingString = currentForecast->GetWindBearingString();
    call->visibility = currentForecast->GetVisibilityString();
    call->cloudCover = currentForecast->GetCloudCoverPercent();
    call->pressure = currentForecast->GetPressureString();
    call->ozone = currentForecast->GetOzoneString();

    cForecast *dailyForecast = daily->GetFirstDaily();
    if (!dailyForecast)
        return true;

    call->minTemperature = dailyForecast->GetTemperatureMinString();
    call->maxTemperature = dailyForecast->GetTemperatureMaxString();

    return true;
}

/*****************************************************************
* PRIVATE FUNCTIONS
*****************************************************************/

void cForecastIO::Clear(void) {
    if (current) {
        delete current;
        current = NULL;
    }
    if (hourly) {
        delete hourly;
        hourly = NULL;
    }
    if (daily) {
        delete daily;
        daily = NULL;
    }
}

bool cForecastIO::CheckUserApiKey(void) {
    if (weatherConfig.userApiKey.size() == 0)
        return false;
    if (weatherConfig.userApiKey.compare(apiKey) == 0) {
        return false;
    }
    return true;
}

int cForecastIO::CalculateCachDuration(void) {
    if (!CheckUserApiKey())
        return cacheDurationDefault;
    if (weatherConfig.userHoursToUpdate > 0 && weatherConfig.userHoursToUpdate <= 24) {
        return weatherConfig.userHoursToUpdate * 60 * 60;
    }
    return cacheDurationDefault;    
}

bool cForecastIO::CacheFileValid(void) {
    if (!FileExists(forecastFile)) {
        dsyslog("weatherforecast: no cached forecast available");
        return false;
    }
    int cacheDuration = CalculateCachDuration();
    time_t fileCreation = FileCreationTime(forecastFile);
    time_t now = time(0);
    int age = now - fileCreation;
    int ageHours = age / 3600;
    int ageMinutes = (age%3600) / 60;
    if (age > cacheDuration) {
        dsyslog("weatherforecast: cached forecast is outdated (age %dh %dmin, cache duration %dh)", ageHours, ageMinutes, cacheDuration / 3600);
        return false;
    }
    return true;
}

void cForecastIO::ReadForecastInitial(void) {
    string forecastJson = "";
    if (!CacheFileValid()) {
        //get new from forecast.io
        dsyslog("weatherforecast: fetching forecast newly from forecast.io");
        forecastJson = FetchOnlineForecast();
        WriteIntoFile(forecastFile, forecastJson);
    } else {
        //using cached data
        dsyslog("weatherforecast: using cached forecast");
        forecastJson = ReadFromFile(forecastFile);
    }
    ParseForecast(forecastJson);

}

void cForecastIO::ReadForecast(void) {
    dsyslog("weatherforecast: updating forecast from forecast.io");
    string forecastJson = "";
    forecastJson = FetchOnlineForecast();
    if (forecastJson.size() > 0) {
        WriteIntoFile(forecastFile, forecastJson);
        Lock();
        Clear();
        ParseForecast(forecastJson);
        Unlock();
    }
}

string cForecastIO::FetchOnlineForecast(void) {
    string myApiKey = apiKey;
    if (CheckUserApiKey())
        myApiKey = weatherConfig.userApiKey;
    stringstream url;
    url << baseURL << "/" << myApiKey;
    url << "/" << latitude << "," << longitude;
    url << "?lang=" << language << "&units=" << unit;
    string outputForecastIO;
    esyslog("weatherforecast: calling URL %s", url.str().c_str());
    CurlGetUrl(url.str().c_str(), &outputForecastIO);
    return outputForecastIO;
}

bool cForecastIO::ParseForecast(string &jsonForecast) {
    json_t *root;
    json_error_t error;

    root = json_loads(jsonForecast.c_str(), 0, &error);
    if ( !root ) {
        return false;
    }
    if ( !json_is_object(root) ) {
        return false;
    }

    //reading current weather
    json_t *currentWeather = json_object_get(root, "currently");
    if (!json_is_object(currentWeather)) {
        return false;
    }
    current = new cForecast();
    current->SetForecastType(ftCurrent);
    current->SetForecast(currentWeather);
    //current->Debug();

    //reading 48 hour forecast
    json_t *hourlyWeather = json_object_get(root, "hourly");
    if (!json_is_object(hourlyWeather)) {
        return false;
    }
    hourly = new cForecasts();
    hourly->SetForecastType(ftHourly);
    hourly->SetForecast(hourlyWeather);
    //hourly->Debug();

    //reading 7 day forecast
    json_t *dailyWeather = json_object_get(root, "daily");
    if (!json_is_object(dailyWeather)) {
        return false;
    }
    daily = new cForecasts();
    daily->SetForecastType(ftDaily);
    daily->SetForecast(dailyWeather);
    //daily->Debug();

    return true;
}
