/** \file    serdisp_colour.c
  *
  * \brief   Colour and colour space specific functions
  * \date    (C) 2003-2012
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

#include "serdisplib/serdisp_control.h"
#include "serdisplib/serdisp_colour.h"

#include <string.h>   /* needed for memset (area-functions) */

/* internal functions */
static SDCol    serdisp_transsdcol_bw       (serdisp_t* dd, SDCol sdcol);
static SDCol    serdisp_transsdgrey_bw      (serdisp_t* dd, byte greyvalue);
static SDCol    serdisp_lookupsdcol_bw      (serdisp_t* dd, SDCol sdcol);
static byte     serdisp_lookupsdgrey_bw     (serdisp_t* dd, SDCol sdcol);

static SDCol    serdisp_transsdcol_grey2_4  (serdisp_t* dd, SDCol sdcol);
static SDCol    serdisp_transsdgrey_grey2_4 (serdisp_t* dd, byte greyvalue);
static SDCol    serdisp_lookupsdcol_grey2_4 (serdisp_t* dd, SDCol sdcol);
static byte     serdisp_lookupsdgrey_grey2_4(serdisp_t* dd, SDCol sdcol);

static SDCol    serdisp_transsdcol_grey8    (serdisp_t* dd, SDCol sdcol);
static SDCol    serdisp_transsdgrey_grey8   (serdisp_t* dd, byte greyvalue);
static SDCol    serdisp_lookupsdcol_grey8   (serdisp_t* dd, SDCol sdcol);
static byte     serdisp_lookupsdgrey_grey8  (serdisp_t* dd, SDCol sdcol);

static SDCol    serdisp_transsdcol_rgb332   (serdisp_t* dd, SDCol sdcol);
static SDCol    serdisp_transsdgrey_rgb332  (serdisp_t* dd, byte greyvalue);
static SDCol    serdisp_lookupsdcol_rgb332  (serdisp_t* dd, SDCol sdcol);
static byte     serdisp_lookupsdgrey_rgb332 (serdisp_t* dd, SDCol sdcol);

static SDCol    serdisp_transsdcol_bgr332   (serdisp_t* dd, SDCol sdcol);
static SDCol    serdisp_transsdgrey_bgr332  (serdisp_t* dd, byte greyvalue);
static SDCol    serdisp_lookupsdcol_bgr332  (serdisp_t* dd, SDCol sdcol);
static byte     serdisp_lookupsdgrey_bgr332 (serdisp_t* dd, SDCol sdcol);

static SDCol    serdisp_transsdcol_rgb444   (serdisp_t* dd, SDCol sdcol);
static SDCol    serdisp_transsdgrey_rgb444  (serdisp_t* dd, byte greyvalue);
static SDCol    serdisp_lookupsdcol_rgb444  (serdisp_t* dd, SDCol sdcol);
static byte     serdisp_lookupsdgrey_rgb444 (serdisp_t* dd, SDCol sdcol);

static SDCol    serdisp_transsdcol_rgb565   (serdisp_t* dd, SDCol sdcol);
static SDCol    serdisp_transsdgrey_rgb565  (serdisp_t* dd, byte greyvalue);
static SDCol    serdisp_lookupsdcol_rgb565  (serdisp_t* dd, SDCol sdcol);
static byte     serdisp_lookupsdgrey_rgb565 (serdisp_t* dd, SDCol sdcol);

static SDCol    serdisp_transsdcol_bgr565   (serdisp_t* dd, SDCol sdcol);
static SDCol    serdisp_transsdgrey_bgr565  (serdisp_t* dd, byte greyvalue);
static SDCol    serdisp_lookupsdcol_bgr565  (serdisp_t* dd, SDCol sdcol);
static byte     serdisp_lookupsdgrey_bgr565 (serdisp_t* dd, SDCol sdcol);

static SDCol    serdisp_transsdcol_rgb666   (serdisp_t* dd, SDCol sdcol);
static SDCol    serdisp_transsdgrey_rgb666  (serdisp_t* dd, byte greyvalue);
static SDCol    serdisp_lookupsdcol_rgb666  (serdisp_t* dd, SDCol sdcol);
static byte     serdisp_lookupsdgrey_rgb666 (serdisp_t* dd, SDCol sdcol);

static SDCol    serdisp_transsdcol_rgb888   (serdisp_t* dd, SDCol sdcol);
static SDCol    serdisp_transsdgrey_rgb888  (serdisp_t* dd, byte greyvalue);
static SDCol    serdisp_lookupsdcol_rgb888  (serdisp_t* dd, SDCol sdcol);
static byte     serdisp_lookupsdgrey_rgb888 (serdisp_t* dd, SDCol sdcol);

static SDCol    serdisp_transsdcol_rgba888  (serdisp_t* dd, SDCol sdcol);
static SDCol    serdisp_transsdgrey_rgba888 (serdisp_t* dd, byte greyvalue);
static SDCol    serdisp_lookupsdcol_rgba888 (serdisp_t* dd, SDCol sdcol);
static byte     serdisp_lookupsdgrey_rgba888(serdisp_t* dd, SDCol sdcol);

static uint32_t serdisp_colourdistance      (SDCol col1, SDCol col2);


/**
  * \brief   set a colour in the display buffer
  *
  * This function sets the colour information <em>hardware independently</em>.\n
  * The information is changed in the <em>display buffer</em> only.
  *
  * \param   dd            device descriptor
  * \param   x             x-position
  * \param   y             y-position
  * \param   sdcol         colour \n
  *                        format: \p 0xAARRGGBB , \p AA .. alpha, \p RR .. red, \p GG .. green, \p BB .. blue
  *
  * \b Examples: \n
  * both set a red pixel in the display buffer at position \p (10/20)
  * \code
  * serdisp_setsdcol(dd, 10, 20, 0xFFFF0000); 
  * serdisp_setsdcol(dd, 10, 20, SD_COL_RED); 
  * \endcode
  *
  * \since  1.98.0
  */
