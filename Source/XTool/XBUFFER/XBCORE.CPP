#include <cstring>
#include <string>
#include <fstream>
#include <memory>
#include <cstdlib>
#include "xerrhand.h"
#include "xbuffer.h"

#if (!defined(_FINAL_VERSION_) || defined(_DEBUG)) && !defined(NASSERT)
#include <iostream>
#endif

void XBuffer::alloc(size_t sz)
{
    free();
	buf = (char*)malloc(size = sz);
	offset = 0L;
	digits = XB_DEFDIGITS;
	automatic_free = true;
	if(size) *buf = 0;
}

XBuffer::XBuffer(size_t sz, bool automatic_realloc_) {
    buf = nullptr;
    automatic_free = true;
    alloc(sz);
    digits = XB_DEFDIGITS;
    automatic_realloc = automatic_realloc_;
}

XBuffer::XBuffer(void* p, size_t sz)
{
	buf = (char*)p;
	size = sz;
	offset = 0;
	digits = XB_DEFDIGITS;
	automatic_free = false;
	automatic_realloc = false;
}

void XBuffer::free()
{
	if (automatic_free && buf) {
		::free(buf);
		buf = nullptr;
    }
}

void XBuffer::fill(char fc)
{ memset(buf,fc,size); }

void XBuffer::set(size_t off, int mode)
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

size_t XBuffer::read(void* s, size_t len) 
{	
	memcpy(s, buf + offset, len); 
	offset += len; 
	return len; 
}

void XBuffer::handleOutOfSize()
{
	if(automatic_realloc) {
        buf = (char*) realloc(buf, size *= 2);
    } else {
		xassert(0 && "Out of XBuffer");
		ErrH.Abort("Out of XBuffer");
	}
}

size_t XBuffer::write(const void* s, size_t len, bool bin_flag) 
{	
	while(offset + len > size) {
        handleOutOfSize();
    }

	memcpy(buf + offset, s, len);
	offset += len;

	if(!bin_flag)
		buf[offset] = '\0';

	return len;
}

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

