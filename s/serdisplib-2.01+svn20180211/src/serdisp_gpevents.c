/** \file    serdisp_gpevents.c
  *
  * \brief   Definitions and functions for general purpose items and event handling
  * \date    (C) 2006-2014
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
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "serdisplib/serdisp_gpevents.h"
#include "serdisplib/serdisp_connect.h"
#include "serdisplib/serdisp_control.h"
#include "serdisplib/serdisp_messages.h"
#include "serdisplib/serdisp_tools.h"


#include <sys/types.h>          /* standard system types        */
#include <fcntl.h>

#include "serdisplib/serdisp_proccmd.h"

#include "serdisplib/serdisp_fctptr.h"

#ifdef HAVE_LIBPTHREAD
/* prototypes for functions with local-only scope */
static void* SDEVLP_eventloop(serdisp_t* dd);
#endif /* HAVE_LIBPTHREAD */


#ifdef HAVE_LIBPTHREAD
/**
  * \brief   asynchronous event loop
  *
  * \param   dd            device descriptor
  *
  * \return  ignored (always NULL - return type void* is required by pthread_create())
  *
  * \since   1.98.0
  */
void* SDEVLP_eventloop(serdisp_t* dd) {
  SDGP_event_t recycle;   /* reusable event */
  SDGP_event_t* event = 0;
#if 0
  int failcount = 0;
#endif
  dd->gpevset->evlp_status = SDEVLP_RUNNING;

  struct sockaddr_in  saddr;            /* Internet address struct */
  struct sockaddr_in  caddr;            /* client's address struct */
  int rc;
  /*int dtable_size = 0;*/
  fd_set              rfd;            /* set of open sockets */
  /*fd_set              c_rfd;*/          /* set of sockets waiting to be read */
  unsigned int        size_caddr;
  int                 sockrcv;

  char                buf[256];
  char                msgbuf[8192];


  /* auto-enable GPIs where enable is preset with 1 */
  if (dd->gpevset->fp_hnd_gpi_enable) {
    int i;
    for (i = 0; i < SDGPI_getamount(dd); i++)
      if (dd->gpevset->gpis[i].enabled)  { /* while initialisaion gpis[i].enable shows if GPI should be started automatically or not */
        dd->gpevset->gpis[i].enabled = 0;  /* from now on use gpis[i].enable as status information */
        SDGPI_enable(dd,i, 1);
      }
  }

  /* open command processor */
  if (dd->gpevset->cmdproc_port) {
     memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = fp_htons(dd->gpevset->cmdproc_port);
    saddr.sin_addr.s_addr = fp_inet_addr("127.0.0.1") /* INADDR_ANY */;
    dd->gpevset->cmdproc_sock = fp_socket(AF_INET, SOCK_STREAM, 0);
    sd_debug(2, "%s(): socket created for command processor. socket ID: %d", __func__, saddr.sin_addr.s_addr);
    if (dd->gpevset->cmdproc_sock < 0) {
      dd->gpevset->cmdproc_port = 0;
      sd_debug(0, "%s(): cannot create socket for command processor, cause: %s", __func__, strerror(errno));
    } else {
      int optval = 1;
      /* try to reuse socket */
      fp_setsockopt(dd->gpevset->cmdproc_sock, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));
      /* bind and listen */
      if (fp_bind(dd->gpevset->cmdproc_sock, (struct sockaddr*) &saddr, sizeof(saddr)) ||
          fp_listen(dd->gpevset->cmdproc_sock, 5)
         ) {
        dd->gpevset->cmdproc_port = 0;
        sd_debug(0, "%s(): cannot bind to command processor or listen() is unsuccessful , cause: %s", __func__, strerror(errno));
        close(dd->gpevset->cmdproc_sock);
        dd->gpevset->cmdproc_sock = -1;;
      } else {
        sd_debug(2, "%s(): bind() and listen() to command processor successful. socket ID: %d", __func__, saddr.sin_addr.s_addr);
        size_caddr = sizeof(caddr);
        /*dtable_size = getdtablesize();*/
        /* set non blocking */
        fcntl( dd->gpevset->cmdproc_sock, F_SETFL, fcntl(dd->gpevset->cmdproc_sock, F_GETFL) |O_NONBLOCK );

        FD_ZERO(&rfd);
        FD_SET(dd->gpevset->cmdproc_sock, &rfd);
      }
    }

  }


  /* loop until thread gets stopped */
  while( dd->gpevset->evlp_status != SDEVLP_STOPPED ) {

    /* reset recycle event */
    memset(&recycle, 0, sizeof(SDGP_event_t));
    event = &recycle;

    if (dd->gpevset->fp_evlp_receiver)
      event = dd->gpevset->fp_evlp_receiver(dd, event);

    if (event && dd->gpevset->fp_evlp_trigevents)
      dd->gpevset->fp_evlp_trigevents(dd, event);

    if (dd->gpevset->fp_evlp_schedevents)
      dd->gpevset->fp_evlp_schedevents(dd);

    /* loop through event listeners */
    if (event && dd->gpevset->eventlistener_chain) {
      SDGP_eventlistener_chain_t* curr;

      curr = dd->gpevset->eventlistener_chain;
      while (curr) {
        curr->eventlistener(dd, event);
        curr = curr->next;
      }
    }

    /* look if some command is waiting for command processor */
    if (dd->gpevset->cmdproc_port && dd->gpevset->cmdproc_sock >= 0) {
      /*c_rfd = rfd;*/
      /*rc = select(dtable_size, &c_rfd, NULL, NULL, (struct timeval *)NULL);*/

      sockrcv = fp_accept (dd->gpevset->cmdproc_sock, (struct sockaddr*)&caddr, &size_caddr);
      if (sockrcv < 0 && errno != EAGAIN) {
        sd_debug(0, "%s(): accept failed, cause: %s", __func__, strerror(errno));
      } else if (sockrcv >= 0) {
        FD_SET(sockrcv, &rfd);
        rc = read(sockrcv, buf, sizeof(buf));
        if (rc == 0) {
          close(sockrcv);
          FD_CLR(sockrcv, &rfd);
        } else {
          /*printf("cmd: %.*s\n", rc, buf);*/
          buf[rc-1] = '\0';

          /* crop carriage return / linefeed (when using telnet) */
          if (buf[rc-2] == '\r' || buf[rc-2] == '\f')
            buf[rc-2] = '\0';

          SDCMD_proccmd(dd, buf, msgbuf, sizeof(msgbuf), 1);
          write(sockrcv, msgbuf, strlen(msgbuf));

          close(sockrcv);
          FD_CLR(sockrcv, &rfd);
        }
      }
    }

    if (dd->gpevset->fp_evlp_receiver)  /* if no receiver (scheduled events only): sleep for 100ns */
      usleep(100); 

#if 0
    if (sd_runtime_error()) {
      sleep(1);
      failcount++;
fprintf(stderr, "failcount: %d\n", failcount);
      if (failcount > 20) {
fprintf(stderr, "resetting ... \n");
        serdisp_fullreset(dd);
        dd->gpevset->evlp_status = SDEVLP_STOPPED;
      }
    }
#endif
  }

  /* close command processor */
  if (dd->gpevset->cmdproc_port && dd->gpevset->cmdproc_sock >= 0) {
    /*shutdown(dd->gpevset->cmdproc_sock, SHUT_RDWR);*/
    close(dd->gpevset->cmdproc_sock);
    FD_CLR(dd->gpevset->cmdproc_sock, &rfd);
  }

  /* disable all enabled GPIs */
  if (dd->gpevset->fp_hnd_gpi_enable) {
    int i;
    for (i = 0; i < SDGPI_getamount(dd); i++)
      if (SDGPI_isenabled(dd, i))
        SDGPI_enable(dd, i, 0);
  }

  return 0;
}
#endif /* HAVE_LIBPTHREAD */


