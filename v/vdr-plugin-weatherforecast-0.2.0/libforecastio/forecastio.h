#ifndef __FORECASTIO_H
#define __FORECASTIO_H

#include <string>
#include "forecasts.h"
#include "forecast.h"
#include "../services.h"

using namespace std;

class cForecastIO : public cThread {
private:
    string forecastFile;
    int cacheDurationDefault;
    string apiKey;
    string language;
    string unit;
    string baseURL;
    double latitude;
    double longitude;
    cForecast *current;
    cForecasts *hourly;
    cForecasts *daily;
    bool loopActive;
    cCondVar waitCondition;
    cMutex waitMutex;
    bool CheckUserApiKey(void);
    int CalculateCachDuration(void);
    bool CacheFileValid(void);
    string FetchOnlineForecast(void);
    bool ParseForecast(string &jsonForecast);
    void Action(void);
    void Clear(void);
    void ReadForecastInitial(void);
    void ReadForecast(void);
public:
    cForecastIO(string cacheDir);
    virtual ~cForecastIO(void);
    void Stop(void); 
    void LockForecasts(void) { Lock(); };
    void UnlockForecasts(void) { Unlock(); };
    cForecast *GetCurrentForecast(void);
    cForecasts *GetHourlyForecast(void) { return hourly; };
    cForecasts *GetDailyForecast(void) { return daily; };
    bool SetCurrentWeather(cServiceCurrentWeather *call);
};


#endif //__FORECASTIO_H