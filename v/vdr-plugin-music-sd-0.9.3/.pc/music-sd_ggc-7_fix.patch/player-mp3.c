/*
 * MP3/MPlayer plugin to VDR (C++)
 *
 * (C) 2001-2005 Stefan Huelswitt <s.huelswitt@gmx.de>
 *
 * This code is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * Or, point your browser to http://www.gnu.org/copyleft/gpl.html
 */

#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <math.h>
#include <glob.h>
#include <linux/types.h>
#include <sys/soundcard.h>

#include <id3tag.h>
#include <vdr/player.h>
#include <vdr/ringbuffer.h>
#include <vdr/thread.h>
#include <vdr/tools.h>
#include <vdr/plugin.h>

#include "i18n.h"
#include "common.h"
#include "setup-mp3.h"
#include "setup.h"
#include "player-mp3.h"
#include "data-mp3.h"
#include "decoder.h"
#include "decoder-core.h"
#include "skin.h"
#include "visual.h"
#include "vars.h"

#ifndef NO_DEBUG
#define DEBUG_MODE      // debug playmode changes
#define DEBUG_BGR       // debug backround scan thread
#define DEBUG_DELAY 300 // debug write/decode delays
//#define ACC_DUMP        // dump limiter lookup table to /tmp/limiter
#endif

#if !defined(NO_DEBUG) && defined(DEBUG_MODE)
#define dm(x) { (x); }
#else
#define dm(x) ; 
#endif

#if !defined(NO_DEBUG) && defined(DEBUG_BGR)
#define db(x) { (x); }
#else
#define db(x) ; 
#endif

// ----------------------------------------------------------------

#define MP3BUFSIZE (1024*1024)                               // output ringbuffer size
#define OUT_BITS 16                                          // output 16 bit samples to DVB driver
#define OUT_FACT (OUT_BITS/8*2)                              // output factor is 16 bit & 2 channels -> 4 bytes
// cResample
#define MAX_NSAMPLES (1152*7)                                // max. buffer for resampled frame
// cNormalize
#define MIN_GAIN   0.03                                      // min. gain required to launch the normalizer
#define MAX_GAIN   3.0                                       // max. allowed gain
#define USE_FAST_LIMITER
#define LIM_ACC    12                                        // bit, accuracy for lookup table
#define F_LIM_MAX  (mad_fixed_t)((1<<(MAD_F_FRACBITS+2))-1)  // max. value covered by lookup table
#define LIM_SHIFT  (MAD_F_FRACBITS-LIM_ACC)                  // shift value for table lookup
#define F_LIM_JMP  (mad_fixed_t)(1<<LIM_SHIFT)               // lookup table jump between values
// cLevel
#define POW_WIN 100                                          // window width for smoothing power values
#define EPSILON 0.00000000001                                // anything less than EPSILON is considered zero


// --- cResample ------------------------------------------------------------

// The resample code has been adapted from the madplay project
// (resample.c) found in the libmad distribution
   
class cResample {
private:
  mad_fixed_t ratio;
  mad_fixed_t step;
  mad_fixed_t last;
  mad_fixed_t resampled[MAX_NSAMPLES];
public:
  bool SetInputRate(unsigned int oldrate, unsigned int newrate);
  unsigned int ResampleBlock(unsigned int nsamples, const mad_fixed_t *old);
  const mad_fixed_t *Resampled(void) { return resampled; }
  };

bool cResample::SetInputRate(unsigned int oldrate, unsigned int newrate)
{
  if(oldrate<8000 || oldrate>newrate*6) { // out of range
    esyslog("music-sd: WARNING: samplerate %d out of range 8000-%d\n",oldrate,newrate*6);
    return 0;
    }
  ratio=mad_f_tofixed((double)oldrate/(double)newrate);
  step=0; last=0;
#ifdef DEBUG
  static mad_fixed_t oldratio=0;
  if(oldratio!=ratio) {
    printf("music-sd: mad: new resample ratio %f (from %d kHz to %d kHz)\n",mad_f_todouble(ratio),oldrate,newrate);
    oldratio=ratio;
    }
#endif
  return ratio!=MAD_F_ONE;
}

unsigned int cResample::ResampleBlock(unsigned int nsamples, const mad_fixed_t *old)
{
  // This resampling algorithm is based on a linear interpolation, which is
  // not at all the best sounding but is relatively fast and efficient.
  //
  // A better algorithm would be one that implements a bandlimited
  // interpolation.

  mad_fixed_t *nsam=resampled;
  const mad_fixed_t *end=old+nsamples;
  const mad_fixed_t *begin=nsam;

  if(step < 0) {
    step = mad_f_fracpart(-step);

    while (step < MAD_F_ONE) {
      *nsam++ = step ? last+mad_f_mul(*old-last,step) : last;
      step += ratio;
      if(((step + 0x00000080L) & 0x0fffff00L) == 0)
	step = (step + 0x00000080L) & ~0x0fffffffL;
      }
    step -= MAD_F_ONE;
    }

  while (end - old > 1 + mad_f_intpart(step)) {
    old += mad_f_intpart(step);
    step = mad_f_fracpart(step);
    *nsam++ = step ? *old + mad_f_mul(old[1] - old[0], step) : *old;
    step += ratio;
    if (((step + 0x00000080L) & 0x0fffff00L) == 0)
      step = (step + 0x00000080L) & ~0x0fffffffL;
    }

  if (end - old == 1 + mad_f_intpart(step)) {
    last = end[-1];
    step = -step;
    }
  else step -= mad_f_fromint(end - old);

  return nsam-begin;
}

// --- cLevel ----------------------------------------------------------------

// The normalize algorithm and parts of the code has been adapted from the
// Normalize 0.7 project. (C) 1999-2002, Chris Vaill <cvaill@cs.columbia.edu>

// A little background on how normalize computes the volume
// of a wav file, in case you want to know just how your
// files are being munged:
//
// The volumes calculated are RMS amplitudes, which corre�
// spond (roughly) to perceived volume. Taking the RMS ampli�
// tude of an entire file would not give us quite the measure
// we want, though, because a quiet song punctuated by short
// loud parts would average out to a quiet song, and the
// adjustment we would compute would make the loud parts
// excessively loud.
//
// What we want is to consider the maximum volume of the
// file, and normalize according to that. We break up the
// signal into 100 chunks per second, and get the signal
// power of each chunk, in order to get an estimation of
// "instantaneous power" over time. This "instantaneous
// power" signal varies too much to get a good measure of the
// original signal's maximum sustained power, so we run a
// smoothing algorithm over the power signal (specifically, a
// mean filter with a window width of 100 elements). The max�
// imum point of the smoothed power signal turns out to be a
// good measure of the maximum sustained power of the file.
// We can then take the square root of the power to get maxi�
// mum sustained RMS amplitude.

class cLevel {
private:
  double maxpow;
  mad_fixed_t peak;
  struct Power {
    // smooth
    int npow, wpow;
    double powsum, pows[POW_WIN];
    // sum
    unsigned int nsum;
    double sum;
    } power[2];
  //
  inline void AddPower(struct Power *p, double pow);
public:
  void Init(void);
  void GetPower(struct mad_pcm *pcm);
  double GetLevel(void);
  double GetPeak(void);
  };

void cLevel::Init(void)
{
  for(int l=0 ; l<2 ; l++) {
    struct Power *p=&power[l];
    p->sum=p->powsum=0.0; p->wpow=p->npow=p->nsum=0;
    for(int i=POW_WIN-1 ; i>=0 ; i--) p->pows[i]=0.0;
    }
  maxpow=0.0; peak=0;
}

void cLevel::GetPower(struct mad_pcm *pcm)
{
  for(int i=0 ; i<pcm->channels ; i++) {
    struct Power *p=&power[i];
    mad_fixed_t *data=pcm->samples[i];
    for(int n=pcm->length ; n>0 ; n--) {
      if(*data < -peak) peak = -*data;
      if(*data >  peak) peak =  *data;
      double s=mad_f_todouble(*data++);
      p->sum+=(s*s);
      if(++(p->nsum)>=pcm->samplerate/100) {
        AddPower(p,p->sum/(double)p->nsum);
        p->sum=0.0; p->nsum=0;
        }
      }
    }
}

void cLevel::AddPower(struct Power *p, double pow)
{
  p->powsum+=pow;
  if(p->npow>=POW_WIN) {
    if(p->powsum>maxpow) maxpow=p->powsum;
    p->powsum-=p->pows[p->wpow];
    }
  else p->npow++;
  p->pows[p->wpow]=pow;
  p->wpow=(p->wpow+1) % POW_WIN;
}

double cLevel::GetLevel(void)
{
  if(maxpow<EPSILON) {
    // Either this whole file has zero power, or was too short to ever
    // fill the smoothing buffer.  In the latter case, we need to just
    // get maxpow from whatever data we did collect.

    if(power[0].powsum>maxpow) maxpow=power[0].powsum;
    if(power[1].powsum>maxpow) maxpow=power[1].powsum;
    }
  double level=sqrt(maxpow/(double)POW_WIN);     // adjust for the smoothing window size and root
  d(printf("music-sd: norm: new volumen level=%f peak=%f\n",level,mad_f_todouble(peak)))
  return level;
}

double cLevel::GetPeak(void)
{
  return mad_f_todouble(peak);
}

// --- cNormalize ------------------------------------------------------------

class cNormalize {
private:
  mad_fixed_t gain;
  double d_limlvl, one_limlvl;
  mad_fixed_t limlvl;
  bool dogain, dolimit;
#ifdef DEBUG
  // stats
  unsigned long limited, clipped, total;
  mad_fixed_t peak;
#endif
  // limiter
#ifdef USE_FAST_LIMITER
  mad_fixed_t *table, tablestart;
  int tablesize;
  inline mad_fixed_t FastLimiter(mad_fixed_t x);
#endif
  inline mad_fixed_t Limiter(mad_fixed_t x);
public:
  cNormalize(void);
  ~cNormalize();
  void Init(double Level, double Peak);
  void Stats(void);
  void AddGain(struct mad_pcm *pcm);
  };