#ifdef HAVE_LIBPTHREAD
/**
  * \brief   starts event loop of device dd
  *
  * \param   dd            device descriptor
  *
  * \retval  0      successful start
  * \retval  1      eventloop already running
  * \retval  2      no event functions given for dd
  * \retval  3      auto start disabled
  * \retval <0      unsuccessful start
  *
  * \since   1.98.0
  */
int SDEVLP_start (serdisp_t* dd) {
  int err;

  if (! SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) || !dd || !dd->gpevset)
    return -1;

  /* if evlp_noautostart is set by a driver:
     don't autostart at the very first call of SDEVLP_start() (called in serdisp_control.c) */
  if (dd->gpevset->evlp_noautostart) {
    dd->gpevset->evlp_noautostart = 0;
    return 3;
  }

  /* if neither a receiver-function, nor a triggered events fct, nor a scheduled events fct, 
     nor a command processor port is defined: don't start */
  if ( !(dd->gpevset->fp_evlp_receiver || dd->gpevset->fp_evlp_trigevents || dd->gpevset->fp_evlp_schedevents ||
         dd->gpevset->cmdproc_port) )
    return 2;

  if (SDEVLP_getstatus(dd) == SDEVLP_RUNNING) {
    sd_debug(1, "%s(): event loop for device '%s' is already running.", __func__, serdisp_getdisplayname(dd));
    return 1;
  }

  err = fp_pthread_create(&dd->gpevset->evlp_thread, 0, (void*)(SDEVLP_eventloop), dd);
  if (err) {
    sd_error(SERDISP_ERUNTIME, "%s(): could not create event loop thread for device '%s'.", __func__, serdisp_getdisplayname(dd));
    dd->gpevset->evlp_status = SDEVLP_STOPPED;
    return -1;
  }

  sd_debug(1, "%s(): event loop for device '%s' started successfully.", __func__, serdisp_getdisplayname(dd));
  return 0;
}
#endif /* HAVE_LIBPTHREAD */


#ifdef HAVE_LIBPTHREAD
/**
  * \brief   stops event loop of device dd
  *
  * \param   dd            device descriptor
  *
  * \retval  0      successful stop
  * \retval  1      eventloop already stopped
  * \retval <0      unsuccessful stop
  *
  * \since   1.98.0
  */
