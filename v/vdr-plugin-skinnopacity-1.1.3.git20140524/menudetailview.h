#ifndef __NOPACITY_MENUDETAILVIEW_H
#define __NOPACITY_MENUDETAILVIEW_H

#include <vdr/recording.h>
#include <vdr/skins.h>

#include <vector>
#include <string>
#include <sstream>
#include "services/scraper2vdr.h"
#include "services/epgsearch.h"
#include "imagecache.h"
#include "config.h"
#include "detailview.h"

enum eDetailViewType {
    dvEvent = 0, 
    dvRecording, 
    dvText
};

class cNopacityDetailView : public cThread {
protected:
    eDetailViewType type;
    cOsd *osd;
    cImageCache *imgCache;
    const cEvent *ev;
    const cRecording *rec;
    const char *text;
    cNopacityView *view;
    cPixmap *scrollBar;
    cPixmap *scrollBarBack;
    int x, width, height, top, border;
    int headerHeight;
    void InitiateViewType(void);
    std::string LoadReruns(void);
    std::string LoadRecordingInformation(void);
    std::string StripXmlTag(std::string &Line, const char *Tag);
    int ReadSizeVdr(const char *strPath);
    virtual void Action(void);
public:
    cNopacityDetailView(eDetailViewType detailViewType, cOsd *osd, cImageCache *imgCache);
    virtual ~cNopacityDetailView(void);
    void SetGeometry(int x, int width, int height, int top, int contentBorder, int headerHeight);
    void SetEvent(const cEvent *e) { ev = e; };
    void SetRecording(const cRecording *r) { rec = r; };
    void SetText(const char *t) { text = t; };
    void SetScrollBar(cPixmap *s, cPixmap *sBack) { scrollBar = s; scrollBarBack = sBack; };
    void KeyInput(bool Up, bool Page);
};

#endif //__NOPACITY_MENUDETAILVIEW_H