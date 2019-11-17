#include <string>
#include <langinfo.h>
#include <vdr/plugin.h>
#include "icons.h"
#include "config.h"

// --- Icons ------------------------------------------------------------------
bool Icons::IsUTF8=false;

void Icons::InitCharSet()
{
  // Taken from VDR's vdr.c
  char *CodeSet=NULL;
  if(setlocale(LC_CTYPE, ""))
    CodeSet=nl_langinfo(CODESET);
  else
  {
    char *LangEnv=getenv("LANG"); // last resort in case locale stuff isn't installed
    if(LangEnv)
    {
      CodeSet=strchr(LangEnv,'.');
      if(CodeSet)
        CodeSet++; // skip the dot
    }
  }

  if(CodeSet && strcasestr(CodeSet,"UTF-8")!=0) {
    IsUTF8=true;
#ifdef DEBUG
    printf("music-sd: CodeSet = UTF-8\n");
#endif
    }
  else {
    IsUTF8=false;
#ifdef DEBUG
    printf("music-sd: Non UTF-8 system\n");
#endif
  }    
}