int SDEVLP_stop (serdisp_t* dd) {
  int err/*, i*/;

  if (! SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) || !dd || !dd->gpevset)
    return -1;

  if (SDEVLP_getstatus(dd) == SDEVLP_STOPPED) {
    sd_debug(1, "%s(): event loop for device '%s' is already stopped.", __func__, serdisp_getdisplayname(dd));
    return 1;
  }

  dd->gpevset->evlp_status = SDEVLP_STOPPED;

  /* cancel thread before joining it (else read threads where timeout==disabled might never finish!) */
  fp_pthread_cancel(dd->gpevset->evlp_thread);
  err = fp_pthread_join(dd->gpevset->evlp_thread, 0);
  if (err) {
    sd_error(SERDISP_ERUNTIME, "%s(): could not stop thread from device '%s'.", __func__, serdisp_getdisplayname(dd));
    return -1;
  }

#if 0
  for (i = 0; i < SDGPI_getamount(dd); i++) {
    /*close(gpi->fd_pipe[0]);*/
  }
#endif

  sd_debug(1, "%s(): event loop for device '%s' stopped successfully.", __func__, serdisp_getdisplayname(dd));

  return 0;  
}
#endif /* HAVE_LIBPTHREAD */


#ifdef HAVE_LIBPTHREAD
/**
  * \brief   status of event loop
  *
  * \param   dd            device descriptor
  *
  * \return  event loop status: one of <tt>SDREVLP_STOPPED</tt>, <tt>SDREVLP_RUNNING</tt>
  *
  * \since   1.98.0
  */
int SDEVLP_getstatus (serdisp_t* dd) {
  if (! SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) || !dd || !dd->gpevset)
    return SDEVLP_STOPPED;

  return dd->gpevset->evlp_status;
}
#endif /* HAVE_LIBPTHREAD */


/* GPOs **************************************/

/**
  * \brief   gets amount of general purpose outputs (GPOs) available for device dd
  *
  * \param   dd            device descriptor
  *
  * \return  amount of available GPOs
  *
  * \since   1.98.0
  */
byte SDGPO_getamount (serdisp_t* dd) {
  if (!dd || !dd->gpevset || !dd->gpevset->gpos)
    return 0;
  return dd->gpevset->amountgpos;
}


/**
  * \brief   gets type of a GPO
  *
  * \param   dd            device descriptor
  * \param   gpid          GPO id
  *
  * \return  type of GPO
  *
  * \since   1.98.0
  */
byte SDGPO_gettype (serdisp_t* dd, byte gpid) {
  if (!dd || !dd->gpevset || !dd->gpevset->gpos || gpid >= dd->gpevset->amountgpos)
    return 0xFF;
  return dd->gpevset->gpos[gpid].type;
}


/**
  * \brief   gets GPO descriptor
  *
  * \param   dd            device descriptor
  * \param   gpid          GPO id
  *
  * \return  GPO descriptor
  *
  * \since   1.98.0
  */
SDGPO_t* SDGPO_getdescriptor (serdisp_t* dd, byte gpid) {
  if (!dd || !dd->gpevset || !dd->gpevset->gpos || gpid >= dd->gpevset->amountgpos)
    return (SDGPO_t*)0;
  return &(dd->gpevset->gpos[gpid]);
}


/**
  * \brief   searches a GPO by name
  *
  * \param   dd            device descriptor
  * \param   gpname        name of GPO
  *
  * \retval    0xFF        GPO with name \em gpname not found
  * \retval  <>0xFF        id of GPO
  *
  * \since   1.98.0
  */
byte SDGPO_search (serdisp_t* dd, const char* gpname) {
  int i;
  char* tempptr;

  if (!dd || !dd->gpevset || !dd->gpevset->gpos)
    return 0xFF;

  /* test if gpname contains numeric id */
  i = strtol(gpname, &tempptr, 10);
  /* verify if gpname contained a valid number */
  if (! (gpname == tempptr || ( (*tempptr != '\0') && (tempptr < (gpname + strlen(gpname))) ) ))
    return ((i < dd->gpevset->amountgpos) ? i : 0xFF);

  i = 0;
  while (i < dd->gpevset->amountgpos) {
    if ( (strcasecmp(dd->gpevset->gpos[i].name, gpname) == 0 || sdtools_isinelemlist(dd->gpevset->gpos[i].aliasnames, gpname, -1) > -1 ) )    
      return i;
    i++;
  }
  return 0xFF;
}


/**
  * \brief   sets a value of a numeric GPO
  *
  * \param   dd            device descriptor
  * \param   gpid          GPO id
  * \param   value         new value
  *
  * \retval   0            operation successful
  * \retval  <0            error
  *
  * \since   1.98.0
  */