void serdisp_setsdcol(serdisp_t* dd, int x, int y, SDCol sdcol) {
  serdisp_setsdpixel(dd, x, y, serdisp_transsdcol(dd, sdcol));
}


/**
  * \brief   set grey value in the display buffer
  *
  * This function sets the grey value <em>hardware independently</em>.\n
  * The information is changed in the <em>display buffer</em> only.
  *
  * \param   dd            device descriptor
  * \param   x             x-position
  * \param   y             y-position
  * \param   grey          grey value; format: <tt>[0 .. 255]</tt>
  *
  * \b Examples: \n
  * set a grey value in the display buffer at position \p (10/20)
  * \code
  * serdisp_setgrey(dd, 10, 20, 0xCC); 
  * \endcode
  *
  * \since 1.98.0
  */
void serdisp_setsdgrey(serdisp_t* dd, int x, int y, byte grey) {
  serdisp_setsdpixel(dd, x, y, serdisp_transsdgrey(dd, grey));
}


/**
  * \brief   gets colour value at position (x/y)
  *
  * Gets the <em>hardware independent</em> colour value at position (x/y)
  *
  * \param   dd            device descriptor
  * \param   x             x-position
  * \param   y             y-position
  *
  * \return hardware independent colour value at (x/y) \n
  *         format: <tt>0xAARRGGBB</tt>, \p AA .. alpha, \p RR .. red, \p GG .. green, \p BB .. blue
  *
  * \since 1.98.0
  */
SDCol serdisp_getsdcol(serdisp_t* dd, int x, int y) {
  return serdisp_lookupsdcol(dd, serdisp_getsdpixel(dd, x, y));
}


/**
  * \brief   gets grey value at position (x/y)
  *
  * Gets the grey value at position (x/y). colour values are converted to the corresponding greyscale values.
  *
  * \param   dd            device descriptor
  * \param   x             x-position
  * \param   y             y-position
  *
  * \return grey value ,format: <tt>[0 .. 255]</tt>
  *
  * \since 1.98.0
  */
byte serdisp_getsdgrey(serdisp_t* dd, int x, int y) {
  return serdisp_lookupsdgrey(dd, serdisp_getsdpixel(dd, x, y));
}


/**
  * \brief   copies an external buffer into the screen buffer
  *
  * \param   dd            device descriptor
  * \param   x             x start position
  * \param   y             y start position
  * \param   w             width
  * \param   h             height
  * \param   inpmode       (inpmode & 0x00FF): input depth (either 24 or 32 bit) \n
                           (inpmode & 0xFF00): flags:\n
                           bit 0: \n \li 0: RGB \li 1: BGR
  * \param   content       image data to fill into given area
  *
  * \retval   0            no error occured
  * \retval  -1            inpmode not supported
  * \retval  -2            not initialised or erraneous parameters
  * \retval  -3            position / dimension parameters out of bounds
  *
  * \b Example: \n
  * copy the image data from \<content\> into the screen buffer of \<dd\> at position 10/10, dimension 50x50.
  * Its depth is 32 and colour scheme is RGB.\n
  * The dimension of the image stored in \<content\> must match parameters w and h!
  * \code
  * byte* content =  <some image byte stream>;
  * serdisp_setarea(dd, 10, 10, 50, 50, 32, content);
  * \endcode
  *
  * \since 1.97.9
  */
int serdisp_setarea (serdisp_t* dd, int x, int y, int w, int h, int inpmode, byte* content) {
  return serdisp_cliparea (dd, x, y, w, h, 0, 0, w, h, inpmode, content);
}

/**
  * \brief   copies a clip from an external buffer into the screen buffer
  *
  * \param   dd            device descriptor
  * \param   x             x start position in display buffer
  * \param   y             y start position in display buffer
  * \param   w             width of clip section to copy
  * \param   h             height of clip section to copy
  * \param   sx            x start position in source clip
  * \param   sy            y start position in source clip
  * \param   cw            total width of source clip
  * \param   ch            total height of source clip
  * \param   inpmode       (inpmode & 0x00FF): input depth (either 24 or 32 bit) \n
                           (inpmode & 0xFF00): flags:\n
                           bit 0: \n \li 0: RGB \li 1: BGR
  * \param   content       image data to fill into given area
  *
  * \retval   0            no error occured
  * \retval  -1            inpmode not supported
  * \retval  -2            not initialised or erraneous parameters
  * \retval  -3            position / dimension parameters out of bounds
  *
  * \b Example: \n
  * copy a clip from \<content\> starting at position 50/50, size 100x100 into the screen buffer of \<dd\> at position 10/10.
  * the dimension of the image stored in \<content\> is 200x200 (altogether), its depth is 24 and colour scheme is BGR.
  * \code
  * byte* content =  <some image byte stream>;
  * serdisp_cliparea(dd, 10, 10, 100, 100, 50, 50, 200, 200, (1 << 8) | 24, content);
  * \endcode
  *
  * \since 1.97.9
  */
