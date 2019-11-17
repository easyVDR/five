#include <vdr/plugin.h>
#include <string>
#include <sstream>
#include <jansson.h>
#include "../tools/jsonhelpers.h"
#include "forecast.h"

using namespace std;

cForecast::cForecast(void) {
    type = ftUnknownForecast;
    forecastTime = 0;
    summary = "";
    icon = "";
    sunriseTime = 0;
    sunsetTime = 0;
    moonPhase = 0.0;
    precipIntensity = 0.0;
    precipProbability = 0.0;
    percipType = "";
    temperature = 0.0;
    apparentTemperature = 0.0;
    temperatureMin = 0.0;
    temperatureMinTime = 0;
    temperatureMax = 0.0;
    temperatureMaxTime = 0;
    humidity = 0.0;
    windSpeed = 0.0;
    windBearing = 0;
    visibility = 0.0;
    cloudCover = 0.0;
    pressure = 0.0;
    ozone = 0.0;
}

cForecast::~cForecast() {
}

void cForecast::SetForecast(json_t *forecast) {
    forecastTime = JsonGetInteger(forecast, "time");
    summary = JsonGetString(forecast, "summary");
    icon = JsonGetString(forecast, "icon");
    sunriseTime = JsonGetInteger(forecast, "sunriseTime");
    sunsetTime = JsonGetInteger(forecast, "sunsetTime");
    moonPhase = JsonGetFloat(forecast, "moonPhase");
    precipIntensity = JsonGetFloat(forecast, "precipIntensity");
    precipProbability = JsonGetFloat(forecast, "precipProbability");
    percipType = JsonGetString(forecast, "percipType");
    temperature = JsonGetFloat(forecast, "temperature");
    apparentTemperature = JsonGetFloat(forecast, "apparentTemperature");
    temperatureMin = JsonGetFloat(forecast, "temperatureMin");
    temperatureMinTime = JsonGetInteger(forecast, "temperatureMinTime");
    temperatureMax = JsonGetFloat(forecast, "temperatureMax");
    temperatureMaxTime = JsonGetInteger(forecast, "temperatureMaxTime");
    humidity = JsonGetFloat(forecast, "humidity");
    windSpeed = JsonGetFloat(forecast, "windSpeed");
    windBearing = JsonGetInteger(forecast, "windBearing");
    visibility = JsonGetFloat(forecast, "visibility");
    cloudCover = JsonGetFloat(forecast, "cloudCover");
    pressure = JsonGetFloat(forecast, "pressure");
    ozone = JsonGetFloat(forecast, "ozone");
}

bool cForecast::TimeMatch(time_t compare) {
    //forecast is valid if time of forecast is between 30mins before or after compare time
    int difference = abs(compare - forecastTime);
    if (difference <= 1800)
        return true;
    return false;
}

bool cForecast::DayMatch(time_t compare) {
    int difference = compare - forecastTime;
    if (difference >= 0 && difference <= 86400)
        return true;
    return false;
}

string cForecast::GetDate(time_t myTime) {
    struct tm *ts;
    ts = localtime(&myTime);
    return *cString::sprintf("%d.%02d", ts->tm_mday, ts->tm_mon+1);
}

string cForecast::GetTime(time_t myTime) {
    struct tm *ts;
    ts = localtime(&myTime);
    return *cString::sprintf("%02d:%02d", ts->tm_hour, ts->tm_min);
}

string cForecast::GetDateTime(time_t myTime) {
    struct tm *ts;
    ts = localtime(&myTime);
    return *cString::sprintf("%d.%02d %02d:%02d", ts->tm_mday, ts->tm_mon+1, ts->tm_hour, ts->tm_min);
}

string cForecast::GetWindBearingString(void) {
    string sBearing = "";
    if (windBearing >= 349 || windBearing <= 11) {
        sBearing = "N";
    } else if (windBearing > 11 && windBearing <= 34) {
        sBearing = "NNE";
    } else if (windBearing > 34 && windBearing <= 56) {
        sBearing = "NE";
    } else if (windBearing > 56 && windBearing <= 79) {
        sBearing = "ENE";
    } else if (windBearing > 79 && windBearing <= 101) {
        sBearing = "E";
    } else if (windBearing > 101 && windBearing <= 123) {
        sBearing = "ESE";
    } else if (windBearing > 123 && windBearing <= 145) {
        sBearing = "SE";
    } else if (windBearing > 145 && windBearing <= 168) {
        sBearing = "SSE";
    } else if (windBearing > 168 && windBearing <= 191) {
        sBearing = "S";
    } else if (windBearing > 191 && windBearing <= 213) {
        sBearing = "SSW";
    } else if (windBearing > 213 && windBearing <= 235) {
        sBearing = "SW";
    } else if (windBearing > 235 && windBearing <= 258) {
        sBearing = "WSW";
    } else if (windBearing > 258 && windBearing <= 281) {
        sBearing = "W";
    } else if (windBearing > 281 && windBearing <= 303) {
        sBearing = "WNW";
    } else if (windBearing > 303 && windBearing <= 325) {
        sBearing = "NW";
    } else if (windBearing > 325 && windBearing <= 348) {
        sBearing = "NNW";
    }
    return sBearing;
}

void cForecast::Debug(bool verbose) {
    if (type == ftCurrent) {
        dsyslog("weatherforecast: current weather for %s at %s, summary: \"%s\", icon: \"%s\"", GetDate(forecastTime).c_str(), GetTime(forecastTime).c_str(), summary.c_str(), icon.c_str());
    } else if (type == ftHourly) {
        dsyslog("weatherforecast: hourly weather for %s on %s, summary: \"%s\", icon: \"%s\"", GetDate(forecastTime).c_str(), GetTime(forecastTime).c_str(), summary.c_str(), icon.c_str());
    } else if (type == ftDaily) {
        dsyslog("weatherforecast: daily weather for %s, summary: \"%s\", icon: \"%s\"", GetDate(forecastTime).c_str(), summary.c_str(), icon.c_str());
    }

    if (!verbose)
        return;
    dsyslog("weatherforecast: sunrise time %ld, sunset time: %ld, moon phase %f", sunriseTime, sunsetTime, moonPhase);
    dsyslog("weatherforecast: min temp %f at %ld, max temp %f at %ld", temperatureMin, temperatureMinTime, temperatureMax, temperatureMaxTime);
    dsyslog("weatherforecast: precipInt %f, precipProp: %f, type %s", precipIntensity, precipProbability, percipType.c_str());
    dsyslog("weatherforecast: temperature %f °C, apparentTemp %f °C, humidity: %f", temperature, apparentTemperature, humidity);
    dsyslog("weatherforecast: windSpeed %f, windBearing: %d, visibility %f, cloudCover: %f, pressure %f, ozone: %f", windSpeed, windBearing, visibility, cloudCover, pressure, ozone);
}
