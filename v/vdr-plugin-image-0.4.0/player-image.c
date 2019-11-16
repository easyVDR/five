/*
 * Image plugin to VDR (C++)
 *
 * (C) 2004-2011 Andreas Brachold    <vdr07 at deltab.de>
 * based on (C) 2003 Kai Tobias Burwieck <kai-at-burwieck.net>
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 *
 */

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <signal.h>
#include <wait.h>

#include "player-image.h"
#include "control-image.h"
#include "setup-image.h"
#include "data-image.h"
#include "image.h"
#include "list.h"
#include <vdr/i18n.h>

#include "libimage/pnm.h"
#include "libimage/xpm.h"


const char *g_szConvertScript = "imageplugin.sh";


//----------cImagePlayer-------------




cImagePlayer::cImagePlayer(cSlideShow *pCurSlideShow)
: cStillImagePlayer(
 ((ImageSetup.m_bLiveAudio != 0)?pmAudioVideo:pmVideoOnly)
,((ImageSetup.m_bUseDeviceStillPicture != 0)?true:false)
)
, m_bConvertRunning(false)
, m_szError(NULL)
{
  theSlideShow.Assign(pCurSlideShow);
}

bool cImagePlayer::GetIndex(int &nCurrent, int &nTotal, bool /*bSnapToIFrame*/)
{
  // Notify other status-plugins with one picture as one second
  nCurrent = theSlideShow.ImageCurrent(); 
  // Notify other status-plugins count of picture as totalcount of seconds
  nTotal = theSlideShow.ImageTotal();

  return true;
}

cImagePlayer::~cImagePlayer()
{
  // Remove Slideshow
  theSlideShow.Shutdown();
}


void cImagePlayer::Activate(bool bOn)
{
  if(bOn) {
    if(theSlideShow.GetImage())
    {
      cStillImagePlayer::Activate(bOn);
    }
  }
  else
    cStillImagePlayer::Activate(false);
}


bool cImagePlayer::NextImage(int nOffset)
{
  return theSlideShow.NextImage(nOffset);
}



bool cImagePlayer::PrevImage(int nOffset)
{
  return theSlideShow.PrevImage(nOffset);
}


bool cImagePlayer::GotoImage(unsigned int nNewPictureIndex)
{
  return theSlideShow.GotoImage(nNewPictureIndex);
}


bool cImagePlayer::Convert(const char *szChange)
{
  cImageData* pImage = theSlideShow.GetImage();
  if(pImage)
  {
    cShellWrapper* pCmd = new cShellWrapper;
    pCmd->bClearBackground = true;  
    pCmd->nOffLeft = m_StillImage.GetBorderWidth();
    pCmd->nOffTop = m_StillImage.GetBorderHeight();
    pCmd->nWidth = UseWidth();
    pCmd->nHeight = UseHeight();
  
    // Build image_convert.sh "source.jpg" "/tmp/image/dest.pnm" 720 576 0 0 0 0 original
    pCmd->szPNM = strdup(pImage->NamePNM());
      
    asprintf(&pCmd->szCmd, "%s \"%s\" \"%s\" %d %d %d %d %d %s", 
        g_szConvertScript,
        pImage->Name(), 
        pCmd->szPNM,
        pCmd->nWidth,
        pCmd->nHeight,
        0,
        0,
        0,
        szChange ? szChange : "");
  
    Exec(pCmd);
    return true;
  }
  return false;
}



