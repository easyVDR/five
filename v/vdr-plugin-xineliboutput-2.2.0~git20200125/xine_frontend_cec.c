/*
 * xine_frontend_cec.c: Forward (local) HDMI-CEC keys to VDR (server)
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 *
 */

#include "features.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef __FreeBSD__
#include <string.h>
#endif
#include <pthread.h>
#include <unistd.h>

#ifdef HAVE_LIBCEC
#include <libcec/cecc.h>
#endif

#define LOG_MODULENAME "[hdmi-cec]  "
#include "logdefs.h"

#include "xine_frontend.h"
#include "xine_frontend_cec.h"

#ifdef HAVE_LIBCEC

/*
 * legacy libcec compat
 */

#if defined(CEC_LIB_VERSION_MAJOR) && CEC_LIB_VERSION_MAJOR >= 3
#  define HAVE_LIBCEC_3
#  if CEC_LIB_VERSION_MAJOR >= 4
#    define HAVE_LIBCEC_4
#  endif
#else
typedef void * libcec_connection_t;
#  define libcec_initialise(c) ((void*)cec_initialise(c))
#  define libcec_init_video_standalone(c) cec_init_video_standalone()
#  define libcec_find_adapters(a,b,c,d) cec_find_adapters(b,c,d)
#  define libcec_ping_adapters(c) cec_ping_adapters()
#  define libcec_open(c,d,e) cec_open(d,e)
#  define libcec_close(c) cec_close()
#  define libcec_destroy(c) cec_destroy()
#endif

/*
 * data
 */

struct input_cec_s {
  pthread_t   cec_thread;
  frontend_t *fe;
  int         cec_hdmi_port;
  int         cec_dev_type; /* 0 - TV, 5 - AVR */
  int         cec_not_found;
  unsigned    last_key;
};

/*
 * key mappings
 */

