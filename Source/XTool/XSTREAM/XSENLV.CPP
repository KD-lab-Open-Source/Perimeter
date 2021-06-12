#include <windows.h>
#include "xconsole.h"
#include "xstream.h"
#include "xerrhand.h"

static char* openMSG	 = "CREATE/OPEN FAILURE";
static char* closeMSG	 = "CLOSE FAILURE";
static char* appendMSG	 = "APPENDING FAILURE";

int XStream::open(const char* name, unsigned f)
{
	DWORD fa = 0;
	DWORD fs = 0;
	DWORD fc = 0;
	DWORD ff = FILE_ATTRIBUTE_NORMAL | FILE_FLAG_RANDOM_ACCESS;

	if(f & XS_IN) fa |= GENERIC_READ;
	if(f & XS_OUT) fa |= GENERIC_WRITE;

	if(!(f & XS_NOSHARING)) fs |= FILE_SHARE_READ | FILE_SHARE_WRITE;

	if((f & XS_IN) || (f & XS_NOREPLACE)) fc = OPEN_EXISTING;
	else if(f & XS_OUT) fc = CREATE_ALWAYS;

	if(f & XS_NOBUFFERING) ff |= FILE_FLAG_NO_BUFFERING;

	handler = CreateFile(name,fa,fs,0,fc,ff,0);
	if(handler == INVALID_HANDLE_VALUE) {
        if (ErrHUsed) ErrH.Abort(openMSG, XERR_USER, GetLastError(), name);
        else return 0;
    }

	if(f & XS_APPEND)
		if(!SetFilePointer(handler,0,0,FILE_END))
			ErrH.Abort(appendMSG,XERR_USER,GetLastError(),name);

	fname = name;
	pos = SetFilePointer(handler,0L,0,FILE_CURRENT);
	eofFlag = 0;
	return 1;
}

int XStream::open(XStream* owner,long s,long ext_sz)
{
	fname = owner -> fname;
	handler = owner -> handler;
	pos = 0;
	owner -> seek(s,XS_BEG);
	eofFlag = owner -> eof();
	extSize = ext_sz;
	extPos = s;
	return 1;
}

void XStream::close(void)
{
	if(handler == INVALID_HANDLE_VALUE) return;

	if(extSize == -1 && !CloseHandle(handler) && ErrHUsed)
		ErrH.Abort(closeMSG,XERR_USER,GetLastError(),fname);

	handler = INVALID_HANDLE_VALUE;
	fname = NULL;
	pos = 0L;
	eofFlag = 1;
	extSize = -1;
	extPos = 0;
}

