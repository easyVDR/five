/** \file    serdisp_fctptr.c
  *
  * \brief   Initialisation of additional libraries and function pointers
  * \date    (C) 2010-2017
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

#include "../config.h"

#include <stdio.h>
#include <string.h>
#include <strings.h>

#include "serdisplib/serdisp_messages.h"
#include "serdisplib/serdisp_fctptr.h"

#ifdef WITH_DYNLOADING
#include <dlfcn.h>
#endif /* WITH_DYNLOADING */

static int fp_initialised    = 0;

static int available_libusb  = 0;  /* usb_*() */
static int available_pthread = 0; /* pthread_*() */
static int available_netsock = 0; /* gethostbyname, gethostbyaddr, sethostent, endhostent, ... */
static int available_libSDL  = 0;
static int available_libdlo  = 0;

static void* libhnd_pthread  = 0;
static void* libhnd_usb      = 0;
static void* libhnd_netsock1 = 0;
static void* libhnd_netsock2 = 0;
static void* libhnd_sdl      = 0;
static void* libhnd_libdlo   = 0;


#ifdef HAVE_LIBPTHREAD
int   (*fp_pthread_create)       (pthread_t* thread, const pthread_attr_t* attr, void* (*start_routine)(void *), void* arg); 
int   (*fp_pthread_join)         (pthread_t thread, void** value_ptr);
int   (*fp_pthread_cancel)       (pthread_t thread);
void  (*fp_pthread_exit)         (void *value_ptr);
int   (*fp_pthread_mutex_lock)   (pthread_mutex_t* mutex);
int   (*fp_pthread_mutex_trylock)(pthread_mutex_t* mutex);
int   (*fp_pthread_mutex_unlock) (pthread_mutex_t* mutex);
#endif /* HAVE_LIBPTHREAD */

#ifdef HAVE_LIBUSB
void            (*fp_usb_init)             (void);
usb_dev_handle* (*fp_usb_open)             (struct usb_device* dev);
int             (*fp_usb_close)            (usb_dev_handle* dev);
int             (*fp_usb_reset)            (usb_dev_handle* dev);
int             (*fp_usb_interrupt_read)   (usb_dev_handle* dev, int ep, char* bytes, int size, int timeout);
int             (*fp_usb_release_interface)(usb_dev_handle* dev, int interface);
int             (*fp_usb_find_busses)      (void);
int             (*fp_usb_find_devices)     (void);
struct usb_bus* (*fp_usb_get_busses)       (void);
int             (*fp_usb_claim_interface)  (usb_dev_handle* dev, int interface);

int             (*fp_usb_bulk_read)        (usb_dev_handle* dev, int ep, char* bytes, int size, int timeout);
int             (*fp_usb_bulk_write)       (usb_dev_handle* dev, int ep, const char* bytes, int size, int timeout);
int             (*fp_usb_control_msg)      (usb_dev_handle* dev, int requesttype, int request,
                                            int value, int index, char *bytes, int size, int timeout);
int             (*fp_usb_clear_halt)       (usb_dev_handle *dev, unsigned int ep);

int             (*fp_usb_set_altinterface) (usb_dev_handle *dev, int alternate);
int             (*fp_usb_set_configuration)(usb_dev_handle *dev, int configuration);
int             (*fp_usb_get_string_simple)(usb_dev_handle *dev, int index, char *buf, size_t buflen);

int             (*fp_usb_detach_kernel_driver_np) (usb_dev_handle* dev, int interface);
#endif /* HAVE_LIBUSB */

#ifdef HAVE_LIBSDL
int             (*fp_SDL_Init)             (uint32_t);
SDL_Surface*    (*fp_SDL_SetVideoMode)     (int, int, int, uint32_t);
char*           (*fp_SDL_GetError)         (void);
void            (*fp_SDL_WM_SetCaption)    (const char*, const char*);
int             (*fp_SDL_LockSurface)      (SDL_Surface*);
void            (*fp_SDL_UnlockSurface)    (SDL_Surface*);
int             (*fp_SDL_Flip)             (SDL_Surface*);
void            (*fp_SDL_FreeSurface)      (SDL_Surface*);
uint32_t        (*fp_SDL_MapRGB)           (SDL_PixelFormat*, uint8_t, uint8_t, uint8_t);
void            (*fp_SDL_PumpEvents)       (void);
uint8_t         (*fp_SDL_GetMouseState)    (int* x, int* y);
int             (*fp_SDL_PollEvent)        (SDL_Event*);
int             (*fp_SDL_WaitEvent)        (SDL_Event*);
void            (*fp_SDL_Quit)             (void);
#endif /* HAVE_LIBSDL */

#ifdef HAVE_LIBDLO
const char*     (*fp_dlo_strerror)         (const dlo_retcode_t);
dlo_retcode_t   (*fp_dlo_init)             (const dlo_init_t);
dlo_retcode_t   (*fp_dlo_final)            (const dlo_final_t);
dlo_dev_t       (*fp_dlo_lookup_device)    (struct usb_device*);
dlo_devlist_t*  (*fp_dlo_enumerate_devices)(void);
dlo_dev_t       (*fp_dlo_claim_first_device)(const dlo_claim_t, const uint32_t);
dlo_dev_t       (*fp_dlo_claim_device)     (const dlo_dev_t, const dlo_claim_t, const uint32_t);
dlo_retcode_t   (*fp_dlo_release_device)   (const dlo_dev_t);
dlo_devinfo_t*  (*fp_dlo_device_info)      (const dlo_dev_t);
dlo_mode_t*     (*fp_dlo_get_mode)         (const dlo_dev_t);
dlo_retcode_t   (*fp_dlo_set_mode)         (const dlo_dev_t, const dlo_mode_t*);
dlo_retcode_t   (*fp_dlo_fill_rect)        (const dlo_dev_t, const dlo_view_t*, const dlo_rect_t*, const dlo_col32_t);
dlo_retcode_t   (*fp_dlo_copy_host_bmp)    (const dlo_dev_t, const dlo_bmpflags_t,
                                            const dlo_fbuf_t*, const dlo_view_t*, const dlo_dot_t*);
