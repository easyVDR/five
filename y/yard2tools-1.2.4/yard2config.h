/***************************************************************************
 *   Copyright (C) 2012 by R. Geigenberger                                 *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef _YARDCONFIG_H
#define _YARDCONFIG_H

#include <time.h>

void printmainmenu(void);
void printLCDmenu(void);
void printLEDmenu(void);
void printSettingsmenu(void);
void SaveYardConfig(void);
void ResetYARD(void);
void WriteRTCTime(unsigned char adr, time_t newtime);
void WriteRTCbyte(unsigned char adr, unsigned char val);
void WriteEEPROMbyte(unsigned char adr, unsigned char val);
void ReadEEPROM(unsigned char adr, unsigned char len);
void ReadLS(void);
int Readinput(unsigned char maxvalue);
void cleanup_and_exit(int exitstate);
void cleanup_and_exit(int exitstate);
void printsettings(unsigned char buffer);
void yardconfigmenu(void);
int yardconfig_init(void);
int yardconfig_deinit(void);
char *yardconfig_rec(int i_int);
#endif