int serdisp_cliparea (serdisp_t* dd, int x, int y, int w, int h, int sx, int sy, int cw, int ch,
                      int inpmode, byte* content) {
  int i, j;
  int depth = serdisp_getdepth(dd);

  int inpdepth = (inpmode & 0x00FF);
  int inpflags = (inpmode & 0xFF00) >> 8;
  int inpbytespercol = (inpdepth == 24) ? 3 : 4;

  if (!dd)
    return -2;

  if (x < 0) {
    sx -= x;
    x = 0;
  }

  if (y < 0) {
    sy -= y;
    y = 0;
  }

  if ( (x + w) > serdisp_getwidth(dd) ) {
    w = serdisp_getwidth(dd) - x;
  }

  if ( (y + h) > serdisp_getheight(dd) ) {
    h = serdisp_getheight(dd) - y;
  }

  if ( ! (inpdepth == 24 || inpdepth == 32) )
    return -1;

  /* fall back to pixel-wise copying if:
     * relocation table involved
     * depth < 24
     * current rotation other than 0 or 180 degrees
   */
  if (dd->xreloctab || dd->yreloctab || depth < 24 || (dd->curr_rotate & 0x02)) {
    int shiftareay = sy * cw * inpbytespercol;
    int shiftareax = sx * inpbytespercol;
    int shiftareaendx = (cw - sx - w) * inpbytespercol;
    int areaidx = shiftareay + shiftareax;

    for (j = y; j < y+h; j++) {
      for (i = x; i < x+w ; i++) {
        SDCol sdcol;

        if (inpdepth == 32) {
          sdcol = (content[areaidx] << 24) | (content[areaidx+1] << 16)  | (content[areaidx+2] << 8) | content[areaidx+3];
          areaidx += 4;
        } else {
          sdcol = (0xFF << 24) | (content[areaidx] << 16)  | (content[areaidx+1] << 8) | content[areaidx+2];
          areaidx += 3;
        }
        serdisp_setsdcol(dd, i, j, sdcol);
      }
      areaidx += shiftareaendx;
    }
  } else {
    byte* scrbuf_chg = dd->scrbuf_chg;
    int bytespercol = (depth == 24) ? 3 : 4;

    int shiftareay = sy * cw * inpbytespercol;
    int shiftareax = sx * inpbytespercol;
    int shiftareaendx = (cw - sx - w) * inpbytespercol;
    int areaidx = shiftareay + shiftareax;

    int rotate = dd->curr_rotate;
    int shiftbufendx = (serdisp_getwidth(dd) - x - w) * bytespercol;
    int bufidx = 0;

    dd->scrbuf_chg = 0;
    byte* scrbuf = dd->scrbuf;

    if (rotate) {
      bufidx = (y + h) * serdisp_getwidth(dd) * bytespercol - shiftbufendx - bytespercol;
    } else {
      bufidx = ( y * serdisp_getwidth(dd) + x ) * bytespercol;
    }

    for (j = 0; j < h; j++) {
      for (i = 0; i < w; i++) {
        if (inpflags & 0x01) { /* BGR */
          if (depth == inpdepth) {
            if (depth == 32) {
              scrbuf[bufidx++] = content[areaidx++];
              scrbuf[bufidx+2] = content[areaidx++];
              scrbuf[bufidx+1] = content[areaidx++];
              scrbuf[bufidx  ] = content[areaidx++];
            } else {
              scrbuf[bufidx+2] = content[areaidx++];
              scrbuf[bufidx+1] = content[areaidx++];
              scrbuf[bufidx  ] = content[areaidx++];
            }
            bufidx += 3;
          } else {
            if (depth == 32) {
              scrbuf[bufidx++] = 0xFF;
            } else {
              areaidx++;
            }
            scrbuf[bufidx+2] = content[areaidx++];
            scrbuf[bufidx+1] = content[areaidx++];
            scrbuf[bufidx  ] = content[areaidx++];
            bufidx += 3;
          }
        } else { /* RGB */
          if (depth == inpdepth) {
            scrbuf[bufidx++] = content[areaidx++];
            scrbuf[bufidx++] = content[areaidx++];
            scrbuf[bufidx++] = content[areaidx++];
            if (depth == 32) {
              scrbuf[bufidx++] = content[areaidx++];
            }
          } else {
            if (depth == 32) {
              scrbuf[bufidx++] = 0xFF;
            } else {
              areaidx++;
            }
            scrbuf[bufidx++] = content[areaidx++];
            scrbuf[bufidx++] = content[areaidx++];
            scrbuf[bufidx++] = content[areaidx++];
          }
        }
        if (rotate) {
          bufidx -= (bytespercol + bytespercol);
        }
      }

      areaidx += shiftareaendx;
      if (rotate) {
        bufidx -= bytespercol - shiftbufendx - bytespercol;
      } else {
        bufidx += shiftbufendx;
      }
    }

    /* bounding box (rotation doesn't matter here) */
    dd->bbox_dirty = 1;
    dd->bbox[0] = x;
    dd->bbox[1] = y;
    dd->bbox[2] = x + w;
    dd->bbox[3] = y + h;

    dd->scrbuf_chg = scrbuf_chg;
    if (dd->scrbuf_chg) {
      int colsize = (serdisp_getwidth(dd) + 7) >> 3;
      int shiftbufx = (x >> 3);
      int bufidx = y * colsize + shiftbufx;
      int chunk = (w + 7 ) >> 3;

      for (j = 0; j < h; j++) {
        memset(scrbuf_chg+bufidx, 0xFF, chunk);
        bufidx += colsize;
      }
    }
  }
  return 0;
}


/**
  * \brief   initialises colour conversion function pointers
  *
  * \param   dd            device descriptor
  *
  * \retval   0            no error occured
  * \retval  -1            depth/colour-space combination not supported
  *
  * \since 1.97.9
  */
