/*
 * vdrdiscovery.c
 *
 * Simple broadcast protocol to search VDR with xineliboutput server
 * from (local) network.
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 *
 */

#ifndef _GNU_SOURCE
#  define _GNU_SOURCE  /* asprintf */
#endif

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>

#ifndef _WIN32
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#include <winsock2.h>
#include <ws2tcpip.h>  // socklen_t
#endif

#ifndef LOG_MODULENAME
#  define LOG_MODULENAME "[discovery] "
#  include "../logdefs.h"
#endif

#include "sock_compat.h"

#include "vdrdiscovery.h"

/*
 * constants
 */

#ifndef DISCOVERY_PORT
#  define DISCOVERY_PORT 37890
#endif

/* discovery protocol strings (v1.0) */
#define DISCOVERY_1_0_HDR     "VDR xineliboutput DISCOVERY 1.0" "\r\n"
#define DISCOVERY_1_0_CLI     "Client: %s:%d"      "\r\n"
#define DISCOVERY_1_0_SVR     "Server port: %d"    "\r\n"
#define DISCOVERY_1_0_ADDR    "Server address: %s" "\r\n"
#define DISCOVERY_1_0_VERSION "Server version: " /*vdr-" VDRVERSION "\r\n\t"*/ \
                              "xineliboutput-" XINELIBOUTPUT_VERSION "\r\n"

/*
 *
 */

