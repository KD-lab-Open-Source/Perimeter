#include <cstring>
#include <cstdio>
#include <string>
#include "tweaks.h"
#include "xutl.h"
#include "xbuffer.h"

XBuffer& XBuffer::operator<= (float var)
{
    snprintf(convBuf, XB_CONV_BUFFER_LEN, "%.*g", digits, var);
    terminate_with_dot(convBuf, XB_CONV_BUFFER_LEN);
	write(convBuf, strlen(convBuf), 0);
	return *this;
}

XBuffer& XBuffer::operator<= (double var)
{
    snprintf(convBuf, XB_CONV_BUFFER_LEN, "%.*g", digits, var);
    terminate_with_dot(convBuf, XB_CONV_BUFFER_LEN);
	write(convBuf, strlen(convBuf), 0);
	return *this;
}

XBuffer& XBuffer::operator<= (long double var)
{
    snprintf(convBuf, XB_CONV_BUFFER_LEN, "%.*Lg", digits, var);
    terminate_with_dot(convBuf, XB_CONV_BUFFER_LEN);
	write(convBuf, strlen(convBuf), 0);
	return *this;
}
