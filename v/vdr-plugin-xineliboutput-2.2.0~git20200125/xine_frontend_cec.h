/*
 * xine_frontend_cec.h:
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 *
 */

#ifndef XINE_FRONTEND_CEC_H
#define XINE_FRONTEND_CEC_H

struct frontend_s;

typedef struct input_cec_s input_cec_t;

input_cec_t *cec_start(struct frontend_s *fe, int hdmi_port, int dev_type);
void cec_stop(input_cec_t **);

#endif /* XINE_FRONTEND_CEC_H */
