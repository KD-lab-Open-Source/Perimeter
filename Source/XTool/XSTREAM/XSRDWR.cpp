#include <fstream>
#include "xstream.h"

int64_t XStream::read(void* buf, int64_t len)
{
    int64_t ret;
	/*
	if(!ReadFile(handler,buf,len,&ret,0)) {
        if (ErrHUsed) ErrH.Abort(readMSG, XERR_USER, GetLastError(), fname);
        else return 0U;
    }
	if(ret < len) eofFlag = 1;
	pos += ret;
	if(extSize != -1 && pos >= extSize) eofFlag = 1;
     */
#ifdef XSTREAM_DEBUG
    fprintf(stderr, "DBG: XStream::read %s %lu\n", fname.c_str(), len);
#endif

    handler->read((char*) buf, len);
    ret = handler->gcount();
    //ret = len;
    if(ret < len) eofFlag = 1;
    pos += ret;
    if(extSize != -1 && pos >= extSize) eofFlag = 1;

	return ret;
}

int64_t XStream::write(const void* buf, int64_t len)
{
    int64_t ret;
	/*
	if(!WriteFile(handler,buf,len,&ret,0)) {
        if (ErrHUsed) ErrH.Abort(writeMSG, XERR_USER, GetLastError(), fname);
        else return 0U;
    }
	*/
#ifdef XSTREAM_DEBUG
    fprintf(stderr, "DBG: XStream::write %s %lu\n", fname.c_str(), len);
#endif

    handler->write((char*) buf, len);
    ret = len;
	pos += ret;

	return ret;
}

int64_t XStream::write_str(const std::string& str) {
    return write(str.c_str(), static_cast<int64_t>(str.length()));
}
