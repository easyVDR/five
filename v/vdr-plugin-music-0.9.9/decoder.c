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

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include <string>
#include <fstream>

#include <vdr/videodir.h>

#include "common.h"
#include "data-mp3.h"
#include "decoder.h"
#include "decoder-core.h"
#include "decoder-mp3.h"
#include "decoder-mp3-stream.h"
#include "decoder-snd.h"
#include "decoder-ogg.h"
//#include "decoder-mp4.h"
#include "setup-mp3.h"
#include "vars.h"

#define CACHEFILENAME     "id3info.dat"
#define CACHESAVETIMEOUT  120 // secs
#define CACHEPURGETIMEOUT 120 // days

extern  cFileSources MP3Sources;

cInfoCache InfoCache;
char *cachedir=0;

int MakeHashBuff(const char *buff, int len)
{
  int h=len;
  while(len--) h=(h*13 + *buff++) & 0x7ff;
  return h;
}

// --- cSongInfo ---------------------------------------------------------------

cSongInfo::cSongInfo(void)
{
  Title=Artist=Album=Genre=Comment=0;
  Clear();
}

cSongInfo::~cSongInfo()
{
  Clear();
}

void cSongInfo::Clear(void)
{
  Frames=0; Total=-1; DecoderID=0;
  SampleFreq=Channels=Bitrate=MaxBitrate=ChMode=-1;
  free(Title); Title=0;
  free(Artist); Artist=0;
  free(Album); Album=0;
  free(Genre); Genre=0;
  free(Comment); Comment=0;
  Rating=0;
  Year=-1;
  Level=Peak=0.0;
  infoDone=false;
}

void cSongInfo::Set(cSongInfo *si)
{
  static char empty[128];
  strcpy(empty,tr("Unknown"));
  Clear(); InfoDone();
  Frames=si->Frames;
  Total=si->Total;
  SampleFreq=si->SampleFreq;
  Channels=si->Channels;
  Bitrate=si->Bitrate;
  MaxBitrate=si->MaxBitrate;
  ChMode=si->ChMode;
  Rating=si->Rating;
  Year=si->Year;
  Title=si->Title ? strdup(si->Title):strdup(empty);
  Artist=si->Artist ? strdup(si->Artist):strdup(empty);
  Album=si->Album ? strdup(si->Album):strdup(empty);
  Genre=si->Genre ? strdup(si->Genre):strdup(empty);
  Comment=si->Comment ? strdup(si->Comment):strdup(tr("No comment"));
  if(si->Level>0.0) { // preserve old level
    Level=si->Level;
    Peak=si->Peak;
    }
  DecoderID=si->DecoderID;
}

void cSongInfo::FakeTitle(const char *filename, const char *extention)
{
  // if no title, try to build a reasonable from the filename
  if(!Title && filename)  {
    const char *s=rindex(filename,'/');
    if(s && *s=='/') {
      s++;
      Title=strdup(s);
      strreplace(Title,'_',' ');
      if(extention) {                            // strip given extention
        int l=strlen(Title)-strlen(extention);
        if(l>0 && !strcasecmp(Title+l,extention)) Title[l]=0;
        }
      else {                                     // strip any extention
        s=rindex(Title,'.');
        if(s && *s=='.' && strlen(s)<=5) s=0;
        }
      d(printf("music: decoder: faking title '%s' from filename '%s'\n",Title,filename))
      }
    }
}

// --- cFileInfo ---------------------------------------------------------------

cFileInfo::cFileInfo(void)
{
  Filename=FsID=0; Clear();
}

cFileInfo::cFileInfo(const char *Name)
{
  Filename=FsID=0; Clear();
  Filename=strdup(Name);
}

cFileInfo::~cFileInfo()
{
  Clear();
}

void cFileInfo::Clear(void)
{
  free(Filename); Filename=0;
  free(FsID); FsID=0;
  Filesize=0; CTime=0; FsType=0; removable=-1;
  infoDone=false;
}

bool cFileInfo::Removable(void)
{
  if(removable<0 && Filename) {
  
    std::string Base = Filename;

    int len = Base.length();
    std::string::size_type pos = Base.rfind('/',len);
    if(pos!=std::string::npos) {
      Base = Base.substr(0,pos);
      cFileSource *src=MP3Sources.FindSource(Base.c_str(), false);
      if(src)
        removable=src->NeedsMount();
      else
       removable=1;
    }
  }

  return (removable!=0);
}

void cFileInfo::Set(cFileInfo *fi)
{
  Clear(); InfoDone();
  Filename=fi->Filename ? strdup(fi->Filename):0;
  FsID=fi->FsID ? strdup(fi->FsID):0;
  Filesize=fi->Filesize;
  CTime=fi->CTime;
}


