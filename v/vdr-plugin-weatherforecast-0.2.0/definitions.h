#ifndef __DEFINITIONS_H
#define __DEFINITIONS_H

/******************************************************************
* Menus
*******************************************************************/
enum class eMenus {
    root,
    detailCurrent,
    detailHourly,
    detailDaily
};

/******************************************************************
* Tokens root menu items
*******************************************************************/
enum class eRootMenuST {
    menuitemtext = 0,
    city,
    latitude,
    longitude,
    timestamp,
    temperature,
    apparenttemperature,
    summary,
    icon,
    precipitationintensity,
    precipitationtype,
    windspeed,
    windbearingstring,
    visibility,
    pressure,
    ozone,
    count
};

enum class eRootMenuIT {
    current = 0,
    iscurrent,
    ishourly,
    isdaily,
    precipitationprobability,
    humidity,
    windbearing,
    cloudcover,
    count
};

enum class eForecastHourlyLT {
    num = 0,
    timestamp,
    temperature,
    apparenttemperature,
    summary,
    icon,
    precipitationintensity,    
    precipitationprobability,
    precipitationtype,
    humidity,
    windspeed,
    windbearing,
    windbearingstring,
    visibility,
    cloudcover,
    pressure,
    ozone,
    count
};

enum class eForecastDailyLT {
    day = 0,
    dayname,
    temperaturemin,
    temperaturemintime,
    temperaturemax,
    temperaturemaxtime,
    summary,
    icon,
    precipitationintensity,    
    precipitationprobability,
    precipitationtype,
    humidity,
    windspeed,
    windbearing,
    windbearingstring,
    visibility,
    cloudcover,
    pressure,
    ozone,
    count
};

enum class eDetailCurrentST {
    menuheader = 0,
    city,
    latitude,
    longitude,
    timestamp,
    temperature,
    apparenttemperature,
    mintemperature,
    maxtemperature,
    summary,
    icon,
    precipitationintensity,
    precipitationtype,
    windspeed,
    windbearingstring,
    visibility,
    pressure,
    ozone,
    count
};

enum class eDetailCurrentIT {
    precipitationprobability = 0,
    humidity,
    windbearing,
    cloudcover,
    count
};

enum class eDetailHourlyST {
    menuheader = 0,
    summary,
    icon,
    city,
    latitude,
    longitude,
    count
};

enum class eDetailDailyST {
    menuheader = 0,
    summary,
    icon,
    city,
    latitude,
    longitude,
    count
};

#endif //__DEFINITIONS_H
