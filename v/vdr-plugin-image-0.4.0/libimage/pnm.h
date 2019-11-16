/***************************************************************************
 * pnm.h
 * (C) 2004-2011 Andreas Brachold    <vdr07 at deltab.de>
 *     based on works from Fabian E. Bustamante <fabianb-at-cs.umd.edu>
 *  Created: Thu Aug  7 2004
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 *
 */

 
#ifndef _pnm_h
#define _pnm_h

/* 
 * CONFIGURE: PGM can store gray values as either bytes or shorts.  For most
 * applications, bytes will be big enough, and the memory savings can be
 * substantial.  However, if you need more than 8 bits of grayscale resolution,
 * then define this symbol.
 */
//#define PGM_BIGGRAYS

/* 
 * CONFIGURE: Normally, PPM handles a pixel as a struct of three grays.
 * If grays are stored in bytes, that's 24 bits per color pixel; if
 * grays are stored as shorts, that's 48 bits per color pixel.  PPM
 * can also be configured to pack the three grays into a single longword,
 * 10 bits each, 30 bits per pixel.
 *
 * If you have configured PGM with the PGM_BIGGRAYS option, AND you don't
 * need more than 10 bits for each color component, AND you care more about
 * memory use than speed, then this option might be a win.  Under these
 * circumstances it will make some of the programs use 1.5 times less space,
 * but all of the programs will run about 1.4 times slower.
 *
 * If you are not using PGM_BIGGRAYS, then this option is useless -- it
 * doesn't save any space, but it still slows things down.
 */
/* #define PPM_PACKCOLORS */

/* END CONFIGURE */


typedef unsigned char bit;
const bit PBM_WHITE = 0;
const bit PBM_BLACK = 1;

/* Magic constants. P1,P2,P3...P6 */

const unsigned char PBM_MAGIC1 = 'P';
const unsigned char PBM_MAGIC2 = '1';
const unsigned char RPBM_MAGIC2 = '4';
const unsigned short  PBM_FORMAT = (PBM_MAGIC1 * 256 + PBM_MAGIC2);
const unsigned short  RPBM_FORMAT = (PBM_MAGIC1 * 256 + RPBM_MAGIC2);
#define PBM_TYPE PBM_FORMAT

const unsigned char PGM_MAGIC1 = 'P';
const unsigned char PGM_MAGIC2 = '2';
const unsigned char RPGM_MAGIC2 = '5';
const unsigned short PGM_FORMAT = (PGM_MAGIC1 * 256 + PGM_MAGIC2);
const unsigned short RPGM_FORMAT = (PGM_MAGIC1 * 256 + RPGM_MAGIC2);
#define PGM_TYPE PGM_FORMAT

const unsigned char PPM_MAGIC1 = 'P';
const unsigned char PPM_MAGIC2 = '3';
const unsigned char RPPM_MAGIC2 = '6';
const unsigned short PPM_FORMAT = (PPM_MAGIC1 * 256 + PPM_MAGIC2);
const unsigned short RPPM_FORMAT = (PPM_MAGIC1 * 256 + RPPM_MAGIC2);
#define PPM_TYPE PPM_FORMAT

#ifdef PGM_BIGGRAYS
typedef unsigned short gray;
#define PGM_MAXMAXVAL 65535
#else /*PGM_BIGGRAYS*/
typedef unsigned char gray;
#define PGM_MAXMAXVAL 255
#endif /*PGM_BIGGRAYS*/

typedef gray pixval;

#ifdef PPM_PACKCOLORS

#define PPM_MAXMAXVAL 1023
typedef unsigned long pixel;
#define PPM_GETR(p) (((p) & 0x3ff00000) >> 20)
#define PPM_GETG(p) (((p) & 0xffc00) >> 10)
#define PPM_GETB(p) ((p) & 0x3ff)
#define PPM_ASSIGN(p,red,grn,blu) (p) = ((pixel) (red) << 20) | ((pixel) (grn) << 10) | (pixel) (blu)