cNormalize::cNormalize(void)
{
  d_limlvl=(double)MP3Setup.LimiterLevel/100.0;
  one_limlvl=1-d_limlvl;
  limlvl=mad_f_tofixed(d_limlvl);
  d(printf("music-sd: norm: lim_lev=%f lim_acc=%d\n",d_limlvl,LIM_ACC))

#ifdef USE_FAST_LIMITER
  mad_fixed_t start=limlvl & ~(F_LIM_JMP-1);
  tablestart=start;
  tablesize=(unsigned int)(F_LIM_MAX-start)/F_LIM_JMP + 2;
  table=new mad_fixed_t[tablesize];
  if(table) {
    d(printf("music-sd: norm: table size=%d start=%08x jump=%08x\n",tablesize,start,F_LIM_JMP))
    for(int i=0 ; i<tablesize ; i++) {
      table[i]=Limiter(start);
      start+=F_LIM_JMP;
      }
    tablesize--; // avoid a -1 in FastLimiter()

    // do a quick accuracy check, just to be sure that FastLimiter() is working
    // as expected :-)
#ifdef ACC_DUMP
    FILE *out=fopen("/tmp/limiter","w");
#endif
    mad_fixed_t maxdiff=0;
    for(mad_fixed_t x=F_LIM_MAX ; x>=limlvl ; x-=mad_f_tofixed(1e-4)) {
      mad_fixed_t diff=mad_f_abs(Limiter(x)-FastLimiter(x));
      if(diff>maxdiff) maxdiff=diff;
#ifdef ACC_DUMP
      fprintf(out,"%0.10f\t%0.10f\t%0.10f\t%0.10f\t%0.10f\n",
        mad_f_todouble(x),mad_f_todouble(Limiter(x)),mad_f_todouble(FastLimiter(x)),mad_f_todouble(diff),mad_f_todouble(maxdiff));
      if(ferror(out)) break;
#endif
      }
#ifdef ACC_DUMP
    fclose(out);
#endif
    d(printf("music-sd: norm: accuracy %.12f\n",mad_f_todouble(maxdiff)))
    if(mad_f_todouble(maxdiff)>1e-6) {
      esyslog("music-sd: ERROR: accuracy check failed, normalizer disabled");
      delete table; table=0;
      }
    }
  else esyslog("music-sd: ERROR: no memory for lookup table, normalizer disabled");
#endif // USE_FAST_LIMITER
}

cNormalize::~cNormalize()
{
#ifdef USE_FAST_LIMITER
  delete[] table;
#endif
}

void cNormalize::Init(double Level, double Peak)
{
  double Target=(double)MP3Setup.TargetLevel/100.0;
  double dgain=Target/Level;
  if(dgain>MAX_GAIN) dgain=MAX_GAIN;
  gain=mad_f_tofixed(dgain);
  // Check if we actually need to apply a gain
  dogain=(Target>0.0 && fabs(1-dgain)>MIN_GAIN);
#ifdef USE_FAST_LIMITER
  if(!table) dogain=false;
#endif
  // Check if we actually need to do limiting:
  // we have to if limiter is enabled, if gain>1 and if the peaks will clip.
  dolimit=(d_limlvl<1.0 && dgain>1.0 && Peak*dgain>1.0);
#ifdef DEBUG
  printf("music-sd: norm: gain=%f dogain=%d dolimit=%d (target=%f level=%f peak=%f)\n",dgain,dogain,dolimit,Target,Level,Peak);
  limited=clipped=total=0; peak=0;
#endif
}

void cNormalize::Stats(void)
{
#ifdef DEBUG
  if(total)
    printf("music-sd: norm: stats tot=%ld lim=%ld/%.3f%% clip=%ld/%.3f%% peak=%.3f\n",
           total,limited,(double)limited/total*100.0,clipped,(double)clipped/total*100.0,mad_f_todouble(peak));
#endif
}

mad_fixed_t cNormalize::Limiter(mad_fixed_t x)
{
// Limiter function:
//
//        / x                                                (for x <= lev)
//   x' = |
//        \ tanh((x - lev) / (1-lev)) * (1-lev) + lev        (for x > lev)
//
// call only with x>=0. For negative samples, preserve sign outside this function
//
// With limiter level = 0, this is equivalent to a tanh() function;
// with limiter level = 1, this is equivalent to clipping.

  if(x>limlvl) {
#ifdef DEBUG
    if(x>MAD_F_ONE) clipped++;
    limited++;
#endif
    x=mad_f_tofixed(tanh((mad_f_todouble(x)-d_limlvl) / one_limlvl) * one_limlvl + d_limlvl);
    }
  return x;
}

#ifdef USE_FAST_LIMITER
mad_fixed_t cNormalize::FastLimiter(mad_fixed_t x)
{
// The fast algorithm is based on a linear interpolation between the
// the values in the lookup table. Relays heavly on libmads fixed point format.

  if(x>limlvl) {
    int i=(unsigned int)(x-tablestart)/F_LIM_JMP;
#ifdef DEBUG
    if(x>MAD_F_ONE) clipped++;
    limited++;
    if(i>=tablesize) printf("music-sd: norm: overflow x=%f x-ts=%f i=%d tsize=%d\n",
                            mad_f_todouble(x),mad_f_todouble(x-tablestart),i,tablesize);
#endif
    mad_fixed_t r=x & (F_LIM_JMP-1);
    x=MAD_F_ONE;
    if(i<tablesize) {
      mad_fixed_t *ptr=&table[i];
      x=*ptr;
      mad_fixed_t d=*(ptr+1)-x;
      //x+=mad_f_mul(d,r)<<LIM_ACC;                // this is not accurate as mad_f_mul() does >>MAD_F_FRACBITS
                                                   // which is senseless in the case of following <<LIM_ACC.
      x+=((long long)d*(long long)r)>>LIM_SHIFT;   // better, don't know if works on all machines
      }
    }
  return x;
}
#endif

#ifdef USE_FAST_LIMITER
#define LIMITER_FUNC FastLimiter
#else
#define LIMITER_FUNC Limiter
#endif

void cNormalize::AddGain(struct mad_pcm *pcm)
{
  if(dogain) {
    for(int i=0 ; i<pcm->channels ; i++) {
      mad_fixed_t *data=pcm->samples[i];
#ifdef DEBUG
      total+=pcm->length;
#endif
      if(dolimit) {
        for(int n=pcm->length ; n>0 ; n--) {
          mad_fixed_t s=mad_f_mul(*data,gain);
          if(s<0) {
            s=-s;
#ifdef DEBUG
            if(s>peak) peak=s;
#endif
            s=LIMITER_FUNC(s);
            s=-s;
            }
          else {
#ifdef DEBUG
            if(s>peak) peak=s;
#endif
            s=LIMITER_FUNC(s);
            }
          *data++=s;
          }
        }
      else {
        for(int n=pcm->length ; n>0 ; n--) {
          mad_fixed_t s=mad_f_mul(*data,gain);
#ifdef DEBUG
          if(s>peak) peak=s;
          else if(-s>peak) peak=-s;
#endif
          if(s>MAD_F_ONE) s=MAD_F_ONE;   // do clipping
          if(s<-MAD_F_ONE) s=-MAD_F_ONE;
          *data++=s;
          }
        }
      }
    }
}

// --- cScale ----------------------------------------------------------------

// The dither code has been adapted from the madplay project
// (audio.c) found in the libmad distribution

enum eAudioMode { amRoundBE, amDitherBE, amRoundLE, amDitherLE };

class cScale {
private:
  enum { MIN=-MAD_F_ONE, MAX=MAD_F_ONE - 1 };
#ifdef DEBUG
  // audio stats
  unsigned long clipped_samples;
  mad_fixed_t peak_clipping;
  mad_fixed_t peak_sample;
#endif
  // dither
  struct dither {
    mad_fixed_t error[3];
    mad_fixed_t random;
    } leftD, rightD;
  //
  inline mad_fixed_t Clip(mad_fixed_t sample, bool stats=true);
  inline unsigned long Prng(unsigned long state);
  signed long LinearRound(mad_fixed_t sample);
  signed long LinearDither(mad_fixed_t sample, struct dither *dither);
public:
  void Init(void);
  void Stats(void);
  unsigned int ScaleBlock(unsigned char *data, unsigned int size, unsigned int &nsamples, const mad_fixed_t * &left, const mad_fixed_t * &right, eAudioMode mode);
  };

void cScale::Init(void)
{
#ifdef DEBUG
  clipped_samples=0; peak_clipping=peak_sample=0;
#endif
  memset(&leftD,0,sizeof(leftD));
  memset(&rightD,0,sizeof(rightD));
}

void cScale::Stats(void)
{
#ifdef DEBUG
  printf("music-sd: scale stats clipped=%ld peak_clip=%f peak=%f\n",
         clipped_samples,mad_f_todouble(peak_clipping),mad_f_todouble(peak_sample));
#endif
}

// gather signal statistics while clipping
mad_fixed_t cScale::Clip(mad_fixed_t sample, bool stats)
{
#ifndef DEBUG
  if (sample > MAX) sample = MAX;
  if (sample < MIN) sample = MIN;
#else
  if(!stats) {
    if (sample > MAX) sample = MAX;
    if (sample < MIN) sample = MIN;
    }
  else {
    if (sample >= peak_sample) {
      if (sample > MAX) {
        ++clipped_samples;
        if (sample - MAX > peak_clipping)
	  peak_clipping = sample - MAX;
        sample = MAX;
        }
      peak_sample = sample;
      }
    else if (sample < -peak_sample) {
      if (sample < MIN) {
        ++clipped_samples;
        if (MIN - sample > peak_clipping)
	  peak_clipping = MIN - sample;
        sample = MIN;
        }
      peak_sample = -sample;
      }
    }
#endif
  return sample;
}

// generic linear sample quantize routine
signed long cScale::LinearRound(mad_fixed_t sample)
{
  // round
  sample += (1L << (MAD_F_FRACBITS - OUT_BITS));
  // clip
  sample=Clip(sample);
  // quantize and scale
  return sample >> (MAD_F_FRACBITS + 1 - OUT_BITS);
}

// 32-bit pseudo-random number generator
unsigned long cScale::Prng(unsigned long state)
{
  return (state * 0x0019660dL + 0x3c6ef35fL) & 0xffffffffL;
}

// generic linear sample quantize and dither routine
signed long cScale::LinearDither(mad_fixed_t sample, struct dither *dither)
{
  // noise shape
  sample += dither->error[0] - dither->error[1] + dither->error[2];
  dither->error[2] = dither->error[1];
  dither->error[1] = dither->error[0] / 2;
  // bias
  mad_fixed_t output = sample + (1L << (MAD_F_FRACBITS + 1 - OUT_BITS - 1));
  const int scalebits = MAD_F_FRACBITS + 1 - OUT_BITS;
  const mad_fixed_t mask = (1L << scalebits) - 1;
  // dither
  const mad_fixed_t random = Prng(dither->random);
  output += (random & mask) - (dither->random & mask);
  dither->random = random;
  // clip
  output=Clip(output);
  sample=Clip(sample,false);
  // quantize
  output &= ~mask;
  // error feedback
  dither->error[0] = sample - output;
  // scale
  return output >> scalebits;
}

#define PUT_BE(data,sample) { *data++=(sample)>>8; *data++=(sample)>>0; }
#define PUT_LE(data,sample) { *data++=(sample)>>0; *data++=(sample)>>8; }

