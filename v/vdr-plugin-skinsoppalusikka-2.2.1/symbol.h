/*
 * symbol.h: The 'soppalusikka' VDR skin
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#ifndef __SKINSOPPALUSIKKA_SYMBOL_H
#define __SKINSOPPALUSIKKA_SYMBOL_H

#include <vdr/tools.h>
#include <vdr/osd.h>

enum eSymbols {
  SYMBOL_ONEPIXEL = 0,
  SYMBOL_AUDIO,
  SYMBOL_AUDIO_LEFT,
  SYMBOL_AUDIO_RIGHT,
  SYMBOL_DOLBY_DIGITAL,
  SYMBOL_ENCRYPTED,
  SYMBOL_FFWD,
  SYMBOL_FFWD1,
  SYMBOL_FFWD2,
  SYMBOL_FFWD3,
  SYMBOL_FREW,
  SYMBOL_FREW1,
  SYMBOL_FREW2,
  SYMBOL_FREW3,
  SYMBOL_PAUSE,
  SYMBOL_PLAY,
  SYMBOL_RADIO,
  SYMBOL_RECORDING,
  SYMBOL_RUNNING,
  SYMBOL_SFWD,
  SYMBOL_SFWD1,
  SYMBOL_SFWD2,
  SYMBOL_SFWD3,
  SYMBOL_SREW,
  SYMBOL_SREW1,
  SYMBOL_SREW2,
  SYMBOL_SREW3,
  SYMBOL_TELETEXT,
  SYMBOL_VPS,
  SYMBOL_EVENT_PARTTIMER,
  SYMBOL_EVENT_TIMER,
  SYMBOL_EVENT_VPS,
  SYMBOL_EVENT_RUNNING,
  SYMBOL_EVENT_RECORDING,
  SYMBOL_RECORDING_NEW,
  SYMBOL_MAX_COUNT
  };

class cSoppalusikkaSymbolCache {
private:
  enum {
    DEFAULT_GAP_TINY   = 1,
    DEFAULT_GAP_SMALL  = 2,
    DEFAULT_GAP_NORMAL = 4,
    DEFAULT_GAP_BIG    = 8,
    DEFAULT_ROUNDNESS  = 10,
    DEFAULT_HEIGHT     = 576,
    DEFAULT_WIDTH      = 720
    };
  double xFactorM;
  double yFactorM;
  bool antiAliasM;
  cVector<cBitmap*> cacheM;
  bool Populate(void);
  bool Flush(void);

public:
  cSoppalusikkaSymbolCache();
  ~cSoppalusikkaSymbolCache();
  void Refresh();
  cBitmap& Get(eSymbols symbolP);
  int GetGapTiny()   { return int(yFactorM * cSoppalusikkaSymbolCache::DEFAULT_GAP_TINY);   }
  int GetGapSmall()  { return int(yFactorM * cSoppalusikkaSymbolCache::DEFAULT_GAP_SMALL);  }
  int GetGapNormal() { return int(yFactorM * cSoppalusikkaSymbolCache::DEFAULT_GAP_NORMAL); }
  int GetGapBig()    { return int(yFactorM * cSoppalusikkaSymbolCache::DEFAULT_GAP_BIG);    }
  int GetRoundness() { return int(yFactorM * cSoppalusikkaSymbolCache::DEFAULT_ROUNDNESS);  }
};

extern cSoppalusikkaSymbolCache SoppalusikkaSymbolCache;

#endif // __SKINSOPPALUSIKKA_SYMBOL_H