int SDGPO_setvalue(serdisp_t* dd, byte gpid, int32_t value) {
  int rangeerr = 0;

  if (!dd || !dd->gpevset || !dd->gpevset->gpos || gpid >= dd->gpevset->amountgpos || !dd->gpevset->fp_hnd_gpo_value)
    return -SERDISP_ENXIO;

  if (gpid >= dd->gpevset->amountgpos) {
    sd_error(SERDISP_ENXIO, "gpid '%d' out of bounds", gpid);
    return -SERDISP_ENXIO;
  }

  /* range-checks */
  switch (dd->gpevset->gpos[gpid].type) {
    case SDGPT_BOOL:
    case SDGPT_INVBOOL:
      if (value <0 || value > 2)  /* min/maxval are ignored; value == 2 => invert */
        rangeerr = 1;
      break;
    case SDGPT_VALUE:
      if ( ((uint32_t)value < (uint32_t) dd->gpevset->gpos[gpid].minval) || 
           ((uint32_t)value > (uint32_t) dd->gpevset->gpos[gpid].maxval) )
         rangeerr = 1;
      break;
    case SDGPT_SGNVALUE:
      if ( (value < dd->gpevset->gpos[gpid].minval) || 
           (value > dd->gpevset->gpos[gpid].maxval) )
         rangeerr = 1;
      break;
  }

  if (rangeerr) {
    sd_error(SERDISP_ERANGE, "value '%d (0x%08x)' for GPO '%d' out of range", value, value, gpid);
    return -SERDISP_ERANGE;
  }

  return dd->gpevset->fp_hnd_gpo_value(dd, gpid, value);
}


/**
  * \brief   inverts a boolean GPO
  *
  * \param   dd            device descriptor
  * \param   gpid          GPO id
  *
  * \retval   0            operation successful
  * \retval  <0            error
  *
  * \since   1.98.0
  */
int SDGPO_invert(serdisp_t* dd, byte gpid) {
  if (!dd || !dd->gpevset || !dd->gpevset->gpos || gpid >= dd->gpevset->amountgpos || dd->gpevset->gpos[gpid].type > SDGPT_INVBOOL)
    return -1;

  return SDGPO_setvalue(dd, gpid, 2);
}


/**
  * \brief   puts a data package to a GPO
  *
  * \param   dd            device descriptor
  * \param   gpid          GPO id
  * \param   data          byte buffer
  * \param   length        length of byte buffer
  *
  * \retval   0            operation successful
  * \retval  <0            error
  *
  * \since   1.98.0
  */
int SDGPO_setpackage(serdisp_t* dd, byte gpid, byte* data, int32_t length) {
  if (!dd || !dd->gpevset || !dd->gpevset->gpos || gpid >= dd->gpevset->amountgpos || !dd->gpevset->fp_hnd_gpo_package)
    return -1;

  return dd->gpevset->fp_hnd_gpo_package(dd, gpid, data, length);
}



#ifdef HAVE_LIBPTHREAD
/* GPIs **************************************/

/**
  * \brief   gets amount of general purpose inputs (GPIs) available for device dd
  *
  * \param   dd            device descriptor
  *
  * \return  amount of available GPIs
  *
  * \since   1.98.0
  */
byte SDGPI_getamount (serdisp_t* dd) {
  if (!dd || !dd->gpevset || !dd->gpevset->gpis)
    return 0;
  return dd->gpevset->amountgpis;
}


/**
  * \brief   gets type of a GPI
  *
  * \param   dd            device descriptor
  * \param   gpid          GPI id
  *
  * \return  type of GPI
  *
  * \since   1.98.0
  */
byte SDGPI_gettype (serdisp_t* dd, byte gpid) {
  if (!dd || !dd->gpevset || !dd->gpevset->gpis || gpid >= dd->gpevset->amountgpis)
    return 0xFF;
  return dd->gpevset->gpis[gpid].type;
}


/**
  * \brief   gets GPI descriptor
  *
  * \param   dd            device descriptor
  * \param   gpid          GPI id
  *
  * \return  GPI descriptor
  *
  * \since   1.98.0
  */
SDGPI_t* SDGPI_getdescriptor (serdisp_t* dd, byte gpid) {
  if (!dd || !dd->gpevset || !dd->gpevset->gpis || gpid >= dd->gpevset->amountgpis)
    return (SDGPI_t*)0;
  return &(dd->gpevset->gpis[gpid]);
}


/**
  * \brief   tests if GPI is enabled
  *
  * \param   dd            device descriptor
  * \param   gpid          GPI id
  *
  * \retval  1             GPI is enabled
  * \retval  <>1           GPI is disabled
  *
  * \since   1.98.0
  */
int SDGPI_isenabled (serdisp_t* dd, byte gpid) {
  if (!dd || !dd->gpevset || !dd->gpevset->gpis || gpid >= dd->gpevset->amountgpis || !dd->gpevset->fp_hnd_gpi_enable)
    return 0;
  return dd->gpevset->gpis[gpid].enabled;
}


/**
  * \brief   enables or displabes a GPI
  *
  * \param   dd            device descriptor
  * \param   gpid          GPI id
  * \param   enable        enable (1) or disable (0) GPO
  *
  * \retval   0            successfully enabled/disabled
  * \retval   1            GPI already enabled/disabled
  * \retval  <0            unsuccessful
  *
  * \since   1.98.0
  */
