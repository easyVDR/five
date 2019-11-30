/*
 * config.c:
 *
 * See the README file for copyright information and how to reach the author.
 *
 */

#include <syslog.h>

#include "config.h"

//***************************************************************************
// Statics
//***************************************************************************

int cConfigBase::logstdout = no;
char* cConfigBase::loglevelString = strdup("1:-1");
int cConfigBase::loglevel = 1;
int cConfigBase::logwidth = na;
int cConfigBase::argLoglevel = na;
int cConfigBase::logFacility = LOG_USER;
const char* cConfigBase::logName = "unknown";

//***************************************************************************
// Common Configuration
//***************************************************************************

cConfigBase::cConfigBase()
{
}

//***************************************************************************
// Set Log Level
//***************************************************************************

void cConfigBase::setLogLevel(const char* s)
{
   if (isEmpty(s))
      return ;

   free(loglevelString);
   loglevelString = strdup(s);

   loglevel = atoi(loglevelString);

   if (const char* w = strchr(loglevelString, ':'))
      logwidth = atoi(w+1);
}

//***************************************************************************
//***************************************************************************
// Common EPG Service Configuration
//***************************************************************************

cEpgConfig::cEpgConfig()
{
   // database connection

   sstrcpy(dbHost, "localhost", sizeof(dbHost));
   dbPort = 3306;
   sstrcpy(dbName, "epg2vdr", sizeof(dbName));
   sstrcpy(dbUser, "epg2vdr", sizeof(dbUser));
   sstrcpy(dbPass, "epg", sizeof(dbPass));

   sstrcpy(netDevice, getFirstInterface(), sizeof(netDevice));

   uuid[0] = 0;

   getepgimages = yes;
}

//***************************************************************************
// Has DB Login Changed
//***************************************************************************

int cEpgConfig::hasDbLoginChanged(cEpgConfig* old)
{
   if (old->dbPort != dbPort ||
       strcmp(old->dbHost, dbHost) != 0 ||
       strcmp(old->dbName, dbName) != 0 ||
       strcmp(old->dbUser, dbUser) != 0 ||
       strcmp(old->dbPass, dbPass) != 0)
   {
      return yes;
   }

   return no;
}