bool cFileInfo::FileInfo(bool log)
{
  if(Filename) {
    struct stat64 ds;
    if(!stat64(Filename,&ds)) {
      if(S_ISREG(ds.st_mode)) {
        free(FsID); FsID=0;
        FsType=0;
        struct statfs64 sfs;
        if(!statfs64(Filename,&sfs)) {
          if(Removable()) {
            asprintf(&FsID,"%llx:%llx",sfs.f_blocks,sfs.f_files);
          }	
          FsType=sfs.f_type;
        }
        else if(errno!=ENOSYS && log) { esyslog("music: ERROR: can't statfs %s: %s",Filename,strerror(errno)); }
        Filesize=ds.st_size;
        CTime=ds.st_ctime;
#ifdef CDFS_MAGIC
        if(FsType==CDFS_MAGIC) CTime=0; // CDFS returns mount time as ctime
#endif
        InfoDone();
        return true;
        }
      else if(log) { esyslog("music: ERROR: %s is not a regular file",Filename); }
      }
    else if(log) { esyslog("music: ERROR: can't stat %s: %s",Filename,strerror(errno)); }
    }
  return false;
}

// --- cDecoders ---------------------------------------------------------------

cDecoder *cDecoders::FindDecoder(cFileObj *Obj)
{
  const char *full=Obj->FullPath();
  cFileInfo fi(full);
  cCacheData *dat;
  cDecoder *decoder=0;
  if(fi.FileInfo(false) && (dat=InfoCache.Search(&fi))) {
    if(dat->DecoderID) {
      //d(printf("mp3: found DecoderID '%s' for %s from cache\n",cDecoders::ID2Str(dat->DecoderID),Filename))
      switch(dat->DecoderID) {
            case DEC_MP3:
                          decoder=new cMP3Decoder(full);
                          break;
            case DEC_MP3S:
                          decoder=new cMP3StreamDecoder(full);
                          break;
#ifdef HAVE_VORBISFILE
            case DEC_OGG:
                          decoder=new cOggDecoder(full);
                          break;
#endif

#ifdef HAVE_SNDFILE
            case DEC_SND:
                          decoder=new cSndDecoder(full);
                          break;
#endif

/*
#ifdef HAVE_MP4
            case DEC_MP4:
                          decoder=new cMP4Decoder(full);
                          break;
#endif
*/
            default:      esyslog("music: ERROR: bad DecoderID '%s' from info cache: %s",cDecoders::ID2Str(dat->DecoderID),full); break;
      }
    }
    dat->Unlock();
  }

  if(!decoder || !decoder->Valid()) {
    // no decoder in cache or cached decoder doesn't matches.
    // try to detect a decoder

    delete decoder; decoder=0;

#ifdef HAVE_VORBISFILE
    if(!decoder) {
      decoder=new cOggDecoder(full);
      if(!decoder || !decoder->Valid()) { delete decoder; decoder=0; }
      }
#endif


#ifdef HAVE_SNDFILE
    if(!decoder) {
      decoder=new cSndDecoder(full);
      if(!decoder || !decoder->Valid()) { delete decoder; decoder=0; }
      }
#endif

/*
#ifdef HAVE_MP4
    if(!decoder) {
      decoder=new cMP4Decoder(full);
      if(!decoder || !decoder->Valid()) { delete decoder; decoder=0; }
      }
#endif
*/
    if(!decoder) {
      decoder=new cMP3StreamDecoder(full);
      if(!decoder || !decoder->Valid()) { delete decoder; decoder=0; }
      }
    if(!decoder) {
      decoder=new cMP3Decoder(full);
      if(!decoder || !decoder->Valid()) { delete decoder; decoder=0; }
      }
    if(!decoder) esyslog("music: ERROR: no decoder found for %s",Obj->Name());
    }
  return decoder;
}

const char *cDecoders::ID2Str(int id)
{
  switch(id) {
    case DEC_MP3:  return DEC_MP3_STR;
    case DEC_MP3S: return DEC_MP3S_STR;
    case DEC_SND:  return DEC_SND_STR;
    case DEC_OGG:  return DEC_OGG_STR;
//    case DEC_MP4:  return DEC_MP4_STR;
    }
  return 0;
}

int cDecoders::Str2ID(const char *str)
{
  if     (!strcmp(str,DEC_MP3_STR )) return DEC_MP3;
  else if(!strcmp(str,DEC_MP3S_STR)) return DEC_MP3S;
  else if(!strcmp(str,DEC_SND_STR )) return DEC_SND;
  else if(!strcmp(str,DEC_OGG_STR )) return DEC_OGG;
//  else if(!strcmp(str,DEC_MP4_STR )) return DEC_MP4;
  return 0;
}

// --- cDecoder ----------------------------------------------------------------

cDecoder::cDecoder(const char *Filename)
{
  filename=strdup(Filename);
  locked=0; urgentLock=playing=false;
}

cDecoder::~cDecoder()
{
  free(filename);
}

void cDecoder::Lock(bool urgent)
{
  locklock.Lock();
  if(urgent && locked) urgentLock=true; // signal other locks to release quickly
  locked++;
  locklock.Unlock(); // don't hold the "locklock" when locking "lock", may cause a deadlock
  lock.Lock();
  urgentLock=false;
}

void cDecoder::Unlock(void)
{
  locklock.Lock();
  locked--;
  lock.Unlock();
  locklock.Unlock();
}

bool cDecoder::TryLock(void)
{
  bool res=false;
  locklock.Lock();
  if(!locked && !playing) {
    Lock();
    res=true;
    }
  locklock.Unlock();
  return res;
}

// --- cCacheData -----------------------------------------------------

cCacheData::cCacheData(void)
{
  touch=0; version=0;
}

void cCacheData::Touch(void)
{
  touch=time(0);
}

#define SECS_PER_DAY (24*60*60)

