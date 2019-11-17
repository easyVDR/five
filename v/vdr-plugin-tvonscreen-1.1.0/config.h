/*
 * config.h
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef TVONSCREEN_CONFIG_H
#define TVONSCREEN_CONFIG_H

#include <vdr/config.h>
#include <vdr/menuitems.h>

#if defined(MAINMENUHOOKSVERSNUM) & !defined(MAINMENUHOOKSVERSION)
#undef  MAINMENUHOOKSVERSNUM
#define MAINMENUHOOKSVERSNUM 10000
#endif

class tvonscreenConfig
{
public:

    tvonscreenConfig(void);
    bool SetupParse(const char *Name, const char *Value);
    bool ProcessArgs(int argc, char *argv[]);
    const char *CommandLineHelp(void);

    int showLogos;
    int noInfoLine;
    int showChannels;
    int bwlogos;
    int usertime1;
    int usertime2;
    int usertime3;
    int thenshownextday;
    int showsearchinitiator;
    int fontdsize;
    int transparency;
#if MAINMENUHOOKSVERSNUM == 10001
    int replaceorgschedule;
#endif
    int onlyminutes;

    char logos[PATH_MAX];
    char vdradminfile[PATH_MAX];
};

extern tvonscreenConfig tvonscreenCfg;

class tvonscreenConfigPage : public cMenuSetupPage
{
private:
    tvonscreenConfig m_NewConfig;

protected:
    virtual void Store(void);

public:
    tvonscreenConfigPage(void);
    virtual ~tvonscreenConfigPage();
};

#endif


