/******
nonport.c
GameSpy Common Code
  
Copyright 1999-2002 GameSpy Industries, Inc

18002 Skypark Circle
Irvine, California 92614
949.798.4200 (Tel)
949.798.4299 (Fax)
devsupport@gamespy.com
******/

#include "nonport.h"

#if defined(_WIN32) && !defined(UNDER_CE) && !defined(_PS2)
#pragma comment(lib, "wsock32")
#pragma comment(lib, "advapi32")
#endif

#ifdef UNDER_CE
#pragma comment(lib, "platutil")
#pragma comment(lib, "winsock")
#endif

// Disable compiler warnings for issues that are unavoidable.
/////////////////////////////////////////////////////////////
#if defined(_MSC_VER) // DevStudio
// Level4, "conditional expression is constant". 
// Occurs with use of the MS provided macro FD_SET
#pragma warning ( disable: 4127 )
#endif // _MSC_VER

#ifdef GSI_MEM_TRACK
// GSI memory tracker
size_t gsimemtrack_total;
size_t gsimemtrack_peak;
int gsimemtrack_num;

void * gsimalloctrack(size_t size)
{
	void * ptr = malloc(size + sizeof(size_t));
	if(!ptr)
		return NULL;
	memcpy(ptr, &size, sizeof(size_t));
	gsimemtrack_total += size;
	gsimemtrack_peak = max(gsimemtrack_peak, gsimemtrack_total);
	gsimemtrack_num++;
	return (void *)((char *)ptr + sizeof(size_t));
}

void gsifreetrack(void * ptr)
{
	size_t size;
	if(!ptr)
		return;
	ptr = (void *)((char *)ptr - sizeof(size_t));
	memcpy(&size, ptr, sizeof(size_t));
	gsimemtrack_total -= size;
	gsimemtrack_num--;
	free(ptr);
}

void * gsirealloctrack(void * ptr, size_t size)
{
	size_t oldSize;
	void * oldPtr;
	if(ptr && !size)
	{
		gsifreetrack(ptr);
		return NULL;
	}
	if(!ptr)
	{
		return gsimalloctrack(size);
	}
	oldPtr = (void *)((char *)ptr - sizeof(size_t));
	memcpy(&oldSize, oldPtr, sizeof(size_t));
	ptr = realloc(oldPtr, size + sizeof(size_t));
	if(!ptr)
		return NULL;
	memcpy(ptr, &size, sizeof(size_t));
	gsimemtrack_total += (size - oldSize);
	gsimemtrack_peak = max(gsimemtrack_peak, gsimemtrack_total);
	return (void *)((char *)ptr + sizeof(size_t));
}
#endif

#ifdef UNDER_CE
time_t time(time_t *timer)
{
	static time_t ret;
	SYSTEMTIME systime;
	FILETIME ftime;
	LONGLONG holder;

	GetLocalTime(&systime);
	SystemTimeToFileTime(&systime, &ftime);
	holder = (ftime.dwHighDateTime << 32) + ftime.dwLowDateTime;
	if (timer == NULL)
		timer = &ret;
	*timer = (time_t)((holder - 116444736000000000) / 10000000);
	return *timer; 
}

int isdigit( int c )
{
	return (c >= '0' && c <= '9');
}

