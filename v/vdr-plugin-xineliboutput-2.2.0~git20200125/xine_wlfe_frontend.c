/*
 * xine_fbfe_frontend.c: Simple front-end, Wayland functions
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 *
 */

#include <inttypes.h>
#include <poll.h>

#include <wayland-client.h>
#include <wayland-cursor.h>
#include <linux/input.h>   /* KEY_* */

#include <xine.h>

#define LOG_MODULENAME "[vdr-wlfe]  "
#include "logdefs.h"

#include "tools/time_ms.h"


#ifndef XINE_VISUAL_TYPE_WAYLAND
#  define XINE_VISUAL_TYPE_WAYLAND 13
typedef struct {
  struct wl_display *display;
  struct wl_surface *surface;

  void *user_data;
  void (*frame_output_cb) (void *user_data,
                           int video_width, int video_height,
                           double video_pixel_aspect,
                           int *dest_x, int *dest_y,
                           int *dest_width, int *dest_height,
                           double *dest_pixel_aspect,
                           int *win_x, int *win_y);
} xine_wayland_visual_t;
#endif

#include "xine_frontend_internal.h"

/*
 *
 */

#define DOUBLECLICK_TIME    500
#define MOUSECURSOR_TIMEOUT 2000
#define RESIZE_BORDER_WIDTH 20

/*
 * data
 */

typedef struct wlfe_s {

  /* function pointers / base class */
  union {
    frontend_t fe;  /* generic frontend */
    fe_t       x;   /* xine frontend */
  };

  /* Wayland */

  struct wl_display    *display;
  struct wl_registry   *registry;
  struct wl_compositor *compositor;

  struct wl_surface       *surface;
  struct wl_shell         *shell;
  struct wl_shell_surface *shell_surface;

  struct wl_seat     *seat;
  struct wl_keyboard *keyboard;
  struct wl_pointer  *pointer;
  struct wl_touch    *touch;
  struct wl_output   *output;

  struct wl_shm          *shm;
  struct wl_cursor_theme *cursor_theme;
  struct wl_cursor       *current_cursor;
  struct wl_surface      *cursor_surface;

  int        shell_w, shell_h;     /* current shell surface dimensions (virtual pixels) */
  int        factor;               /* current zoom factor */
  wl_fixed_t pointer_x, pointer_y; /* last pointer location */
  wl_fixed_t touch_x, touch_y;     /* last touch location */
  uint32_t   pointer_enter_serial; /* last pointer_enter event serial */

  /* frontend */
  uint8_t   fullscreen : 1;      /* current fullscreen state */
  uint8_t   touchscreen : 1;     /* touchscreen control enable */
  uint8_t   gui_hotkeys : 1;
  uint32_t  prev_click_time;     /* time of previous mouse button click (grab double clicks) */

  int       mousecursor_timeout; /* time (ms) left until hide mouse cursor */
  enum wl_shell_surface_resize resize_cursor_mode;   /* indicates current resize cursor mode */

  int       initial_fullscreen_req;
} wlfe_t;

/*
 *
 */

static int _handle_touch(wlfe_t *wlfe, wl_fixed_t xpos, wl_fixed_t ypos)
{
  if (wlfe->touchscreen) {
    int x = wl_fixed_to_int(xpos) * 4 / wlfe->shell_w;
    int y = wl_fixed_to_int(ypos) * 3 / wlfe->shell_h;
    static const char * const map[3][4] = {
      {"Menu", "Up", "Back", "Ok"},
      {"Left", "Down", "Right", "Ok"},
      {"Red", "Green", "Yellow", "Blue"}};
    if (map[y][x]) {
      char tmp[128];
      sprintf(tmp, "KEY %s", map[y][x]);
      wlfe->x.fe.send_event(&wlfe->fe, tmp);
    }
    return 1;
  }
  return 0;
}


/*
 * pointer
 */

static void show_cursor(wlfe_t *wlfe, struct wl_pointer *pointer, uint32_t serial, int on)
{
  if (wlfe->current_cursor && on) {
    struct wl_buffer *buffer;
    struct wl_cursor_image *image;
    image = wlfe->current_cursor->images[0];
    buffer = wl_cursor_image_get_buffer(image);
    wl_pointer_set_cursor(pointer, serial,
                          wlfe->cursor_surface,
                          image->hotspot_x,
                          image->hotspot_y);
    wl_surface_attach(wlfe->cursor_surface, buffer, 0, 0);
    wl_surface_damage(wlfe->cursor_surface, 0, 0,
                      image->width, image->height);
    wl_surface_commit(wlfe->cursor_surface);
  } else {
    wl_pointer_set_cursor(pointer, serial, NULL, 0, 0);
  }
}

