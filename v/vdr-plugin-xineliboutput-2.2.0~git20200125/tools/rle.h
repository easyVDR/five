/*
 * rle.h: RLE utils
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 *
 */

#ifndef XINELIBOUTPUT_RLE_H_
#define XINELIBOUTPUT_RLE_H_

#if defined __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

typedef enum {
  scale_fast = 0,         /* simple pixel doubling/dropping */
  scale_good_BW = 1,      /* linear interpolation, palette re-generation */
} scale_mode_t;


struct osd_rle_elem_s;
struct osd_clut_s;


unsigned rle_compress(struct osd_rle_elem_s **rle_data, const uint8_t *data, unsigned w, unsigned h);
unsigned rle_recompress_net(uint8_t *raw, struct osd_rle_elem_s *data, unsigned elems);

void rle_palette_to_argb(uint32_t *argb, const struct osd_clut_s *palette, unsigned entries);
void rle_palette_to_rgba(uint32_t *rgba, const struct osd_clut_s *palette, unsigned entries);

void rle_uncompress_lut8(uint8_t *dst,
                         unsigned w, unsigned h, unsigned stride,
                         const struct osd_rle_elem_s *rle_data, unsigned num_rle);
void rle_uncompress_argb(uint32_t *dst,
                         unsigned w, unsigned h, unsigned stride,
                         const struct osd_rle_elem_s *rle_data, unsigned num_rle,
                         const struct osd_clut_s *palette, unsigned palette_entries);
void rle_uncompress_rgba(uint32_t *dst,
                         unsigned w, unsigned h, unsigned stride,
                         const struct osd_rle_elem_s *rle_data, unsigned num_rle,
                         const struct osd_clut_s *palette, unsigned palette_entries);

/*
 * rle_scale_nearest()
 *
 * - Simple nearest-neighbour scaling for RLE-compressed image
 * - fast scaling in compressed form without decompression
 */
struct osd_rle_elem_s *rle_scale_nearest(const struct osd_rle_elem_s *old_rle,
                                          int *rle_elems,
                                          unsigned w, unsigned h, unsigned new_w, unsigned new_h);


/*
 * HDMV (BluRay) presentation graphics format
 */

size_t rle_compress_hdmv(uint8_t **rle_data, const uint8_t *data, unsigned w, unsigned h, int *num_rle);
int rle_uncompress_hdmv(struct osd_rle_elem_s **data,
                        unsigned w, unsigned h,
                        const uint8_t *rle_data, unsigned num_rle, size_t rle_size);

/*
 * ARGB overlay compression
 */
size_t rle_compress_argbrle(uint8_t **rle_data, const uint32_t *data,
                            unsigned w, unsigned h, int *num_rle);
int rle_uncompress_argbrle(uint32_t *dst,
                           unsigned w, unsigned h, unsigned stride,
                           const uint8_t *rle_data, unsigned num_rle,
                           size_t rle_size);

#if defined __cplusplus
}
#endif

#endif /* XINELIBOUTPUT_RLE_H_ */
