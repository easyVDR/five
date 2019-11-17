#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
 
#include <time.h>
#include <sys/time.h>

#include <gtk/gtk.h>

#include <glib/gi18n.h>
#include "mouse.h"
#include "error.h"


#include <X11/Xlib.h>

//sudo apt-get install  libx11-dev 

extern double getTime_ms(void);

extern int iScreenWidth,iScreenHeight;
extern char acInfo[];
int iXpos,iYpos;

static GdkDisplay *gdk_display;
static GdkScreen* gdk_screen = NULL;
static GdkDevice  *gdk_pointer;

static Display *display;
static XEvent event;

int bMouse=FALSE;
static Window root_window;
struct timespec ts_res, ts_start, ts_end;



static Time fake_timestamp()
     /*seems that xfree86 computes the timestamps like this     */
     /*strange but it relies on the *1000-32bit-wrap-around     */
     /*if anybody knows exactly how to do it, please contact me */
{
	int tint;
	struct timeval tv;
	struct timezone tz;	/* is not used since ages */
	gettimeofday(&tv, &tz);
	tint = (int)tv.tv_sec * 1000;
	tint = tint / 1000 * 1000;
	tint = tint + tv.tv_usec / 1000;
	return (Time) tint;
}

int SendPressButton(int iButton,int bDoubleClick,Window window)
{
int i;
int static bReady=TRUE;

//Testen ob noch ein click in bearbeitung ist
if(bReady!=TRUE)
	return (TRUE);

bReady=FALSE;
	
for(i=0;i<2;i++)
{
	event.type = ButtonPress;
	event.xbutton.button = iButton;
	event.xbutton.same_screen = True;
	
	event.xbutton.time = fake_timestamp();
	
	if(XSendEvent(display,window , True, 0xfff, &event) == 0)
	{
		fprintf(stderr, "Error SendButton\n");
		bReady=TRUE;
		return (FALSE);
	}

	XFlush(display);

	//0,5s 0,500000
	//200000 /Funktioniert gut
	//usleep(200000);
	usleep(180000);
	event.type = ButtonRelease;
	event.xbutton.state = 0x100;
	event.xbutton.time = fake_timestamp();

	if(XSendEvent(display, window, True, 0xfff, &event) == 0) 
	{
		fprintf(stderr, "Error SendButton\n");
		bReady=TRUE;
		return (FALSE);
	}

	XFlush(display);

	if(bDoubleClick!=TRUE)
	{
		bReady=TRUE;
		return (TRUE);
	}
}
bReady=TRUE;
return (TRUE);
}

//Mausklick simulieren
void PressButton(int iButton,int iDouble)
{

	memset(&event, 0x00, sizeof(event));
	
	XQueryPointer(display, RootWindow(display, DefaultScreen(display)), &event.xbutton.root, &event.xbutton.window, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
	
	event.xbutton.subwindow = event.xbutton.window;
	
	while(event.xbutton.subwindow)
	{
		event.xbutton.window = event.xbutton.subwindow;
		XQueryPointer(display, event.xbutton.window, &event.xbutton.root, &event.xbutton.subwindow, &event.xbutton.x_root, &event.xbutton.y_root, &event.xbutton.x, &event.xbutton.y, &event.xbutton.state);
	}

	
//sendpointer_enter_or_leave(event.xbutton.window, EnterNotify);
	//sendpointer_enter_or_leave(event.xbutton.subwindow, EnterNotify);

	SendPressButton(iButton,iDouble,PointerWindow);
	
//sendpointer_enter_or_leave(event.xbutton.subwindow, LeaveNotify);
//sendpointer_enter_or_leave(event.xbutton.window, LeaveNotify);	
	XFlush(display);

}	
	
 

int init_mouse(void)
{
GdkSeat *gdk_seat_display;
	
	if(bMouse==TRUE)
		return TRUE;
	//Default Screen muß nicht der sein auf dem der Program-changer läuft
	gdk_display = gdk_display_get_default ();
	gdk_screen = gdk_screen_get_default ();
	
   
	gdk_seat_display=gdk_display_get_default_seat (gdk_display);
	gdk_pointer=gdk_seat_get_pointer (gdk_seat_display);
	
	display = XOpenDisplay(NULL);
	root_window = RootWindow(display, DefaultScreen(display));

	if(display == NULL)		
	{
		fprintf(stderr, "init_mouse: X Display kann nicht geöffnet werden\n");
		bMouse=FALSE;
		return FALSE;
	}

	//Maus in Bildmitte Positionieren
	iXpos=iScreenWidth/2;
	iYpos=iScreenHeight/6;
    gdk_device_warp(gdk_pointer, gdk_screen, iXpos,iYpos);

	bMouse=TRUE;
	
return TRUE;
}


int deinit_mouse(void)
{
	if(bMouse==FALSE)
		return TRUE;
	
	XCloseDisplay(display);
	bMouse=FALSE;
	return TRUE;
}


void mouse_move(int iXFaktor,int iYFaktor)
{

if(iXFaktor!=0)
{
	iXpos+=iXFaktor;

	if(iXpos<0)
		iXpos=0;
	if(iXpos>iScreenWidth)
		iXpos=iScreenWidth;
}

if(iYFaktor!=0)
{
	iYpos+=iYFaktor;

	if(iYpos<0)
		iYpos=0;
	if(iYpos>iScreenHeight)
		iYpos=iScreenHeight;
}
	
    gdk_device_warp(gdk_pointer, gdk_screen,  iXpos,iYpos);
}


void mouse_click(int iButton)
{
static double dLasttime=8000;

	//Zu schnelle Tasten abfangen
	if(getTime_ms()-dLasttime>20)
	{	
		PressButton(iButton,FALSE);
		sprintf(acInfo,"Mous Click Button: %d  ",iButton);
		info_log(acInfo);
		dLasttime=getTime_ms();
	}
} 


void mouse_click_double()
{
static double dLasttime=10000;

	//Zu schnelle Tasten abfangen
	if(getTime_ms()-dLasttime>300)
	{	
		PressButton(1,TRUE);
		sprintf(acInfo,"Mous Click double Button: 1");
		info_log(acInfo);
		dLasttime=getTime_ms();
	}
} 