static void
pointer_handle_enter(void *data, struct wl_pointer *pointer,
                     uint32_t serial, struct wl_surface *surface,
                     wl_fixed_t sx, wl_fixed_t sy)
{
  wlfe_t *wlfe = data;

  wlfe->pointer_enter_serial = serial;
  wlfe->pointer_x = sx;
  wlfe->pointer_y = sy;

  wlfe->mousecursor_timeout = MOUSECURSOR_TIMEOUT;
  show_cursor(wlfe, pointer, serial, 1);
}

static void
pointer_handle_leave(void *data, struct wl_pointer *pointer,
                     uint32_t serial, struct wl_surface *surface)
{
}

static inline enum wl_shell_surface_resize
_check_bounds(int x, int y, int w, int h, int dx, int dy)
{
  if (x < dx) {
    if (y < dy) {
      return WL_SHELL_SURFACE_RESIZE_TOP_LEFT;
    } else if (y > h - dy) {
     return WL_SHELL_SURFACE_RESIZE_BOTTOM_LEFT;
    } else {
      return WL_SHELL_SURFACE_RESIZE_LEFT;
    }
  } else if (x > w - dx) {
    if (y < dy) {
      return WL_SHELL_SURFACE_RESIZE_TOP_RIGHT;
    } else if (y > h - dy) {
      return WL_SHELL_SURFACE_RESIZE_BOTTOM_RIGHT;
    } else {
      return WL_SHELL_SURFACE_RESIZE_RIGHT;
    }
  } else if (y < dy) {
    return WL_SHELL_SURFACE_RESIZE_TOP;
  } else if (y > h - dx) {
    return WL_SHELL_SURFACE_RESIZE_BOTTOM;
  }
  return WL_SHELL_SURFACE_RESIZE_NONE;
}

static void
pointer_handle_motion(void *data, struct wl_pointer *pointer,
                      uint32_t time, wl_fixed_t sx, wl_fixed_t sy)
{
  static const char *resize_cursor_names[] = {
    [WL_SHELL_SURFACE_RESIZE_BOTTOM_LEFT] = "bottom_left_corner",
    [WL_SHELL_SURFACE_RESIZE_BOTTOM_RIGHT] = "bottom_right_corner",
    [WL_SHELL_SURFACE_RESIZE_BOTTOM] = "bottom_side",
    [WL_SHELL_SURFACE_RESIZE_NONE] = "left_ptr",
    [WL_SHELL_SURFACE_RESIZE_LEFT] = "left_side",
    [WL_SHELL_SURFACE_RESIZE_RIGHT] = "right_side",
    [WL_SHELL_SURFACE_RESIZE_TOP_LEFT] = "top_left_corner",
    [WL_SHELL_SURFACE_RESIZE_TOP_RIGHT] = "top_right_corner",
    [WL_SHELL_SURFACE_RESIZE_TOP] = "top_side",
  };

  wlfe_t *wlfe = data;

  wlfe->pointer_x = sx;
  wlfe->pointer_y = sy;

  /* change cursor to resize at window edges */
  enum wl_shell_surface_resize mode;
  if (!wlfe->fullscreen) {
    int x = wl_fixed_to_int(sx);
    int y = wl_fixed_to_int(sy);
    mode = _check_bounds(x, y, wlfe->shell_w, wlfe->shell_h,
                         RESIZE_BORDER_WIDTH, RESIZE_BORDER_WIDTH);
  } else {
    /* reset cursor in fullscreen mode */
    mode = WL_SHELL_SURFACE_RESIZE_NONE;
  }
  if (wlfe->resize_cursor_mode != mode) {
    wlfe->resize_cursor_mode = mode;
    wlfe->current_cursor = wl_cursor_theme_get_cursor(wlfe->cursor_theme, resize_cursor_names[mode]);
    show_cursor(wlfe, pointer, wlfe->pointer_enter_serial, 1);
  }

  /* reset cursor hiding */
  if (wlfe->mousecursor_timeout <= 0) {
    show_cursor(wlfe, pointer, wlfe->pointer_enter_serial, 1);
  }
  wlfe->mousecursor_timeout = MOUSECURSOR_TIMEOUT;
}

