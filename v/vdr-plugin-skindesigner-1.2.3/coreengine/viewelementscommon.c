#include "viewelementscommon.h"
#include "../config.h"
#include "../services/weatherforecast.h"

/******************************************************************
* cVeDateTime
******************************************************************/
cVeDateTime::cVeDateTime(void) {
    lastMinute = -1;
}

cVeDateTime::~cVeDateTime(void) {
}

void cVeDateTime::Close(void) {
    lastMinute = -1;
    cViewElement::Close();
}

void cVeDateTime::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{time}", (int)eDateTimeST::time);
    tokenContainer->DefineStringToken("{dayleadingzero}", (int)eDateTimeST::dayleadingzero);
    tokenContainer->DefineStringToken("{dayname}", (int)eDateTimeST::dayname);
    tokenContainer->DefineStringToken("{daynameshort}", (int)eDateTimeST::daynameshort);
    tokenContainer->DefineStringToken("{month}", (int)eDateTimeST::month);
    tokenContainer->DefineStringToken("{monthname}", (int)eDateTimeST::monthname);
    tokenContainer->DefineStringToken("{monthnameshort}", (int)eDateTimeST::monthnameshort);
    tokenContainer->DefineIntToken("{day}", (int)eDateTimeIT::day);
    tokenContainer->DefineIntToken("{year}", (int)eDateTimeIT::year);
    InheritTokenContainer();
}

bool cVeDateTime::Parse(bool forced) {
    if (!cViewElement::Parse(forced))
        return false;
    time_t t = time(0);   // get time now
    struct tm * now = localtime(&t);
    int min = now->tm_min;
    if (!forced && min == lastMinute) {
        return false;
    }
    lastMinute = min;
    SetDirty();

    tokenContainer->Clear();
    tokenContainer->AddIntToken((int)eDateTimeIT::year, now->tm_year + 1900);
    tokenContainer->AddIntToken((int)eDateTimeIT::day, now->tm_mday);

    char monthname[20];
    char monthshort[10];
    strftime(monthshort, sizeof(monthshort), "%b", now);
    strftime(monthname, sizeof(monthname), "%B", now);

    tokenContainer->AddStringToken((int)eDateTimeST::monthname, monthname);
    tokenContainer->AddStringToken((int)eDateTimeST::monthnameshort, monthshort);
    tokenContainer->AddStringToken((int)eDateTimeST::month, *cString::sprintf("%02d", now->tm_mon + 1));
    tokenContainer->AddStringToken((int)eDateTimeST::dayleadingzero, *cString::sprintf("%02d", now->tm_mday));
    tokenContainer->AddStringToken((int)eDateTimeST::dayname, *WeekDayNameFull(now->tm_wday));
    tokenContainer->AddStringToken((int)eDateTimeST::daynameshort, *WeekDayName(now->tm_wday));
    tokenContainer->AddStringToken((int)eDateTimeST::time, *TimeString(t));
    return true;
}

/******************************************************************
* cVeTime
******************************************************************/
cVeTime::cVeTime(void) {
    lastSecond = -1;
}

cVeTime::~cVeTime(void) {
}

void cVeTime::Close(void) {
    lastSecond = -1;
    cViewElement::Close();
}

void cVeTime::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineIntToken("{sec}", (int)eTimeIT::sec);
    tokenContainer->DefineIntToken("{min}", (int)eTimeIT::min);
    tokenContainer->DefineIntToken("{hour}", (int)eTimeIT::hour);
    tokenContainer->DefineIntToken("{hmins}", (int)eTimeIT::hmins);
    InheritTokenContainer();
}