int isxdigit( int c )
{
	return ((c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f') || (c >= '0' && c <= '9'));
}

int isalnum( int c )
{
	return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9'));
}

int isspace(int c)
{
	return ((c >= 0x9 && c <= 0xD) || (c == 0x20)); 
}
#endif

#ifdef _PS2
extern int sceCdReadClock();

#ifndef __MWERKS__
typedef unsigned char u_char;
#endif

typedef struct {
        u_char stat;            /* status */
        u_char second;          /* second */
        u_char minute;          /* minute */
        u_char hour;            /* hour   */

        u_char pad;             /* pad    */
        u_char day;             /* day    */
        u_char month;           /* month  */
        u_char year;            /* year   */
} sceCdCLOCK;

static unsigned long GetTicks()
{
	unsigned long ticks;
	asm volatile (" mfc0 %0, $9 " : "=r" (ticks));
    return ticks;
}

#define DEC(x) (10*(x/16)+(x%16))
#define _BASE_YEAR 			 70L
#define _MAX_YEAR 			138L
#define _LEAP_YEAR_ADJUST 	 17L
int _days[] = {-1, 30, 58, 89, 119, 150, 180, 211, 242, 272, 303, 333, 364};

static time_t _gmtotime_t (
        int yr,     /* 0 based */
        int mo,     /* 1 based */
        int dy,     /* 1 based */
        int hr,
        int mn,
        int sc
        )
{
        int tmpdays;
        long tmptim;
        struct tm tb;

        if ( ((long)(yr -= 1900) < _BASE_YEAR) || ((long)yr > _MAX_YEAR) )
                return (time_t)(-1);

        tmpdays = dy + _days[mo - 1];
        if ( !(yr & 3) && (mo > 2) )
                tmpdays++;

        tmptim = (long)yr - _BASE_YEAR;

        tmptim = ( ( ( ( tmptim ) * 365L
                 + ((long)(yr - 1) >> 2) - (long)_LEAP_YEAR_ADJUST
                 + (long)tmpdays )
                 * 24L + (long)hr )
                 * 60L + (long)mn )
                 * 60L + (long)sc;

        tb.tm_yday = tmpdays;
        tb.tm_year = yr;
        tb.tm_mon = mo - 1;
        tb.tm_hour = hr;
        
        return (tmptim >= 0) ? (time_t)tmptim : (time_t)(-1);
}

time_t time(time_t *timer)
{
	time_t tim;
	sceCdCLOCK clocktime; /* defined in libcdvd.h */

	sceCdReadClock(&clocktime); /* libcdvd.a */

	tim =   _gmtotime_t ( DEC(clocktime.year)+2000,
							DEC(clocktime.month),
							DEC(clocktime.day),
							DEC(clocktime.hour),
							DEC(clocktime.minute),
							DEC(clocktime.second));

	if(timer)
		*timer = tim;
		
	return tim;
}

#endif /* _PS2 */

unsigned long current_time()  //returns current time in msec
{ 
#ifdef _WIN32
	return (GetTickCount()); 
#endif

#ifdef _MACOS
	return (TickCount() * 50) / 3;
#endif

#ifdef _PS2
	unsigned int ticks;
	static unsigned int msec = 0;
	static unsigned int lastticks = 0;
	sceCdCLOCK lasttimecalled; /* defined in libcdvd.h */

	if(!msec)
	{
		sceCdReadClock(&lasttimecalled); /* libcdvd.a */
		msec =  (DEC(lasttimecalled.day) * 86400000) +
				(DEC(lasttimecalled.hour) * 3600000) +
				(DEC(lasttimecalled.minute) * 60000) +
				(DEC(lasttimecalled.second) * 1000) + 
				(msec % 1000);
	}

	ticks = GetTicks();
	if(lastticks > ticks)
		msec += ((sizeof(unsigned int) - lastticks) + ticks) / 300000;
	else
		msec += (unsigned int)(ticks-lastticks) / 300000;
	lastticks = ticks;

	return msec;
#endif

#ifdef UNDER_UNIX
	struct timeval time;
	
	gettimeofday(&time, NULL);
	return (time.tv_sec * 1000 + time.tv_usec / 1000);
#endif
}

void msleep(unsigned long msec)
{
#ifdef _WIN32
	Sleep(msec);
#endif

#ifdef _MACOS
//	EventRecord rec;
	WaitNextEvent(everyEvent,/*&rec*/NULL, (msec*1000)/60, NULL);
#endif

#ifdef _PS2
	#ifdef SN_SYSTEMS
		sn_delay(msec);
	#endif
	#ifdef EENET
		if(msec >= 1000)
		{
			sleep(msec / 1000);
			msec -= (msec / 1000);
		}
		if(msec)
			usleep(msec * 1000);
	#endif
#endif

#ifdef UNDER_UNIX
	usleep(msec * 1000);
#endif
}

void SocketStartUp()
{
#if defined(_WIN32) || defined(_MACOS)
	WSADATA data;
	WSAStartup(0x0101, &data);
#endif
}

void SocketShutDown()
{
#if defined(_WIN32) || defined(_MACOS)
	WSACleanup();
#endif
}

char * goastrdup(const char *src)
{
	char *res;
	if(src == NULL)      //PANTS|02.11.00|check for NULL before strlen
		return NULL;
	res = (char *)gsimalloc(strlen(src) + 1);
	if(res != NULL)      //PANTS|02.02.00|check for NULL before strcpy
		strcpy(res, src);
	return res;
}

#if !defined(_WIN32) || defined(UNDER_CE)
#include <ctype.h>

char *_strlwr(char *string)
{
	char *hold = string;
	while (*string)
	{
		*string = tolower(*string);
		string++;
	}

	return hold;
}

char *_strupr(char *string)
{
	char *hold = string;
	while (*string)
	{
		*string = toupper(*string);
		string++;
	}

	return hold;
}
#endif

int SetSockBlocking(SOCKET sock, int isblocking)
{
	int rcode;
#ifdef EENET
	socklen_t argp;
#else
	unsigned long argp;
#endif
	
	if(isblocking)
		argp = 0;
	else
		argp = 1;

#ifdef _PS2
	#ifdef SN_SYSTEMS
		rcode = setsockopt(sock, SOL_SOCKET, (isblocking) ? SO_BIO : SO_NBIO, &argp, sizeof(argp));
	#endif

	#ifdef EENET
		rcode = setsockopt(sock, SOL_SOCKET, SO_NBIO, &argp, sizeof(argp));
	#endif

#else
	rcode = ioctlsocket(sock, FIONBIO, &argp);
#endif

	if(rcode == 0)
		return 1;

	return 0;
}

int DisableNagle(SOCKET sock)
{
#if defined(_WIN32) || defined(UNDER_UNIX)
	int rcode;
	int noDelay = 1;

	rcode = setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *)&noDelay, sizeof(int));

	return (rcode != SOCKET_ERROR);	
#else
	GSI_UNUSED(sock);
#endif

	// not supported
	return 0;
}