static void
pointer_handle_button(void *data, struct wl_pointer *wl_pointer,
                      uint32_t serial, uint32_t time, uint32_t button,
                      uint32_t state)
{
  wlfe_t *wlfe = data;

  if (button == BTN_LEFT && state == WL_POINTER_BUTTON_STATE_PRESSED) {
    /* Double-click toggles between fullscreen and windowed mode */
    if(time - wlfe->prev_click_time < DOUBLECLICK_TIME) {
      /* Toggle fullscreen */
      wlfe->x.toggle_fullscreen_cb(&wlfe->x, -1);
      wlfe->prev_click_time = 0; /* don't react to third click ... */
    } else {
      wlfe->prev_click_time = time;
    }

    /* handle fullscreen touch controls */
    if (wlfe->fullscreen) {
      if (_handle_touch(wlfe, wlfe->pointer_x, wlfe->pointer_y))
        return;
    }

    /* handle window move / resize */
    if (!wlfe->fullscreen) {

      /* if at edges, resize */
      int x = wl_fixed_to_int(wlfe->pointer_x);
      int y = wl_fixed_to_int(wlfe->pointer_y);
      int dir = _check_bounds(x, y, wlfe->shell_w, wlfe->shell_h,
                              RESIZE_BORDER_WIDTH, RESIZE_BORDER_WIDTH);
      if (dir != WL_SHELL_SURFACE_RESIZE_NONE) {
        if (wlfe->shell_surface)
          wl_shell_surface_resize(wlfe->shell_surface, wlfe->seat, serial, dir);
        return;
      }

      /* if at middle, move */
      if (wlfe->shell_surface)
        wl_shell_surface_move(wlfe->shell_surface, wlfe->seat, serial);
    }
  }
}

static void
pointer_handle_axis(void *data, struct wl_pointer *wl_pointer,
                    uint32_t time, uint32_t axis, wl_fixed_t value)
{
}

static void pointer_handle_frame(void *data, struct wl_pointer *wl_pointer)
{
}

static void
pointer_handle_axis_source(void *data, struct wl_pointer *wl_pointer, uint32_t axis_source)
{
}

static void
pointer_handle_axis_stop(void *data, struct wl_pointer *wl_pointer,
                         uint32_t time, uint32_t axis)
{
}

static void
pointer_handle_axis_discrete(void *data, struct wl_pointer *wl_pointer,
                              uint32_t axis, int32_t discrete)
{
}

static const struct wl_pointer_listener pointer_listener = {
  pointer_handle_enter,
  pointer_handle_leave,
  pointer_handle_motion,
  pointer_handle_button,
  pointer_handle_axis,
  pointer_handle_frame,
  pointer_handle_axis_source,
  pointer_handle_axis_stop,
  pointer_handle_axis_discrete,
};

/*
 * touch
 */

static void
touch_handle_down(void *data, struct wl_touch *wl_touch,
                  uint32_t serial, uint32_t time, struct wl_surface *surface,
                  int32_t id, wl_fixed_t x_w, wl_fixed_t y_w)
{
  wlfe_t *wlfe = data;

  wlfe->touch_x = x_w;
  wlfe->touch_y = y_w;

  /* handle fullscreen touch controls */
  if (wlfe->fullscreen) {
    if (_handle_touch(wlfe, wlfe->touch_x, wlfe->touch_y))
      return;
  }

  /* move window in windowed mode */
  if (!wlfe->fullscreen) {
    if (wlfe->shell_surface)
      wl_shell_surface_move(wlfe->shell_surface, wlfe->seat, serial);
  }
}

static void
touch_handle_up(void *data, struct wl_touch *wl_touch,
                uint32_t serial, uint32_t time, int32_t id)
{
}

static void
touch_handle_motion(void *data, struct wl_touch *wl_touch,
                    uint32_t time, int32_t id, wl_fixed_t x_w, wl_fixed_t y_w)
{
  wlfe_t *wlfe = data;

  wlfe->touch_x = x_w;
  wlfe->touch_y = y_w;
}

static void
touch_handle_frame(void *data, struct wl_touch *wl_touch)
{
}

static void
touch_handle_cancel(void *data, struct wl_touch *wl_touch)
{
}

#ifdef WL_TOUCH_SHAPE_SINCE_VERSION
static void
touch_handle_shape(void *data,
                   struct wl_touch *wl_touch,
                   int32_t id,
                   wl_fixed_t major,
                   wl_fixed_t minor)
{
}
#endif
#ifdef WL_TOUCH_ORIENTATION_SINCE_VERSION
static void
touch_handle_orientation(void *data,
                         struct wl_touch *wl_touch,
                         int32_t id,
                         wl_fixed_t orientation)
{
}
#endif

