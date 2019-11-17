#ifndef __SKINDESIGNER_H
#define __SKINDESIGNER_H

class cSkinDesigner;

#include "config.h"
#include "coreengine/viewdisplaychannel.h"
#include "coreengine/viewdisplaymenu.h"
#include "coreengine/viewdisplayreplay.h"
#include "coreengine/viewdisplayvolume.h"
#include "coreengine/viewdisplaytracks.h"
#include "coreengine/viewdisplaymessage.h"
#include "coreengine/viewdisplayplugin.h"
#include "displaychannel.h"
#include "displaymenu.h"
#include "displayreplay.h"
#include "displayvolume.h"
#include "displaytracks.h"
#include "displaymessage.h"
#include <vdr/skinlcars.h>

class cSkinDesigner : public cSkin {
private:
    bool init;
    string skin;
    cSkinLCARS *backupSkin;
    bool useBackupSkin;
    cGlobals *globals;
    cViewChannel  *channelView;
    cViewMenu     *menuView;
    cViewMessage  *messageView;
    cViewReplay   *replayView;
    cViewVolume   *volumeView;
    cViewTracks   *tracksView;
    cSDDisplayMenu *currentMenu;
    map<int, cViewPlugin* > pluginViews;
    void Init(void);
    void ReloadCaches(void);
    void DeleteViews(void);
    bool LoadViews(void);
    void LoadPluginViews(void);
    void CacheViews(void);
public:
    cSkinDesigner(string skin, cTheme *theme);
    virtual ~cSkinDesigner(void);
    virtual const char *Description(void);
    virtual cSkinDisplayChannel *DisplayChannel(bool WithInfo);
    virtual cSkinDisplayMenu *DisplayMenu(void);
    virtual cSkinDisplayReplay *DisplayReplay(bool ModeOnly);
    virtual cSkinDisplayVolume *DisplayVolume(void);
    virtual cSkinDisplayTracks *DisplayTracks(const char *Title, int NumTracks, const char * const *Tracks);
    virtual cSkinDisplayMessage *DisplayMessage(void);
    void ActivateBackupSkin(void) { useBackupSkin = true; };
    void Reload(void);
    void ListAvailableFonts(void);
    bool SetCustomIntToken(string option);
    bool SetCustomStringToken(string option);
    void ListCustomTokens(void);
    cSDDisplayMenu *GetDisplayMenu(void) { return currentMenu; };
    skindesignerapi::ISkinDisplayPlugin *GetDisplayPlugin(int plugId);
};

#endif //__SKINDESIGNER_H
