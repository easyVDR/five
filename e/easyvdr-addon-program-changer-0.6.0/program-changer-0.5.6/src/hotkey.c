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
#include "program-changer.h"
#include "pconfig.h"
#include "error.h"

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
    pthread_exit(0);
  }

  if (d->category != XRecordFromServer)
	return;
  unsigned char type = ((unsigned char*)d->data)[0] & 0x7F;
  unsigned char detail = ((unsigned char*)d->data)[1];
  if (type == KeyPress)
  { // fprintf(stderr,"Key %d\n",detail);
    if( detail==program_config.iKey_Pchanger)
	//if( detail==24)		  
	{
		if(PchangerKey==FALSE)
			PchangerKey=TRUE;
		else
			PchangerKey=FALSE;
	}
/*
	if( detail==24)
	   system("/usr/share/easyvdr/program-changer/utilities.sh master_volume_down");
	if( detail==25)
	   system("/usr/share/easyvdr/program-changer/utilities.sh master_volume_up");
	if( detail==26)
	   system("/usr/share/easyvdr/program-changer/utilities.sh master_volume_mute");
	*/  
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
  if (!XRecordEnableContext(dpy, rc, key_pressed_cb,  (XPointer)(void*)data  )) {
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
