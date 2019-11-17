/*
 * symbol.c: The 'soppalusikka' VDR skin
 *
 * $Id: $
 */

#include <vdr/device.h>

#include "common.h"
#include "config.h"
#include "log.h"
#include "tools.h"
#include "symbol.h"

#include "symbols/audio.xpm"
#include "symbols/audioleft.xpm"
#include "symbols/audioright.xpm"
#include "symbols/dolbydigital.xpm"
#include "symbols/encrypted.xpm"
#include "symbols/ffwd.xpm"
#include "symbols/ffwd1.xpm"
#include "symbols/ffwd2.xpm"
#include "symbols/ffwd3.xpm"
#include "symbols/frew.xpm"
#include "symbols/frew1.xpm"
#include "symbols/frew2.xpm"
#include "symbols/frew3.xpm"
#include "symbols/pause.xpm"
#include "symbols/play.xpm"
#include "symbols/radio.xpm"
#include "symbols/recording.xpm"
#include "symbols/running.xpm"
#include "symbols/sfwd.xpm"
#include "symbols/sfwd1.xpm"
#include "symbols/sfwd2.xpm"
#include "symbols/sfwd3.xpm"
#include "symbols/srew.xpm"
#include "symbols/srew1.xpm"
#include "symbols/srew2.xpm"
#include "symbols/srew3.xpm"
#include "symbols/teletext.xpm"
#include "symbols/vps.xpm"
#include "symbols/eventparttimer.xpm"
#include "symbols/eventtimer.xpm"
#include "symbols/eventvps.xpm"
#include "symbols/eventrunning.xpm"
#include "symbols/eventrecording.xpm"
#include "symbols/recordingnew.xpm"

static cBitmap bmOnePixelS(1, 1, 1);
static cBitmap bmAudioS(audio_xpm);
static cBitmap bmAudioLeftS(audioleft_xpm);
static cBitmap bmAudioRightS(audioright_xpm);
static cBitmap bmDolbyDigitalS(dolbydigital_xpm);
static cBitmap bmEncryptedS(encrypted_xpm);
static cBitmap bmFastForwardS(ffwd_xpm);
static cBitmap bmFastForward1S(ffwd1_xpm);
static cBitmap bmFastForward2S(ffwd2_xpm);
static cBitmap bmFastForward3S(ffwd3_xpm);
static cBitmap bmFastReverseS(frew_xpm);
static cBitmap bmFastReverse1S(frew1_xpm);
static cBitmap bmFastReverse2S(frew2_xpm);
static cBitmap bmFastReverse3S(frew3_xpm);
static cBitmap bmPauseS(pause_xpm);
static cBitmap bmPlayS(play_xpm);
static cBitmap bmRadioS(radio_xpm);
static cBitmap bmRecordingS(recording_xpm);
static cBitmap bmRunningS(running_xpm);
static cBitmap bmSlowForwardS(sfwd_xpm);
static cBitmap bmSlowForward1S(sfwd1_xpm);
static cBitmap bmSlowForward2S(sfwd2_xpm);
static cBitmap bmSlowForward3S(sfwd3_xpm);
static cBitmap bmSlowReverseS(srew_xpm);
static cBitmap bmSlowReverse1S(srew1_xpm);
static cBitmap bmSlowReverse2S(srew2_xpm);
static cBitmap bmSlowReverse3S(srew3_xpm);
static cBitmap bmTeletextS(teletext_xpm);
static cBitmap bmVpsS(vps_xpm);
static cBitmap bmEventPartTimerS(eventparttimer_xpm);
static cBitmap bmEventTimerS(eventtimer_xpm);
static cBitmap bmEventVpsS(eventvps_xpm);
static cBitmap bmEventRunningS(eventrunning_xpm);
static cBitmap bmEventRecordingS(eventrecording_xpm);
static cBitmap bmRecordingNewS(recordingnew_xpm);

cSoppalusikkaSymbolCache SoppalusikkaSymbolCache;

cSoppalusikkaSymbolCache::cSoppalusikkaSymbolCache()
: xFactorM(1.0),
  yFactorM(1.0),
  antiAliasM(false)
{
  Populate();
}

cSoppalusikkaSymbolCache::~cSoppalusikkaSymbolCache()
{
  Flush();
}

void cSoppalusikkaSymbolCache::Refresh()
{
  int width, height;
  double aspect, xfactor, yfactor;
  cDevice::PrimaryDevice()->GetOsdSize(width, height, aspect);
  debug1("%s width=%d height=%d", __PRETTY_FUNCTION__, width, height);
  xfactor = (double)width / DEFAULT_WIDTH;
  yfactor = (double)height / DEFAULT_HEIGHT;
  if (!DoubleEqual(xfactor, xFactorM) || !DoubleEqual(yfactor, yFactorM)) {
     xFactorM = xfactor;
     yFactorM = yfactor;
     Populate();
     }
}

