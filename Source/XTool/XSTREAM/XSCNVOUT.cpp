#include <string>
#include "xutl.h"
#include "xstream.h"

XStream& XStream::operator>= (char& var)
{
    int ret = read(convBuf,4);
    if(!ret) return *this;
    char* p = convBuf;
    p[ret] = ' ';
    var = (char)strtol(p,&p,0);
    seek(p - convBuf + 1 - ret,XS_CUR);
    return *this;
}

XStream& XStream::operator>= (int8_t& var)
{
    int ret = read(convBuf,4);
    if(!ret) return *this;
    char* p = convBuf;
    p[ret] = ' ';
    var = (int8_t)strtol(p,&p,0);
    seek(p - convBuf + 1 - ret,XS_CUR);
    return *this;
}

XStream& XStream::operator>= (uint8_t& var)
{
	int ret = read(convBuf,4);
	if(!ret) return *this;
	char* p = convBuf;
	p[ret] = ' ';
	var = (uint8_t)strtoul(p,&p,0);
	seek(p - convBuf + 1 - ret,XS_CUR);
	return *this;
}

XStream& XStream::operator>= (int16_t& var)
{
	int ret = read(convBuf,6);
	if(!ret) return *this;
	char* p = convBuf;
	p[ret] = ' ';
	var = (int16_t)strtol(p,&p,0);
	seek(p - convBuf + 1 - ret,XS_CUR);
	return *this;
}

XStream& XStream::operator>= (uint16_t& var)
{
	int ret = read(convBuf,6);
	if(!ret) return *this;
	char* p = convBuf;
	p[ret] = ' ';
	var = (uint16_t)strtoul(p,&p,0);
	seek(p - convBuf + 1 - ret,XS_CUR);
	return *this;
}

XStream& XStream::operator>= (int32_t& var)
{
	int ret = read(convBuf,16);
	if(!ret) return *this;
	char* p = convBuf;
	p[ret] = ' ';
	var = (int32_t)strtol(p,&p,0);
	seek(p - convBuf + 1 - ret,XS_CUR);
	return *this;
}

XStream& XStream::operator>= (uint32_t& var)
{
	int ret = read(convBuf,16);
	if(!ret) return *this;
	char* p = convBuf;
	p[ret] = ' ';
	var = (uint32_t)strtoul(p,&p,0);
	seek(p - convBuf + 1 - ret,XS_CUR);
	return *this;
}

XStream& XStream::operator>= (int64_t& var)
{
	int ret = read(convBuf,16);
	if(!ret) return *this;
	char* p = convBuf;
	p[ret] = ' ';
	var = strtoll(p,&p,0);
	seek(p - convBuf + 1 - ret,XS_CUR);
	return *this;
}

XStream& XStream::operator>= (uint64_t& var)
{
	int ret = read(convBuf,16);
	if(!ret) return *this;
	char* p = convBuf;
	p[ret] = ' ';
	var = strtoull(p,&p,0);
	seek(p - convBuf + 1 - ret,XS_CUR);
	return *this;
}

XStream& XStream::operator>= (double& var)
{
	int ret = read(convBuf,XS_CONV_BUFFER_LEN);
	if(!ret) return *this;
	char* p = convBuf;
	p[ret] = ' ';
	var = string_to_double(p,&p);
	seek(p - convBuf + 1 - ret,XS_CUR);
	return *this;
}

XStream& XStream::operator>= (float& var)
{
	int ret = read(convBuf,XS_CONV_BUFFER_LEN);
	if(!ret) return *this;
	char* p = convBuf;
	p[ret] = ' ';
	var = string_to_float(p,&p);
	seek(p - convBuf + 1 - ret,XS_CUR);
	return *this;
}
