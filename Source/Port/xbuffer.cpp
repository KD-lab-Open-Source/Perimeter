#include "StdAfx.h"
#include "xutil.h"

XBuffer::XBuffer(unsigned int sz /*= XB_DEFSIZE*/, int automatic_realloc_ /*= 0*/)
    : automatic_realloc(automatic_realloc_)
{
    alloc(sz);
}

XBuffer::XBuffer(void* p, int sz)
{
    buf = reinterpret_cast<char*>(p);
    size = sz;
    MakeFree = 0;
}

void XBuffer::alloc(unsigned int sz)
{
    buf = new char[size = sz];
    offset = 0;
    MakeFree = 1;
    *buf = 0;
}

void XBuffer::free()
{
    if (MakeFree)
    {
        delete[] buf;
        buf = NULL;
    }
}

void XBuffer::set(int off, int mode /*= XB_BEG*/)
{
    switch (mode)
    {
    case XB_BEG: offset = off; break;
    case XB_CUR: offset += off; break;
    case XB_END: offset = size - off; break;
    }
}

unsigned int XBuffer::read(void* s, unsigned int len)
{
    // FIXME
    return 0;
}
unsigned int XBuffer::write(const void* s, unsigned int len, int bin_flag /*= 1*/)
{
    //FIXME
    return 0;
}

void XBuffer::handleOutOfSize()
{
    //FIXME
}

XBuffer& XBuffer::operator< (const char* v)
{
    write(v, strlen(v));
    return *this;
}

XBuffer& XBuffer::operator>= (char&var)
{
    char* p = buf + offset;
    var = std::strtol(p, &p, 0);
    offset += p - (buf + offset);
    return *this;
}

XBuffer& XBuffer::operator>= (unsigned char&var)
{
    char* p = buf + offset;
    var = std::strtoul(p, &p, 0);
    offset += p - (buf + offset);
    return *this;
}

XBuffer& XBuffer::operator>= (short&var)
{
    char* p = buf + offset;
    var = std::strtol(p, &p, 0);
    offset += p - (buf + offset);
    return *this;
}

XBuffer& XBuffer::operator>= (unsigned short&var)
{
    char* p = buf + offset;
    var = std::strtoul(p, &p, 0);
    offset += p - (buf + offset);
    return *this;
}

XBuffer& XBuffer::operator>= (int&var)
{
    char* p = buf + offset;
    var = std::strtol(p, &p, 0);
    offset += p - (buf + offset);
    return *this;
}

XBuffer& XBuffer::operator>= (unsigned int&var)
{
    char* p = buf + offset;
    var = std::strtoul(p, &p, 0);
    offset += p - (buf + offset);
    return *this;
}

XBuffer& XBuffer::operator>= (long&var)
{
    char* p = buf + offset;
    var = std::strtoll(p, &p, 0);
    offset += p - (buf + offset);
    return *this;
}

XBuffer& XBuffer::operator>= (unsigned long&var)
{
    char* p = buf + offset;
    var = std::strtoull(p, &p, 0);
    offset += p - (buf + offset);
    return *this;
}

XBuffer& XBuffer::operator>= (float&var)
{
    char* p = buf + offset;
    var = std::strtof(p, &p);
    offset += p - (buf + offset);
    return *this;
}

XBuffer& XBuffer::operator>= (double&var)
{
    char* p = buf + offset;
    var = std::strtod(p, &p);
    offset += p - (buf + offset);
    return *this;
}

XBuffer& XBuffer::operator>= (long double&var)
{
    char* p = buf + offset;
    var = std::strtold(p, &p);
    offset += p - (buf + offset);
    return *this;
}

XBuffer& XBuffer::operator<= (char var)
{
    std::string s = std::to_string(var);
    write(s.c_str(), s.size(), 0);
    return *this;
}

XBuffer& XBuffer::operator<= (unsigned char var)
{
    std::string s = std::to_string(var);
    write(s.c_str(), s.size(), 0);
    return *this;
}

XBuffer& XBuffer::operator<= (short var)
{
    std::string s = std::to_string(var);
    write(s.c_str(), s.size(), 0);
    return *this;
}


XBuffer& XBuffer::operator<= (unsigned short var)
{
    std::string s = std::to_string(var);
    write(s.c_str(), s.size(), 0);
    return *this;
}

XBuffer& XBuffer::operator<= (int var)
{
    std::string s = std::to_string(var);
    write(s.c_str(), s.size(), 0);
    return *this;
}

XBuffer& XBuffer::operator<= (unsigned int var)
{
    std::string s = std::to_string(var);
    write(s.c_str(), s.size(), 0);
    return *this;
}

XBuffer& XBuffer::operator<= (long var)
{
    std::string s = std::to_string(var);
    write(s.c_str(), s.size(), 0);
    return *this;
}

XBuffer& XBuffer::operator<= (unsigned long var)
{
    std::string s = std::to_string(var);
    write(s.c_str(), s.size(), 0);
    return *this;
}

XBuffer& XBuffer::operator<= (float var)
{
    std::string s = std::to_string(var);
    write(s.c_str(), s.size(), 0);
    return *this;
}

XBuffer& XBuffer::operator<= (double var)
{
    std::string s = std::to_string(var);
    write(s.c_str(), s.size(), 0);
    return *this;
}

XBuffer& XBuffer::operator<= (long double var)
{
    std::string s = std::to_string(var);
    write(s.c_str(), s.size(), 0);
    return *this;
}
