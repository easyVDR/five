#include "config.h"
#include "tools/jsonhelpers.h"
#include "weatherosd.h"

extern cWeatherforecastConfig weatherConfig;

cWeatherOsd::cWeatherOsd(cForecastIO *forecastIO, skindesignerapi::cPluginStructure *plugStruct) : cSkindesignerOsdMenu(plugStruct, "Weather Forecast") {
    isDetailedView = false;
    lastRootMenuElement = 0;
    this->forecastIO = forecastIO;
    SetRootMenu();
}

cWeatherOsd::~cWeatherOsd(void) {
}

eOSState cWeatherOsd::ProcessKey(eKeys key) {
    eOSState state = cOsdMenu::ProcessKey(key);
    switch (key) {
        case kOk: {
            if (isDetailedView) {
                SetRootMenu();
            } else {
                int element = Current();
                lastRootMenuElement = element;
                if (element == 0)
                    SetDetailViewCurrent();
                else if (element == 1)
                    SetDetailViewHourly();
                else if (element == 2)
                    SetDetailViewDaily();
            }
            state = osContinue;
            break;
        } case kLeft: {
            TextKeyLeft();
            state = osContinue;
            break;
        } case kRight: {
            TextKeyRight();
            state = osContinue;            
            break;
        } case kUp: {
            TextKeyUp();
            state = osContinue;            
            break;
        } case kDown: {
            TextKeyDown();
            state = osContinue;
            break;
        } case kBack: {
            if (isDetailedView) {
                SetRootMenu();
                state = osContinue;
            }
            break;
        } default:
            break;
    }
    return state;
}

