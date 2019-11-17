#include "scrapmanager.h"
#include "../coreengine/definitions.h"
#include "helpers.h"

cPlugin *cScrapManager::pScraper = NULL;

cScrapManager::cScrapManager(void) {
    if (!pScraper) {
        pScraper = GetScraperPlugin();
    }
    movie = NULL;
    series = NULL;
}

cScrapManager::~cScrapManager(void) {
    delete movie;
    delete series;
}

bool cScrapManager::LoadFullScrapInfo(const cEvent *event, const cRecording *recording) {
    if (!pScraper) {
        return false;
    }
    delete movie;
    movie = NULL;
    delete series;
    series = NULL;
    
    ScraperGetEventType getType;
    getType.event = event;
    getType.recording = recording;
    if (!pScraper->Service("GetEventType", &getType)) {
        return false;
    }
    if (getType.type == tMovie) {
        movie = new cMovie();
        movie->movieId = getType.movieId;
        pScraper->Service("GetMovie", movie);
        return true;
    } else if (getType.type == tSeries) {
        series = new cSeries();
        series->seriesId = getType.seriesId;
        series->episodeId = getType.episodeId;
        pScraper->Service("GetSeries", series);
        return true;
    }
    return false;
}

void cScrapManager::SetFullScrapInfo(skindesignerapi::cTokenContainer *tk, int actorsIndex) {
    if (series) {
        tk->AddIntToken((int)eScraperIT::ismovie, 0);
        tk->AddIntToken((int)eScraperIT::isseries, 1);
        SetSeries(tk, actorsIndex);
    } else if (movie) {
        tk->AddIntToken((int)eScraperIT::ismovie, 1);
        tk->AddIntToken((int)eScraperIT::isseries, 0);
        SetMovie(tk, actorsIndex);
    } else {
        tk->AddIntToken((int)eScraperIT::ismovie, 0);
        tk->AddIntToken((int)eScraperIT::isseries, 0);        
    }
}


int cScrapManager::NumActors(void) {
    if (series) {
        return series->actors.size();
    } else if (movie) {
        return movie->actors.size();
    }    
    return 0;
}

void cScrapManager::SetHeaderScrapInfo(skindesignerapi::cTokenContainer *tk) {
    if (series) {
        tk->AddIntToken((int)eScraperHeaderIT::ismovie, 0);
        tk->AddIntToken((int)eScraperHeaderIT::isseries, 1);
        vector<cTvMedia>::iterator poster = series->posters.begin();
        if (poster != series->posters.end()) {
            tk->AddIntToken((int)eScraperHeaderIT::posteravailable, true);
            tk->AddIntToken((int)eScraperHeaderIT::posterwidth, (*poster).width);
            tk->AddIntToken((int)eScraperHeaderIT::posterheight, (*poster).height);
            tk->AddStringToken((int)eScraperHeaderST::posterpath, (*poster).path.c_str());
        }
        vector<cTvMedia>::iterator banner = series->banners.begin();
        if (banner != series->banners.end()) {
            tk->AddIntToken((int)eScraperHeaderIT::banneravailable, true);
            tk->AddIntToken((int)eScraperHeaderIT::bannerwidth, (*banner).width);
            tk->AddIntToken((int)eScraperHeaderIT::bannerheight, (*banner).height);
            tk->AddStringToken((int)eScraperHeaderST::bannerpath, (*banner).path.c_str());
        }
    } else if (movie) {
        tk->AddIntToken((int)eScraperHeaderIT::ismovie, 1);
        tk->AddIntToken((int)eScraperHeaderIT::isseries, 0);
        tk->AddIntToken((int)eScraperHeaderIT::posteravailable, true);
        tk->AddIntToken((int)eScraperHeaderIT::banneravailable, false);
        tk->AddIntToken((int)eScraperHeaderIT::posterwidth, movie->poster.width);
        tk->AddIntToken((int)eScraperHeaderIT::posterheight, movie->poster.height);
        tk->AddStringToken((int)eScraperHeaderST::posterpath, movie->poster.path.c_str());
    } else {
        tk->AddIntToken((int)eScraperHeaderIT::ismovie, 0);
        tk->AddIntToken((int)eScraperHeaderIT::isseries, 0);        
    }    
}

