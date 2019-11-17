#ifndef __FORECASTS_H
#define __FORECASTS_H

#include <string>
#include <jansson.h>
#include "forecast.h"

using namespace std;

class cForecasts {
private:
    eForecastType type;
    string summary;
    string icon;
    int numDataPoints;
    int dataPointPointer;
    cForecast **dataPoints;
public:
    cForecasts(void);
    virtual ~cForecasts(void);
    void SetForecast(json_t *forecast);
    void SetForecastType(eForecastType type) { this->type = type; };
    string GetSummary(void) { return summary; };
    string GetIcon(void) { return icon; };
    int NumHourly(void);
    int NumDaily(void);
    cForecast *GetForecast(int dataPoint);
    cForecast *GetCurrent(void);
    cForecast *GetFirstHourly(void);
    cForecast *GetFirstDaily(void);
    cForecast *GetNext(void);
    void Debug(void);
};


#endif //__FORECASTS_H