#include <cstring>
#include <memory>
#include <malloc.h>
#include <cctype>
#include <cstdlib>
#include <windows.h>
#include "xutil.h"

static char* openMSG	 = "CREATE/OPEN FAILURE";
static char* closeMSG	 = "CLOSE FAILURE";
static char* appendMSG	 = "APPENDING FAILURE";
static char* readMSG	= "WRONG READING";
static char* writeMSG	= "WRONG WRITING";
static char *seekMSG	= "BAD SEEK";
static char *flushMSG	= "FLUSH FILE BUFFERS ERROR";
static char *sizeMSG	= "FILE SIZE CALCULATION ERROR";

XStream::XStream(int err)
{
	ErrHUsed = err;
	handler  = INVALID_HANDLE_VALUE;
	eofFlag  = 1;
	radix	 = XS_DEFRADIX;
	digits	 = XS_DEFDIGITS;
	extSize  = -1;
	extPos	 = 0;
}

XStream::~XStream()
{
	close();
}

XStream::XStream(const char* name, unsigned flags,int err)
{
	ErrHUsed = err;
	handler  = INVALID_HANDLE_VALUE;
	eofFlag  = 1;
	radix	 = XS_DEFRADIX;
	digits	 = XS_DEFDIGITS;
	extSize  = -1;
	extPos	 = 0;
	open(name,flags);
}

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


char* XStream::getline(char* buf, unsigned len)
{
	int i = -1;
	do {
		i++;
		read(&buf[i],1);
	   } while(buf[i] != '\n' && i < (int)len && !eof());
	if(eof())
		buf[i] = '\0';
	else
		buf[i - 1] = '\0';
	return buf;
}

unsigned xsReadBytes = 0;
unsigned xsReadBytesDelta = 0;
unsigned xsWriteBytes = 0;
unsigned xsWriteBytesDelta = 0;

unsigned xsRdWrResolution = 500000U;

void (*xsReadHandler)(unsigned) = NULL;
void (*xsWriteHandler)(unsigned) = NULL;

void xsSetReadHandler(void (*fp)(unsigned),unsigned res)
{
    xsReadHandler = fp;
    xsRdWrResolution = res;
    xsReadBytes = 0;
}

void xsSetWriteHandler(void (*fp)(unsigned),unsigned res)
{
    xsWriteHandler = fp;
    xsRdWrResolution = res;
    xsWriteBytes = 0;
}

unsigned long XStream::read(void* buf, unsigned long len)
{
    unsigned long ret;
    if(!ReadFile(handler,buf,len,&ret,0)) {
        if (ErrHUsed) ErrH.Abort(readMSG, XERR_USER, GetLastError(), fname);
        else return 0U;
    }
    if(ret < len) eofFlag = 1;
    pos += ret;
    if(extSize != -1 && pos >= extSize) eofFlag = 1;

    if (xsReadHandler) {
        xsReadBytesDelta += ret;
        if(xsReadBytesDelta >= xsRdWrResolution){
            xsReadBytes += xsReadBytesDelta;
            xsReadBytesDelta = 0;
            (*xsReadHandler)(xsReadBytes);
        }
    }
    return ret;
}

unsigned long XStream::write(const void* buf, unsigned long len)
{
    unsigned long ret;
    if(!WriteFile(handler,buf,len,&ret,0)) {
        if (ErrHUsed) ErrH.Abort(writeMSG, XERR_USER, GetLastError(), fname);
        else return 0U;
    }
    pos += ret;

    if(xsWriteHandler){
        xsWriteBytesDelta += ret;
        if(xsWriteBytesDelta >= xsRdWrResolution){
            xsWriteBytes += xsWriteBytesDelta;
            xsWriteBytesDelta = 0;
            (*xsWriteHandler)(xsWriteBytes);
        }
    }
    return ret;
}

long XStream::seek(long offset, int dir)
{
    long ret;

    if(extSize != -1){
        switch(dir){
            case XS_BEG:
                ret = SetFilePointer(handler,extPos + offset,0,dir) - extPos;
                break;
            case XS_END:
                ret = SetFilePointer(handler,extPos + extSize - offset - 1,0,XS_BEG) - extPos;
                break;
            case XS_CUR:
                ret = SetFilePointer(handler,extPos + pos + offset,0,XS_BEG) - extPos;
                break;
        }
    }
    else
        ret = SetFilePointer(handler,offset,0,dir);

    if (ret == -1L) {
        if (ErrHUsed) ErrH.Abort(seekMSG, XERR_USER, GetLastError(), fname);
        else return -1L;
    }
    if (ret >= size() - 1) eofFlag = 1;  else eofFlag = 0;
    return pos = ret;
}

void XStream::flush()
{
    if (!FlushFileBuffers(handler) && ErrHUsed)
        ErrH.Abort(flushMSG,XERR_USER,GetLastError(),fname);
}

