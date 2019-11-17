#ifndef __FORECASTLOCATOR_H
#define __FORECASTLOCATOR_H

#include <string>

using namespace std;

class cForecastLocator {
private:
    string urlIPLApi;
    string city;
    float lat;
    float lon;
public:
    cForecastLocator(void);
    virtual ~cForecastLocator(void);
    bool ReadLocationByIP(void);
    void WriteToSetup(cPlugin *weatherPlug);
};


#endif //__FORECASTLOCATOR_H