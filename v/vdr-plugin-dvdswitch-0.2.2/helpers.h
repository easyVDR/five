#ifndef __HELPERS_DVDSWITCH_H
#define __HELPERS_DVDSWITCH_H

#include <sys/types.h>
#include <sys/stat.h>
#include <regex.h>
#include <ctype.h>
#include <cmath>
#include <stdio.h>
#include <unistd.h>

#include <vdr/tools.h>
#include <vdr/interface.h>
#include <vdr/thread.h>

#define FREENULL(p) (free(p), p = NULL)

void OsdMsg(eMessageType Type, const char *Msg);
void OSDErrorNumMsg(int err, const char* szDef);
void ChangeChars(char *name, char *chars);
bool RegIMatch(const char *string,const char *pattern);
char *get_strerror(int n);

// --- cStringValue -------------------------------------

class cStringValue
{
  private:
    char *String;

    void Init(const char *string)
    {
      FREENULL(String);
      String = string ? strdup(string) : NULL;
    }
  public:
    cStringValue(void) { String = NULL; }
    ~cStringValue(void) { free(String); }

    cStringValue &operator= (const char *string)
    {
      Init(string);
      return *this;
    }
    cStringValue &operator+= (const char *string)
    {
      if(String && string)
      {
        String = (char*) realloc(String, strlen(String) + strlen(string) + 1);
        strcat(String, string);
      }
      if(!String)
        Init(string);

      return *this;
    }
    char *operator& (void) { return String; }

    int len(void) const { return String ? strlen(String) : 0; }
};

// --- cTokenizer ---------------------------------------

class cToken : public cListObject
{
  private:
    char *String;
  public:
    cToken(char *string) {
      String = string ? strdup(string) : NULL;
    }
    ~cToken(void) { free(String); }
    char *Value(void) { return String; }
};

class cTokenizer : public cList<cToken>
{
  private:
    char *String;
    char *Delim;

    void Tokenize(bool trim);
  public:
    cTokenizer(const char *string,const char *delim, bool trim = false)
    {
      String = string ? strdup(string) : NULL;
      Delim = delim ? strdup(delim) : NULL;
      if(String && Delim)
        Tokenize(trim);
    }
    ~cTokenizer(void)
    {
      free(String);
      free(Delim);
    }
    const char *GetToken(int index) const { return index > 0 && index <= Count() ? Get(index - 1)->Value() : NULL; }
};

// --- cFileInfo ----------------------------------------

enum eFileInfo
{
  tNone = 0,
  tDir,
  tFile,
  tUnknown,
};

class cFileInfo
{
  private:
    char *File;
    char *buffer;
    struct stat64 Info;
    unsigned long long int size;

  protected:
    bool hasGroup(gid_t gid);

  public:
    cFileInfo(const char *file);
    ~cFileInfo(void);

    char *Path(void);
    char *FileName(void);
    char *FileNameWithoutExt(void);
    char *Extension(bool withPoint = true);
    eFileInfo Type(void);
    unsigned long long int SizeByte(void);
    double SizeKByte(int decimals = 2);
    double SizeMByte(int decimals = 2);
    double SizeGByte(int decimals = 1);
    bool isReadable(void);
    bool isWriteable(void);
    bool isExecutable(void);
    bool isExists(void);
};

// --- cFileCMD ------------------------------------------

class cFileCMD
{
  public:
    static bool Del(const char *file);
    static bool Mkdir(const char *dir) { return MakeDirs(dir, true); }
    static bool Rn(const char *oldfile, const char *newfile) { return 0 == rename(oldfile, newfile); }
    static bool DirIsEmpty(const char *file);
};

// --- cFileList -------------------------------

enum eFileList
{
  sNone,
  sAsc,
  sDesc,
};

class cFileOptListItem : public cListObject
{
  private:
    char *RegEx;
    eFileInfo Type;
  public:
    cFileOptListItem(const char *regex, eFileInfo type = tNone)
    {
      RegEx = (regex && !isempty(regex)) ? strdup(regex) : NULL;
      Type = type;
    }
    ~cFileOptListItem(void) { free(RegEx); }

    char *Regex(void) { return RegEx; }
    eFileInfo FileType(void) { return Type; }
};

class cFileOptList : public cList<cFileOptListItem> {};

class cFileList;

class cFileListItem : public cListObject
{
  friend class cFileList;
  private:
    char *File;
  public:
    cFileListItem(const char *dir, const char *file);
    cFileListItem(const char *file);
    ~cFileListItem(void) { free(File); }
    char *Value(void) { return File; }
};

class cFileList : public cList<cFileListItem>
{
  private:
    char *Dir;
    bool Sub;

    cFileOptList *Includes;
    cFileOptList *Excludes;
    bool Protect;

    eFileList SortMode;
    bool SortToFilename;
    eFileInfo Type;

    bool Read(const char *dir, bool withsub);
    void SortIn(const char *dir, const char *file);

    bool CheckIncludes(const char *dir, const char *file);
    bool CheckExcludes(const char *dir, const char *file);
    bool CheckType(const char *dir, const char *file, eFileInfo type);
  public:
    cFileList(void);
    ~cFileList(void);

    bool Load(const char *dir = NULL, bool withsub = false);
    void OptInclude(const char *include, eFileInfo type = tNone)
    {
      if(!Includes)
        Includes = new cFileOptList();
      if(!include)
        Includes->Clear();
      else
        Includes->Add(new cFileOptListItem(include, type));
    }
    void OptExclude(const char *exclude,  eFileInfo type = tNone)
    {
      if(!Excludes)
        Excludes = new cFileOptList();
      if(!exclude)
        Excludes->Clear();
      else
        Excludes->Add(new cFileOptListItem(exclude, type));
    }
    void OptSort(eFileList mode, bool tofilename = false)
    {
      SortMode = mode;
      SortToFilename = tofilename;
    }
    void OptFilterType(eFileInfo type) { Type = type; }

    bool DirIsEmpty(cFileListItem *item);
    static bool DirIsIn(const char *file, const char *strings);
    static bool DirIsIn(cFileListItem *item, const char *strings) { return DirIsIn(item->Value(), strings); }
};

#endif // __HELPERS_DVDSWITCH_H