// write a block of signed 16-bit PCM samples
unsigned int cScale::ScaleBlock(unsigned char *data, unsigned int size, unsigned int &nsamples, const mad_fixed_t * &left, const mad_fixed_t * &right, eAudioMode mode)
{
  unsigned int len=size/OUT_FACT;
  if(len>nsamples) { len=nsamples; size=len*OUT_FACT; }
  nsamples-=len;
  switch(mode) {
    case amRoundBE:
      while(len--) {
        signed int sample=LinearRound(*left++);
        PUT_BE(data,sample);
        if(right) sample=LinearRound(*right++);
        PUT_BE(data,sample);
        }
      break;
    case amDitherBE:
      while(len--) {
	signed int sample=LinearDither(*left++,&leftD);
        PUT_BE(data,sample);
	if(right) sample=LinearDither(*right++,&rightD);
        PUT_BE(data,sample);
        }
      break;
    case amRoundLE:
      while(len--) {
        signed int sample=LinearRound(*left++);
        PUT_LE(data,sample);
        if(right) sample=LinearRound(*right++);
        PUT_LE(data,sample);
        }
      break;
    case amDitherLE:
      while(len--) {
	signed int sample=LinearDither(*left++,&leftD);
        PUT_LE(data,sample);
	if(right) sample=LinearDither(*right++,&rightD);
        PUT_LE(data,sample);
        }
      break;
    }
 return size;
}

// --- cShuffle ----------------------------------------------------------------

class cShuffle {
private:
  int *shuffle, max;
  unsigned int seed;
  //
  int Index(int pos);
public:
  cShuffle(void);
  ~cShuffle();
  void Shuffle(int num, int curr);
  void Del(int pos);
  void Flush(void);
  int First(void);
  int Next(int curr);
  int Prev(int curr);
  int Goto(int pos, int curr);
  };


cShuffle::cShuffle(void)
{
  shuffle=0; max=0;
  seed=time(0);
}

cShuffle::~cShuffle(void)
{
  Flush();
}

void cShuffle::Flush(void)
{
  delete shuffle; shuffle=0;
  max=0;
}

int cShuffle::Index(int pos)
{
  if(pos>=0)
    for(int i=0; i<max; i++) if(shuffle[i]==pos) return i;
  return -1;
}

void cShuffle::Shuffle(int num, int curr)
{
  int oldmax=0;
  if(num!=max) {
    int *ns=new int[num];
    if(shuffle) {
      if(num>max) {
        memcpy(ns,shuffle,max*sizeof(int));
        oldmax=max;
        }
      delete shuffle;
      }
    shuffle=ns; max=num;
    }
  if(!oldmax) curr=-1;
  for(int i=oldmax ; i<max ; i++) shuffle[i]=i;

  int in=Index(curr)+1; if(in<0) in=0;
  if((max-in)>=2) {
    for(int i=in ; i<max ; i++) {
      int ran=(rand_r(&seed) % ((max-in)*4-4))/4; ran+=((ran+in) >= i);
      int t=shuffle[i];
      shuffle[i]=shuffle[ran+in];
      shuffle[ran+in]=t;
      }
    }
#ifdef DEBUG
  printf("music-sd: shuffle: order (%d , %d -> %d) ",num,curr,in);
  for(int i=0 ; i<max ; i++) printf("%d ",shuffle[i]);
#endif
}

void cShuffle::Del(int pos)
{
  int i=Index(pos);
  if(i>=0) {
    if(i+1<max) memmove(&shuffle[i],&shuffle[i+1],(max-i-1)*sizeof(int));
    max--;
    }
}

int cShuffle::First(void)
{
  return shuffle[0];
}

int cShuffle::Next(int curr)
{
  int i=Index(curr);
  return (i>=0 && i+1<max) ? shuffle[i+1] : -1;
}

int cShuffle::Prev(int curr)
{
  int i=Index(curr);
  return (i>0) ? shuffle[i-1] : -1;
}

int cShuffle::Goto(int pos, int curr)
{
  int i=Index(curr);
  int g=Index(pos);
  if(g>=0) {
    if(g<i) {
      for(int l=g; l<i; l++) shuffle[l]=shuffle[l+1];
      shuffle[i]=pos;
      }
    else if(g>i) {
      for(int l=g; l>i+1; l--) shuffle[l]=shuffle[l-1];
      shuffle[i+1]=pos;
      }
#ifdef DEBUG
    printf("music-sd: shuffle: goto order (%d -> %d , %d -> %d) ",pos,g,curr,i);
    for(int i=0 ; i<max ; i++) printf("%d ",shuffle[i]);
    printf("\n");
#endif
    return pos;
    }
  return -1;
}

// --- cPlayManager ------------------------------------------------------------

#define SCANNED_ID3 1
#define SCANNED_LVL 2

cPlayManager *mgr=0;
char Songname[256];
char coverpicture[256];
bool InStopMode;

cPlayManager::cPlayManager(void)
{
  currindex=-1;
  playnow = false;
  scanning = false;
  curr=0; currIndex=-1;
  scan=0; stopscan=throttle=pass2=release=false;
  play=0; playNew=eol=false;
  tracksadded=InStopMode=false;
  shuffle=new cShuffle;
  loopMode=(MP3Setup.InitLoopMode>0);
  shuffleMode=(MP3Setup.InitShuffleMode>0);
}

cPlayManager::~cPlayManager()
{
  Flush();
  Release();
  listMutex.Lock();
  stopscan=true; bgCond.Broadcast();
  listMutex.Unlock();
  Cancel(2);
  delete shuffle;
}

void cPlayManager::ThrottleWait(void)
{
  while(!stopscan && !release && throttle) {
    db(printf("music-sd: mgr: background scan throttled\n"))
    bgCond.Wait(listMutex);
    db(printf("music-sd: mgr: background scan throttle wakeup\n"))
    }
}

void cPlayManager::Action(void)
{
  db(printf("music-sd: mgr: background scan thread started (pid=%d)\n", getpid()))
  nice(5);
  listMutex.Lock();
  while(!stopscan) {
    for(scan=list.First(); !stopscan && !release && scan; scan=list.Next(scan)) {
      ThrottleWait();
      listMutex.Unlock();
      if(!(scan->user & SCANNED_ID3)) {
        scanning = true;
        db(printf("music-sd: mgr: scanning (id3) %s\n",scan->Name()))
        cSongInfo *si=scan->Info(true);
        if(si && si->Level>0.0) scan->user|=SCANNED_LVL;
        scan->user|=SCANNED_ID3;
// TEST IT
        scanning=false;
        }
      listMutex.Lock();
      }
    if(MP3Setup.BgrScan>1) {
      pass2=true;
      for(scan=list.First(); !stopscan && !release && scan; scan=list.Next(scan)) {
        if(scan==curr) continue;
        ThrottleWait();
        listMutex.Unlock();
        if(!(scan->user & SCANNED_LVL)) {
          cDecoder *dec=scan->Decoder();
          if(dec) {
// TEST IT
            scanning=true;          
            cSongInfo *si=scan->Info(false);
            if(!dec->IsStream() && (!si || si->Level<=0.0) && dec->Start()) {
              db(printf("music-sd: mgr: scanning (lvl) %s\n",scan->Name()))
              cLevel level;
              level.Init();
              bool go=true;
              while(go && !release) {
                if(throttle) {
                  listMutex.Lock(); ThrottleWait(); listMutex.Unlock();
                  continue;
                }
                struct Decode *ds=dec->Decode();
                switch(ds->status) {
                  case dsPlay:
                    level.GetPower(ds->pcm);
                    break;
                  case dsSkip:
                  case dsSoftError:
                    break;
                  case dsEof:
                    {
                    double l=level.GetLevel();
                    if(l>0.0) {
                      cSongInfo *si=dec->SongInfo(false);
                      cFileInfo *fi=dec->FileInfo();
                      if(si && fi) {
                        si->Level=l;
                        si->Peak=level.GetPeak();
                        InfoCache.Cache(si,fi);
                        }
                      }
                    }
                    //fall through
                  case dsOK:
                  case dsError:
                    scan->user|=SCANNED_LVL;
                    go=false;
                    break;
                }
              }
            }
            else scan->user|=SCANNED_LVL;
            dec->Stop();
          }
// TEST IT
          scanning=false;
        }
        listMutex.Lock();
      }
      pass2=false;
    }
    do {
      scan=0; release=false; fgCond.Broadcast();
      db(printf("music-sd: mgr: background scan idle\n"))
      scanning = false;
      bgCond.Wait(listMutex);
      db(printf("music-sd: mgr: background scan idle wakeup\n"))
      scanning = true;
    } while(!stopscan && (release || throttle));
  }
  listMutex.Unlock();
  db(printf("music-sd: mgr: background scan thread ended (pid=%d)\n", getpid()))
//TEST IT  
  scanning=false;
}


bool cPlayManager::Scanning(void)
{
  bool res = false;
  res = scanning;
  
  return res;
}  


bool cPlayManager::SaveList(const char *pl_filename, bool full)  //Path for playlist , save with fullpath
{
  char *buffer;
  bool res=false;
  
  if( FILE *f = fopen ( pl_filename , "w")) {
  
    if(full) {
      for(cSong *song=list.First(); song; song=list.Next(song)) {
        asprintf(&buffer, "%s\n", song->Fullname());
        fprintf(f, buffer);
        d(printf("music-sd: mgr: add track to %s: %s",pl_filename, buffer));
	free(buffer);
        }
      }
    else {
      for(cSong *song=list.First(); song; song=list.Next(song)) {
      asprintf(&buffer, "%s\n", song->BASEDIR());
      fprintf(f, buffer);
      d(printf("music-sd: mgr: add track to %s: %s",pl_filename, buffer));
      free(buffer);
      }
    }	
	
    fclose(f);
    res = true;
  }
  
  return res;
}


void cPlayManager::DeleteRecord(int Sidx)
{

// maxIndex beginnt mit 0
// wenn 1. Lied spielt , dann currIndex = 0
// wenn 1. Lied spielt , dann currindex = 0

  char *buff;
  int sidx=Sidx;

  if(MP3Setup.BgrScan) { 
    stopscan=true;
//    if(Active()) Stop();
  }
  bgCond.Broadcast();

  if(!maxIndex==0 && !shuffleMode) {
    cMutexLock lock(&listMutex);

   cSong *ns=curr;
	
  if(sidx<0) { sidx=currIndex; ns=curr; }
  else      { ns=list.Get(sidx); }

    if(sidx==maxIndex && currIndex==maxIndex) {
      Goto(1);
      currIndex=0;
      currindex = 0;
      }
	else if(sidx == currIndex){
	  Next();
      if(!currIndex==0) {currIndex=currIndex -1; currindex = currIndex;}
    }
    else if(sidx<currIndex) {
      currIndex=currIndex -1; currindex = currindex -1;
    }	  
    else {
	}
    list.Del(ns,true);
    maxIndex=list.Count()-1;
    if(MP3Setup.BgrScan) { 
      stopscan=false; if(!Active()) Start();
      }
    else
      stopscan=true;
    bgCond.Broadcast();

	//save current playlist to file
    asprintf(&buff, "%s/%s", BaseSource, "@current.m3u");
    if(SaveList(buff, false)) {
      isyslog("music-sd: ctrl: refreshed playlist to file: %s\n", buff);
      }
    else
      esyslog("music-sd: ctrl: couldn't save current playlist to file");
    free(buff);
  }      
}


