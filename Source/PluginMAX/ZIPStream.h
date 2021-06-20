#ifndef _ZIPSTREAM_H_
#define _ZIPSTREAM_H_

#include <xutil.h>

class ZIPStream : public XStream
{
public:
	ZIPStream();
    int	open(const char* name, unsigned f = XS_IN);
};

//Если не вызывать ZIPOpen, то файл будет открываться просто файл
void ZIPOpen(const char* zip_filename);
void ZIPClose();
bool ZIPIsOpen();

#endif //_ZIPSTREAM_H_