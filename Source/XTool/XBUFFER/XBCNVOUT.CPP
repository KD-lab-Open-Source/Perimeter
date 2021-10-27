#include <cstdlib>
#include <cstring>
#include "xbuffer.h"

XBuffer& XBuffer::operator>= (char& var)
{
    char* p = buf + offset;
    var = (char)strtol(p,&p,0);
    offset += p - (buf + offset);
    return *this;
}

XBuffer& XBuffer::operator>= (int8_t& var)
{
    char* p = buf + offset;
    var = (int8_t)strtol(p,&p,0);
    offset += p - (buf + offset);
    return *this;
}

XBuffer& XBuffer::operator>= (uint8_t& var)
{
	char* p = buf + offset;
	var = (uint8_t)strtoul(p,&p,0);
	offset += p - (buf + offset);
	return *this;
}

XBuffer& XBuffer::operator>= (int16_t& var)
{
	char* p = buf + offset;
	var = (int16_t)strtol(p,&p,0);
	offset += p - (buf + offset);
	return *this;
}

XBuffer& XBuffer::operator>= (uint16_t& var)
{
	char* p = buf + offset;
	var = (uint16_t)strtoul(p,&p,0);
	offset += p - (buf + offset);
	return *this;
}

XBuffer& XBuffer::operator>= (int32_t& var)
{
	char* p = buf + offset;
	var = strtol(p,&p,0);
	offset += p - (buf + offset);
	return *this;
}

XBuffer& XBuffer::operator>= (uint32_t& var)
{
	char* p = buf + offset;
	var = strtoul(p,&p,0);
	offset += p - (buf + offset);
	return *this;
}

XBuffer& XBuffer::operator>= (int64_t& var)
{
	char* p = buf + offset;
	var = strtol(p,&p,0);
	offset += p - (buf + offset);
	return *this;
}

XBuffer& XBuffer::operator>= (uint64_t& var)
{
	char* p = buf + offset;
	var = strtoul(p,&p,0);
	offset += p - (buf + offset);
	return *this;
}

XBuffer& XBuffer::operator>= (double& var)
{
	char* p = buf + offset;
	var = strtod(p,&p);
	offset += p - (buf + offset);
	return *this;
}

XBuffer& XBuffer::operator>= (long double& var)
{
	char* p = buf + offset;
	var = strtod(p,&p);
	offset += p - (buf + offset);
	return *this;
}

XBuffer& XBuffer::operator>= (float& var)
{
	char* p = buf + offset;
	var = (float)strtod(p,&p);
	offset += p - (buf + offset);
	return *this;
}


