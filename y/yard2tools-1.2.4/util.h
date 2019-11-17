
#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdio.h>
#include <stdarg.h>
#include "global_config.h"

#define CHECK_BIT(var, pos)	((var) & (1 << (pos)))
#define SET_BIT(var, pos)   ((var) |= (1 << (pos)))
#define CLR_BIT(var, pos) 	((var) &= ~(1 << (pos)))

void gotoxy(int x, int y);
void clrscr(int n, int go);
void printhex(const unsigned char* c, int len);
void waitfor(int waitcount, int sd);
unsigned long long GetTickCount(void);
unsigned char decToBcd(int val);
unsigned char bcdToDec(int val);
int get_line(FILE *fp, char *buffer, size_t buflen);
int GetNextFreeEvent(void);
void readirmap(char *mapfile, struct irmap *mymap);//[MAX_IRMAP_SIZE]);

typedef enum loglevel_e
{
	LL_OFF = 0
,	LL_ALWAYS = LL_OFF
,	LL_ERROR
,	LL_WARNING
,	LL_INFO
,	LL_VERBOSE
, 	LL_DEBUG
, 	LL_MAX
} loglevel_t;
static int logging(loglevel_t loglevel, const char *format, ...) __attribute__ ((format (printf, 2, 3)));
extern loglevel_t loglevel_current;

extern int do_logging(loglevel_t loglevel, const char *format, va_list args);
extern int loghex(loglevel_t loglevel, const unsigned char* c, int len);

extern void logtosyslog(const char *ident);
extern void logclosesyslog();

// return true if logging shall be done
static inline int check_loglevel(loglevel_t loglevel)
{
	return (loglevel <= loglevel_current);
}

static inline int logging(loglevel_t loglevel, const char *format, ...)
{
	va_list args;

	if (!check_loglevel(loglevel)) return 0;

 	va_start( args, format );
	return do_logging(loglevel, format, args);
}


#endif