int SetReceiveBufferSize(SOCKET sock, int size)
{
	int rcode;

	rcode = setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (const char *)&size, sizeof(int));

	return (rcode != SOCKET_ERROR);
}

int SetSendBufferSize(SOCKET sock, int size)
{
	int rcode;

	rcode = setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (const char *)&size, sizeof(int));

	return (rcode != SOCKET_ERROR);
}

int GetReceiveBufferSize(SOCKET sock)
{
	int rcode;
	int size;
	int len;

	len = sizeof(size);
	rcode = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char *)&size, &len);
	if(rcode == SOCKET_ERROR)
		return -1;

	return size;
}

int GetSendBufferSize(SOCKET sock)
{
	int rcode;
	int size;
	int len;

	len = sizeof(size);
	rcode = getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char *)&size, &len);
	if(rcode == SOCKET_ERROR)
		return -1;

	return size;
}

int CanReceiveOnSocket(SOCKET sock)
{
	fd_set fd;
	struct timeval timeout;
	int rcode;
#ifdef SN_SYSTEMS
	int count = 0;
#endif

	// setup the fd set
	FD_ZERO(&fd);
	FD_SET(sock, &fd);

	// setup the timeout
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

#ifdef SN_SYSTEMS
repeat:
#endif

	// do the actual select
	rcode = select(FD_SETSIZE, &fd, NULL, NULL, &timeout);
	if((rcode == SOCKET_ERROR) || (rcode == 0))
		return 0;

#ifdef SN_SYSTEMS
	// check for an error, but don't get stuck forever
	if(GOAGetLastError(sock))
	{
		if(++count == 10)
			return 0;

		goto repeat;
	}
#endif

	// it was set
	return 1;
}

int CanSendOnSocket(SOCKET sock)
{
	fd_set fd;
	struct timeval timeout;
	int rcode;
#ifdef SN_SYSTEMS
	int count = 0;

repeat:
#endif

	// setup the fd set
	FD_ZERO(&fd);
	FD_SET(sock, &fd);

	// setup the timeout
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	// do the actual select
	rcode = select(FD_SETSIZE, NULL, &fd, NULL, &timeout);
	if((rcode == SOCKET_ERROR) || (rcode == 0))
		return 0;

#ifdef SN_SYSTEMS
	// check for an error, but don't get stuck forever
	if(GOAGetLastError(sock))
	{
		if(++count == 10)
			return 0;
		goto repeat;
	}
#endif

	// it was set
	return 1;
}

