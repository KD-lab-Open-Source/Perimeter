#include <cstring>
#include <cstdio>
#include <string>
#include "tweaks.h"
#include "xutl.h"
#include "xbuffer.h"

XBuffer& XBuffer::operator<= (float var)
{
    snprintf(convBuf, XB_CONV_BUFFER_LEN, PRINTF_FLOATING_FORMAT, std::abs(digits), var);
    terminate_float_text(convBuf, XB_CONV_BUFFER_LEN, digits);
	write(convBuf, strlen(convBuf), 0);
	return *this;
}

XBuffer& XBuffer::operator<= (double var)
{
    snprintf(convBuf, XB_CONV_BUFFER_LEN, PRINTF_FLOATING_FORMAT, std::abs(digits), var);
    terminate_float_text(convBuf, XB_CONV_BUFFER_LEN, digits);
	write(convBuf, strlen(convBuf), 0);
	return *this;
}

XBuffer& XBuffer::operator<= (long double var)
{
    snprintf(convBuf, XB_CONV_BUFFER_LEN, PRINTF_LONG_FLOATING_FORMAT, std::abs(digits), var);
    terminate_float_text(convBuf, XB_CONV_BUFFER_LEN, digits);
	write(convBuf, strlen(convBuf), 0);
	return *this;
}
