/*
 * sock_compat.h
 *
 * Socket compatibility layer for winsock
 *
 * See the main source file 'xineliboutput.c' for copyright information and
 * how to reach the author.
 */

#ifdef _WIN32

# include <winsock2.h>
# include <ws2tcpip.h>  // socklen_t

#  ifndef MSG_TRUNC
#    define MSG_TRUNC 0
#  endif
#  ifndef SHUT_RDWR
#    define SHUT_RDWR SD_BOTH
#  endif

static inline ssize_t _recv(int sockfd, void *buf, size_t len, int flags)
{
  return recv(sockfd, buf, len, flags);
}

static inline ssize_t _recvfrom(int sockfd, void *buf, size_t len, int flags,
                         struct sockaddr *src_addr, socklen_t *addrlen)
{
  return recvfrom(sockfd, buf, len, flags, src_addr, addrlen);
}

static inline int _setsockopt(int fd, int level, int optname, const void *optval, socklen_t optlen)
{
  return setsockopt(fd, level, optname, optval, optlen);
}

static inline int _getsockopt(int fd, int level, int optname, void *optval, socklen_t *optlen)
{
  return getsockopt(fd, level, optname, optval, optlen);
}

static inline void sock_init(void)
{
  WSADATA data;
  int ret = WSAStartup( MAKEWORD( 1, 1 ), &data );
  if (ret) {
    LOGERR("can't initiate WinSocks, error %d\n", ret);
  }
}

static inline void sock_cleanup(void)
{
  WSACleanup();
}

#  define setsockopt(a,b,c,d,e) _setsockopt(a,b,c,d,e)
#  define getsockopt(a,b,c,d,e) _getsockopt(a,b,c,d,e)
#  define recvfrom(a,b,c,d,e,f) _recvfrom(a,b,c,d,e,f)
#  define recv(a,b,c,d)         _recv(a,b,c,d)

#else  /* _WIN32 */

#  include <sys/types.h>
#  include <sys/socket.h>

#  define closesocket(s) close(s)
#  define sock_init() do {} while(0)
#  define sock_cleanup() do {} while(0)

#endif /* _WIN32 */

static inline int sock_set_bool_opt(int s, int opt, int val)
{
#ifdef _WIN32
  BOOL _val = val;
  return setsockopt(s, SOL_SOCKET, opt, &_val, sizeof(_val));
#else
  return setsockopt(s, SOL_SOCKET, opt, &val, sizeof(val));
#endif
}

static inline int sock_set_broadcast(int s, int val)
{
  return sock_set_bool_opt(s, SO_BROADCAST, val);
}

static inline int sock_set_reuseaddr(int s, int val)
{
  return sock_set_bool_opt(s, SO_REUSEADDR, val);
}

#include <unistd.h>
#include <fcntl.h>

static inline int io_set_nonblock(int fd)
{
#ifdef _WIN32
  u_long one = 1;
  return ioctlsocket(fd, FIONBIO, &one);
#else
  int flags, result;

  flags = fcntl (fd, F_GETFL);
  if (flags < 0) {
    LOGERR("fcntl(F_GETFL) failed");
    return flags;
  }
  result = fcntl (fd, F_SETFL, flags | O_NONBLOCK);
  if (result < 0) {
    LOGERR("Failed setting fd to non-blocking mode");
  }
  return result;
#endif
}

