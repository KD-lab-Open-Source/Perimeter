/******
nonport.h
GameSpy Common Code
  
Copyright 1999-2002 GameSpy Industries, Inc

18002 Skypark Circle
Irvine, California 92614
949.798.4200 (Tel)
949.798.4299 (Fax)
devsupport@gamespy.com
******/

#ifndef _NONPORT_H_
#define _NONPORT_H_

#ifndef GSI_MEM_ONLY

#if defined(applec) || defined(THINK_C) || defined(__MWERKS__) && !defined(__mips64) && !defined(_WIN32)
	#define _MACOS
#endif

#ifdef __mips64
	#if !defined(SN_SYSTEMS) && !defined(EENET)
		#define EENET
	#endif
	#define _PS2
#endif

#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <winsock.h>
#else
#ifdef _MACOS
	#include <events.h>
	#include "mwinsock.h"
	#define GS_BIG_ENDIAN
#else
#ifdef _PS2
	#define GS_BIG_ENDIAN
	#ifdef EENET
		#include <libeenet.h>
		#include <eenetctl.h>
		#include <ifaddrs.h>
		#include <sys/socket.h>
		#include <sys/errno.h>
		#include <netinet/in.h>
		#include <arpa/inet.h>
		#include <net/if.h>
		#include <sys/select.h>
		#include <malloc.h>
	#endif
	#include <eekernel.h>
	#include <stdio.h>
	#include <sifdev.h>
	#include <sifrpc.h>
	#include <sifcmd.h>
	#include <ilink.h>
	#include <ilsock.h>
	#include <ilsocksf.h>
	#ifdef SN_SYSTEMS	
		#include "sntypes.h"
		#include "snsocket.h"
		#include "sneeutil.h"
		#include "sntcutil.h"
	#endif
#else //UNIX
	#define UNDER_UNIX
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <stdio.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/ioctl.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#include <arpa/inet.h>
	#include <ctype.h>
	#include <errno.h>
	#include <sys/time.h>
	#include <limits.h>
	//#include <sys/syslimits.h>
	#include <netinet/tcp.h>
#endif
#endif
#endif

#ifdef UNDER_CE
#include <platutil.h>
#endif

#if !defined(UNDER_CE)
#include <assert.h>
#else
#define assert(a)
#endif

#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef GSI_MEM_TRACK
#define gsimalloc malloc
#define gsifree free
#define gsirealloc realloc
#else
void * gsimalloctrack(size_t size);
void gsifreetrack(void * ptr);
void * gsirealloctrack(void * ptr, size_t size);
#define gsimalloc gsimalloctrack
#define gsifree gsifreetrack
#define gsirealloc gsirealloctrack
#endif

#ifndef GSI_MEM_ONLY
	
// Get rid of compiler warnings when parameters are never used
#ifdef __MWERKS__
	#define GSI_UNUSED(x) x
#else
	#define GSI_UNUSED(x)
#endif

typedef int goa_int32;
typedef unsigned int goa_uint32;

unsigned long current_time();
void msleep(unsigned long msec);

void SocketStartUp();
void SocketShutDown();

#ifndef SOCKET_ERROR 
	#define SOCKET_ERROR (-1)
#endif

#ifndef INADDR_NONE
   #define INADDR_NONE 0xffffffff
#endif

#ifndef INVALID_SOCKET 
	#define INVALID_SOCKET (-1)
#endif

#if defined(_WIN32) && !defined(UNDER_CE)
	#define strcasecmp _stricmp
	#define strncasecmp _strnicmp
#else	
	char *_strlwr(char *string);
	char *_strupr(char *string);
#endif

#undef strdup
#define strdup goastrdup
#undef _strdup
#define _strdup goastrdup
char * goastrdup(const char *src);

#if defined(_MACOS) || defined(UNDER_CE)
	int strcasecmp(const char *string1, const char *string2);
	int strncasecmp(const char *string1, const char *string2, size_t count);
#endif

#ifdef SN_SYSTEMS
	#define IPPROTO_TCP PF_INET
	#define IPPROTO_UDP PF_INET
	#define FD_SETSIZE SN_MAX_SOCKETS
#endif

