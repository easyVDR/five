#include <vdr/plugin.h>
#include <string>
#include <sstream>
#include <jansson.h>
#include "../tools/jsonhelpers.h"
#include "forecasts.h"

using namespace std;

cForecasts::cForecasts(void) {
    type = ftUnknownForecast;
    summary = "";
    icon = "";
    numDataPoints = 0;
    dataPointPointer = 0;
    dataPoints = NULL;
}

cForecasts::~cForecasts() {
    if (dataPoints && numDataPoints > 0) {
        for (int i=0; i<numDataPoints; i++) {
            if (dataPoints[i])
                delete dataPoints[i];
        }
        delete[] dataPoints;
    }
}

void cForecasts::SetForecast(json_t *forecast) {
    
    summary = JsonGetString(forecast, "summary");
    icon = JsonGetString(forecast, "icon");

    json_t *data = json_object_get(forecast, "data");
    if ( !json_is_array(data) ) {
        return;
    }
    numDataPoints = json_array_size(data);

    dataPoints = new cForecast*[numDataPoints];

    for (int i = 0; i < numDataPoints; i++) {
        json_t *jDataPoint = json_array_get(data, i);
        cForecast *dataPoint = new cForecast();
        dataPoint->SetForecast(jDataPoint);
        dataPoint->SetForecastType(type);
        dataPoints[i] = dataPoint;
    }
}

int cForecasts::NumHourly(void) {
    int num = 0;
    cForecast *f = GetFirstHourly();
    while (f) {
        num++;
        f = GetNext();
    }
    return num;
}

int cForecasts::NumDaily(void) {
    int num = 0;
    cForecast *f = GetFirstDaily();
    while (f) {
        num++;
        f = GetNext();
    }
    return num;
}

cForecast *cForecasts::GetForecast(int dataPoint) {
    if (!dataPoints)
        return NULL;
    if (dataPoint < 0 || dataPoint >= numDataPoints)
        return NULL;
    return dataPoints[dataPoint];
}

cForecast *cForecasts::GetCurrent(void) {
    if (!dataPoints)
        return NULL;
    time_t now = time(0);
    for (int i=0; i<numDataPoints; i++) {
        if (dataPoints[i] && dataPoints[i]->TimeMatch(now))
            return dataPoints[i];
    }
    return NULL;
}

cForecast *cForecasts::GetFirstHourly(void) {
    if (!dataPoints)
        return NULL;
    time_t now = time(0);
    for (int i=0; i<numDataPoints; i++) {
        if (dataPoints[i] && dataPoints[i]->TimeMatch(now)) {
            dataPointPointer = i;
            return dataPoints[dataPointPointer];
        }
    }
    return NULL;
}

cForecast *cForecasts::GetFirstDaily(void) {
    if (!dataPoints)
        return NULL;
    time_t now = time(0);
    for (int i=0; i<numDataPoints; i++) {
        if (dataPoints[i] && dataPoints[i]->DayMatch(now)) {
            dataPointPointer = i;
            return dataPoints[dataPointPointer];
        }
    }
    return NULL;    
}


cForecast *cForecasts::GetNext(void) {
    if (!dataPoints)
        return NULL;
    dataPointPointer++;
    if (dataPointPointer < numDataPoints) {
        return dataPoints[dataPointPointer];        
    }
    dataPointPointer = 0;
    return NULL;
}

void cForecasts::Debug(void) {
    if (type == ftHourly) {
        dsyslog("weatherforecast: 48 hour forecast");
    } else if (type == ftDaily) {
        dsyslog("weatherforecast: 7 day forecast");
    } 
    dsyslog("weatherforecast: summary: \"%s\"", summary.c_str());
    dsyslog("weatherforecast: icon: \"%s\"", icon.c_str());
    for (int i = 0; i < numDataPoints; i++) {
        dsyslog("weatherforecast: ------------ %s %d -----------", (type == ftHourly)?"Hour":"Day", i);
        dataPoints[i]->Debug();
    }
}