static const struct keymap_item {
  const uint8_t map;
  const char    key[12];
} keymap[] = {
  [CEC_USER_CONTROL_CODE_SELECT]                   = {0, "Ok"},
  [CEC_USER_CONTROL_CODE_UP]                       = {0, "Up"},
  [CEC_USER_CONTROL_CODE_DOWN]                     = {0, "Down"},
  [CEC_USER_CONTROL_CODE_LEFT]                     = {0, "Left"},
  [CEC_USER_CONTROL_CODE_RIGHT]                    = {0, "Right"},
  [CEC_USER_CONTROL_CODE_RIGHT_UP]                 = {1, "RIGHT_UP"},
  [CEC_USER_CONTROL_CODE_RIGHT_DOWN]               = {1, "RIGHT_DOWN"},
  [CEC_USER_CONTROL_CODE_LEFT_UP]                  = {1, "LEFT_UP"},
  [CEC_USER_CONTROL_CODE_LEFT_DOWN]                = {1, "LEFT_DOWN"},
  [CEC_USER_CONTROL_CODE_ROOT_MENU]                = {0, "Menu"},
  [CEC_USER_CONTROL_CODE_SETUP_MENU]               = {0, "Menu"},
  [CEC_USER_CONTROL_CODE_CONTENTS_MENU]            = {0, "Recordings"},
  [CEC_USER_CONTROL_CODE_FAVORITE_MENU]            = {1, "FAVORITE"},
  [CEC_USER_CONTROL_CODE_EXIT]                     = {0, "Back"},
  [CEC_USER_CONTROL_CODE_NUMBER0]                  = {0, "0"},
  [CEC_USER_CONTROL_CODE_NUMBER1]                  = {0, "1"},
  [CEC_USER_CONTROL_CODE_NUMBER2]                  = {0, "2"},
  [CEC_USER_CONTROL_CODE_NUMBER3]                  = {0, "3"},
  [CEC_USER_CONTROL_CODE_NUMBER4]                  = {0, "4"},
  [CEC_USER_CONTROL_CODE_NUMBER5]                  = {0, "5"},
  [CEC_USER_CONTROL_CODE_NUMBER6]                  = {0, "6"},
  [CEC_USER_CONTROL_CODE_NUMBER7]                  = {0, "7"},
  [CEC_USER_CONTROL_CODE_NUMBER8]                  = {0, "8"},
  [CEC_USER_CONTROL_CODE_NUMBER9]                  = {0, "9"},
  [CEC_USER_CONTROL_CODE_DOT]                      = {1, "DOT"},
  [CEC_USER_CONTROL_CODE_ENTER]                    = {0, "Ok"},
  [CEC_USER_CONTROL_CODE_CLEAR]                    = {1, "CLEAR"},
  [CEC_USER_CONTROL_CODE_NEXT_FAVORITE]            = {1, "FAVORITE+"},
  [CEC_USER_CONTROL_CODE_CHANNEL_UP]               = {0, "Channel+"},
  [CEC_USER_CONTROL_CODE_CHANNEL_DOWN]             = {0, "Channel-"},
  [CEC_USER_CONTROL_CODE_PREVIOUS_CHANNEL]         = {0, "Previous"},
  [CEC_USER_CONTROL_CODE_SOUND_SELECT]             = {0, "Audio"},
  [CEC_USER_CONTROL_CODE_INPUT_SELECT]             = {1, "INP_SELECT"},
  [CEC_USER_CONTROL_CODE_DISPLAY_INFORMATION]      = {0, "Info"},
  [CEC_USER_CONTROL_CODE_HELP]                     = {1, "HELP"},
  [CEC_USER_CONTROL_CODE_PAGE_UP]                  = {1, "PAGE_UP"},
  [CEC_USER_CONTROL_CODE_PAGE_DOWN]                = {1, "PAGE_DOWN"},
  [CEC_USER_CONTROL_CODE_POWER]                    = {0, "Power"},
  [CEC_USER_CONTROL_CODE_VOLUME_UP]                = {0, "Volume+"},
  [CEC_USER_CONTROL_CODE_VOLUME_DOWN]              = {0, "Volume-"},
  [CEC_USER_CONTROL_CODE_MUTE]                     = {0, "Mute"},
  [CEC_USER_CONTROL_CODE_PLAY]                     = {0, "Play"},
  [CEC_USER_CONTROL_CODE_STOP]                     = {0, "Stop"},
  [CEC_USER_CONTROL_CODE_PAUSE]                    = {0, "Pause"},
  [CEC_USER_CONTROL_CODE_RECORD]                   = {0, "Record"},
  [CEC_USER_CONTROL_CODE_REWIND]                   = {0, "FastRew"},
  [CEC_USER_CONTROL_CODE_FAST_FORWARD]             = {0, "FastFwd"},
  [CEC_USER_CONTROL_CODE_EJECT]                    = {1, "EJECT"},
  [CEC_USER_CONTROL_CODE_FORWARD]                  = {0, "Next"},
  [CEC_USER_CONTROL_CODE_BACKWARD]                 = {0, "Previous"},
  //[CEC_USER_CONTROL_CODE_STOP_RECORD]            = {0, ""}, //0x4D,
  //[CEC_USER_CONTROL_CODE_PAUSE_RECORD]           = {0, ""}, //0x4E,
  [CEC_USER_CONTROL_CODE_ANGLE]                    = {1, "ANGLE"},
  [CEC_USER_CONTROL_CODE_SUB_PICTURE]              = {0, "Subtitles"},
  //[CEC_USER_CONTROL_CODE_VIDEO_ON_DEMAND]        = {0, ""}, //0x52,
  [CEC_USER_CONTROL_CODE_ELECTRONIC_PROGRAM_GUIDE] = {0, "Schedule"},
  [CEC_USER_CONTROL_CODE_TIMER_PROGRAMMING]        = {0, "Timers"},
  //[CEC_USER_CONTROL_CODE_INITIAL_CONFIGURATION]    = {0, ""},
  //[CEC_USER_CONTROL_CODE_PLAY_FUNCTION]            = {0, ""},
  //[CEC_USER_CONTROL_CODE_PAUSE_PLAY_FUNCTION]      = {0, ""},
  //[CEC_USER_CONTROL_CODE_RECORD_FUNCTION]          = {0, ""},
  //[CEC_USER_CONTROL_CODE_PAUSE_RECORD_FUNCTION]    = {0, ""},
  //[CEC_USER_CONTROL_CODE_STOP_FUNCTION]            = {0, ""},
  //[CEC_USER_CONTROL_CODE_MUTE_FUNCTION]            = {0, ""},
  //[CEC_USER_CONTROL_CODE_RESTORE_VOLUME_FUNCTION]  = {0, ""},
  //[CEC_USER_CONTROL_CODE_TUNE_FUNCTION]            = {0, ""},
  //[CEC_USER_CONTROL_CODE_SELECT_MEDIA_FUNCTION]       = {0, ""},
  //[CEC_USER_CONTROL_CODE_SELECT_AV_INPUT_FUNCTION]    = {0, ""},
  [CEC_USER_CONTROL_CODE_SELECT_AUDIO_INPUT_FUNCTION] = {0, "Audio"},
  [CEC_USER_CONTROL_CODE_POWER_TOGGLE_FUNCTION]       = {0, "Power"},
  [CEC_USER_CONTROL_CODE_POWER_OFF_FUNCTION]          = {0, "Power"},
  //[CEC_USER_CONTROL_CODE_POWER_ON_FUNCTION]           = {0, ""},
  [CEC_USER_CONTROL_CODE_F1_BLUE]                  = {0, "Blue"},
  [CEC_USER_CONTROL_CODE_F2_RED]                   = {0, "Red"},
  [CEC_USER_CONTROL_CODE_F3_GREEN]                 = {0, "Green"},
  [CEC_USER_CONTROL_CODE_F4_YELLOW]                = {0, "Yellow"},
  [CEC_USER_CONTROL_CODE_F5]                       = {0, "User1"},
  [CEC_USER_CONTROL_CODE_DATA]                     = {1, "DATA"},
  //[CEC_USER_CONTROL_CODE_AN_RETURN]              = {0, ""},
  [CEC_USER_CONTROL_CODE_AN_CHANNELS_LIST]         = {0, "Channels"},
};