void cScrapManager::SetScraperPosterBanner(skindesignerapi::cTokenContainer *tk) {
    if (movie) {
        tk->AddIntToken((int)eCeMenuSchedulesIT::hasposter, 1);
        tk->AddStringToken((int)eCeMenuSchedulesST::posterpath, movie->poster.path.c_str());
        tk->AddIntToken((int)eCeMenuSchedulesIT::posterwidth, movie->poster.width);
        tk->AddIntToken((int)eCeMenuSchedulesIT::posterheight, movie->poster.height);
    } else if (series) {
        vector<cTvMedia>::iterator poster = series->posters.begin();
        if (poster != series->posters.end()) {
            tk->AddIntToken((int)eCeMenuSchedulesIT::hasposter, 1);
            tk->AddIntToken((int)eCeMenuSchedulesIT::posterwidth, (*poster).width);
            tk->AddIntToken((int)eCeMenuSchedulesIT::posterheight, (*poster).height);
            tk->AddStringToken((int)eCeMenuSchedulesST::posterpath, (*poster).path.c_str());
        }
        vector<cTvMedia>::iterator banner = series->banners.begin();
        if (banner != series->banners.end()) {
            tk->AddIntToken((int)eCeMenuSchedulesIT::hasbanner, 1);
            tk->AddIntToken((int)eCeMenuSchedulesIT::bannerwidth, (*banner).width);
            tk->AddIntToken((int)eCeMenuSchedulesIT::bannerheight, (*banner).height);
            tk->AddStringToken((int)eCeMenuSchedulesST::bannerpath, (*banner).path.c_str());
        }
    }
}

void cScrapManager::SetScraperPosterBannerChannel(skindesignerapi::cTokenContainer *tk) {
    if (movie) {
        tk->AddIntToken((int)eCeMenuChannelsIT::hasposter, 1);
        tk->AddStringToken((int)eCeMenuChannelsST::posterpath, movie->poster.path.c_str());
        tk->AddIntToken((int)eCeMenuChannelsIT::posterwidth, movie->poster.width);
        tk->AddIntToken((int)eCeMenuChannelsIT::posterheight, movie->poster.height);
    } else if (series) {
        vector<cTvMedia>::iterator poster = series->posters.begin();
        if (poster != series->posters.end()) {
            tk->AddIntToken((int)eCeMenuChannelsIT::hasposter, 1);
            tk->AddIntToken((int)eCeMenuChannelsIT::posterwidth, (*poster).width);
            tk->AddIntToken((int)eCeMenuChannelsIT::posterheight, (*poster).height);
            tk->AddStringToken((int)eCeMenuChannelsST::posterpath, (*poster).path.c_str());
        }
        vector<cTvMedia>::iterator banner = series->banners.begin();
        if (banner != series->banners.end()) {
            tk->AddIntToken((int)eCeMenuChannelsIT::hasbanner, 1);
            tk->AddIntToken((int)eCeMenuChannelsIT::bannerwidth, (*banner).width);
            tk->AddIntToken((int)eCeMenuChannelsIT::bannerheight, (*banner).height);
            tk->AddStringToken((int)eCeMenuChannelsST::bannerpath, (*banner).path.c_str());
        }
    }
}

