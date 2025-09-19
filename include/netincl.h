/* $Id$ */

#ifndef __NETINCL_H__
#define __NETINCL_H__

#ifdef _WIN32
#include <winsock2.h>
#include <windns.h>
#include <in6addr.h>
#include <ws2ipdef.h>

#define net_close closesocket
#define net_write(x,y,z) send(x,y,z,0)
#define net_read(x,y,z) recv(x,y,z,0)
#else
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <netdb.h>

#define net_close close
#define net_write(x,y,z) write(x,y,z)
#define net_read(x,y,z) read(x,y,z)
#endif

#ifdef USE_CYRUS_SASL2
#include <sasl/sasl.h>
#endif

#ifdef USE_SSL
#include <openssl/ssl.h>
#endif

#endif