bool cSoppalusikkaSymbolCache::Populate(void)
{
  debug1("%s xfactor=%.02f yfactor=%.02f ", __PRETTY_FUNCTION__, xFactorM, yFactorM);
  if (!DoubleEqual(0.0, xFactorM) || !DoubleEqual(0.0, yFactorM)) {
     Flush();

     // pushing order must follow the enumeration - keep original proportions
     cacheM.Append(bmOnePixelS.Scaled(yFactorM, yFactorM, antiAliasM));       // SYMBOL_ONEPIXEL
     cacheM.Append(bmAudioS.Scaled(yFactorM, yFactorM, antiAliasM));          // SYMBOL_AUDIO
     cacheM.Append(bmAudioLeftS.Scaled(yFactorM, yFactorM, antiAliasM));      // SYMBOL_AUDIO_LEFT
     cacheM.Append(bmAudioRightS.Scaled(yFactorM, yFactorM, antiAliasM));     // SYMBOL_AUDIO_RIGHT
     cacheM.Append(bmDolbyDigitalS.Scaled(yFactorM, yFactorM, antiAliasM));   // SYMBOL_DOLBY_DIGITAL
     cacheM.Append(bmEncryptedS.Scaled(yFactorM, yFactorM, antiAliasM));      // SYMBOL_ENCRYPTED
     cacheM.Append(bmFastForwardS.Scaled(yFactorM, yFactorM, antiAliasM));    // SYMBOL_FFWD
     cacheM.Append(bmFastForward1S.Scaled(yFactorM, yFactorM, antiAliasM));   // SYMBOL_FFWD1
     cacheM.Append(bmFastForward2S.Scaled(yFactorM, yFactorM, antiAliasM));   // SYMBOL_FFWD2
     cacheM.Append(bmFastForward3S.Scaled(yFactorM, yFactorM, antiAliasM));   // SYMBOL_FFWD3
     cacheM.Append(bmFastReverseS.Scaled(yFactorM, yFactorM, antiAliasM));    // SYMBOL_FREW
     cacheM.Append(bmFastReverse1S.Scaled(yFactorM, yFactorM, antiAliasM));   // SYMBOL_FREW1
     cacheM.Append(bmFastReverse2S.Scaled(yFactorM, yFactorM, antiAliasM));   // SYMBOL_FREW2
     cacheM.Append(bmFastReverse3S.Scaled(yFactorM, yFactorM, antiAliasM));   // SYMBOL_FREW3
     cacheM.Append(bmPauseS.Scaled(yFactorM, yFactorM, antiAliasM));          // SYMBOL_PAUSE
     cacheM.Append(bmPlayS.Scaled(yFactorM, yFactorM, antiAliasM));           // SYMBOL_PLAY
     cacheM.Append(bmRadioS.Scaled(yFactorM, yFactorM, antiAliasM));          // SYMBOL_RADIO
     cacheM.Append(bmRecordingS.Scaled(yFactorM, yFactorM, antiAliasM));      // SYMBOL_RECORDING
     cacheM.Append(bmRunningS.Scaled(yFactorM, yFactorM, antiAliasM));        // SYMBOL_RUNNING
     cacheM.Append(bmSlowForwardS.Scaled(yFactorM, yFactorM, antiAliasM));    // SYMBOL_SFWD
     cacheM.Append(bmSlowForward1S.Scaled(yFactorM, yFactorM, antiAliasM));   // SYMBOL_SFWD1
     cacheM.Append(bmSlowForward2S.Scaled(yFactorM, yFactorM, antiAliasM));   // SYMBOL_SFWD2
     cacheM.Append(bmSlowForward3S.Scaled(yFactorM, yFactorM, antiAliasM));   // SYMBOL_SFWD3
     cacheM.Append(bmSlowReverseS.Scaled(yFactorM, yFactorM, antiAliasM));    // SYMBOL_SREW
     cacheM.Append(bmSlowReverse1S.Scaled(yFactorM, yFactorM, antiAliasM));   // SYMBOL_SREW1
     cacheM.Append(bmSlowReverse2S.Scaled(yFactorM, yFactorM, antiAliasM));   // SYMBOL_SREW2
     cacheM.Append(bmSlowReverse3S.Scaled(yFactorM, yFactorM, antiAliasM));   // SYMBOL_SREW3
     cacheM.Append(bmTeletextS.Scaled(yFactorM, yFactorM, antiAliasM));       // SYMBOL_TELETEXT
     cacheM.Append(bmVpsS.Scaled(yFactorM, yFactorM, antiAliasM));            // SYMBOL_VPS
     cacheM.Append(bmEventPartTimerS.Scaled(yFactorM, yFactorM, antiAliasM)); // SYMBOL_EVENT_PARTTIMER
     cacheM.Append(bmEventTimerS.Scaled(yFactorM, yFactorM, antiAliasM));     // SYMBOL_EVENT_TIMER
     cacheM.Append(bmEventVpsS.Scaled(yFactorM, yFactorM, antiAliasM));       // SYMBOL_EVENT_VPS
     cacheM.Append(bmEventRunningS.Scaled(yFactorM, yFactorM, antiAliasM));   // SYMBOL_EVENT_RUNNING
     cacheM.Append(bmEventRecordingS.Scaled(yFactorM, yFactorM, antiAliasM)); // SYMBOL_EVENT_RECORDING
     cacheM.Append(bmRecordingNewS.Scaled(yFactorM, yFactorM, antiAliasM));   // SYMBOL_RECORDING_NEW

     return true;
     }

  return false;
}

bool cSoppalusikkaSymbolCache::Flush(void)
{
  debug1("%s", __PRETTY_FUNCTION__);

  for (int i = 0; i < cacheM.Size(); ++i) {
      cBitmap *bmp = cacheM[i];
      DELETENULL(bmp);
      }
  cacheM.Clear();

  return true;
}

cBitmap& cSoppalusikkaSymbolCache::Get(eSymbols symbolP)
{
  cBitmap *bitmapM = cacheM[SYMBOL_ONEPIXEL];

  if (symbolP < cacheM.Size())
     bitmapM = cacheM[symbolP];
  else
     error("%s Invalid symbol %d", __PRETTY_FUNCTION__, symbolP);

  return *bitmapM;
}