bool cVeTime::Parse(bool forced) {
    if (!cViewElement::Parse(forced))
        return false;
    time_t t = time(0);   // get time now
    struct tm * now = localtime(&t);
    int sec = now->tm_sec;
    if (!forced && sec == lastSecond) {
        return false;
    }
    lastSecond = sec;
    SetDirty();

    int min = now->tm_min;
    int hour = now->tm_hour;
    int hourMinutes = hour%12 * 5 + min / 12;

    tokenContainer->Clear();
    tokenContainer->AddIntToken((int)eTimeIT::sec, sec);
    tokenContainer->AddIntToken((int)eTimeIT::min, min);
    tokenContainer->AddIntToken((int)eTimeIT::hour, hour);
    tokenContainer->AddIntToken((int)eTimeIT::hmins, hourMinutes);
    return true;
}
/******************************************************************
* cVeMessage
******************************************************************/
cVeMessage::cVeMessage(void) {
    changed = false;
    text = NULL;
    type = mtStatus;
}

cVeMessage::~cVeMessage(void) {
    free(text);
}

void cVeMessage::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{text}", (int)eMessageST::text);
    tokenContainer->DefineIntToken("{status}", (int)eMessageIT::status);
    tokenContainer->DefineIntToken("{info}", (int)eMessageIT::info);
    tokenContainer->DefineIntToken("{warning}", (int)eMessageIT::warning);
    tokenContainer->DefineIntToken("{error}", (int)eMessageIT::error);
    InheritTokenContainer();
}

void cVeMessage::Set(eMessageType type, const char *text) {
    if (!text)
        return;
    free(this->text);
    this->text = strdup(text);
    this->type = type; 
    changed = true;
}

bool cVeMessage::Parse(bool forced) {
    if (!cViewElement::Parse(forced) || !changed)
        return false;

    tokenContainer->Clear();
    tokenContainer->AddIntToken((int)eMessageIT::status, (type == mtStatus)  ? true : false);
    tokenContainer->AddIntToken((int)eMessageIT::info, (type == mtInfo)  ? true : false);
    tokenContainer->AddIntToken((int)eMessageIT::warning, (type == mtWarning)  ? true : false);
    tokenContainer->AddIntToken((int)eMessageIT::error, (type == mtError)  ? true : false);
    tokenContainer->AddStringToken((int)eMessageST::text, text);
    SetDirty();
    changed = false;
    return true;
}

/******************************************************************
* cVeDevices
******************************************************************/
cVeDevices::cVeDevices(void) {
    lastRefresh = 0;
    initial = true;
    devicesIndex = -1;
    lastSignalStrength = NULL;
    lastSignalQuality = NULL;
    recDevices = NULL;
}

cVeDevices::~cVeDevices(void) {
    mutexDevices.Lock();
    delete[] lastSignalStrength;
    lastSignalStrength = NULL;
    delete[] lastSignalQuality;
    lastSignalQuality = NULL;
    delete[] recDevices;
    recDevices = NULL;
    mutexDevices.Unlock();
}

void cVeDevices::Init(void) {
    light = (attribs->Mode() == (int)eViewElementMode::light) ? true : false;  
    int totalDevices = cDevice::NumDevices();
    for (int i = 0; i < totalDevices; i++) {
        const cDevice *device = cDevice::GetDevice(i);
        if (!device || !device->NumProvidedSystems()) {
            continue;
        }
        devices.push_back(i);
    }
    numDevices = devices.size();    
    lastSignalStrength = new int[numDevices];
    lastSignalQuality = new int[numDevices];
    recDevices = new bool[numDevices];
    mutexDevices.Lock();
    for (int i=0; i<numDevices; i++) {
        lastSignalStrength[i] = 0;
        lastSignalQuality[i] = 0;
        recDevices[i] = false;
    }
    mutexDevices.Unlock();
}

void cVeDevices::Close(void) {
    devices.clear();
    initial = true;
    lastRefresh = 0;
    numDevices = 0;
    mutexDevices.Lock();
    delete[] lastSignalStrength;
    lastSignalStrength = NULL;
    delete[] lastSignalQuality;
    lastSignalQuality = NULL;
    delete[] recDevices;
    recDevices = NULL;
    mutexDevices.Unlock();
    cViewElement::Close();
}

