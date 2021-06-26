#include <cstring>
#include "xstream.h"

XStream::XStream(int err)
{
	ErrHUsed = err;
	handler  = nullptr;
	eofFlag  = 1;
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
	handler  = nullptr;
	eofFlag  = 1;
	extSize  = -1;
	extPos	 = 0;
	open(name,flags);
}