static const struct wl_touch_listener touch_listener = {
  touch_handle_down,
  touch_handle_up,
  touch_handle_motion,
  touch_handle_frame,
  touch_handle_cancel,
#ifdef WL_TOUCH_SHAPE_SINCE_VERSION
  touch_handle_shape,
#endif
#ifdef WL_TOUCH_ORIENTATION_SINCE_VERSION
  touch_handle_orientation,
#endif
};

/*
 * Keyboard
 */

static void
keyboard_handle_keymap(void *data, struct wl_keyboard *keyboard,
                       uint32_t format, int fd, uint32_t size)
{
}

static void
keyboard_handle_enter(void *data, struct wl_keyboard *keyboard,
                      uint32_t serial, struct wl_surface *surface,
                      struct wl_array *keys)
{
}

static void
keyboard_handle_leave(void *data, struct wl_keyboard *keyboard,
                      uint32_t serial, struct wl_surface *surface)
{
}

static void
keyboard_handle_key(void *data, struct wl_keyboard *keyboard,
                    uint32_t serial, uint32_t time, uint32_t key,
                    uint32_t state)
{
  wlfe_t *wlfe = data;
  const char *fe_event = NULL;

  if (state == WL_KEYBOARD_KEY_STATE_PRESSED)
  switch (key) {

    case KEY_ESC:
      if (!wlfe->fe.fe_message_cb) /* ESC exits only in remote mode */
        fe_event = "QUIT";
      break;
    case KEY_F11:
      fe_event = "TOGGLE_FULLSCREEN";
      break;

    case KEY_F:
      if (wlfe->gui_hotkeys)
        fe_event = "TOGGLE_FULLSCREEN";
      break;
    case KEY_D:
      if (wlfe->gui_hotkeys)
        fe_event = "TOGGLE_DEINTERLACE";
      break;
    case KEY_P:
      if (wlfe->gui_hotkeys)
        fe_event = "POWER_OFF";
      break;

    case KEY_ENTER: wlfe->fe.send_input_event(&wlfe->fe, NULL, "Ok", 0, 0); break;
    case KEY_BACKSPACE: wlfe->fe.send_input_event(&wlfe->fe, NULL, "Back", 0, 0); break;
    case KEY_LEFT:  wlfe->fe.send_input_event(&wlfe->fe, NULL, "Left", 0, 0); break;
    case KEY_RIGHT: wlfe->fe.send_input_event(&wlfe->fe, NULL, "Right", 0, 0); break;
    case KEY_UP:    wlfe->fe.send_input_event(&wlfe->fe, NULL, "Up", 0, 0); break;
    case KEY_DOWN:  wlfe->fe.send_input_event(&wlfe->fe, NULL, "Down", 0, 0); break;

    case KEY_F1:  wlfe->fe.send_input_event(&wlfe->fe, NULL, "Menu", 0, 0); break;
    case KEY_F2:  wlfe->fe.send_input_event(&wlfe->fe, NULL, "Red", 0, 0); break;
    case KEY_F3:  wlfe->fe.send_input_event(&wlfe->fe, NULL, "Green", 0, 0); break;
    case KEY_F4:  wlfe->fe.send_input_event(&wlfe->fe, NULL, "Yellow", 0, 0); break;
    case KEY_F5:  wlfe->fe.send_input_event(&wlfe->fe, NULL, "Blue", 0, 0); break;

    case KEY_SPACE: wlfe->fe.send_input_event(&wlfe->fe, NULL, "Pause", 0, 0); break;

    case KEY_0:  wlfe->fe.send_input_event(&wlfe->fe, NULL, "0", 0, 0); break;
    case KEY_1:  wlfe->fe.send_input_event(&wlfe->fe, NULL, "1", 0, 0); break;
    case KEY_2:  wlfe->fe.send_input_event(&wlfe->fe, NULL, "2", 0, 0); break;
    case KEY_3:  wlfe->fe.send_input_event(&wlfe->fe, NULL, "3", 0, 0); break;
    case KEY_4:  wlfe->fe.send_input_event(&wlfe->fe, NULL, "4", 0, 0); break;
    case KEY_5:  wlfe->fe.send_input_event(&wlfe->fe, NULL, "5", 0, 0); break;
    case KEY_6:  wlfe->fe.send_input_event(&wlfe->fe, NULL, "6", 0, 0); break;
    case KEY_7:  wlfe->fe.send_input_event(&wlfe->fe, NULL, "7", 0, 0); break;
    case KEY_8:  wlfe->fe.send_input_event(&wlfe->fe, NULL, "8", 0, 0); break;
    case KEY_9:  wlfe->fe.send_input_event(&wlfe->fe, NULL, "9", 0, 0); break;

    default:;
  }

  if (fe_event) {
    wlfe->fe.send_event(&wlfe->fe, fe_event);
    return;
  }
}