void cVeDevices::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineIntToken("{numdevices}", (int)eDevicesIT::numdevices);
    tokenContainer->DefineLoopToken("{devices[num]}", (int)eDevicesLT::num);
    tokenContainer->DefineLoopToken("{devices[type]}", (int)eDevicesLT::type);
    tokenContainer->DefineLoopToken("{devices[istuned]}", (int)eDevicesLT::istuned);
    tokenContainer->DefineLoopToken("{devices[livetv]}", (int)eDevicesLT::livetv);
    tokenContainer->DefineLoopToken("{devices[recording]}", (int)eDevicesLT::recording);
    tokenContainer->DefineLoopToken("{devices[hascam]}", (int)eDevicesLT::hascam);
    tokenContainer->DefineLoopToken("{devices[cam]}", (int)eDevicesLT::cam);
    tokenContainer->DefineLoopToken("{devices[signalstrength]}", (int)eDevicesLT::signalstrength);
    tokenContainer->DefineLoopToken("{devices[signalquality]}", (int)eDevicesLT::signalquality);
    tokenContainer->DefineLoopToken("{devices[channelnumber]}", (int)eDevicesLT::channelnumber);
    tokenContainer->DefineLoopToken("{devices[channelname]}", (int)eDevicesLT::channelname);
    tokenContainer->DefineLoopToken("{devices[channellogoexists]}", (int)eDevicesLT::channellogoexists);
    tokenContainer->DefineLoopToken("{devices[channelid]}", (int)eDevicesLT::channelid);
    tokenContainer->DefineLoopToken("{devices[source]}", (int)eDevicesLT::source);

    devicesIndex = tokenContainer->LoopIndex("devices");
    InheritTokenContainer();
}

