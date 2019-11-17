/** \file    common.h
  *
  * \brief   Common functions for simple tasks
  * \date    (C) 2009
  * \author  wolfgang astleitner (mrwastl@users.sourceforge.net)
  */

/*
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

/** \addtogroup SERDISP_TOOLS_COMMON

  \section INTRODUCTION Introduction
  Functions for simple tasks (eg: reading and writing uncompressed BMPs)

  * @{
  */

#ifndef SERDISP_TOOLS_COMMON_H
#define SERDISP_TOOLS_COMMON_H

#include "serdisplib/serdisp_control.h"
#include "serdisplib/serdisp_messages.h"


/* define 'byte' if not available yet */
#ifndef byte
  #define byte unsigned char
#endif

#ifndef BOOL
  #define BOOL int
#endif


#ifdef __cplusplus
extern "C" {
#endif
/** \name BMP functions
  */
/*!@{*/
  int   common_read_simplebmp  (char* bmpfile, byte* imgbuffer, int width, int height, int depth);
  int   common_write_simplebmp (char* bmpfile, byte* imgbuffer, int width, int height, int depth, int overwrite);
/*!@}*/

/** \name Firmware read/write functions
  */
/*!@{*/
  int   read_firmware_binary   (char* fwfile, byte* fw, size_t fwsize);
  int   read_firmware_intelhex (char* fwfile, byte* fw, size_t fwsize);
/*!@}*/
#ifdef __cplusplus
    }
#endif


#endif /* SERDISP_TOOLS_COMMON_H */
/*! @} */