bool cImagePlayer::ConvertJump(int nOffset)
{
  register unsigned int w,h;
  const unsigned int MAX_BILDER = 9;
  cImageData* pImage[MAX_BILDER];
  for (w = 0; w < MAX_BILDER; ++w)
    pImage[w] = NULL;
  int nBilder = theSlideShow.GetJumpNames(nOffset,pImage,MAX_BILDER);
  if(nBilder > 0 
    && pImage[0]) {
      
    unsigned int nMatrix = (nBilder < 5) ? 2 : 3;
  
    for (h = 0; h < nMatrix; ++h) 
      for (w = 0; w < nMatrix && pImage[(h*nMatrix)+w]; ++w) 
      {
        cShellWrapper* pCmd = new cShellWrapper;
      
        pCmd->bClearBackground = (w == 0 && h == 0);  
        pCmd->nWidth = UseWidth();
        pCmd->nHeight = UseHeight();
        pCmd->nWidth /= nMatrix;
        pCmd->nHeight /= nMatrix;
        pCmd->nOffLeft = (pCmd->nWidth * w) +  m_StillImage.GetBorderWidth();
        pCmd->nOffTop =  (pCmd->nHeight * h) + m_StillImage.GetBorderHeight();
      
        // Build image_convert.sh "source.jpg" "/tmp/image/source.jpg-i.pnm" 256 192 0 0 0 0 original
        pCmd->szPNM = strdup(pImage[(h*nMatrix)+w]->NameIndex());

      
    
        asprintf(&pCmd->szCmd, "%s \"%s\" \"%s\" %d %d %d %d %d %s", 
            g_szConvertScript,
            pImage[(h*nMatrix)+w]->Name(), 
            pCmd->szPNM,
            pCmd->nWidth,
            pCmd->nHeight,
            0,
            0,
            0,
            /*szChange ? szChange : */"");
        pCmd->szNumber = '0'+((h*nMatrix)+w)+1;

        Exec(pCmd);
      }
      return true;
  }
  return false;
}


bool cImagePlayer::ConvertZoom(const char *szChange, int nZoomFaktor,
			       int nLeftPos, int nTopPos)
{
  cImageData* pImage = theSlideShow.GetImage();
  if(pImage)
  {
    cShellWrapper* pCmd = new cShellWrapper;
    pCmd->bClearBackground = true;  
    pCmd->nOffLeft = m_StillImage.GetBorderWidth() + (nLeftPos>0?0:(nLeftPos*-1));
    pCmd->nOffTop = m_StillImage.GetBorderHeight() + (nTopPos>0?0:(nTopPos*-1));
    pCmd->nWidth = UseWidth();
    pCmd->nHeight = UseHeight();
  
    // Build image_convert.sh "source.jpg" "/tmp/image/dest.pnm" 720 576 0 0 0 0 original
    pCmd->szPNM = strdup(pImage->NameZoom());
      
    asprintf(&pCmd->szCmd, "%s \"%s\" \"%s\" %d %d %d %d %d %s", 
        g_szConvertScript,
        pImage->Name(), 
        pCmd->szPNM,
        pCmd->nWidth,
        pCmd->nHeight,
        nZoomFaktor,
        nLeftPos>0?nLeftPos:0,
        nTopPos>0?nTopPos:0,
        szChange ? szChange : "");
  
    Exec(pCmd);
    return true;
  }
  return false;
}


