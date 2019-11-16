/*
 * Image plugin to VDR (C++)
 *
 * (C) 2004-2011 Andreas Brachold    <vdr07 at deltab.de>
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 *
 */

#ifndef ___IMAGE_LIST_H
#define ___IMAGE_LIST_H

class cSlideShow;
class cImageData;
class cFileSource;

class cActivSlideShow
{
  /** The current active filelist with the slide*/
  cSlideShow                  *m_pCurSlideShow;
  /** Current number of Image in slide */  
  unsigned int                m_nCurrentImage;
  /** Total number of Images in slide */  
  unsigned int                m_nTotalImages;
  /** Reference to current Image of this player  */
  cImageData                  *m_pCurImage;
  
public:
  cActivSlideShow();
  void Assign(cSlideShow *pCurSlideShow);
  void Shutdown(void);
  cSlideShow *SlideShow(void);
  cImageData* GetImage();
  bool PrevImage(int nOffset);
  bool GotoImage(unsigned int nNewPictureIndex);
  bool NextImage(int nOffset);
  int GetJumpNames(int nOffset,cImageData* pImage[],const unsigned int nMAX_BILDER);

  void Remove(cFileSource* src);

  /** Deliver the current number of viewed Image */
  int ImageCurrent(void) const    { return m_nCurrentImage;     }
  /** Deliver the total number of viewed Image */
  int ImageTotal(void) const      { return m_nTotalImages;      }

} extern theSlideShow;

#endif				//___IMAGE_LIST_H
