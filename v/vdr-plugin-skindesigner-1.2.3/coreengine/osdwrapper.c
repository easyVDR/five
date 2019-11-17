#include "osdwrapper.h"

cSdOsd::cSdOsd(void) {
    osd = NULL;
}

cSdOsd::~cSdOsd(void) {
    DeleteOsd();
}

void cSdOsd::Lock(void) {
    mutex.Lock();
}

void cSdOsd::Unlock(void) {
    mutex.Unlock();
}

bool cSdOsd::CreateOsd(int x, int y, int width, int height) {
    cOsd *newOsd = cOsdProvider::NewOsd(cOsd::OsdLeft() + x, cOsd::OsdTop() + y);
    if (newOsd) {
        tArea Area = { 0, 0, width - 1, height - 1,  32 };
        if (newOsd->SetAreas(&Area, 1) == oeOk) {
            Lock();
            osd = newOsd;
            Unlock();
            return true;
        }
    }
    return false;
}

void cSdOsd::DeleteOsd(void) {
    Lock();
    delete osd;
    osd = NULL;
    Unlock();
}

cPixmap *cSdOsd::CreatePixmap(int layer, cRect &viewPort, cRect &drawPort) {
    if (osd) {
        return osd->CreatePixmap(layer, viewPort, drawPort);
    }
    return NULL;
}

void cSdOsd::DestroyPixmap(cPixmap *pix) {
    if (osd) {
        osd->DestroyPixmap(pix);
    }
}

void cSdOsd::Flush(void) {
    if (osd) {
        osd->Flush();
    }
}
