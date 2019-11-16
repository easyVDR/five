/*
 * Image plugin to VDR (C++)
 *
 * (C) 2004-2011 Andreas Brachold    <vdr07 at deltab.de>
 *     2003 Kai Tobias Burwieck <kai-at-burwieck.net>
 *
 * This code is distributed under the terms and conditions of the
 * GNU GENERAL PUBLIC LICENSE. See the file COPYING for details.
 *
 */

#ifndef ___DVB_IMAGE_H
#define ___DVB_IMAGE_H

#include <vector>
#define __STL_CONFIG_H

#include <vdr/thread.h>
#include <vdr/player.h>
#include <liboutput/stillimage-player.h>

class cImageData;
class cSlideShow;    

struct cShellWrapper {
    
  char* szCmd;
  char* szPNM;
  char  szNumber;
  unsigned int nOffLeft;
  unsigned int nOffTop;
  unsigned int nWidth;
  unsigned int nHeight;
  bool bClearBackground;

  cShellWrapper()
  : szCmd(NULL)
  , szPNM(NULL)
  , szNumber('\0')
  , nOffLeft(0)
  , nOffTop(0)
  , nWidth(0)
  , nHeight(0)
  , bClearBackground(false)

  {
  }

  virtual ~cShellWrapper() {
    if(szPNM)
      free(szPNM);
    if(szCmd)
      free(szCmd);
  }
};

struct cShellWrapperQueue
 : public std::vector<cShellWrapper*>
{
  virtual ~cShellWrapperQueue()
  {
    iterator i = begin();
    const_iterator e = end();
    for(;e!=i;++i)
        delete(*i);
    clear();
  }

  inline size_t max_size() const 
  {
    return 64;
  }

  inline bool add(cShellWrapper* pCmd) {
    if(size()<max_size())
    {
      if(NULL == pCmd->szPNM || !pCmd->bClearBackground) //Pregeneration or Index
        push_back(pCmd);
      else { 
        // Remove all other viewed images from queue
        iterator i = begin();
        while(end()!=i) {
          if((*i)->szPNM) {
            delete(*i);
            erase(i);
            }
          else
            ++i;
          }
        //Place next viewed image at front of the queue
        insert(begin(),pCmd);
      }
      return true;
    }  
    return false;
  }
  
};

class cImagePlayer
: public cStillImagePlayer
{
    
  volatile bool               m_bConvertRunning;

  cMutex                      m_Mutex;
  cShellWrapperQueue          m_queue;
  cMutex                      m_MutexErr;
  char*                       m_szError;
protected:
  void Exec(cShellWrapper* pCmd);

  void LoadImage(cShellWrapper* pShell);
  /** Show Errorimage if operation failed*/
  void ExecFailed(cShellWrapper* pShell,const char* szErr);

  virtual void Activate(bool On);
  virtual bool Worker(bool bDoIt);
public:

  cImagePlayer(cSlideShow *pCurSlideShow);
  virtual ~ cImagePlayer();
  bool IsConvertRunning() const { return m_bConvertRunning; };
  bool NextImage(int Step);
  bool PrevImage(int Step);
  bool GotoImage(unsigned int nNewPictureIndex);
  
  /** Deliver the filename from the current number of viewed Image */
  const char* FileName(void) const;

  bool Convert(const char *szChange);
  bool ConvertZoom(const char *szChange, int nZoomFaktor,
			       int nLeftPos, int nTopPos);
  bool ConvertJump(int Step);
  
  /** Returns the current and total frame index, optionally snapped to the
  nearest I-frame.*/
  virtual bool GetIndex(int &Current, int &Total, bool SnapToIFrame);
  /** ThreadSafe Method to show messages from Worker thread*/
  void ErrorMsg();
};

#endif				//__DVB_IMAGE_H