#define KEY_NONE (unsigned)-1

/*
 * libcec callbacks
 */

static void _cec4_config_changed_cb(void *this_gen, const libcec_configuration *config)
{
  LOGDBG("cec_config_changed");
}
#ifndef HAVE_LIBCEC_4
static int _cec_config_changed_cb(void *this_gen, const libcec_configuration config)
{
  _cec4_config_changed_cb(this_gen, &config);
  return 1;
}
#endif

static int _cec4_menu_state_changed_cb(void *this_gen, const cec_menu_state state)
{
  LOGDBG("cec_menu_state_changed");
  return 1;
}
#ifndef HAVE_LIBCEC_4
static int _cec_menu_state_changed_cb(void *this_gen, const cec_menu_state state)
{
  return _cec4_menu_state_changed_cb(this_gen, state);
}
#endif

static void _cec_source_activated_cb(void *this_gen, const cec_logical_address address, const uint8_t param)
{
  LOGMSG("cec_source_activated: address %d param %d", address, param);
}

static void _cec4_log_cb(void *this_gen, const cec_log_message *message)
{
  if (message->level <= CEC_LOG_ERROR) {
    errno = 0;
    LOGERR("%s", message->message);
  } else if (message->level <= CEC_LOG_NOTICE) {
    LOGMSG("%s", message->message);
  } else if (message->level <= CEC_LOG_DEBUG) {
    LOGDBG("%s", message->message);
  } else {
    LOGVERBOSE("%s", message->message);
  }
}
#ifndef HAVE_LIBCEC_4
static int _cec_log_cb(void *this_gen, const cec_log_message message)
{
  _cec4_log_cb(this_gen, &message);
  return 1;
}
#endif

static void _cec4_keypress_cb(void *this_gen, const cec_keypress *keypress)
{
  input_cec_t *cec = this_gen;
  frontend_t  *fe  = cec->fe;
  unsigned int keycode = keypress->keycode;

  LOGVERBOSE("keypress 0x%x duration %d", keycode, keypress->duration);

  if (keycode == cec->last_key && keypress->duration > 0)
    return;

  if (keypress->duration > 0)
    cec->last_key = KEY_NONE;
  else
    cec->last_key = keycode;

  if (keycode >= sizeof(keymap) / sizeof(keymap[0]) ||
      !keymap[keycode].key[0]) {
    LOGMSG("unknown keycode 0x%x", keycode);
    return;
  }

  LOGDBG("sending key %s%s", keymap[keycode].map ? "CEC." : "", keymap[keycode].key);

  alarm(3);
  fe->send_input_event(fe, keymap[keycode].map ? "CEC" : NULL,
                       keymap[keycode].key, 0, 1);
  alarm(0);

  return;
}
#ifndef HAVE_LIBCEC_4
static int _cec_keypress_cb(void *this_gen, const cec_keypress keypress)
{
  _cec4_keypress_cb(this_gen, &keypress);
  return 1;
}
#endif

