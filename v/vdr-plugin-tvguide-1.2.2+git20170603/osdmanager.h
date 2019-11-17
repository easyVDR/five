#ifndef __TVGUIDE_OSDMANAGER_H
#define __TVGUIDE_OSDMANAGER_H

#include <vdr/osd.h>

class cOsdManager {
    private:
        cOsd *osd;
    public:
        cOsdManager(void);
        bool setOsd();
        void setBackground();
        void flush() {osd->Flush();};
        cPixmap *requestPixmap(int Layer, const cRect &ViewPort, const cRect &DrawPort = cRect::Null);
        void releasePixmap(cPixmap *pixmap);
        void deleteOsd() {delete osd;};
        int Width() { return osd->Width(); };
        int Height() { return osd->Height(); };
        int Top() { return osd->Top(); };
        int Left() { return osd->Left(); };
};

#endif //__TVGUIDE_OSDMANAGER_H
