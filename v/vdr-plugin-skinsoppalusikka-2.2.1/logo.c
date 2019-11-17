/*
 * logo.c: The 'soppalusikka' VDR skin
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include <vdr/tools.h>
#include <vdr/plugin.h>

#include "common.h"
#include "config.h"
#include "log.h"
#include "logo.h"

cSoppalusikkaLogoCache SoppalusikkaLogoCache(0);

cSoppalusikkaLogoCache::cSoppalusikkaLogoCache(unsigned int cacheSizeP)
: xFactorM(1.0),
  yFactorM(1.0),
  antiAliasM(false),
  cacheSizeM(cacheSizeP),
  bitmapM(NULL)
{
}

cSoppalusikkaLogoCache::~cSoppalusikkaLogoCache()
{
  Flush();
}

void cSoppalusikkaLogoCache::Refresh()
{
  int width, height;
  double aspect, xfactor, yfactor;
  cDevice::PrimaryDevice()->GetOsdSize(width, height, aspect);
  debug1("%s width=%d height=%d", __PRETTY_FUNCTION__, width, height);
  xfactor = (double)width / DEFAULT_OSD_WIDTH;
  yfactor = (double)height / DEFAULT_OSD_HEIGHT;
  if (!DoubleEqual(xfactor, xFactorM) || !DoubleEqual(yfactor, yFactorM)) {
     xFactorM = xfactor;
     yFactorM = yfactor;
     Flush();
     }
}

bool cSoppalusikkaLogoCache::Resize(unsigned int cacheSizeP)
{
  debug1("%s (%d)", __PRETTY_FUNCTION__, cacheSizeP);
  // flush cache only if it's smaller than before
  if (cacheSizeP < cacheSizeM) {
     Flush();
     }
  // resize current cache
  cacheSizeM = cacheSizeP;
  return true;
}

bool cSoppalusikkaLogoCache::Load(const char *fileNameP)
{
  char *fileName = strdup(fileNameP);
  if (!fileName)
     return false;
  // replace '/' characters with '~'
  strreplace(fileName, '/', '~');
  debug1("%s (%s)", __PRETTY_FUNCTION__, fileName);
  // does the logo exist already in map
  std::map<std::string, cBitmap*>::iterator i = cacheMapM.find(fileName);
  if (i != cacheMapM.end()) {
     // yes - cache hit!
     debug2("%s Cache hit", __PRETTY_FUNCTION__);
     // check if logo really exist
     if (i->second == NULL) {
        debug2("%s Empty", __PRETTY_FUNCTION__);
        // empty logo in cache
        free(fileName);
        return false;
        }
     bitmapM = i->second;
     }
  else {
     // no - cache miss!
     debug2("%s Cache miss", __PRETTY_FUNCTION__);
     // try to load xpm logo
     LoadXpm(fileName);
     // check if cache is active
     if (cacheSizeM) {
        // update map
        if (cacheMapM.size() >= cacheSizeM) {
           // cache full - remove first
           debug2("%s Delete", __PRETTY_FUNCTION__);
           if (cacheMapM.begin()->second != NULL) {
              // logo exists - delete it
              cBitmap *bmp = cacheMapM.begin()->second;
              DELETENULL(bmp);
              }
           // erase item
           cacheMapM.erase(cacheMapM.begin());
           }
        // insert logo into map
        debug2("%s Insert %s", fileName, __PRETTY_FUNCTION__);
        cacheMapM.insert(std::make_pair(fileName, bitmapM));
        }
     // check if logo really exist
     if (bitmapM == NULL) {
        debug2("%s Empty", __PRETTY_FUNCTION__);
        // empty logo in cache
        free(fileName);
        return false;
        }
     }
  free(fileName);
  return true;
}

cBitmap& cSoppalusikkaLogoCache::Get(void)
{
  return *bitmapM;
}

bool cSoppalusikkaLogoCache::LoadXpm(const char *fileNameP)
{
  struct stat stbuf;
  cBitmap *bmp = new cBitmap(1, 1, 1);

  // create absolute filename
  cString filename = cString::sprintf("%s/%s.xpm", SoppalusikkaConfig.GetLogoDir(), fileNameP);
  debug1("%s (%s)", __PRETTY_FUNCTION__, *filename);
  // check validity
  if ((stat(*filename, &stbuf) == 0) && bmp->LoadXpm(*filename)) {
     // assign bitmap
     debug2("%s Logo found", __PRETTY_FUNCTION__);
     if (!DoubleEqual(1.0, xFactorM) || !DoubleEqual(1.0, yFactorM) || (bmp->Width() != DEFAULT_LOGO_WIDTH) || (bmp->Height() != DEFAULT_LOGO_HEIGHT)) {
        cBitmap *tmp = bmp;
        // scale bitmap keeping default proportions
        bmp = bmp->Scaled(yFactorM * DEFAULT_LOGO_WIDTH / bmp->Width(), yFactorM * DEFAULT_LOGO_HEIGHT / bmp->Height(), bmp->Bpp() > 4);
        debug2("%s Logo scaled", __PRETTY_FUNCTION__);
        DELETENULL(tmp);
        }
     bitmapM = bmp;
     return true;
     }
  // no valid xpm logo found - delete bitmap
  debug2("%s Logo not found or invalid resolution", __PRETTY_FUNCTION__);
  DELETENULL(bmp);
  bitmapM = NULL;
  return false;
}

bool cSoppalusikkaLogoCache::Flush(void)
{
  debug1("%s", __PRETTY_FUNCTION__);
  // check if map is empty
  if (!cacheMapM.empty()) {
     debug2("%s Non-empty", __PRETTY_FUNCTION__);
     // delete bitmaps and clear map
     std::map<std::string, cBitmap*>::iterator i = cacheMapM.begin();
     while (i != cacheMapM.end()) {
         cBitmap *bmp = i->second;
         cacheMapM.erase(i++);
         DELETENULL(bmp);
         }
     // nullify bitmap pointer
     bitmapM = NULL;
     }
  return true;
}

