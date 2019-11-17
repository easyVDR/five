
#ifndef __SETUP_H
#define __SETUP_H


#include <ctype.h>
#include <climits>
#include <vector>
#include <string>
#include <vdr/plugin.h>

struct cWetterSetup
{
public:
 cWetterSetup(void);
 int w_update;
 int w_left;
 int w_top;
 int w_width;
 int w_height;
 int w_hiquality;
 int w_osdoffset_x;
 int w_osdoffset_y;
 int w_dither;
 int w_alpha;
 int w_dayf;
 int w_treedepth;

 char w_id1[64];
 char w_id2[64];
 char w_id3[64];

 char w_id1_name[64];
 char w_id2_name[64];
 char w_id3_name[64];



};

extern cWetterSetup wetterSetup;

#endif //__SETUP_H 