static void
keyboard_handle_modifiers(void *data, struct wl_keyboard *keyboard,
                          uint32_t serial, uint32_t mods_depressed,
                          uint32_t mods_latched, uint32_t mods_locked,
                          uint32_t group)
{
}

static void
keyboard_handle_repeat_info(void *data,
                            struct wl_keyboard *wl_keyboard,
                            int32_t rate,
                            int32_t delay)
{
}

static const struct wl_keyboard_listener keyboard_listener = {
  keyboard_handle_keymap,
  keyboard_handle_enter,
  keyboard_handle_leave,
  keyboard_handle_key,
  keyboard_handle_modifiers,
  keyboard_handle_repeat_info,
};

/*
 * seat
 */

static void
seat_handle_capabilities(void *data, struct wl_seat *seat,
                         enum wl_seat_capability caps)
{
  wlfe_t *wlfe = data;

  if ((caps & WL_SEAT_CAPABILITY_POINTER) && !wlfe->pointer) {
    wlfe->pointer = wl_seat_get_pointer(seat);
    wl_pointer_add_listener(wlfe->pointer, &pointer_listener, wlfe);
  } else if (!(caps & WL_SEAT_CAPABILITY_POINTER) && wlfe->pointer) {
      wl_pointer_destroy(wlfe->pointer);
      wlfe->pointer = NULL;
  }

  if ((caps & WL_SEAT_CAPABILITY_KEYBOARD) && !wlfe->keyboard) {
    wlfe->keyboard = wl_seat_get_keyboard(seat);
    wl_keyboard_add_listener(wlfe->keyboard, &keyboard_listener, wlfe);
  } else if (!(caps & WL_SEAT_CAPABILITY_KEYBOARD) && wlfe->keyboard) {
    wl_keyboard_destroy(wlfe->keyboard);
    wlfe->keyboard = NULL;
  }

  if ((caps & WL_SEAT_CAPABILITY_TOUCH) && !wlfe->touch) {
    wlfe->touch = wl_seat_get_touch(seat);
    wl_touch_add_listener(wlfe->touch, &touch_listener, wlfe);
  } else if (!(caps & WL_SEAT_CAPABILITY_TOUCH) && wlfe->touch) {
    wl_touch_destroy(wlfe->touch);
    wlfe->touch = NULL;
  }
}

static void seat_handle_name(void *data, struct wl_seat *wl_seat, const char *name)
{
}

static const struct wl_seat_listener seat_listener = {
  seat_handle_capabilities,
  seat_handle_name,
};

/*
 * output
 */

static void output_geometry(void *data,
                            struct wl_output *wl_output,
                            int32_t x,
                            int32_t y,
                            int32_t physical_width,
                            int32_t physical_height,
                            int32_t subpixel,
                            const char *make,
                            const char *model,
                            int32_t transform)
{
}

static void output_mode(void *data,
                        struct wl_output *wl_output,
                        uint32_t flags,
                        int32_t width,
                        int32_t height,
                        int32_t refresh)
{
}

static void output_done(void *data,
                        struct wl_output *wl_output)
{
}

static void output_scale(void *data,
                         struct wl_output *wl_output,
                         int32_t factor)
{
  wlfe_t *wlfe = data;
  fprintf(stderr, "scale %d\n", factor);

  wlfe->factor=factor;
  if (wlfe->surface) {
    wl_surface_set_buffer_scale(wlfe->surface, factor);
    wl_surface_commit(wlfe->surface);
  }
}

static const struct wl_output_listener output_listener = {
  output_geometry,
  output_mode,
  output_done,
  output_scale,
};

/*
 * registry
 */

static void
global_registry_handler(void *data, struct wl_registry *registry, uint32_t id,
                        const char *interface, uint32_t version)
{
  wlfe_t *wlfe = data;

  fprintf(stderr, "*** %s %d\n", interface, version);
  if (!strcmp(interface, "wl_compositor")) {
    wlfe->compositor = wl_registry_bind(registry,
                                        id,
                                        &wl_compositor_interface,
                                        3);
  } else if (strcmp(interface, "wl_shell") == 0) {
    wlfe->shell = wl_registry_bind(registry, id,
                                   &wl_shell_interface, 1);

  } else if (strcmp(interface, "wl_output") == 0) {
    wlfe->output = wl_registry_bind(registry, id,
                                          &wl_output_interface, 2);
    wl_output_add_listener(wlfe->output, &output_listener, wlfe);

  } else if (strcmp(interface, "wl_seat") == 0) {
    wlfe->seat = wl_registry_bind(registry, id,
                                  &wl_seat_interface, 1);
    wl_seat_add_listener(wlfe->seat, &seat_listener, wlfe);

  } else if (strcmp(interface, "wl_shm") == 0) {
    wlfe->shm = wl_registry_bind(registry, id,
                                 &wl_shm_interface, 1);

    wlfe->cursor_theme = wl_cursor_theme_load(NULL, 32, wlfe->shm);
    wlfe->current_cursor = wl_cursor_theme_get_cursor(wlfe->cursor_theme, "left_ptr");
  }
}