void cScrapManager::SetScraperPosterBannerTimer(skindesignerapi::cTokenContainer *tk) {
    if (movie) {
        tk->AddIntToken((int)eCeMenuTimersIT::hasposter, 1);
        tk->AddStringToken((int)eCeMenuTimersST::posterpath, movie->poster.path.c_str());
        tk->AddIntToken((int)eCeMenuTimersIT::posterwidth, movie->poster.width);
        tk->AddIntToken((int)eCeMenuTimersIT::posterheight, movie->poster.height);
    } else if (series) {
        vector<cTvMedia>::iterator poster = series->posters.begin();
        if (poster != series->posters.end()) {
            tk->AddIntToken((int)eCeMenuTimersIT::hasposter, 1);
            tk->AddIntToken((int)eCeMenuTimersIT::posterwidth, (*poster).width);
            tk->AddIntToken((int)eCeMenuTimersIT::posterheight, (*poster).height);
            tk->AddStringToken((int)eCeMenuTimersST::posterpath, (*poster).path.c_str());
        }
        vector<cTvMedia>::iterator banner = series->banners.begin();
        if (banner != series->banners.end()) {
            tk->AddIntToken((int)eCeMenuTimersIT::hasbanner, 1);
            tk->AddIntToken((int)eCeMenuTimersIT::bannerwidth, (*banner).width);
            tk->AddIntToken((int)eCeMenuTimersIT::bannerheight, (*banner).height);
            tk->AddStringToken((int)eCeMenuTimersST::bannerpath, (*banner).path.c_str());
        }
    }
}

void cScrapManager::SetScraperRecordingPoster(skindesignerapi::cTokenContainer *tk, const cRecording *recording, bool isListElement) {
    if (!pScraper) {
        return;
    }
    ScraperGetPosterThumb call;
    call.event = NULL;
    call.recording = recording;
    if (pScraper->Service("GetPosterThumb", &call)) {
        if (isListElement) {
            tk->AddIntToken((int)eLeMenuRecordingsIT::hasposterthumbnail, FileExists(call.poster.path));
            tk->AddIntToken((int)eLeMenuRecordingsIT::thumbnailwidth, call.poster.width);
            tk->AddIntToken((int)eLeMenuRecordingsIT::thumbnailheight, call.poster.height);
            tk->AddStringToken((int)eLeMenuRecordingsST::thumbnailpath, call.poster.path.c_str());
        } else {
            tk->AddIntToken((int)eCeMenuRecordingsIT::hasposterthumbnail, FileExists(call.poster.path));
            tk->AddIntToken((int)eCeMenuRecordingsIT::thumbnailwidth, call.poster.width);
            tk->AddIntToken((int)eCeMenuRecordingsIT::thumbnailheight, call.poster.height);
            tk->AddStringToken((int)eCeMenuRecordingsST::thumbnailpath, call.poster.path.c_str());
        }
    }

    ScraperGetPoster call2;
    call2.event = NULL;
    call2.recording = recording;
    if (pScraper->Service("GetPoster", &call2)) {
        if (isListElement) {
            tk->AddIntToken((int)eLeMenuRecordingsIT::hasposter, FileExists(call2.poster.path));
            tk->AddIntToken((int)eLeMenuRecordingsIT::posterwidth, call2.poster.width);
            tk->AddIntToken((int)eLeMenuRecordingsIT::posterheight, call2.poster.height);
            tk->AddStringToken((int)eLeMenuRecordingsST::posterpath, call2.poster.path.c_str());
        } else {
            tk->AddIntToken((int)eCeMenuRecordingsIT::hasposter, FileExists(call2.poster.path));
            tk->AddIntToken((int)eCeMenuRecordingsIT::posterwidth, call2.poster.width);
            tk->AddIntToken((int)eCeMenuRecordingsIT::posterheight, call2.poster.height);
            tk->AddStringToken((int)eCeMenuRecordingsST::posterpath, call2.poster.path.c_str());            
        }
    }
}

cPlugin *cScrapManager::GetScraperPlugin(void) {
    static cPlugin *pScraper = cPluginManager::GetPlugin("scraper2vdr");
    if( !pScraper ) // if it doesn't exit, try tvscraper
        pScraper = cPluginManager::GetPlugin("tvscraper");
    return pScraper;
}