void cWeatherOsd::SetRootMenu(void) {
    isDetailedView = false;
    SetPluginMenu((int)eMenus::root, skindesignerapi::mtList);
    Clear();
    SetTitle(tr("Weather Forecast"));
    
    forecastIO->LockForecasts();

    skindesignerapi::cTokenContainer *tkRoot = GetTokenContainer((int)eMenus::root);
    skindesignerapi::cSkindesignerOsdItem *currentWeather = new skindesignerapi::cSkindesignerOsdItem(tkRoot);

    string itemLabelCurrent = tr("Current Weather");
    currentWeather->SetText(itemLabelCurrent.c_str());
    currentWeather->AddStringToken((int)eRootMenuST::menuitemtext, itemLabelCurrent.c_str());
    currentWeather->AddIntToken((int)eRootMenuIT::iscurrent, 1);
    currentWeather->AddIntToken((int)eRootMenuIT::ishourly, 0);
    currentWeather->AddIntToken((int)eRootMenuIT::isdaily, 0);
    currentWeather->AddStringToken((int)eRootMenuST::city, weatherConfig.city.c_str());
    currentWeather->AddStringToken((int)eRootMenuST::latitude, FloatToString(weatherConfig.lat).c_str());
    currentWeather->AddStringToken((int)eRootMenuST::longitude, FloatToString(weatherConfig.lon).c_str());

    cForecast *currentForecast = forecastIO->GetCurrentForecast();
    if (currentForecast) {
        currentWeather->AddStringToken((int)eRootMenuST::timestamp, currentForecast->GetDateTimeString().c_str());
        currentWeather->AddStringToken((int)eRootMenuST::temperature, currentForecast->GetTemperatureString().c_str());
        currentWeather->AddStringToken((int)eRootMenuST::apparenttemperature, currentForecast->GetApparentTemperatureString().c_str());
        currentWeather->AddStringToken((int)eRootMenuST::summary, currentForecast->GetSummary().c_str());
        currentWeather->AddStringToken((int)eRootMenuST::icon, currentForecast->GetIcon().c_str());
        currentWeather->AddStringToken((int)eRootMenuST::precipitationintensity, currentForecast->GetPrecipIntensityString().c_str());
        currentWeather->AddIntToken((int)eRootMenuIT::precipitationprobability, currentForecast->GetPrecipProbabilityPercent());
        currentWeather->AddStringToken((int)eRootMenuST::precipitationtype, currentForecast->GetPercipType().c_str());
        currentWeather->AddIntToken((int)eRootMenuIT::humidity, currentForecast->GetHumidityPercent());
        currentWeather->AddStringToken((int)eRootMenuST::windspeed, currentForecast->GetWindSpeedString().c_str());
        currentWeather->AddIntToken((int)eRootMenuIT::windbearing, currentForecast->GetWindBearing());
        currentWeather->AddStringToken((int)eRootMenuST::windbearingstring, currentForecast->GetWindBearingString().c_str());
        currentWeather->AddStringToken((int)eRootMenuST::visibility, currentForecast->GetVisibilityString().c_str());
        currentWeather->AddIntToken((int)eRootMenuIT::cloudcover, currentForecast->GetCloudCoverPercent());
        currentWeather->AddStringToken((int)eRootMenuST::pressure, currentForecast->GetPressureString().c_str());
        currentWeather->AddStringToken((int)eRootMenuST::ozone, currentForecast->GetOzoneString().c_str());
    }
    vector<int> loopInfo;        
    loopInfo.push_back(0);
    loopInfo.push_back(0);
    currentWeather->SetLoop(loopInfo);
    Add(currentWeather, (lastRootMenuElement == 0)?true:false);

    skindesignerapi::cSkindesignerOsdItem *nextHours = new skindesignerapi::cSkindesignerOsdItem(tkRoot);
    string itemLabelNextHours = tr("Next 48 Hours");
    nextHours->SetText(itemLabelNextHours.c_str());
    nextHours->AddStringToken((int)eRootMenuST::menuitemtext, itemLabelNextHours.c_str());
    nextHours->AddIntToken((int)eRootMenuIT::iscurrent, 0);
    nextHours->AddIntToken((int)eRootMenuIT::ishourly, 1);
    nextHours->AddIntToken((int)eRootMenuIT::isdaily, 0);
    nextHours->AddStringToken((int)eRootMenuST::city, weatherConfig.city.c_str());
    nextHours->AddStringToken((int)eRootMenuST::latitude, FloatToString(weatherConfig.lat).c_str());
    nextHours->AddStringToken((int)eRootMenuST::longitude, FloatToString(weatherConfig.lon).c_str());

    cForecasts *hourlyForecast = forecastIO->GetHourlyForecast();
    if (hourlyForecast) {
        nextHours->AddStringToken((int)eRootMenuST::summary, hourlyForecast->GetSummary().c_str());
        nextHours->AddStringToken((int)eRootMenuST::icon, hourlyForecast->GetIcon().c_str());

        int hourlyIndex = nextHours->GetLoopIndex("hourly");
        int numHourly = hourlyForecast->NumHourly();

        vector<int> loopInfo;        
        loopInfo.push_back(numHourly);
        loopInfo.push_back(0);
        nextHours->SetLoop(loopInfo);
        cForecast *fc = hourlyForecast->GetFirstHourly();
        int i = 0;
        while (fc) {
            nextHours->AddLoopToken(hourlyIndex, i, (int)eForecastHourlyLT::num, IntToString(i+1).c_str());
            nextHours->AddLoopToken(hourlyIndex, i, (int)eForecastHourlyLT::timestamp, fc->GetTimeString().c_str());
            nextHours->AddLoopToken(hourlyIndex, i, (int)eForecastHourlyLT::temperature, fc->GetTemperatureString().c_str());
            nextHours->AddLoopToken(hourlyIndex, i, (int)eForecastHourlyLT::apparenttemperature, fc->GetApparentTemperatureString().c_str());
            nextHours->AddLoopToken(hourlyIndex, i, (int)eForecastHourlyLT::summary, fc->GetSummary().c_str());
            nextHours->AddLoopToken(hourlyIndex, i, (int)eForecastHourlyLT::icon, fc->GetIcon().c_str());
            nextHours->AddLoopToken(hourlyIndex, i, (int)eForecastHourlyLT::precipitationintensity, fc->GetPrecipIntensityString().c_str());
            nextHours->AddLoopToken(hourlyIndex, i, (int)eForecastHourlyLT::precipitationprobability, IntToString(fc->GetPrecipProbabilityPercent()).c_str());
            nextHours->AddLoopToken(hourlyIndex, i, (int)eForecastHourlyLT::precipitationtype, fc->GetPercipType().c_str());
            nextHours->AddLoopToken(hourlyIndex, i, (int)eForecastHourlyLT::humidity, IntToString(fc->GetHumidityPercent()).c_str());
            nextHours->AddLoopToken(hourlyIndex, i, (int)eForecastHourlyLT::windspeed, fc->GetWindSpeedString().c_str());
            nextHours->AddLoopToken(hourlyIndex, i, (int)eForecastHourlyLT::windbearing, IntToString(fc->GetWindBearing()).c_str());
            nextHours->AddLoopToken(hourlyIndex, i, (int)eForecastHourlyLT::windbearingstring, fc->GetWindBearingString().c_str());
            nextHours->AddLoopToken(hourlyIndex, i, (int)eForecastHourlyLT::visibility, fc->GetWindBearingString().c_str());
            nextHours->AddLoopToken(hourlyIndex, i, (int)eForecastHourlyLT::cloudcover, IntToString(fc->GetCloudCoverPercent()).c_str());
            nextHours->AddLoopToken(hourlyIndex, i, (int)eForecastHourlyLT::pressure, fc->GetPressureString().c_str());
            nextHours->AddLoopToken(hourlyIndex, i, (int)eForecastHourlyLT::ozone, fc->GetOzoneString().c_str());
            fc = hourlyForecast->GetNext();
            i++;
        }
    }

    Add(nextHours, (lastRootMenuElement == 1)?true:false);

    skindesignerapi::cSkindesignerOsdItem *nextDays = new skindesignerapi::cSkindesignerOsdItem(tkRoot);
    string itemLabelNextDays = tr("Next 7 Days");
    nextDays->SetText(itemLabelNextDays.c_str());
    nextDays->AddStringToken((int)eRootMenuST::menuitemtext, itemLabelNextDays.c_str());
    nextDays->AddIntToken((int)eRootMenuIT::iscurrent, 0);
    nextDays->AddIntToken((int)eRootMenuIT::ishourly, 0);
    nextDays->AddIntToken((int)eRootMenuIT::isdaily, 1);
    nextDays->AddStringToken((int)eRootMenuST::city, weatherConfig.city.c_str());
    nextDays->AddStringToken((int)eRootMenuST::latitude, FloatToString(weatherConfig.lat).c_str());
    nextDays->AddStringToken((int)eRootMenuST::longitude, FloatToString(weatherConfig.lon).c_str());

    cForecasts *dailyForecast = forecastIO->GetDailyForecast();
    if (dailyForecast) {
        nextDays->AddStringToken((int)eRootMenuST::summary, dailyForecast->GetSummary().c_str());
        nextDays->AddStringToken((int)eRootMenuST::icon, dailyForecast->GetIcon().c_str());

        int dailyIndex = nextDays->GetLoopIndex("daily");
        int numHourly = 0;
        int numDaily = dailyForecast->NumDaily();
        
        vector<int> loopInfo;
        loopInfo.push_back(numHourly);
        loopInfo.push_back(numDaily);
        nextDays->SetLoop(loopInfo);

        cForecast *fc = dailyForecast->GetFirstDaily();
        int i=0;
        while (fc) {
            nextDays->AddLoopToken(dailyIndex, i, (int)eForecastDailyLT::day, fc->GetDateString().c_str());
            nextDays->AddLoopToken(dailyIndex, i, (int)eForecastDailyLT::dayname, fc->GetDayName().c_str());
            nextDays->AddLoopToken(dailyIndex, i, (int)eForecastDailyLT::temperaturemin, fc->GetTemperatureMinString().c_str());
            nextDays->AddLoopToken(dailyIndex, i, (int)eForecastDailyLT::temperaturemintime, fc->GetTemperatureMinTimeString().c_str());
            nextDays->AddLoopToken(dailyIndex, i, (int)eForecastDailyLT::temperaturemax, fc->GetTemperatureMaxString().c_str());
            nextDays->AddLoopToken(dailyIndex, i, (int)eForecastDailyLT::temperaturemaxtime, fc->GetTemperatureMaxTimeString().c_str());
            nextDays->AddLoopToken(dailyIndex, i, (int)eForecastDailyLT::summary, fc->GetSummary().c_str());
            nextDays->AddLoopToken(dailyIndex, i, (int)eForecastDailyLT::icon, fc->GetIcon().c_str());
            nextDays->AddLoopToken(dailyIndex, i, (int)eForecastDailyLT::precipitationintensity, fc->GetPrecipIntensityString().c_str());
            nextDays->AddLoopToken(dailyIndex, i, (int)eForecastDailyLT::precipitationprobability, IntToString(fc->GetPrecipProbabilityPercent()).c_str());
            nextDays->AddLoopToken(dailyIndex, i, (int)eForecastDailyLT::precipitationtype, fc->GetPercipType().c_str());
            nextDays->AddLoopToken(dailyIndex, i, (int)eForecastDailyLT::humidity, IntToString(fc->GetHumidityPercent()).c_str());
            nextDays->AddLoopToken(dailyIndex, i, (int)eForecastDailyLT::windspeed, fc->GetWindSpeedString().c_str());
            nextDays->AddLoopToken(dailyIndex, i, (int)eForecastDailyLT::windbearing, IntToString(fc->GetWindBearing()).c_str());
            nextDays->AddLoopToken(dailyIndex, i, (int)eForecastDailyLT::windbearingstring, fc->GetWindBearingString().c_str());
            nextDays->AddLoopToken(dailyIndex, i, (int)eForecastDailyLT::visibility, fc->GetVisibilityString().c_str());
            nextDays->AddLoopToken(dailyIndex, i, (int)eForecastDailyLT::cloudcover, IntToString(fc->GetCloudCoverPercent()).c_str());
            nextDays->AddLoopToken(dailyIndex, i, (int)eForecastDailyLT::pressure, fc->GetPressureString().c_str());
            nextDays->AddLoopToken(dailyIndex, i, (int)eForecastDailyLT::ozone, fc->GetOzoneString().c_str());
            fc = dailyForecast->GetNext();
            i++;
        }
    }

    forecastIO->UnlockForecasts();

    Add(nextDays, (lastRootMenuElement == 2)?true:false);

    Display();
}

