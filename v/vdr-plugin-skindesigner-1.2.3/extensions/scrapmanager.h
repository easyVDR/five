#ifndef __SCRAPMANAGER_H
#define __SCRAPMANAGER_H

#include "../services/scraper2vdr.h"
#include "../libskindesignerapi/tokencontainer.h"

class cScrapManager {
private:
    static cPlugin *pScraper;
    cMovie *movie;
    cSeries *series;
    cPlugin *GetScraperPlugin(void);
    void SetMovie(skindesignerapi::cTokenContainer *tk, int actorsIndex);
    void SetSeries(skindesignerapi::cTokenContainer *tk, int actorsIndex);
protected:
    bool LoadFullScrapInfo(const cEvent *event, const cRecording *recording);
    void SetFullScrapInfo(skindesignerapi::cTokenContainer *tk, int actorsIndex);
    int NumActors(void);
    void SetHeaderScrapInfo(skindesignerapi::cTokenContainer *tk);
    void SetScraperPosterBanner(skindesignerapi::cTokenContainer *tk);
    void SetScraperPosterBannerChannel(skindesignerapi::cTokenContainer *tk);
    void SetScraperPosterBannerTimer(skindesignerapi::cTokenContainer *tk);
    void SetScraperRecordingPoster(skindesignerapi::cTokenContainer *tk, const cRecording *recording, bool isListElement);
    void RecPoster(const cRecording *rec, int &posterWidth, int &posterHeight, string &path, bool &hasPoster);
    void SetPosterBanner(skindesignerapi::cTokenContainer *tk, const cEvent *event, const cRecording *recording);
public:
    cScrapManager(void);
    virtual ~cScrapManager(void);
};
#endif //__SCRAPMANAGER_H