static void
global_registry_remover(void *data, struct wl_registry *registry, uint32_t id)
{
}

static const struct wl_registry_listener registry_listener = {
  global_registry_handler,
  global_registry_remover
};

/*
 * Shell surface
 */

static void
handle_ping(void *data, struct wl_shell_surface *shell_surface, uint32_t serial)
{
  wl_shell_surface_pong(shell_surface, serial);
}

static void
handle_configure(void *data, struct wl_shell_surface *shell_surface,
                 uint32_t edges, int32_t width, int32_t height)
{
  wlfe_t *wlfe = data;
  struct wl_region *region;

  /* store virtual size for pointer/touch mapping */
  wlfe->shell_w = width;
  wlfe->shell_h = height;

  region = wl_compositor_create_region(wlfe->compositor);
  wl_region_add(region, 0, 0, width, height);
  wl_surface_set_opaque_region(wlfe->surface, region);
  wl_surface_set_input_region(wlfe->surface, region);
  wl_region_destroy(region);

  /* tell compositor we handle scaling (render HiDPI at output resolution) */
  wl_surface_set_buffer_scale(wlfe->surface, wlfe->factor);

  wl_surface_commit(wlfe->surface);

  /* Output resolution for xine-lib video */
  wlfe->x.width  = width * wlfe->factor;
  wlfe->x.height = height * wlfe->factor;

  /* Tell size to VDR (OSD size) */
  char str[64];
  snprintf(str, sizeof(str), "INFO WINDOW %dx%d", wlfe->x.width, wlfe->x.height);
  wlfe->x.fe.send_event(&wlfe->fe, str);

  if (wlfe->initial_fullscreen_req) {
    /* handle initial fullscreen mode */
    wlfe->initial_fullscreen_req = 0;
    wlfe->x.toggle_fullscreen_cb(&wlfe->x, 1);
  }
}

static void
handle_popup_done(void *data, struct wl_shell_surface *shell_surface)
{
}

static const struct wl_shell_surface_listener shell_surface_listener = {
  handle_ping,
  handle_configure,
  handle_popup_done,
};

/*
 * wlfe_display_open
 */