void cWeatherOsd::SetDetailViewCurrent(void) {
    isDetailedView = true;
    SetPluginMenu((int)eMenus::detailCurrent, skindesignerapi::mtText);
    Clear();
    SetTitle(tr("Current Weather"));

    skindesignerapi::cTokenContainer *tkCurrent = GetTokenContainer((int)eMenus::detailCurrent);
    SetTokenContainer(tkCurrent);
    ClearTokens();

    cForecast *current = NULL;
    cForecasts *daily = NULL;
    cForecast *today = NULL;

    forecastIO->LockForecasts();
    bool ok = true;
    
    current = forecastIO->GetCurrentForecast();
    if (!current) ok = false;
    
    if (ok)
        daily = forecastIO->GetDailyForecast();
    if (!daily) ok = false;
    
    if (ok)
        today = daily->GetFirstDaily();
    if (!today) ok = false;
    
    if (!ok) {
        forecastIO->UnlockForecasts();
        return;
    }

    stringstream plainText;
    plainText << tr("Weather for") << " " << weatherConfig.city << " " << current->GetDateTimeString() << ": " <<  current->GetSummary() << "\n";
    plainText << tr("Current Temperature") << ": " << current->GetTemperatureString() << "°C" << "\n";
    plainText << tr("Apparent Temperature") << ": " << current->GetApparentTemperatureString() << "°C" << "\n";
    plainText << tr("Minimum Temperature today") << ": " << today->GetTemperatureMinString() << "°C" << "\n";
    plainText << tr("Maximum Temperature today") << ": " << today->GetTemperatureMaxString() << "°C" << "\n";
    plainText << tr("Precipitation Probability") << ": " << current->GetPrecipProbabilityPercent() << " %" << "\n";
    plainText << tr("Precipitation Intensity") << ": " << current->GetPrecipIntensityString() << " l/qm" << "\n";
    plainText << tr("Humidity") << ": " << current->GetHumidityPercent() << " %" << "\n";
    plainText << tr("Wind Speed") << ": " << current->GetWindSpeedString() << " km/h" << "\n";
    plainText << tr("Wind Bearing") << ": " << current->GetWindBearing() << "° " << current->GetWindBearingString() << "\n";
    plainText << tr("Visibility") << ": " << current->GetVisibilityString() << " km" << "\n";
    plainText << tr("Cloud Cover") << ": " << current->GetCloudCoverPercent() << " %" << "\n";
    plainText << tr("Pressure") << ": " << current->GetPressureString() << " HPa" << "\n";
    plainText << tr("Ozone") << ": " << current->GetOzoneString() << " DU" << "\n";
    SetText(plainText.str().c_str());

    AddStringToken((int)eDetailCurrentST::menuheader, tr("Current Weather"));
    AddStringToken((int)eDetailCurrentST::city, weatherConfig.city.c_str());
    AddStringToken((int)eDetailCurrentST::latitude, FloatToString(weatherConfig.lat).c_str());
    AddStringToken((int)eDetailCurrentST::longitude, FloatToString(weatherConfig.lon).c_str());
    AddStringToken((int)eDetailCurrentST::timestamp, current->GetDateTimeString().c_str());
    AddStringToken((int)eDetailCurrentST::temperature, current->GetTemperatureString().c_str());
    AddStringToken((int)eDetailCurrentST::apparenttemperature, current->GetApparentTemperatureString().c_str());
    AddStringToken((int)eDetailCurrentST::mintemperature, today->GetTemperatureMinString().c_str());
    AddStringToken((int)eDetailCurrentST::maxtemperature, today->GetTemperatureMaxString().c_str());
    AddStringToken((int)eDetailCurrentST::summary, current->GetSummary().c_str());
    AddStringToken((int)eDetailCurrentST::icon, current->GetIcon().c_str());
    AddStringToken((int)eDetailCurrentST::precipitationintensity, current->GetPrecipIntensityString().c_str());
    AddIntToken((int)eDetailCurrentIT::precipitationprobability, current->GetPrecipProbabilityPercent());
    AddStringToken((int)eDetailCurrentST::precipitationtype, current->GetPercipType().c_str());
    AddIntToken((int)eDetailCurrentIT::humidity, current->GetHumidityPercent());
    AddStringToken((int)eDetailCurrentST::windspeed, current->GetWindSpeedString().c_str());
    AddIntToken((int)eDetailCurrentIT::windbearing, current->GetWindBearing());
    AddStringToken((int)eDetailCurrentST::windbearingstring, current->GetWindBearingString().c_str());
    AddStringToken((int)eDetailCurrentST::visibility, current->GetVisibilityString().c_str());
    AddIntToken((int)eDetailCurrentIT::cloudcover, current->GetCloudCoverPercent());
    AddStringToken((int)eDetailCurrentST::pressure, current->GetPressureString().c_str());
    AddStringToken((int)eDetailCurrentST::ozone, current->GetOzoneString().c_str());

    forecastIO->UnlockForecasts();
    
    Display();

}