#if !defined(_MACOS) && !defined(_WIN32)
	#define SOCKET int
	
	#ifdef SN_SYSTEMS
		int GOAGetLastError(SOCKET s);
	#endif

	#ifdef EENET
		#define GOAGetLastError(s) sceEENetErrno
	#endif
	
	#ifdef UNDER_UNIX
		#define GOAGetLastError(s) errno
	#endif

	#ifdef EENET
		#define closesocket sceEENetClose
	#endif
	
	#ifdef UNDER_UNIX
		#define closesocket close //on unix
	#endif

	#define ioctlsocket ioctl

	#define WSAEWOULDBLOCK EWOULDBLOCK             
	#define WSAEINPROGRESS EINPROGRESS             
	#define WSAEALREADY EALREADY                
	#define WSAENOTSOCK ENOTSOCK                
	#define WSAEDESTADDRREQ EDESTADDRREQ            
	#define WSAEMSGSIZE EMSGSIZE                
	#define WSAEPROTOTYPE EPROTOTYPE              
	#define WSAENOPROTOOPT ENOPROTOOPT             
	#define WSAEPROTONOSUPPORT EPROTONOSUPPORT         
	#define WSAESOCKTNOSUPPORT ESOCKTNOSUPPORT         
	#define WSAEOPNOTSUPP EOPNOTSUPP              
	#define WSAEPFNOSUPPORT EPFNOSUPPORT            
	#define WSAEAFNOSUPPORT EAFNOSUPPORT            
	#define WSAEADDRINUSE EADDRINUSE              
	#define WSAEADDRNOTAVAIL EADDRNOTAVAIL           
	#define WSAENETDOWN ENETDOWN                
	#define WSAENETUNREACH ENETUNREACH             
	#define WSAENETRESET ENETRESET               
	#define WSAECONNABORTED ECONNABORTED            
	#define WSAECONNRESET ECONNRESET              
	#define WSAENOBUFS ENOBUFS                 
	#define WSAEISCONN EISCONN                 
	#define WSAENOTCONN ENOTCONN                
	#define WSAESHUTDOWN ESHUTDOWN               
	#define WSAETOOMANYREFS ETOOMANYREFS            
	#define WSAETIMEDOUT ETIMEDOUT               
	#define WSAECONNREFUSED ECONNREFUSED            
	#define WSAELOOP ELOOP                   
	#define WSAENAMETOOLONG ENAMETOOLONG            
	#define WSAEHOSTDOWN EHOSTDOWN               
	#define WSAEHOSTUNREACH EHOSTUNREACH            
	#define WSAENOTEMPTY ENOTEMPTY               
	#define WSAEPROCLIM EPROCLIM                
	#define WSAEUSERS EUSERS                  
	#define WSAEDQUOT EDQUOT                  
	#define WSAESTALE ESTALE                  
	#define WSAEREMOTE EREMOTE
	#define WSAEINVAL EINVAL
#else
	#define GOAGetLastError(s) WSAGetLastError()
#endif

#ifndef _WIN32
	typedef struct sockaddr SOCKADDR;
	typedef struct sockaddr_in SOCKADDR_IN;
	typedef struct in_addr IN_ADDR;
	typedef struct hostent HOSTENT;
	typedef struct timeval TIMEVAL;
#endif

#ifdef _WIN32
	#define PATHCHAR '\\'
#else
#ifdef MACOS
	#define PATHCHAR ':'
#else
	#define PATHCHAR '/'
#endif
#endif

int SetSockBlocking(SOCKET sock, int isblocking);
int DisableNagle(SOCKET sock);
int SetReceiveBufferSize(SOCKET sock, int size);
int SetSendBufferSize(SOCKET sock, int size);
int GetReceiveBufferSize(SOCKET sock);
int GetSendBufferSize(SOCKET sock);
int CanReceiveOnSocket(SOCKET sock);
int CanSendOnSocket(SOCKET sock);

HOSTENT * getlocalhost(void);

int IsPrivateIP(IN_ADDR * addr);

#if defined(UNDER_CE)
//CE does not have the stdlib time() call
	time_t time(time_t *timer);
#else
	#include <time.h>
#endif

// SN Systems doesn't support gethostbyaddr
#if defined(SN_SYSTEMS)
	#define gethostbyaddr(a,b,c)   NULL
#endif

#ifndef UNDER_CE
	#include <ctype.h>
#else
	int isdigit(int c);
	int isxdigit(int c);
	int isalnum(int c);
	int isspace(int c);
#endif

#if defined(UNDER_CE) || defined(_PS2)
	#define NOFILE
#endif

#ifndef SOMAXCONN
	#define SOMAXCONN 5
#endif

typedef const char * (* GetUniqueIDFunction)();

extern GetUniqueIDFunction GOAGetUniqueID;

#endif // #ifndef GSI_MEM_ONLY

#ifdef __cplusplus
}
#endif

#endif 