void cScrapManager::SetMovie(skindesignerapi::cTokenContainer *tk, int actorsIndex) {
    tk->AddStringToken((int)eScraperST::movietitle, movie->title.c_str());
    tk->AddStringToken((int)eScraperST::movieoriginalTitle, movie->originalTitle.c_str());
    tk->AddStringToken((int)eScraperST::movietagline, movie->tagline.c_str());
    tk->AddStringToken((int)eScraperST::movieoverview, movie->overview.c_str());
    tk->AddStringToken((int)eScraperST::moviegenres, movie->genres.c_str());
    tk->AddStringToken((int)eScraperST::moviehomepage, movie->homepage.c_str());
    tk->AddStringToken((int)eScraperST::moviereleasedate, movie->releaseDate.c_str());
    tk->AddStringToken((int)eScraperST::moviepopularity, *cString::sprintf("%f", movie->popularity));
    tk->AddStringToken((int)eScraperST::movievoteaverage, *cString::sprintf("%f", movie->voteAverage));
    tk->AddStringToken((int)eScraperST::posterpath, movie->poster.path.c_str());
    tk->AddStringToken((int)eScraperST::fanartpath, movie->fanart.path.c_str());
    tk->AddStringToken((int)eScraperST::collectionposterpath, movie->collectionPoster.path.c_str());
    tk->AddStringToken((int)eScraperST::collectionfanartpath, movie->collectionFanart.path.c_str());
    tk->AddIntToken((int)eScraperIT::movieadult, movie->adult);
    tk->AddIntToken((int)eScraperIT::moviebudget, movie->budget);
    tk->AddIntToken((int)eScraperIT::movierevenue, movie->revenue);
    tk->AddIntToken((int)eScraperIT::movieruntime, movie->runtime);
    tk->AddIntToken((int)eScraperIT::posterwidth, movie->poster.width);
    tk->AddIntToken((int)eScraperIT::posterheight, movie->poster.height);
    tk->AddIntToken((int)eScraperIT::fanartwidth, movie->fanart.width);
    tk->AddIntToken((int)eScraperIT::fanartheight, movie->fanart.height);
    tk->AddIntToken((int)eScraperIT::collectionposterwidth, movie->collectionPoster.width);
    tk->AddIntToken((int)eScraperIT::collectionposterheight, movie->collectionPoster.height);
    tk->AddIntToken((int)eScraperIT::collectionfanartwidth, movie->collectionFanart.width);
    tk->AddIntToken((int)eScraperIT::collectionfanartheight, movie->collectionFanart.height);
    if (movie->collectionPoster.path.size() > 0)
        tk->AddIntToken((int)eScraperIT::movieiscollection, 1);
    int i=0;
    for (vector<cActor>::iterator act = movie->actors.begin(); act != movie->actors.end(); act++) {
        tk->AddLoopToken(actorsIndex, i, (int)eScraperLT::name, (*act).name.c_str());
        tk->AddLoopToken(actorsIndex, i, (int)eScraperLT::role, (*act).role.c_str());
        tk->AddLoopToken(actorsIndex, i, (int)eScraperLT::thumb, (*act).actorThumb.path.c_str());
        tk->AddLoopToken(actorsIndex, i, (int)eScraperLT::thumbwidth, *cString::sprintf("%d", (*act).actorThumb.width));
        tk->AddLoopToken(actorsIndex, i, (int)eScraperLT::thumbheight, *cString::sprintf("%d", (*act).actorThumb.height));
        i++;
    }
}

