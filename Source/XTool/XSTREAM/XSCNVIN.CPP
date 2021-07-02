#include <cstdio>
#include <cstring>
#include <string>
#include "tweaks.h"
#include "xutl.h"
#include "xstream.h"

XStream& XStream::operator<= (float var)
{
    snprintf(convBuf, XS_CONV_BUFFER_LEN, "%.*g", XS_DEFDIGITS, var);
    terminate_float_with_dot(convBuf, XS_CONV_BUFFER_LEN);
    write(convBuf, strlen(convBuf));
    return *this;
}

XStream& XStream::operator<= (double var)
{
    snprintf(convBuf, XS_CONV_BUFFER_LEN, "%.*g", XS_DEFDIGITS, var);
    terminate_float_with_dot(convBuf, XS_CONV_BUFFER_LEN);
    write(convBuf, strlen(convBuf));
    return *this;
}

XStream& XStream::operator<= (long double var)
{
    snprintf(convBuf, XS_CONV_BUFFER_LEN, "%.*Lg", XS_DEFDIGITS, var);
    terminate_float_with_dot(convBuf, XS_CONV_BUFFER_LEN);
    write(convBuf, strlen(convBuf));
    return *this;
}
