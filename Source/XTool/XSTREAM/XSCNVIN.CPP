#include "xstream.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

XStream& XStream::operator<= (char var)
{
    sprintf(convBuf, "%c", var);
    write(convBuf, strlen(convBuf));
    return *this;
}

XStream& XStream::operator<= (unsigned char var)
{
    sprintf(convBuf, "%uc", var);
    write(convBuf, strlen(convBuf));
	return *this;
}

XStream& XStream::operator<= (short var)
{
    sprintf(convBuf, "%hd", var);
    write(convBuf, strlen(convBuf));
	return *this;
}

XStream& XStream::operator<= (unsigned short var)
{
    sprintf(convBuf, "%hu", var);
    write(convBuf, strlen(convBuf));
	return *this;
}

XStream& XStream::operator<= (int var)
{
    sprintf(convBuf, "%d", var);
    write(convBuf, strlen(convBuf));
	return *this;
}

XStream& XStream::operator<= (unsigned var)
{
    sprintf(convBuf, "%u", var);
    write(convBuf, strlen(convBuf));
	return *this;
}

XStream& XStream::operator<= (long var)
{
    sprintf(convBuf, "%ld", var);
    write(convBuf, strlen(convBuf));
	return *this;
}

XStream& XStream::operator<= (unsigned long var)
{
    sprintf(convBuf, "%lu", var);
    write(convBuf, strlen(convBuf));
	return *this;
}

XStream& XStream::operator<= (float var)
{
    sprintf(convBuf, "%*g", XS_DEFDIGITS, var);
    write(convBuf, strlen(convBuf));
	return *this;
}

XStream& XStream::operator<= (double var)
{
    sprintf(convBuf, "%*g", XS_DEFDIGITS, var);
    write(convBuf, strlen(convBuf));
	return *this;
}

XStream& XStream::operator<= (long double var)
{
    sprintf(convBuf, "%*Lg", XS_DEFDIGITS, var);
    write(convBuf, strlen(convBuf));
	return *this;
}