void cScrapManager::SetSeries(skindesignerapi::cTokenContainer *tk, int actorsIndex) {
    //Series Basics
    tk->AddStringToken((int)eScraperST::seriesname, series->name.c_str());
    tk->AddStringToken((int)eScraperST::seriesoverview, series->overview.c_str());
    tk->AddStringToken((int)eScraperST::seriesfirstaired, series->firstAired.c_str());
    tk->AddStringToken((int)eScraperST::seriesnetwork, series->network.c_str());
    tk->AddStringToken((int)eScraperST::seriesgenre, series->genre.c_str());
    tk->AddStringToken((int)eScraperST::seriesrating, *cString::sprintf("%f", series->rating));
    tk->AddStringToken((int)eScraperST::seriesstatus, series->status.c_str());
    //Episode Information
    tk->AddIntToken((int)eScraperIT::episodenumber, series->episode.number);
    tk->AddIntToken((int)eScraperIT::episodeseason, series->episode.season);
    tk->AddStringToken((int)eScraperST::episodetitle, series->episode.name.c_str());
    tk->AddStringToken((int)eScraperST::episodefirstaired, series->episode.firstAired.c_str());
    tk->AddStringToken((int)eScraperST::episodegueststars, series->episode.guestStars.c_str());
    tk->AddStringToken((int)eScraperST::episodeoverview, series->episode.overview.c_str());
    tk->AddStringToken((int)eScraperST::episoderating, *cString::sprintf("%f", series->episode.rating));
    tk->AddIntToken((int)eScraperIT::episodeimagewidth, series->episode.episodeImage.width);
    tk->AddIntToken((int)eScraperIT::episodeimageheight, series->episode.episodeImage.height);
    tk->AddStringToken((int)eScraperST::episodeimagepath, series->episode.episodeImage.path.c_str());
    //Seasonposter
    tk->AddIntToken((int)eScraperIT::seasonposterwidth, series->seasonPoster.width);
    tk->AddIntToken((int)eScraperIT::seasonposterheight, series->seasonPoster.height);
    tk->AddStringToken((int)eScraperST::seasonposterpath, series->seasonPoster.path.c_str());
    //Posters
    int indexInt = (int)eScraperIT::seriesposter1width;
    int indexStr = (int)eScraperST::seriesposter1path;
    for(vector<cTvMedia>::iterator poster = series->posters.begin(); poster != series->posters.end(); poster++) {
        tk->AddIntToken(indexInt, (*poster).width);
        tk->AddIntToken(indexInt+1, (*poster).height);
        tk->AddStringToken(indexStr, (*poster).path.c_str());
        indexInt += 2;
        indexStr++;
    }
    //Banners
    indexInt = (int)eScraperIT::seriesbanner1width;
    indexStr = (int)eScraperST::seriesbanner1path;
    for(vector<cTvMedia>::iterator banner = series->banners.begin(); banner != series->banners.end(); banner++) {
        tk->AddIntToken(indexInt, (*banner).width);
        tk->AddIntToken(indexInt+1, (*banner).height);
        tk->AddStringToken(indexStr, (*banner).path.c_str());
        indexInt += 2;
        indexStr++;
    }
    //Fanarts
    indexInt = (int)eScraperIT::seriesfanart1width;
    indexStr = (int)eScraperST::seriesfanart1path;
    for(vector<cTvMedia>::iterator fanart = series->fanarts.begin(); fanart != series->fanarts.end(); fanart++) {
        tk->AddIntToken(indexInt, (*fanart).width);
        tk->AddIntToken(indexInt+1, (*fanart).height);
        tk->AddStringToken(indexStr, (*fanart).path.c_str());
        indexInt += 2;
        indexStr++;
    }
    //Actors
    int i=0;
    for (vector<cActor>::iterator act = series->actors.begin(); act != series->actors.end(); act++) {
        tk->AddLoopToken(actorsIndex, i, (int)eScraperLT::name, (*act).name.c_str());
        tk->AddLoopToken(actorsIndex, i, (int)eScraperLT::role, (*act).role.c_str());
        tk->AddLoopToken(actorsIndex, i, (int)eScraperLT::thumb, (*act).actorThumb.path.c_str());
        tk->AddLoopToken(actorsIndex, i, (int)eScraperLT::thumbwidth, *cString::sprintf("%d", (*act).actorThumb.width));
        tk->AddLoopToken(actorsIndex, i, (int)eScraperLT::thumbheight, *cString::sprintf("%d", (*act).actorThumb.height));
        i++;
    }
}