#else /*PPM_PACKCOLORS*/

#define PPM_MAXMAXVAL PGM_MAXMAXVAL
typedef struct
{
  pixval r, g, b;
} pixel;

#define PPM_GETR(p) ((p).r)
#define PPM_GETG(p) ((p).g)
#define PPM_GETB(p) ((p).b)
#define PPM_ASSIGN(p,red,grn,blu) do { (p).r = (red); (p).g = (grn); (p).b = (blu); } while ( 0 )

#endif /*PPM_PACKCOLORS*/

typedef pixel xel;
typedef pixval xelval;
#define PNM_MAXMAXVAL PPM_MAXMAXVAL
#define PNM_ASSIGN1(x,v) PPM_ASSIGN(x,v,v,v)


/* Macro for turning a format number into a type number. */
#define PBM_FORMAT_TYPE(f) ((f) == PBM_FORMAT || (f) == RPBM_FORMAT ? PBM_TYPE : -1)
#define PGM_FORMAT_TYPE(f) ((f) == PGM_FORMAT || (f) == RPGM_FORMAT ? PGM_TYPE : PBM_FORMAT_TYPE(f))
#define PNM_FORMAT_TYPE(f) ((f) == PPM_FORMAT || (f) == RPPM_FORMAT ? PPM_TYPE : PGM_FORMAT_TYPE(f))


class cPNM 
{
  unsigned int m_nFormat;
  unsigned int m_nColorDepth;
  unsigned int m_nHeight;
  unsigned int m_nWidth;
  char* m_szError;
protected:
  /*****************************************************************************
  Print error message and get out
  @param variable list of arguments explaining error
  @return - none*/
  void error (const char* format, ... );

  /*****************************************************************************
  Get pnm magic number (P1, P2, ...)
  @param file - source file
  @return - magic number*/
  bool MagicNumber(FILE* f,unsigned int& nFormat);

  /*****************************************************************************
  Get next significant character, i.e. jump over comments
  @param file - source file
  @param - Next significant character*/
  bool getchar(FILE* f,char& nNext);

  /*****************************************************************************
  Get next bit from file
  @param file - source file
  @param - Next bit*/
  bool getbit(FILE* f, bit& nNext );

  /*****************************************************************************
  Get next byte from file
  @param file - source file
  @param - Next byte   */
  bool getrawbyte(FILE* f, unsigned char& nNext);

  /*****************************************************************************
  Get integer from file
  @param file - source file
  @param - Next integer  */
  //bool getint(FILE* file, unsigned int& nNext);
  template<class T> bool getint(FILE* f, T& nNext);


  bool readpbmrow(FILE* file, bit* bitrow);
  bool readpgmrow(FILE* file, gray* grayrow);
  bool readppmrow(FILE* file, pixel* pixelrow);

public:
  cPNM();
  virtual ~cPNM();


  bool readHeader(FILE* Infp);
  bool readrow(FILE* file, xel* xelrow);
  inline unsigned int GetHeight() const      { return m_nHeight;} 
  inline unsigned int GetWidth() const       { return m_nWidth;} 
  inline unsigned int GetColorDepth() const  { return m_nColorDepth;} 
  /*****************************************************************************
  Get an array from memory to hold 'm_nWidth' elements of size 'size'
  @param int size - of each element
  @return char* - Pointer to allocated array  */
  template<class T> bool allocrow(T** p)
  {
    *p = (T*) malloc( m_nWidth * sizeof(T) );
    if (*p == (T*) 0 ) 
    {  
      cPNM::error( "out of memory allocating a row" );
      return false;
    }
    return true;
  }
  /*****************************************************************************
  Free memory chunk previously assigned
  @param char *p - pointer to memory chunk
  @return - none  */
  template<class T> void freerow(T *p)
  {
    if(p != NULL)
      free((char*)p);
  }
    
  const char* GetError() const { return m_szError; }
};


#endif /* _pnm_h */
