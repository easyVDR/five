/*
 * seduservice.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id: seduservice.c,v 1.3 2012/11/22 18:02:59 wendel Exp $
 */

#include <math.h>

#include "seduservice.h"
#include "common.h"

//***************************************************************************
// View Modes
//***************************************************************************

const char* cSeduService::viewModes[] =
{
   "atmo",
   "fixed color",
   "rainbow",
   "moving color wheel",
   "static color wheel",
   "black",
   "detached",

   0
};

const char* cSeduService::toName(ViewMode vm)
{
   return viewModes[vm];
}

//***************************************************************************
// Color conversion functions
//***************************************************************************

void cSeduService::rgb2hsv(int r, int g, int b, double* h, double* s, double* v)
{
   double minC, maxC, delta, rc, gc, bc;
   rc = (double)r / 255.0;
   gc = (double)g / 255.0;
   bc = (double)b / 255.0;
   maxC = max(rc, max(gc, bc));
   minC = min(rc, min(gc, bc));
   delta = maxC - minC;
   *v = maxC;

   if (maxC != 0)
      *s = delta / maxC;
   else
      *s = 0;

   if (*s == 0) 
   {
      *h = 0; 
   }
   else 
   {
      if (rc == maxC)
         *h = (gc - bc) / delta;
      else if (gc == maxC)
         *h = 2 + (bc - rc) / delta;
      else if (bc == maxC)
         *h = 4 + (rc - gc) / delta;
      
      *h *= 60.0;

      if (*h < 0)
         *h += 360.0;
   }
}

//***************************************************************************
// 
//***************************************************************************

Pixel cSeduService::hsv2rgb(int h, double s, double v)
{
   Pixel p;

   double rr = 0; 
   double gg = 0; 
   double bb = 0;
   
   int i = floor(h/60.0);
   double f = h/60.0 - i;
   double pv = v * (1 - s);
   double qv = v * (1 - s * f);
   double tv = v * (1 - s * (1-f));
   
   switch (i)
   {
      case 0:    // rojo dominante
         rr = v;
         gg = tv;
         bb = pv;
         break;

      case 1:    // verde
         rr = qv;
         gg = v;
         bb = pv;
         break;

      case 2: 
         rr = pv;
         gg = v;
         bb = tv;
         break;

      case 3:    // azul
         rr = pv;
         gg = qv;
         bb = v;
         break;

      case 4:
         rr = tv;
         gg = pv;
         bb = v;
         break;

      case 5:    // rojo
         rr = v;
         gg = pv;
         bb = qv;
         break;
   }

   // set each component to a integer value between 0 and 255

   p.r = minMax(255*rr, 0, 255);
   p.g = minMax(255*gg, 0, 255);
   p.b = minMax(255*bb, 0, 255);
   
   return p;
}
