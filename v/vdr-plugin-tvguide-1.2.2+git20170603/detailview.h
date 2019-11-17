#ifndef __TVGUIDE_DETAILVIEW_H
#define __TVGUIDE_DETAILVIEW_H

#include <vdr/plugin.h>
#include <vdr/epg.h>
#include "config.h"
#include "tools.h"
#include "styledpixmap.h"
#include "footer.h"
#include "view.h"

// --- cDetailView  -------------------------------------------------------------

class cDetailView : public cThread {
private:
    const cEvent *event;
    cFooter *footer;
    cView *view;
    void InitiateView(void);
    std::string LoadReruns(void);
    void Action(void);
public:
    cDetailView(const cEvent *event, cFooter *footer);
    virtual ~cDetailView(void);
    eOSState ProcessKey(eKeys Key);
};

#endif //__TVGUIDE_DETAILVIEW_H