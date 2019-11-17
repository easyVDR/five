#ifndef __VIEWDISPLAYTRACKS_H
#define __VIEWDISPLAYTRACKS_H

#include "view.h"

class cViewTracks : public cView {
private:
    cViewListAudioTracks *viewList;
    cVeDtBackground *veBackground;
    cVeDtHeader *veHeader;
    bool change;
    void SetViewElements(void);
    void SetViewElementObjects(void);
    void ClearVariables(void);
public:
    cViewTracks(void);
    virtual ~cViewTracks(void);
    void Close(void);
    void AddViewList(cViewList *viewList);
    void PreCache(void);
    void SetTitle(const char *title);
    void SetNumtracks(int numTracks);
    void SetTracks(const char * const *tracks);
    void SetAudiochannel(int audioChannel);
    void SetCurrentTrack(int index);
    void Flush(void);
};

#endif //__VIEWDISPLAYTRACKS_H