int SDGPI_enable (serdisp_t* dd, byte gpid, int enable) {
  int rc;

  if (!dd || !dd->gpevset || !dd->gpevset->gpis || gpid >= dd->gpevset->amountgpis || !dd->gpevset->fp_hnd_gpi_enable)
    return -1;

  if (SDGPI_isenabled(dd, gpid) == enable) {
    sd_debug(1, "%s(): GPI '%s' already %s.", __func__, dd->gpevset->gpis[gpid].name, ((enable) ? "enabled" : "disabled"));
    return 1;
  }

  rc = dd->gpevset->fp_hnd_gpi_enable(dd, gpid, enable);

  sd_debug(1, "%s(): GPI '%s' %s %ssuccessfully.", __func__, dd->gpevset->gpis[gpid].name, ((enable) ? "enabled" : "disabled"), ((rc) ? "un" : ""));

  if (!rc) {
    dd->gpevset->gpis[gpid].enabled = ((enable) ? 1 : 0);
  }
  return rc;
}


/**
  * \brief   gets file pointer containing the streaming input
  *
  * \param   dd            device descriptor
  * \param   gpid          GPI id
  *
  * \retval    0           unable to retrieve file pointer
  * \retval   >0           file pointer of streaming input
  *
  * \since   1.98.0
  */
int SDGPI_getstreamfd (serdisp_t* dd, byte gpid) {
  if (!dd || !dd->gpevset || !dd->gpevset->gpis || gpid >= dd->gpevset->amountgpis )
    return 0;

  return dd->gpevset->gpis[gpid].fd[0];
}


/**
  * \brief   searches a GPI by name
  *
  * \param   dd            device descriptor
  * \param   gpname        name of GPI
  *
  * \retval    0xFF        GPI with name \em gpname not found
  * \retval  <>0xFF        id of GPI
  *
  * \since   1.98.0
  */
byte SDGPI_search (serdisp_t* dd, const char* gpname) {
  int i;
  char* tempptr;

  if (!dd || !dd->gpevset || !dd->gpevset->gpis)
    return 0xFF;

  /* test if gpname contains numeric id */
  i = strtol(gpname, &tempptr, 10);
  /* verify if gpname contained a valid number */
  if (! (gpname == tempptr || ( (*tempptr != '\0') && (tempptr < (gpname + strlen(gpname))) ) ))
    return ((i < dd->gpevset->amountgpis) ? i : 0xFF);

  i = 0;
  while (i < dd->gpevset->amountgpis) {
    if ( (strcasecmp(dd->gpevset->gpis[i].name, gpname) == 0 || sdtools_isinelemlist(dd->gpevset->gpis[i].aliasnames, gpname, -1) > -1 ) )    
      return i;
    i++;
  }
  return 0xFF;
}
#endif /* HAVE_LIBPTHREAD */

#ifdef HAVE_LIBPTHREAD
/**
  * \brief   add an event listener for a GPI
  *
  * \param   dd            device descriptor
  * \param   gpid          GPI id
  * \param   eventlistener eventlistener (function pointer) to be added
  *
  * \retval   0            successfully added eventlistener
  * \retval   1            eventlistener already available
  * \retval  <0            unsuccessful
  *
  * \since   1.98.0
  */
int SDEVLP_add_listener  (struct serdisp_s* dd, byte gpid, fp_eventlistener_t eventlistener) {
  SDGP_eventlistener_chain_t* curr;
  SDGP_eventlistener_chain_t* prev = 0;

  if (!dd || !dd->gpevset)
    return -1;

  curr = dd->gpevset->eventlistener_chain;
  while (curr) {
    if (curr->eventlistener == eventlistener && curr->gpid == gpid)
      return 1;
    prev = curr;
    curr = curr->next;
  }
  if (! (curr = (SDGP_eventlistener_chain_t*) sdtools_malloc( sizeof(SDGP_eventlistener_chain_t)) )) {
    sd_debug(0, "%s(): cannot allocate chain entry for event listener. ignoring it ...", __func__);
  } else {
    curr->eventlistener = eventlistener;
    curr->gpid = gpid;
    curr->next = 0;
    if (prev) {
      prev->next = curr;
    } else {
      dd->gpevset->eventlistener_chain = curr;
    }
  }
  return 0;
}


/**
  * \brief   delete an event listener defined for a GPI
  *
  * \param   dd            device descriptor
  * \param   gpid          GPI id
  * \param   eventlistener eventlistener (function pointer) to be deleted
  *
  * \retval   0            successfully deleted eventlistener
  * \retval   1            eventlistener not in list
  * \retval  <0            unsuccessful
  *
  * \since   1.98.0
  */
int SDEVLP_del_listener  (struct serdisp_s* dd, byte gpid, fp_eventlistener_t eventlistener) {
  SDGP_eventlistener_chain_t* curr;
  SDGP_eventlistener_chain_t* prev = 0;

  if (!dd || !dd->gpevset)
    return -1;

  curr = dd->gpevset->eventlistener_chain;
  while (curr) {
    if (curr->eventlistener == eventlistener && curr->gpid == gpid) {
      if (!prev) {
        dd->gpevset->eventlistener_chain = curr->next;
      } else {
        prev->next = curr->next;
      }
      free(curr);
      return 0;
    }
    prev = curr;
    curr = curr->next;
  }
  return 1;
}


