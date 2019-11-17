/*
 * logo.h: The 'soppalusikka' VDR skin
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __SKINSOPPALUSIKKA_LOGO_H
#define __SKINSOPPALUSIKKA_LOGO_H

#include <map>
#include <string>
#include <vdr/osd.h>

class cSoppalusikkaLogoCache {
private:
  enum {
    DEFAULT_LOGO_HEIGHT = 48,
    DEFAULT_LOGO_WIDTH  = 64,
    DEFAULT_OSD_HEIGHT  = 576,
    DEFAULT_OSD_WIDTH   = 720
    };
  double xFactorM;
  double yFactorM;
  bool antiAliasM;
  unsigned int cacheSizeM;
  cBitmap *bitmapM;
  std::map<std::string, cBitmap*> cacheMapM;
  bool LoadXpm(const char *fileNameP);
public:
  cSoppalusikkaLogoCache(unsigned int cacheSizeP);
  ~cSoppalusikkaLogoCache();
  void Refresh();
  bool Resize(unsigned int cacheSizeP);
  bool Load(const char *fileNameP);
  cBitmap& Get(void);
  bool Flush(void);
  int GetLogoHeight() { return int(yFactorM * cSoppalusikkaLogoCache::DEFAULT_LOGO_HEIGHT); }
  int GetLogoWidth()  { return int(yFactorM * cSoppalusikkaLogoCache::DEFAULT_LOGO_WIDTH);  }
};

extern cSoppalusikkaLogoCache SoppalusikkaLogoCache;

#endif // __SKINSOPPALUSIKKA_LOGO_H
