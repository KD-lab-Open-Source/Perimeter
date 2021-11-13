#include <cstdlib>
#include <cstring>
#include "xutl.h"
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
	var = (int32_t)strtol(p,&p,0);
	offset += p - (buf + offset);
	return *this;
}

XBuffer& XBuffer::operator>= (uint32_t& var)
{
	char* p = buf + offset;
	var = (uint32_t)strtoul(p,&p,0);
	offset += p - (buf + offset);
	return *this;
}

XBuffer& XBuffer::operator>= (int64_t& var)
{
	char* p = buf + offset;
	var = strtoll(p,&p,0);
	offset += p - (buf + offset);
	return *this;
}

XBuffer& XBuffer::operator>= (uint64_t& var)
{
	char* p = buf + offset;
	var = strtoull(p,&p,0);
	offset += p - (buf + offset);
	return *this;
}

XBuffer& XBuffer::operator>= (double& var)
{
	char* p = buf + offset;
	var = string_to_double(p,&p);
	offset += p - (buf + offset);
	return *this;
}

XBuffer& XBuffer::operator>= (float& var)
{
	char* p = buf + offset;
	var = string_to_float(p,&p);
	offset += p - (buf + offset);
	return *this;
}


