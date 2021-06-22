#include "tweaks.h"
#include <windows.h>
#include <cmath>
#include <cstring>
#include <xutil.h>

#include "ZIPStream.h"

#ifndef _NOZIP
#include "zip_resource.h"
static XZIP_Resource* pzip=NULL;
#endif //_NOZIP

bool ZIPIsOpen()
{
#ifndef _NOZIP
	return pzip!=NULL;
#else
	return false;
#endif //_NOZIP
}

void ZIPOpen(const char* zip_filename)
{
#ifndef _NOZIP
	ZIPClose();
	pzip=new XZIP_Resource((char*)zip_filename,XZIP_ENABLE_ZIP_HEADERS);
#endif //_NOZIP
}

void ZIPClose()
{
#ifndef _NOZIP
	delete pzip;
	pzip=NULL;
#endif //_NOZIP
}

ZIPStream::ZIPStream() : XStream(0) {
}

int	ZIPStream::open(const char* name, unsigned f) {
	if (!name) return 0;
#ifndef _NOZIP
	if(pzip)	
	{
		bool b_open=XStream::open(name, f)!=0;
		if(!b_open)
			b_open=pzip->open((char*)name,*this)!=0;
		if(!b_open)
			b_open=XStream::open(name, f)!=0;
        return b_open ? 1 : 0;
	}else
#endif //_NOZIP
	{
        return XStream::open(name, f);
	}
}