bool cVeDevices::Parse(bool forced) {
    if (!cViewElement::Parse(forced))
        return false;
    if (initial) {
        Init();
        initial = false;
    } else {
        //in light modus content is static
        if (light)
            return false;
        //check if drawing is necessary
        if (cTimeMs::Now() - lastRefresh < 500)
            return false;
        bool changed = false;
        for (int i = 0; i < numDevices; i++) {
            const cDevice *device = cDevice::GetDevice(devices[i]);
            if (!device || !device->NumProvidedSystems()) {
                continue;
            }
            int signalStrength = device->SignalStrength();
            int signalQuality = device->SignalQuality();
            int lastSigStr = 0;
            int lastSigQual = 0;
            mutexDevices.Lock();
            if (lastSignalStrength && lastSignalQuality) {
                lastSigStr = lastSignalStrength[i];
                lastSigQual = lastSignalQuality[i];
            }
            mutexDevices.Unlock();
            if ((signalStrength != lastSigStr) || (signalQuality != lastSigQual)) {
                changed = true;
                break;
            }            
        }
        if (!changed) {
            return false;
        }
    }

    //check device which currently displays live tv
    int deviceLiveTV = -1;
    cDevice *primaryDevice = cDevice::PrimaryDevice();
    if (primaryDevice) {
        if (!primaryDevice->Replaying() || primaryDevice->Transferring())
            deviceLiveTV = cDevice::ActualDevice()->DeviceNumber();
        else
            deviceLiveTV = primaryDevice->DeviceNumber();
    }
    // check currently recording devices
    // BLOCK for LOCK_TIMERS_READ scope !!
    {
#if defined (APIVERSNUM) && (APIVERSNUM >= 20301)
        LOCK_TIMERS_READ;
        const cTimers* timers = Timers;
#else
        const cTimers* timers = &Timers;
#endif
        for (const cTimer *timer = timers->First(); timer; timer = timers->Next(timer)) {
            if (!timer->Recording()) {
                continue;
            }
            if (cRecordControl *RecordControl = cRecordControls::GetRecordControl(timer)) {
                const cDevice *recDevice = RecordControl->Device();
                if (recDevice) {
                    mutexDevices.Lock();
                    if (recDevices)
                        recDevices[recDevice->DeviceNumber()] = true;
                    mutexDevices.Unlock();
                }
            }
        }
    }

    //create loop container
    tokenContainer->Clear();
    
    vector<int> loopInfo;
    loopInfo.push_back(numDevices);
    tokenContainer->CreateLoopTokenContainer(&loopInfo);
    tokenContainer->AddIntToken((int)eDevicesIT::numdevices, numDevices);

    for (int i = 0; i < numDevices; i++) {
        const cDevice *device = cDevice::GetDevice(devices[i]);
        if (!device || !device->NumProvidedSystems()) {
            continue;
        }
        tokenContainer->AddLoopToken(devicesIndex, i, (int)eDevicesLT::num, *cString::sprintf("%d", i));
        tokenContainer->AddLoopToken(devicesIndex, i, (int)eDevicesLT::type, *(device->DeviceType()));

        cCamSlot *camSlot = device->CamSlot();
        int camNumber = -1;
        if (camSlot) {
            camNumber = camSlot->SlotNumber();
            tokenContainer->AddLoopToken(devicesIndex, i, (int)eDevicesLT::hascam, "1");
        } else {
            tokenContainer->AddLoopToken(devicesIndex, i, (int)eDevicesLT::hascam, "0");
        }
        tokenContainer->AddLoopToken(devicesIndex, i, (int)eDevicesLT::cam, *cString::sprintf("%d", camNumber));

        int signalStrength = 0;
        int signalQuality = 0;
        if (!light) {
            signalStrength = device->SignalStrength();
            signalQuality = device->SignalQuality();
        }
        tokenContainer->AddLoopToken(devicesIndex, i, (int)eDevicesLT::signalstrength, *cString::sprintf("%d", signalStrength));
        tokenContainer->AddLoopToken(devicesIndex, i, (int)eDevicesLT::signalquality, *cString::sprintf("%d", signalQuality));
        tokenContainer->AddLoopToken(devicesIndex, i, (int)eDevicesLT::livetv, i == deviceLiveTV ? "1" : "0");

        bool isRecording = false;
        mutexDevices.Lock();
        if (recDevices && recDevices[i])
            isRecording = true;
        mutexDevices.Unlock();
        tokenContainer->AddLoopToken(devicesIndex, i, (int)eDevicesLT::recording, isRecording ? "1" : "0");

        const cChannel *channel = device->GetCurrentlyTunedTransponder();
        const cSource *source = (channel) ? Sources.Get(channel->Source()) : NULL;
        if (channel && channel->Number() > 0) {
            tokenContainer->AddLoopToken(devicesIndex, i, (int)eDevicesLT::channelnumber, *cString::sprintf("%d", channel->Number()));
            tokenContainer->AddLoopToken(devicesIndex, i, (int)eDevicesLT::channelname, channel->Name());
            string channelID = *(channel->GetChannelID().ToString());
            tokenContainer->AddLoopToken(devicesIndex, i, (int)eDevicesLT::channelid, channelID.c_str());
            tokenContainer->AddLoopToken(devicesIndex, i, (int)eDevicesLT::channellogoexists, imgCache->LogoExists(channelID) ? "1" : "0");
            tokenContainer->AddLoopToken(devicesIndex, i, (int)eDevicesLT::istuned, "1");
        } else {
            tokenContainer->AddLoopToken(devicesIndex, i, (int)eDevicesLT::istuned, "0");
        }
    
        tokenContainer->AddLoopToken(devicesIndex, i, (int)eDevicesLT::source, source ? source->Description() : "");

        mutexDevices.Lock();
        if (lastSignalStrength)
            lastSignalStrength[i] = signalStrength;
        if (lastSignalQuality)
            lastSignalQuality[i] = signalQuality;
        mutexDevices.Unlock();
    }

    SetDirty();
    lastRefresh = cTimeMs::Now();
    return true;
}
/******************************************************************
* cVeCurrentWeather
******************************************************************/
cVeCurrentWeather::cVeCurrentWeather(void) {
}

cVeCurrentWeather::~cVeCurrentWeather(void) {
}

