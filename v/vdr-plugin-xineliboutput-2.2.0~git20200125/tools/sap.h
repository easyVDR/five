/*
 * sap.h: RFC2974 Session Announcement Protocol (SAP) version 2
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 *
 */

#ifndef XINELIBOUTPUT_SAP_H_
#define XINELIBOUTPUT_SAP_H_

#include <stdint.h>

/* SAP IPv4 multicast addresses */
#define SAP_IP_ADDRESS_GLOBAL "224.2.127.254"   /* SAPv1 IP4 global scope multicast address */
#define SAP_IP_ADDRESS_ORG    "239.195.255.255" /* organization-local */
#define SAP_IP_ADDRESS_LOCAL  "239.255.255.255" /* local */
#define SAP_IP_ADDRESS_LINK   "224.0.0.255"     /* link-local */


int sap_send_announce(int *pfd, uint32_t dst_addr,
                      uint32_t src_ip, uint16_t msgid,
                      int announce,
                      const char *payload_type, const char *payload);

#endif /* XINELIBOUTPUT_SAP_H_ */