void cScrapManager::RecPoster(const cRecording *rec, int &posterWidth, int &posterHeight, string &path, bool &hasPoster) {
    if (!pScraper) {
        return;
    }
    ScraperGetPoster callPoster;
    callPoster.event = NULL;
    callPoster.recording = rec;
    if (pScraper->Service("GetPoster", &callPoster)) {
        posterWidth = callPoster.poster.width;
        posterHeight = callPoster.poster.height;
        path = callPoster.poster.path;
        if (path.size() > 0)
            hasPoster = true;
    }
}

void cScrapManager::SetPosterBanner(skindesignerapi::cTokenContainer *tk, const cEvent *event, const cRecording *recording) {
    if (!pScraper) {
        return;
    }
    int mediaWidth = 0;
    int mediaHeight = 0;
    string mediaPath = "";
    bool isBanner = false;
    int posterWidth = 0;
    int posterHeight = 0;
    string posterPath = "";
    bool hasPoster = false;
    int bannerWidth = 0;
    int bannerHeight = 0;
    string bannerPath = "";
    bool hasBanner = false;

    ScraperGetPosterBannerV2 call;
    call.event = event;
    call.recording = recording;
    if (pScraper->Service("GetPosterBannerV2", &call)) {
        if ((call.type == tSeries) && call.banner.path.size() > 0) {
            mediaWidth = call.banner.width;
            mediaHeight = call.banner.height;
            mediaPath = call.banner.path;
            isBanner = true;
            bannerWidth = mediaWidth;
            bannerHeight = mediaHeight;
            bannerPath = mediaPath;
            hasBanner = true;
            ScraperGetPoster callPoster;
            callPoster.event = event;
            callPoster.recording = recording;
            if (pScraper->Service("GetPoster", &callPoster)) {
                posterWidth = callPoster.poster.width;
                posterHeight = callPoster.poster.height;
                posterPath = callPoster.poster.path;
                hasPoster = true;
            }
        } else if (call.type == tMovie && call.poster.path.size() > 0 && call.poster.height > 0) {
            mediaWidth = call.poster.width;
            mediaHeight = call.poster.height;
            mediaPath = call.poster.path;
            posterWidth = call.poster.width;
            posterHeight = call.poster.height;
            posterPath = call.poster.path;
            hasPoster = true;
        }
    }
    tk->AddIntToken((int)eScraperPosterBannerIT::mediawidth, mediaWidth);
    tk->AddIntToken((int)eScraperPosterBannerIT::mediaheight, mediaHeight);
    tk->AddIntToken((int)eScraperPosterBannerIT::isbanner, isBanner);
    tk->AddStringToken((int)eScraperPosterBannerST::mediapath, mediaPath.c_str());
    tk->AddIntToken((int)eScraperPosterBannerIT::posterwidth, posterWidth);
    tk->AddIntToken((int)eScraperPosterBannerIT::posterheight, posterHeight);
    tk->AddStringToken((int)eScraperPosterBannerST::posterpath, posterPath.c_str());
    tk->AddIntToken((int)eScraperPosterBannerIT::hasposter, hasPoster);
    tk->AddIntToken((int)eScraperPosterBannerIT::bannerwidth, bannerWidth);
    tk->AddIntToken((int)eScraperPosterBannerIT::bannerheight, bannerHeight);
    tk->AddStringToken((int)eScraperPosterBannerST::bannerpath, bannerPath.c_str());
    tk->AddIntToken((int)eScraperPosterBannerIT::hasbanner, hasBanner);
}