int serdisp_sdcol_init (serdisp_t* dd) {
  switch (serdisp_getdepth(dd)) {
    case 1:
      dd->fp_transsdcol   = serdisp_transsdcol_bw;
      dd->fp_transsdgrey  = serdisp_transsdgrey_bw;
      dd->fp_lookupsdcol  = serdisp_lookupsdcol_bw;
      dd->fp_lookupsdgrey = serdisp_lookupsdgrey_bw;
      break;
    case 2:
    case 4:
      dd->fp_transsdcol   = serdisp_transsdcol_grey2_4;
      dd->fp_transsdgrey  = serdisp_transsdgrey_grey2_4;
      dd->fp_lookupsdcol  = serdisp_lookupsdcol_grey2_4;
      dd->fp_lookupsdgrey = serdisp_lookupsdgrey_grey2_4;
      break;
    case 8:
      if (dd->colour_spaces & SD_CS_RGB332) {
        if (SD_CS_ISBGR(dd)) {
          dd->fp_transsdcol   = serdisp_transsdcol_bgr332;
          dd->fp_transsdgrey  = serdisp_transsdgrey_bgr332;
          dd->fp_lookupsdcol  = serdisp_lookupsdcol_bgr332;
          dd->fp_lookupsdgrey = serdisp_lookupsdgrey_bgr332;
        } else {
          dd->fp_transsdcol   = serdisp_transsdcol_rgb332;
          dd->fp_transsdgrey  = serdisp_transsdgrey_rgb332;
          dd->fp_lookupsdcol  = serdisp_lookupsdcol_rgb332;
          dd->fp_lookupsdgrey = serdisp_lookupsdgrey_rgb332;
        }
      } else {
        dd->fp_transsdcol   = serdisp_transsdcol_grey8;
        dd->fp_transsdgrey  = serdisp_transsdgrey_grey8;
        dd->fp_lookupsdcol  = serdisp_lookupsdcol_grey8;
        dd->fp_lookupsdgrey = serdisp_lookupsdgrey_grey8;
      }
      break;
    case 12:
      if (SD_CS_ISBGR(dd))
        return -1;
      dd->fp_transsdcol   = serdisp_transsdcol_rgb444;
      dd->fp_transsdgrey  = serdisp_transsdgrey_rgb444;
      dd->fp_lookupsdcol  = serdisp_lookupsdcol_rgb444;
      dd->fp_lookupsdgrey = serdisp_lookupsdgrey_rgb444;
      break;
    case 16:
      if (dd->colour_spaces & SD_CS_RGB565) {
        if (SD_CS_ISBGR(dd)) {
          dd->fp_transsdcol   = serdisp_transsdcol_bgr565;
          dd->fp_transsdgrey  = serdisp_transsdgrey_bgr565;
          dd->fp_lookupsdcol  = serdisp_lookupsdcol_bgr565;
          dd->fp_lookupsdgrey = serdisp_lookupsdgrey_bgr565;
        } else {
          dd->fp_transsdcol   = serdisp_transsdcol_rgb565;
          dd->fp_transsdgrey  = serdisp_transsdgrey_rgb565;
          dd->fp_lookupsdcol  = serdisp_lookupsdcol_rgb565;
          dd->fp_lookupsdgrey = serdisp_lookupsdgrey_rgb565;
        }
      }
      break;
    case 18:
      if (SD_CS_ISBGR(dd))
        return -1;
      dd->fp_transsdcol   = serdisp_transsdcol_rgb666;
      dd->fp_transsdgrey  = serdisp_transsdgrey_rgb666;
      dd->fp_lookupsdcol  = serdisp_lookupsdcol_rgb666;
      dd->fp_lookupsdgrey = serdisp_lookupsdgrey_rgb666;
      break;
    case 24:
      if (SD_CS_ISBGR(dd))
        return -1;
      dd->fp_transsdcol   = serdisp_transsdcol_rgb888;
      dd->fp_transsdgrey  = serdisp_transsdgrey_rgb888;
      dd->fp_lookupsdcol  = serdisp_lookupsdcol_rgb888;
      dd->fp_lookupsdgrey = serdisp_lookupsdgrey_rgb888;
      break;
    case 32:
      if (SD_CS_ISBGR(dd))
        return -1;
      dd->fp_transsdcol   = serdisp_transsdcol_rgba888;
      dd->fp_transsdgrey  = serdisp_transsdgrey_rgba888;
      dd->fp_lookupsdcol  = serdisp_lookupsdcol_rgba888;
      dd->fp_lookupsdgrey = serdisp_lookupsdgrey_rgba888;
      break;
    default:
      return -1;
  }
  return 0;
}


/**
  * \brief   translates a colour value to the hardware dependent value
  *
  * Translates an \p 0xAARRGGBB colour value to a hardware dependent value that is suitable for serdisp_setsdpixel().
  *
  * \param   dd            device descriptor
  * \param   sdcol         \p 0xAARRGGBB colour value
  *
  * \return translated, serdisp_setsdpixel()-compliant, hardware dependent colour value
  *
  * \since 1.98.0
  */
SDCol serdisp_transsdcol(serdisp_t* dd, SDCol sdcol) {
  return dd->fp_transsdcol(dd, sdcol);
}



/**
  * \brief   translates a grey value to the hardware dependent value
  *
  * Translates a grey value to a hardware dependent value that is suitable for serdisp_setsdpixel()
  *
  * \param   dd            device descriptor
  * \param   greyvalue     grey value
  *
  * \return translated, serdisp_setsdpixel()-compliant, hardware dependent colour value
  *
  * \since 1.98.0
  */
SDCol serdisp_transsdgrey(serdisp_t* dd, byte greyvalue) {
  return dd->fp_transsdgrey(dd, greyvalue);
}



/**
  * \brief   looks up hardware independent colour value to the hardware dependent value
  *
  * Looks up the corresponding \p 0xAARRGGBB colour value to a 
  * serdisp_setpixel()-compliant colour value.
  *
  * \param   dd            device descriptor
  * \param   sdcol         serdisp_setsdpixel()-compliant, hardware dependent colour value
  *
  * \return translated hardware independent colour value, format: \p 0xAARRGGBB
  *
  * \since 1.98.0
  */
SDCol serdisp_lookupsdcol(serdisp_t* dd, SDCol sdcol) {
  return dd->fp_lookupsdcol(dd, sdcol);
}



/**
  * \brief   looks up hardware independent grey value to the hardware dependent colour value
  *
  * Looks up the corresponding grey value to a serdisp_setsdpixel()-compliant colour value.
  *
  * \param   dd            device descriptor
  * \param   sdcol         serdisp_setsdpixel()-compliant, hardware dependent colour value
  *
  * \return translated grey value, format <tt>[0 .. 255]</tt>
  *
  * \since 1.98.0
  */