static void _cec4_alert_cb(void *this_gen, const libcec_alert type, const libcec_parameter param)
{
  switch (type) {
    case CEC_ALERT_CONNECTION_LOST:
      LOGMSG("ALERT: CEC connection lost");
      break;
    case CEC_ALERT_PERMISSION_ERROR:
      LOGMSG("ALERT: Permission error");
      break;
    case CEC_ALERT_PORT_BUSY:
      LOGMSG("ALERT: Port busy");
      break;
    case CEC_ALERT_PHYSICAL_ADDRESS_ERROR:
      LOGMSG("ALERT: Physical address error");
      break;
    case CEC_ALERT_TV_POLL_FAILED:
      LOGMSG("ALERT: TV poll failed");
      break;

    case CEC_ALERT_SERVICE_DEVICE:
    default:
      break;
  }
}
#ifndef HAVE_LIBCEC_4
static int _cec_alert_cb(void *this_gen, const libcec_alert type, const libcec_parameter param)
{
  _cec4_alert_cb(this_gen, type, param);
  return 0;
}
#endif

static void _cec4_command_cb(void *this_gen, const cec_command *command)
{
  LOGMSG("Received command 0x%x from 0x%x", command->opcode, command->initiator);

  switch (command->opcode) {
    case CEC_OPCODE_STANDBY:
    case CEC_OPCODE_SET_MENU_LANGUAGE:
    case CEC_OPCODE_DECK_CONTROL:
    case CEC_OPCODE_PLAY:
    default:
      break;
  }
}
#ifndef HAVE_LIBCEC_4
static int _cec_command_cb(void *this_gen, const cec_command command)
{
  _cec4_command_cb(this_gen, &command);
  return 1;
}
#endif


ICECCallbacks callbacks = {
#ifdef HAVE_LIBCEC_4
  .logMessage           = _cec4_log_cb,
  .keyPress             = _cec4_keypress_cb,
  .commandReceived      = _cec4_command_cb,
  .configurationChanged = _cec4_config_changed_cb,
  .alert                = _cec4_alert_cb,
  .menuStateChanged     = _cec4_menu_state_changed_cb,
  .sourceActivated      = _cec_source_activated_cb,
#else
  .CBCecKeyPress             = _cec_keypress_cb,
  .CBCecCommand              = _cec_command_cb,
  .CBCecLogMessage           = _cec_log_cb,
  .CBCecAlert                = _cec_alert_cb,
  .CBCecConfigurationChanged = _cec_config_changed_cb,
  .CBCecSourceActivated      = _cec_source_activated_cb,
  .CBCecMenuStateChanged     = _cec_menu_state_changed_cb,
#endif
};

/*
 * configuration
 */

