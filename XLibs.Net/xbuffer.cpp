#include <cstring>
#include <memory>
#include <malloc.h>
#include <cctype>
#include <cstdlib>
#include "xutil.h"

char _ConvertBuffer[_CONV_BUFFER_LEN + 1];

static char* outboundMSG = "Out of XBuffer";

void XBuffer::alloc(unsigned int sz)
{
    buf = (char*)malloc(size = sz);
    offset = 0L;
    radix = XB_DEFRADIX;
    digits = XB_DEFDIGITS;
    MakeFree = 1;
    *buf = 0;
}

XBuffer::XBuffer(unsigned int sz, int automatic_realloc_) {
    automatic_realloc = automatic_realloc_;
    alloc(sz);
}

XBuffer::XBuffer(void* p,int sz)
{
    buf = (char*)malloc(0);
    offset = 0L;
    radix = XB_DEFRADIX;
    digits = XB_DEFDIGITS;
    MakeFree = 0;
    size = sz;
}

void XBuffer::free(void)
{
    if(MakeFree && buf){
        ::free(buf);
        buf = NULL;
    }
}

void XBuffer::fill(char fc)
{ memset(buf,fc,size); }

void XBuffer::set(int off,int mode)
{
    switch(mode){
        case XB_BEG:
            offset = off;
            break;
        case XB_CUR:
            offset+=off;
            break;
        case XB_END:
            offset = size - off;
            break;
    }
}

unsigned int XBuffer::read(void* s, unsigned int len)
{
    memcpy(s, buf + offset, len);
    offset += len;
    return len;
}

void XBuffer::handleOutOfSize()
{
    if(automatic_realloc)
        buf = (char*)realloc(buf, size *= 2);
    else {
        xassert(0 && "Out of XBuffer");
        ErrH.Abort("Out of XBuffer");
    }
}

unsigned int XBuffer::write(const void* s, unsigned int len, int bin_flag)
{
    while(offset + len >= size)
        handleOutOfSize();

    memcpy(buf + offset, s, len);
    offset += len;

    if(!bin_flag)
        buf[offset] = '\0';

    return len;
}

int XBuffer::search(char* what, int mode, int cs)
{
    int i,j;
    unsigned int wlen = strlen(what) - 1;
    if(mode == XB_GLOBAL) { i = 0; mode = XB_FORWARD; } else i = offset;
    if(mode == XB_FORWARD){
        if(cs == XB_CASEON){
            while(buf[i]){
                if(buf[i] == *what){
                    j = wlen;
                    while(buf[i + j] == what[j] && j) j--;
                    if(j <= 0) { offset = i; return 1; };
                }
                i++;
            }
        }
        else {
            while(buf[i]){
                if(toupper(buf[i]) == toupper(*what)){
                    j = wlen;
                    while(toupper(buf[i + j]) == toupper(what[j]) && j) j--;
                    if(j <= 0) { offset = i; return 1; };
                }
                i++;
            }
        }
    }
    else {
        i-=wlen + 1;
        if(cs == XB_CASEON){
            while(i >= 0){
                if(buf[i] == *what){
                    j = wlen;
                    while(buf[i + j] == what[j] && j) j--;
                    if(j <= 0) { offset = i; return 1; };
                }
                i--;
            }
        }
        else {
            while(i >= 0){
                if(toupper(buf[i]) == toupper(*what)){
                    j = wlen;
                    while(toupper(buf[i + j]) == toupper(what[j]) && j) j--;
                    if(j <= 0) { offset = i; return 1; };
                }
                i--;
            }
        }
    }
    return 0;
}

XBuffer& XBuffer::operator<= (char var)
{
	char* s = itoa(var,_ConvertBuffer,radix);
	write(s,strlen(s),0);
	return *this;
}

XBuffer& XBuffer::operator<= (unsigned char var)
{
	char* s = itoa(var,_ConvertBuffer,radix);
	write(s,strlen(s),0);
	return *this;
}

XBuffer& XBuffer::operator<= (short var)
{
	char* s = itoa(var,_ConvertBuffer,radix);
	write(s,strlen(s),0);
	return *this;
}

XBuffer& XBuffer::operator<= (unsigned short var)
{
	char* s = ltoa(var,_ConvertBuffer,radix);
	write(s,strlen(s),0);
	return *this;
}

XBuffer& XBuffer::operator<= (int var)
{
	char* s = itoa(var,_ConvertBuffer,radix);
	write(s,strlen(s),0);
	return *this;
}

XBuffer& XBuffer::operator<= (unsigned var)
{
    char* s = ltoa(var,_ConvertBuffer,radix);
	write(s,strlen(s),0);
	return *this;
}

XBuffer& XBuffer::operator<= (long var)
{
	char* s = ltoa(var,_ConvertBuffer,radix);
	write(s,strlen(s),0);
	return *this;
}

XBuffer& XBuffer::operator<= (unsigned long var)
{
    char* s = ltoa(var,_ConvertBuffer,radix);
	write(s,strlen(s),0);
	return *this;
}

XBuffer& XBuffer::operator<= (float var)
{
	char* s = gcvt(var,digits,_ConvertBuffer);
	write(s,strlen(s),0);
	return *this;
}

XBuffer& XBuffer::operator<= (double var)
{
	char* s = gcvt(var,digits,_ConvertBuffer);
	write(s,strlen(s),0);
	return *this;
}

XBuffer& XBuffer::operator<= (long double var)
{
	char* s = gcvt(var,digits,_ConvertBuffer);
	write(s,strlen(s),0);
	return *this;
}

