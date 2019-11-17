

#include "global_config.h"
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "util.h"
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>     /* strtoull */
#include <sys/stat.h>
#include <syslog.h>

loglevel_t loglevel_current = LL_WARNING;
int logTargetSyslog = 0;

#define buf_size 1024


void clrscr(int n, int go) //0=clr to end; 1=clr to top; 2=clr all
{
	//printf("\033[2J\033[1;1H");
	printf("\033[%dJ",n);
	if(go==1) gotoxy(1,1);
}

void gotoxy(int x, int y)
{
	printf("\033[%d;%df",y,x);
}

void logtosyslog(const char *ident)
{
	// ident need to be a real static string
	static char id[buf_size];

	if (!logTargetSyslog)
	{
		strcpy(id, ident);
		setlogmask (LOG_UPTO (LOG_DEBUG));
		openlog (id, LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
		logTargetSyslog = 1;
	}
}

void logclosesyslog()
{
	if (logTargetSyslog)
	{
		logTargetSyslog = 0;
		closelog();
	}
}

/*
 * helper functions
 */
void printhex(const unsigned char* c, int len) {
  int i=0;
  while (i<len) {
    printf( "%02X ", (unsigned char) c[i] ); i++;
  }
  fflush(stdout);
}

static int write_logging(loglevel_t loglevel, char *buffer, int len )
{
	if (logTargetSyslog)
	{
		int ll2facility[LL_MAX] = {
			0				// LL_OFF
		,	LOG_ERR			// LL_ERROR
		,	LOG_WARNING		// LL_WARNING
		,	LOG_NOTICE		// LL_INFO
		,	LOG_INFO		// LL_VERBOSE
		,	LOG_DEBUG		// LL_DEBUG
		};

		syslog(ll2facility[loglevel], "%s", buffer);
		return len;
	}
	else
	{
		int fd = loglevel > LOG_WARNING ? STDOUT_FILENO : STDERR_FILENO;
		return write( fd, buffer, len);
	}
}

int do_logging(loglevel_t loglevel, const char *format, va_list args)
{
	static char buffer[buf_size];
	int len = vsnprintf( buffer, buf_size, format, args);
	buffer[ buf_size-1 ] = '\0';

	return write_logging(loglevel, buffer, len);
}

int loghex(loglevel_t loglevel, const unsigned char* c, int len)
{
	static char buffer[buf_size];
	char *pprint = buffer;
	int lenght=0;
	int i=0;

	if (!check_loglevel(loglevel)) return 0;

	while (i<len)
	{
		sprintf( pprint, "%02X ", c[i] );
		pprint += 3;
		lenght += 3;
		i++;
		if (lenght >= buf_size) break;
	}

	return write_logging(loglevel, buffer, lenght);
}

void waitfor(int waitcount, int sd) {
  printf("waiting: ");
  
  while (waitcount>0) {
    sleep(1);
    printf(" %d",waitcount);
    fflush( stdout );
    waitcount--;
  }
  printf("\n");
  printf("socket fd: %d \n",sd);
}

unsigned long long GetTickCount(void)
{
  struct timespec now;
  if (clock_gettime(CLOCK_MONOTONIC, &now))
    return 0;
  return (now.tv_sec * 1000.0 + now.tv_nsec / 1000000);
}

unsigned char decToBcd(int val)
{
  return ( (val/10*16) + (val%10) );
}

unsigned char bcdToDec(int val)
{
  return ( (val/16*10) + (val%16) );
}

int file_exist (char *filename)
{
  struct stat   buffer;   
  return (stat (filename, &buffer) == 0);
}

int GetNextFreeEvent(void)
{
int i;
char nextevent[50];
	

  i = 0;
  sprintf(nextevent,"/dev/input/event%d",i);
  while(file_exist (nextevent))
  {
#ifdef DEBUG
    printf ("Event %s exists\n",nextevent);
#endif
    i++;
    sprintf(nextevent,"/dev/input/event%d",i);
   }
 return i++;
}

int get_line(FILE *fp, char *buffer, size_t buflen)
{
    char *end = buffer + buflen - 1; /* Allow space for null terminator */
    char *dst = buffer;
    int c;
    while ((c = getc(fp)) != EOF && c != '\n' && dst < end)
        *dst++ = c;
    *dst = '\0';
    return((c == EOF && dst == buffer) ? EOF : dst - buffer);
}

void readirmap(char *mapfile, struct irmap *mymap)//[MAX_IRMAP_SIZE])
{
int i;
FILE *fh;
char delimiter[] = "@";
char line[255];
char *ptr;
  
#ifdef DEBUG
  printf("Open IRMAP file %s\n",mapfile);
#endif
  if ( ( fh = fopen(mapfile, "r") ) == NULL )
  {
    printf("Error open irmap file %s !\n", mapfile);
    return;
  }

  i = 0; //reset line marker
  while (get_line(fh,line,80) > 0)
  {
	//printf("READ Line %s\n",line);

	//IR Code
	ptr = strtok(line, delimiter);
	if(ptr != NULL)
	{
		strcpy(mymap[i].IRcode_str,ptr); //copy to myirmap
		mymap[i].IRcode = (unsigned long long)strtoull(mymap[i].IRcode_str, NULL, 16); //convert to 64bit value
	}

	//BTNname Code
	ptr = strtok(NULL, delimiter);
	if(ptr != NULL)
	{
		strcpy(mymap[i].btn_name,ptr);  //copy to myirmap
	}

	//keyB Code
	ptr = strtok(NULL, delimiter);
	if(ptr != NULL)
	{
		mymap[i].keybord_map = (unsigned int)strtol(ptr, NULL, 10);
	}

	logging(LL_DEBUG, "IR%d: %s - %lld -> Btn: %s -> KeyID: %d\n",i,mymap[i].IRcode_str,mymap[i].IRcode,mymap[i].btn_name,mymap[i].keybord_map);

    i++;
  } // while

  fclose(fh);
  return;
}