byte serdisp_lookupsdgrey(serdisp_t* dd, SDCol sdcol) {
  return dd->fp_lookupsdgrey(dd, sdcol);
}


#ifdef SD_SUPP_ARCHDEP_COLOUR_FUNCTIONS
/****************************************/
/* deprecated / compatibility functions */
/****************************************/
/**
  * \brief   set a colour in the display buffer
  *
  * This function sets the colour information <em>hardware independently</em>.\n
  * The information is changed in the <em>display buffer</em> only.
  *
  * \param   dd            device descriptor
  * \param   x             x-position
  * \param   y             y-position
  * \param   colour        colour \n
  *                        format: \p 0xAARRGGBB , \p AA .. alpha, \p RR .. red, \p GG .. green, \p BB .. blue
  *
  * \b Examples: \n
  * both set a red pixel in the display buffer at position \p (10/20)
  * \code
  * serdisp_setcolour(dd, 10, 20, 0xFFFF0000); 
  * serdisp_setcolour(dd, 10, 20, SD_COL_RED); 
  * \endcode
  *
  * \deprecated The byte size of colour is architecture dependent. \n
  *             Architecture independent function serdisp_setsdcol() should be used instead.
  */
void serdisp_setcolour(serdisp_t* dd, int x, int y, long colour) {
  serdisp_setsdpixel(dd, x, y, serdisp_transsdcol(dd, (SDCol)colour));
}


/**
  * \brief   set grey value in the display buffer
  *
  * This function sets the grey value <em>hardware independently</em>.\n
  * The information is changed in the <em>display buffer</em> only.
  *
  * \param   dd            device descriptor
  * \param   x             x-position
  * \param   y             y-position
  * \param   grey          grey value; format: <tt>[0 .. 255]</tt>
  *
  * \b Examples: \n
  * set a grey value in the display buffer at position \p (10/20)
  * \code
  * serdisp_setgrey(dd, 10, 20, 0xCC); 
  * \endcode
  *
  * \deprecated Old naming scheme for architecture dependent colour functions. \n
  *             serdisp_setsdgrey() should be used instead.
  */
void serdisp_setgrey(serdisp_t* dd, int x, int y, byte grey) {
  serdisp_setsdgrey(dd, x, y, grey);
}


/**
  * \brief   gets colour value at position (x/y)
  *
  * Gets the <em>hardware independent</em> colour value at position (x/y)
  *
  * \param   dd            device descriptor
  * \param   x             x-position
  * \param   y             y-position
  *
  * \return hardware independent colour value at (x/y) \n
  *         format: <tt>0xAARRGGBB</tt>, \p AA .. alpha, \p RR .. red, \p GG .. green, \p BB .. blue
  *
  * \deprecated The byte size of colour is architecture dependent. \n
  *             Architecture independent function serdisp_getsdcol() should be used instead.
  */
long serdisp_getcolour(serdisp_t* dd, int x, int y) {
  return (long)(serdisp_lookupsdcol(dd, serdisp_getsdpixel(dd, x, y)));
}


/**
  * \brief   gets grey value at position (x/y)
  *
  * Gets the grey value at position (x/y). colour values are converted to the corresponding greyscale values.
  *
  * \param   dd            device descriptor
  * \param   x             x-position
  * \param   y             y-position
  *
  * \return grey value ,format: <tt>[0 .. 255]</tt>
  *
  * \deprecated Old naming scheme for architecture dependent colour functions. \n
  *             serdisp_getsdgrey() should be used instead.
  */
byte serdisp_getgrey(serdisp_t* dd, int x, int y) {
  return serdisp_getsdgrey(dd, x, y);
}




/**
  * \brief   translates a colour value to the hardware dependent value
  *
  * Translates an \p 0xAARRGGBB colour value to a hardware dependent value that is suitable for serdisp_setpixel().
  *
  * \param   dd            device descriptor
  * \param   colour        \p 0xAARRGGBB colour value
  *
  * \return translated, serdisp_setpixel()-compliant, hardware dependent colour value
  *
  * \deprecated The byte size of colour is architecture dependent. \n
  *             Architecture independent function serdisp_transsdcol() should be used instead.
  */
long serdisp_transcolour(serdisp_t* dd, long colour) {
  return (long)(dd->fp_transsdcol(dd, (SDCol)colour));
}



/**
  * \brief   translates a grey value to the hardware dependent value
  *
  * Translates a grey value to a hardware dependent value that is suitable for serdisp_setpixel()
  *
  * \param   dd            device descriptor
  * \param   greyvalue     grey value
  *
  * \return translated, serdisp_setpixel()-compliant, hardware dependent colour value
  *
  * \deprecated The byte size of colour is architecture dependent. \n
  *             Architecture independent function serdisp_transsdgrey() should be used instead.
  */
long serdisp_transgrey(serdisp_t* dd, byte greyvalue) {
  return (long)(dd->fp_transsdgrey(dd, greyvalue));
}



/**
  * \brief   looks up hardware independent colour value to the hardware dependent value
  *
  * Looks up the corresponding \p 0xAARRGGBB colour value to a 
  * serdisp_setpixel()-compliant colour value.
  *
  * \param   dd            device descriptor
  * \param   colour        serdisp_setpixel()-compliant, hardware dependent colour value
  *
  * \return translated hardware independent colour value, format: \p 0xAARRGGBB
  *
  * \deprecated The byte size of colour is architecture dependent. \n
  *             Architecture independent function serdisp_lookupsdcol() should be used instead.
  */
long serdisp_lookupcolour(serdisp_t* dd, long colour) {
  return (long)(dd->fp_lookupsdcol(dd, (SDCol)colour));
}