void cWeatherOsd::SetDetailViewHourly(void) {
    isDetailedView = true;
    SetPluginMenu((int)eMenus::detailHourly, skindesignerapi::mtText);
    Clear();
    SetTitle(tr("Weather in the next 48 Hours"));

    skindesignerapi::cTokenContainer *tkHourly = GetTokenContainer((int)eMenus::detailHourly);
    SetTokenContainer(tkHourly);
    ClearTokens();

    forecastIO->LockForecasts();
    cForecasts *hourly = forecastIO->GetHourlyForecast();
    if (!hourly) {
        forecastIO->UnlockForecasts();
        return;
    }

    stringstream plainText;

    AddStringToken((int)eDetailHourlyST::menuheader, tr("Weather in the next 48 Hours"));
    AddStringToken((int)eDetailHourlyST::summary, hourly->GetSummary().c_str());
    AddStringToken((int)eDetailHourlyST::icon, hourly->GetIcon().c_str());
    AddStringToken((int)eDetailHourlyST::city, weatherConfig.city.c_str());
    AddStringToken((int)eDetailHourlyST::latitude, FloatToString(weatherConfig.lat).c_str());
    AddStringToken((int)eDetailHourlyST::longitude, FloatToString(weatherConfig.lon).c_str());

    int hourlyIndex = GetLoopIndex("hourly");
    int numHourly = hourly->NumHourly();
    vector<int> loopInfo;        
    loopInfo.push_back(numHourly);
    SetLoop(loopInfo);

    cForecast *fc = hourly->GetFirstHourly();
    int i = 0;
    while (fc) {
        AddLoopToken(hourlyIndex, i, (int)eForecastHourlyLT::num, IntToString(i+1).c_str());
        AddLoopToken(hourlyIndex, i, (int)eForecastHourlyLT::timestamp, fc->GetTimeString().c_str());
        AddLoopToken(hourlyIndex, i, (int)eForecastHourlyLT::temperature, fc->GetTemperatureString().c_str());
        AddLoopToken(hourlyIndex, i, (int)eForecastHourlyLT::apparenttemperature, fc->GetApparentTemperatureString().c_str());
        AddLoopToken(hourlyIndex, i, (int)eForecastHourlyLT::summary, fc->GetSummary().c_str());
        AddLoopToken(hourlyIndex, i, (int)eForecastHourlyLT::icon, fc->GetIcon().c_str());
        AddLoopToken(hourlyIndex, i, (int)eForecastHourlyLT::precipitationintensity, fc->GetPrecipIntensityString().c_str());
        AddLoopToken(hourlyIndex, i, (int)eForecastHourlyLT::precipitationprobability, IntToString(fc->GetPrecipProbabilityPercent()).c_str());
        AddLoopToken(hourlyIndex, i, (int)eForecastHourlyLT::precipitationtype, fc->GetPercipType().c_str());
        AddLoopToken(hourlyIndex, i, (int)eForecastHourlyLT::humidity, IntToString(fc->GetHumidityPercent()).c_str());
        AddLoopToken(hourlyIndex, i, (int)eForecastHourlyLT::windspeed, fc->GetWindSpeedString().c_str());
        AddLoopToken(hourlyIndex, i, (int)eForecastHourlyLT::windbearing, IntToString(fc->GetWindBearing()).c_str());
        AddLoopToken(hourlyIndex, i, (int)eForecastHourlyLT::windbearingstring, fc->GetWindBearingString().c_str());
        AddLoopToken(hourlyIndex, i, (int)eForecastHourlyLT::visibility, fc->GetWindBearingString().c_str());
        AddLoopToken(hourlyIndex, i, (int)eForecastHourlyLT::cloudcover, IntToString(fc->GetCloudCoverPercent()).c_str());
        AddLoopToken(hourlyIndex, i, (int)eForecastHourlyLT::pressure, fc->GetPressureString().c_str());
        AddLoopToken(hourlyIndex, i, (int)eForecastHourlyLT::ozone, fc->GetOzoneString().c_str());
    
        plainText << fc->GetDateTimeString() << ": " <<  fc->GetSummary() << "\n";
        plainText << tr("Temperature") << ": " << fc->GetTemperatureString() << "°C, " << tr("felt") << " " << fc->GetApparentTemperatureString() << "°C""\n";
        plainText << tr("Precipitation Probability") << ": " << fc->GetPrecipProbabilityPercent() << " %, " << tr("Intensity") << ": " << fc->GetPrecipIntensityString() << " l/qm""\n";
        plainText << tr("Wind") << ": " << fc->GetWindSpeedString() << " km/h, " << fc->GetWindBearing() << "° " << fc->GetWindBearingString() << "\n";
        plainText << tr("Cloud Cover") << ": " << fc->GetCloudCoverPercent() << " %" << "\n";
        plainText << "\n";

        fc = hourly->GetNext();
        i++;
    }
    forecastIO->UnlockForecasts();

    SetText(plainText.str().c_str());
    Display();
}

