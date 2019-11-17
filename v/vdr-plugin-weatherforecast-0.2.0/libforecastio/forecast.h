#ifndef __FORECAST_H
#define __FORECAST_H

#include <string>
#include <jansson.h>

using namespace std;

enum eForecastType {
    ftUnknownForecast,
    ftCurrent,
    ftHourly,
    ftDaily,
};

class cForecast {
private:
    eForecastType type;
    time_t forecastTime;
    string summary;
    string icon;
    time_t sunriseTime;
    time_t sunsetTime;
    float moonPhase;
    float precipIntensity;
    float precipProbability;
    string percipType;
    float temperature;
    float apparentTemperature;
    float temperatureMin;
    time_t temperatureMinTime;
    float temperatureMax;
    time_t temperatureMaxTime;
    float humidity;
    float windSpeed;
    int windBearing;
    float visibility;
    float cloudCover;
    float pressure;
    float ozone;
    string GetDate(time_t myTime);
    string GetTime(time_t myTime);
    string GetDateTime(time_t myTime);
public:
    cForecast(void);
    virtual ~cForecast(void);
    void SetForecast(json_t *forecast);
    void SetForecastType(eForecastType type) { this->type = type; };
    bool TimeMatch(time_t compare);
    bool DayMatch(time_t compare);
    void Debug(bool verbose = false);
    //Getter Functions for native access to data
    time_t GetTime(void) { return forecastTime; };
    string GetSummary(void) { return summary; };
    string GetIcon(void) { return icon; };
    time_t GetSunrise(void) { return sunriseTime; };
    time_t GetSunset(void) { return sunsetTime; };
    float GetMoonphase(void) { return moonPhase; };
    float GetPrecipIntensity(void) { return precipIntensity; };
    float GetPrecipProbability(void) { return precipProbability; };
    string GetPercipType(void) { return percipType; };
    float GetTemperature(void) { return temperature; };
    float GetTemperatureMin(void) { return temperatureMin; };
    time_t GetTemperatureMinTime(void) { return temperatureMinTime; };
    float GetTemperatureMax(void) { return temperatureMax; };
    time_t GetTemperatureMaxTime(void) { return temperatureMaxTime; };
    float GetApparentTemperature(void) { return apparentTemperature; };
    float GetHumidity(void) { return humidity; };
    float GetWindSpeed(void) { return windSpeed; };
    int GetWindSpeedKnots(void) { return (windSpeed+10)/6; };
    int GetWindBearing(void) { return windBearing; };
    float GetVisibility(void) { return visibility; };
    float GetCloudCover(void) { return cloudCover; };
    float GetPressure(void) { return pressure; };
    float GetOzone(void) { return ozone; };
    //convenient Getter Functions
    string GetDateTimeString(void) { return GetDateTime(forecastTime); };
    string GetDateString(void) { return GetDate(forecastTime); };
    string GetTimeString(void) { return GetTime(forecastTime); };
    string GetDayName(void) { return *WeekDayName(forecastTime); };    
    string GetSunriseString(void) { return GetTime(sunriseTime); };
    string GetSunsetString(void) { return GetTime(sunsetTime); };
    int GetMoonphasePercent(void) { return (int)(moonPhase*100); };
    string GetPrecipIntensityString(void) { return *cString::sprintf("%.1f", precipIntensity); };
    int GetPrecipProbabilityPercent(void) { return (int)(precipProbability*100); };
    string GetTemperatureString(void) { return *cString::sprintf("%.1f", temperature); };
    string GetTemperatureMinString(void) { return *cString::sprintf("%.1f", temperatureMin); };
    string GetTemperatureMinTimeString(void) { return GetTime(temperatureMinTime); };
    string GetTemperatureMaxString(void) { return *cString::sprintf("%.1f", temperatureMax); };
    string GetTemperatureMaxTimeString(void) { return GetTime(temperatureMaxTime); };
    string GetApparentTemperatureString(void) { return *cString::sprintf("%.1f", apparentTemperature); };
    int GetHumidityPercent(void) { return (int)(humidity*100); };
    string GetWindSpeedString(void) { return *cString::sprintf("%.1f", windSpeed); };
    string GetWindBearingString(void);
    string GetVisibilityString(void) { return *cString::sprintf("%.1f", visibility); };
    int GetCloudCoverPercent(void) { return (int)(cloudCover*100); };
    string GetPressureString(void) { return *cString::sprintf("%.1f", pressure); };
    string GetOzoneString(void) { return *cString::sprintf("%.1f", ozone); };
};


#endif //__FORECASTIO_H