void cPlayManager::Throttle(bool thr)
{
  if(MP3Setup.BgrScan) {
    if(!thr && throttle) {
      db(printf("music-sd: mgr: bgr-scan -> run (%d)\n",time_ms()))
      listMutex.Lock();
      throttle=false; bgCond.Broadcast();
      listMutex.Unlock();
      }
    if(thr && !throttle) {
      db(printf("music-sd: mgr: bgr-scan -> throttle (%d)\n",time_ms()))
      throttle=true;
    }
  }
}

void cPlayManager::ToggleShuffle(void)
{
  shuffleMode=!shuffleMode;
  d(printf("music-sd: mgr: shuffle mode toggled : %d\n",shuffleMode))
  if(shuffleMode && !eol) {
    curr=0; currIndex=-1;
    shuffle->Shuffle(maxIndex+1,-1);
    Next();
  }
}

void cPlayManager::ToggleLoop(void)
{
  loopMode=!loopMode;
  d(printf("music-sd: mgr: loop mode toggled : %d\n",loopMode))
}


bool cPlayManager::Info(int num, cMP3PlayInfo *pi)
{
  cSong *s;

  int idx=num-1;
  if(idx<0) { idx=currIndex; s=curr; }
  else      { s=list.Get(idx); }
  memset(pi,0,sizeof(*pi));
  pi->Num=idx+1;
  pi->MaxNum=maxIndex+1;
  pi->Loop=loopMode;
  pi->Shuffle=shuffleMode;
  bool res=false;

  if(s) {
    strn0cpy(pi->Title,s->Name(),sizeof(pi->Title));
    strn0cpy(pi->Filename,s->FullPath(),sizeof(pi->Filename));
    cSongInfo *si=s->Info(false);
    if(si && si->HasInfo()) {
      static const char *modestr[] = { "Mono","Dual","Joint-Stereo","Stereo" };

      if(si->Title)       strn0cpy(pi->Title,si->Title,sizeof(pi->Title));
      if(si->Artist)      strn0cpy(pi->Artist,si->Artist,sizeof(pi->Artist));
      if(si->Album)       strn0cpy(pi->Album,si->Album,sizeof(pi->Album));
      if(si->Genre)       strn0cpy(pi->Genre,si->Genre,sizeof(pi->Genre));
      if(si->Comment)     strn0cpy(pi->Comment,si->Comment,sizeof(pi->Comment));
      strn0cpy(pi->SMode,modestr[si->ChMode],sizeof(pi->SMode));
      pi->Year=si->Year;
      pi->Rating=si->Rating;
      pi->SampleFreq=si->SampleFreq;
      pi->Bitrate=si->Bitrate;
      pi->MaxBitrate=si->MaxBitrate;
      res=true;
      }
    }
  pi->Hash=MakeHashBuff((char *)pi,(char *)&pi->Loop-(char *)pi);
  return res;
}

/*
char *cPlayManager::TrackInfo(int index, bool toggleSort) {

  int idx = index;
  cSong *s;
  std::string result(tr("Unknown"));
  static char tmpFile[256];
  char *res = NULL;

  s = (idx>=0) ? list.Get(idx) :0;  

  cSongInfo *si=s->Info(false);
  if(!si)  si=s->Info();

  if(si && si->HasInfo()) {
    if(!toggleSort) {
      if(si->Artist) result = si->Artist;
      result = result + "  -  ";    
      if(si->Title) 
        result = result + si->Title;
      else
        result = result + tr("Unknown");
      }
    else {
      if(si->Title) result = si->Title;
      result = result + "  -  ";    
      if(si->Artist) 
        result = result + si->Artist;
      else
        result = result + tr("Unknown");
    }
  }
  else {
    result = result + "  -  ";
    result = result + tr("Unknown");
  }  
 
  strncpy(tmpFile,result.c_str(),sizeof(tmpFile));
  res = tmpFile;
  
  return res;
}
*/

void cPlayManager::RateSong(int sz_rating, cSong *r_Song) {
  cSong *s;
//  s=curr;

  s = r_Song;

  cSongInfo *si=s->Info(false);
  if(!si) si=s->Info();
  cFileInfo *fi= new cFileInfo(s->FullPath());

  if(si && fi) {
    if(sz_rating>=0) si->Rating = sz_rating;
    InfoCache.Update(sz_rating, fi);
  }

  delete fi;
} 


void cPlayManager::UpdateSong(const char *sz_artist, const char *sz_album, const char *sz_title, const char *sz_genre, int sz_year, int sz_rating, cSong *r_Song) {
  cSong *s;

  s = r_Song;
  cSongInfo *si=s->Info(false);
  if(!si) si=s->Info();
  cFileInfo *fi= new cFileInfo(s->FullPath());
  if(si && fi) {
    if(sz_artist && si->Artist)     strcpy(si->Artist,sz_artist);
    if(sz_album && si->Album)       strcpy(si->Album ,sz_album);
    if(sz_title && si->Title)       strcpy(si->Title ,sz_title);
    if(sz_genre && si->Genre)       strcpy(si->Genre ,sz_genre);
    if(sz_year>=0)      si->Year   = sz_year;
    if(sz_rating>=0)  si->Rating = sz_rating;
  }
    delete fi;
} 


int cPlayManager::GetListLength(void) {
  length = 0;
  for(cSong *song=list.First(); song; song=list.Next(song)) {
    cSongInfo *si=song->Info(false);
    if(si && si->HasInfo()) {
      if(si->Total) { length = length + si->Total; }
    }  
  }
  
  return length;
}


void cPlayManager::Add(cPlayList *pl)
{
  char *buf;

  d(printf("music-sd: mgr: Add1: maxIndex: %d , currIndex-> %d\n",maxIndex, currIndex))

  cMutexLock lock(&listMutex);
  bool real=false;
  for(cSong *song=pl->First(); song; song=pl->cList<cSong>::Next(song)) {
    cSong *ns=new cSong(song);

    list.Add(ns);
    real=true;
    }
  if(real) {
    if(MP3Setup.BgrScan) { stopscan=false; if(!Active()) Start(); }
    else stopscan=true;
    bgCond.Broadcast();
    maxIndex=list.Count()-1;

    d(printf("music-sd: mgr: Add2: maxIndex: %d , currIndex-> %d\n",maxIndex, currIndex))

    if(shuffleMode) shuffle->Shuffle(maxIndex+1,currIndex);
    if(!curr) Next();
    else
      if(InStopMode) {
        Next();
        tracksadded = true;
      }	
  }
  
    // Save current playlist to file  
    asprintf(&buf, "%s/%s", BaseSource, "@current.m3u");
    if(SaveList(buf, false)) {
      isyslog("music-sd: ctrl: refreshed playlist to file: %s\n", buf);
      }
    else
      esyslog("music-sd: ctrl: couldnt save current playlist to file");
    free(buf);  
}

void cPlayManager::Flush(void)
{
  cMutexLock lock(&listMutex);
  Halt();
  list.Clear();
  shuffle->Flush();
}

void cPlayManager::Halt(void)
{
  cMutexLock lock(&listMutex);
  curr=0; currIndex=-1; maxIndex=-1; currindex = maxIndex;
  playNew=true;
  stopscan=true; bgCond.Broadcast();
  NoScan(0);
  NoPlay(0);
}

void cPlayManager::NoScan(cSong *nono)
{
  // call with listMutex locked!!
  while((nono && pass2 && scan==nono) || (!nono && scan)) {
    release=true; bgCond.Broadcast();
    d(printf("music-sd: mgr: waiting for bgr release ... (pass2=%d nono=%p scan=%p)\n",pass2,nono,scan))
    fgCond.Wait(listMutex);
    }
}

void cPlayManager::NoPlay(cSong *nono)
{
  // call with listMutex locked!!
  while((nono && play==nono) || (!nono && play)) {
    playNew=true;
    fgCond.Wait(listMutex);
    }
}

bool cPlayManager::Next(void)
{
  cMutexLock lock(&listMutex);
  int ni;
  cSong *n;
  if(shuffleMode) {
    if(curr) {
      ni=shuffle->Next(currIndex);
      if(ni<0) {
        if(loopMode || eol) {
          shuffle->Shuffle(maxIndex+1,-1);
          ni=shuffle->First();
          }
        else eol=true;
        }
      }
    else
      ni=shuffle->First();
    n=(ni>=0) ? list.Get(ni) : 0;
    }
  else {
    if(curr) {
      n=list.cList<cSong>::Next(curr);
      if(!n) {
        if(loopMode || eol) n=list.First();
        else eol=true;
        }
      }
    else
      n=list.First();
    ni=n ? n->Index() : -1;
    }
  if(n) {
    curr=n; currIndex=ni;
    playNew=true; eol=false;

    d(printf("music-sd: mgr: Next(): maxIndex: %d , currIndex: %d ,  currindex: %d\n",maxIndex, currIndex, currindex))

    tracksadded = false;

    return true;
  }

  tracksadded = false;
  return false;
}

bool cPlayManager::Prev(void)
{
  cMutexLock lock(&listMutex);
  int ni;
  cSong *n;
  if(shuffleMode) {
    ni=shuffle->Prev(currIndex);
    n=(ni>=0) ? list.Get(ni) : 0;
    }
  else {
    n=list.cList<cSong>::Prev(curr);
    ni=n ? n->Index() : -1;
    }
  if(n) {
    curr=n; currIndex=ni;
    playNew=true; eol=false;
    d(printf("music-sd: mgr: prev Track-> %d , Playlistindex-> %d\n",currIndex+1, currIndex))
    return true;
  }

  return false;
}

void cPlayManager::Goto(int num)
{
  cMutexLock lock(&listMutex);
  if(num>0 && num<=maxIndex+1) {
    int idx=num-1;
    if(shuffleMode) {
      if(eol) {
        shuffle->Shuffle(maxIndex+1,-1);
        currIndex=shuffle->Goto(idx,-1);
        }
      else
        currIndex=shuffle->Goto(idx,currIndex);
      }
    else
      currIndex=idx;

    curr=(currIndex>=0) ? list.Get(currIndex) : 0;

    playNew=true; eol=false;
    d(printf("music-sd: mgr: goto Track-> %d , Playlistindex-> %d\n",currIndex + 1, currIndex))
  }
}