/**
  * \brief   purge all event listeners defined for a GPI
  *
  * \param   dd            device descriptor
  * \param   gpid          GPI id (0xFF -> purge all listeners)
  *
  * \retval   0            successfully purged eventlisteners
  * \retval  <0            unsuccessful
  *
  * \since   1.98.0
  */
int SDEVLP_purge_listeners  (struct serdisp_s* dd, byte gpid) {
  SDGP_eventlistener_chain_t* curr;
  SDGP_eventlistener_chain_t* prev = 0;

  if (!dd || !dd->gpevset)
    return -1;

  curr = dd->gpevset->eventlistener_chain;
  while (curr) {
    if (gpid == 0xFF && curr->gpid == gpid) {
      if (!prev) {
        dd->gpevset->eventlistener_chain = curr->next;
      } else {
        prev->next = curr->next;
      }
      free(curr);
      curr = (prev) ? prev->next : dd->gpevset->eventlistener_chain;
    } else {
      prev = curr;
      curr = curr->next;
    }
  }
  return 0;
}


/**
  * \brief   count number of event listeners defined for a GPI
  *
  * \param   dd            device descriptor
  * \param   gpid          GPI id (0xFF -> count all listeners)
  *
  * \retval   >=0          number of event listeners in chain
  * \retval  <0            unsuccessful
  *
  * \since   1.98.0
  */
int SDEVLP_count_listeners  (struct serdisp_s* dd, byte gpid) {
  SDGP_eventlistener_chain_t* curr;
  int cnt = 0;

  if (!dd || !dd->gpevset)
    return -1;

  curr = dd->gpevset->eventlistener_chain;
  while (curr) {
    if (gpid == 0xFF || curr->gpid == gpid)
      cnt ++;
    curr = curr->next;
  }
  return cnt;
}
#endif /* HAVE_LIBPTHREAD */


#ifdef HAVE_LIBPTHREAD

/* event decode and encode **************************************/

/**
  * \brief   converts an event header from host byte order to network byte order
  *
  * \param   event         event header
  *
  * \since   1.98.0
  */
void      SDGPT_event_header_hton   (struct SDGP_event_s* event) {
  if (SDGPT_GETCATEGORY(event->type) == SDGPT_CATEGORYVALUE)
    event->value = (int32_t) fp_htonl((uint32_t) event->value);
  else
    event->length = (uint16_t) fp_htons((uint16_t) event->length);
  event->timestamp.tv_sec = (time_t) fp_htonl((uint32_t) event->timestamp.tv_sec);
  event->timestamp.tv_usec = (long) fp_htonl((uint32_t) event->timestamp.tv_usec);
}


/**
  * \brief   converts an event header from network byte order to host byte order
  *
  * \param   event         event header
  *
  * \since   1.98.0
  */
void      SDGPT_event_header_ntoh   (struct SDGP_event_s* event) {
  if (SDGPT_GETCATEGORY(event->type) == SDGPT_CATEGORYVALUE)
    event->value = (int32_t) fp_ntohl((uint32_t) event->value);
  else
    event->length = (uint16_t) fp_ntohs((uint16_t) event->length);
  event->timestamp.tv_sec = (time_t) fp_ntohl((uint32_t) event->timestamp.tv_sec);
  event->timestamp.tv_usec = (long) fp_ntohl((uint32_t) event->timestamp.tv_usec);
}


/**
  * \brief   converts an event payload from host byte order to network byte order
  *
  * \param   payload         event payload
  * \param   bytes           amount of event payload data
  * \param   word_length     word length of a payload data
  *
  * \since   1.98.0
  */
void      SDGPT_event_payload_hton  (void* payload, int bytes, byte word_length) {
  int i;

  switch (word_length) {
   case 2:
    for (i = 0; i < (bytes >> 1); i++)
      ((uint16_t*)payload)[i] = fp_htons( ((uint16_t*)payload)[i] );
    break;
   case 4:
    for (i = 0; i < (bytes >> 2); i++)
      ((uint32_t*)payload)[i] =  fp_htonl( ((uint32_t*)payload)[i] );
    break;
    /* silently ignore all other word_length (0, 1: byte, others: erraneous or not supported) */
  }
}


/**
  * \brief   converts an event payload from network byte order to host byte order
  *
  * \param   payload         event payload
  * \param   bytes           amount of event payload data
  * \param   word_length     word length of a payload data
  *
  * \since   1.98.0
  */
void      SDGPT_event_payload_ntoh  (void* payload, int bytes, byte word_length) {
  int i;

  switch (word_length) {
   case 2:
    for (i = 0; i < (bytes >> 1); i++)
      ((uint16_t*)payload)[i] = fp_ntohs( ((uint16_t*)payload)[i] );
    break;
   case 4:
    for (i = 0; i < (bytes >> 2); i++)
      ((uint32_t*)payload)[i] =  fp_ntohl( ((uint32_t*)payload)[i] );
    break;
    /* silently ignore all other word_length (0, 1: byte, others: erraneous or not supported) */
  }
}

#endif /* HAVE_LIBPTHREAD */


/* TOUCHDEVICES **************************************/
#if defined(__linux__) && defined(HAVE_LIBPTHREAD)