/**
  * \brief   looks up hardware independent grey value to the hardware dependent colour value
  *
  * Looks up the corresponding grey value to a serdisp_setpixel()-compliant colour value.
  *
  * \param   dd            device descriptor
  * \param   colour        serdisp_setpixel()-compliant, hardware dependent colour value
  *
  * \return translated grey value, format <tt>[0 .. 255]</tt>
  *
  * \deprecated The byte size of colour is architecture dependent. \n
  *             Architecture independent function serdisp_lookupsdgrey() should be used instead.
  */
byte serdisp_lookupgrey(serdisp_t* dd, long colour) {
  return (long)(dd->fp_lookupsdgrey(dd, (SDCol)colour));
}
#endif


/* *********************************
   void serdisp_setsdcoltabentry (dd, idx, colour)
   *********************************
   set a colour in the colour table
   *********************************
   dd     ... display descriptor
   idx    ... index in colour table
   sdcol  ... colour (format: 0xAARRGGBB)
   *********************************
*/
void serdisp_setsdcoltabentry(serdisp_t* dd, int idx, SDCol sdcol) {
  if ( (!dd->ctable) || idx >= serdisp_getcolours(dd))
    return;
  dd->ctable[idx] = sdcol;
}


#ifdef SD_SUPP_ARCHDEP_COLOUR_FUNCTIONS
void serdisp_setcoltabentry(serdisp_t* dd, int idx, long colour) {
  serdisp_setsdcoltabentry(dd, idx, (SDCol)colour);
}
#endif

/* *********************************
   long serdisp_getsdcoltabentry (dd, idx)
   *********************************
   get  a colour from the colour table
   *********************************
   dd     ... display descriptor
   idx    ... index in colour table
   *********************************
   returns colour at index idx
*/
SDCol serdisp_getsdcoltabentry(serdisp_t* dd, int idx) {
  if ( (!dd->ctable) || idx >= serdisp_getcolours(dd)) 
    return 0;
  return dd->ctable[idx];
}


#ifdef SD_SUPP_ARCHDEP_COLOUR_FUNCTIONS
long serdisp_getcoltabentry(serdisp_t* dd, int idx) {
  return (long)serdisp_getsdcoltabentry(dd, idx);
}
#endif

/* internal use */

/* source: http://www.compuphase.com/cmetric.htm */
uint32_t serdisp_colourdistance(SDCol col1, SDCol col2) {
  uint32_t r1, g1, b1;
  uint32_t r2, g2, b2;
  uint32_t distr, distg, distb, meanr;

  b1 = col1 & 0xFF;  g1 = (col1 & 0xFF00) >> 8; r1 = (col1 & 0xFF0000) >> 16;
  b2 = col2 & 0xFF;  g2 = (col2 & 0xFF00) >> 8; r2 = (col2 & 0xFF0000) >> 16;

  distr = r1-r2;  distg = g1-g2; distb = b1-b2;
  meanr = (r1 + r2) >> 1;

  /* return distr*distr + distg*distg + distb*distb; */
  return (((512+meanr)*distr*distr)>>8) + 4*distg*distg + (((767-meanr)*distb*distb)>>8);
}


/* 
 * colour scheme functions
 */


/* *********************************
   SDCol serdisp_transsdcol_* (dd, sdcol)
   *********************************
   translates an ARGB colour to a value suitable for setsdpixel
   *********************************
   dd     ... display descriptor
   sdcol  ... ARGB colour value
   *********************************
   returns a translated, setsdpixel-compliant colour value
*/
SDCol serdisp_transsdcol_bw (serdisp_t* dd, SDCol sdcol) {
  /* depth 1 (b/w) anomaly: index 0 == white, index 1 == black */
  return ( (serdisp_ARGB2GREY(sdcol) > 127 ) ? 0x00000000 : 0x00000001);
}

SDCol serdisp_transsdcol_grey2_4(serdisp_t* dd, SDCol sdcol) {
  SDCol    minidx = 0;
  uint32_t mincross = serdisp_colourdistance(sdcol, serdisp_getsdcoltabentry(dd, 0));
  uint32_t tempcolcross;
  uint32_t i;

  for (i = 1; i < (1 << dd->depth); i++) {
    tempcolcross = serdisp_colourdistance(sdcol, serdisp_getsdcoltabentry(dd, i));
    if (tempcolcross < mincross) {
      mincross = tempcolcross;
      minidx = i;
    }
  }
  return minidx;
}

SDCol serdisp_transsdcol_grey8 (serdisp_t* dd, SDCol sdcol) {
  return ((SDCol)(serdisp_ARGB2GREY(sdcol)));
}


/* 8 bit colour parts -> 3/3/2 bit by throwing away least significant bits */
/* 0xAARRGGBB -> byte(RRRGGGBB) */
SDCol serdisp_transsdcol_rgb332 (serdisp_t* dd, SDCol sdcol) {
  return  ( (( sdcol & 0x00E00000) >> 16) | ((sdcol & 0x0000E000) >> 11) | ((sdcol & 0x000000C0) >> 6) );
}

/* 8 bit colour parts -> 3/3/2 bit by throwing away least significant bits */
/* 0xAARRGGBB -> byte(BBBGGGRR) */
SDCol serdisp_transsdcol_bgr332 (serdisp_t* dd, SDCol sdcol) {
  return  ( (( sdcol & 0x00C00000) >> 22) | ((sdcol & 0x0000E000) >> 11) | ((sdcol & 0x000000E0) >> 0) );
}

/* 8 bit colour part -> 4 bit by throwing away 4 least significant bits */
/* 0xAARRGGBB -> 0x00000RGB */
SDCol serdisp_transsdcol_rgb444 (serdisp_t* dd, SDCol sdcol) {
  return  ( (( sdcol & 0x00F00000) >> 12) | ((sdcol & 0x0000F000) >> 8) | ((sdcol & 0x000000F0) >> 4) );
}

/* 8 bit colour part -> 5 or 6 bit by throwing away 3 or 2 least significant bits */
/* 0xAARRGGBB -> 5bit R 6bit G 5bit B */
SDCol serdisp_transsdcol_rgb565 (serdisp_t* dd, SDCol sdcol) {
  return  ( (( sdcol & 0x00F80000) >> 8) | ((sdcol & 0x0000FC00) >> 5) | ((sdcol & 0x000000F8) >> 3) );
}

