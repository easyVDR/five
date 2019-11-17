#include <vdr/plugin.h>
#include <jansson.h>
#include "../tools/curlfuncs.h"
#include "../tools/jsonhelpers.h"
#include "locator.h"

using namespace std;

cForecastLocator::cForecastLocator(void) {
    urlIPLApi = "http://ip-api.com/json";
    city = "";
    lat = 0.0;
    lon = 0.0;
}

cForecastLocator::~cForecastLocator() {

}

bool cForecastLocator::ReadLocationByIP(void) {
    string outputJson;
    CurlGetUrl(urlIPLApi.c_str(), &outputJson);
    
    json_t *root;
    json_error_t error;

    root = json_loads(outputJson.c_str(), 0, &error);
    if ( !root ) {
        return false;
    }
    if ( !json_is_object(root) ) {
        return false;
    }

    city    = JsonGetString(root, "city");
    lat     = JsonGetFloat(root, "lat");
    lon     = JsonGetFloat(root, "lon");

    dsyslog("weatherforecast: location read from IP: city \"%s\", latitude: %f, longitude: %f", city.c_str(), lat, lon);
    
    return true;
}

void cForecastLocator::WriteToSetup(cPlugin *weatherPlug) {
    //save to setup
    weatherPlug->SetupStore("city", city.c_str());
    weatherPlug->SetupStore("lat", FloatToString(lat).c_str());
    weatherPlug->SetupStore("lon", FloatToString(lon).c_str());
    //set values in plugin config
    weatherPlug->SetupParse("city", city.c_str());
    weatherPlug->SetupParse("lat", FloatToString(lat).c_str());
    weatherPlug->SetupParse("lon", FloatToString(lon).c_str());
}