HOSTENT * getlocalhost(void)
{
#ifdef EENET
	#define MAX_IPS  5

	static HOSTENT localhost;
	static char * aliases = NULL;
	static char * ipPtrs[MAX_IPS + 1];
	static unsigned int ips[MAX_IPS];

	struct sceEENetIfname * interfaces;
	struct sceEENetIfname * interface;
	int num;
	int i;
	int count;
	int len;
	u_short flags;
	IN_ADDR address;

	// initialize the host
	localhost.h_name = "localhost";
	localhost.h_aliases = &aliases;
	localhost.h_addrtype = AF_INET;
	localhost.h_length = 0;
	localhost.h_addr_list = ipPtrs;

	// get the local interfaces
	sceEENetGetIfnames(NULL, &num);
	interfaces = (struct sceEENetIfname *)gsimalloc(num * sizeof(struct sceEENetIfname));
	if(!interfaces)
		return NULL;
	sceEENetGetIfnames(interfaces, &num);

	// loop through the interfaces
	count = 0;
	for(i = 0 ; i < num ; i++)
	{
		// the next interface
		interface = &interfaces[i];
		//printf("eenet%d: %s\n", i, interface->ifn_name);

		// get the flags
		len = sizeof(flags);
		if(sceEENetGetIfinfo(interface->ifn_name, sceEENET_IFINFO_IFFLAGS, &flags, &len) != 0)
			continue;
		//printf("eenet%d flags: 0x%X\n", i, flags);

		// check for up, running, and non-loopback
		if(!(flags & (IFF_UP|IFF_RUNNING)) || (flags & IFF_LOOPBACK))
			continue;
		//printf("eenet%d: up and running, non-loopback\n", i);

		// get the address
		len = sizeof(address);
		if(sceEENetGetIfinfo(interface->ifn_name, sceEENET_IFINFO_ADDR, &address, &len) != 0)
			continue;
		//printf("eenet%d: %s\n", i, inet_ntoa(address));

		// add this address
		ips[count] = address.s_addr;
		ipPtrs[count] = (char *)&ips[count];
		count++;
	}

	// free the interfaces
	gsifree(interfaces);

	// check that we got at least one IP
	if(!count)
		return NULL;

	// finish filling in the host struct
	localhost.h_length = (4 * count);
	ipPtrs[count] = NULL;

	return &localhost;
#else
	char hostname[256] = "";

	// get the local host's name
	gethostname(hostname, sizeof(hostname));

	// return the host for that name
	return gethostbyname(hostname);
#endif
}

int IsPrivateIP(IN_ADDR * addr)
{
	int b1;
	int b2;
	unsigned int ip;

	// get the first 2 bytes
	ip = ntohl(addr->s_addr);
	b1 = ((ip >> 24) & 0xFF);
	b2 = ((ip >> 16) & 0xFF);

	// 10.X.X.X
	if(b1 == 10)
		return 1;

	// 172.16-31.X.X
	if((b1 == 172) && ((b2 >= 16) && (b2 <= 31)))
		return 1;

	// 192.168.X.X
	if((b1 == 192) && (b2 == 168))
		return 1;

	return 0;
}

#if defined(_MACOS) || defined(UNDER_CE)
int strcasecmp(const char *string1, const char *string2)
{
	while (tolower(*string1) == tolower(*string2) && *string1 != 0 && *string2 != 0)
	{
		*string1++; *string2++;
	}
	return tolower(*string1) - tolower(*string2);
}

int strncasecmp(const char *string1, const char *string2, size_t count)
{
	while (--count > 0 && tolower(*string1) == tolower(*string2) && *string1 != 0 && *string2 != 0)
	{
		*string1++; *string2++;
	}
	return tolower(*string1) - tolower(*string2);
}
#endif

