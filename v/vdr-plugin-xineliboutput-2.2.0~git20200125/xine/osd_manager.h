/*
 * osd_manager.h:
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 *
 */

#ifndef XINELIBOUTPUT_OSD_MANAGER_H_
#define XINELIBOUTPUT_OSD_MANAGER_H_

/*
 * OSD manager executes OSD control messages from VDR.
 * - cache OSD data
 * - scale OSD when required
 * - re-scale OSD when video size changes
 * - generate xine overlay events
 */

#include <xine.h>

struct osd_command_s;

typedef struct osd_manager_s osd_manager_t;

struct osd_manager_s {
  int  (*command)(osd_manager_t *, struct osd_command_s *, xine_stream_t *);
  void (*dispose)(osd_manager_t *, xine_stream_t *);

  void (*video_size_changed)(osd_manager_t *, xine_stream_t *, int width, int height);

  int  (*argb_supported)(xine_stream_t *);
};

osd_manager_t *init_osd_manager(void);


#endif /* XINELIBOUTPUT_OSD_MANAGER_H_ */