/* 8 bit colour part -> 5 or 6 bit by throwing away 3 or 2 least significant bits */
/* 0xAARRGGBB -> 5bit B 6bit G 5bit R */
SDCol serdisp_transsdcol_bgr565 (serdisp_t* dd, SDCol sdcol) {
  return  ( (( sdcol & 0x00F80000) >> 19) | ((sdcol & 0x0000FC00) >> 5) | ((sdcol & 0x000000F8) << 8) );
}

/* 8 bit colour part -> 6 bit by throwing away 2 least significant bits */
/* 0xAARRGGBB -> 6bit R 6bit G 6bit B */
SDCol serdisp_transsdcol_rgb666 (serdisp_t* dd, SDCol sdcol) {
  return  ( (( sdcol & 0x00FC0000) >> 6) | ((sdcol & 0x0000FC00) >> 4) | ((sdcol & 0x000000FC) >> 2) );
}

/* truecolour without alpha */
SDCol serdisp_transsdcol_rgb888 (serdisp_t* dd, SDCol sdcol) {
  return  ( sdcol & 0x00FFFFFF );
}

/* truecolour with alpha */
SDCol serdisp_transsdcol_rgba888 (serdisp_t* dd, SDCol sdcol) {
  return sdcol;
}



/* *********************************
   SDCol serdisp_transsdgrey_* (dd, greyvalue)
   *********************************
   translates a grey value to a value suitable for setsdpixel
   *********************************
   dd        ... display descriptor
   greyvalue ... grey value
   *********************************
   returns a translated, setsdpixel-compliant colour value
*/
SDCol serdisp_transsdgrey_bw (serdisp_t* dd, byte greyvalue) {
  /* depth 1 (b/w) anomaly: index 0 == white, index 1 == black */
  return ( ( greyvalue > 127 ) ? 0x00000000 : 0x00000001);
}

SDCol serdisp_transsdgrey_grey2_4(serdisp_t* dd, byte greyvalue) {
  SDCol    minidx = 0;
  uint32_t mincross = serdisp_colourdistance(serdisp_GREY2ARGB(greyvalue), serdisp_getsdcoltabentry(dd, 0));
  uint32_t tempcolcross;
  uint32_t i;

  for (i = 1; i < (1 << dd->depth); i++) {
    tempcolcross = serdisp_colourdistance(serdisp_GREY2ARGB(greyvalue), serdisp_getsdcoltabentry(dd, i));
    if (tempcolcross < mincross) {
      mincross = tempcolcross;
      minidx = i;
    }
  }
  return minidx;
}


SDCol serdisp_transsdgrey_grey8 (serdisp_t* dd, byte greyvalue) {
  return ( (SDCol)greyvalue );
}

SDCol serdisp_transsdgrey_rgb332 (serdisp_t* dd, byte greyvalue) {
  return ( (SDCol)(0xE0 & greyvalue) | ((SDCol)(0xE0 & greyvalue) >> 3) | ((SDCol)(0xC0 & greyvalue) >> 6) );
}

SDCol serdisp_transsdgrey_bgr332 (serdisp_t* dd, byte greyvalue) {
  return ( ((SDCol)(0xC0 & greyvalue) >> 6) | ((SDCol)(0xE0 & greyvalue) >> 3) | (SDCol)(0xE0 & greyvalue) );
}

SDCol serdisp_transsdgrey_rgb444 (serdisp_t* dd, byte greyvalue) {
  return ( ((SDCol)(0xF0 & greyvalue) << 4) | (SDCol)(0xF0 & greyvalue) | ((SDCol)(0xF0 & greyvalue) >> 4) );
}

SDCol serdisp_transsdgrey_rgb565 (serdisp_t* dd, byte greyvalue) {
  return ( ((SDCol)(0xF8 & greyvalue) << 8) | ((SDCol)(0xFC & greyvalue) <<3 ) | ((SDCol)(0xF8 & greyvalue) >> 3) );
}

SDCol serdisp_transsdgrey_bgr565 (serdisp_t* dd, byte greyvalue) {
  return ( ((SDCol)(0xF8 & greyvalue) << 8) | ((SDCol)(0xFC & greyvalue) <<3 ) | ((SDCol)(0xF8 & greyvalue) >> 3) );
}

SDCol serdisp_transsdgrey_rgb666 (serdisp_t* dd, byte greyvalue) {
  return ( ((SDCol)(0xFC & greyvalue) << 10) | ((SDCol)(0xFC & greyvalue) <<4 ) | ((SDCol)(0xFC & greyvalue) >> 2) );
}

/* truecolour without alpha */
SDCol serdisp_transsdgrey_rgb888 (serdisp_t* dd, byte greyvalue) {
  return  ( ((SDCol)greyvalue << 16) | ((SDCol)greyvalue << 8) | (SDCol)greyvalue );
}

/* truecolour with alpha */
SDCol serdisp_transsdgrey_rgba888 (serdisp_t* dd, byte greyvalue) {
  return  ( 0xFF000000 | ((SDCol)greyvalue << 16) | ((SDCol)greyvalue << 8) | (SDCol)greyvalue );
}



/* *********************************
   SDCol serdisp_lookupsdcol_* (dd, sdcol)
   *********************************
   looks up the corresponding ARGB colour value to a setsdpixel-compliant colour value
   *********************************
   dd     ... display descriptor
   sdcol  ... setsdpixel-compliant colour value
   *********************************
   returns a translated colour value (format: 0xAARRGGBB)
*/
SDCol serdisp_lookupsdcol_bw(serdisp_t* dd, SDCol sdcol) {
  /* depth 1 (b/w) anomaly: index 0 == white, index 1 == black */
  return ( (sdcol > 0x00000000) ? 0xFF000000 : 0xFFFFFFFF);
}