void cVeCurrentWeather::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineStringToken("{timestamp}", (int)eCurrentWeatherST::timestamp);
    tokenContainer->DefineStringToken("{summary}", (int)eCurrentWeatherST::summary);
    tokenContainer->DefineStringToken("{icon}", (int)eCurrentWeatherST::icon);
    tokenContainer->DefineStringToken("{precipitationtype}", (int)eCurrentWeatherST::precipitationtype);
    tokenContainer->DefineStringToken("{windbearingstring}", (int)eCurrentWeatherST::windbearingstring);
    tokenContainer->DefineStringToken("{temperature}", (int)eCurrentWeatherST::temperature);
    tokenContainer->DefineStringToken("{apparenttemperature}", (int)eCurrentWeatherST::apparenttemperature);
    tokenContainer->DefineStringToken("{mintemperature}", (int)eCurrentWeatherST::mintemperature);
    tokenContainer->DefineStringToken("{maxtemperature}", (int)eCurrentWeatherST::maxtemperature);
    tokenContainer->DefineStringToken("{precipitationintensity}", (int)eCurrentWeatherST::precipitationintensity);
    tokenContainer->DefineStringToken("{windspeed}", (int)eCurrentWeatherST::windspeed);
    tokenContainer->DefineStringToken("{visibility}", (int)eCurrentWeatherST::visibility);
    tokenContainer->DefineStringToken("{pressure}", (int)eCurrentWeatherST::pressure);
    tokenContainer->DefineStringToken("{ozone}", (int)eCurrentWeatherST::ozone);
    tokenContainer->DefineIntToken("{precipitationprobability}", (int)eCurrentWeatherIT::precipitationprobability);
    tokenContainer->DefineIntToken("{humidity}", (int)eCurrentWeatherIT::humidity);
    tokenContainer->DefineIntToken("{windbearing}", (int)eCurrentWeatherIT::windbearing);
    tokenContainer->DefineIntToken("{cloudcover}", (int)eCurrentWeatherIT::cloudcover);
    InheritTokenContainer();
}

bool cVeCurrentWeather::Parse(bool forced) {
    if (!cViewElement::Parse(forced))
        return false;
    static cPlugin *pWeatherForecast = cPluginManager::GetPlugin("weatherforecast");
    if (!pWeatherForecast)
        return false;
    cServiceCurrentWeather currentWeather;
    if (!pWeatherForecast->Service("GetCurrentWeather", &currentWeather)) {
        return false;
    }
    SetDirty();
    tokenContainer->Clear();
    tokenContainer->AddStringToken((int)eCurrentWeatherST::timestamp, currentWeather.timeStamp.c_str());
    tokenContainer->AddStringToken((int)eCurrentWeatherST::temperature, currentWeather.temperature.c_str());
    tokenContainer->AddStringToken((int)eCurrentWeatherST::apparenttemperature, currentWeather.apparentTemperature.c_str());
    tokenContainer->AddStringToken((int)eCurrentWeatherST::mintemperature, currentWeather.minTemperature.c_str());
    tokenContainer->AddStringToken((int)eCurrentWeatherST::maxtemperature, currentWeather.maxTemperature.c_str());
    tokenContainer->AddStringToken((int)eCurrentWeatherST::summary, currentWeather.summary.c_str());
    tokenContainer->AddStringToken((int)eCurrentWeatherST::icon, currentWeather.icon.c_str());
    tokenContainer->AddStringToken((int)eCurrentWeatherST::precipitationintensity, currentWeather.precipitationIntensity.c_str());
    tokenContainer->AddStringToken((int)eCurrentWeatherST::precipitationtype, currentWeather.precipitationType.c_str());
    tokenContainer->AddStringToken((int)eCurrentWeatherST::windspeed, currentWeather.windSpeed.c_str());
    tokenContainer->AddStringToken((int)eCurrentWeatherST::windbearingstring, currentWeather.windBearingString.c_str());
    tokenContainer->AddStringToken((int)eCurrentWeatherST::visibility, currentWeather.visibility.c_str());
    tokenContainer->AddStringToken((int)eCurrentWeatherST::pressure, currentWeather.pressure.c_str());
    tokenContainer->AddStringToken((int)eCurrentWeatherST::ozone, currentWeather.ozone.c_str());
    tokenContainer->AddIntToken((int)eCurrentWeatherIT::precipitationprobability, currentWeather.precipitationProbability);
    tokenContainer->AddIntToken((int)eCurrentWeatherIT::humidity, currentWeather.humidity);
    tokenContainer->AddIntToken((int)eCurrentWeatherIT::windbearing, currentWeather.windBearing);
    tokenContainer->AddIntToken((int)eCurrentWeatherIT::cloudcover, currentWeather.cloudCover);
    return true;
}
/******************************************************************
* cVeCustomTokens
******************************************************************/
cVeCustomTokens::cVeCustomTokens(void) {
}