void cImagePlayer::LoadImage(cShellWrapper* pShell)
{
    cPNM pnmImage;
    bool bSuccess = false;
    register unsigned int nHeight = m_StillImage.GetHeight();
    register unsigned int nWidth = m_StillImage.GetWidth();
    register unsigned int nOffLeft = 0;
    register unsigned int nOffTop = 0;
    errno = 0;
    if(!pShell || pShell->bClearBackground)
      m_StillImage.ClearRGBMem();

    if(pShell && pShell->szPNM)
    {  
      nHeight = std::min(nHeight,pShell->nHeight);
      nWidth = std::min(nWidth,pShell->nWidth);
      nOffLeft = pShell->nOffLeft;
      nOffTop = pShell->nOffTop;
  
      FILE *f=fopen(pShell->szPNM, "r");
      if(f)
      {
        xel* pRow = NULL;
        register unsigned int w;
        register unsigned int h;
      
        if(pnmImage.readHeader(f))
        {
          register unsigned int nColorDepth = pnmImage.GetColorDepth();
        
          if(pnmImage.GetWidth() < nWidth)
            nOffLeft +=  (nWidth - pnmImage.GetWidth()) / 2;
    
          if(pnmImage.GetHeight() < nHeight)
            nOffTop +=  (nHeight - pnmImage.GetHeight()) / 2;
  
  
          for(h = 0;
              h < pnmImage.GetHeight() 
              && h < nHeight 
              && h+nOffTop < m_StillImage.GetHeight();
              ++h)
            {  
              if(!pnmImage.allocrow(&pRow) 
                ||!pnmImage.readrow(f, pRow) )
               break;
  
              xel* pP = pRow;
              for(w = 0;w < pnmImage.GetWidth() 
                 && w < nWidth
                 && w+nOffLeft < m_StillImage.GetWidth();
                 ++w,++pP)
              {  
                uint8_t* pImageRGB = m_StillImage.GetRGBMem() + 
                          ((((h+nOffTop)*m_StillImage.GetWidth())+w+nOffLeft)*3);
            
                if(nColorDepth == 0xFF) // normal 8-bit at any canal (24 Bit) colordepth
                {  
                  *(pImageRGB + 0) = (uint8_t) PPM_GETR(*pP);
                  *(pImageRGB + 1) = (uint8_t) PPM_GETG(*pP);
                  *(pImageRGB + 2) = (uint8_t) PPM_GETB(*pP);
                }
                else if(nColorDepth == 1) // black/white image
                {  
                  *(pImageRGB + 0) = (uint8_t) PPM_GETR(*pP)==0?0x00:0xFF;
                  *(pImageRGB + 1) = (uint8_t) PPM_GETG(*pP)==0?0x00:0xFF;
                  *(pImageRGB + 2) = (uint8_t) PPM_GETB(*pP)==0?0x00:0xFF;
                }
                else // Adjust other colordepth to 8bit
                {  
                  *(pImageRGB + 0) = (uint8_t) (PPM_GETR(*pP)*255 / pnmImage.GetColorDepth()) & 0xFF;
                  *(pImageRGB + 1) = (uint8_t) (PPM_GETG(*pP)*255 / pnmImage.GetColorDepth()) & 0xFF;
                  *(pImageRGB + 2) = (uint8_t) (PPM_GETB(*pP)*255 / pnmImage.GetColorDepth()) & 0xFF;
                }
              }
              pnmImage.freerow(pRow);
              pRow = NULL;
            }
          if(pRow)
            pnmImage.freerow((char*)pRow);
          else
          {
            if(pShell->szNumber && ImageSetup.m_bShowNumbers)
              cXPM::Overlay(pShell->szNumber,m_StillImage.GetRGBMem(),
                  m_StillImage.GetWidth(),m_StillImage.GetHeight(),
                  cXPM::TopRight,nOffLeft,nOffTop,pnmImage.GetWidth(),pnmImage.GetHeight());
  
            bSuccess = true;
          }
        }
        fclose(f);
        if (ImageSetup.m_bRemoveImmediately) {
            cImageData::Unlink (pShell->szPNM);
        }
      }
    }
  if(!bSuccess) {
  
    // Merge Errorimage with Encoder-Memory
    if(pShell && pShell->szNumber)
      cXPM::Overlay('s',m_StillImage.GetRGBMem(),
          m_StillImage.GetWidth(),m_StillImage.GetHeight(),
          cXPM::Center,pShell->nOffLeft,pShell->nOffTop,pShell->nWidth,pShell->nHeight);
    else
      cXPM::Error(m_StillImage.GetRGBMem(),
        m_StillImage.GetWidth(),m_StillImage.GetHeight());
    
    // Build Error, depends PNM Errormsg or system messages
    char szErr[128];
    szErr[0] = '\0';

    if(pnmImage.GetError())
      strncpy(szErr,pnmImage.GetError(),sizeof(szErr));
    else if(errno) {  
        int nErr = errno;
        szErr[sizeof(szErr)-1] = '\0';
        if(0 != strerror_r(nErr,szErr,sizeof(szErr)-1)) {
            szErr[0] = '\0';
        } 
    }

    // Make Syslog entry
    if(pShell && pShell->szPNM && szErr[0] != '\0') 
      esyslog("imageplugin: Error until read %s : '%s'", pShell->szPNM, szErr);
    else if(pShell && pShell->szPNM) 
      esyslog("imageplugin: Error until read %s", pShell->szPNM);
    else  
      esyslog("imageplugin: Error until read image %s",(szErr[0] != '\0')?szErr:"");
    
    { // Copy Errormessage forward to OSD Thread
      cMutexLock lock(&m_MutexErr);
        if(m_szError)
          free(m_szError);
        if(szErr[0] != '\0')
          asprintf(&m_szError, "%s : %s", tr("Image couldn't load"),szErr);
        else
          m_szError = strdup(tr("Image couldn't load"));
    }
  }
}