static void _libcec_config_clear(libcec_configuration *p)
{
  memset(p, 0, sizeof(*p));

  p->iPhysicalAddress = CEC_PHYSICAL_ADDRESS_TV;
  p->baseDevice = CEC_DEFAULT_BASE_DEVICE;
  p->iHDMIPort = CEC_DEFAULT_HDMI_PORT;
  p->tvVendor = CEC_VENDOR_UNKNOWN;
#ifdef HAVE_LIBCEC_3
  p->clientVersion = LIBCEC_VERSION_CURRENT;
  p->serverVersion = LIBCEC_VERSION_CURRENT;
#else
  p->clientVersion = CEC_CLIENT_VERSION_CURRENT;
  p->serverVersion = CEC_SERVER_VERSION_CURRENT;
#endif
  p->bAutodetectAddress = CEC_DEFAULT_SETTING_AUTODETECT_ADDRESS;
  p->bGetSettingsFromROM = CEC_DEFAULT_SETTING_GET_SETTINGS_FROM_ROM;
#ifndef HAVE_LIBCEC_4
  p->bUseTVMenuLanguage = CEC_DEFAULT_SETTING_USE_TV_MENU_LANGUAGE;
#endif
  p->bActivateSource = CEC_DEFAULT_SETTING_ACTIVATE_SOURCE;
#ifndef HAVE_LIBCEC_4
  p->bPowerOffScreensaver = CEC_DEFAULT_SETTING_POWER_OFF_SCREENSAVER;
  p->bPowerOnScreensaver = CEC_DEFAULT_SETTING_POWER_ON_SCREENSAVER;
#endif
  p->bPowerOffOnStandby = CEC_DEFAULT_SETTING_POWER_OFF_ON_STANDBY;
#ifndef HAVE_LIBCEC_4
  p->bShutdownOnStandby = CEC_DEFAULT_SETTING_SHUTDOWN_ON_STANDBY;
  p->bSendInactiveSource = CEC_DEFAULT_SETTING_SEND_INACTIVE_SOURCE;
#endif
  p->iFirmwareVersion = CEC_FW_VERSION_UNKNOWN;
#ifndef HAVE_LIBCEC_4
  p->bPowerOffDevicesOnStandby = CEC_DEFAULT_SETTING_POWER_OFF_DEVICES_STANDBY;
#endif
  memcpy(p->strDeviceLanguage, CEC_DEFAULT_DEVICE_LANGUAGE, 3);
  p->iFirmwareBuildDate = CEC_FW_BUILD_UNKNOWN;
  p->bMonitorOnly = 0;
  p->cecVersion = CEC_DEFAULT_SETTING_CEC_VERSION;
  p->adapterType = ADAPTERTYPE_UNKNOWN;

#ifdef CEC_DOUBLE_TAP_TIMEOUT_50_MS
  p->iDoubleTapTimeout50Ms = CEC_DOUBLE_TAP_TIMEOUT_50_MS;
#else
  p->iDoubleTapTimeoutMs = CEC_DOUBLE_TAP_TIMEOUT_MS;
#endif
  p->comboKey = CEC_USER_CONTROL_CODE_STOP;
  p->iComboKeyTimeoutMs = CEC_DEFAULT_COMBO_TIMEOUT_MS;

  memset(p->strDeviceName, 0, sizeof(p->strDeviceName));

  //deviceTypes.Clear();
  size_t i;
  for (i = 0; i < sizeof(p->deviceTypes.types) / sizeof(p->deviceTypes.types[0]); i++)
    p->deviceTypes.types[i] = CEC_DEVICE_TYPE_RESERVED;
  //logicalAddresses.Clear();
  p->logicalAddresses.primary = CECDEVICE_UNREGISTERED;
  memset(p->logicalAddresses.addresses, 0, sizeof(p->logicalAddresses.addresses));
  //wakeDevices.Clear();
  p->wakeDevices.primary = CECDEVICE_UNREGISTERED;
  memset(p->wakeDevices.addresses, 0, sizeof(p->wakeDevices.addresses));
  //powerOffDevices.Clear();
  p->powerOffDevices.primary = CECDEVICE_UNREGISTERED;
  memset(p->powerOffDevices.addresses, 0, sizeof(p->powerOffDevices.addresses));

  #if CEC_DEFAULT_SETTING_POWER_OFF_SHUTDOWN == 1
  p->powerOffDevices.primary = CECDEVICE_BROADCAST;
  #endif
  #if CEC_DEFAULT_SETTING_ACTIVATE_SOURCE == 1
  p->wakeDevices.primary = CECDEVICE_TV;
  #endif

  p->callbackParam = NULL;
  p->callbacks = NULL;
}

static int _cec_parse_edid(uint8_t *edid, int size)
{
  /* get cec physical address from edid vendor-specific block */
  int i;
  for (i = 0; i < size; i++) {
    if (edid[i] == 0x03 && edid[i + 1] == 0x0c && edid[i + 2] == 0x00) {
      /* hdmi marker found */
      LOGMSG("Got CEC physical address from edid: %d.%d.%d.%d",
             edid[i + 3] >> 4 & 0xf,
             edid[i + 3]      & 0xf,
             edid[i + 4] >> 4 & 0xf,
             edid[i + 4]      & 0xf);

      return (edid[i + 3] << 8) | edid[i + 4];
    }
  }
  return -1;
}

static int _detect_hdmi_address(input_cec_t *cec)
{
  if (cec->cec_hdmi_port <= 0) {
    frontend_t *fe = (frontend_t*)cec->fe;
    if (fe->fe_display_edid) {
      int cec_hdmi_address;
      int size = 0;
      uint8_t *edid;
      edid = fe->fe_display_edid(fe, &size);
      if (edid) {
        cec_hdmi_address = _cec_parse_edid(edid, size);
        free(edid);

        if (cec_hdmi_address > 0) {
          return cec_hdmi_address;
        }
      }
    }
    LOGMSG("WARNING: CEC HDMI port not given and edid reading/parsing failed");
  }
  return 0;
}