void cWeatherOsd::SetDetailViewDaily(void) {
    isDetailedView = true;
    SetPluginMenu((int)eMenus::detailDaily, skindesignerapi::mtText);
    Clear();
    SetTitle(tr("Weather the next 7 days"));

    skindesignerapi::cTokenContainer *tkDaily = GetTokenContainer((int)eMenus::detailDaily);
    SetTokenContainer(tkDaily);
    ClearTokens();

    forecastIO->LockForecasts();
    cForecasts *daily = forecastIO->GetDailyForecast();
    if (!daily) {
        forecastIO->UnlockForecasts();
        return;
    }

    stringstream plainText;

    AddStringToken((int)eDetailDailyST::menuheader, tr("Weather the next 7 days"));
    AddStringToken((int)eDetailDailyST::summary, daily->GetSummary().c_str());
    AddStringToken((int)eDetailDailyST::icon, daily->GetIcon().c_str());
    AddStringToken((int)eDetailDailyST::city, weatherConfig.city.c_str());
    AddStringToken((int)eDetailDailyST::latitude, FloatToString(weatherConfig.lat).c_str());
    AddStringToken((int)eDetailDailyST::longitude, FloatToString(weatherConfig.lon).c_str());

    int dailyIndex = GetLoopIndex("daily");
    int numDaily = daily->NumDaily();
    vector<int> loopInfo;        
    loopInfo.push_back(numDaily);
    SetLoop(loopInfo);

    cForecast *fc = daily->GetFirstDaily();
    int i=0;
    while (fc) {
        AddLoopToken(dailyIndex, i, (int)eForecastDailyLT::day, fc->GetDateString().c_str());
        AddLoopToken(dailyIndex, i, (int)eForecastDailyLT::dayname, fc->GetDayName().c_str());
        AddLoopToken(dailyIndex, i, (int)eForecastDailyLT::temperaturemin, fc->GetTemperatureMinString().c_str());
        AddLoopToken(dailyIndex, i, (int)eForecastDailyLT::temperaturemintime, fc->GetTemperatureMinTimeString().c_str());
        AddLoopToken(dailyIndex, i, (int)eForecastDailyLT::temperaturemax, fc->GetTemperatureMaxString().c_str());
        AddLoopToken(dailyIndex, i, (int)eForecastDailyLT::temperaturemaxtime, fc->GetTemperatureMaxTimeString().c_str());
        AddLoopToken(dailyIndex, i, (int)eForecastDailyLT::summary, fc->GetSummary().c_str());
        AddLoopToken(dailyIndex, i, (int)eForecastDailyLT::icon, fc->GetIcon().c_str());
        AddLoopToken(dailyIndex, i, (int)eForecastDailyLT::precipitationintensity, fc->GetPrecipIntensityString().c_str());
        AddLoopToken(dailyIndex, i, (int)eForecastDailyLT::precipitationprobability, IntToString(fc->GetPrecipProbabilityPercent()).c_str());
        AddLoopToken(dailyIndex, i, (int)eForecastDailyLT::precipitationtype, fc->GetPercipType().c_str());
        AddLoopToken(dailyIndex, i, (int)eForecastDailyLT::humidity, IntToString(fc->GetHumidityPercent()).c_str());
        AddLoopToken(dailyIndex, i, (int)eForecastDailyLT::windspeed, fc->GetWindSpeedString().c_str());
        AddLoopToken(dailyIndex, i, (int)eForecastDailyLT::windbearing, IntToString(fc->GetWindBearing()).c_str());
        AddLoopToken(dailyIndex, i, (int)eForecastDailyLT::windbearingstring, fc->GetWindBearingString().c_str());
        AddLoopToken(dailyIndex, i, (int)eForecastDailyLT::visibility, fc->GetVisibilityString().c_str());
        AddLoopToken(dailyIndex, i, (int)eForecastDailyLT::cloudcover, IntToString(fc->GetCloudCoverPercent()).c_str());
        AddLoopToken(dailyIndex, i, (int)eForecastDailyLT::pressure, fc->GetPressureString().c_str());
        AddLoopToken(dailyIndex, i, (int)eForecastDailyLT::ozone, fc->GetOzoneString().c_str());

        plainText << fc->GetDayName() << " " << fc->GetDateString() << ": " <<  fc->GetSummary() << "\n";
        plainText << tr("Minimum Temperature") << ": " << fc->GetTemperatureMinString() << "°C " << tr("at") << " " << fc->GetTemperatureMinTimeString() << " ,";
        plainText << tr("Maximum Temperature") << ": " << fc->GetTemperatureMaxString() << "°C " << tr("at") << " " << fc->GetTemperatureMaxTimeString() << "\n";
        plainText << tr("Precipitation Probability") << ": " << fc->GetPrecipProbabilityPercent() << " %, " << tr("Intensity") << ": " << fc->GetPrecipIntensityString() << " l/qm""\n";
        plainText << tr("Wind") << ": " << fc->GetWindSpeedString() << " km/h, " << fc->GetWindBearing() << "° " << fc->GetWindBearingString() << "\n";
        plainText << tr("Cloud Cover") << ": " << fc->GetCloudCoverPercent() << " %" << "\n";
        plainText << tr("Pressure") << ": " << fc->GetPressureString() << " HPa" << "\n";
        plainText << tr("Ozone") << ": " << fc->GetOzoneString() << " DU" << "\n";
        plainText << "\n";

        fc = daily->GetNext();
        i++;
    }
    forecastIO->UnlockForecasts();
    
    SetText(plainText.str().c_str());
    Display();
}

