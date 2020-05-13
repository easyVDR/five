/*
 * sdp.h: RFC2974 Session Description Protocol (SDP)
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 *
 */

#ifndef XINELIBOUTPUT_SDP_H_
#define XINELIBOUTPUT_SDP_H_

#include <stdint.h>

#include <vdr/tools.h>  // cString

#define SDP_MIME_TYPE  "application/sdp"

#define SDP_PAYLOAD_MPEG_PES  96
#define SDP_PAYLOAD_MPEG_TS   33

cString vdr_sdp_description(const char *vdr_ip,
                            int vdr_svdrp_port,
                            int vdr_xineliboutput_port,
                            const char *rtp_ip,
                            uint32_t rtp_ssrc,
                            uint32_t payload_type,
                            int rtp_port,
                            int rtp_ttl);

#endif  /* XINELIBOUTPUT_SDP_H_ */
