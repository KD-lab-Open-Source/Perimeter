#include "xstream.h"
#include <cstdio>
#include <cstring>

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
