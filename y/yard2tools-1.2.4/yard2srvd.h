/***************************************************************************
 *   Copyright (C) 2012 R. Geigenberger / Frank Kohlmann                   *
 *   Y.A.R.D.2 USB daemon                                                  *
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

#ifndef _YARD2SRVD_H
#define _YARD2SRVD_H

#include <fcntl.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <termios.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/select.h>

void cleanup_and_exit(int exitstate);
void StartDaemon(void);
void StartDaemon(void);
int init_yard(const char* yard_device);
void open_yard(void);
void signal_handler_yard( int status );
int accept_ir_clients(int sockfd);
int start_listen_socket(int id);
int start_ir_listen_socket(void);
int start_config_listen_socket(void);
int start_lcd_listen_socket(void);
void send_ir_data_to_lirc(unsigned char* buf, int len, int socket);
void send_data_to_config(unsigned char* buf, int len);
void send_data_to_yard(unsigned char* buf, int len);
void close_sockets(void);
void getirmapname(unsigned char *mapname, unsigned long long IR);
int getirkeyb(unsigned long long IR);
void send_key_event(int fd, unsigned int keycode, int keyvalue);
void UpdateRTCTime(void);




#endif
