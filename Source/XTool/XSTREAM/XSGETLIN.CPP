#include "xstream.h"

char* XStream::getline(char* buf, int64_t len)
{
    int64_t i = -1;
	do {
		i++;
		read(&buf[i],1);
	   } while(buf[i] != '\n' && i < len && !eof());
	if(eof())
		buf[i] = '\0';
	else
		buf[i - 1] = '\0';
	return buf;
}

