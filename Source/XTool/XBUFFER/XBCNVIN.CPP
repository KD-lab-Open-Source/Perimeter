#include <cstring>
#include <cstdlib>
#include "xbuffer.h"

XBuffer& XBuffer::operator<= (char var)
{
	char* s = itoa(var,_ConvertBuffer,radix);
	write(s,strlen(s),0);
	return *this;
}

XBuffer& XBuffer::operator<= (unsigned char var)
{
	char* s = itoa(var,_ConvertBuffer,radix);
	write(s,strlen(s),0);
	return *this;
}

XBuffer& XBuffer::operator<= (short var)
{
	char* s = itoa(var,_ConvertBuffer,radix);
	write(s,strlen(s),0);
	return *this;
}

XBuffer& XBuffer::operator<= (unsigned short var)
{
	char* s = ltoa(var,_ConvertBuffer,radix);
	write(s,strlen(s),0);
	return *this;
}

XBuffer& XBuffer::operator<= (int var)
{
	char* s = itoa(var,_ConvertBuffer,radix);
	write(s,strlen(s),0);
	return *this;
}

XBuffer& XBuffer::operator<= (unsigned var)
{
	char* s = _ultoa(var,_ConvertBuffer,radix);
	write(s,strlen(s),0);
	return *this;
}

XBuffer& XBuffer::operator<= (long var)
{
	char* s = ltoa(var,_ConvertBuffer,radix);
	write(s,strlen(s),0);
	return *this;
}

XBuffer& XBuffer::operator<= (unsigned long var)
{
	char* s = _ultoa(var,_ConvertBuffer,radix);
	write(s,strlen(s),0);
	return *this;
}

XBuffer& XBuffer::operator<= (unsigned long long var)
{
    char* s = _ultoa(var,_ConvertBuffer,radix);
    write(s,strlen(s),0);
    return *this;
}

XBuffer& XBuffer::operator<= (float var)
{
	char* s = gcvt(var,digits,_ConvertBuffer);
	write(s,strlen(s),0);
	return *this;
}

XBuffer& XBuffer::operator<= (double var)
{
	char* s = gcvt(var,digits,_ConvertBuffer);
	write(s,strlen(s),0);
	return *this;
}

XBuffer& XBuffer::operator<= (long double var)
{
	char* s = gcvt(var,digits,_ConvertBuffer);
	write(s,strlen(s),0);
	return *this;
}
