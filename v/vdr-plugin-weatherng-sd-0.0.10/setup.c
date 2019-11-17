#include "setup.h"

cWetterSetup wetterSetup;

cWetterSetup::cWetterSetup(void)
{
	w_update       = 0;
	w_left         = 110;
	w_top          = 138;
	w_width        = 500;
	w_height       = 300;
    w_hiquality    = 0;
	w_osdoffset_x  = 0;
	w_osdoffset_y  = 0;
    w_dither       = 1;
	w_alpha        = 255;

        strncpy(w_id1,"GMXX6195",sizeof(w_id1));        
        strncpy(w_id2,"GMXX5525",sizeof(w_id2));        
        strncpy(w_id3,"UKXX0085",sizeof(w_id3));        

        strncpy(w_id1_name,"Ellwangen",sizeof(w_id1_name));        
        strncpy(w_id2_name,"Weil/Rhein",sizeof(w_id2_name));        
        strncpy(w_id3_name,"London",sizeof(w_id3_name));      

#ifdef USE_MAGICK
	w_treedepth    = 4;
#endif


}
