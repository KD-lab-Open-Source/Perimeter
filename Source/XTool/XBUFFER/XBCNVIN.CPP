#include <cstring>
#include <cstdio>
#include "xbuffer.h"

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
