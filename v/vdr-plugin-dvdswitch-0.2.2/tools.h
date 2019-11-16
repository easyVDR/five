#ifndef __TOOLS_DVDSWITCH_H
#define __TOOLS_DVDSWITCH_H

#include "helpers.h"

class cDirList : public cFileList
{
  public:
    cDirList(cImageList &ImageList);
};

class cFileDelThread : public cThread
{
  private:
    char *File;
    bool Ok;

    bool RightCheck(const char *value);
  protected:
    virtual void Action(void);
  public:
    cFileDelThread(const char *file);
    virtual ~cFileDelThread(void);
    bool OK(void) { return Ok; };
};

class cFileMoveThread : public cThread
{
    char *FileName;
    char *File;
    char *Dest;
    bool Ok;

    bool RightCheck(const char *value);
  protected:
    virtual void Action(void);
  public:
    cFileMoveThread(const char *file, char *dest);
    virtual ~cFileMoveThread(void);
    bool OK(void) { return Ok; };
};

#endif // __TOOLS_DVDSWITCH_H
