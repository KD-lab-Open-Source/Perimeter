#include <cstdio>
#include <cstring>
#include <string>
#include "tweaks.h"
#include "xutl.h"
#include "xstream.h"

XStream& XStream::operator<= (float var)
{
    snprintf(convBuf, XS_CONV_BUFFER_LEN, PRINTF_FLOATING_FORMAT, XS_DEFDIGITS, var);
    terminate_float_text(convBuf, XS_CONV_BUFFER_LEN, XS_DEFDIGITS);
    write(convBuf, strlen(convBuf));
    return *this;
}

XStream& XStream::operator<= (double var)
{
    snprintf(convBuf, XS_CONV_BUFFER_LEN, PRINTF_FLOATING_FORMAT, XS_DEFDIGITS, var);
    terminate_float_text(convBuf, XS_CONV_BUFFER_LEN, XS_DEFDIGITS);
    write(convBuf, strlen(convBuf));
    return *this;
}

XStream& XStream::operator<= (long double var)
{
    snprintf(convBuf, XS_CONV_BUFFER_LEN, PRINTF_LONG_FLOATING_FORMAT, XS_DEFDIGITS, var);
    terminate_float_text(convBuf, XS_CONV_BUFFER_LEN, XS_DEFDIGITS);
    write(convBuf, strlen(convBuf));
    return *this;
}
