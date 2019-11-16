#ifndef __DVDPLUGIN_DVDSWITCH_H
#define __DVDPLUGIN_DVDSWITCH_H

#include <vdr/thread.h>
#include "helpers.h"
#include "setup.h"

class cDVDPluginThread : public cThread
{
  private:
    char *Image;
  protected:
    virtual void Action(void);
  public:
    cDVDPluginThread(const char *image = NULL);
    ~cDVDPluginThread(void);
};

class cDVDPlugin
{
  private:
    static cDVDPluginThread *thread;
    static void DetectDevice(void);
    static void SetLink(void);
    static void KillLink(void)
    {
      cFileCMD::Del(DVDSwitchSetup.DVDLink);
    };
  public:
    static void Start(const char *image = NULL);
    static void Exit(void);
    static void ChangeLink(const char *target, const char *link);
    static void Init(void)
    {
      DetectDevice();
      ChangeLink(DVDSwitchSetup.DVDLinkOrg, DVDSwitchSetup.DVDLink);
      SetLink();
    }
    static void Finish(void)
    {
      Exit();
      KillLink();
    }
};

#endif // __DVDPLUGIN_DVDSWITCH_H