#ifdef SN_SYSTEMS
int GOAGetLastError(SOCKET s)
{
	int val;
	int soval = sizeof(val);
	getsockopt(s,SOL_SOCKET,SO_ERROR,&val,&soval);
	return val;
}
#endif

#ifdef UNDER_CE
const char * GOAGetUniqueID_Internal(void)
{
	static char keyval[17];
	unsigned char buff[8];
	DWORD size;

	size = 0;
	for (size = 0 ; size < sizeof(buff) ; size++)
		buff[size] = 0;
	size = sizeof(buff);
	FirmwareGetSerialNumber(buff, &size);
	for (size = 0 ; size < sizeof(buff) ; size++)
	{
		sprintf(keyval + (size * 2),"%02x",buff[size]);
	}
	return keyval;
}
#endif //UNDER_CE


#ifdef _PS2
#ifdef UNIQUEID

#if defined(EENET)

#include <net/if_dl.h>
#include <net/if_types.h>
#include <net/if_ether.h>

static const char * GetMAC(void)
{
	static struct sceEENetEtherAddr linkAddress;
	struct sceEENetIfname * interfaces;
	struct sceEENetIfname * interface;
	int num;
	int type;
	int len;
	int i;
	const unsigned char * MAC = NULL;

	// get the local interfaces
	sceEENetGetIfnames(NULL, &num);
	interfaces = (struct sceEENetIfname *)gsimalloc(num * sizeof(struct sceEENetIfname));
	if(!interfaces)
		return NULL;
	sceEENetGetIfnames(interfaces, &num);

	// loop through the interfaces
	for(i = 0 ; i < num ; i++)
	{
		// the next interface
		interface = &interfaces[i];
		//printf("eenet%d: %s\n", i, interface->ifn_name);

		// get the type
		len = sizeof(type);
		if(sceEENetGetIfinfo(interface->ifn_name, sceEENET_IFINFO_IFTYPE, &type, &len) != 0)
			continue;
		//printf("eenet%d type: %d\n", i, type);

		// check for ethernet
		if(type != sceEENET_IFTYPE_ETHER)
			continue;
		//printf("eenet%d: ethernet\n", i);

		// get the address
		len = sizeof(linkAddress);
		if(sceEENetGetIfinfo(interface->ifn_name, sceEENET_IFINFO_MACADDR, &linkAddress, &len) != 0)
			continue;
		MAC = linkAddress.ether_addr_octet;
		//printf("eenet%d: MAC: %02X-%02X-%02X-%02X-%02X-%02X\n", i, MAC[0], MAC[1], MAC[2], MAC[3], MAC[4], MAC[5]);

		break;
	}

	// free the interfaces
	gsifree(interfaces);

	return MAC;
}

#elif defined(SN_SYSTEMS)

static const char * GetMAC(void)
{
	static char MAC[6];
	int len = sizeof(MAC);
	int rcode;

	// get the MAC
	rcode = sndev_get_status(0, SN_DEV_STAT_MAC, MAC, &len);
	if((rcode != 0) || (len != 6))
		return NULL;

	return MAC;
}

#endif

static const char * GOAGetUniqueID_Internal(void)
{
	static char keyval[17];
	const char * MAC;

	// check if we already have the Unique ID
	if(keyval[0])
		return keyval;

	// get the MAC
	MAC = GetMAC();
	if(!MAC)
	{
		// error getting the MAC
		static char errorMAC[6] = { 1, 2, 3, 4, 5, 6 };
		MAC = errorMAC;
	}

	// format it
	sprintf(keyval, "%02X%02X%02X%02X%02X%02X0000",
		MAC[0] & 0xFF,
		MAC[1] & 0xFF,
		MAC[2] & 0xFF,
		MAC[3] & 0xFF,
		MAC[4] & 0xFF,
		MAC[5] & 0xFF);

	return keyval;
}

#endif // UNIQUEID
#endif // _PS2

#ifdef _MACOS
const char * GOAGetUniqueID_Internal(void)
{
	static char keyval[17];
	keyval[0] = '\0';
	return keyval;
}
#endif // _MACOS

#if (defined(_WIN32) || defined(UNDER_UNIX)) && !defined(UNDER_CE)

