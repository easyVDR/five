/*
 * xine_frontend_lirc.c: Forward (local) lirc keys to VDR (server)
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 *
 */
/*
 *
 * Almost directly copied from vdr-1.4.3-2 (lirc.c : cLircRemote)
 *
 */
/*
 * lirc.c: LIRC remote control
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * LIRC support added by Carsten Koch <Carsten.Koch@icem.de>  2000-06-16.
 *
 */

#include "xine_frontend_lirc.h"

#include <stdlib.h>

#ifdef _WIN32
input_lirc_t *lirc_start(struct frontend_s *fe, const char *lirc_dev, int repeat_emu, int gui_hotkeys)
{
  return NULL;
}
void lirc_stop(input_lirc_t **l)
{
}
#else

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "tools/time_ms.h"

#define LOG_MODULENAME "[lirc]      "
#include "logdefs.h"

#include "xine_frontend.h"

#define REPEATDELAY     350 /* ms */
#define REPEATFREQ      100 /* ms */
#define REPEATTIMEOUT   500 /* ms */
#define RECONNECTDELAY 3000 /* ms */

#define LIRC_KEY_BUF      30
#define LIRC_BUFFER_SIZE 128
#define MIN_LIRCD_CMD_LEN  5

struct input_lirc_s {
  pthread_t   lirc_thread;
  frontend_t *fe;
  char       *lirc_device_name;
  int         fd_lirc;
  uint8_t     lirc_repeat_emu;
  uint8_t     gui_hotkeys;
};

