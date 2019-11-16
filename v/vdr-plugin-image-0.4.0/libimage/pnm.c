/***************************************************************************
 * pnm.c
 * (C) 2004-2011 Andreas Brachold    <vdr07 at deltab.de>
 *     based on works from Fabian E. Bustamante <fabianb-at-cs.umd.edu>
 *  Created: Thu Aug  7 2004
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 *
 */

#include <stdio.h>
#include <stdlib.h>
/* #include <sys/stdtypes.h> for size_t */
#include <stdarg.h>
#include <errno.h>

#include "pnm.h"

/*****************************************************************************
Print error message and get out
@param variable list of arguments explaining error
@return - none*/
void cPNM::error(const char* format, ... )
{
  if(m_szError)
    free(m_szError);
  m_szError = NULL;
  
  va_list args;
  va_start( args, format );
 
  (void) vasprintf( &m_szError, format, args );
  va_end( args );
} 


/*****************************************************************************
Get pnm magic number (P1, P2, ...)
@param file - source file
@return - magic number*/
bool cPNM::MagicNumber(FILE* f,unsigned int& nFormat)
{
  int ich1, ich2;
  
  ich1 = getc( f );
  if ( ich1 == EOF ) {
    cPNM::error( "End of file, read error reading magic number" );
    return false;
  }
  ich2 = getc( f );
  if ( ich2 == EOF ) {
    cPNM::error( "End of file, read error reading magic number" );
    return false;
  }
  nFormat = ich1 * 256 + ich2;
  return true;
}

/*****************************************************************************
Get next significant character, i.e. jump over comments
@param file - source file
@param - Next significant character*/
bool cPNM::getchar(FILE* f, char& nNext)
{
  register int ich;
  register char ch;
  
  ich = getc( f );
  if ( ich == EOF )
  {  
    cPNM::error( "End of file, read error" );
    return false;
  }
  ch = (char) ich;
  
  if ( ch == '#' ) {
    do {
      ich = getc( f );
      if ( ich == EOF )
      {
	      cPNM::error( "End of file, read error" );
        return false;
      }
      ch = (char) ich;
    } while ( ch != '\n' && ch != '\r' );
  }
  
  nNext = ch;
  return true;
}