bool cCacheData::Purge(void)
{
  time_t now=time(0);
  //XXX does this realy made sense?
  //if(touch+CACHEPURGETIMEOUT*SECS_PER_DAY < now) {
  //  d(printf("cache: purged: timeout %s\n",Filename))
  //  return true;
  //  }
  if(touch+CACHEPURGETIMEOUT*SECS_PER_DAY/10 < now) {
    if(!Removable()) {                            // is this a permant source?
      struct stat64 ds;                           // does the file exists? if not, purge
      if(stat64(Filename,&ds) || !S_ISREG(ds.st_mode) || access(Filename,R_OK)) {
        d(printf("music: decoder: cache: purged: file not found %s\n",Filename))
        return true;
        }
      }
    }
  return false;
}

bool cCacheData::Upgrade(void)
{
  if(version<7) {
    if(DecoderID==DEC_SND || (Title && startswith(Title,"track-")))
      return false;              // Trash older SND entries (incomplete)

    if(Removable()) {
      if(!FsID) FsID=strdup("old"); // Dummy entry, will be replaced in InfoCache::Search()
      }
    else { free(FsID); FsID=0; }
    }
  if(version<4) {
    Touch();                     // Touch entry
    }
  if(version<3 && !Title) {
    FakeTitle(Filename,".mp3");  // Fake title
    }
  if(version<2 && Bitrate<=0) {
    return false;                // Trash entry without bitrate
    }
  return true;
}

void cCacheData::Create(cFileInfo *fi, cSongInfo *si)
{
  cFileInfo::Set(fi);
  cSongInfo::Set(si);
  hash=MakeHash(Filename);
  Touch();
}

bool cCacheData::Save(FILE *f)
{
  fprintf(f,"##BEGIN\n"
            "Filename=%s\n"
            "Filesize=%lld\n"
            "Timestamp=%ld\n"
            "Touch=%ld\n"
            "Version=%d\n"
            "Frames=%d\n"
            "Total=%d\n"
            "SampleFreq=%d\n"
            "Channels=%d\n"
            "Bitrate=%d\n"
            "MaxBitrate=%d\n"
            "ChMode=%d\n"
            "Year=%d\n"
            "Rating=%d\n"
            "Level=%.4f\n"
            "Peak=%.4f\n",
            Filename,Filesize,CTime,touch,CACHE_VERSION,Frames,Total,SampleFreq,Channels,Bitrate,MaxBitrate,ChMode,Year,Rating,Level,Peak);
  if(Title)      fprintf(f,"Title=%s\n"      ,Title);
  if(Artist)     fprintf(f,"Artist=%s\n"     ,Artist);
  if(Album)      fprintf(f,"Album=%s\n"      ,Album);
  if(Genre)      fprintf(f,"Genre=%s\n"      ,Genre);
  if(Comment)    fprintf(f,"Comment=%s\n"    ,Comment);
  if(DecoderID)  fprintf(f,"DecoderID=%s\n"  ,cDecoders::ID2Str(DecoderID));
  if(FsID)       fprintf(f,"FsID=%s\n"       ,FsID);
  fprintf(f,"##END\n");
  return ferror(f)==0;
}

bool cCacheData::Load(FILE *f)
{
  static const char delimiters[] = { "=\n" };
  char buf[1024];

  cFileInfo::Clear();
  cSongInfo::Clear();
  while(fgets(buf,sizeof(buf),f)) {
    char *ptrptr;
    char *name =strtok_r(buf ,delimiters,&ptrptr);
    char *value=strtok_r(0,delimiters,&ptrptr);
    if(name) {
      if(!strcasecmp(name,"##END")) break;
      if(value) {
        if     (!strcasecmp(name,"Filename"))   Filename  =strdup(value);
        else if(!strcasecmp(name,"Filesize") ||
                !strcasecmp(name,"Size"))       Filesize  =atoll(value);
        else if(!strcasecmp(name,"FsID"))       FsID      =strdup(value);
        else if(!strcasecmp(name,"Timestamp"))  CTime     =atol(value);
        else if(!strcasecmp(name,"Touch"))      touch     =atol(value);
        else if(!strcasecmp(name,"Version"))    version   =atoi(value);
        else if(!strcasecmp(name,"DecoderID"))  DecoderID =cDecoders::Str2ID(value);
        else if(!strcasecmp(name,"Frames"))     Frames    =atoi(value);
        else if(!strcasecmp(name,"Total"))      Total     =atoi(value);
        else if(!strcasecmp(name,"SampleFreq")) SampleFreq=atoi(value);
        else if(!strcasecmp(name,"Channels"))   Channels  =atoi(value);
        else if(!strcasecmp(name,"Bitrate"))    Bitrate   =atoi(value);
        else if(!strcasecmp(name,"MaxBitrate")) MaxBitrate=atoi(value);
        else if(!strcasecmp(name,"ChMode"))     ChMode    =atoi(value);
        else if(!strcasecmp(name,"Year"))       Year      =atoi(value);
        else if(!strcasecmp(name,"Rating"))     Rating    =atoi(value);
        else if(!strcasecmp(name,"Title"))      Title     =strdup(value);
        else if(!strcasecmp(name,"Artist"))     Artist    =strdup(value);
        else if(!strcasecmp(name,"Album"))      Album     =strdup(value);
        else if(!strcasecmp(name,"Genre"))      Genre     =strdup(value);
        else if(!strcasecmp(name,"Comment"))    Comment   =strdup(value);
        else if(!strcasecmp(name,"Level"))      Level     =atof(value);
        else if(!strcasecmp(name,"Peak"))       Peak      =atof(value);
        else d(printf("music: decoder: cache: ignoring bad token '%s' from cache file\n",name))
        }
      }
    }

  if(ferror(f) || !Filename) return false;
  hash=MakeHash(Filename);
  return true;
}

