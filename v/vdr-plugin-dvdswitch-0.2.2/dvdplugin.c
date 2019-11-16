
#include <vdr/plugin.h>
#include <vdr/player.h>
#include <unistd.h>
#include "imagelist.h"
#include "dvdplugin.h"

cDVDPluginThread::cDVDPluginThread(const char *image)
  :cThread("DVDPluginThread")
{
  dsyslog("dvdswitch: Create new DVD Thread");
  Image = image ? strdup(image) : NULL;
}

cDVDPluginThread::~cDVDPluginThread(void)
{
  dsyslog("dvdswitch: DVD Thread stopped");

  free(Image);
  Cancel();
}

void cDVDPluginThread::Action(void)
{
  dsyslog("dvdswitch: DVD Thread started");
  if(Image)
    cDVDPlugin::ChangeLink(Image, DVDSwitchSetup.DVDLink);

#if VDRVERSNUM < 10332
  cPlugin *plugin = cPluginManager::GetPlugin("dvd");
  if(plugin)
  {
    plugin->MainMenuAction();
    dsyslog("dvdswitch: DVD MainMenuAction called");
  }
#else
  cRemote::CallPlugin("dvd");
  dsyslog("dvdswitch: DVD Plugin called");
#endif

  cCondWait::SleepMs(2 * 1000);
  cControl *control = cControl::Control();
  dsyslog("dvdswitch: DVD control %s",control ? "found" : "not found");
  while(control)
  {
    cCondWait::SleepMs(5 * 1000);
    control = cControl::Control();
  }
  dsyslog("dvdswitch: DVD control closed");

  if(Image)
    cDVDPlugin::ChangeLink(DVDSwitchSetup.DVDLinkOrg, DVDSwitchSetup.DVDLink);

  dsyslog("dvdswitch: DVD Thread closed");
  cDVDPlugin::Exit();
}

// --- cDVDPlugin ----------------------------------------------------------------

cDVDPluginThread *cDVDPlugin::thread = NULL;

void cDVDPlugin::DetectDevice(void)
{
  dsyslog("dvdswitch: parse cmdline of dvd-plugin for DVD Device");
  char *file = NULL;
  char *dvd = NULL;

  if(0 < asprintf(&file, "/proc/%i/cmdline", getpid())) {
    /* read /proc/`pidof vdr`/cmdline */
    char buffer[1024];
    int len = -1;

    int fd = open(file,O_RDONLY);
    if (fd >= 0) {
      do {
          len = read(fd,buffer,sizeof(buffer)); }
      while (len == -1 && errno == EINTR);
      close(fd);
    } else {
      char* szErr = get_strerror(errno);
      esyslog("dvdswitch: could not open %s:%s", file, szErr ? szErr : "");
      if(szErr) free(szErr);
    }
    free(file);

    if (len < 0) {
      char* szErr = get_strerror(errno);
      esyslog("dvdswitch: could not read %s:%s", file, szErr ? szErr : "");
      if(szErr) free(szErr);
    } else {
      char *output = NULL;
      for(int n = 0; n < len && !dvd;n += (strlen(output) + 1)) {
        output = buffer + n;
        //dsyslog("dvdswitch: parse '%s'", output);
        if(0 == strncmp(output, "--plugin=dvd ",13)
            || 0 == strncmp(output, "-Pdvd ",6)
            || 0 == strncmp(output, "-P dvd ",7)
            || 0 == strncmp(output, "-P'dvd ",7)
            || 0 == strncmp(output, "-P\"dvd ",7)) {

        cTokenizer *token = new cTokenizer(output, " ");
        for(int i = 0; i <= token->Count(); i++) {
            const char* t = token->GetToken(i);
            if(t && 0 == strcmp(t, "-C")
                 && token->GetToken(i + 1)) {
              dvd = strdup(token->GetToken(i + 1));
              break;
            }
            else if(t && 0 == strncmp(t, "--dvd=", 6)) {
              const char *p = strchr(t, '=');
              dvd = strdup(p + 1);
              break;
            }
          }
          DELETENULL(token);
        }
      }
    }
    if(!dvd) {
      dvd = strdup("/dev/dvd");
    }
    if(dvd) {
      isyslog("dvdswitch: use DVD Device: %s", dvd);
      DVDSwitchSetup.SetDVDDevice(dvd);
      free(dvd);
    }
  }
}

void cDVDPlugin::SetLink(void)
{
  int argc = 2;
  char *argv[2] = { NULL, NULL };

  if(0 < asprintf(&argv[0], "%s", "dvd")) {
    if(0 < asprintf(&argv[1], "--dvd=%s", DVDSwitchSetup.DVDLink)) {

      cPlugin *plugin = cPluginManager::GetPlugin("dvd");
      if(plugin)
      {
        optind = 0; //reset for getopt
        plugin->ProcessArgs(argc, argv);
      }

      free(argv[1]);
    }
    free(argv[0]);
  }
}

void cDVDPlugin::Start(const char *image)
{
  if(!thread)
  {
    thread = new cDVDPluginThread(image);
    thread->Start();
  }
}

void cDVDPlugin::Exit(void)
{
  if(thread)
    DELETENULL(thread);
}

void cDVDPlugin::ChangeLink(const char *target,const char *link)
{
  if(target && link)
  {
    isyslog("dvdswitch: create link %s to %s", link, target);
    
    if(-1 == unlink(link) && errno != ENOENT) {
      char* szErr = get_strerror(errno);
      esyslog("dvdswitch: could not remove symbolic link %s : %s (%d)", link, szErr ? szErr : "", errno);
      if(szErr) free(szErr);
    }
    if(-1 == symlink(target, link)) {
      char* szErr = get_strerror(errno);
      esyslog("dvdswitch: could not create link %s to %s :%s (%d)", link, target, szErr ? szErr : "", errno);
      if(szErr) free(szErr);
    }
  }
}
