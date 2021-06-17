#include <fstream>
#include "xstream.h"

static char* readMSG	= "WRONG READING";
static char* writeMSG	= "WRONG WRITING";

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

    ((std::fstream *)handler)->read((char*) buf, len);
    //len = ((std::fstream *)handler)->gcount();
    //ret = len;
    ret = ((std::fstream *)handler)->gcount();
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

    ((std::fstream *)handler)->write((char*) buf, len);
    ret = len;
	pos += ret;

	return ret;
}