/*****************************************************************************
Get next bit from file
@param file - source file
@param - Next bit*/
bool cPNM::getbit(FILE* f, bit& nNext)
{
  register char ch;
  
  do {
    if(!getchar( f, ch ))
      return false;
  } while ( ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' );
  
  if ( ch != '0' && ch != '1' )
  {  
    cPNM::error( "junk in file where bits should be" );
    return false;
  }
  nNext = ( ch == '1' ) ? 1 : 0;
  return true;
}


/*****************************************************************************
Get next byte from file
@param file - source file
@param - Next byte   */
bool cPNM::getrawbyte(FILE* f,unsigned char& nNext )
{
  register int iby;
  
  iby = getc( f );
  if ( iby == EOF )
  {  
    cPNM::error( "End of file, read error" );
    return false;
  }
  nNext = (unsigned char) iby;
  return true;
}


/*****************************************************************************
Get integer from file
@param file - source file
@param - Next integer  */
//bool cPNM::getint(FILE* f, unsigned int& nNext)
template<class T> bool cPNM::getint(FILE* f, T& nNext)
{
  register char ch;
  register T i;
  
  do {
    if(!getchar( f,ch ))
       return false;
  } while ( ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' );
  
  if ( ch < '0' || ch > '9' )
  {  
    cPNM::error( "junk in file where an integer should be" );
    return false;
  }
  i = 0;
  do {
    i = i * 10 + ch - '0';
    if(!getchar( f,ch ))
      return false;
  } while ( ch >= '0' && ch <= '9' );
  nNext = i;
  return true;
}


/*****************************************************************************
Read Header from file
@param FILE* f - source file
@return bool - true successful reading*/
bool cPNM::readHeader(FILE* f)
{
  if(!MagicNumber(f,m_nFormat) /* Read magic number. */
     || !getint(f,m_nWidth)        /* Read size. */
     || !getint(f,m_nHeight))
      return false;

  /* Check magic number. */
  switch (PNM_FORMAT_TYPE(m_nFormat)) {
  case PPM_TYPE:  
    /* Read m_nColorDepth. */
    if(!getint(f,m_nColorDepth))
      return false;
    if ( (m_nColorDepth) > PPM_MAXMAXVAL ) 
    {
      cPNM::error( "ColorDepth is too large" );
      return false;
    }
    return true;
    
  case PGM_TYPE:
    /* Read ColorDepth. */
    if(!getint(f,m_nColorDepth))
      return false;
    if ( m_nColorDepth > PGM_MAXMAXVAL )
    {  
      cPNM::error( "ColorDepth is too large" );
      return false;
    }
    return true;
    
  case PBM_TYPE:
    m_nColorDepth = 1;		/* pbmmaxval */
    return true;
    
  default:
    cPNM::error( "bad magic number - not a ppm, pgm, or pbm file" );
  } 

  return false;

}


/*****************************************************************************
Read Row from file with Black White Values
@param FILE* f - source file
@return bool - true successful reading*/
bool cPNM::readpbmrow(FILE* f, bit* pBits)
{
  register unsigned int w;
  register int bitshift;
  register unsigned char item;
  register bit* bP;
  
  switch ( m_nFormat ) {
  case PBM_FORMAT:
    for ( w = 0, bP = pBits; w < m_nWidth; ++w, ++bP )
      {
       if(!getbit( f,*bP ))
         return false;
      }
    return true;
    
  case RPBM_FORMAT:
    bitshift = -1;
    item = 0;
    for ( w = 0, bP = pBits; w < m_nWidth; ++w, ++bP ) {
	if ( bitshift == -1 ) {
	  if(!getrawbyte( f,item ))
      return false;
	  bitshift = 7;
	}
	*bP = ( item >> bitshift ) & 1;
	--bitshift;
    }
    return true;
    
  default:
    cPNM::error( "Wrong fileformat" );
    return false;
  }
} /* end cPNM::readpbmrow() */


/*****************************************************************************
Read Row from file with Gray Values
@param FILE* f - source file
@return bool - true successful reading*/
bool cPNM::readpgmrow( FILE* f, gray* pGrays)
{
  register unsigned int w;
  register gray* gP;
  bit* pBits;
  register bit* bP;
  
  switch ( m_nFormat ) {
  case PGM_FORMAT:
    for ( w = 0, gP = pGrays; w < m_nWidth; ++w, ++gP ) {
       if(!getint( f,*gP ))
         return false;
    }
    return true;
    
  case RPGM_FORMAT:
    for ( w = 0, gP = pGrays; w < m_nWidth; ++w, ++gP ) {
       if(!getrawbyte( f,*gP ))
         return false;
    }
    return true;
    
  case PBM_FORMAT:
  case RPBM_FORMAT:
    if(!allocrow( &pBits )
      || !readpbmrow( f, pBits ))
    {
      freerow( pBits );
      return false;
    }
    for ( w = 0, gP = pGrays, bP = pBits; w < m_nWidth; ++w, ++gP, ++bP )
      *gP = ( *bP == PBM_WHITE ) ? m_nColorDepth : 0;
    freerow( pBits );
    return true;
    
  default:
    cPNM::error( "Wrong fileformat" );
    return false;
  }
} 



/*****************************************************************************
Read Row from file with Colored Values
@param FILE* f - source file
@return bool - true successful reading*/
bool cPNM::readppmrow(FILE* f, pixel* pixelrow)
{
  register unsigned int w;
  register pixel* pP;
  register pixval r, g, b;
  gray* pGrays;
  register gray* gP;
  bit* pBits;
  register bit* bP;
  
  switch ( m_nFormat ) {
  case PPM_FORMAT:
    for ( w = 0, pP = pixelrow; w < m_nWidth; ++w, ++pP ) {
      if(!getint( f,r )
       ||!getint( f,g )
       ||!getint( f,b ))
          return false;
      PPM_ASSIGN( *pP, r, g, b );
    }
    return true;
    
  case RPPM_FORMAT:
    for ( w = 0, pP = pixelrow; w < m_nWidth; ++w, ++pP ) {
      if(!getrawbyte( f,r )
       ||!getrawbyte( f,g )
       ||!getrawbyte( f,b ))
          return false;
      PPM_ASSIGN( *pP, r, g, b );
    }
    return true;
    
  case PGM_FORMAT:
  case RPGM_FORMAT:
    if(!allocrow(&pGrays)
      || !readpgmrow( f, pGrays ))
    {
      freerow( pGrays );
      return false;
    }
    for ( w = 0, gP = pGrays, pP = pixelrow; w < m_nWidth; ++w, ++gP, ++pP ) {
      r = *gP;
      PPM_ASSIGN( *pP, r, r, r );
    }
    freerow( pGrays );
    return true;
    
  case PBM_FORMAT:
  case RPBM_FORMAT:
    if(!allocrow( &pBits )
      || !readpbmrow( f, pBits ))
    {
      freerow( pBits );
      return false;
    }
    for ( w = 0, bP = pBits, pP = pixelrow; w < m_nWidth; ++w, ++bP, ++pP ) {
      r = ( *bP == PBM_WHITE ) ? m_nColorDepth : 0;
      PPM_ASSIGN( *pP, r, r, r );
    }
    freerow( pBits );
    return true;
    
  default:
    cPNM::error( "Wrong fileformat" );

    return false;
  }
} 


/*****************************************************************************
Read Row from file with any Values
@param FILE* f - source file
@return bool - true successful reading*/
bool cPNM::readrow(FILE* f, xel* xelrow)
{
  register unsigned int w;
  register xel* xP;
  gray* pGrays;
  register gray* gP;
  bit* pBits;
  register bit* bP;
  
  switch ( PNM_FORMAT_TYPE(m_nFormat) ) {
  case PPM_TYPE:
    return readppmrow( f, (pixel*) xelrow );
    
  case PGM_TYPE:
    if(!allocrow(&pGrays) 
      || !readpgmrow( f, pGrays ))
    {
      freerow( pGrays );
      return false;
    }
    for ( w = 0, xP = xelrow, gP = pGrays; w < m_nWidth; ++w, ++xP, ++gP ) {
      PNM_ASSIGN1( *xP, *gP );
    }
    freerow( pGrays );
    return true;
    
  case PBM_TYPE:
    if(!allocrow(&pBits) 
      || !readpbmrow( f, pBits ))
    {
      freerow( pBits );
      return false;
    }
    for ( w = 0, xP = xelrow, bP = pBits; w < m_nWidth; ++w, ++xP, ++bP ) {
      PNM_ASSIGN1( *xP, *bP == PBM_BLACK ? 0: PNM_MAXMAXVAL );
    }
    freerow( pBits );
    return true;
    
  default:
    cPNM::error( "Wrong fileformat" );
    return false;
  }
}




cPNM::cPNM()
{
  m_nFormat = 0;
  m_nColorDepth = 0;
  m_nHeight = 0;
  m_nWidth = 0;
  m_szError = NULL;
}

cPNM::~cPNM()
{
  if(m_szError)
    free(m_szError);
}




/*
int main()
{
    cPNM pnmImage;
    FILE *f=fopen("test.pnm", "r");
    if(f)
    {
      xel* pRow = NULL;
      register unsigned int w;
      register unsigned int h;
    
      if(pnmImage.readHeader(f))
      {
        for(h = 0;h < pnmImage.GetHeight() && h < GetHeight();++h)
        {  
          if(!pnmImage.allocrow(&pRow) 
            ||!pnmImage.readrow(f, pRow) )
           break;

          xel* pP = pRow;
          if(pnmImage.GetWidth() < GetWidth())
              w =  (GetWidth() - pnmImage.GetWidth()) / 2;
          else w = 0;
          for(;w < pnmImage.GetWidth() && w < GetWidth();++w,++pP)
          {  
            unsigned char* pImageRGB = m_pImageRGB + (((h*GetWidth())+w)*3);
            if(pnmImage.GetColorDepth() == 255)
            {  
              *(pImageRGB + 0) = (unsigned char) PPM_GETR(*pP);
              *(pImageRGB + 1) = (unsigned char) PPM_GETG(*pP);
              *(pImageRGB + 2) = (unsigned char) PPM_GETB(*pP);
            }
            else if(pnmImage.GetColorDepth() == 1)
            {  
              *(pImageRGB + 0) = (unsigned char) PPM_GETR(*pP)==0?0x00:0xFF;
              *(pImageRGB + 1) = (unsigned char) PPM_GETG(*pP)==0?0x00:0xFF;
              *(pImageRGB + 2) = (unsigned char) PPM_GETB(*pP)==0?0x00:0xFF;
            }
            else 
            {  
              *(pImageRGB + 0) = (unsigned char) (PPM_GETR(*pP)*255 / pnmImage.GetColorDepth()) & 0xFF;
              *(pImageRGB + 1) = (unsigned char) (PPM_GETG(*pP)*255 / pnmImage.GetColorDepth()) & 0xFF;
              *(pImageRGB + 2) = (unsigned char) (PPM_GETB(*pP)*255 / pnmImage.GetColorDepth()) & 0xFF;
            }
          }
          pnmImage.freerow(pRow);
          pRow = NULL;
        }
        if(pRow)
          pnmImage.freerow((char*)pRow);
      }
      fclose(f);
    }
}
*/
