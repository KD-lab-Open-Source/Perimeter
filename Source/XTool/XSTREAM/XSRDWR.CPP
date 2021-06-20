#include <fstream>
#include "xstream.h"

unsigned long XStream::read(void* buf, unsigned long len)
{
	unsigned long ret;
	/*
	if(!ReadFile(handler,buf,len,&ret,0)) {
        if (ErrHUsed) ErrH.Abort(readMSG, XERR_USER, GetLastError(), fname);
        else return 0U;
    }
	if(ret < len) eofFlag = 1;
	pos += ret;
	if(extSize != -1 && pos >= extSize) eofFlag = 1;
     */

    handler->read((char*) buf, len);
    ret = handler->gcount();
    //ret = len;
    if(ret < len) eofFlag = 1;
    pos += ret;
    if(extSize != -1 && pos >= extSize) eofFlag = 1;

	return ret;
}

unsigned long XStream::write(const void* buf, unsigned long len)
{
	unsigned long ret;
	/*
	if(!WriteFile(handler,buf,len,&ret,0)) {
        if (ErrHUsed) ErrH.Abort(writeMSG, XERR_USER, GetLastError(), fname);
        else return 0U;
    }
	*/

    handler->write((char*) buf, len);
    ret = len;
	pos += ret;

	return ret;
}

