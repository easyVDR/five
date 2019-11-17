/*
 * weatherforecast.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include <vdr/plugin.h>
#include <libskindesignerapi/skindesignerapi.h>
#include "config.h"
#include "setup.h"
#include "libforecastio/forecastio.h"
#include "libforecastio/locator.h"
#include "weatherosd.h"
#include "services.h"

//***************************************************************************
// Constants
//***************************************************************************
static const char *VERSION        = "0.2.0";
static const char *DESCRIPTION    = "Weatherforecast based on forecast.io";

//***************************************************************************
// Globals
//***************************************************************************
cWeatherforecastConfig weatherConfig;

//***************************************************************************
// cPluginWeatherforecast
//***************************************************************************
class cPluginWeatherforecast : public cPlugin {
private:
    skindesignerapi::cPluginStructure *plugStruct;
    cForecastIO *forecastIO;
public:
    cPluginWeatherforecast(void);
    virtual ~cPluginWeatherforecast();
    virtual const char *Version(void) { return VERSION; }
    virtual const char *Description(void) { return DESCRIPTION; }
    virtual const char *CommandLineHelp(void);
    virtual bool ProcessArgs(int argc, char *argv[]);
    virtual bool Initialize(void);
    virtual bool Start(void);
    virtual void Stop(void);
    virtual void Housekeeping(void);
    virtual void MainThreadHook(void);
    virtual cString Active(void);
    virtual time_t WakeupTime(void);
    virtual const char *MainMenuEntry(void) { return tr("WeatherForecast"); }
    virtual cOsdObject *MainMenuAction(void);
    virtual cMenuSetupPage *SetupMenu(void);
    virtual bool SetupParse(const char *Name, const char *Value);
    virtual bool Service(const char *Id, void *Data = NULL);
    virtual const char **SVDRPHelpPages(void);
    virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
};

cPluginWeatherforecast::cPluginWeatherforecast(void) {
    plugStruct = NULL;
    forecastIO = NULL;
}

cPluginWeatherforecast::~cPluginWeatherforecast() {
    delete forecastIO;
    delete plugStruct;
}

const char *cPluginWeatherforecast::CommandLineHelp(void) {
    return NULL;
}

bool cPluginWeatherforecast::ProcessArgs(int argc, char *argv[]) {
    return true;
}

bool cPluginWeatherforecast::Initialize(void) {
    //check geolocation on first start
    if (weatherConfig.city.size() == 0) {
        dsyslog("weatherforecast: location not set, setting from own public IP");
        cForecastLocator locator;
        if (locator.ReadLocationByIP()) {
            locator.WriteToSetup(this);
        }
        dsyslog("weatherforecast: location wrote to setup");
    }
    return true;
}

bool cPluginWeatherforecast::Start(void) {
    string cacheDir = cPlugin::CacheDirectory(PLUGIN_NAME_I18N);
    forecastIO = new cForecastIO(cacheDir);
    forecastIO->Start();
    
    plugStruct = new skindesignerapi::cPluginStructure();
    plugStruct->name = "weatherforecast";
    plugStruct->libskindesignerAPIVersion = LIBSKINDESIGNERAPIVERSION;

    skindesignerapi::cTokenContainer *tkMenuRoot = new skindesignerapi::cTokenContainer();
    cWeatherOsd::DefineTokens(eMenus::root, tkMenuRoot);
    plugStruct->RegisterMenu((int)eMenus::root, skindesignerapi::mtList, "weatherforecast.xml", tkMenuRoot);

    skindesignerapi::cTokenContainer *tkDetailCurrent = new skindesignerapi::cTokenContainer();
    cWeatherOsd::DefineTokens(eMenus::detailCurrent, tkDetailCurrent);
    plugStruct->RegisterMenu((int)eMenus::detailCurrent, skindesignerapi::mtText, "weatherforecastdetailcurrent.xml", tkDetailCurrent);

    skindesignerapi::cTokenContainer *tkDetailHourly = new skindesignerapi::cTokenContainer();
    cWeatherOsd::DefineTokens(eMenus::detailHourly, tkDetailHourly);
    plugStruct->RegisterMenu((int)eMenus::detailHourly, skindesignerapi::mtText, "weatherforecastdetailhourly.xml", tkDetailHourly);

    skindesignerapi::cTokenContainer *tkDetailDaily = new skindesignerapi::cTokenContainer();
    cWeatherOsd::DefineTokens(eMenus::detailDaily, tkDetailDaily);
    plugStruct->RegisterMenu((int)eMenus::detailDaily, skindesignerapi::mtText, "weatherforecastdetaildaily.xml", tkDetailDaily);

    if (!skindesignerapi::SkindesignerAPI::RegisterPlugin(plugStruct)) {
        esyslog("weatherforecast: skindesigner not available");
    } else {
        dsyslog("weatherforecast: successfully registered at skindesigner, id %d", plugStruct->id);
    }
    return true;
}

void cPluginWeatherforecast::Stop(void) {
    if (forecastIO) {
        forecastIO->Stop();
    }
}

void cPluginWeatherforecast::Housekeeping(void) {

}

void cPluginWeatherforecast::MainThreadHook(void) {

}

cString cPluginWeatherforecast::Active(void) {
    return NULL;
}

time_t cPluginWeatherforecast::WakeupTime(void) {
    return 0;
}

cOsdObject *cPluginWeatherforecast::MainMenuAction(void) {
    cWeatherOsd *menu = new cWeatherOsd(forecastIO, plugStruct);
    return menu;
}

cMenuSetupPage *cPluginWeatherforecast::SetupMenu(void) {
    return new cWeatherforecastSetup();
}

bool cPluginWeatherforecast::SetupParse(const char *Name, const char *Value) {
    return weatherConfig.SetupParse(Name, Value);
}

bool cPluginWeatherforecast::Service(const char *Id, void *Data) {
    if (Data == NULL)
        return false;
    if (strcmp(Id, "GetCurrentWeather") == 0) {
        cServiceCurrentWeather* call = (cServiceCurrentWeather*) Data;
        bool ok = false;
        forecastIO->LockForecasts();
        ok = forecastIO->SetCurrentWeather(call);
        forecastIO->UnlockForecasts();
        return ok;
    }
    return false;
}

const char **cPluginWeatherforecast::SVDRPHelpPages(void) {
    return NULL;
}

cString cPluginWeatherforecast::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode) {
    return NULL;
}

VDRPLUGINCREATOR(cPluginWeatherforecast); // Don't touch this!