////////////////////////////////////////////////////////////////////////////////
/** void cImagePlayer::ExecFailed()
Show a precompiled Image if Exec can't find a converted Image
@param  - char* szErr
@return - nothing */
void cImagePlayer::ExecFailed(cShellWrapper* pShell,const char* szErr)
{
    if(!pShell || pShell->bClearBackground)
      m_StillImage.ClearRGBMem();
    
    // Merge Errorimage with Encoder-Memory
    if(pShell && pShell->szNumber)
      cXPM::Overlay('s',m_StillImage.GetRGBMem(),
          m_StillImage.GetWidth(),m_StillImage.GetHeight(),
          cXPM::Center,pShell->nOffLeft,pShell->nOffTop,pShell->nWidth,pShell->nHeight);
    else
      cXPM::Error(m_StillImage.GetRGBMem(),
        m_StillImage.GetWidth(),m_StillImage.GetHeight());


  // Store Message for OSD-Thread
  {
    cMutexLock lock(&m_MutexErr);
      if(m_szError)
        free(m_szError);
      m_szError = strdup(szErr);
  }
}

////////////////////////////////////////////////////////////////////////////////
/** void cImagePlayer::ErrorMsg()
ThreadSafe Method to show messages from Worker thread,
this functions is called only from cImageControl::ProcessKey(kNone)
@return - nothing */
void cImagePlayer::ErrorMsg()
{
  char* szErr = NULL;
  {
    cMutexLock lock(&m_MutexErr);
    szErr = m_szError;
    m_szError = NULL;
  }
  if(szErr)
  {  
    OSD_ErrorMsg(szErr);
    free(szErr);
  }
}

const char* cImagePlayer::FileName(void) const
{ 
  cImageData* pImage = theSlideShow.GetImage();
  return pImage?pImage->Name():NULL;
}

void cImagePlayer::Exec(cShellWrapper* pCmd)
{
  if(pCmd->szPNM)
    m_bConvertRunning = true;
  if(pCmd) {
    cMutexLock lock(&m_Mutex);
    if(!m_queue.add(pCmd))
      delete pCmd;          //Queue full or Thread is dead     
  }
}

bool cImagePlayer::Worker(bool bDoIt)
{
  bool bQueueEmpty;
  cShellWrapper *pShell = NULL;
  
  { // Protect the queue ++
    cMutexLock lock(&m_Mutex);
    if(bDoIt && !m_queue.empty()) {
      pShell = *m_queue.begin();
      m_queue.erase(m_queue.begin());
    }
    bQueueEmpty = m_queue.empty();
  } // ++

  if(!pShell) 
  {
    m_bConvertRunning = m_StillImage.EncodeRequired();
    return bQueueEmpty;
  }  

  if(pShell->szCmd) {
    //dsyslog("imageplugin: executing script '%s'", pShell->szCmd);
    
    ImageSetup.SetEnv();
    if(0 == SystemExec(pShell->szCmd))
    {  
      if(pShell->szPNM) {
          LoadImage(pShell);
          m_StillImage.EncodeRequired(true);
      }
    }    
    else
    {
      esyslog("imageplugin: script execution failed '%s'", pShell->szCmd);
      if(pShell->szPNM) {
        ExecFailed(pShell,tr("Script execution failed"));        
        m_StillImage.EncodeRequired(true);
      }
    }   
  } 
  delete pShell;
  return bQueueEmpty;
}
