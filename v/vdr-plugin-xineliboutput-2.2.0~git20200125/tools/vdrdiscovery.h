/*
 * vdrdiscovery.h
 *
 * Simple broadcast protocol to search VDR with xineliboutput server 
 * from (local) network.
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 *
 * $Id$
 *
 */

#ifndef _VDRDISCOVERY_H_
#define _VDRDISCOVERY_H_

#define DISCOVERY_MSG_MAXSIZE  1024

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>    // size_t

struct sockaddr;

/*
 * Client interface
 */

typedef struct {
  char *host;   /* server address */
  int   port;   /* server port */
  char *descr;  /* server description (name, version) */
} vdr_server;

/*
 * udp_discovery_find_servers()
 *
 * Search for servers.
 * Return null-terminated array of pointers to server records, or NULL on error.
 * Application must free returned records with vdr_discovery_free_servers().
 *
 * parameters:
 *  fast  return immediately when first server is found.
 */
vdr_server **udp_discovery_find_servers(int fast);

/*
 * void udp_discovery_free_servers()
 *
 * Free array of server records returned from udp_discovery_find_servers().
 *
 */
void udp_discovery_free_servers(vdr_server ***);

/*
 * udp_discovery_find_server()
 *
 * Search for server. Return address and port.
 * Returns > 0 on success.
 */
int udp_discovery_find_server(int *port, char *address, size_t address_len);

/*
 * Server interface
 */

/*
 * udp_discovery_init()
 *
 * Initialize server socket. Return value is the socket file descriptor,
 * and can be used for polling.
 * Returns < 0 on error.
 */
int udp_discovery_init(void);

/*
 * udp_discovery_broadcast()
 *
 * Send server announcement.
 * Returns 0 on success.
 */
int udp_discovery_broadcast(int fd_discovery, int server_port, const char *server_address);

/*
 * udp_discovery_recv()
 *
 * Receive query or announcement.
 * Returns number of bytes received, <= 0 on error.
 * Result is null-terminated string, not more than DISCOVERY_MSG_MAXSIZE bytes.
 */
int udp_discovery_recv(int fd_discovery, char *buf, int timeout,
		       struct sockaddr *source, socklen_t *source_len);

/*
 * udp_discovery_is_valid_search()
 *
 * Check if string is valid search message.
 * Returns 1 for valid messages, 0 for invalid messages.
 */
int udp_discovery_is_valid_search(const char *buf);

#ifdef __cplusplus
};
#endif


#endif // _VDRDISCOVERY_H_
