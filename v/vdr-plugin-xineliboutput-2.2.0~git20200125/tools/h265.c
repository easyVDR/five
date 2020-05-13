/*
 * h265.c: H.265 bitstream decoding
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 *
 */

#include <stdint.h>
#include <string.h>

#ifndef LOG_MODULENAME
#  define LOG_MODULENAME "[h265     ] "
#  include "../logdefs.h"
#endif

#define NOCACHE 1
#include "bitstream.h"

#include "h265.h"

static int h265_parse_sps(const uint8_t *buf, size_t len, h265_sps_data_t *sps)
{
  br_state br = BR_INIT(buf, len);
  unsigned i;

  /* sps_video_parameter_set_id = */ br_skip_bits(&br, 4);
  uint8_t sps_max_sub_layers_minus1 = br_get_bits(&br,3);

  br_skip_bits(&br, 1 + 8 + 32 + 4 + 43 + 1 + 8);

  uint8_t sub_layer_profile_present_flag[8] = {0,0,0,0, 0,0,0,0};
  uint8_t sub_layer_level_present_flag[8] = {0,0,0,0, 0,0,0,0};
  for (i = 0; i < sps_max_sub_layers_minus1; i++) {
    sub_layer_profile_present_flag[i] = br_get_bit(&br);
    sub_layer_level_present_flag[i] = br_get_bit(&br);
  }

  if (sps_max_sub_layers_minus1 > 0) {
    for (i = sps_max_sub_layers_minus1; i < 8; i++) {
      br_skip_bits(&br, 2);
    }
  }

  for (i = 0; i < sps_max_sub_layers_minus1; i++) {
    if (sub_layer_profile_present_flag[i]) {
      br_skip_bits(&br, 8 + 32 + 4 + 43 + 1);
    }
    if (sub_layer_level_present_flag[i]) {
      br_skip_bits(&br, 8);
    }
  }

  /* sps_seq_parameter_set_id = */br_skip_ue_golomb(&br);
  unsigned int chroma_format_idc = br_get_ue_golomb(&br);

  if (chroma_format_idc == 3) {
    /* separate_colour_plane_flag = */br_skip_bit(&br);
  }

  sps->width  = br_get_ue_golomb(&br);
  sps->height = br_get_ue_golomb(&br);

  if (BR_EOF(&br)) {
    LOGMSG("h265_parse_sps: not enough data ?");
    return 0;
  }

  return 1;
}

static int h265_nal_unescape(uint8_t *dst, const uint8_t *src, size_t len)
{
  size_t s = 0, d = 0;
  while (s < len - 3) {

    if (!src[s] && !src[s+1] && src[s+2]) {
      /* hit 00 00 xx */
      dst[d] = dst[d+1] = 0;
      s += 2;
      d += 2;

      if (src[s] == 3) {
        s++; /* 00 00 03 xx --> 00 00 xx */
        /*LOGDBG("h265_nal_unescape: hit 00 00 03 %02x", src[s]);*/
        if (s >= len)
          return d;
      } /* else if (src[s] == 0 || src[s] == 1) {
        LOGDBG("h265_nal_unescape: invalid NAL sequence 00 00 %02x %02x", src[s], src[s+1]);
        return -1;
      }*/
    } else {
      dst[d++] = src[s++];
    }
  }
  return d;
}

/*
 * input: start of H.265 video data (not PES)
 */
int h265_get_video_size(const uint8_t *buf, size_t len, struct video_size_s *size)
{
  size_t i;

  /* scan video packet for sequence parameter set */
  if (len > 5)
  for (i = 5; i < len-5; i++) {
    if (buf[i] == 0 && buf[i + 1] == 0 && buf[i + 2] == 1 &&
        (buf[i + 3] >> 1) == H265_NAL_SPS) {

      uint8_t nal_data[len];
      int     nal_len;

      LOGDBG("H.265: Found NAL SPS at offset %zu/%zu", i, len);
      if (0 < (nal_len = h265_nal_unescape(nal_data, buf+i+5, len-i-5))) {
        h265_sps_data_t sps = {0, 0};

        if (h265_parse_sps(nal_data, nal_len, &sps)) {
          size->width  = sps.width;
          size->height = sps.height;

          /* XXX */
          size->pixel_aspect.num = 1;
          size->pixel_aspect.den = 1;
          return 1;
        }
        LOGMSG("h265_get_video_size: not enough data ?");
      }
    }
  }

  return 0;
}
