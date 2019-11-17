/*
 * config.c
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include "config.h"
#include "magazine.h"

#include <getopt.h>
#include <unistd.h>
#include <vdr/menuitems.h>

tvonscreenConfig tvonscreenCfg;

tvonscreenConfig::tvonscreenConfig(void)
{
    showLogos=false;
    noInfoLine=false;
    showChannels=true;
    bwlogos=false;

    usertime1=1700;
    usertime2=2000;
    usertime3=2300;
    thenshownextday=true;
    showsearchinitiator=true;
    fontdsize=0;
    transparency=100;
#if MAINMENUHOOKSVERSNUM == 10001
    replaceorgschedule=false;
#endif
    onlyminutes=false;

    memset(logos,0,sizeof(logos));
    memset(vdradminfile,0,sizeof(vdradminfile));
}

bool tvonscreenConfig::SetupParse(const char *Name, const char *Value)
{
    if      (strcmp(Name,"showLogos")==0) showLogos = atoi(Value);
    else if (strcmp(Name,"noInfoLine")==0) noInfoLine = atoi(Value);
    else if (strcmp(Name,"showChannels")==0) showChannels = atoi(Value);
    else if (strcmp(Name,"bwlogos")==0) bwlogos = atoi(Value);
    else if (strcmp(Name,"usertime1")==0) usertime1 = atoi(Value);
    else if (strcmp(Name,"usertime2")==0) usertime2 = atoi(Value);
    else if (strcmp(Name,"usertime3")==0) usertime3 = atoi(Value);
    else if (strcmp(Name,"thenshownextday")==0) thenshownextday = atoi(Value);
    else if (strcmp(Name,"showsearchinitiator")==0) showsearchinitiator = atoi(Value);
    else if (strcmp(Name,"fontdsize")==0) fontdsize = atoi(Value);
    else if (strcmp(Name,"transparency")==0) transparency = atoi(Value);
    else if (strcmp(Name,"onlyminutes")==0) onlyminutes = atoi(Value);
#if MAINMENUHOOKSVERSNUM == 10001
    else if (strcmp(Name,"replaceorgschedule")==0) replaceorgschedule = atoi(Value);
#endif
    else
        return false;

    return true;
}
const char *tvonscreenConfig::CommandLineHelp(void)
{
    // Return a string that describes all known command line options.
    return "  -l PathToLogos\n  --logos=PathToLogos\n  -v vdradmind.at\n  --vdradminfile=vdradmind.at\n";
}

bool tvonscreenConfig::ProcessArgs(int argc, char *argv[])
{
    static struct option long_options[] =
    {
        { "logos",       required_argument, NULL, 'l'
        },
        { "vdradminfile",required_argument, NULL, 'v' },
        { NULL, 0, NULL, 0 }
    };

    bool retval=true;
    int c;
    while ((c = getopt_long(argc, argv, "l:v:", long_options, NULL)) != -1)
    {
        switch (c)
        {
        case 'l':
            strncpy(logos,optarg,sizeof(logos)-1);
            logos[sizeof(logos)-1]=0;
            retval=true;
            break;
        case 'v':
            strncpy(vdradminfile,optarg,sizeof(vdradminfile)-1);
            vdradminfile[sizeof(vdradminfile)-1]=0;
            retval=true;
            break;
        default:
            break;
        }
    }
    return retval;
}

// ----------------------------------------------------------------------

tvonscreenConfigPage::tvonscreenConfigPage(void) : cMenuSetupPage()
{
    m_NewConfig = tvonscreenCfg;

    Add(new cMenuEditIntItem(tr("font size"),
                             &m_NewConfig.fontdsize,-5,5));
    Add(new cMenuEditIntItem(tr("transparency"),
                             &m_NewConfig.transparency,0,100));
#if MAINMENUHOOKSVERSNUM == 10001
    Add(new cMenuEditBoolItem(tr("replace original schedule"),
                              &m_NewConfig.replaceorgschedule));
#endif
    Add(new cMenuEditBoolItem(tr("show only minutes"),
                              &m_NewConfig.onlyminutes));

    Add(new cMenuEditBoolItem(tr("show channel logos"),
                              &m_NewConfig.showLogos));
    Add(new cMenuEditBoolItem(tr("show channel names"),
                              &m_NewConfig.showChannels));
    Add(new cMenuEditBoolItem(tr("show logos in black&white"),
                              &m_NewConfig.bwlogos));
    Add(new cMenuEditBoolItem(tr("hide info line"),
                              &m_NewConfig.noInfoLine));
    Add(new cMenuEditTimeItem(tr("user point in time 1 (Key 4)"),
                              &m_NewConfig.usertime1));
    Add(new cMenuEditTimeItem(tr("user point in time 2 (Key 5)"),
                              &m_NewConfig.usertime2));
    Add(new cMenuEditTimeItem(tr("user point in time 3 (Key 6)"),
                              &m_NewConfig.usertime3));
    Add(new cMenuEditBoolItem(tr("jump to next day point if ago"),
                              &m_NewConfig.thenshownextday));
    Add(new cMenuEditBoolItem(tr("show search item itself"),
                              &m_NewConfig.showsearchinitiator));
}

tvonscreenConfigPage::~tvonscreenConfigPage()
{
}

void tvonscreenConfigPage::Store(void)
{
    SetupStore("showLogos", m_NewConfig.showLogos);
    SetupStore("noInfoLine", m_NewConfig.noInfoLine);
    SetupStore("showChannels", m_NewConfig.showChannels);
    SetupStore("bwlogos", m_NewConfig.bwlogos);
    SetupStore("usertime1", m_NewConfig.usertime1);
    SetupStore("usertime2", m_NewConfig.usertime2);
    SetupStore("usertime3", m_NewConfig.usertime3);
    SetupStore("thenshownextday", m_NewConfig.thenshownextday);
    SetupStore("showsearchinitiator", m_NewConfig.showsearchinitiator);
    SetupStore("fontdsize", m_NewConfig.fontdsize);
    SetupStore("transparency", m_NewConfig.transparency);
    SetupStore("onlyminutes", m_NewConfig.onlyminutes);
#if MAINMENUHOOKSVERSNUM == 10001
    SetupStore("replaceorgschedule", m_NewConfig.replaceorgschedule);
#endif
    tvonscreenCfg = m_NewConfig;
}
