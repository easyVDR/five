#include "setup.h"

cWetterSetup wetterSetup;

cWetterSetup::cWetterSetup(void)
{
  w_update       = 0;
  w_left         = 100;
  w_top          = 130;
  w_width        = 560;
  w_height       = 316;
  w_hiquality    = 0;
  w_osdoffset_x  = 0;
  w_osdoffset_y  = 0;
  w_dither       = 1;
  w_alpha        = 210;
  w_dayf         = 3;

  strncpy(w_id1,"EUR|DE|GM001|WEIL AM RHEIN",sizeof(w_id1));        
  strncpy(w_id2,"EUR|DE|GM001|ELLWANGEN",sizeof(w_id2));        
  strncpy(w_id3,"EUR|DE|GM006|HAMBURG",sizeof(w_id3));        

  strncpy(w_id1_name,"Weil am Rhein",sizeof(w_id1_name));        
  strncpy(w_id2_name,"Ellwangen",sizeof(w_id2_name));        
  strncpy(w_id3_name,"Hamburg",sizeof(w_id3_name));        

  w_treedepth    = 4;
}