XBuffer& XBuffer::operator>= (char& var)
{
	char* p = buf + offset;
	var = (char)strtol(p,&p,0);
	offset += p - (buf + offset);
	return *this;
}

XBuffer& XBuffer::operator>= (unsigned char& var)
{
	char* p = buf + offset;
	var = (unsigned char)strtoul(p,&p,0);
	offset += p - (buf + offset);
	return *this;
}

XBuffer& XBuffer::operator>= (short& var)
{
	char* p = buf + offset;
	var = (short)strtol(p,&p,0);
	offset += p - (buf + offset);
	return *this;
}

XBuffer& XBuffer::operator>= (unsigned short& var)
{
	char* p = buf + offset;
	var = (unsigned short)strtoul(p,&p,0);
	offset += p - (buf + offset);
	return *this;
}

XBuffer& XBuffer::operator>= (int& var)
{
	char* p = buf + offset;
	var = strtol(p,&p,0);
	offset += p - (buf + offset);
	return *this;
}

XBuffer& XBuffer::operator>= (unsigned& var)
{
	char* p = buf + offset;
	var = strtoul(p,&p,0);
	offset += p - (buf + offset);
	return *this;
}

XBuffer& XBuffer::operator>= (long& var)
{
	char* p = buf + offset;
	var = strtol(p,&p,0);
	offset += p - (buf + offset);
	return *this;
}

XBuffer& XBuffer::operator>= (unsigned long& var)
{
	char* p = buf + offset;
	var = strtoul(p,&p,0);
	offset += p - (buf + offset);
	return *this;
}

XBuffer& XBuffer::operator>= (double& var)
{
	char* p = buf + offset;
	var = strtod(p,&p);
	offset += p - (buf + offset);
	return *this;
}

XBuffer& XBuffer::operator>= (long double& var)
{
	char* p = buf + offset;
	var = strtod(p,&p);
	offset += p - (buf + offset);
	return *this;
}

XBuffer& XBuffer::operator>= (float& var)
{
	char* p = buf + offset;
	var = (float)strtod(p,&p);
	offset += p - (buf + offset);
	return *this;
}

/*
XBuffer& XBuffer::operator< (const char* v) 
{ 
	if(v) 
		write(v, strlen(v), 0); 
	return *this; 
}

XBuffer& XBuffer::operator> (char* v) 
{ 
	if(v) 
		read(v, strlen(buf + offset) + 1); 
	return *this; 
}

XBuffer& XBuffer::operator> (char* v)
{
	if(v) read(v,(unsigned int)strlen(buf + offset) + 1);
	return *this;
}

XBuffer& XBuffer::operator> (char& v)
{
	read(&v,(unsigned int)sizeof(char));
	return *this;
}

XBuffer& XBuffer::operator> (unsigned char& v)
{
	read(&v,(unsigned int)sizeof(unsigned char));
	return *this;
}

XBuffer& XBuffer::operator> (short& v)
{
	read(&v,(unsigned int)sizeof(short));
	return *this;
}

XBuffer& XBuffer::operator> (unsigned short& v)
{
	read(&v,(unsigned int)sizeof(unsigned short));
	return *this;
}

XBuffer& XBuffer::operator> (int& v)
{
	read(&v,(unsigned int)sizeof(int));
	return *this;
}

XBuffer& XBuffer::operator> (unsigned int& v)
{
	read(&v,(unsigned int)sizeof(unsigned int));
	return *this;
}

XBuffer& XBuffer::operator> (long& v)
{
	read(&v,(unsigned int)sizeof(long));
	return *this;
}

XBuffer& XBuffer::operator> (unsigned long& v)
{
	read(&v,(unsigned int)sizeof(unsigned long));
	return *this;
}

XBuffer& XBuffer::operator> (float& v)
{
	read(&v,(unsigned int)sizeof(float));
	return *this;
}

XBuffer& XBuffer::operator> (double& v)
{
	read(&v,(unsigned int)sizeof(double));
	return *this;
}

XBuffer& XBuffer::operator> (long double& v)
{
	read(&v,(unsigned int)sizeof(long double));
	return *this;
}

XBuffer& XBuffer::operator< (const char* v)
{
	if(v) write(v,(unsigned int)strlen(v),0);
	return *this;
}

XBuffer& XBuffer::operator< (char v)
{
	write(&v,(unsigned int)sizeof(char));
	return *this;
}

XBuffer& XBuffer::operator< (unsigned char v)
{
	write(&v,(unsigned int)sizeof(unsigned char));
	return *this;
}

XBuffer& XBuffer::operator< (short v)
{
	write(&v,(unsigned int)sizeof(short));
	return *this;
}

XBuffer& XBuffer::operator< (unsigned short v)
{
	write(&v,(unsigned int)sizeof(unsigned short));
	return *this;
}

XBuffer& XBuffer::operator< (int v)
{
	write(&v,(unsigned int)sizeof(int));
	return *this;
}

XBuffer& XBuffer::operator< (unsigned int v)
{
	write(&v,(unsigned int)sizeof(unsigned int));
	return *this;
}

XBuffer& XBuffer::operator< (long v)
{
	write(&v,(unsigned int)sizeof(long));
	return *this;
}

XBuffer& XBuffer::operator< (unsigned long v)
{
	write(&v,(unsigned int)sizeof(unsigned long));
	return *this;
}

XBuffer& XBuffer::operator< (float v)
{
	write(&v,(unsigned int)sizeof(float));
	return *this;
}

XBuffer& XBuffer::operator< (double v)
{
	write(&v,(unsigned int)sizeof(double));
	return *this;
}

XBuffer& XBuffer::operator< (long double v)
{
	write(&v,(unsigned int)sizeof(long double));
	return *this;
}
*/
 