cVeCustomTokens::~cVeCustomTokens(void) {
}

void cVeCustomTokens::Reset(void) {
    globals->ResetCustomTokenChange();
}

void cVeCustomTokens::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    int numTokens = config.numCustomTokens;
    for (int i=0; i < numTokens; i++) {
        cString nameIntToken = cString::sprintf("{customint%d}", i+1);
        cString nameStringToken = cString::sprintf("{customstring%d}", i+1);
        tokenContainer->DefineStringToken(*nameStringToken, i);
        tokenContainer->DefineIntToken(*nameIntToken, i);
    }
    InheritTokenContainer();
}

bool cVeCustomTokens::Parse(bool forced) {
    if (!cViewElement::Parse(forced))
        return false;

    if (!globals->CustomTokenChange())
        return false;

    tokenContainer->Clear();
    map <int, string> customStringTokens = globals->GetCustomStringTokens();
    for (map<int, string>::iterator it = customStringTokens.begin(); it != customStringTokens.end(); it++) {
        int index = it->first;
        if (index > 0 && index <= config.numCustomTokens) {
            tokenContainer->AddStringToken(index-1, it->second.c_str());
        }
    }
    map <int, int> customIntTokkens = globals-> GetCustomIntTokens();
    for (map<int, int>::iterator it = customIntTokkens.begin(); it != customIntTokkens.end(); it++) {
        int index = it->first;
        if (index > 0 && index <= config.numCustomTokens) {
            tokenContainer->AddIntToken(index-1, it->second);
        }
    }
    SetDirty();
    return true;
}

/******************************************************************
* cVeVolume
******************************************************************/
cVeVolume::cVeVolume(void) {
    current = 0;
    total = 0;
    mute = false;
    changed = true;
}

cVeVolume::~cVeVolume(void) {
}

void cVeVolume::SetTokenContainer(void) {
    tokenContainer = new skindesignerapi::cTokenContainer();
    tokenContainer->DefineIntToken("{volume}", (int)eDVVolumeIT::volume);
    tokenContainer->DefineIntToken("{volpercent}", (int)eDVVolumeIT::volpercent);
    tokenContainer->DefineIntToken("{maxvolume}", (int)eDVVolumeIT::maxvolume);
    tokenContainer->DefineIntToken("{mute}", (int)eDVVolumeIT::mute);
    InheritTokenContainer();
}

void cVeVolume::Set(int current, int total, bool mute) {
    this->current = current;
    this->total = total;
    this->mute = mute;
    changed = true;
}

bool cVeVolume::Parse(bool forced) {
    if (!cViewElement::Parse(forced) || !changed)
        return false;
    tokenContainer->Clear();
    tokenContainer->AddIntToken((int)eDVVolumeIT::volume, current);
    tokenContainer->AddIntToken((int)eDVVolumeIT::maxvolume, total);
    tokenContainer->AddIntToken((int)eDVVolumeIT::volpercent, (double)current *100 / (double)total);
    tokenContainer->AddIntToken((int)eDVVolumeIT::mute, mute);
    changed = false;
    SetDirty();
    return true;
}