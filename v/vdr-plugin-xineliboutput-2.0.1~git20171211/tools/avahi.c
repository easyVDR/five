/*
 * avahi.c: mDNS announce
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 *
 */

#include "avahi.h"

#include "../logdefs.h"            // logging
#include "../features.h"

#include <stdlib.h>

#ifdef HAVE_AVAHI_CLIENT

#include <errno.h>
#include <stdio.h>  // snprintf
#include <string.h>

#include <pthread.h>

#include <avahi-client/client.h>
#include <avahi-client/publish.h>
#include <avahi-common/alternative.h>
#include <avahi-common/simple-watch.h>
#include <avahi-common/malloc.h>
#include <avahi-common/error.h>
#include <avahi-common/timeval.h>

typedef struct {
  AvahiEntryGroup *group;
  AvahiSimplePoll *simple_poll;

  char *name;
  int   port;

  int   rtsp;
  int   http;

  pthread_t thread;
} avahi_data;

static void _create_services(AvahiClient *c, avahi_data *d);

static void _entry_group_callback(AvahiEntryGroup *g, AvahiEntryGroupState state, void *userdata)
{
  avahi_data *d = (avahi_data *)userdata;
  d->group = g;

  switch (state) {
  case AVAHI_ENTRY_GROUP_ESTABLISHED:
    LOGMSG("AVAHI service '%s' successfully established.", d->name);
    break;

  case AVAHI_ENTRY_GROUP_COLLISION: {
    char *n;
    /* A service name collision with a remote service
     * happened. Let's pick a new name */
    n = avahi_alternative_service_name(d->name);
    avahi_free(d->name);
    d->name = n;
    LOGERR("AVAHI service name collision, renaming service to '%s'", d->name);
    _create_services(avahi_entry_group_get_client(g), d);
    break;
  }

  case AVAHI_ENTRY_GROUP_FAILURE :
    LOGERR("AVAHI entry group failure: %s", avahi_strerror(avahi_client_errno(avahi_entry_group_get_client(g))));
    avahi_simple_poll_quit(d->simple_poll);
    break;
  case AVAHI_ENTRY_GROUP_UNCOMMITED:
  case AVAHI_ENTRY_GROUP_REGISTERING:
    break;
  }
}

static void _create_services(AvahiClient *c, avahi_data *d)
{
  char *n;
  int ret;

  if (!d->group) {
    if (!(d->group = avahi_entry_group_new(c, _entry_group_callback, d))) {
      LOGERR("avahi_entry_group_new() failed: %s", avahi_strerror(avahi_client_errno(c)));
      goto fail;
    }
  }

  if (avahi_entry_group_is_empty(d->group)) {
    LOGMSG("AVAHI: adding service '%s'", d->name);

    if (d->rtsp) {
      if ((ret = avahi_entry_group_add_service(d->group, AVAHI_IF_UNSPEC, AVAHI_PROTO_INET/*UNSPEC*/, (AvahiPublishFlags)0,
                                               d->name, "_rtsp._tcp", NULL, NULL, d->port, NULL)) < 0) {
        if (ret == AVAHI_ERR_COLLISION)
          goto collision;
        LOGERR("AVAHI failed to add _rtsp._tcp service: %s", avahi_strerror(ret));
        goto fail;
      }
    }

    if (d->http) {
      if ((ret = avahi_entry_group_add_service(d->group, AVAHI_IF_UNSPEC, AVAHI_PROTO_INET/*UNSPEC*/, (AvahiPublishFlags)0,
                                               d->name, "_http._tcp", NULL, NULL, d->port, NULL)) < 0) {
        if (ret == AVAHI_ERR_COLLISION)
          goto collision;
        LOGERR("AVAHI failed to add _http._tcp service: %s", avahi_strerror(ret));
        goto fail;
      }
    }

    if ((ret = avahi_entry_group_add_service(d->group, AVAHI_IF_UNSPEC, AVAHI_PROTO_INET/*UNSPEC*/, (AvahiPublishFlags)0,
                                             d->name, "_xvdr._tcp", NULL, NULL, d->port, NULL)) < 0) {
      if (ret == AVAHI_ERR_COLLISION)
        goto collision;
      LOGERR("AVAHI failed to add _xvdr._tcp service: %s", avahi_strerror(ret));
      goto fail;
    }


    /* Tell the server to register the service */
    if ((ret = avahi_entry_group_commit(d->group)) < 0) {
      LOGERR("AVAHI failed to commit entry group: %s", avahi_strerror(ret));
      goto fail;
    }
  }
  return;

 collision:
  n = avahi_alternative_service_name(d->name);
  avahi_free(d->name);
  d->name = n;
  LOGMSG("AVAHI service name collision, renaming service to '%s'", d->name);
  avahi_entry_group_reset(d->group);
  _create_services(c, d);
  return;

 fail:
  avahi_simple_poll_quit(d->simple_poll);
}