cSong *cPlayManager::Track(int index)
{
  cSong *s;  
  s =list.Get(index);  
  return s;
}


cSong *cPlayManager::Current(void)
{
  cMutexLock lock(&listMutex);
  if(!play) {
    NoScan(curr);
    play=curr;
    playNew=false;

    if(play){
      d(printf("music-sd: mgr: playing %s\n",play->Name()))
      }
    else d(printf("music-sd: mgr: nothing to play\n"))

    fgCond.Broadcast();
    }
  return play;
}

bool cPlayManager::NextCurrent(void)
{
  cMutexLock lock(&listMutex);
  return (!eol && (playNew || Next() ) );
}

bool cPlayManager::NewCurrent(void)
{
  return playNew;
}

bool cPlayManager::TracksAdded(void)
{
  return tracksadded;
}

void cPlayManager::Release(void)
{
  cMutexLock lock(&listMutex);
  play=0;
  fgCond.Broadcast();
}


// --- cOutput -----------------------------------------------------------------

struct FrameHeader {
  unsigned int samplerate;
  };
#define FHS sizeof(struct FrameHeader)

class cOutput {
protected:
  cMP3Player *player;
  cScale scale;
public:
  cOutput(cMP3Player *Player);
  virtual ~cOutput() {}
  virtual void Init(void);
  virtual unsigned int SampleRate(unsigned int PcmSampleRate)=0;
  virtual cFrame *MakeFrame(unsigned int & Samples, const mad_fixed_t **Data, int index, int sr)=0;
  virtual int Output(const unsigned char *Data, int Len, bool SOF)=0;
  virtual bool Poll(void)=0;
  virtual void Play(void)=0;
  virtual void Pause(void)=0;

#ifdef DEBUG
  virtual void Stats(void);
#endif
  };

cOutput::cOutput(cMP3Player *Player)
{
  player=Player;
}

void cOutput::Init(void)
{
  scale.Init();
}

#ifdef DEBUG
void cOutput::Stats(void)
{
  scale.Stats();
}
#endif

// --- cOutputDvb --------------------------------------------------------------

/*
struct LPCMHeader { int id:8;              // id
                    int frame_count:8;     // number of frames
                    int access_ptr:16;     // first acces unit pointer, i.e. start of audio frame
                    bool emphasis:1;       // audio emphasis on-off
                    bool mute:1;           // audio mute on-off
                    bool reserved:1;       // reserved
                    int frame_number:5;    // audio frame number
                    int quant_wlen:2;      // quantization word length
                    int sample_freq:2;     // audio sampling frequency (48khz=0, 96khz=1, 44,1khz=2, 32khz=3)
                    bool reserved2:1;      // reserved
                    int chan_count:3;      // number of audio channels - 1 (e.g. stereo = 1)
                    int dyn_range_ctrl:8;  // dynamic range control (0x80 if off)
                    };
*/

#define FRAMESIZE 2048 // max. frame size allowed for DVB driver

class cOutputDvb : public cOutput {
private:
  cPoller poll;
  unsigned int outSr;
  bool only48khz;
public:
  cOutputDvb(cMP3Player *Player);
  virtual unsigned int SampleRate(unsigned int PcmSampleRate);
  virtual cFrame *MakeFrame(unsigned int & Samples, const mad_fixed_t **Data, int index, int sr);
  virtual int Output(const unsigned char *Data, int Len, bool SOF);
  virtual bool Poll(void);
  virtual void Play(void);
  virtual void Pause(void);
  };

cOutputDvb::cOutputDvb(cMP3Player *Player)
:cOutput(Player)
{
  only48khz=MP3Setup.Only48kHz;
  outSr=0;
#if APIVERSNUM == 10318
  cDevice::PrimaryDevice()->SetCurrentAudioTrack(ttDolbyFirst);
#elif APIVERSNUM >= 10319
  cDevice::PrimaryDevice()->SetCurrentAudioTrack(ttAudio);
#endif
  d(printf("music-sd: dvb: using DVB output\n"))
}

unsigned int cOutputDvb::SampleRate(unsigned int PcmSampleRate)
{
  unsigned int samplerate=48000;
  if(!only48khz) {
    switch(PcmSampleRate) { // If one of the supported frequencies, do it without resampling.
      case 96000:           // Select a "even" upsampling frequency if possible, too.
        samplerate=96000;
        break;
      //case 48000: // this is already the default ...
      //  samplerate=48000;
      //  break;
      case 11025:
      case 22050:
      case 44100:
        samplerate=44100;
        break;
      case 8000:
      case 16000:
      case 32000:
        samplerate=32000;
        break;
      }
    }
  return samplerate;
}

cFrame *cOutputDvb::MakeFrame(unsigned int & Samples, const mad_fixed_t **Data, int index, int sr)
{
  static const unsigned char header[] = {
    0x00, // PES header
    0x00,
    0x01,
    0xBD, // private stream
    0x00,
    0x00,
    0x87, // mpeg2, aligned, copyright, original
    0x00, // no pts/dts
    0x00, // PES header data len
    0xA0, // aLPCM header
    0xFF,
    0x00,
    0x04,
    0x00,
    0x01, // 2-channel stereo (n-1)
    0x80  // neutral dynamic range
    };
  cFrame *f=0;
  unsigned char *buff=MALLOC(uchar,FRAMESIZE);
  if(buff) {
    struct FrameHeader *fh=(struct FrameHeader *)buff;
    fh->samplerate=sr;
    memcpy(buff+FHS,header,sizeof(header));
    int srMode;
    switch(sr) {
      default:
      case 48000: srMode=0<<4; break;
      case 96000: srMode=1<<4; break;
      case 44100: srMode=2<<4; break;
      case 32000: srMode=3<<4; break;
      }
    buff[14+FHS]|=srMode;
    unsigned int outlen=scale.ScaleBlock(buff+sizeof(header)+FHS,FRAMESIZE-sizeof(header)-FHS,Samples,Data[0],Data[1],MP3Setup.AudioMode?amDitherBE:amRoundBE);
    if(outlen) {
      // lPCM has 600 fps which is 80 samples at 48kHz per channel
      // Frame size = (sample_rate * quantization * channels)/4800
      buff[10+FHS]=outlen*(4800/16/2)/sr;
      outlen+=(sizeof(header)-6);
      buff[4+FHS]=outlen>>8;
      buff[5+FHS]=outlen;

      // Spectrum Analyzer: Hand over the PCM16-data to SpAn
      int offset = sizeof(header)+FHS;
      Span_SetPcmData_1_0 SetPcmData;
      cPlugin *Plugin = cPluginManager::CallFirstService(SPAN_SET_PCM_DATA_ID, NULL);
      if (Plugin)
      {
           SetPcmData.length = (unsigned int)(outlen-offset);
           // the timestamp (ms) of the frame(s) to be visualized:
           SetPcmData.index = index;
           // tell SpAn the ringbuffer's size for it's internal bookkeeping of the data to be visualized:
           SetPcmData.bufferSize = MP3BUFSIZE;
           SetPcmData.data = buff + offset + 1; // get rid of the header
           SetPcmData.bigEndian = true;
           cPluginManager::CallFirstService(SPAN_SET_PCM_DATA_ID, &SetPcmData);
      }

      f=new cFrame(buff,-(outlen+6+FHS),ftUnknown,index);
      }
    if(!f) free(buff);
    }
  return f;
}

#ifdef BROKEN_PCM
#include "player-mp3-sample.c"
#endif

int cOutputDvb::Output(const unsigned char *Data, int Len, bool SOF)
{
  int n=0;
  if(SOF) {
#ifdef BROKEN_PCM
    struct FrameHeader *fh=(struct FrameHeader *)Data;
    if(fh->samplerate!=outSr) {
      if(outSr) {
        // at this point we would need access to AUDIO_STOP/AUDIO_PLAY
        // ioctl, but unfortunaly VDR's API doesn't provides this.
        // So we have to do magic to make the driver switch samplerate.
        const unsigned char *p=testAudio;
        int pc=sizeof(testAudio);
        int r;
        do {
#if APIVERSNUM < 10318
          r=player->PlayVideo(p,pc);
#else
          r=player->PlayPes(p,pc);
#endif
          if(r>0) { p+=r; pc-=r; }
          if(r==0) Poll();
          } while(r>=0 && pc>0);
        }
      outSr=fh->samplerate;
      d(printf("music-sd: dvb: output samplerate now %d\n",outSr))
      }
#endif
    n=FHS;
    Data+=n; Len-=n;
    }
#if APIVERSNUM < 10318
  int r=player->PlayVideo(Data,Len);
#else
  int r=player->PlayPes(Data,Len);
#endif
  return (r>=0 ? r+n : -1);
}

bool cOutputDvb::Poll(void)
{
  return player->DevicePoll(poll,500);
}

void cOutputDvb::Play(void)
{
#ifndef BROKEN_PCM
  player->DevicePlay();
#endif
}

void cOutputDvb::Pause(void)
{
#ifndef BROKEN_PCM
  player->DeviceFreeze();
#endif
}



// --- cOutputOss --------------------------------------------------------------


const char *dspdevice="/dev/dsp";

class cOutputOss : public cOutput {
private:
  int fd;
  cPoller poll;
  unsigned int outSr;
  unsigned char buff[8192];
  //
  bool Reset(unsigned int sr);
public:
  cOutputOss(cMP3Player *Player);
  virtual ~cOutputOss();
  virtual void Init(void);
  virtual unsigned int SampleRate(unsigned int PcmSampleRate);
  virtual cFrame *MakeFrame(unsigned int & Samples, const mad_fixed_t **Data, int index, int sr);
  virtual int Output(const unsigned char *Data, int Len, bool SOF);
  virtual bool Poll(void);
  virtual void Play(void);
  virtual void Pause(void);
  };

cOutputOss::cOutputOss(cMP3Player *Player)
:cOutput(Player)
{
  fd=-1; outSr=0;
  d(printf("music-sd: oss: using OSS output\n"))
}

cOutputOss::~cOutputOss()
{
  if(fd>=0) close(fd);
}

void cOutputOss::Init(void)
{
  if(fd<0) {
    fd=open(dspdevice,O_WRONLY|O_NONBLOCK);

    if(fd>=0) {
      if(fcntl(fd,F_SETFL,0)==0)
        poll.Add(fd,true);
      else {
        esyslog("music-sd: ERROR: Cannot make dsp device '%s' blocking: %s!",dspdevice,strerror(errno));
        close(fd); fd=-1;
        }
      }
    else
      esyslog("music-sd: ERROR: Cannot open dsp device '%s': %s!",dspdevice,strerror(errno));

    }

  cOutput::Init();
}