void cWeatherOsd::DefineTokens(eMenus menu, skindesignerapi::cTokenContainer *tk) {
    if (menu == eMenus::root) {
        tk->DefineIntToken("{current}", (int)eRootMenuIT::current);
        tk->DefineIntToken("{iscurrent}", (int)eRootMenuIT::iscurrent);
        tk->DefineIntToken("{ishourly}", (int)eRootMenuIT::ishourly);
        tk->DefineIntToken("{isdaily}", (int)eRootMenuIT::isdaily);
        tk->DefineIntToken("{precipitationprobability}", (int)eRootMenuIT::precipitationprobability);
        tk->DefineIntToken("{humidity}", (int)eRootMenuIT::humidity);
        tk->DefineIntToken("{windbearing}", (int)eRootMenuIT::windbearing);
        tk->DefineIntToken("{cloudcover}", (int)eRootMenuIT::cloudcover);
        tk->DefineStringToken("{menuitemtext}", (int)eRootMenuST::menuitemtext);
        tk->DefineStringToken("{city}", (int)eRootMenuST::city);
        tk->DefineStringToken("{latitude}", (int)eRootMenuST::latitude);
        tk->DefineStringToken("{longitude}", (int)eRootMenuST::longitude);
        tk->DefineStringToken("{timestamp}", (int)eRootMenuST::timestamp);
        tk->DefineStringToken("{temperature}", (int)eRootMenuST::temperature);
        tk->DefineStringToken("{apparenttemperature}", (int)eRootMenuST::apparenttemperature);
        tk->DefineStringToken("{summary}", (int)eRootMenuST::summary);
        tk->DefineStringToken("{icon}", (int)eRootMenuST::icon);
        tk->DefineStringToken("{precipitationintensity}", (int)eRootMenuST::precipitationintensity);
        tk->DefineStringToken("{precipitationtype}", (int)eRootMenuST::precipitationtype);
        tk->DefineStringToken("{windspeed}", (int)eRootMenuST::windspeed);
        tk->DefineStringToken("{windbearingstring}", (int)eRootMenuST::windbearingstring);
        tk->DefineStringToken("{visibility}", (int)eRootMenuST::visibility);
        tk->DefineStringToken("{pressure}", (int)eRootMenuST::pressure);
        tk->DefineStringToken("{ozone}", (int)eRootMenuST::ozone);
        tk->DefineLoopToken("{hourly[num]}", (int)eForecastHourlyLT::num);
        tk->DefineLoopToken("{hourly[timestamp]}", (int)eForecastHourlyLT::timestamp);
        tk->DefineLoopToken("{hourly[temperature]}", (int)eForecastHourlyLT::temperature);
        tk->DefineLoopToken("{hourly[apparenttemperature]}", (int)eForecastHourlyLT::apparenttemperature);
        tk->DefineLoopToken("{hourly[summary]}", (int)eForecastHourlyLT::summary);
        tk->DefineLoopToken("{hourly[icon]}", (int)eForecastHourlyLT::icon);
        tk->DefineLoopToken("{hourly[precipitationintensity]}", (int)eForecastHourlyLT::precipitationintensity);
        tk->DefineLoopToken("{hourly[precipitationprobability]}", (int)eForecastHourlyLT::precipitationprobability);
        tk->DefineLoopToken("{hourly[precipitationtype]}", (int)eForecastHourlyLT::precipitationtype);
        tk->DefineLoopToken("{hourly[humidity]}", (int)eForecastHourlyLT::humidity);
        tk->DefineLoopToken("{hourly[windspeed]}", (int)eForecastHourlyLT::windspeed);
        tk->DefineLoopToken("{hourly[windbearing]}", (int)eForecastHourlyLT::windbearing);
        tk->DefineLoopToken("{hourly[windbearingstring]}", (int)eForecastHourlyLT::windbearingstring);
        tk->DefineLoopToken("{hourly[visibility]}", (int)eForecastHourlyLT::visibility);
        tk->DefineLoopToken("{hourly[cloudcover]}", (int)eForecastHourlyLT::cloudcover);
        tk->DefineLoopToken("{hourly[pressure]}", (int)eForecastHourlyLT::pressure);
        tk->DefineLoopToken("{hourly[ozone]}", (int)eForecastHourlyLT::ozone);
        tk->DefineLoopToken("{daily[day]}", (int)eForecastDailyLT::day);
        tk->DefineLoopToken("{daily[dayname]}", (int)eForecastDailyLT::dayname);
        tk->DefineLoopToken("{daily[temperaturemin]}", (int)eForecastDailyLT::temperaturemin);
        tk->DefineLoopToken("{daily[temperaturemintime]}", (int)eForecastDailyLT::temperaturemintime);
        tk->DefineLoopToken("{daily[temperaturemax]}", (int)eForecastDailyLT::temperaturemax);
        tk->DefineLoopToken("{daily[temperaturemaxtime]}", (int)eForecastDailyLT::temperaturemaxtime);
        tk->DefineLoopToken("{daily[summary]}", (int)eForecastDailyLT::summary);
        tk->DefineLoopToken("{daily[icon]}", (int)eForecastDailyLT::icon);
        tk->DefineLoopToken("{daily[precipitationintensity]}", (int)eForecastDailyLT::precipitationintensity);
        tk->DefineLoopToken("{daily[precipitationprobability]}", (int)eForecastDailyLT::precipitationprobability);
        tk->DefineLoopToken("{daily[precipitationtype]}", (int)eForecastDailyLT::precipitationtype);
        tk->DefineLoopToken("{daily[humidity]}", (int)eForecastDailyLT::humidity);
        tk->DefineLoopToken("{daily[windspeed]}", (int)eForecastDailyLT::windspeed);
        tk->DefineLoopToken("{daily[windbearing]}", (int)eForecastDailyLT::windbearing);
        tk->DefineLoopToken("{daily[windbearingstring]}", (int)eForecastDailyLT::windbearingstring);
        tk->DefineLoopToken("{daily[visibility]}", (int)eForecastDailyLT::visibility);
        tk->DefineLoopToken("{daily[cloudcover]}", (int)eForecastDailyLT::cloudcover);
        tk->DefineLoopToken("{daily[pressure]}", (int)eForecastDailyLT::pressure);
        tk->DefineLoopToken("{daily[ozone]}", (int)eForecastDailyLT::ozone);
    } else if (menu == eMenus::detailCurrent) {
        tk->DefineIntToken("{precipitationprobability}", (int)eDetailCurrentIT::precipitationprobability);
        tk->DefineIntToken("{humidity}", (int)eDetailCurrentIT::humidity);
        tk->DefineIntToken("{windbearing}", (int)eDetailCurrentIT::windbearing);
        tk->DefineIntToken("{cloudcover}", (int)eDetailCurrentIT::cloudcover);
        tk->DefineStringToken("{menuheader}", (int)eDetailCurrentST::menuheader);
        tk->DefineStringToken("{city}", (int)eDetailCurrentST::city);
        tk->DefineStringToken("{latitude}", (int)eDetailCurrentST::latitude);
        tk->DefineStringToken("{longitude}", (int)eDetailCurrentST::longitude);
        tk->DefineStringToken("{timestamp}", (int)eDetailCurrentST::timestamp);
        tk->DefineStringToken("{temperature}", (int)eDetailCurrentST::temperature);
        tk->DefineStringToken("{apparenttemperature}", (int)eDetailCurrentST::apparenttemperature);
        tk->DefineStringToken("{mintemperature}", (int)eDetailCurrentST::mintemperature);
        tk->DefineStringToken("{maxtemperature}", (int)eDetailCurrentST::maxtemperature);
        tk->DefineStringToken("{summary}", (int)eDetailCurrentST::summary);
        tk->DefineStringToken("{icon}", (int)eDetailCurrentST::icon);
        tk->DefineStringToken("{precipitationintensity}", (int)eDetailCurrentST::precipitationintensity);
        tk->DefineStringToken("{precipitationtype}", (int)eDetailCurrentST::precipitationtype);
        tk->DefineStringToken("{windspeed}", (int)eDetailCurrentST::windspeed);
        tk->DefineStringToken("{windbearingstring}", (int)eDetailCurrentST::windbearingstring);
        tk->DefineStringToken("{visibility}", (int)eDetailCurrentST::visibility);
        tk->DefineStringToken("{pressure}", (int)eDetailCurrentST::pressure);
        tk->DefineStringToken("{ozone}", (int)eDetailCurrentST::ozone);
    } else if (menu == eMenus::detailHourly) {
        tk->DefineStringToken("{menuheader}", (int)eDetailHourlyST::menuheader);
        tk->DefineStringToken("{summary}", (int)eDetailHourlyST::summary);
        tk->DefineStringToken("{icon}", (int)eDetailHourlyST::icon);
        tk->DefineStringToken("{city}", (int)eDetailHourlyST::city);
        tk->DefineStringToken("{latitude}", (int)eDetailHourlyST::latitude);
        tk->DefineStringToken("{longitude}", (int)eDetailHourlyST::longitude);
        tk->DefineLoopToken("{hourly[num]}", (int)eForecastHourlyLT::num);
        tk->DefineLoopToken("{hourly[timestamp]}", (int)eForecastHourlyLT::timestamp);
        tk->DefineLoopToken("{hourly[temperature]}", (int)eForecastHourlyLT::temperature);
        tk->DefineLoopToken("{hourly[apparenttemperature]}", (int)eForecastHourlyLT::apparenttemperature);
        tk->DefineLoopToken("{hourly[summary]}", (int)eForecastHourlyLT::summary);
        tk->DefineLoopToken("{hourly[icon]}", (int)eForecastHourlyLT::icon);
        tk->DefineLoopToken("{hourly[precipitationintensity]}", (int)eForecastHourlyLT::precipitationintensity);
        tk->DefineLoopToken("{hourly[precipitationprobability]}", (int)eForecastHourlyLT::precipitationprobability);
        tk->DefineLoopToken("{hourly[precipitationtype]}", (int)eForecastHourlyLT::precipitationtype);
        tk->DefineLoopToken("{hourly[humidity]}", (int)eForecastHourlyLT::humidity);
        tk->DefineLoopToken("{hourly[windspeed]}", (int)eForecastHourlyLT::windspeed);
        tk->DefineLoopToken("{hourly[windbearing]}", (int)eForecastHourlyLT::windbearing);
        tk->DefineLoopToken("{hourly[windbearingstring]}", (int)eForecastHourlyLT::windbearingstring);
        tk->DefineLoopToken("{hourly[visibility]}", (int)eForecastHourlyLT::visibility);
        tk->DefineLoopToken("{hourly[cloudcover]}", (int)eForecastHourlyLT::cloudcover);
        tk->DefineLoopToken("{hourly[pressure]}", (int)eForecastHourlyLT::pressure);
        tk->DefineLoopToken("{hourly[ozone]}", (int)eForecastHourlyLT::ozone);
    } else if (menu == eMenus::detailDaily) {
        tk->DefineStringToken("{menuheader}", (int)eDetailDailyST::menuheader);
        tk->DefineStringToken("{summary}", (int)eDetailDailyST::summary);
        tk->DefineStringToken("{icon}", (int)eDetailDailyST::icon);
        tk->DefineStringToken("{city}", (int)eDetailDailyST::city);
        tk->DefineStringToken("{latitude}", (int)eDetailDailyST::latitude);
        tk->DefineStringToken("{longitude}", (int)eDetailDailyST::longitude);
        tk->DefineLoopToken("{daily[day]}", (int)eForecastDailyLT::day);
        tk->DefineLoopToken("{daily[dayname]}", (int)eForecastDailyLT::dayname);
        tk->DefineLoopToken("{daily[temperaturemin]}", (int)eForecastDailyLT::temperaturemin);
        tk->DefineLoopToken("{daily[temperaturemintime]}", (int)eForecastDailyLT::temperaturemintime);
        tk->DefineLoopToken("{daily[temperaturemax]}", (int)eForecastDailyLT::temperaturemax);
        tk->DefineLoopToken("{daily[temperaturemaxtime]}", (int)eForecastDailyLT::temperaturemaxtime);
        tk->DefineLoopToken("{daily[summary]}", (int)eForecastDailyLT::summary);
        tk->DefineLoopToken("{daily[icon]}", (int)eForecastDailyLT::icon);
        tk->DefineLoopToken("{daily[precipitationintensity]}", (int)eForecastDailyLT::precipitationintensity);
        tk->DefineLoopToken("{daily[precipitationprobability]}", (int)eForecastDailyLT::precipitationprobability);
        tk->DefineLoopToken("{daily[precipitationtype]}", (int)eForecastDailyLT::precipitationtype);
        tk->DefineLoopToken("{daily[humidity]}", (int)eForecastDailyLT::humidity);
        tk->DefineLoopToken("{daily[windspeed]}", (int)eForecastDailyLT::windspeed);
        tk->DefineLoopToken("{daily[windbearing]}", (int)eForecastDailyLT::windbearing);
        tk->DefineLoopToken("{daily[windbearingstring]}", (int)eForecastDailyLT::windbearingstring);
        tk->DefineLoopToken("{daily[visibility]}", (int)eForecastDailyLT::visibility);
        tk->DefineLoopToken("{daily[cloudcover]}", (int)eForecastDailyLT::cloudcover);
        tk->DefineLoopToken("{daily[pressure]}", (int)eForecastDailyLT::pressure);
        tk->DefineLoopToken("{daily[ozone]}", (int)eForecastDailyLT::ozone);
    }
}