#endif /* HAVE_LIBDLO */

#ifdef HAVE_NETSOCK_LIBS
int             (*fp_accept)               (int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int             (*fp_bind)                 (int sockfd, const struct sockaddr *my_addr, socklen_t addrlen);
int             (*fp_connect)              (int sockfd, const struct sockaddr *serv_addr, socklen_t addrlen);
int             (*fp_listen)               (int sockfd, int backlog);
ssize_t         (*fp_recv)                 (int s, void *buf, size_t len, int flags);
ssize_t         (*fp_send)                 (int s, const void *buf, size_t len, int flags);
int             (*fp_setsockopt)           (int s, int level, int optname, const void *optval, socklen_t optlen);
int             (*fp_shutdown)             (int s, int how);
int             (*fp_socket)               (int domain, int type, int protocol);

void            (*fp_endhostent)           (void);
struct hostent* (*fp_gethostbyaddr)        (const void *addr, int len, int type);
struct hostent* (*fp_gethostbyname)        (const char *name);
int             (*fp_getnameinfo)          (const struct sockaddr *sa, socklen_t salen,
                                            char *host, size_t hostlen,
                                            char *serv, size_t servlen, int flags);

uint32_t        (*fp_htonl)                (uint32_t hostlong);
uint16_t        (*fp_htons)                (uint16_t hostshort);
uint32_t        (*fp_ntohl)                (uint32_t netlong);
uint16_t        (*fp_ntohs)                (uint16_t netshort);
in_addr_t       (*fp_inet_addr)            (const char *cp);
#endif /* HAVE_NETSOCK_LIBS */

/* some define to ease life */
#define dlcheck_continue(_symbol, _wosupport) { \
   if (dlerror()) { \
       sd_debug(1, "SDFCTPTR_init(): %s not found. continuing without %s-support", (_symbol), (_wosupport)); \
       err = 1; \
   } \
 }


#ifdef WITH_DYNLOADING
static void* SDFCTPTR_dlsym_mult(const char* symbol, void* libhnd1, void* libhnd2);
#endif


/**
  * \brief   initialises additional function pointers
  *
  * additional function pointers are initialised when calling this function.
  * if dynamic loading of libraries is used: dlopen/dlsym are used for initialising the function pointers,
  * defines are used if additional libraries are to be linked directly to serdisplib.
  *
  * \attention  this function will only be executed once. further calls will be ignored.
  *
  * \since 1.98.0
  */