// --- cInfoCache ----------------------------------------------------

cInfoCache::cInfoCache(void)
{
  lasttime=expr1=expr2=0; modified=false;
  lastpurge=time(0)-(50*60);
}

void cInfoCache::Shutdown(void)
{
  Cancel(10);
  Save(true);
} 

void cInfoCache::Cache(cSongInfo *info, cFileInfo *file)
{
  lock.Lock();
  cCacheData *dat=Search(file);
  if(dat) {
    dat->Create(file,info);
    Modified();
    dat->Unlock();
    d(printf("music: decoder: cache: updating infos for %s\n",file->Filename))
    }
  else {
    dat=new cCacheData;
    dat->Create(file,info);
    AddEntry(dat);
    d(printf("music: decoder: cache: New caching infos for %s\n",file->Filename))
    }
  lock.Unlock();
}


void cInfoCache::Update(int rating, cFileInfo *file)
{
  int hash=MakeHash(file->Filename);
  lock.Lock();
  cCacheData *dat=FirstEntry(hash);
  while(dat) {
    if(dat->hash==hash && !strcmp(dat->Filename,file->Filename)) {
      d(printf("music: decoder: Update: Updating infos for %s\n",file->Filename))
      dat->Lock();
      dat->Rating = rating;
      dat->Unlock();
    }

    dat=(cCacheData *)dat->Next();
  }    

  lock.Unlock();
}  


void cInfoCache::Empty(void)
{
  lock.Lock();

  for(int i=0 ; i<CACHELINES ; i++) {
    lists[i].Clear();
  }  
  
  lock.Unlock();

  modified=true;
  Save(true);
  
#ifdef DEBUG
  printf("music: decoder: ID3 cache emptied\n");  
#endif  
}


cCacheData *cInfoCache::Search(cFileInfo *file)
{
  int hash=MakeHash(file->Filename);
  lock.Lock();
  cCacheData *dat=FirstEntry(hash);
  while(dat) {

//    if(dat->hash==hash && !strcmp(dat->Filename,file->Filename) && dat->Filesize==file->Filesize) {
    if(dat->hash==hash && !strcmp(dat->Filename,file->Filename)) {
      d(printf("music: decoder: cache: Existing entry found: %s\n",dat->Filename))
      dat->Lock();
      if(file->FsID && dat->FsID && !strcmp(dat->FsID,"old")) { // duplicate FsID for old entries
        dat->FsID=strdup(file->FsID);
        dat->Touch(); Modified();
        //d(printf("adding FsID for %s\n",dat->Filename))
        }

      if((!file->FsID && !dat->FsID) || (file->FsID && dat->FsID && !strcmp(dat->FsID,file->FsID))) {
        //d(printf("cache: found cache entry for %s\n",dat->Filename))
        dat->Touch(); Modified();
        if(dat->CTime!=file->CTime) {
          d(printf("music: decoder: cache: ctime differs, removing from cache: %s\n",dat->Filename))
          DelEntry(dat); dat=0;
          }
        break;
        }
      dat->Unlock();
      }
    dat=(cCacheData *)dat->Next();
    }
  lock.Unlock();
  return dat;
}


void cInfoCache::AddEntry(cCacheData *dat)
{
  lists[dat->hash%CACHELINES].Add(dat);
  Modified();
}

void cInfoCache::DelEntry(cCacheData *dat)
{
  dat->Lock();
  lists[dat->hash%CACHELINES].Del(dat);
  Modified();
}

cCacheData *cInfoCache::FirstEntry(int hash)
{
  return lists[hash%CACHELINES].First();
}

bool cInfoCache::Purge(void)
{
  time_t now=time(0);
  if(now-lastpurge>(60*60)) {
    isyslog("cleaning up id3 cache");
    Start();
    lastpurge=now;
//    d(printf("music: cleaned up id3 cache\n"));
    }
  return Active();
}

void cInfoCache::Action(void)
{
  d(printf("music: cache: id3 cache purge thread started (pid=%d)\n",getpid()))
  nice(3);
  lock.Lock();
  for(int i=0,n=0 ; i<CACHELINES && Running(); i++) {
    cCacheData *dat=FirstEntry(i);
    while(dat && Running()) {
      cCacheData *ndat=(cCacheData *)dat->Next();
      if(dat->Purge()) DelEntry(dat);
      dat=ndat;

      if(++n>30) {
        lastmod=false; n=0;
        lock.Unlock(); lock.Lock();    // give concurrent thread an access chance
        if(lastmod) dat=FirstEntry(i); // restart line, if cache changed meanwhile
        }
      }
    }
  lock.Unlock();
  d(printf("music: cache: id3 cache purge thread ended (pid=%d)\n",getpid()))
}

