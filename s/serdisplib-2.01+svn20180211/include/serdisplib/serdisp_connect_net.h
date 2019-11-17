/*
 *************************************************************************
 *
 * serdisp_connect_net.h
 * routines for accessing displays over a network-based protocol
 *
 *************************************************************************
 *
 * copyright (C) 2011-2018  wolfgang astleitner
 * email     mrwastl@users.sourceforge.net
 *
 *************************************************************************
 * This program is free software; you can redistribute it and/or modify   
 * it under the terms of the GNU General Public License as published by   
 * the Free Software Foundation; either version 2 of the License, or (at  
 * your option) any later version.                                        
 *                                                                        
 * This program is distributed in the hope that it will be useful, but    
 * WITHOUT ANY WARRANTY; without even the implied warranty of             
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      
 * General Public License for more details.                               
 *                                                                        
 * You should have received a copy of the GNU General Public License      
 * along with this program; if not, write to the Free Software            
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA              
 * 02111-1307, USA.  Or, point your browser to                            
 * http://www.gnu.org/copyleft/gpl.html                                   
 *************************************************************************
 */


#ifndef SERDISP_CONNECT_NET_H
#define SERDISP_CONNECT_NET_H

#define SD_NET_READBUF_SIZE    32
#define SD_NET_WRITEBUF_SIZE   256

#define SD_NET_DEFPORT         9876

#if 0
typedef struct {
  int                  fd;
  byte                 readbuf[SD_NET_READBUF_SIZE];
  byte                 writebuf[SD_NET_WRITEBUF_SIZE];
} serdisp_netdev_t;
#endif


#ifdef __cplusplus
extern "C" {
#endif
  serdisp_CONN_t* SDCONNnet_open           (const char sdcdev[]);
  void            SDCONNnet_close          (serdisp_CONN_t* sdcd);
//  void            SDCONNnet_write          (serdisp_CONN_t* sdcd, uint32_t data, byte flags);
//  void            SDCONNnet_writedelay     (serdisp_CONN_t* sdcd, uint32_t data, byte flags, long ns);
//  uint32_t        SDCONNnet_read           (serdisp_CONN_t* sdcd, byte flags);
//  int             SDCONNnet_readstream     (serdisp_CONN_t* sdcd, byte* buf, int count);
//  void            SDCONNnet_commit         (serdisp_CONN_t* sdcd);
#ifdef __cplusplus
    }
#endif




#endif /* SERDISP_CONNECT_NET_H */