static int wlfe_display_open(frontend_t *this_gen,
                             int xpos, int ypos,
                             int width, int height, int fullscreen, int hud, int opengl,
                             int modeswitch, const char *modeline, int aspect,
                             int no_x_kbd, int gui_hotkeys, int touchscreen,
                             const char *video_port, int scale_video,
                             const char *aspect_controller, int window_id)
{
  wlfe_t *wlfe = (wlfe_t*)this_gen;

  if (!wlfe) {
    return 0;
  }

  if (wlfe->display) {
    wlfe->fe.fe_display_close(this_gen);
  }

  if(wlfe->fe.fe_message_cb) {
    wlfe->fe.fe_message_cb(wlfe->fe.fe_message_h, "KBD", "");
  }

  LOGDBG("wlfe_display_open(width=%d, height=%d, fullscreen=%d, display=%s)",
         width, height, fullscreen, video_port);

  wlfe->x.xpos          = xpos;
  wlfe->x.ypos          = ypos;
  wlfe->x.width         = width;
  wlfe->x.height        = height;
  wlfe->x.aspect        = aspect;
  wlfe->x.scale_video   = scale_video;
  wlfe->x.overscan      = 0;
  wlfe->x.display_ratio = 1.0;
  wlfe->x.aspect_controller = aspect_controller ? strdup(aspect_controller) : NULL;

  wlfe->fullscreen      = fullscreen;
  wlfe->touchscreen     = touchscreen;
  wlfe->gui_hotkeys     = gui_hotkeys;

  wlfe->factor = 1;
  wlfe->mousecursor_timeout = 5000;

  if (video_port && *video_port) {
    wlfe->display = wl_display_connect(video_port);
    if (!wlfe->display)
      LOGERR("wlfe_display_open: failed to connect to Wayland server '%s'", video_port);
  }

  if (!wlfe->display) {
    video_port = getenv("WAYLAND_DISPLAY");
    if (video_port && *video_port) {
      wlfe->display = wl_display_connect(video_port);
      if (!wlfe->display)
        LOGERR("wlfe_display_open: failed to connect to Wayland server '%s'", video_port);
    }
  }

  if (!wlfe->display) {
    wlfe->display = wl_display_connect(NULL);
  }

  if (!wlfe->display) {
    wlfe->display = wl_display_connect("wayland-0");
    if (!wlfe->display)
      LOGERR("wlfe_display_open: failed to connect to Wayland server 'wayland-0'");
  }

  if (!wlfe->display) {
    LOGERR("Failed to connect to Wayland server, giving up");
    return 0;
  }

  wlfe->registry = wl_display_get_registry(wlfe->display);
  wl_registry_add_listener(wlfe->registry, &registry_listener, wlfe);

  wl_display_dispatch(wlfe->display);
  wl_display_roundtrip(wlfe->display);

  if (!wlfe->compositor) {
    LOGERR("Can't find Wayland compositor");
    return 0;
  }

  wlfe->surface = wl_compositor_create_surface(wlfe->compositor);
  if (!wlfe->surface) {
    LOGERR("Can't create Wayland surface");
    return 0;
  }

  wlfe->shell_surface = wl_shell_get_shell_surface(wlfe->shell, wlfe->surface);
  if (!wlfe->shell_surface) {
    LOGERR("Can't create shell surface");
    exit(1);
  }
  wl_shell_surface_add_listener(wlfe->shell_surface,
                                &shell_surface_listener, wlfe);

  wl_surface_set_opaque_region(wlfe->surface, NULL);
  wl_shell_surface_set_toplevel(wlfe->shell_surface);
  wl_shell_surface_set_title(wlfe->shell_surface, "VDR");

  /* Window size comes from attached buffer. This is set by EGL deep inside xine. */
  /* -> go to fullscreen later ... to get the initial window size right. */
  wlfe->initial_fullscreen_req = fullscreen;

  wlfe->cursor_surface = wl_compositor_create_surface(wlfe->compositor);

  /* setup xine visual */
  wlfe->x.xine_visual_type = XINE_VISUAL_TYPE_WAYLAND;
  wlfe->x.vis_wl.frame_output_cb = wlfe->x.frame_output_handler;
  wlfe->x.vis_wl.user_data = wlfe;
  wlfe->x.vis_wl.display = wlfe->display;
  wlfe->x.vis_wl.surface = wlfe->surface;

  return 1;
}

/*
 * fbfe_display_config
 *
 * configure windows
 */
static int wlfe_display_config(frontend_t *this_gen,
                               int xpos, int ypos,
                               int width, int height, int fullscreen,
                               int modeswitch, const char *modeline,
                               int aspect, int scale_video)
{
  wlfe_t *wlfe = (wlfe_t*)this_gen;

  if (!wlfe)
    return 0;

  wlfe->x.xpos        = xpos   >= 0 ? xpos   : wlfe->x.xpos;
  wlfe->x.ypos        = ypos   >= 0 ? ypos   : wlfe->x.ypos;
  wlfe->x.width       = width  >= 0 ? width  : wlfe->x.width;
  wlfe->x.height      = height >= 0 ? height : wlfe->x.height;

  wlfe->x.aspect      = aspect;
  wlfe->x.scale_video = scale_video >= 0 ? scale_video : wlfe->x.scale_video;
  wlfe->fullscreen    = fullscreen >= 0 ? fullscreen : wlfe->fullscreen;

  if (wlfe->fullscreen) {
    wl_shell_surface_set_fullscreen(wlfe->shell_surface,
                                    WL_SHELL_SURFACE_FULLSCREEN_METHOD_FILL,
                                    0, NULL);
  } else {
    wl_shell_surface_set_toplevel(wlfe->shell_surface);
  }

  return 1;
}

static void wlfe_toggle_fullscreen(fe_t *this_gen, int fullscreen)
{
  wlfe_t *wlfe = (wlfe_t*)this_gen;

  if (fullscreen < 0)
    fullscreen = !wlfe->fullscreen;

  wlfe->fe.fe_display_config(&wlfe->fe, -1, -1, -1, -1,
                             fullscreen, 0, NULL,
                             wlfe->x.aspect, wlfe->x.scale_video);
}

static void wlfe_interrupt(frontend_t *this_gen)
{
  /* stop fbfe_run() */
}

static void check_mouse_cursor_hide(wlfe_t *wlfe, int64_t elapsed)
{
  if (wlfe->mousecursor_timeout > 0) {
    if (elapsed > 0 && elapsed < 500) {
      wlfe->mousecursor_timeout -= elapsed;
      if (wlfe->mousecursor_timeout <= 0) {
        show_cursor(wlfe, wlfe->pointer, wlfe->pointer_enter_serial, 0);
      }
    }
  }
}