void SDFCTPTR_init(void) {
#ifdef WITH_DYNLOADING
  int err = 0;
#endif

  if (fp_initialised)
    return;

  fp_initialised = 1;

  available_libusb   = 0;
  available_pthread  = 0;
  available_netsock  = 0;
  available_libSDL   = 0;
  available_libdlo   = 0;

  libhnd_pthread  = 0;
  libhnd_usb      = 0;
  libhnd_netsock1 = 0;  /* glibc or libsocket - depending on operating system */
  libhnd_netsock2 = 0;  /* glibc, libsocket, libnsl, or libnet .... - depending on operating system */
  libhnd_sdl      = 0;
  libhnd_libdlo   = 0;

#ifdef HAVE_LIBPTHREAD
 #ifdef WITH_DYNLOADING
  /* dynamically load libpthread using dlopen() & co. */

  libhnd_pthread = dlopen("libpthread.so", RTLD_LAZY);
  if(!libhnd_pthread) {  /* try /usr/local/lib */
    libhnd_pthread = dlopen("/usr/local/lib/libpthread.so", RTLD_LAZY);
  }
#ifdef FALLBACK_SONAME_libpthread
  if(!libhnd_pthread) {  /* try fallback SONAME that was installed when compiling */
    libhnd_pthread = dlopen(FALLBACK_SONAME_libpthread, RTLD_LAZY);
  }
#endif

  if(!libhnd_pthread) { /* libpthread seems not to be installed */
    sd_debug(1, "%s(): libpthread seems not to be present or could not be found. continuing without pthread-support", __func__);
  } else {

    dlerror(); /* clear error code */

    fp_pthread_create = (int(*)(pthread_t*, const pthread_attr_t*, void*(*)(void*), void*)) dlsym(libhnd_pthread, "pthread_create");
    dlcheck_continue("pthread_create()", "pthread");

    if (!err) {
      fp_pthread_join = (int(*)(pthread_t, void**)) dlsym(libhnd_pthread, "pthread_join");
      dlcheck_continue("pthread_join()", "pthread");
    }

    if (!err) {
      fp_pthread_cancel = (int(*)(pthread_t)) dlsym(libhnd_pthread, "pthread_cancel");
      dlcheck_continue("pthread_cancel()", "pthread");
    }

    if (!err) {
      fp_pthread_exit = (void(*)(void*)) dlsym(libhnd_pthread, "pthread_exit");
      dlcheck_continue("pthread_exit()", "pthread");
    }

    if (!err) {
      fp_pthread_mutex_lock = (int(*)(pthread_mutex_t* mutex)) dlsym(libhnd_pthread, "pthread_mutex_lock");
      dlcheck_continue("pthread_mutex_lock()", "pthread");
    }

    if (!err) {
      fp_pthread_mutex_trylock = (int(*)(pthread_mutex_t* mutex)) dlsym(libhnd_pthread, "pthread_mutex_trylock");
      dlcheck_continue("pthread_mutex_trylock()", "pthread");
    }

    if (!err) {
      fp_pthread_mutex_unlock = (int(*)(pthread_mutex_t* mutex)) dlsym(libhnd_pthread, "pthread_mutex_unlock");
      dlcheck_continue("pthread_mutex_unlock()", "pthread");
    }

    if (err) {
      fp_pthread_create = 0;
      fp_pthread_join = 0;
      fp_pthread_cancel = 0;
      fp_pthread_exit = 0;
      fp_pthread_mutex_lock = 0;
      fp_pthread_mutex_trylock = 0;
      fp_pthread_mutex_unlock = 0;
      dlclose(libhnd_pthread);
      libhnd_pthread = 0;
      sd_debug(1, "%s(): not all needed libpthread-functions could be loaded. continuing without pthread-support", __func__);
    } else {
      available_pthread = 1;
      sd_debug(2, "%s(): libpthread and all required functions found", __func__);
    }
  }
 #else /* ! WITH_DYNLOADING */
  fp_pthread_create        = pthread_create;
  fp_pthread_join          = pthread_join;
  fp_pthread_cancel        = pthread_cancel;
  fp_pthread_exit          = pthread_exit;
  fp_pthread_mutex_lock    = pthread_mutex_lock;
  fp_pthread_mutex_trylock = pthread_mutex_trylock;
  fp_pthread_mutex_unlock  = pthread_mutex_unlock;

  available_pthread = 1;
 #endif /* WITH_DYNLOADING */
#endif /* HAVE_LIBPTHREAD */

#ifdef HAVE_LIBUSB
 #ifdef WITH_DYNLOADING
  dlerror(); /* clear error code */

  libhnd_usb = dlopen("libusb.so", RTLD_LAZY);
  if(!libhnd_usb) {  /* try /usr/local/lib */
    libhnd_usb = dlopen("/usr/local/lib/libusb.so", RTLD_LAZY);
  }
#ifdef FALLBACK_SONAME_libusb
  if(!libhnd_usb) {  /* try fallback SONAME that was installed when compiling */
    libhnd_usb = dlopen(FALLBACK_SONAME_libusb, RTLD_LAZY);
  }
#endif

  if(!libhnd_usb) { /* libusb seems not to be installed */
    sd_debug(1, "%s(): libusb seems not to be present or could not be found. continuing without libusb-support", __func__);
  } else {

    err = 0;
    dlerror(); /* clear error code */

    fp_usb_detach_kernel_driver_np = (int(*)(usb_dev_handle*, int)) dlsym(libhnd_usb, "usb_detach_kernel_driver_np");
    if (dlerror()) {
      sd_debug(1, "%s(): usb_detach_kernel_driver_np() not supported by this version of libusb", __func__);
      sd_debug(1, "                 either upgrade libusb to at least version 0.8 (only when using linux) or you will");
      sd_debug(1, "                 have to detach kernel modules by yourself.");
      fp_usb_detach_kernel_driver_np = 0;
    }
    dlerror();

    fp_usb_init = (void(*)(void)) dlsym(libhnd_usb, "usb_init");
    dlcheck_continue("usb_init()", "libusb");

    if (!err) {
      fp_usb_open = (struct usb_dev_handle*(*)(struct usb_device*)) dlsym(libhnd_usb, "usb_open");
      dlcheck_continue("usb_open()", "libusb");
    }

    if (!err) {
      fp_usb_close = (int(*)(usb_dev_handle*)) dlsym(libhnd_usb, "usb_close");
      dlcheck_continue("usb_close()", "libusb");
    }

    if (!err) {
      fp_usb_reset = (int(*)(usb_dev_handle*)) dlsym(libhnd_usb, "usb_reset");
      dlcheck_continue("usb_reset()", "libusb");
    }

    if (!err) {
      fp_usb_interrupt_read = (int(*)(usb_dev_handle*, int, char*, int, int)) dlsym(libhnd_usb, "usb_interrupt_read");
      dlcheck_continue("usb_interrupt_read()", "libusb");
    }

    if (!err) {
      fp_usb_release_interface = (int(*)(usb_dev_handle*, int)) dlsym(libhnd_usb, "usb_release_interface");
      dlcheck_continue("usb_release_interface()", "libusb");
    }

    if (!err) {
      fp_usb_find_busses = (int(*)(void)) dlsym(libhnd_usb, "usb_find_busses");
      dlcheck_continue("usb_find_busses()", "libusb");
    }

    if (!err) {
      fp_usb_find_devices = (int(*)(void)) dlsym(libhnd_usb, "usb_find_devices");
      dlcheck_continue("usb_find_devices()", "libusb");
    }

    if (!err) {
      fp_usb_get_busses = (struct usb_bus*(*)(void)) dlsym(libhnd_usb, "usb_get_busses");
      dlcheck_continue("usb_get_busses()", "libusb");
    }

    if (!err) {
      fp_usb_claim_interface = (int(*)(usb_dev_handle*, int)) dlsym(libhnd_usb, "usb_claim_interface");
      dlcheck_continue("usb_claim_interface()", "libusb");
    }

    if (!err) {
      fp_usb_bulk_read = (int(*)(usb_dev_handle*, int, char*, int, int)) dlsym(libhnd_usb, "usb_bulk_read");
      dlcheck_continue("usb_bulk_read()", "libusb");
    }

    if (!err) {
      fp_usb_bulk_write = (int(*)(usb_dev_handle*, int, const char*, int, int)) dlsym(libhnd_usb, "usb_bulk_write");
      dlcheck_continue("usb_bulk_write()", "libusb");
    }

    if (!err) {
      fp_usb_control_msg = (int(*)(usb_dev_handle*, int, int, int,int, char*, int, int)) dlsym(libhnd_usb, "usb_control_msg");
      dlcheck_continue("usb_control_msg()", "libusb");
    }

    if (!err) {
      fp_usb_clear_halt = (int(*)(usb_dev_handle*, unsigned int ep)) dlsym(libhnd_usb, "usb_clear_halt");
      dlcheck_continue("usb_clear_halt()", "libusb");
    }

    if (!err) {
      fp_usb_set_altinterface = (int(*)(usb_dev_handle*, int)) dlsym(libhnd_usb, "usb_set_altinterface");
      dlcheck_continue("usb_set_altinterface()", "libusb");
    }

    if (!err) {
      fp_usb_set_configuration = (int(*)(usb_dev_handle*, int)) dlsym(libhnd_usb, "usb_set_configuration");
      dlcheck_continue("usb_set_configuration()", "libusb");
    }

    if (!err) {
      fp_usb_get_string_simple = (int(*)(usb_dev_handle*, int, char*, size_t)) dlsym(libhnd_usb, "usb_get_string_simple");
      dlcheck_continue("usb_get_string_simple()", "libusb");
    }


    if (err) {
      fp_usb_init               = 0;
      fp_usb_open               = 0;
      fp_usb_close              = 0;
      fp_usb_interrupt_read     = 0;
      fp_usb_release_interface  = 0;
      fp_usb_find_busses        = 0;
      fp_usb_find_devices       = 0;
      fp_usb_get_busses         = 0;
      fp_usb_claim_interface    = 0;

      fp_usb_bulk_read          = 0;
      fp_usb_bulk_write         = 0;
      fp_usb_control_msg        = 0;

      fp_usb_clear_halt         = 0;
      dlclose(libhnd_usb);
      libhnd_usb = 0;
      sd_debug(1, "%s(): not all needed libusb-functions could be loaded. continuing without usb-support", __func__);
    } else {
      available_libusb = 1;
      sd_debug(2, "%s(): libusb and all required functions found", __func__);
    }
  }
 #else /* ! WITH_DYNLOADING */
  fp_usb_init                    = usb_init;
  fp_usb_open                    = usb_open;
  fp_usb_close                   = usb_close;
  fp_usb_reset                   = usb_reset;
  fp_usb_interrupt_read          = usb_interrupt_read;
  fp_usb_release_interface       = usb_release_interface;
  fp_usb_find_busses             = usb_find_busses;
  fp_usb_find_devices            = usb_find_devices;
  fp_usb_get_busses              = usb_get_busses;
  fp_usb_claim_interface         = usb_claim_interface;

  fp_usb_bulk_read               = usb_bulk_read;
  fp_usb_bulk_write              = usb_bulk_write;
  fp_usb_control_msg             = usb_control_msg;
  fp_usb_clear_halt              = usb_clear_halt;

  fp_usb_set_altinterface        = usb_set_altinterface;
  fp_usb_set_configuration       = usb_set_configuration;
  fp_usb_get_string_simple       = usb_get_string_simple;

  #ifdef LIBUSB_HAS_DETACH_KERNEL_DRIVER_NP
    fp_usb_detach_kernel_driver_np = usb_detach_kernel_driver_np;
  #else
    fp_usb_detach_kernel_driver_np = 0;
  #endif

  available_libusb = 1;
 #endif /* WITH_DYNLOADING */
#endif /* HAVE_LIBUSB */

#ifdef HAVE_LIBSDL
 #ifdef WITH_DYNLOADING
  err = 0;
  dlerror(); /* clear error code */

  libhnd_sdl = dlopen("libSDL.so", RTLD_LAZY);
  if(!libhnd_sdl) {  /* try /usr/local/lib */
    libhnd_sdl = dlopen("/usr/local/lib/libSDL.so", RTLD_LAZY);
  }
#ifdef FALLBACK_SONAME_libSDL
  if(!libhnd_sdl) {  /* try fallback SONAME that was installed when compiling */
    libhnd_sdl = dlopen(FALLBACK_SONAME_libSDL, RTLD_LAZY);
  }
#endif

  if(!libhnd_sdl) { /* libpthread seems not to be installed */
    sd_debug(1, "%s(): libSDL seems not to be present or could not be found.", __func__);
  } else {
    err = 0;
    dlerror(); /* clear error code */

    fp_SDL_Init = (int(*)(uint32_t)) dlsym(libhnd_sdl, "SDL_Init");
    dlcheck_continue("SDL_Init()", "libSDL");

    if (!err) {
      fp_SDL_SetVideoMode = (SDL_Surface*(*)(int,int,int,uint32_t)) dlsym(libhnd_sdl, "SDL_SetVideoMode");
      dlcheck_continue("SDL_SetVideoMode()", "libSDL");
    }

    if (!err) {
      fp_SDL_GetError = (char*(*)(void)) dlsym(libhnd_sdl, "SDL_GetError");
      dlcheck_continue("SDL_GetError()", "libSDL");
    }

    if (!err) {
      fp_SDL_WM_SetCaption = (void(*)(const char*, const char*)) dlsym(libhnd_sdl, "SDL_WM_SetCaption");
      dlcheck_continue("SDL_WM_SetCaption()", "libSDL");
    }

    if (!err) {
      fp_SDL_LockSurface = (int(*)(SDL_Surface*)) dlsym(libhnd_sdl, "SDL_LockSurface");
      dlcheck_continue("SDL_LockSurface()", "libSDL");
    }

    if (!err) {
      fp_SDL_UnlockSurface = (void(*)(SDL_Surface*)) dlsym(libhnd_sdl, "SDL_UnlockSurface");
      dlcheck_continue("SDL_UnlockSurface()", "libSDL");
    }

    if (!err) {
      fp_SDL_Flip = (int(*)(SDL_Surface*)) dlsym(libhnd_sdl, "SDL_Flip");
      dlcheck_continue("SDL_Flip()", "libSDL");
    }

    if (!err) {
      fp_SDL_FreeSurface = (void(*)(SDL_Surface*)) dlsym(libhnd_sdl, "SDL_FreeSurface");
      dlcheck_continue("SDL_FreeSurface()", "libSDL");
    }

    if (!err) {
      fp_SDL_MapRGB = (uint32_t(*)(SDL_PixelFormat*,uint8_t,uint8_t,uint8_t)) dlsym(libhnd_sdl, "SDL_MapRGB");
      dlcheck_continue("SDL_MapRGB()", "libSDL");
    }

    if (!err) {
      fp_SDL_PumpEvents = (void(*)(void)) dlsym(libhnd_sdl, "SDL_PumpEvents");
      dlcheck_continue("SDL_PumpEvents()", "libSDL");
    }

    if (!err) {
      fp_SDL_GetMouseState = (uint8_t(*)(int*, int*)) dlsym(libhnd_sdl, "SDL_GetMouseState");
      dlcheck_continue("SDL_GetMouseState()", "libSDL");
    }

    if (!err) {
      fp_SDL_PollEvent = (int(*)(SDL_Event*)) dlsym(libhnd_sdl, "SDL_PollEvent");
      dlcheck_continue("SDL_PollEvent()", "libSDL");
    }

    if (!err) {
      fp_SDL_WaitEvent = (int(*)(SDL_Event*)) dlsym(libhnd_sdl, "SDL_WaitEvent");
      dlcheck_continue("SDL_WaitEvent()", "libSDL");
    }

    if (!err) {
      fp_SDL_Quit = (void(*)(void)) dlsym(libhnd_sdl, "SDL_Quit");
      dlcheck_continue("SDL_Quit()", "libSDL");
    }

    if (err) {
      fp_SDL_Init               = 0;
      fp_SDL_SetVideoMode       = 0;
      fp_SDL_GetError           = 0;
      fp_SDL_WM_SetCaption      = 0;
      fp_SDL_LockSurface        = 0;
      fp_SDL_UnlockSurface      = 0;
      fp_SDL_Flip               = 0;
      fp_SDL_FreeSurface        = 0;
      fp_SDL_MapRGB             = 0;
      fp_SDL_PumpEvents         = 0;
      fp_SDL_GetMouseState      = 0;
      fp_SDL_PollEvent          = 0;
      fp_SDL_WaitEvent          = 0;
      fp_SDL_Quit               = 0;
      dlclose(libhnd_sdl);
      libhnd_sdl = 0;
      sd_debug(1, "%s(): not all needed libSDL-functions could be loaded.", __func__);
    } else {
      available_libSDL = 1;
      sd_debug(2, "%s(): libSDL and all required functions found", __func__);
      /*serdisp_directgfx_internal_getStruct(dd)->libhnd_sdl = libhnd_sdl;*/
    }
  }
 #else /* ! WITH_DYNLOADING */
  fp_SDL_Init                    = (int(*)(uint32_t)) SDL_Init;
  fp_SDL_SetVideoMode            = (SDL_Surface*(*)(int, int, int, uint32_t)) SDL_SetVideoMode;
  fp_SDL_GetError                = (char*(*)(void)) SDL_GetError;
  fp_SDL_WM_SetCaption           = (void(*)(const char*, const char*)) SDL_WM_SetCaption;
  fp_SDL_LockSurface             = (int(*)(SDL_Surface*)) SDL_LockSurface;
  fp_SDL_UnlockSurface           = (void(*)(SDL_Surface*)) SDL_UnlockSurface;
  fp_SDL_Flip                    = (int(*)(SDL_Surface*)) SDL_Flip;
  fp_SDL_FreeSurface             = (void(*)(SDL_Surface*)) SDL_FreeSurface;
  fp_SDL_MapRGB                  = (uint32_t(*)(SDL_PixelFormat*, uint8_t, uint8_t, uint8_t)) SDL_MapRGB;
  fp_SDL_PumpEvents              = (void(*)(void)) SDL_PumpEvents;
  fp_SDL_GetMouseState           = (uint8_t(*)(int*, int*)) SDL_GetMouseState;
  fp_SDL_PollEvent               = (int(*)(SDL_Event*)) SDL_PollEvent;
  fp_SDL_WaitEvent               = (int(*)(SDL_Event*)) SDL_WaitEvent;
  fp_SDL_Quit                    = (void(*)(void)) SDL_Quit;

  available_libSDL = 1;
 #endif /* WITH_DYNLOADING */
#endif /* HAVE_LIBSDL */

#ifdef HAVE_LIBDLO
 #ifdef WITH_DYNLOADING
  err = 0;
  dlerror(); /* clear error code */

  libhnd_libdlo = dlopen("libdlo.so", RTLD_LAZY);
  if(!libhnd_libdlo) {  /* try /usr/local/lib */
    libhnd_libdlo = dlopen("/usr/local/lib/libdlo.so", RTLD_LAZY);
  }

  if(!libhnd_libdlo) { /* libdlo seems not to be installed */
    sd_debug(1, "%s(): libdlo seems not to be present or could not be found.", __func__);
  } else {
    err = 0;
    dlerror(); /* clear error code */

    fp_dlo_strerror = (const char*(*)(const dlo_retcode_t)) dlsym(libhnd_libdlo, "dlo_strerror");
    dlcheck_continue("dlo_stderror()", "libdlo");

    if (!err) {
      fp_dlo_init = (dlo_retcode_t(*)(const dlo_init_t)) dlsym(libhnd_libdlo, "dlo_init");
      dlcheck_continue("dlo_init()", "libdlo");
    }

    if (!err) {
      fp_dlo_final = (dlo_retcode_t(*)(const dlo_final_t)) dlsym(libhnd_libdlo, "dlo_final");
      dlcheck_continue("dlo_final()", "libdlo");
    }

#if 0
    if (!err) {
      fp_dlo_lookup_device = (dlo_dev_t(*)(struct usb_device*)) dlsym(libhnd_libdlo, "dlo_lookup_device");
      dlcheck_continue("dlo_lookup_device()", "libdlo");
    }
#endif

    if (!err) {
      fp_dlo_enumerate_devices = (dlo_devlist_t*(*)(void)) dlsym(libhnd_libdlo, "dlo_enumerate_devices");
      dlcheck_continue("dlo_enumerate_devices()", "libdlo");
    }

    if (!err) {
      fp_dlo_claim_first_device = (dlo_dev_t(*)(const dlo_claim_t, const uint32_t)) dlsym(libhnd_libdlo, "dlo_claim_first_device");
      dlcheck_continue("dlo_claim_first_device()", "libdlo");
    }

    if (!err) {
      fp_dlo_claim_device = (dlo_dev_t(*)(const dlo_dev_t, const dlo_claim_t, const uint32_t)) dlsym(libhnd_libdlo, "dlo_claim_device");
      dlcheck_continue("dlo_claim_device()", "libdlo");
    }

    if (!err) {
      fp_dlo_release_device = (dlo_retcode_t(*)(const dlo_dev_t)) dlsym(libhnd_libdlo, "dlo_release_device");
      dlcheck_continue("dlo_release_device()", "libdlo");
    }

    if (!err) {
      fp_dlo_device_info = (dlo_devinfo_t*(*)(const dlo_dev_t)) dlsym(libhnd_libdlo, "dlo_device_info");
      dlcheck_continue("dlo_device_info()", "libdlo");
    }

    if (!err) {
      fp_dlo_get_mode = (dlo_mode_t*(*)(const dlo_dev_t)) dlsym(libhnd_libdlo, "dlo_get_mode");
      dlcheck_continue("dlo_get_mode()", "libdlo");
    }

    if (!err) {
      fp_dlo_set_mode = (dlo_retcode_t(*)(const dlo_dev_t,const dlo_mode_t*)) dlsym(libhnd_libdlo, "dlo_set_mode");
      dlcheck_continue("dlo_set_mode()", "libdlo");
    }

    if (!err) {
      fp_dlo_fill_rect =
       (dlo_retcode_t(*)(const dlo_dev_t, const dlo_view_t*, const dlo_rect_t*, const dlo_col32_t))
        dlsym(libhnd_libdlo, "dlo_fill_rect");
      dlcheck_continue("dlo_fill_rect()", "libdlo");
    }

    if (!err) {
      fp_dlo_copy_host_bmp =
        (dlo_retcode_t(*)(const dlo_dev_t,const dlo_bmpflags_t,const dlo_fbuf_t*,const dlo_view_t*, const dlo_dot_t*))
        dlsym(libhnd_libdlo, "dlo_copy_host_bmp");
      dlcheck_continue("dlo_copy_host_bmp()", "libdlo");
    }

    if (err) {
      fp_dlo_strerror           = 0;
      fp_dlo_init               = 0;
      fp_dlo_final              = 0;
#if 0
      fp_dlo_lookup_device      = 0;
#endif
      fp_dlo_enumerate_devices  = 0;
      fp_dlo_claim_first_device = 0;
      fp_dlo_claim_device       = 0;
      fp_dlo_release_device     = 0;
      fp_dlo_device_info        = 0;
      fp_dlo_get_mode           = 0;
      fp_dlo_set_mode           = 0;
      fp_dlo_fill_rect          = 0;
      fp_dlo_copy_host_bmp      = 0;
      dlclose(libhnd_libdlo);
      libhnd_libdlo = 0;
      sd_debug(1, "%s(): not all needed libdlo-functions could be loaded.", __func__);
    } else {
      available_libdlo = 1;
      sd_debug(2, "%s(): libdlo and all required functions found", __func__);
    }
  }
 #else /* ! WITH_DYNLOADING */
  fp_dlo_strerror                = (const char*(*)(const dlo_retcode_t)) dlo_strerror;
  fp_dlo_init                    = (dlo_retcode_t(*)(const dlo_init_t)) dlo_init;
  fp_dlo_final                   = (dlo_retcode_t(*)(const dlo_final_t)) dlo_final;
#if 0
  fp_dlo_lookup_device           = (dlo_dev_t(*)(struct usb_device*)) dlo_lookup_device;
#endif
  fp_dlo_enumerate_devices       = (dlo_devlist_t*(*)(void)) dlo_enumerate_devices;
  fp_dlo_claim_first_device      = (dlo_dev_t(*)(const dlo_claim_t, const uint32_t)) dlo_claim_first_device;
  fp_dlo_claim_device            = (dlo_dev_t(*)(const dlo_dev_t, const dlo_claim_t, const uint32_t)) dlo_claim_device;
  fp_dlo_release_device          = (dlo_retcode_t(*)(const dlo_dev_t)) dlo_release_device;
  fp_dlo_device_info             = (dlo_devinfo_t*(*)(const dlo_dev_t)) dlo_device_info;
  fp_dlo_get_mode                = (dlo_mode_t*(*)(const dlo_dev_t)) dlo_get_mode;
  fp_dlo_set_mode                = (dlo_retcode_t(*)(const dlo_dev_t, const dlo_mode_t*)) dlo_set_mode;
  fp_dlo_fill_rect               = (dlo_retcode_t(*)
                                    (const dlo_dev_t, const dlo_view_t*, const dlo_rect_t*, const dlo_col32_t
                                    ) ) dlo_fill_rect;
  fp_dlo_copy_host_bmp           = (dlo_retcode_t(*)
                                    (const dlo_dev_t, const dlo_bmpflags_t, const dlo_fbuf_t*,
                                     const dlo_view_t*, const dlo_dot_t*
                                    ) ) dlo_copy_host_bmp;
  available_libdlo = 1;
 #endif /* WITH_DYNLOADING */
#endif /* HAVE_LIBDLO */

#ifdef HAVE_NETSOCK_LIBS
  if (sizeof(int) == sizeof(socklen_t)) { /* usually sizeof(int) == sizeof(socklen_t). if not: disable netsock-stuff */
 #ifdef WITH_DYNLOADING
    int found = 0;
    int i;
    char* test_netsock1[] = {NULL, "libsocket.so", "/usr/local/lib/libsocket.so"};
    char* test_netsock2[] = {NULL, "libsocket.so", "/usr/local/lib/libsocket.so", "libnsl.so", "libnet.so"};
#if 0
    int (*fp_socket)   (int, int, int);
    void (*fp_endhostent) (void);
#endif

    i = 0;
    while (!found && i < sizeof(test_netsock1)/sizeof(char*)) {
      /* initialising function pointers for all socket functions */
      libhnd_netsock1 = dlopen(test_netsock1[i], RTLD_LAZY);
      dlerror();
      fp_socket = (int(*)(int, int, int)) dlsym(libhnd_netsock1, "socket");
      if (dlerror())
        i++;
      else
        found = 1;
    }
    if (!found) {
      sd_debug(1, "%s(): socket() not found. continuing without socket and netdb support.", __func__);
    } else {
      sd_debug(2, "%s(): socket() & co. found in %s", __func__, ((test_netsock1[i]) ? test_netsock1[i] : "main program"));

      err = 0;
      dlerror(); /* clear error code */

      fp_accept = (int(*)(int, struct sockaddr*, socklen_t*)) SDFCTPTR_dlsym_mult("accept", libhnd_netsock1, libhnd_netsock2);
      dlcheck_continue("accept()", "netsock");

      if (!err) {
        fp_bind = (int(*)(int, const struct sockaddr*, socklen_t)) SDFCTPTR_dlsym_mult("bind", libhnd_netsock1, libhnd_netsock2);
        dlcheck_continue("bind()", "netsock");
      }

      if (!err) {
        fp_connect = (int(*)(int, const struct sockaddr*, socklen_t)) SDFCTPTR_dlsym_mult("connect", libhnd_netsock1, libhnd_netsock2);
        dlcheck_continue("connect()", "netsock");
      }

      if (!err) {
        fp_listen = (int(*)(int, int)) SDFCTPTR_dlsym_mult("listen", libhnd_netsock1, libhnd_netsock2);
        dlcheck_continue("listen()", "netsock");
      }

      if (!err) {
        fp_recv = (ssize_t(*)(int, void*, size_t, int)) SDFCTPTR_dlsym_mult("recv", libhnd_netsock1, libhnd_netsock2);
        dlcheck_continue("recv()", "netsock");
      }

      if (!err) {
        fp_send = (ssize_t(*)(int, const void*, size_t, int)) SDFCTPTR_dlsym_mult("send", libhnd_netsock1, libhnd_netsock2);
        dlcheck_continue("send()", "netsock");
      }

      if (!err) {
        fp_setsockopt = (int(*)(int, int, int, const void*, socklen_t)) SDFCTPTR_dlsym_mult("setsockopt", libhnd_netsock1, libhnd_netsock2);
        dlcheck_continue("setsockopt()", "netsock");
      }

      if (!err) {
        fp_shutdown = (int(*)(int, int)) SDFCTPTR_dlsym_mult("shutdown", libhnd_netsock1, libhnd_netsock2);
        dlcheck_continue("shutdown()", "netsock");
      }

      if (!err) {
        found = 0;
        i = 0;
        while (!found && i < sizeof(test_netsock2)/sizeof(char*)) {
          /* initialising function pointers for all socket functions */
          libhnd_netsock2 = dlopen(test_netsock2[i], RTLD_LAZY);
          dlerror();
          fp_endhostent = (void(*)(void)) dlsym(libhnd_netsock2, "endhostent");
          if (dlerror())
            i++;
          else
            found = 1;
        }

        if (!found) {
          sd_debug(1, "%s(): endhostent() not found. continuing without socket and netdb support.", __func__);
        } else {
          sd_debug(2, "%s(): endhostent() & co. found in %s", __func__, ((test_netsock2[i]) ? test_netsock2[i] : "main program"));

          err = 0;
          dlerror(); /* clear error code */

          fp_gethostbyaddr = (struct hostent*(*)(const void*, int, int)) SDFCTPTR_dlsym_mult("gethostbyaddr", libhnd_netsock2, libhnd_netsock1);
          dlcheck_continue("gethostbyaddr()", "netsock");

          if (!err) {
            fp_gethostbyname = (struct hostent*(*)(const char*)) SDFCTPTR_dlsym_mult("gethostbyname", libhnd_netsock2, libhnd_netsock1);
            dlcheck_continue("gethostbyname()", "netsock");
          }

          if (!err) {
            fp_getnameinfo = (int(*)(const struct sockaddr*, socklen_t, char*, size_t, char *, size_t, int))
                             SDFCTPTR_dlsym_mult("getnameinfo", libhnd_netsock2, libhnd_netsock1);
            dlcheck_continue("getnameinfo()", "netsock");
          }

          if (!err) {
            fp_htonl = (uint32_t(*)(uint32_t)) SDFCTPTR_dlsym_mult("htonl", libhnd_netsock2, libhnd_netsock1);
            dlcheck_continue("htonl()", "netsock");
          }

          if (!err) {
            fp_htons = (uint16_t(*)(uint16_t)) SDFCTPTR_dlsym_mult("htons", libhnd_netsock2, libhnd_netsock1);
            dlcheck_continue("htons()", "netsock");
          }

          if (!err) {
            fp_ntohl = (uint32_t(*)(uint32_t)) SDFCTPTR_dlsym_mult("ntohl", libhnd_netsock2, libhnd_netsock1);
            dlcheck_continue("ntohl()", "netsock");
          }

          if (!err) {
            fp_ntohs = (uint16_t(*)(uint16_t)) SDFCTPTR_dlsym_mult("ntohs", libhnd_netsock2, libhnd_netsock1);
            dlcheck_continue("ntohs()", "netsock");
          }

          if (!err) {
            fp_inet_addr = (in_addr_t(*)(const char*)) SDFCTPTR_dlsym_mult("inet_addr", libhnd_netsock2, libhnd_netsock1);
            dlcheck_continue("inet_addr()", "netsock");
          }

          if (!err)
            available_netsock = 1;
        }
      }
    }
 #else /* ! WITH_DYNLOADING */
    fp_accept                      = (int(*)(int, struct sockaddr*, socklen_t*)) accept;
    fp_bind                        = (int(*)(int, const struct sockaddr*, socklen_t)) bind;
    fp_connect                     = (int(*)(int, const struct sockaddr*, socklen_t)) connect;
    fp_listen                      = (int(*)(int, int)) listen;
    fp_recv                        = (ssize_t(*)(int, void*, size_t, int)) recv;
    fp_send                        = (ssize_t(*)(int, const void*, size_t, int)) send;
    fp_setsockopt                  = (int(*)(int, int, int, const void*, socklen_t)) setsockopt;
    fp_shutdown                    = (int(*)(int, int)) shutdown;
    fp_socket                      = (int(*)(int, int, int)) socket;

    fp_endhostent                  = (void(*)(void)) endhostent;
    fp_gethostbyaddr               = (struct hostent*(*)(const void*, int, int)) gethostbyaddr;
    fp_gethostbyname               = (struct hostent*(*)(const char*)) gethostbyname;
    fp_getnameinfo                 = (int(*)(const struct sockaddr*, socklen_t, char*, size_t, char *, size_t, int)) getnameinfo;

    fp_htonl                       = (uint32_t(*)(uint32_t)) htonl;
    fp_htons                       = (uint16_t(*)(uint16_t)) htons;
    fp_ntohl                       = (uint32_t(*)(uint32_t)) ntohl;
    fp_ntohs                       = (uint16_t(*)(uint16_t)) ntohs;
    fp_inet_addr                   = (in_addr_t(*)(const char*)) inet_addr;

    available_netsock = 1;
 #endif /* WITH_DYNLOADING */
  } else {
    sd_debug(1, "%s(): sizeof(int) != sizeof(socklen_t). continuing without socket and netdb support.", __func__);
    available_netsock = 0;
  }
#endif /* WITH_NETSOCK_LIBS */
}