static libcec_connection_t _libcec_init(input_cec_t *cec)
{
  libcec_configuration config;
  libcec_connection_t conn;

  _libcec_config_clear(&config);

  strncpy(config.strDeviceName, "VDR", sizeof(config.strDeviceName));

  config.iPhysicalAddress = _detect_hdmi_address(cec);
  config.iHDMIPort = cec->cec_hdmi_port;
  config.baseDevice = cec->cec_dev_type;

  config.bActivateSource = 0;
  config.callbackParam = cec;
  config.callbacks = &callbacks;

  config.deviceTypes.types[0] = CEC_DEVICE_TYPE_PLAYBACK_DEVICE;
  config.deviceTypes.types[1] = CEC_DEVICE_TYPE_RECORDING_DEVICE;
  config.deviceTypes.types[2] = CEC_DEVICE_TYPE_TUNER;
  //config.deviceTypes.types[3] = CEC_DEVICE_TYPE_AUDIO_SYSTEM;

  if (!(conn = libcec_initialise(&config))) {
    LOGMSG("libcec_initialize() failed");
    return NULL;
  }

  libcec_init_video_standalone(conn);

  return conn;
}

/*
 *
 */

static int _libcec_open(input_cec_t *cec, libcec_connection_t conn)
{
  cec_adapter devices[10];
  int count = libcec_find_adapters(conn, devices, 10, NULL);
  if (count < 1) {
    if (!cec->cec_not_found) {
      LOGMSG("No HDMI-CEC adapters found");
      cec->cec_not_found = 1;
    }
    return 0;
  }

  LOGMSG("%d adapters found. Opening %s", count, devices[0].comm);

  if (!libcec_open(conn, devices[0].comm, 3000)) {
    LOGMSG("error opening CEC adapter");
    return 0;
  }

  LOGMSG("opened adapter %s", devices[0].comm);

  return 1;
}

static int _libcec_check_device(libcec_connection_t conn)
{
  if (!libcec_ping_adapters(conn)) {
    LOGMSG("libcec_ping_adapters() failed");
    return 0;
  }

  return 1;
}

static void _cleanup(void *p)
{
#ifdef HAVE_LIBCEC_3
  libcec_connection_t conn = *(libcec_connection_t *)p;
#endif
  if (conn) {
    libcec_close(conn);
    libcec_destroy(conn);
  }
}

static void *_cec_receiver_thread(void *cec_gen)
{
  input_cec_t *cec = cec_gen;
  frontend_t  *fe  = cec->fe;
  libcec_connection_t conn = NULL;

  LOGDBG("started");

  pthread_cleanup_push(_cleanup, &conn);

  enum { INIT, WAIT_DEVICE, RUNNING } state = INIT;

  while (fe->xine_is_finished(fe, 0) != FE_XINE_EXIT) {

    pthread_testcancel();

    switch (state) {
    case INIT:
      pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
      if (!(conn = _libcec_init(cec))) {
        return NULL;
      }
      pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
      state = WAIT_DEVICE;
      break;
    case WAIT_DEVICE:
      pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
      if (_libcec_open(cec, conn)) {
        state = RUNNING;
      }
      pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
      usleep(5000*1000);
      break;
    case RUNNING:
      pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
      if (!_libcec_check_device(conn)) {
        libcec_close(conn);
        state = WAIT_DEVICE;
      }
      pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
      usleep(1000*1000);
      break;
    }
  }

  pthread_cleanup_pop(1);

  pthread_exit(NULL);
  return NULL; /* never reached */
}

#endif /* HAVE_LIBCEC */

/*
 * interface
 */

input_cec_t *cec_start(struct frontend_s *fe, int hdmi_port, int dev_type)
{
  input_cec_t *cec = NULL;

#ifdef HAVE_LIBCEC
  int err;

  if (hdmi_port < 0) {
    return NULL;
  }

  cec = calloc(1, sizeof(*cec));
  if (!cec) {
    return NULL;
  }

  cec->fe            = fe;
  cec->cec_hdmi_port = hdmi_port;
  cec->cec_dev_type  = dev_type;
  cec->last_key      = KEY_NONE;

  if ((err = pthread_create (&cec->cec_thread,
                             NULL, _cec_receiver_thread,
                             (void*)cec)) != 0) {
    fprintf(stderr, "can't create new thread for HDMI-CEC (%s)\n",
            strerror(err));
    free(cec);
    cec = NULL;
  }
#endif /* HAVE_LIBCEC */

  return cec;
}

void cec_stop(input_cec_t **pcec)
{
#ifdef HAVE_LIBCEC
  if (*pcec) {
    input_cec_t *cec = *pcec;
    void *p;

    *pcec = NULL;

    pthread_cancel (cec->cec_thread);
    pthread_join (cec->cec_thread, &p);

    free(cec);
  }
#endif /* HAVE_LIBCEC */
}