bool cOutputOss::Reset(unsigned int sr)
{
  if(fd>=0) {
    CHECK(ioctl(fd,SNDCTL_DSP_SYNC,0));
    int format=AFMT_S16_LE;
    CHECK(ioctl(fd,SNDCTL_DSP_SETFMT,&format));
    if(format==AFMT_S16_LE) {
      int channels=2;
      CHECK(ioctl(fd,SNDCTL_DSP_CHANNELS,&channels));
      if(channels==2) {
        int real=sr;
        CHECK(ioctl(fd,SNDCTL_DSP_SPEED,&real));
        d(printf("music-sd: oss: DSP samplerate now %d\n",real))
        if(abs(real-sr)<sr/50) {
          outSr=sr;
          d(printf("music-sd: oss: DSP reset done\n"))
          return true;
          }
        else {
          d(printf("music-sd: oss: driver can't handle samplerate %d, got %d\n",sr,real))
          esyslog("music-sd: ERROR: OSS driver can't handle samplerate %d, got %d\n",sr,real);
          }
        }
      else {
        d(printf("music-sd: oss: 2-channel stereo not supported\n"))
        esyslog("music-sd: ERROR: OSS driver doesn't support 2-channel stereo.");
        }
      }
    else {
      d(printf("music-sd: oss: little-endian samples not supported\n"))
      esyslog("music-sd: ERROR: OSS driver doesn't support 16-bit little-endian samples.");
      }
    close(fd); fd=-1;
    }
  return false;
}

unsigned int cOutputOss::SampleRate(unsigned int PcmSampleRate)
{
  return PcmSampleRate;
}


cFrame *cOutputOss::MakeFrame(unsigned int & Samples, const mad_fixed_t **Data, int index, int sr)
{
  struct FrameHeader *fh=(struct FrameHeader *)buff;
  fh->samplerate=sr;
  cFrame *f=0;
  unsigned int outlen=scale.ScaleBlock(buff+FHS,sizeof(buff)-FHS,Samples,Data[0],Data[1],MP3Setup.AudioMode?amDitherLE:amRoundLE);
  if(outlen)
  {
// Spectrum Analyzer: Hand over the PCM16-data to SpAn
       int offset = FHS;
       Span_SetPcmData_1_0 SetPcmData;
       cPlugin *Plugin = cPluginManager::CallFirstService(SPAN_SET_PCM_DATA_ID, NULL);
       if (Plugin)
       {
            SetPcmData.length = (unsigned int)(outlen-offset);
            // the timestamp (ms) of the frame(s) to be visualized:
            SetPcmData.index = index;
            // tell SpAn the ringbuffer's size for it's internal bookkeeping of the data to be visualized:
            SetPcmData.bufferSize = MP3BUFSIZE;
            SetPcmData.data = buff + offset + 1; // get rid of the header
            SetPcmData.bigEndian = false;
            cPluginManager::CallFirstService(SPAN_SET_PCM_DATA_ID, &SetPcmData);
       }

    f=new cFrame(buff,outlen+FHS,ftUnknown,index);
  }
  return f;
}


int cOutputOss::Output(const unsigned char *Data, int Len, bool SOF)
{
  if(fd>=0) {
    int n=0;
    if(SOF) {
      struct FrameHeader *fh=(struct FrameHeader *)Data;
      if(fh->samplerate!=outSr) Reset(fh->samplerate);
      n=FHS;
      Data+=n; Len-=n;
      }
    if(poll.Poll(0)) {
      int r=write(fd,Data,Len);
      if(r<0 && FATALERRNO) return -1;
      if(r>0) n+=r;
      }
    return n;
    }
  return -1;
}

bool cOutputOss::Poll(void)
{
  return fd>=0 ? poll.Poll(500) : false;
}

void cOutputOss::Play(void)
{
}

void cOutputOss::Pause(void)
{
  CHECK(ioctl(fd,SNDCTL_DSP_POST,0));
}  



// --- cMP3Player --------------------------------------------------------------

cMP3Player::cMP3Player()
:cPlayer(MP3Setup.BgrMode == 1 ? pmAudioOnly : pmAudioOnlyBlack)
{
  statusMode=0;
  active=true; started=false; isStream=false;
  ringBuffer=new cRingBufferFrame(MP3BUFSIZE);
  rframe=0; pframe=0; decoder=0; output=0;
  playMode=pmStartup; state=msStop;
  playindex=total=0;
  m_size = KILOBYTE(100);
  m_length = 0;
  m_buffer = MALLOC(uchar, m_size);

  file = "";
  mpegfile = "";
  oldmpegfile = "";
}

cMP3Player::~cMP3Player()
{
  free(m_buffer);
  Detach();
  delete ringBuffer;
}

void cMP3Player::Activate(bool On)
{
  if(On) {
    d(printf("music-sd: player: player active true requested...\n"))
    if(!started) {
	    playMode=pmStartup; Start(); started=true;
      playModeMutex.Lock();
      WaitPlayMode(pmStartup,true); // wait for the decoder to become ready
      playModeMutex.Unlock();
      Lock();
      Play();
      Unlock();
      MP3Setup.PlayerActive = 1;
      }
    d(printf("music-sd: player: player active true done\n"))
    }
  else if(started && active) {
    d(printf("music-sd: player: player active false requested...\n"))
    Lock();
    StopPlay();
    Unlock();
    active=false;
    MP3Setup.PlayerActive = 0;
    SetPlayMode(pmStartup);
    Cancel(2);
    d(printf("music-sd: player: player active false done\n"))
    }
}

void cMP3Player::SetPlayMode(ePlayMode mode)
{
  if(mode==pmStopped) InStopMode=true; else InStopMode = false;  

  playModeMutex.Lock();
  if(mode!=playMode) {
    playMode=mode;
    dm(printf("music-sd: player: setting mode=%d (pid=%d)\n",mode,getpid()))
    playModeCond.Broadcast();
    }
  playModeMutex.Unlock();
}

void cMP3Player::WaitPlayMode(ePlayMode mode, bool inv)
{
  // must be called with playModeMutex LOCKED !!!

  while(active && ((!inv && mode!=playMode) || (inv && mode==playMode))) {
    dm(printf("music-sd: player: entering wait for mode%s%d with mode=%d (pid=%d)\n",inv?"!=":"==",mode,playMode,getpid()))
    playModeCond.Wait(playModeMutex);
    dm(printf("music-sd: player: returning from wait with mode=%d (pid=%d)\n",playMode,getpid()))
    }
}

