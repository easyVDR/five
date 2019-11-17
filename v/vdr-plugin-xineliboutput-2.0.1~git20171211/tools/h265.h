/*
 * h265.h:
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 *
 */

#ifndef _XINELIBOUTPUT_H265_H_
#define _XINELIBOUTPUT_H265_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

#include "mpeg.h"


#define H265_NAL_SPS      0x21  /* Sequence Parameter Set */
#define H265_NAL_AUD      0x23  /* Access Unit Delimiter */
#define H265_NAL_EOS_NUT  0x24  /* End of Sequence */
#define H265_NAL_EOB_NUT  0x25  /* End of bitstream */


#if defined(__i386__) || defined(__x86_64__)
#  define IS_H265_NAL_AUD(buf)     (*(const uint32_t *)(buf) == 0x46010000U)
#else
#  define IS_H265_NAL_AUD(buf)     ((buf)[0] == 0 && (buf)[1] == 0 && (buf)[2] == 1 && (buf)[3] == (H265_NAL_AUD<<1))
#endif

typedef struct {
  uint16_t        width;
  uint16_t        height;
  /* ... */
} h265_sps_data_t;

/*
 * input: start of H.265 video data (not PES)
 */
int  h265_get_video_size(const uint8_t *buf, size_t len, struct video_size_s *size);


#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* _XINELIBOUTPUT_H265_H_ */