char *cInfoCache::CacheFile(void)
{
  using namespace std;
  string cachedir;
  string cachefile;
  string username;
  string datei;
  ifstream filestr;
  
  datei    = config;
  cachedir = datei;
  datei    = datei + "/data/current_user.dat"; 

  // load username from current_user.dat
#ifdef DEBUG
  isyslog("music: decoder: load current_user.dat from '%s'", datei.c_str());
#endif
  filestr.open(datei.c_str());
  if(filestr) {
    getline(filestr,username,'\n');
    filestr.close();
  }    

  cachedir = cachedir + "/data/" + username;
  cachefile = cachedir + "/" + CACHEFILENAME;
  d(printf("music: decoder: cachedir = '%s'\n", cachedir.c_str()));

  // create dir and cachefile if not exist
  if(!(DirectoryOk(cachedir.c_str(),true))) {
    if(MakeDirs(cachedir.c_str(), true)) {
      isyslog("music: cachedir created '%s'\n",cachedir.c_str());

      ofstream out(cachefile.c_str());
      out<<""<<endl;
      out.close();
      isyslog("music: empty cachefile created '%s'\n",cachefile.c_str());
      }
    else
      esyslog("music: ERROR: could not create cachedir '%s'\n", cachedir.c_str());
  }

  if(!strcmp(username.c_str(),"admin"))
    MP3Setup.user_is_admin = 1;
  else
    MP3Setup.user_is_admin = 0;    

  return AddPath(cachedir.c_str(),CACHEFILENAME);
}

void cInfoCache::Save(bool force)
{
//  if(MP3Setup.AutoPurge)  Purge();
  if(modified && (force || (!Purge() && time(0)>lasttime))) {
    char *name=CacheFile();
    cSafeFile f(name);
    free(name);
    if(f.Open()) {
      lock.Lock();
      fprintf(f,"## This is a generated file. DO NOT EDIT!!\n"
                "## This file will be OVERWRITTEN WITHOUT WARNING!!\n");
      for(int i=0 ; i<CACHELINES ; i++) {
        cCacheData *dat=FirstEntry(i);
        while(dat) {
          if(!dat->Save(f)) { i=CACHELINES+1; break; }
          dat=(cCacheData *)dat->Next();
          }
        }
      lock.Unlock();
      f.Close();
      modified=false; lasttime=time(0)+CACHESAVETIMEOUT;
      d(printf("music: decoder: cache: saved cache to file\n"))
      }
    }
}

bool cInfoCache::FullFileExist(const char *FullFile)
{
    return access(FullFile, F_OK) == 0;
}  


void cInfoCache::Load(bool reload)
{
  char *name=CacheFile();
  if(access(name,F_OK)==0) {
    if(!reload)  
      isyslog("music: loading id3 cache from %s",name);
    else  
      isyslog("music: reloading new id3 cache from %s",name);
    FILE *f=fopen(name,"r");
    if(f) {
      char buf[256];
      bool mod=false;
      lock.Lock();
      for(int i=0 ; i<CACHELINES ; i++) lists[i].Clear();
      while(fgets(buf,sizeof(buf),f)) {
        if(!strcasecmp(buf,"##BEGIN\n")) {
          cCacheData *dat=new cCacheData;
          if(dat->Load(f)) {
            if(dat->version!=CACHE_VERSION) {
              if(dat->Upgrade()) mod=true;
              else { delete dat; continue; }
              }
            AddEntry(dat);
            }
          else {
            delete dat;
            if(ferror(f)) {
              esyslog("music: ERROR: failed to load id3 cache");
              break;
              }
            }
          }
        }
      lock.Unlock();
      fclose(f);
      modified=false; if(mod) Modified();
      }
    else LOG_ERROR_STR(name);
    }
  free(name);
}