/**
  * \brief   openes an idev-based touch device
  *
  * \param   idev_name       device name
  * \param   touchdev        pointer to touchdevice struct
  *
  * \retval   0              opened successfully
  * \retval  <0              unsuccessful
  *
  * \since   2.00
  */
int  SDTOUCH_idev_open  (const char* idev_name, SDTOUCH_idev_touchdevice_t* touchdev) {
  /* only one GPI (touchscreen) is supported: so activate events only if touchscreen device is set */
  if (touchdev->fd == -1) {
    if((touchdev->fd = open(idev_name, O_RDONLY)) != -1) {
      struct input_absinfo iabs;
      int err = 0;

      /* grab touchscreen exclusively */
      if (ioctl (touchdev->fd, EVIOCGRAB, 1) >= 0) {
        int32_t minrawx, minrawy, maxrawx, maxrawy;

        touchdev->currtype = SDGPT_TOUCHUP;

        err = ioctl (touchdev->fd, EVIOCGABS (ABS_X), &iabs) < 0;
        if (!err) {
          minrawx = iabs.minimum;
          maxrawx = iabs.maximum;
          err = ioctl (touchdev->fd, EVIOCGABS (ABS_Y), &iabs) < 0;
          if (!err) {
            minrawy = iabs.minimum;
            maxrawy = iabs.maximum;
          }
        }

        if (!err) {
          int32_t raww = maxrawx - minrawx;
          int32_t rawh = maxrawy - minrawy;

          /* only request min/max-info if not set from outside */
          if ( ! touchdev->minrawx ) touchdev->minrawx = minrawx;
          if ( ! touchdev->minrawy ) touchdev->minrawy = minrawy;
          if ( ! touchdev->maxrawx ) touchdev->maxrawx = maxrawx;
          if ( ! touchdev->maxrawy ) touchdev->maxrawy = maxrawy;

          /* bounding box and plausibility checks */
          if ( touchdev->minrawx < minrawx || ((touchdev->minrawx - minrawx) > (raww / 10)) ) {
            touchdev->minrawx = minrawx;
            sd_debug(0, "%s(): TOUCHMINX not plausible. resetting to default mininum value '%d'", __func__, minrawx);
          }
          if ( touchdev->minrawy < minrawy || ((touchdev->minrawy - minrawy) > (rawh / 10)) ) {
            touchdev->minrawy = minrawy;
            sd_debug(0, "%s(): TOUCHMINY not plausible. resetting to default mininum value '%d'", __func__, minrawy);
          }
          if ( touchdev->maxrawx > maxrawx || ((maxrawx - touchdev->maxrawx) > (raww / 10)) ) {
            touchdev->maxrawx = maxrawx;
            sd_debug(0, "%s(): TOUCHMAXX not plausible. resetting to default maximum value '%d'", __func__, maxrawx);
          }
          if ( touchdev->maxrawy > maxrawy || ((maxrawy - touchdev->maxrawy) > (rawh / 10)) ) {
            touchdev->maxrawy = maxrawy;
            sd_debug(0, "%s(): TOUCHMAXY not plausible. resetting to default maximum value '%d'", __func__, maxrawy);
          }
          return 0;
        } else {
          close(touchdev->fd);
          touchdev->fd = -1;
          sd_error(SERDISP_ERUNTIME, "%s(): cannot open touchscreen (unable to request min/max info)", __func__);
        }
      } else {
        close(touchdev->fd);
        touchdev->fd = -1;
        sd_error(SERDISP_ERUNTIME, "%s(): unable to exclusively lock touchscreen",  __func__);
      }
    } else {
      sd_error(SERDISP_ERUNTIME, "%s(): cannot open touchscreen", __func__);
    }
  } else {
    sd_error(SERDISP_ERUNTIME, "%s(): touchscreen already opened", __func__);
    return -2;
  }
  return -1;
}


/**
  * \brief   closes an idev-based touch device
  *
  * \param   touchdev        pointer to touchdevice struct
  *
  * \since   2.00
  */
void SDTOUCH_idev_close (SDTOUCH_idev_touchdevice_t* touchdev) {
  if (touchdev->fd != -1) {
    ioctl(touchdev->fd, EVIOCGRAB, 0);
    close(touchdev->fd);
    touchdev->fd = -1;
  }
}


/**
  * \brief   enables or disables idev stream
  *
  * \param   touchdev        pointer to touchdevice struct
  * \param   enable          enable (1) or disable (0) touch device
  *
  * \retval   0              successfully enabled/disabled
  * \retval  <0              unsuccessful
  *
  * \since   2.00
  */
int  SDTOUCH_idev_enable  (SDTOUCH_idev_touchdevice_t* touchdev, int enable) {
    /* blocking read: only care about enabling: flush stream */
    if (enable ) {
      struct input_event         ie;
      int bytes;

      /* flush stream */
      //lseek(serdisp_framebuffer_internal_getStruct(dd)->ts_fd, 0, SEEK_END);
      fcntl(touchdev->fd, F_SETFL, fcntl(touchdev->fd, F_GETFL) |O_NONBLOCK );
      do {
        bytes = read(touchdev->fd, &ie, sizeof(struct input_event));
      } while (bytes > 0);
      fcntl(touchdev->fd, F_SETFL, fcntl(touchdev->fd, F_GETFL) &~O_NONBLOCK );
      return 0;
    }
    return 0;
}


