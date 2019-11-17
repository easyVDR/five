/*
 * tools.c: The 'soppalusikka' VDR skin
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "log.h"
#include "tools.h"

char *strcatrealloc(char *destP, const char *srcP)
{
  if (srcP) {
     size_t l = strlen(destP) + strlen(srcP) + 1;
     destP = (char *)realloc(destP, l);
     if (destP)
        strcat(destP, srcP);
     else
        error("%s Out of memory!", __PRETTY_FUNCTION__);
  }
  return destP;
}

char *strncatrealloc(char *destP, const char *srcP, size_t lenP)
{
  if (srcP) {
     size_t l = strlen(destP) + min(strlen(srcP), lenP) + 1;
     destP = (char *)realloc(destP, l);
     if (destP)
        strncat(destP, srcP, min(strlen(srcP), lenP));
     else
        error("%s Out of memory!", __PRETTY_FUNCTION__);
  }
  return destP;
}

static char *striptags(char *strP)
{
  char *r, *c;
  int t = 0, d = 0;
  bool s = false;
  r = strP;
  c = strP;
  while (*strP != '\0') {
    if (*strP == '<') {
       t++;
       s = true;
       }
    else if (*strP == '>') {
       t--;
       }
    else if (t < 1) {
       *(c++) = *strP;
       }
    else if (s) {
       if (*strP == '/') {
          d--;
          }
       else {
          d++;
          }
       if (d < 1) {
          *(c++) = '\n';
          }
       else if (d > 1) {
          *(c++) = ' ';
          }
       s = false;
       }
    strP++;
    }
  *c = '\0';
  return r;
}

#define AUX_HEADER_EPGSEARCH           "EPGSearch: "
#define AUX_TAGS_EPGSEARCH_START       "<epgsearch>"
#define AUX_TAGS_EPGSEARCH_ITEM1_START "<channel>"
#define AUX_TAGS_EPGSEARCH_ITEM1_END   "</channel>"
#define AUX_TAGS_EPGSEARCH_ITEM2_START "<searchtimer>"
#define AUX_TAGS_EPGSEARCH_ITEM2_END   "</searchtimer>"
#define AUX_TAGS_EPGSEARCH_ITEM3_START "<search timer>"
#define AUX_TAGS_EPGSEARCH_ITEM3_END   "</search timer>"
#define AUX_TAGS_EPGSEARCH_END         "</epgsearch>"

#define AUX_HEADER_VDRADMIN            "VDRAdmin: "
#define AUX_TAGS_VDRADMIN_START        "<vdradmin-am>"
#define AUX_TAGS_VDRADMIN_ITEM1_START  "<pattern>"
#define AUX_TAGS_VDRADMIN_ITEM1_END    "</pattern>"
#define AUX_TAGS_VDRADMIN_END          "</vdradmin-am>"

#define AUX_HEADER_PIN			"Protected: "
#define AUX_TAGS_PIN_START		"<pin-plugin>"
#define AUX_TAGS_PIN_ITEM1_START	"<protected>"
#define AUX_TAGS_PIN_ITEM1_END		"</protected>"
#define AUX_TAGS_PIN_END		"</pin-plugin>"

char *parseaux(char *auxP)
{
  char *start, *end;
  // check if egpsearch
  start = strcasestr(auxP, AUX_TAGS_EPGSEARCH_START);
  end = strcasestr(auxP, AUX_TAGS_EPGSEARCH_END);
  if (start && end) {
     char *tmp;
     // duplicate string
     char *s = strdup(auxP);
     char *r = s;
     size_t len = strlen(AUX_HEADER_EPGSEARCH);
     bool founditem = false;
     // add header
     strncpy(r, AUX_HEADER_EPGSEARCH, len);
     r += len;
     // parse first item
     len = strlen(AUX_TAGS_EPGSEARCH_ITEM1_START);
     if ((tmp = strcasestr(start, AUX_TAGS_EPGSEARCH_ITEM1_START)) != NULL) {
        if (tmp < end) {
            char *tmp2;
            if ((tmp2 = strcasestr(tmp, AUX_TAGS_EPGSEARCH_ITEM1_END)) != NULL) {
               // add channel
               strncpy(r, tmp + len, tmp2 - (tmp + len));
               r += (tmp2 - (tmp + len));
               founditem = true;
               }
            else {
               founditem = false;
               }
            }
        }
     // parse second item
     len = strlen(AUX_TAGS_EPGSEARCH_ITEM2_START);
     if ((tmp = strcasestr(start, AUX_TAGS_EPGSEARCH_ITEM2_START)) != NULL) {
        if (tmp < end) {
            char *tmp2;
            if ((tmp2 = strcasestr(tmp, AUX_TAGS_EPGSEARCH_ITEM2_END)) != NULL) {
               // add separator
               if (founditem) {
                  *(r++) = ',';
                  *(r++) = ' ';
                  }
               // add search item
               strncpy(r, tmp + len, tmp2 - (tmp + len));
               r += (tmp2 - (tmp + len));
               founditem = true;
               }
            else {
               founditem = false;
               }
            }
        }
     else {
        // parse third item
        len = strlen(AUX_TAGS_EPGSEARCH_ITEM3_START);
        if ((tmp = strcasestr(start, AUX_TAGS_EPGSEARCH_ITEM3_START)) != NULL) {
           if (tmp < end) {
               char *tmp2;
               if ((tmp2 = strcasestr(tmp, AUX_TAGS_EPGSEARCH_ITEM3_END)) != NULL) {
                  // add separator
                  if (founditem) {
                     *(r++) = ',';
                     *(r++) = ' ';
                     }
                  // add search item
                  strncpy(r, tmp + len, tmp2 - (tmp + len));
                  r += (tmp2 - (tmp + len));
                  founditem = true;
                  }
               else {
                  founditem = false;
                  }
               }
           }
        }
     // use old syntax
     if (!founditem) {
        len = strlen(AUX_HEADER_EPGSEARCH);
        strncpy(r, start + len, end - (start + len));
        r += (end - (start + len));
        }
     // copy duplicate string to aux
     *r = 0;
     strn0cpy(auxP, s, strlen(auxP));
     // free duplicated string
     free(s);
     // .. and return
     return auxP;
     }
  // check if vdradmin
  start = strcasestr(auxP, AUX_TAGS_VDRADMIN_START);
  end = strcasestr(auxP, AUX_TAGS_VDRADMIN_END);
  if (start && end) {
     char *tmp;
     // duplicate string
     char *s = strdup(auxP);
     char *r = s;
     size_t len = strlen(AUX_HEADER_VDRADMIN);
     // add header
     strncpy(r, AUX_HEADER_VDRADMIN, len);
     r += len;
     // parse first item
     len = strlen(AUX_TAGS_VDRADMIN_ITEM1_START);
     if ((tmp = strcasestr(start, AUX_TAGS_VDRADMIN_ITEM1_START)) != NULL) {
        if (tmp < end) {
            char *tmp2;
            if ((tmp2 = strcasestr(tmp, AUX_TAGS_VDRADMIN_ITEM1_END)) != NULL) {
               // add search item
               strncpy(r, tmp + len, tmp2 - (tmp + len));
               r += (tmp2 - (tmp + len));
               }
            }
        }
     // copy duplicate string to aux
     *r = 0;
     strn0cpy(auxP, s, strlen(auxP));
     // free duplicated string
     free(s);
     // .. and return
     return auxP;
     }
  // check if pin
  start = strcasestr(auxP, AUX_TAGS_PIN_START);
  end = strcasestr(auxP, AUX_TAGS_PIN_END);
  if (start && end) {
     char *tmp;
     // duplicate string
     char *s = strdup(auxP);
     char *r = s;
     size_t len = strlen(AUX_HEADER_PIN);
     // add header
     strncpy(r, AUX_HEADER_PIN, len);
     r += len;
     // parse first item
     len = strlen(AUX_TAGS_PIN_ITEM1_START);
     if ((tmp = strcasestr(start, AUX_TAGS_PIN_ITEM1_START)) != NULL) {
        if (tmp < end) {
            char *tmp2;
            if ((tmp2 = strcasestr(tmp, AUX_TAGS_PIN_ITEM1_END)) != NULL) {
               // add search item
               strncpy(r, tmp + len, tmp2 - (tmp + len));
               r += (tmp2 - (tmp + len));
               }
            }
        }
     // copy duplicate string to aux
     *r = 0;
     strn0cpy(auxP, s, strlen(auxP));
     // free duplicated string
     free(s);
     // .. and return
     return auxP;
     }
  // just strip tags
  return striptags(auxP);
}

bool ischaracters(const char *strP, const char *maskP)
{
  bool match = true;
  const char *p = strP;
  for (; *p; ++p) {
      const char *m = maskP;
      bool tmp = false;
      for (; *m; ++m) {
          if (*p == *m)
             tmp = true;
          }
      match = match && tmp;
      }
  return match;
}

bool ischaracter(const char chP, const char *maskP)
{
  bool match = false;
  const char *m = maskP;
  for (; *m; ++m) {
      if (chP == *m)
         match = true;
      }
  return match;
}