static void _client_callback(AvahiClient *c, AvahiClientState state, void * userdata)
{
  avahi_data *d = (avahi_data *)userdata;

  switch (state) {
  case AVAHI_CLIENT_S_RUNNING:
    _create_services(c, d);
    break;

  case AVAHI_CLIENT_FAILURE:
    LOGERR("AVAHI client failure: %s", avahi_strerror(avahi_client_errno(c)));
    avahi_simple_poll_quit(d->simple_poll);
    break;

  case AVAHI_CLIENT_S_COLLISION:
  case AVAHI_CLIENT_S_REGISTERING:
    if (d->group)
      avahi_entry_group_reset(d->group);
    break;

  case AVAHI_CLIENT_CONNECTING:
    break;
  }
}

/*
 *
 */

static void *_avahi_run(void *h)
{
  avahi_data *d = (avahi_data *)h;
  AvahiClient *client = NULL;
  int error;

  if (!(d->simple_poll = avahi_simple_poll_new())) {
    LOGMSG("AVAHI failed to create simple poll object");
    return NULL;
  }

  d->name = avahi_strdup("VDR (xineliboutput)");

  client = avahi_client_new(avahi_simple_poll_get(d->simple_poll), (AvahiClientFlags)0, _client_callback, d, &error);
  if (!client) {
    LOGERR("AVAHI failed to create client: %s", avahi_strerror(error));
    return NULL;
  }

  /* Run the main loop */
  avahi_simple_poll_loop(d->simple_poll);

  LOGMSG("AVAHI terminating");

  avahi_client_free(client);

  return NULL;
}

void *x_avahi_start(int port, int rtsp, int http)
{
  void *h = calloc(1, sizeof(avahi_data));

  if (h) {
    avahi_data *d = (avahi_data *)h;
    int err;

    d->port = port;
    d->rtsp = rtsp;
    d->http = http;

    if ((err = pthread_create (&d->thread, NULL, _avahi_run, h)) != 0) {
      LOGERR("AVAHI can't create new thread (%s)", strerror(err));
      free(h);
      h = NULL;
    }
  }

  return h;
}

static void _avahi_free(void *h)
{
  avahi_data *d = (avahi_data *)h;
  if (d->simple_poll)
    avahi_simple_poll_free(d->simple_poll);
  avahi_free(d->name);

  free(h);
}

void x_avahi_stop(void *h)
{
  avahi_data *d = (avahi_data *)h;
  void *p;

  avahi_simple_poll_quit(d->simple_poll);

  pthread_cancel (d->thread);
  pthread_join (d->thread, &p);

  _avahi_free(d);
}

#else /* HAVE_AVAHI */

void *x_avahi_start(int port, int rtsp, int http)
{
  return NULL;
}

void x_avahi_stop(void *h)
{
}

#endif /* HAVE_AVAHI */
