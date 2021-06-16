#include "tweaks.h"
#ifdef _WINDOWS_
#include <windows.h>
#endif
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

ZIPStream::ZIPStream(const char* fname)
:XStream(0)
{
	if(!fname)return;
	bool b_open=false;
#ifndef _NOZIP
	if(pzip)	
	{
		b_open=open(fname)!=0;
		if(!b_open)
			b_open=pzip->open((char*)fname,*this)!=0;
		if(!b_open)
			b_open=open(fname)!=0;
	}else
#endif //_NOZIP
	{
		b_open=open(fname)!=0;
	}
}