void cMP3Player::Action(void)
{

  cSong *playing=0;
  struct mad_pcm *pcm=0;
  cResample resample[2];
  unsigned int nsamples[2];
  const mad_fixed_t *data[2];
  cLevel level;
  cNormalize norm;
  bool haslevel=false;
  const unsigned char *p=0;
  int pc=0, readindex=0;
#ifdef DEBUG
  int beat=0;
#endif
#ifdef DEBUG_DELAY
  int lastwrite=0;
#endif
  bool frameIncomplete = true;

  dsyslog("music-sd: player: player thread started (pid=%d)", getpid());
  state=msStop;
  SetPlayMode(pmStopped);

  delete output; output=0;

  if(MP3Setup.AudioOutMode==AUDIOOUTMODE_OSS)   output=new cOutputOss(this);
    else 
       output=new cOutputDvb(this);

  if(!output) {
    d(printf("music-sd: player: audiooutmode mismatch or no output driver\n"))
    esyslog("music-sd: player: ERROR: no audio output driver. balling out");
    goto abort;
  }

  while(active) {
#ifdef DEBUG
    {
    int now=time(0);
    if(now>=beat) {
      int avail=ringBuffer->Available();
      printf("music-sd: player: heartbeat buffer=%d now=%d\n",avail,now&4095);
      //output->Stats(); if(haslevel) norm.Stats();
      beat=now+(avail>(MP3BUFSIZE*10/100) ? (avail<(MP3BUFSIZE*50/100) ? 2 : 20) : 1);
      }
    }
#endif

    Lock();

next:
    bool SOF=false;
    if(!pframe && playMode==pmPlay) {
      pframe=ringBuffer->Get();
      if(pframe) {
        playindex=pframe->Index();
        p=pframe->Data();
        pc=pframe->Count();
        SOF=true;
        }
      }

    if(pframe) {
#ifdef DEBUG_DELAY
      {
      int now=time_ms();
      if(lastwrite && lastwrite<now-(DEBUG_DELAY+50))
        printf("music-sd: player: write delayed %d ms\n",now-lastwrite);
      lastwrite=now;
      }
#endif
      int w=output->Output(p,pc,SOF);
 				   
      if(w>0) {
// Spectrum Analyzer: Tell SpAn which timestamp is currently playing
        if ( frameIncomplete ) {
          Span_SetPlayindex_1_0 SetPlayindexData;
          cPlugin *Plugin = cPluginManager::CallFirstService(SPAN_SET_PLAYINDEX_ID, NULL);
          if (Plugin) {
            SetPlayindexData.index = playindex;
            cPluginManager::CallFirstService(SPAN_SET_PLAYINDEX_ID, &SetPlayindexData);
          }
          frameIncomplete = false;
        }

        p+=w; pc-=w;
        if(pc<=0) {
          ringBuffer->Drop(pframe);
          pframe=0;
	        frameIncomplete = true;
          goto next;
          }
        }
      else if(w<0 && FATALERRNO) {
        LOG_ERROR;
        d(printf("music-sd: player: output failed: %s\n",strerror(errno)))
        Unlock();
        goto abort;
        }
      }

    if(mgr->NewCurrent() && playMode==pmPlay && state!=msStart) {
      Empty();
      state=msRestart;
      d(printf("music-sd: player: stale song change, restart.\n"))
    }

    if(!rframe && playMode==pmPlay) {
      switch(state) {
        case msStart:
          d(printf("music-sd: player: starting play\n"))
          mgr->Throttle(true);
          playindex=readindex=total=0;
          playing=mgr->Current();

// check for existing file
          if(playing) {
            if((decoder=playing->Decoder()) && decoder->Start()) {
              strncpy(Songname,playing->Fullname(),sizeof(Songname));
              isStream=decoder->IsStream(); levelgood=!isStream; haslevel=false;
              cSongInfo *si=playing->Info(true);
              if(si) {
                if(si->Level>0.0) {
                  d(printf("music-sd: player: found song level=%f peak=%f\n",si->Level,si->Peak))
                  haslevel=true;
                  norm.Init(si->Level,si->Peak);
                }
                if(si->HasInfo()) {
                  total=SecondsToFrames(si->Total);
                }
	            }	
              d(printf("music-sd: player: isStream=%d levelgood=%d haslevel=%d\n",isStream,levelgood,haslevel))
              output->Init();
              level.Init();
              state=msDecode;
	            mgr->currindex=mgr->currIndex;
	            MP3Setup.isWebstream=IsStream();
	            statusMode=2;


              if( (MP3Skin.isMpeg == 1) && !(MP3Skin.reloadmpeg) ) CheckMpeg();

              break;
              }
            else {
              esyslog("music-sd: player: ERROR: playlist entry %s is not a valid file",playing->Name());
			      }
          }
          else
            d(printf("music-sd: player: no current on start play\n"))
          state=msEof;
          break;
        case msDecode:
          {
          // change theme on-the-fly
          if( (MP3Skin.reloadmpeg) && (MP3Skin.isMpeg ==1) ){
            d(printf("music-sd: player: reloadmpeg = 1 , now check for different mpeg\n"))
            CheckMpeg();
	        }   

#ifdef DEBUG_DELAY
          int now=time_ms();
#endif
          struct Decode *ds=decoder->Decode();
#ifdef DEBUG_DELAY
          now=time_ms()-now;
          if(now>DEBUG_DELAY) printf("music-sd: player: decode delayed %d ms\n",now);
#endif
          switch(ds->status) {
            case dsPlay:
              pcm=ds->pcm;
              readindex=ds->index;
              state=msNormalize;
              break;
            case dsSkip:
            case dsSoftError:
              // skipping, state unchanged, next decode
              break;
            case dsEof:
              if(!haslevel && levelgood) { // save level & peak to infocache on eof
                double l=level.GetLevel();
                if(l>0.0) {
                  cSongInfo *si=decoder->SongInfo(false);
                  cFileInfo *fi=decoder->FileInfo();
                  if(si && fi) {
                    si->Level=l;
                    si->Peak=level.GetPeak();
                    InfoCache.Cache(si,fi);
                    }
                  }
                }
              state=msEof;
              break;
            case dsOK:
            case dsError:
              state=msError;
              break;
            }
          break;
          }
        case msNormalize:
          if(!haslevel) { if(levelgood) level.GetPower(pcm); }
          else norm.AddGain(pcm);
          state=msResample;
          break;
        case msResample:
#ifdef DEBUG
          {
          static unsigned int oldrate=0;
          if(oldrate!=pcm->samplerate) {
            printf("music-sd: player: new input sample rate %d\n",pcm->samplerate);
            oldrate=pcm->samplerate;
            }
          }
#endif
          nsamples[0]=nsamples[1]=pcm->length;
          data[0]=pcm->samples[0];
          data[1]=pcm->channels>1 ? pcm->samples[1]:0;

          dvbSampleRate=output->SampleRate(pcm->samplerate);
          if(dvbSampleRate!=pcm->samplerate) {
            if(resample[0].SetInputRate(pcm->samplerate,dvbSampleRate)) {
              nsamples[0]=resample[0].ResampleBlock(nsamples[0],data[0]);
              data[0]    =resample[0].Resampled();
              }
            if(data[1] && resample[1].SetInputRate(pcm->samplerate,dvbSampleRate)) {
              nsamples[1]=resample[1].ResampleBlock(nsamples[1],data[1]);
              data[1]    =resample[1].Resampled();
              }
            }
          state=msOutput;
          break;
        case msOutput:
          if(nsamples[0]>0) rframe=output->MakeFrame(nsamples[0],data,readindex,dvbSampleRate);
          else state=msDecode;
          break;
        case msError:
        case msEof:
          d(printf("music-sd: player: eof or error\n"))
          state=msWait;
          d(printf("music-sd: player: state=msEof\n"))
          // fall through
        case msRestart:
        case msStop:
          d(printf("music-sd: player: stopping play\n"))
          if(decoder) { decoder->Stop(); decoder=0; }
          mgr->Release(); playing=0;
          levelgood=false;
#ifdef DEBUG
          output->Stats(); if(haslevel) norm.Stats();
#endif
          d(printf("music-sd: player: state=msStop\n"))
          if(state==msStop) SetPlayMode(pmStopped);
          if(state==msRestart) state=msStart;
          break;
        case msWait:
          if(ringBuffer->Available()==0) {
            if(mgr->NextCurrent()) {
              d(printf("music-sd: player: playing next\n"))
              state=msStart;
              }
            else {
              d(printf("music-sd: player: end of playlist\n"))
              d(printf("music-sd: player: player idle...\n"))
              d(printf("music-sd: player: state=msWait\n"))
              SetPlayMode(pmStopped);
              statusMode=1;
              }
            }
          break;
        }
      }

    if(rframe && ringBuffer->Put(rframe)) rframe=0;

    Unlock();

    if((rframe || state==msWait) && pframe) {
//      d(printf("music-sd: player: (rframe || state=msWait) && pframe -> Throttle(false)\n"))
      mgr->Throttle(false);
      output->Poll();
      }
    else if(playMode!=pmPlay) {
//      d(printf("music-sd: player: playMode!=pmPlay -> Throttle(false)\n"))
      mgr->Throttle(false);
      playModeMutex.Lock();
      if(playMode!=pmPlay) WaitPlayMode(playMode,true);
      playModeMutex.Unlock();
#ifdef DEBUG_DELAY
      lastwrite=0;
#endif
      }
    else if(state!=msWait && ringBuffer->Available()<(MP3BUFSIZE*50/100)) {
//      d(printf("music-sd: player: state!=msWait && ringBuffer->Avaiable()<(MP3BUFSIZE*50/100) -> Throttle(true)\n"))
      mgr->Throttle(true);
      }
    }

abort:
  Lock();
  delete rframe;
  delete output; output=0;
  if(decoder) { decoder->Stop(); decoder=0; }
  mgr->Release(); playing=0;
  SetPlayMode(pmStopped);
  Unlock();
  active=false;

  dsyslog("music-sd: player: player thread ended (pid=%d)", getpid());
}

void cMP3Player::Empty(void)
{
  Lock();
  delete rframe;
   rframe=0;
    pframe=0;
  ringBuffer->Clear();
  DeviceClear();
  Unlock();
}

void cMP3Player::StopPlay(void) // StopPlay() must be called in locked state!!!
{
  if(playMode!=pmStopped) {
    Empty();
    state=msStop;
    SetPlayMode(pmPlay);
    Unlock();                 // let the decode thread process the stop signal
    playModeMutex.Lock();
    WaitPlayMode(pmStopped,false);
    playModeMutex.Unlock();
    Lock();
    }
}

void cMP3Player::Pause(void)
{
  Lock();
  if(playMode==pmPaused) Play();
  else if(playMode==pmPlay && !isStream) {
    d(printf("music-sd: player: pause\n"))
    if(output) output->Pause();
    SetPlayMode(pmPaused);
    statusMode=3;
    }
  Unlock();
}


void cMP3Player::Play(void)
{
  Lock();
  if(playMode!=pmPlay) {
    d(printf("music-sd: player: play\n"))
    if(playMode==pmStopped) state=msStart;
    if(output) output->Play();
    SetPlayMode(pmPlay);
    }
  statusMode=2;    
  Unlock();
}


void cMP3Player::DeleteTrack(bool All, int idx)
{
  int all=All;

  if (all) {
    statusMode=1;
    Lock();
    StopPlay();
    Unlock();
    mgr->Halt();
    mgr->Flush();
    total=0; playindex=0;
    file = config;
    file = file + "/themes/defaultcover/music-default-cover.png";
    strncpy(coverpicture, file.c_str(),sizeof(coverpicture));
    MP3Setup.CanLoadCover = 1;
    }
  else {
    if (idx == mgr->currIndex) { 
      Lock();
      StopPlay();
      Unlock();
      mgr->DeleteRecord(idx);
      Play();
      }
    else {
      mgr->DeleteRecord(idx);
    }    
  }
}


bool cMP3Player::PrevCheck(void)
{
  bool res=false;
  Lock();
  if(playindex>=2000 && !isStream) {
    state=msRestart; res=true;
    Empty();
    d(printf("music-sd: player: skip to start of song\n"))
    }
  Unlock();
  return res;
}

void cMP3Player::SkipSeconds(int secs)
{
  if(playMode!=pmStopped && !isStream) {
    Lock();
    d(printf("music-sd: player: skip secs %d\n",secs))
    if(playMode==pmPaused) SetPlayMode(pmPlay);
    float bufsecs=(float)ringBuffer->Available() / (float)(dvbSampleRate*OUT_FACT);
    d(printf("music-sd: player: ringbuffer available %f secs\n",bufsecs))
    if(secs>0 && bufsecs>=(float)secs) {
      // clear intermediate queue
      if(pframe) {
        ringBuffer->Drop(pframe);
        pframe=0;
        }
      DeviceClear();
      // skip inside ringbuffer
      int skipindex=playindex+secs*1000;
      d(printf("music-sd: player: skipping play=%d skip=%d ...\n",playindex,skipindex))
      cFrame *f;

      do {
        f=ringBuffer->Get();
        if(f) {
          playindex=f->Index();
          ringBuffer->Drop(f);
          d(printf("*"))
          }
        } while(f && playindex<skipindex);
      d(printf("music-sd: player: skipped play=%d skip=%d\n",playindex,skipindex))
      }
    else {
      if(decoder && decoder->Skip(secs,bufsecs)) levelgood=false;
      Empty();
      }

    Unlock();
    }
}


bool cMP3Player::Muted(void)
{
  bool res = false;
  res = cDevice::PrimaryDevice()->IsMute();
  return res;
}

int cMP3Player::CurrentVolume(void)
{
  return cDevice::PrimaryDevice()->CurrentVolume();
}


bool cMP3Player::GetIndex(int &Current, int &Total, bool SnapToIFrame)
{
  Current=SecondsToFrames(playindex/1000); Total=total;
  return total>=0;
}

bool cMP3Player::GetReplayMode(bool &Play, bool &Forward, int &Speed)
{
  Play=(playMode==pmPlay);
  Forward=true;
  Speed=-1;
  return true;
}

char *cMP3Player::CheckImage (char *fileName)
{
  static char tmpFile[256];
  char *result = NULL;
  FILE *fp;


    if ((fp = fopen (fileName, "rb")))
    {
      fclose (fp);
      d(printf("music-sd[%d]: player: image found '%s'\n", getpid (), fileName));
      strncpy(tmpFile,fileName,sizeof(tmpFile));
      result = tmpFile;
    }
  return result;
}


//bool cMP3Player::ExistsLyrics(const char *songpath)
bool cMP3Player::ExistsLyrics(const char *songpath)
{
  bool LyricsFound = false;

  std::string::size_type pos;
  std::string fullfilename;
  std::string filename;
  
  fullfilename = songpath;
  
  pos = fullfilename.rfind(".");
  if(pos!=std::string::npos) {
    filename = fullfilename.substr(0, fullfilename.length() -(fullfilename.length() -pos));
    filename += ".lyrics";
#ifdef DEBUG_COVER
    printf("music-sd: player: check for lyrics '%s'\n", filename.c_str());
#endif
    if( FILE *f = fopen(filename.c_str(), "r") ) {
#ifdef DEBUG_COVER
	printf("music-sd: player: found lyrics '%s'\n", filename.c_str());
#endif
	LyricsFound = true;
      fclose(f);
      }
    else {
#ifdef DEBUG_COVER
      printf("music-sd: player: no lyrics found\n");
#endif
      LyricsFound = false;
    }
  }      

  return LyricsFound;
}