long XStream::size() const
{
    long tmp = extSize;

    if(tmp == -1){
        tmp=GetFileSize(handler,0);
        if (tmp == -1L) {
            if (ErrHUsed) ErrH.Abort(sizeMSG, XERR_USER, GetLastError(), fname);
            else return -1;
        }
    }
    return tmp;
}

static char *timeMSG="FILE GET TIME ERROR";

void XStream::gettime(unsigned& fdate,unsigned& ftime)
{
    fdate=0; ftime=0;
    unsigned short dt,tm;


    FILETIME ft;
    if (!GetFileTime(handler,0,0,&ft)) {
        if (ErrHUsed) ErrH.Abort(timeMSG, XERR_USER, GetLastError(), fname);
        else
            return;
    }
    if (!FileTimeToDosDateTime(&ft,&dt,&tm)) {
        if (ErrHUsed) ErrH.Abort(timeMSG, XERR_USER, GetLastError(), fname);
        else
            return;
    }

    fdate = dt;
    ftime = tm;

}

XStream& XStream::operator<= (char var)
{
    char* s = itoa(var,_ConvertBuffer,radix);
    write(s,strlen(s));
    return *this;
}

XStream& XStream::operator<= (unsigned char var)
{
    char* s = itoa(var,_ConvertBuffer,radix);
    write(s,strlen(s));
    return *this;
}

XStream& XStream::operator<= (short var)
{
    char* s = itoa(var,_ConvertBuffer,radix);
    write(s,strlen(s));
    return *this;
}

XStream& XStream::operator<= (unsigned short var)
{
    char* s = ltoa(var,_ConvertBuffer,radix);
    write(s,strlen(s));
    return *this;
}

XStream& XStream::operator<= (int var)
{
    char* s = itoa(var,_ConvertBuffer,radix);
    write(s,strlen(s));
    return *this;
}

XStream& XStream::operator<= (unsigned var)
{
    char* s = ltoa(var,_ConvertBuffer,radix);
    write(s,strlen(s));
    return *this;
}

XStream& XStream::operator<= (long var)
{
    char* s = ltoa(var,_ConvertBuffer,radix);
    write(s,strlen(s));
    return *this;
}

XStream& XStream::operator<= (unsigned long var)
{
    char* s = ltoa(var,_ConvertBuffer,radix);
    write(s,strlen(s));
    return *this;
}

XStream& XStream::operator<= (float var)
{
    char* s = gcvt(var,digits,_ConvertBuffer);
    write(s,strlen(s));
    return *this;
}

XStream& XStream::operator<= (double var)
{
    char* s = gcvt(var,digits,_ConvertBuffer);
    write(s,strlen(s));
    return *this;
}

XStream& XStream::operator<= (long double var)
{
    char* s = gcvt(var,digits,_ConvertBuffer);
    write(s,strlen(s));
    return *this;
}

XStream& XStream::operator>= (char& var)
{
    int ret = read(_ConvertBuffer,4);
    if(!ret) return *this;
    char* p = _ConvertBuffer;
    p[ret] = ' ';
    var = (char)strtol(p,&p,0);
    seek(p - _ConvertBuffer + 1 - ret,XS_CUR);
    return *this;
}

XStream& XStream::operator>= (unsigned char& var)
{
    int ret = read(_ConvertBuffer,4);
    if(!ret) return *this;
    char* p = _ConvertBuffer;
    p[ret] = ' ';
    var = (unsigned char)strtoul(p,&p,0);
    seek(p - _ConvertBuffer + 1 - ret,XS_CUR);
    return *this;
}

XStream& XStream::operator>= (short& var)
{
    int ret = read(_ConvertBuffer,6);
    if(!ret) return *this;
    char* p = _ConvertBuffer;
    p[ret] = ' ';
    var = (short)strtol(p,&p,0);
    seek(p - _ConvertBuffer + 1 - ret,XS_CUR);
    return *this;
}

XStream& XStream::operator>= (unsigned short& var)
{
    int ret = read(_ConvertBuffer,6);
    if(!ret) return *this;
    char* p = _ConvertBuffer;
    p[ret] = ' ';
    var = (unsigned short)strtoul(p,&p,0);
    seek(p - _ConvertBuffer + 1 - ret,XS_CUR);
    return *this;
}

XStream& XStream::operator>= (int& var)
{
    int ret = read(_ConvertBuffer,16);
    if(!ret) return *this;
    char* p = _ConvertBuffer;
    p[ret] = ' ';
    var = strtol(p,&p,0);
    seek(p - _ConvertBuffer + 1 - ret,XS_CUR);
    return *this;
}

XStream& XStream::operator>= (unsigned& var)
{
    int ret = read(_ConvertBuffer,16);
    if(!ret) return *this;
    char* p = _ConvertBuffer;
    p[ret] = ' ';
    var = strtoul(p,&p,0);
    seek(p - _ConvertBuffer + 1 - ret,XS_CUR);
    return *this;
}