/*
 * main loop
 */
static int wlfe_run(frontend_t *this_gen)
{
  wlfe_t *wlfe = (wlfe_t*)this_gen;

  if (!wlfe || wlfe->fe.xine_is_finished(this_gen, 0) != FE_XINE_RUNNING)
    return 0;

  while (wl_display_prepare_read(wlfe->display) != 0) {
    wl_display_dispatch_pending(wlfe->display);
  }
  wl_display_flush(wlfe->display);

  const int poll_timeout = 50;
  struct pollfd pfd = {
    .fd = wl_display_get_fd(wlfe->display),
    .events = POLLIN,
  };

  uint64_t poll_time = 0;
  if (wlfe->mousecursor_timeout > 0) {
    poll_time = time_ms();
  }

  if (poll(&pfd, 1, poll_timeout) < 1 || !(pfd.revents & POLLIN)) {
    wl_display_cancel_read(wlfe->display);

    check_mouse_cursor_hide(wlfe, poll_timeout);

    return !wlfe->fe.xine_is_finished(&wlfe->fe, 0);
  }

  if (wlfe->mousecursor_timeout > 0) {
    check_mouse_cursor_hide(wlfe, elapsed(poll_time));
  }

  wl_display_read_events(wlfe->display);
  wl_display_dispatch_pending(wlfe->display);

  return 1;
}

static void wlfe_display_close(frontend_t *this_gen)
{
  wlfe_t *wlfe = (wlfe_t*)this_gen;

  if (!wlfe) {
    return;
  }

  if (wlfe->x.xine) {
    wlfe->fe.xine_exit(this_gen);
  }

  if (wlfe->display) {
    if (wlfe->touch)
      wl_touch_destroy(wlfe->touch);
    wlfe->touch = NULL;
    if (wlfe->keyboard)
      wl_keyboard_destroy(wlfe->keyboard);
    wlfe->keyboard = NULL;
    if (wlfe->pointer)
      wl_pointer_destroy(wlfe->pointer);
    wlfe->pointer = NULL;
    if (wlfe->seat)
      wl_seat_destroy(wlfe->seat);
    wlfe->seat = NULL;
    if (wlfe->output)
      wl_output_destroy(wlfe->output);
    wlfe->output = NULL;

    if (wlfe->cursor_surface)
      wl_surface_destroy(wlfe->cursor_surface);
    wlfe->cursor_surface = NULL;
    if (wlfe->cursor_theme)
      wl_cursor_theme_destroy(wlfe->cursor_theme);
    wlfe->cursor_theme = NULL;

    if (wlfe->surface)
      wl_surface_destroy(wlfe->surface);
    wlfe->surface = NULL;

    if (wlfe->shell_surface)
      wl_shell_surface_destroy(wlfe->shell_surface);
    wlfe->shell_surface = NULL;
    if (wlfe->shell)
      wl_shell_destroy(wlfe->shell);
    wlfe->shell = NULL;

    if (wlfe->shm)
      wl_shm_destroy(wlfe->shm);
    wlfe->shm = NULL;
    if (wlfe->compositor)
      wl_compositor_destroy(wlfe->compositor);
    wlfe->compositor = NULL;

    if (wlfe->registry)
      wl_registry_destroy(wlfe->registry);
    wlfe->registry = NULL;

    wl_display_disconnect(wlfe->display);
    wlfe->display = NULL;
  }

  free(wlfe->x.video_port_name);
  wlfe->x.video_port_name = NULL;

  free(wlfe->x.aspect_controller);
  wlfe->x.aspect_controller = NULL;
}

static frontend_t *wlfe_get_frontend(void)
{
  wlfe_t *wlfe = calloc(1, sizeof(wlfe_t));

  if (!wlfe)
    return NULL;

  init_fe(&wlfe->x);

  wlfe->fe.fe_display_open   = wlfe_display_open;
  wlfe->fe.fe_display_config = wlfe_display_config;
  wlfe->fe.fe_display_close  = wlfe_display_close;

  wlfe->fe.fe_run       = wlfe_run;
  wlfe->fe.fe_interrupt = wlfe_interrupt;

  wlfe->x.toggle_fullscreen_cb = wlfe_toggle_fullscreen;

  return &wlfe->fe;
}

/* ENTRY POINT */
const fe_creator_f fe_creator __attribute__((visibility("default"))) = wlfe_get_frontend;