void cMP3Player::DefaultImage(void)
{
  if (isStream) {
#ifdef DEBUG_COVER
	printf("music-sd: player: using default cover for streams\n");
#endif
  file = config;
    file = file + "/themes/defaultcover";
    file = file + MP3Skin.streamcover;
    strncpy(coverpicture, file.c_str(),sizeof(coverpicture));
    }	
  else {
#ifdef DEBUG_COVER
	printf("music-sd: player: using default cover for local track\n");
#endif
    file = config;
    file = file + "/themes/defaultcover";
    file = file + MP3Skin.localcover;
    strncpy(coverpicture, file.c_str(),sizeof(coverpicture));
  }
}


void cMP3Player::LoadImage(const char *fullname, const char *artist, bool coveronly)
{
  size_t i;
  char imageFile[256];
  char oldimagefile[256];
  char *p, *q = NULL;
  char Artist[256];
  const char *imageSuffixes[] = { "jpg", "png" };
 
  strncpy (oldimagefile, coverpicture,sizeof(oldimagefile));
  strncpy (imageFile, fullname,sizeof(imageFile));
  strncpy (coverpicture, "",sizeof(coverpicture));

#ifdef DEBUG_COVER
  printf("music-sd[%d]: player: checking '%s' for images. artist='%s'\n", getpid (), fullname, artist);
#endif

  //
  // track specific image, e.g. <song>.jpg
  //
  if (!strlen (coverpicture)) {
    p = strrchr (imageFile, '.');
    if (p)
    {
      for (i = 0; i < sizeof (imageSuffixes) / sizeof (imageSuffixes[0]); i++)
      {
        strncpy (p + 1, imageSuffixes[i], sizeof(p +1));
#ifdef DEBUG_COVER
        printf("music-sd[%d]: player: try trackimage: '%s'\n", getpid (), imageFile);
#endif
        q = CheckImage (imageFile);
        if (q)
        {
          strncpy (coverpicture, q, sizeof(coverpicture));
        }
      }
    } 
  }
  //
  // album specific image, e.g. cover-/front-/folder.jpg in song directory
  //
  if (!strlen (coverpicture)) {
    std::string fullfilename;
    std::string Base;
    
    fullfilename = imageFile;
    int len = fullfilename.length();

    std::string::size_type pos  = fullfilename.rfind('/',len);
    if(pos!=std::string::npos) {
      //BaseDir
      Base     = fullfilename.substr(0,pos+1);
    }  


    std::string jpegs    ="";
    std::string b_jpegs  ="";
    std::string pngs     ="";
    std::string b_pngs   ="";

    jpegs   = Base + "*.jpg";
    b_jpegs = Base + "*.JPG";
    pngs    = Base + "*.png";
    b_pngs  = Base + "*.PNG";

#ifdef DEBUG_COVER
    printf("music-sd[%d]: player: try albumimage in '%s'\n", getpid (), Base.c_str());
#endif
    unsigned int i;
    glob_t glob_buffer;
    glob_buffer.gl_offs = 4;
    glob(  jpegs.c_str(), 0              , NULL, &glob_buffer);
    glob(b_jpegs.c_str(), GLOB_APPEND    , NULL, &glob_buffer);
    glob(  pngs.c_str() , GLOB_APPEND    , NULL, &glob_buffer);
    glob(b_pngs.c_str() , GLOB_APPEND    , NULL, &glob_buffer);

    for (i=0; i < glob_buffer.gl_pathc; ++i) {
#ifdef DEBUG_COVER
      printf("music-sd[%d]: player: look for '%s'\n", getpid(), glob_buffer.gl_pathv[i]);
#endif
      if(strcasestr(glob_buffer.gl_pathv[i], "cover")) {
        q = CheckImage (glob_buffer.gl_pathv[i]);
        }    
      else if(strcasestr(glob_buffer.gl_pathv[i], "front")) {
        q = CheckImage (glob_buffer.gl_pathv[i]);
        }    
      else if(strcasestr(glob_buffer.gl_pathv[i], "folder")) {
        q = CheckImage (glob_buffer.gl_pathv[i]);
      }    

      if (q) { strncpy(coverpicture, q, sizeof(coverpicture)); }
    } 
    globfree(&glob_buffer);
  }
  //
  // Artist specific image, e.g. Acdc.jpg in Artistcover directory
  //
  if (!strlen (coverpicture)) {
    if(artist !=NULL) {
      strncpy(Artist, MP3Setup.CoverDir,sizeof(Artist));
      strcat(Artist, "/");
      strcat(Artist,artist);
      strcat(Artist, ".jpg");
#ifdef DEBUG_COVER
      printf("music-sd[%d]: player: try artistimage '%s'\n", getpid (), Artist);
#endif
      q = CheckImage (Artist);
      if (q) {
        strncpy (coverpicture, q, sizeof(coverpicture));
      }
    }
  }
  //
  // Default image
  //
  if (!strlen (coverpicture)) {
    DefaultImage();
  }
  //
  // path to cover changed ?

  if (!(strcmp(coverpicture, oldimagefile))==0) {

//    cPlugin *Plugin = cPluginManager::CallFirstService(GRAPHTFT_COVERNAME_ID, NULL);
    cPlugin *Plugin=cPluginManager::GetPlugin("graphtft");
    if (Plugin) {
           MusicService_Covername_1_0 servicecovername;
           servicecovername.name = coverpicture;
           cPluginManager::CallFirstService(GRAPHTFT_COVERNAME_ID, &servicecovername);
    }

    MP3Setup.CanLoadCover = 1;
    }
  else {
    MP3Setup.CanLoadCover = 0;
  }
}

/*
void cMP3Player::SendCover()
{
  ServiceID="Get-Coverpicture-v1";
  ServiceAvailable=cPluginManager::CallFirstService(*ServiceID, NULL);
  
  if(!ServiceAvailable) {
    esyslog("music-sd: ERROR: could not reach serviceid 'Get-Coverpicture-v1'");
    }
  else {
    MP3ServiceData ServiceData;
    ServiceData.data.filename=coverpicture;
    
    if(cPluginManager::CallFirstService(ServiceID, &ServiceData)) {
      if(!ServiceData.result) {
        esyslog("music-sd: ERROR: couldnt send path for coverpicture !");
      }
    }  	
  }
}

*/

void cMP3Player::CheckMpeg(void) {
  oldmpegfile = mpegfile;

  if (!isStream) {
    mpegfile = config;
    mpegfile = mpegfile + "/" + MP3Skin.localbackground;
    }
  else {
    mpegfile = config;
    mpegfile = mpegfile + "/" + MP3Skin.streambackground;
  }
  	
  MP3Skin.reloadmpeg = false;
  // load only if picture is different  
  if(!(strcmp(mpegfile.c_str(), oldmpegfile.c_str()))==0) { 
    ShowMpeg(mpegfile.c_str());
  }
}


void cMP3Player::ShowMpeg(const char *FileName)
{
  int fd = open(FileName, O_RDONLY);
  if (fd >= 0) {
    for (;;) {
      m_length = read(fd, m_buffer, m_size);
      if (m_length >0) {
        if (m_length >= m_size) {
          m_size = m_size * 3 / 2;
          m_buffer = (uchar *)realloc(m_buffer, m_size);
          lseek(fd, 0, SEEK_SET);
          continue;
          }
        SLEEP(80);  
        DeviceStillPicture(m_buffer, m_length);      
        }
     else
       LOG_ERROR_STR(FileName);
     break;
     }
   close(fd);
   }
   else
     LOG_ERROR_STR(FileName);
}




/*
void cMP3Player::ShowMpeg(void)
{
  uchar *buffer;
  int fd;
  struct stat st;
  struct video_still_picture sp;

    if ((fd = open (mpegfile.c_str(), O_RDONLY)) >= 0)  {
#ifdef DEBUG_MPEG
      printf("music-sd[%d]: player: cover still picture %s\n", getpid (), mpegfile.c_str());
#endif
      fstat (fd, &st);
      sp.iFrame = (char *) malloc (st.st_size);

      if (sp.iFrame) {
        sp.size = st.st_size;

        if (read (fd, sp.iFrame, sp.size) > 0) {
          buffer = (uchar *) sp.iFrame;  
#ifdef DEBUG_MPEG
          printf("music-sd[%d]: player: new image frame (size %d)\n", getpid(), sp.size);
#endif
          if(MP3Setup.UseDeviceStillPicture) {
			SLEEP(80);
            DeviceStillPicture (buffer, sp.size);
	    }
          else {
            for (int i = 1; i <= 25; i++) {
              send_pes_packet (buffer, sp.size, i);
            }
          }
        }

        free (sp.iFrame);
      }
      else {
        esyslog ("music-sd[%d]: player: cannot allocate memory (%d bytes) for still image", getpid(), (int) st.st_size);
      }

      close (fd);
    }
    else  {
      esyslog ("music-sd[%d]: player: cannot open image file '%s'", getpid(), mpegfile.c_str());
    }

}
*/

void cMP3Player::send_pes_packet(unsigned char *data, int len, int timestamp)
{
#define PES_MAX_SIZE 2048
    int ptslen = timestamp ? 5 : 1;
    static unsigned char pes_header[PES_MAX_SIZE];

    pes_header[0] = pes_header[1] = 0;
    pes_header[2] = 1;
    pes_header[3] = 0xe0;

    while(len > 0)
     {
        int payload_size = len;
        if(6 + ptslen + payload_size > PES_MAX_SIZE)
	     payload_size = PES_MAX_SIZE - (6 + ptslen);

        pes_header[4] = (ptslen + payload_size) >> 8;
        pes_header[5] = (ptslen + payload_size) & 255;

        if(ptslen == 5)
	 {
	    int x;
	    x = (0x02 << 4) | (((timestamp >> 30) & 0x07) << 1) | 1;
	    pes_header[8] = x;
	    x = ((((timestamp >> 15) & 0x7fff) << 1) | 1);
	    pes_header[7] = x >> 8;
	    pes_header[8] = x & 255;
	    x = ((((timestamp) & 0x7fff) < 1) | 1);
	    pes_header[9] = x >> 8;
	    pes_header[10] = x & 255;
         }
	  else
	 {
	    pes_header[6] = 0x0f;
	 }

        memcpy(&pes_header[6 + ptslen], data, payload_size);

#if APIVERSNUM < 10318
    skipPlayVideo(pes_header, 6 + ptslen + payload_size);
#else
            PlayPes(pes_header, 6 + ptslen + payload_size);
#endif

	len -= payload_size;
	data += payload_size;
	ptslen = 1;
    }
}

