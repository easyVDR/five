//Original von @author Wei-Ning Huang (AZ) <aitjcize@gmail.com>

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <X11/Xlib.h>
#include <X11/extensions/record.h>
#include "parameter.h"
#include "make_lirc_conf_files.h"
#include "message_and_error_log.h"


static Display* dpy = 0;
static XRecordContext rc;
static gboolean stop_thread = FALSE;
 

void key_pressed_cb(XPointer arg, XRecordInterceptData *d) {
  if (stop_thread) {
    if (!XRecordDisableContext(dpy, rc)) {
		error("key_pressed_cb XRecordDisableContext",
		         "key_pressed_cb XRecordDisableContext error");
		return;
    }
    if (!XRecordFreeContext(dpy, rc)) {
		error("key_pressed_cb XRecordFreeContext",
		         "key_pressed_cb XRecordFreeContext error");
		return;
    }
	stop_thread=FALSE;
    pthread_exit(0);
  }

  if (d->category != XRecordFromServer)
	return;
  unsigned char type = ((unsigned char*)d->data)[0] & 0x7F;
  unsigned char detail = ((unsigned char*)d->data)[1];
  if (type == KeyPress)
  {  

	  	if(detail!=36) // ->Return
	  	{
			if(bcheck_keypchanger==TRUE)
			{	
				Key_Pchanger=detail;
			}
			else
			{
				if(bcheck_Key_Red==TRUE)
				{
					Key_Red=detail;
				}
				else
				{
					if(bcheck_Key_Green==TRUE)
					{
						Key_Green=detail;
					}
					else
					{
						if(bcheck_Key_Yellow==TRUE)		   
						{
							Key_Yellow=detail;
						}
						else
						{
							if(bcheck_Key_Blue==TRUE)
							{								
								Key_Blue=detail;
							}
		  					else
							{								
			  					if(bcheck_Key_Menu==TRUE)
								{								
									Key_Menu=detail;
								}	
							}
									
						} 
					}
				}

			}
		}

		//	  fprintf(stderr,"X Key d %d \n",detail);//,detail);
//	  fprintf(stderr,"R %d G %d Y %d B %d",Key_Red,Key_Green,Key_Yellow,Key_Blue);//,detail);
	}  
}


void* intercept_key_thread(void* data)
{
  XRecordClientSpec rcs;
  XRecordRange* rr;
  dpy = XOpenDisplay(0);

  if (!(rr = XRecordAllocRange())) {
		error("intercept_key_thread XRecordAllocRange",
	         "intercept_key_thread XRecordAllocRange error");
		return 0;
  }
  rr->device_events.first = KeyPress;
  rr->device_events.last = MotionNotify;
  rcs = XRecordAllClients;

  if (!(rc = XRecordCreateContext(dpy, 0, &rcs, 1, &rr, 1))) {
		error("intercept_key_thread XRecordCreateContext",
	         "intercept_key_thread XRecordCreateContext error");
		return 0;
  }
  XFree(rr);
  if (!XRecordEnableContext(dpy, rc, key_pressed_cb, data)) {
		error("intercept_key_thread XRecordEnableContext",
	         "intercept_key_thread XRecordEnableContext error");
		return 0;

  }
  return 0;
}


void stop_intercept_key_thread(void)
{
  void* result;
  stop_thread = TRUE;
  if (0 != pthread_join(thread, &result)) {
		error("stop_intercept_key_thread pthread_join",
	         "stop_intercept_key_thread pthread_join error");
		return;
  }
}