/**
  * \brief   checks if an additional functionality is supported
  *
  * \param   libID          one of SDFCTPTR_[LIBUSB | PTHREAD | NETSOCK | LIBSDL | LIBDLO]
  *
  * \retval 0   additional functionality is not supported
  * \retval 1   additional functionality is supported
  *
  * \since 1.98.0
  */
int SDFCTPTR_checkavail(int libID) {
  switch (libID) {
    case SDFCTPTR_LIBUSB:  return (available_libusb == 1); break;
    case SDFCTPTR_PTHREAD: return (available_pthread == 1); break;
    case SDFCTPTR_NETSOCK: return (available_netsock == 1); break;
    case SDFCTPTR_LIBSDL:  return (available_libSDL == 1); break;
    case SDFCTPTR_LIBDLO:  return (available_libdlo == 1); break;
  }
  return 0;
}


/**
  * \brief   closes all open dyn. loaded libraries
  *
  * \since 1.98.0
  */
void SDFCTPTR_cleanup  (void) {
#ifdef WITH_DYNLOADING
  if (libhnd_pthread) {
    dlclose(libhnd_pthread);
    libhnd_pthread = 0;
  }
  if (libhnd_usb) {
    dlclose(libhnd_usb);
    libhnd_usb = 0;
  }
  if (libhnd_netsock1) {
    dlclose(libhnd_netsock1);
    libhnd_netsock1 = 0;
  }
  if (libhnd_netsock2) {
    dlclose(libhnd_netsock2);
    libhnd_netsock2 = 0;
  }
  if (libhnd_sdl) {
    dlclose(libhnd_sdl);
    libhnd_sdl = 0;
  }
  if (libhnd_libdlo) {
    dlclose(libhnd_libdlo);
    libhnd_libdlo = 0;
  }
#endif
}

#ifdef WITH_DYNLOADING
void* SDFCTPTR_dlsym_mult(const char* symbol, void* libhnd1, void* libhnd2) {
  void* retval = 0;
  retval = dlsym(libhnd1, symbol);
  if (dlerror()) {
    retval = dlsym(libhnd2, symbol);
  }
  return retval;
}
#endif
