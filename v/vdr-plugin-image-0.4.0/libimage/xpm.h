/***************************************************************************
 * xpm.h
 *
 * (C) 2004-2011 Andreas Brachold    <vdr07 at deltab.de>
 *  Created: Thu Aug  11 2004
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 *
 */

 
#ifndef _xpm_h
#define _xpm_h

class cXPM 
{

  const char **m_pXPM;
  unsigned int m_nWidth;
  unsigned int m_nHeight;
  unsigned int m_nColors;

  struct colormap
  {
    unsigned int color;
    unsigned int rgb;
  } *m_Colors;
protected:
  bool GetColor(unsigned int color,unsigned int& rgb) const;
public:
  cXPM(const char* pXPM[]);
  virtual ~cXPM();

  enum ePlacement
  {
    TopLeft,
    TopRight,
    Center,
    BottomLeft,
    BottomRight
  };

  bool Overlay(unsigned char* pRGBMem,unsigned int nMemWidth,unsigned int nMemHeight,unsigned int OffLeft,unsigned int OffTop) const;
  static bool Overlay(const char sz,unsigned char* pRGBMem,
                      unsigned int nMemWidth,unsigned int nMemHeight,
                      cXPM::ePlacement place, unsigned int nOffLeft,unsigned int nOffTop,
                      unsigned int nWidth,unsigned int nHeight);
  static bool Error(unsigned char* pRGBMem,unsigned int nMemWidth,unsigned int nMemHeight);
};

#endif /* _xpm_h */
