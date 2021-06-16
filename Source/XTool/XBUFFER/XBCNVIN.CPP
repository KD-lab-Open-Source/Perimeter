#include <cstring>
#include <cstdio>
#include <cstdlib>
#include "xbuffer.h"

XBuffer& XBuffer::operator<= (char var)
{
    sprintf(convBuf, "%c", var);
	write(convBuf, strlen(convBuf), 0);
	return *this;
}

XBuffer& XBuffer::operator<= (unsigned char var)
{
    sprintf(convBuf, "%uc", var);
	write(convBuf, strlen(convBuf), 0);
	return *this;
}

XBuffer& XBuffer::operator<= (short var)
{
    sprintf(convBuf, "%hd", var);
	write(convBuf, strlen(convBuf), 0);
	return *this;
}

XBuffer& XBuffer::operator<= (unsigned short var)
{
    sprintf(convBuf, "%hu", var);
	write(convBuf, strlen(convBuf), 0);
	return *this;
}

XBuffer& XBuffer::operator<= (int var)
{
    sprintf(convBuf, "%d", var);
	write(convBuf, strlen(convBuf), 0);
	return *this;
}

XBuffer& XBuffer::operator<= (unsigned var)
{
    sprintf(convBuf, "%u", var);
	write(convBuf, strlen(convBuf), 0);
	return *this;
}

XBuffer& XBuffer::operator<= (long var)
{
    sprintf(convBuf, "%ld", var);
	write(convBuf, strlen(convBuf), 0);
	return *this;
}

XBuffer& XBuffer::operator<= (unsigned long var)
{
    sprintf(convBuf, "%lu", var);
	write(convBuf, strlen(convBuf), 0);
	return *this;
}

XBuffer& XBuffer::operator<= (unsigned long long var)
{
    sprintf(convBuf, "%llu", var);
    write(convBuf, strlen(convBuf), 0);
    return *this;
}

XBuffer& XBuffer::operator<= (float var)
{
    sprintf(convBuf, "%*g", digits, var);
	write(convBuf, strlen(convBuf), 0);
	return *this;
}

XBuffer& XBuffer::operator<= (double var)
{
    sprintf(convBuf, "%*g", digits, var);
	write(convBuf, strlen(convBuf), 0);
	return *this;
}

XBuffer& XBuffer::operator<= (long double var)
{
    sprintf(convBuf, "%*Lg", digits, var);
	write(convBuf, strlen(convBuf), 0);
	return *this;
}