SDCol serdisp_lookupsdcol_grey2_4(serdisp_t* dd, SDCol sdcol) {
  return ( serdisp_getsdcoltabentry(dd, sdcol) );
}

SDCol serdisp_lookupsdcol_grey8(serdisp_t* dd, SDCol sdcol) {
  return ( serdisp_GREY2ARGB( (byte)(0x000000FF | sdcol) ) );
}


/* byte(RRRGGGBB) -> 0xAARRGGBB */
SDCol serdisp_lookupsdcol_rgb332(serdisp_t* dd, SDCol sdcol) {
  return ( 0xFF1F1F3F |  ((0xE0 & sdcol) << 16 ) | ((0x1C & sdcol) << 11 ) | ((0x00000003 & sdcol) << 6 ) );
}

/* byte(BBBGGGRR) -> 0xAARRGGBB */
SDCol serdisp_lookupsdcol_bgr332(serdisp_t* dd, SDCol sdcol) {
  return ( 0xFF3F1F1F |  (0xE0 & sdcol) | ((0x1C & sdcol) << 11 ) | ((0x00000003 & sdcol) << 22 ) );
}


/* 0x00000RGB -> 0xAARRGGBB */
SDCol serdisp_lookupsdcol_rgb444(serdisp_t* dd, SDCol sdcol) {
  return ( 0xFF0F0F0F |  ((0x00000F00 & sdcol) << 12 ) | ((0x000000F0 & sdcol) << 8 ) | ((0x0000000F & sdcol) << 4 ) );
}

/* 5bit R  6bit G  5bit B -> 0xAARRGGBB */
SDCol serdisp_lookupsdcol_rgb565(serdisp_t* dd, SDCol sdcol) {
  return ( 0xFF070307 |  ((0xF800 & sdcol) << 8 ) | ((0x07E0 & sdcol) << 5 ) | ((0x001F & sdcol) << 3 ) );
}

/* 5bit B  6bit G  5bit R -> 0xAARRGGBB */
SDCol serdisp_lookupsdcol_bgr565(serdisp_t* dd, SDCol sdcol) {
  return ( 0xFF070307 |  ((0x001F & sdcol) << 19 ) | ((0x07E0 & sdcol) << 5 ) | ((0xF800 & sdcol) >> 8 ) );
}

/* 6bit R  6bit G  6bit B -> 0xAARRGGBB */
SDCol serdisp_lookupsdcol_rgb666(serdisp_t* dd, SDCol sdcol) {
  return ( 0xFF030303 |  ((0x03F000 & sdcol) << 6 ) | ((0x000FC0 & sdcol) << 4 ) | ((0x00003F & sdcol) << 2 ) );
}

/* truecolour without alpha */
SDCol serdisp_lookupsdcol_rgb888 (serdisp_t* dd, SDCol sdcol) {
  return  ( 0xFF000000 | sdcol );
}

/* truecolour with alpha */
SDCol serdisp_lookupsdcol_rgba888 (serdisp_t* dd, SDCol sdcol) {
  return sdcol;
}



/* *********************************
   byte serdisp_lookupsdgrey_* (dd, sdcol)
   *********************************
   translates a colour/grey value so that it is useable for setsdpixel
   *********************************
   dd     ... display descriptor
   sdcol  ... setsdpixel-compliant colour value
   *********************************
   returns an 8-bit grey value
*/
byte serdisp_lookupsdgrey_bw(serdisp_t* dd, SDCol sdcol) {
  /* depth 1 (b/w) anomaly: index 0 == white, index 1 == black */
  return ( (sdcol > 0x00000000) ? 0 : 255);
}


byte serdisp_lookupsdgrey_grey2_4(serdisp_t* dd, SDCol sdcol) {
  return ( serdisp_getsdcoltabentry(dd, sdcol) );
}


byte serdisp_lookupsdgrey_grey8(serdisp_t* dd, SDCol sdcol) {
  return ( (byte)(0x000000FF | sdcol) );
}


byte serdisp_lookupsdgrey_rgb332(serdisp_t* dd, SDCol sdcol) {
  return ( serdisp_ARGB2GREY(serdisp_lookupsdcol_rgb332(dd, sdcol)) );
}

byte serdisp_lookupsdgrey_bgr332(serdisp_t* dd, SDCol sdcol) {
  return ( serdisp_ARGB2GREY(serdisp_lookupsdcol_bgr332(dd, sdcol)) );
}

byte serdisp_lookupsdgrey_rgb444(serdisp_t* dd, SDCol sdcol) {
  return ( serdisp_ARGB2GREY(serdisp_lookupsdcol_rgb444(dd, sdcol)) );
}

byte serdisp_lookupsdgrey_rgb565(serdisp_t* dd, SDCol sdcol) {
  return ( serdisp_ARGB2GREY(serdisp_lookupsdcol_rgb565(dd, sdcol)) );
}

byte serdisp_lookupsdgrey_bgr565(serdisp_t* dd, SDCol sdcol) {
  return ( serdisp_ARGB2GREY(serdisp_lookupsdcol_bgr565(dd, sdcol)) );
}

byte serdisp_lookupsdgrey_rgb666(serdisp_t* dd, SDCol sdcol) {
  return ( serdisp_ARGB2GREY(serdisp_lookupsdcol_rgb666(dd, sdcol)) );
}

/* truecolour without alpha */
byte serdisp_lookupsdgrey_rgb888 (serdisp_t* dd, SDCol sdcol) {
  return ( serdisp_ARGB2GREY(serdisp_lookupsdcol_rgb888(dd, sdcol)) );
}

/* truecolour with alpha */
byte serdisp_lookupsdgrey_rgba888 (serdisp_t* dd, SDCol sdcol) {
  return ( serdisp_ARGB2GREY(serdisp_lookupsdcol_rgba888(dd, sdcol)) );
}