#define RANa 16807         /* multiplier */
#define LONGRAND_MAX 2147483647L   /* 2**31 - 1 */

static long randomnum = 1;

static long nextlongrand(long seed)
{
	unsigned

	long lo, hi;
	lo = RANa *(long)(seed & 0xFFFF);
	hi = RANa *(long)((unsigned long)seed >> 16);
	lo += (hi & 0x7FFF) << 16;

	if (lo > LONGRAND_MAX)
	{
		lo &= LONGRAND_MAX;
		++lo;
	}
	lo += hi >> 15;

	if (lo > LONGRAND_MAX)
	{
		lo &= LONGRAND_MAX;
		++lo;
	}

	return(long)lo;
}

static long longrand(void)/* return next random long */
{
	randomnum = nextlongrand(randomnum);
	return randomnum;
}

static void Util_RandSeed(unsigned long seed)/* to seed it */
{
	randomnum = seed ? (seed & LONGRAND_MAX) : 1;
	/* nonzero seed */
}

static int Util_RandInt(int low, int high)
{
	int range = high-low;
	int num = longrand() % range;

	return(num + low);
}

static void GenerateID(char *keyval)
{
	int i;
	const char crypttab[63] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
#ifdef _WIN32
	LARGE_INTEGER l1;
	UINT seed;
	if (QueryPerformanceCounter(&l1))
		seed = (l1.LowPart ^ l1.HighPart);
	else
		seed = 0;
	Util_RandSeed(seed ^ GetTickCount() ^ time(NULL) ^ clock());
#else
	Util_RandSeed(time(NULL) ^ clock());
#endif
	for (i = 0; i < 19; i++)
		if (i == 4 || i == 9 || i == 14)
			keyval[i] = '-';
	else
		keyval[i] = crypttab[Util_RandInt(0, 62)];
	keyval[19] = 0;
}

#ifndef PATH_MAX
#define PATH_MAX MAX_PATH
#endif

#ifdef _WIN32
#define REG_KEY	  "Software\\GameSpy\\GameSpy 3D\\Registration"
#endif

const char * GOAGetUniqueID_Internal(void)
{
	static char keyval[PATH_MAX] = "";
	unsigned int ret;

	int docreate;

#ifdef _WIN32
	HKEY thekey;
	DWORD thetype = REG_SZ;
	DWORD len = MAX_PATH;
	DWORD disp;

	if (RegOpenKeyExA(HKEY_CURRENT_USER, REG_KEY, 0, KEY_ALL_ACCESS, &thekey) != ERROR_SUCCESS)
		docreate = 1;
	else
		docreate = 0;
	ret = RegQueryValueExA(thekey, (LPCSTR)"Crypt", 0, &thetype, (LPBYTE)keyval, &len);
#else
	FILE *f;
	f = fopen("id.bin","r");
	if (!f)
		ret = 0;
	else
	{
		ret = fread(keyval,1,19,f);
		keyval[ret] = 0;
		fclose(f);
	}
#endif

	if (ret != 0 || strlen(keyval) != 19)//need to generate a new key
	{
		GenerateID(keyval);
#ifdef _WIN32
		if (docreate)
		{
			ret = RegCreateKeyExA(HKEY_CURRENT_USER, REG_KEY, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &thekey, &disp);
		}
		RegSetValueExA(thekey, (LPCSTR)"Crypt", 0, REG_SZ, (const LPBYTE)keyval, strlen(keyval)+1);
#else
		f = fopen("id.bin","w");
		if (f)
		{
			fwrite(keyval,1,19,f);
			fclose(f);
		} else
			keyval[0] = 0; //don't generate one each time!!
#endif
	}

#ifdef _WIN32
	RegCloseKey(thekey);
#endif

	// Strip out the -'s.
	/////////////////////
	memmove(keyval + 4, keyval + 5, 4);
	memmove(keyval + 8, keyval + 10, 4);
	memmove(keyval + 12, keyval + 15, 4);
	keyval[16] = '\0';
	
	return keyval;
}

#endif

#if !defined(_PS2) || defined(UNIQUEID)
GetUniqueIDFunction GOAGetUniqueID = GOAGetUniqueID_Internal;
#endif