bool cInfoCache::SearchUp(const char *szArtist, const char *szTitle, const char *szGenre, const char *szFullFile, int szYearFrom, int szYearTo, int szRatingFrom, int szRatingTo, bool szPlayable)
{
  using namespace std;
  FILE *fs;
  FILE *ft;
  
  std::string Datei;
  std::string Outfile;
  std::string Artist;
  std::string Title;
  std::string Genre;
  std::string FullFile;

  std::string lineout;
  
  const char *artist=0;
  const char *title=0;
  const char *fullfile=0;
  const char *genre=0;
  const char *year=0;
  const char *rating=0;
  const char *bitrate=0;
  const char *total=0;
  
  int count=0;
  int index=0;

  bool searchresult = false;
  bool yearFromTo   = false;
  bool ratingFromTo = false;

  if((szYearFrom > -1) && (szYearTo > -1))     yearFromTo   = true;
  if((szRatingFrom > -2) && (szRatingTo > -2)) ratingFromTo = true;

  Artist  = szArtist;
  Title   = szTitle;
  Genre   = szGenre;
  FullFile= szFullFile;

// Check for Artist
  Datei   = BaseSource;
  Datei   = Datei + "/@Suche.dat";
  Outfile = BaseSource;
  Outfile = Outfile + "/@Temp.dat";


  int x,y,x2,y2;
  x  = szRatingFrom;
  x2 = szRatingTo;
  y  = 0;
  y2 = 0;

// This is different to rating.c because  there is 0 = 'not rated' and in cache 'not rated' is -1 AND 0
// Set Rating to real values

  if(x==-2)        y=-2;   // dont search for
   else if(x==-1)  y=-1;   // not rated
   else if(x==0)   y=3;    // can delete
   else if(x==1)   y=28;   // 0,5
   else if(x==2)   y=53;   // 1,0
   else if(x==3)   y=78;   // 1,5
   else if(x==4)   y=104;  // 2,0
   else if(x==5)   y=129;  // 2,5
   else if(x==6)   y=154;  // 3,0
   else if(x==7)   y=179;  // 3,5
   else if(x==8)   y=205;  // 4,0
   else if(x==9)   y=230;  // 4,5
   else if(x==10)  y=255;  // 5,0

  if(x2==-2)       y2=-2;  // dont search for
   else if(x2==-1) y2=0;   // not rated
   else if(x2==0)  y2=3;   // can delete
   else if(x2==1)  y2=28;  // 0,5
   else if(x2==2)  y2=53;  // 1,0
   else if(x2==3)  y2=78;  // 1,5
   else if(x2==4)  y2=104; // 2,0
   else if(x2==5)  y2=129; // 2,5
   else if(x2==6)  y2=154; // 3,0
   else if(x2==7)  y2=179; // 3,5
   else if(x2==8)  y2=205; // 4,0
   else if(x2==9)  y2=230; // 4,5
   else if(x2==10) y2=255; // 5,0

     
  if( (fs=fopen(Datei.c_str() ,"w")) ) {
    if(!(Artist.length()==0)){
      for(int i=0 ; i<CACHELINES ; i++) {
        cCacheData *dat=FirstEntry(i);
          while(dat) {
            index++;
            if(dat->Artist && strcasestr(dat->Artist,szArtist)) { 
	      count++;
              fprintf(fs,"%s;%s;%s;%d;%d;%s;%d;%d\n",dat->Filename,dat->Artist,dat->Title,dat->Bitrate,dat->Total,dat->Genre,dat->Year,dat->Rating);
            }
            dat=(cCacheData *)dat->Next();
          }
       }
    }
    else {
      for(int i=0 ; i<CACHELINES ; i++) {
        cCacheData *dat=FirstEntry(i);
          while(dat) {
            count++;
	    index++;
            fprintf(fs,"%s;%s;%s;%d;%d;%s;%d;%d\n",dat->Filename,dat->Artist,dat->Title,dat->Bitrate,dat->Total,dat->Genre,dat->Year,dat->Rating);
            dat=(cCacheData *)dat->Next();
          }
       }
    }

  fclose(fs);
  }


 char line[1024];
 const char* delim=";";
 char* word = NULL;
 int i = 0;

// Check for Title
  if(!(Title.length()==0)) {
    if( (fs=fopen(Datei.c_str(), "r")) && (ft=fopen(Outfile.c_str(), "w")) ) {
      count=0;
      while( fgets(line, sizeof(line), fs) !=NULL ) {
        word = strtok(line ,delim);
	for(i=0; word!=NULL; i++) {
	  if(i==0) fullfile=word;
	   else if(i==1) artist=word;
	    else if(i==2) title=word;
	     else if(i==3) bitrate=word;
	      else if(i==4) total=word;
	       else if(i==5) genre=word;
	        else if(i==6) year=word;
		 else if(i==7) rating=word;     
          word = strtok(NULL,delim);
        }

        if(strcasestr(title,szTitle)) {
          count++;
          fprintf(ft,"%s;%s;%s;%d;%d;%s;%d;%d\n",fullfile,artist,title,atoi(bitrate),atoi(total),genre,atoi(year),atoi(rating));
	}  

      }      
      fclose(ft);
      fclose(fs);
    }  
  }    
  else {       
    if( (fs = fopen(Datei.c_str(),"r")) && (ft=fopen(Outfile.c_str(),"w")) ) {
      count=0;
      while( fgets(line, sizeof(line),fs) != NULL ) {
        count++;
	fprintf(ft,line);
      }
      fclose(ft);
      fclose(fs);	
    }
  }





// Check for Genre
  if(!(Genre.length()==0)) {
    if( (fs=fopen(Outfile.c_str(), "r")) && (ft=fopen(Datei.c_str(), "w")) ) {
      count=0;
      while( fgets(line, sizeof(line), fs) !=NULL ) {
        word = strtok(line ,delim);
	for(i=0; word!=NULL; i++) {
	  if(i==0) fullfile=word;
	   else if(i==1) artist=word;
	    else if(i==2) title=word;
	     else if(i==3) bitrate=word;
	      else if(i==4) total=word;
	       else if(i==5) genre=word;
	        else if(i==6) year=word;
		 else if(i==7) rating=word;     
          word = strtok(NULL,delim);
        }

        if(strcasestr(genre,szGenre)) {
          count++;
          fprintf(ft,"%s;%s;%s;%d;%d;%s;%d;%d\n",fullfile,artist,title,atoi(bitrate),atoi(total),genre,atoi(year),atoi(rating));
	}  

      }      
      fclose(ft);
      fclose(fs);
    }  
  }    
  else {       
    if( (fs = fopen(Outfile.c_str(),"r")) && (ft=fopen(Datei.c_str(),"w")) ) {
      count=0;
      while( fgets(line, sizeof(line),fs) != NULL ) {
        count++;
	fprintf(ft,line);
      }
      fclose(ft);
      fclose(fs);	
    }
  }



// Check for Filename
  if(!(FullFile.length()==0)) {
    if( (fs=fopen(Datei.c_str(), "r")) && (ft=fopen(Outfile.c_str(), "w")) ) {
      count=0;
      while( fgets(line, sizeof(line), fs) !=NULL ) {
        word = strtok(line ,delim);
	for(i=0; word!=NULL; i++) {
	  if(i==0) fullfile=word;
	   else if(i==1) artist=word;
	    else if(i==2) title=word;
	     else if(i==3) bitrate=word;
	      else if(i==4) total=word;
	       else if(i==5) genre=word;
	        else if(i==6) year=word;
		 else if(i==7) rating=word;     
          word = strtok(NULL,delim);
        }

        if(strcasestr(fullfile,szFullFile)) {
          count++;
          fprintf(ft,"%s;%s;%s;%d;%d;%s;%d;%d\n",fullfile,artist,title,atoi(bitrate),atoi(total),genre,atoi(year),atoi(rating));
	}  

      }      
      fclose(ft);
      fclose(fs);
    }  
  }    
  else {       
    if( (fs = fopen(Datei.c_str(),"r")) && (ft=fopen(Outfile.c_str(),"w")) ) {
      count=0;
      while( fgets(line, sizeof(line),fs) != NULL ) {
        count++;
	fprintf(ft,line);
      }
      fclose(ft);
      fclose(fs);	
    }
  }




// Check for Year
  if(yearFromTo) {
    if( (fs=fopen(Outfile.c_str(), "r")) && (ft=fopen(Datei.c_str(), "w")) ) {
      count=0;
      while( fgets(line, sizeof(line), fs) !=NULL ) {
        word = strtok(line ,delim);
	for(i=0; word!=NULL; i++) {
	  if(i==0) fullfile=word;
	   else if(i==1) artist=word;
	    else if(i==2) title=word;
	     else if(i==3) bitrate=word;
	      else if(i==4) total=word;
	       else if(i==5) genre=word;
	        else if(i==6) year=word;
		 else if(i==7) rating=word;     
          word = strtok(NULL,delim);
        }

        if( (atoi(year)>= szYearFrom) && (atoi(year) <= szYearTo) ) {
          count++;
          fprintf(ft,"%s;%s;%s;%d;%d;%s;%d;%d\n",fullfile,artist,title,atoi(bitrate),atoi(total),genre,atoi(year),atoi(rating));
	}  

      }      
      fclose(ft);
      fclose(fs);
    }  
  }
  else if (szYearFrom > -1) {
    if( (fs=fopen(Outfile.c_str(), "r")) && (ft=fopen(Datei.c_str(), "w")) ) {
      count=0;
      while( fgets(line, sizeof(line), fs) !=NULL ) {
        word = strtok(line ,delim);
	for(i=0; word!=NULL; i++) {
	  if(i==0) fullfile=word;
	   else if(i==1) artist=word;
	    else if(i==2) title=word;
	     else if(i==3) bitrate=word;
	      else if(i==4) total=word;
	       else if(i==5) genre=word;
	        else if(i==6) year=word;
		 else if(i==7) rating=word;     
          word = strtok(NULL,delim);
        }

        if( atoi(year)>= szYearFrom ) {
          count++;
          fprintf(ft,"%s;%s;%s;%d;%d;%s;%d;%d\n",fullfile,artist,title,atoi(bitrate),atoi(total),genre,atoi(year),atoi(rating));
	}  

      }      
      fclose(ft);
      fclose(fs);
    }  
  }
  else if (szYearTo > -1) {
    if( (fs=fopen(Outfile.c_str(), "r")) && (ft=fopen(Datei.c_str(), "w")) ) {
      count=0;
      while( fgets(line, sizeof(line), fs) !=NULL ) {
        word = strtok(line ,delim);
	for(i=0; word!=NULL; i++) {
	  if(i==0) fullfile=word;
	   else if(i==1) artist=word;
	    else if(i==2) title=word;
	     else if(i==3) bitrate=word;
	      else if(i==4) total=word;
	       else if(i==5) genre=word;
	        else if(i==6) year=word;
		 else if(i==7) rating=word;     
          word = strtok(NULL,delim);
        }

        if( (atoi(year)<= szYearTo) && (atoi(year) > 1) ) {
          count++;
          fprintf(ft,"%s;%s;%s;%d;%d;%s;%d;%d\n",fullfile,artist,title,atoi(bitrate),atoi(total),genre,atoi(year),atoi(rating));
	}  

      }      
      fclose(ft);
      fclose(fs);
    }  
  }
  else if ((szYearFrom == -1) && (szYearTo == -1)) {
    if( (fs = fopen(Outfile.c_str(),"r")) && (ft=fopen(Datei.c_str(),"w")) ) {
      count=0;
      while( fgets(line, sizeof(line),fs) != NULL ) {
        count++;
	fprintf(ft,line);
      }
      fclose(ft);
      fclose(fs);	
    }
  }  



// y  = Real value of RatingFrom;
// y2 = Real value of RatingTo;

// Check for Rating
// between ratingFrom <-> ratingTo
  if(ratingFromTo) {
    isyslog("music: Search ratingFromTo");
    if( (fs=fopen(Datei.c_str(), "r")) && (ft=fopen(Outfile.c_str(), "w")) ) {
      count=0;
      while( fgets(line, sizeof(line), fs) !=NULL ) {
        word = strtok(line ,delim);
	for(i=0; word!=NULL; i++) {
	  if(i==0) fullfile=word;
	   else if(i==1) artist=word;
	    else if(i==2) title=word;
	     else if(i==3) bitrate=word;
	      else if(i==4) total=word;
	       else if(i==5) genre=word;
	        else if(i==6) year=word;
		 else if(i==7) rating=word;     
          word = strtok(NULL,delim);
        }

        if( (atoi(rating)>= y) && (atoi(rating) <= y2) ) {
          count++;
          fprintf(ft,"%s;%s;%s;%d;%d;%s;%d;%d\n",fullfile,artist,title,atoi(bitrate),atoi(total),genre,atoi(year),atoi(rating));
	}  

      }      
      fclose(ft);
      fclose(fs);
    }  
  }
// check ratingFrom
  else if (szRatingFrom > -2) {
    isyslog("music: Search ratingFrom '%d' '%d'",szRatingFrom, y );
    if( (fs=fopen(Datei.c_str(), "r")) && (ft=fopen(Outfile.c_str(), "w")) ) {
      count=0;
      while( fgets(line, sizeof(line), fs) !=NULL ) {
        word = strtok(line ,delim);
	for(i=0; word!=NULL; i++) {
	  if(i==0) fullfile=word;
	   else if(i==1) artist=word;
	    else if(i==2) title=word;
	     else if(i==3) bitrate=word;
	      else if(i==4) total=word;
	       else if(i==5) genre=word;
	        else if(i==6) year=word;
		 else if(i==7) rating=word;     
          word = strtok(NULL,delim);
        }

        if( (atoi(rating)>= y) ) {
          count++;
          fprintf(ft,"%s;%s;%s;%d;%d;%s;%d;%d\n",fullfile,artist,title,atoi(bitrate),atoi(total),genre,atoi(year),atoi(rating));
	}  

      }      
      fclose(ft);
      fclose(fs);
    }  
  }
// check ratingTo
  else if (szRatingTo > -2) {
    isyslog("music: Search ratingTo '%d' '%d'",szRatingTo, y2 );
    if( (fs=fopen(Datei.c_str(), "r")) && (ft=fopen(Outfile.c_str(), "w")) ) {
      count=0;
      while( fgets(line, sizeof(line), fs) !=NULL ) {
        word = strtok(line ,delim);
	for(i=0; word!=NULL; i++) {
	  if(i==0) fullfile=word;
	   else if(i==1) artist=word;
	    else if(i==2) title=word;
	     else if(i==3) bitrate=word;
	      else if(i==4) total=word;
	       else if(i==5) genre=word;
	        else if(i==6) year=word;
		 else if(i==7) rating=word;     
          word = strtok(NULL,delim);
        }

        if( (atoi(rating)<= y2) ) {
          count++;
          fprintf(ft,"%s;%s;%s;%d;%d;%s;%d;%d\n",fullfile,artist,title,atoi(bitrate),atoi(total),genre,atoi(year),atoi(rating));
	}  

      }      
      fclose(ft);
      fclose(fs);
    }  
  }
// dont check for rating
  else if ((szRatingFrom == -2) && (szRatingTo == -2)) {
    isyslog("music: Search not for rating '%d' '%d'",szRatingFrom, szRatingTo );
    if( (fs = fopen(Datei.c_str(),"r")) && (ft=fopen(Outfile.c_str(),"w")) ) {
      count=0;
      while( fgets(line, sizeof(line),fs) != NULL ) {
        count++;
	fprintf(ft,line);
      }
      fclose(ft);
      fclose(fs);	
    }
  }



// Check for Playable => @Suchergebnis.dat
  if(szPlayable) {
    if( (fs=fopen(Outfile.c_str(), "r")) && (ft=fopen(Datei.c_str(), "w")) ) {
      count=0;
      while( fgets(line, sizeof(line), fs) !=NULL ) {
        word = strtok(line ,delim);
	for(i=0; word!=NULL; i++) {
	  if(i==0) fullfile=word;
	   else if(i==1) artist=word;
	    else if(i==2) title=word;
	     else if(i==3) bitrate=word;
	      else if(i==4) total=word;
	       else if(i==5) genre=word;
	        else if(i==6) year=word;
		 else if(i==7) rating=word;     
          word = strtok(NULL,delim);
        }

        if( FullFileExist(fullfile) ) {
          count++;
          fprintf(ft,"%s;%s;%s;%d;%d;%s;%d;%d\n",fullfile,artist,title,atoi(bitrate),atoi(total),genre,atoi(year),atoi(rating));
	}  

      }      
      fclose(ft);
      fclose(fs);
    }  
  }
  else {
    if( (ft=fopen(Outfile.c_str(), "r")) && (fs=fopen(Datei.c_str(),"w")) ) {
      count=0;
      while(fgets(line,sizeof(line),ft) != NULL) {
        count++;
        fprintf(fs, line);
      }
      fclose(ft);
      fclose(fs);  
    }          
  }  

  if(count>0) searchresult=true;
  expr1= count;
  expr2= index;

  return searchresult;

}