static void lircd_connect(input_lirc_t *this)
{
  union {
    struct sockaddr    sa;
    struct sockaddr_un un;
  } addr;

  if (this->fd_lirc >= 0) {
    close(this->fd_lirc);
    this->fd_lirc = -1;
  }

  if (!this->lirc_device_name) {
    LOGDBG("no lirc device given");
    return;
  }

  addr.un.sun_family = AF_UNIX;
  strncpy(addr.un.sun_path, this->lirc_device_name, sizeof(addr.un.sun_path));
  addr.un.sun_path[sizeof(addr.un.sun_path)-1] = 0;

  if ((this->fd_lirc = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
    LOGERR("lirc error: socket() < 0");
    return;
  }

  if (connect(this->fd_lirc, &addr.sa, sizeof(addr))) {
    LOGERR("lirc error: connect(%s) < 0", this->lirc_device_name);
    close(this->fd_lirc);
    this->fd_lirc = -1;
    return;
  }
}

static int _select(int fd, int timeout)
{
  fd_set set;
  int result;

  FD_ZERO(&set);
  FD_SET(fd, &set);

  if (timeout >= 0) {
    struct timeval tv;
    tv.tv_sec  = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;
    result = select(FD_SETSIZE, &set, NULL, NULL, &tv) > 0 && FD_ISSET(fd, &set);
  } else {
    result = select(FD_SETSIZE, &set, NULL, NULL, NULL) > 0 && FD_ISSET(fd, &set);
  }

  return result;
}

static void *lirc_receiver_thread(void *this_gen)
{
  input_lirc_t *this = this_gen;
  frontend_t   *fe = this->fe;
  const int   priority = -1;
  int      timeout = -1;
  int      repeat = 0;
  uint64_t FirstTime = time_ms();
  uint64_t LastTime = time_ms();
  char     buf[LIRC_BUFFER_SIZE];
  char     LastKeyName[LIRC_KEY_BUF] = "";

  LOGMSG("lirc forwarding started");

  errno = 0;
  if ((nice(priority) == -1) && errno)
    LOGDBG("LIRC: Can't nice to value: %d", priority);

  lircd_connect(this);

  while (fe->xine_is_finished(fe, 0) != FE_XINE_EXIT && this->fd_lirc >= 0) {
    int ready, ret = -1;

    pthread_testcancel();
    ready = _select(this->fd_lirc, timeout);

    pthread_testcancel();
    if (ready < 0) {
      LOGMSG("LIRC connection lost ?");
      break;
    }

    if (ready) {

      do {
        errno = 0;
        ret = read(this->fd_lirc, buf, sizeof(buf));
        pthread_testcancel();
      } while (ret < 0 && errno == EINTR);

      if (ret <= 0 ) {
        /* try reconnecting */
        LOGERR("LIRC connection lost");
        lircd_connect(this);
        while (this->fd_lirc < 0) {
          pthread_testcancel();
          sleep(RECONNECTDELAY/1000);
          lircd_connect(this);
        }
        if (this->fd_lirc >= 0)
          LOGMSG("LIRC reconnected");
	continue;
      }

      if (ret >= MIN_LIRCD_CMD_LEN) {
        unsigned int count;
        char KeyName[LIRC_KEY_BUF];
        LOGDBG("LIRC: %s", buf);

        if (sscanf(buf, "%*x %x %29s", &count, KeyName) != 2) {
          /* '29' in '%29s' is LIRC_KEY_BUF-1! */
          LOGMSG("unparseable lirc command: %s", buf);
          continue;
        }

        if (this->lirc_repeat_emu)
          if (strcmp(KeyName, LastKeyName) == 0 && elapsed(LastTime) < REPEATDELAY)
            count = repeat + 1;

        if (count == 0) {
          if (strcmp(KeyName, LastKeyName) == 0 && elapsed(FirstTime) < REPEATDELAY)
            continue; /* skip keys coming in too fast */
          if (repeat) {
            alarm(3);
            fe->send_input_event(fe, "LIRC", LastKeyName, 0, 1);
            alarm(0);
          }

          strcpy(LastKeyName, KeyName);
          repeat = 0;
          FirstTime = time_ms();
          timeout = -1;
        }
        else {
          if (elapsed(LastTime) < REPEATFREQ)
            continue; /* repeat function kicks in after a short delay */

          if (elapsed(FirstTime) < REPEATDELAY) {
            if (this->lirc_repeat_emu)
              LastTime = time_ms();
            continue; /* skip keys coming in too fast */
          }
          repeat = 1;
          timeout = REPEATDELAY;
        }
        LastTime = time_ms();

        if (this->gui_hotkeys) {
          if (!strcmp(KeyName, "Quit")) {
            fe->send_event(fe, "QUIT");
            break;
          }
          if (!strcmp(KeyName, "PowerOff")) {
            fe->send_event(fe, "POWER_OFF");
            break;
          }
          if (!strcmp(KeyName, "Fullscreen")) {
            if (!repeat)
              fe->send_event(fe, "TOGGLE_FULLSCREEN");
            continue;
          }
          if (!strcmp(KeyName, "Deinterlace")) {
            if (!repeat)
              fe->send_event(fe, "TOGGLE_DEINTERLACE");
            continue;
          }
        }

        alarm(3);
        fe->send_input_event(fe, "LIRC", KeyName, repeat, 0);
        alarm(0);

      }

    }
    if (repeat && (!ready || ret < MIN_LIRCD_CMD_LEN)) { /* the last one was a repeat, so let's generate a release */
      if (elapsed(LastTime) >= REPEATTIMEOUT) {
        alarm(3);
        fe->send_input_event(fe, "LIRC", LastKeyName, 0, 1);
        alarm(0);
        repeat = 0;
        *LastKeyName = 0;
        timeout = -1;
      }
    }
  }


  if (this->fd_lirc >= 0)
    close(this->fd_lirc);
  this->fd_lirc = -1;
  pthread_exit(NULL);
  return NULL; /* never reached */
}

input_lirc_t *lirc_start(struct frontend_s *fe, const char *lirc_dev, int repeat_emu, int gui_hotkeys)
{
  int err;
  input_lirc_t *this;

  if (!lirc_dev) {
    return NULL;
  }

  this = calloc(1, sizeof(*this));
  if (!this) {
    return NULL;
  }

  this->fe               = fe;
  this->lirc_device_name = strdup(lirc_dev);
  this->lirc_repeat_emu  = repeat_emu;
  this->gui_hotkeys      = gui_hotkeys;
  this->fd_lirc          = -1;

  if ((err = pthread_create (&this->lirc_thread,
                             NULL, lirc_receiver_thread,
                             (void*)this)) != 0) {
    fprintf(stderr, "can't create new thread for lirc (%s)\n",
            strerror(err));

    free(this->lirc_device_name);
    free(this);
    this = NULL;
  }

  return this;
}

void lirc_stop(input_lirc_t **plirc)
{
  if (*plirc) {
    input_lirc_t *this = *plirc;
    void *p;

    pthread_cancel (this->lirc_thread);
    pthread_join (this->lirc_thread, &p);

    if (this->fd_lirc >= 0)
      close(this->fd_lirc);
    this->fd_lirc = -1;

    free(this->lirc_device_name);
    free(this);
  }
}

#endif /* WIN32 */
