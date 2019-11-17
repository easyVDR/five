#include "config.h"
#include "osdmanager.h"

cOsdManager::cOsdManager(void) {
}

bool cOsdManager::setOsd() {
    osd = cOsdProvider::NewOsd(cOsd::OsdLeft(), cOsd::OsdTop());
    if (osd) {
        tArea Area = { 0, 0, cOsd::OsdWidth(), cOsd::OsdHeight(),  32 };
        if (osd->SetAreas(&Area, 1) == oeOk) {  
            return true;
        }
    }
    return false;
}

void cOsdManager::setBackground() {
    
    if (tvguideConfig.displayStatusHeader && tvguideConfig.scaleVideo) {
        int widthStatus = cOsd::OsdWidth() - geoManager.statusHeaderHeight * 16 / 9;
        osd->DrawRectangle(0, 0, widthStatus, geoManager.statusHeaderHeight, theme.Color(clrBackgroundOSD));
        osd->DrawRectangle(0, geoManager.statusHeaderHeight, Width(), Height(), theme.Color(clrBackgroundOSD));    
    }
    else
        osd->DrawRectangle(0, 0, Width(), Height(), theme.Color(clrBackgroundOSD));
    
}

cPixmap *cOsdManager::requestPixmap(int Layer, const cRect &ViewPort, const cRect &DrawPort) {
    return osd->CreatePixmap(Layer, ViewPort, DrawPort);
}

void cOsdManager::releasePixmap(cPixmap *pixmap) {
    if (!pixmap)
        return;
    osd->DestroyPixmap(pixmap);
}