static inline int discovery_init(int port)
{
  int fd_discovery = -1;
  union {
    struct sockaddr    sa;
    struct sockaddr_in in;
  } addr;

  if ((fd_discovery = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    LOGERR("discovery_init: socket() failed");
    return -1;
  }

  if (sock_set_broadcast(fd_discovery, 1) < 0)
    LOGERR("discovery_init: setsockopt(SO_BROADCAST) failed");

  if (sock_set_reuseaddr(fd_discovery, 1) < 0)
    LOGERR("discovery_init: setsockopt(SO_REUSEADDR) failed");

  addr.in.sin_family = AF_INET;
  addr.in.sin_port   = htons(port);
  addr.in.sin_addr.s_addr = htonl(INADDR_BROADCAST);

  if (bind(fd_discovery, &addr.sa, sizeof(addr)) < 0) {
    LOGERR("discovery_init: bind() failed");
    closesocket(fd_discovery);
    return -1;
  }

  return fd_discovery;
}

int udp_discovery_init(void)
{
  return discovery_init(DISCOVERY_PORT);
}

static inline int udp_discovery_send(int fd_discovery, int port, char *msg)
{
  union {
    struct sockaddr    sa;
    struct sockaddr_in in;
  } addr;
  size_t len;

  len = strlen(msg);
  if (len > DISCOVERY_MSG_MAXSIZE)
    return -1;

  addr.in.sin_family = AF_INET;
  addr.in.sin_port   = htons(port);
  addr.in.sin_addr.s_addr = INADDR_BROADCAST;

  if ((ssize_t)len != sendto(fd_discovery, msg, len, 0, &addr.sa, sizeof(addr))) {
    LOGERR("UDP broadcast send failed (discovery)");
    return -1;
  }

  //LOGDBG("UDP broadcast send succeed (discovery)");
  return 0;
}

int udp_discovery_broadcast(int fd_discovery, int server_port, const char *server_address)
{
  char *msg = NULL;
  int result;

  if(server_address && *server_address) {
    result = asprintf(&msg,
             DISCOVERY_1_0_HDR     //"VDR xineliboutput DISCOVERY 1.0" "\r\n"
             DISCOVERY_1_0_SVR     //"Server port: %d" "\r\n"
             DISCOVERY_1_0_ADDR    //"Server Address: %d.%d.%d.%d \r\n"
             DISCOVERY_1_0_VERSION //"Server version: xineliboutput-" XINELIBOUTPUT_VERSION "\r\n"
             "\r\n",
             server_port, server_address);
  } else {
    result = asprintf(&msg,
             DISCOVERY_1_0_HDR     //"VDR xineliboutput DISCOVERY 1.0" "\r\n"
             DISCOVERY_1_0_SVR     //"Server port: %d" "\r\n"
             DISCOVERY_1_0_VERSION //"Server version: xineliboutput-" XINELIBOUTPUT_VERSION "\r\n"
             "\r\n",
             server_port);
  }

  if (result >= 0) {
    result = udp_discovery_send(fd_discovery, DISCOVERY_PORT, msg);
    free(msg);
  }

  return result;
}

static inline int udp_discovery_search(int fd_discovery, int port)
{
  char *msg = NULL;
  int result;

  result = asprintf(&msg,
           DISCOVERY_1_0_HDR  /* "VDR xineliboutput DISCOVERY 1.0" "\r\n" */
           DISCOVERY_1_0_CLI  /* "Client: %s:%d" "\r\n" */
           "\r\n",
           "255.255.255.255",
           port);

  if (result >= 0) {
    result = udp_discovery_send(fd_discovery, port, msg);
    free(msg);
  }

  return result;
}

int udp_discovery_recv(int fd_discovery, char *buf, int timeout,
                       struct sockaddr *source, socklen_t *sourcelen)
{
  int err;

  struct timeval select_timeout;
  fd_set fdset;

  FD_ZERO (&fdset);
  FD_SET  (fd_discovery, &fdset);

  select_timeout.tv_sec  = 0;
  select_timeout.tv_usec = 1000 * timeout;

  err = select (fd_discovery + 1, &fdset, NULL, NULL, &select_timeout);

  if(err < 1) {
    if(err < 0)
      LOGERR("broadcast poll error");
    return err;
  }

  memset(source, 0, *sourcelen);
  memset(buf, 0, DISCOVERY_MSG_MAXSIZE);

  err = recvfrom(fd_discovery, buf, DISCOVERY_MSG_MAXSIZE-1, 0,
                 source, sourcelen);

  if(err <= 0)
    LOGDBG("fd_discovery recvfrom() error");

  return err;
}

int udp_discovery_is_valid_search(const char *buf)
{
  static const char id_string[] = { DISCOVERY_1_0_HDR "Client:" };

  if(!strncmp(id_string, buf, strlen(id_string))) {
    LOGMSG("Received valid discovery message %s", buf);
    return 1;
  }

  LOGDBG("BROADCAST: %s", buf);
  return 0;
}

/*
 * server list
 */

static void _free_server(vdr_server **p)
{
  vdr_server *s = *p;
  if (s) {
    free(s->host);
    free(s->descr);
    free(s);
    *p = NULL;
  }
}

static vdr_server *_new_server(const char *host, int port, const char *descr)
{
  vdr_server *r = (vdr_server *)calloc(1, sizeof(*r));
  if (r) {
    r->host  = strdup(host);
    r->port  = port;
    r->descr = strdup(descr ? descr : "");
  }
  return r;
}

void udp_discovery_free_servers(vdr_server ***p)
{
  vdr_server **s = *p;
  if (s) {
    for (; *s; s++) {
      _free_server(s);
    }
    free(*p);
    *p = NULL;
  }
}

static vdr_server **_add_server(vdr_server **l, vdr_server *s)
{
  size_t cnt;
  vdr_server **r;
  for (cnt = 0; l && l[cnt]; cnt++)
    ;
  r = (vdr_server **)realloc(l, sizeof(*l) * (cnt + 2));
  if (!r) {
    _free_server(&s);
    return l;
  }
  r[cnt] = s;
  r[cnt + 1] = NULL;
  return r;
}

/*
 * server search
 */

static vdr_server **_udp_discovery_find_servers(int fd_discovery, int fast)
{
  static const char mystring[] = DISCOVERY_1_0_HDR "Server port: ";
  union {
    struct sockaddr    sa;
    struct sockaddr_in in;
  } from;
  socklen_t from_len = sizeof(from);
  char buf[DISCOVERY_MSG_MAXSIZE];
  int trycount = 0;
  int err = 0;
  vdr_server **svrs = NULL;
  int port;
  char address[16];

  while(err >= 0 && ++trycount < 4) {

    if((err = udp_discovery_search(fd_discovery, DISCOVERY_PORT) >= 0)) {

      errno = 0;
      while( (err = udp_discovery_recv(fd_discovery, buf, 500, &from.sa, &from_len)) > 0) {

        uint32_t tmp = ntohl(from.in.sin_addr.s_addr);

        buf[err] = 0;
        LOGDBG("Reveived broadcast: %d bytes from %d.%d.%d.%d \n%s",
               err,
               ((tmp>>24)&0xff), ((tmp>>16)&0xff),
               ((tmp>>8)&0xff), ((tmp)&0xff),
               buf);
        if(!strncmp(mystring, buf, strlen(mystring))) {
          char *iploc;
          LOGDBG("Valid discovery message");

          // default: use broadcast source address
          sprintf(address, "%d.%d.%d.%d",
                  ((tmp>>24)&0xff), ((tmp>>16)&0xff),
                  ((tmp>>8)&0xff), ((tmp)&0xff));

          // Check if announce message includes alternative server address
          iploc = strstr(buf + strlen(mystring), "Server address: ");
          if(iploc) {
            uint32_t svraddr;
            iploc += strlen("Server address: ");
            svraddr = inet_addr(iploc);
            if(svraddr == INADDR_NONE || svraddr == INADDR_ANY) {
              LOGMSG("Server provided invalid address !");
            } else {
              svraddr = ntohl(svraddr);
              sprintf(address, "%d.%d.%d.%d",
                      ((svraddr>>24)&0xff), ((svraddr>>16)&0xff),
                      ((svraddr>>8)&0xff), ((svraddr)&0xff));
              LOGMSG("Replacing broadcast source address %d.%d.%d.%d "
                     "with server-given address %s",
                     ((tmp>>24)&0xff), ((tmp>>16)&0xff),
                     ((tmp>>8)&0xff), ((tmp)&0xff),
                     address);
            }
          }

          port = -1;
          if (1 == sscanf(buf + strlen(mystring), "%d", &port) &&
              port >= 1000 && port <= 0xffff) {

            /* valid entry */

            char *descr = strstr(buf, "Server version: ");
            if (descr) {
              descr += strlen("Server version: ");
              char *lf = strchr(descr, '\r');
              if (lf) {
                *lf = 0;
              } else {
                descr = NULL;
              }
            }

            svrs = _add_server(svrs, _new_server(address, port, descr));
            if (fast)
              return svrs;
          } else {
            LOGMSG("Server-given port is invalid !");
          }
        } else {
          LOGDBG("NOT valid discovery message");
        }
      }

      /* return if found */
      if (svrs)
        return svrs;
    }
  }

  if (err >= 0)
    return _add_server(svrs, NULL);  /* no error, return empty list */

  /* failed */
  return NULL;
}

vdr_server **udp_discovery_find_servers(int fast)
{
  int fd_discovery;
  vdr_server **ret = NULL;

  sock_init();

  if((fd_discovery = discovery_init(DISCOVERY_PORT)) >= 0) {
    ret = _udp_discovery_find_servers(fd_discovery, fast);
    closesocket(fd_discovery);
  }

  sock_cleanup();

  return ret;
}

int udp_discovery_find_server(int *port, char *address, size_t address_size)
{
  vdr_server **svrs;

  svrs = udp_discovery_find_servers(1);
  if (!svrs || !svrs[0]) {
    udp_discovery_free_servers(&svrs);
    return 0;
  }

  strncpy(address, svrs[0]->host, address_size);
  address[address_size - 1] = 0;
  *port = svrs[0]->port;

  udp_discovery_free_servers(&svrs);
  return 1;
}