XStream& XStream::operator>= (long& var)
{
    int ret = read(_ConvertBuffer,16);
    if(!ret) return *this;
    char* p = _ConvertBuffer;
    p[ret] = ' ';
    var = strtol(p,&p,0);
    seek(p - _ConvertBuffer + 1 - ret,XS_CUR);
    return *this;
}

XStream& XStream::operator>= (unsigned long& var)
{
    int ret = read(_ConvertBuffer,16);
    if(!ret) return *this;
    char* p = _ConvertBuffer;
    p[ret] = ' ';
    var = strtoul(p,&p,0);
    seek(p - _ConvertBuffer + 1 - ret,XS_CUR);
    return *this;
}

XStream& XStream::operator>= (double& var)
{
    int ret = read(_ConvertBuffer,_CONV_BUFFER_LEN);
    if(!ret) return *this;
    char* p = _ConvertBuffer;
    p[ret] = ' ';
    var = strtod(p,&p);
    seek(p - _ConvertBuffer + 1 - ret,XS_CUR);
    return *this;
}

XStream& XStream::operator>= (float& var)
{
    int ret = read(_ConvertBuffer,_CONV_BUFFER_LEN);
    if(!ret) return *this;
    char* p = _ConvertBuffer;
    p[ret] = ' ';
    var = (float)strtod(p,&p);
    seek(p - _ConvertBuffer + 1 - ret,XS_CUR);
    return *this;
}

XStream& XStream::operator>= (long double& var)
{
    int ret = read(_ConvertBuffer,_CONV_BUFFER_LEN);
    if(!ret) return *this;
    char* p = _ConvertBuffer;
    p[ret] = ' ';
    var = strtod(p,&p);
    seek(p - _ConvertBuffer + 1 - ret,XS_CUR);
    return *this;
}

XStream& XStream::operator< (const char* v)
{
	write(v,(unsigned)strlen(v));
	return *this;
}

XStream& XStream::operator< (char v)
{
	write(&v,(unsigned)sizeof(char));
	return *this;
}

XStream& XStream::operator< (unsigned char v)
{
	write(&v,(unsigned)sizeof(char));
	return *this;
}

XStream& XStream::operator< (short v)
{
	write(&v,(unsigned)sizeof(short));
	return *this;
}

XStream& XStream::operator< (unsigned short v)
{
	write(&v,(unsigned)sizeof(unsigned short));
	return *this;
}

XStream& XStream::operator< (int v)
{
	write(&v,(unsigned)sizeof(int));
	return *this;
}

XStream& XStream::operator< (unsigned v)
{
	write(&v,(unsigned)sizeof(unsigned));
	return *this;
}

XStream& XStream::operator< (long v)
{
	write(&v,(unsigned)sizeof(long));
	return *this;
}

XStream& XStream::operator< (unsigned long v)
{
	write(&v,(unsigned)sizeof(unsigned long));
	return *this;
}

XStream& XStream::operator< (double v)
{
	write(&v,(unsigned)sizeof(double));
	return *this;
}

XStream& XStream::operator< (float v)
{
	write(&v,(unsigned)sizeof(float));
	return *this;
}

XStream& XStream::operator< (long double v)
{
	write(&v,(unsigned)sizeof(long double));
	return *this;
}

XStream& XStream::operator> (char* v)
{
	read(v,(unsigned)strlen(v));
	return *this;
}

XStream& XStream::operator> (char& v)
{
	read(&v,(unsigned)sizeof(char));
	return *this;
}

XStream& XStream::operator> (unsigned char& v)
{
	read(&v,(unsigned)sizeof(char));
	return *this;
}

XStream& XStream::operator> (short& v)
{
	read(&v,(unsigned)sizeof(short));
	return *this;
}

XStream& XStream::operator> (unsigned short& v)
{
	read(&v,(unsigned)sizeof(unsigned short));
	return *this;
}

XStream& XStream::operator> (int& v)
{
	read(&v,(unsigned)sizeof(int));
	return *this;
}

XStream& XStream::operator> (unsigned& v)
{
	read(&v,(unsigned)sizeof(unsigned));
	return *this;
}

XStream& XStream::operator> (long& v)
{
	read(&v,(unsigned)sizeof(long));
	return *this;
}

XStream& XStream::operator> (unsigned long& v)
{
	read(&v,(unsigned)sizeof(unsigned long));
	return *this;
}

XStream& XStream::operator> (double& v)
{
	read(&v,(unsigned)sizeof(double));
	return *this;
}

XStream& XStream::operator> (float& v)
{
	read(&v,(unsigned)sizeof(float));
	return *this;
}

XStream& XStream::operator> (long double& v)
{
	read(&v,(unsigned)sizeof(long double));
	return *this;
}