/**
  * \brief   reads next event in line
  *
  * \param   dd              device descriptor
  * \param   touchdev        pointer to touchdevice struct
  * \param   recycle         reusable event. if == 0:  space for event is allocated and has to be freed afterwards!
  *
  * \retval    0             no event or error
  * \retval  <>0             event
  *
  * \since   2.00
  */
SDGP_event_t* SDTOUCH_idev_evlp_receiver (serdisp_t* dd, SDTOUCH_idev_touchdevice_t* touchdev, SDGP_event_t* recycle) {
  int            bytes;
  SDGP_event_t*  event = 0;
  SDGP_evpkt_generictouch_t  generictouch;
  struct input_event         ie;
  int            ev_complete = 0;

  int32_t raw_x = 0, raw_y = 0;
  int16_t x = 0, y = 0, t = 0;
  int16_t w = (int16_t)(serdisp_getwidth(dd));
  int16_t h = (int16_t)(serdisp_getheight(dd));

  /* window between maxrawx, minrawx, maxrawy, minrawy */
  int32_t winx = touchdev->maxrawx - touchdev->minrawx;
  int32_t winy = touchdev->maxrawy - touchdev->minrawy;

  /* safety exit to avoid flooding of syslog */
  if (sd_runtime_error())
    return 0;

  if (SDGPI_isenabled(dd, 0)) {

    while (! ev_complete) {
      bytes = read(touchdev->fd, &ie, sizeof(struct input_event));  /* open was initialised with BLOCKING */

      if( bytes < 0 || bytes != sizeof(struct input_event) ) {
        return (SDGP_event_t*)0;
      }

      /* 0: SYN, 1: starts up or down event, 3: get one absolute coordinate (x or y) */
      if (ie.type == 1 && ie.code == 330 ) {
        ev_complete = 0;
        t = (ie.value) ? SDGPT_TOUCHDOWN : SDGPT_TOUCHUP;
      } else if (ie.type == 3) {
        if (ie.code == 0) {
          touchdev->currrawx = ie.value;
        } else if (ie.code == 1) {
          touchdev->currrawy = ie.value;
        } /* else: ignore */
      } else if (ie.type == 0 && ev_complete == 0) {
        /* shifted raw values */
        raw_x = touchdev->currrawx - touchdev->minrawx;
        raw_y = touchdev->currrawy - touchdev->minrawy;

        ev_complete = 1;

        switch (dd->curr_rotate) {
          case 0:  /* 0 degrees */
            x = raw_x / (winx / w);
            y = raw_y / (winy / h);
          break;
          case 1:  /* 180 degrees */
            x = w - (raw_x / (winx / w));
            y = h - (raw_y / (winy / h));
          break;
          case 2:  /* 90 degrees */
            x =     (raw_y / (winy / w));
            y = h - (raw_x / (winx / h));
          break;
          default:  /* 270 degrees */
            x = w - (raw_y / (winy / w));
            y =     (raw_x / (winx / h));
        }

        if (touchdev->swapx) {
          x = w - x;
        }
        if (touchdev->swapy) {
          y = h - y;
        }

        if (t == SDGPT_TOUCHDOWN && touchdev->currtype == SDGPT_TOUCHDOWN) {
          generictouch.type = SDGPT_TOUCHMOVE;
        } else {
          generictouch.type = t;
        }
        generictouch.flags = 0x11;  /* flags: 0x_1: binary touch, 0x1_: raw unscaled x/y values included */
        generictouch.norm_x = x;
        generictouch.norm_y = y;
        generictouch.norm_touch = (t == SDGPT_TOUCHDOWN) ? 1 : 0;
        generictouch.raw_x = touchdev->currrawx;
        generictouch.raw_y = touchdev->currrawy;
        touchdev->currtype = t;

        /* if no recycle-event struct: create new one */  
        if (!recycle) {
          event = sdtools_malloc(sizeof(SDGP_event_t));
          if (!event) {
            sd_error(SERDISP_EMALLOC, "%s(): cannot allocate memory for event", __func__);
            return 0;
          }
        } else {
          event = recycle;
        }

        memset(event, 0, sizeof(SDGP_event_t));

        event->cmdid = SD_CMD_SEND_GPI;
        event->devid = 0;
        event->subid = 0; /* gpid */
        event->type = SDGPT_GENERICTOUCH;
        gettimeofday(&event->timestamp, 0);
        memcpy(&event->data, &generictouch, sizeof(SDGP_evpkt_generictouch_t));

#if 0
        /* convert header and payload to network byte order */
        SDGPT_event_header_hton(event);
        SDGPT_event_payload_hton(stream, sizeof(stream), (byte)sizeof(int16_t));


        /* convert header and payload back to host byte order */
        SDGPT_event_header_ntoh(event);
        SDGPT_event_payload_ntoh(stream, sizeof(stream), (byte)sizeof(int16_t));
#endif
        return event;
      }
    }
  }

  return 0;
}

#endif /* HAVE_LIBPTHREAD */
