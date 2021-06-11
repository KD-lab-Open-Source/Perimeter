#include "StdAfx.h"
#include "xutil.h"

XStream::XStream(int err /*= 1*/)
{

}

XStream::XStream(const char* name, unsigned flags, int err /*= 1*/)
{

}

XStream::~XStream()
{
    close();
}

int	XStream::open(const char* name, unsigned f /*= XS_IN*/)
{
    return 0;
}

int	XStream::open(XStream* owner, long start, long ext_sz /*= -1*/)
{
    return 0;
}

void XStream::close()
{

}

long XStream::seek(long offset, int dir)
{
    return 0;
}

long	XStream::size() const
{
    return 0;
}

unsigned long XStream::read(void* buf, unsigned long len)
{
    //FIXME
    return 0;
}
unsigned long XStream::write(const void* buf, unsigned long len)
{
    //FIXME
    return 0;
}

XStream& XStream::operator< (const char* v)
{
    write(v, strlen(v));
    return *this;
}

XStream& XStream::operator< (char v)
{
    write(&v, (unsigned)(sizeof v));
    return *this;
}

XStream& XStream::operator< (unsigned char v)
{
    write(&v, (unsigned)(sizeof v));
    return *this;
}
XStream& XStream::operator< (short v)
{
    write(&v, (unsigned)(sizeof v));
    return *this;
}
XStream& XStream::operator< (unsigned short v)
{
    write(&v, (unsigned)(sizeof v));
    return *this;
}
XStream& XStream::operator< (int v)
{
    write(&v, (unsigned)(sizeof v));
    return *this;
}
XStream& XStream::operator< (unsigned int v)
{
    write(&v, (unsigned)(sizeof v));
    return *this;
}
XStream& XStream::operator< (long v)
{
    write(&v, (unsigned)(sizeof v));
    return *this;
}
XStream& XStream::operator< (unsigned long v)
{
    write(&v, (unsigned)(sizeof v));
    return *this;
}
XStream& XStream::operator< (float v)
{
    write(&v, (unsigned)(sizeof v));
    return *this;
}
XStream& XStream::operator< (double v)
{
    write(&v, (unsigned)(sizeof v));
    return *this;
}
XStream& XStream::operator< (long double v)
{
    write(&v, (unsigned)(sizeof v));
    return *this;
}


XStream& XStream::operator> (char*v)
{
    read(v, (unsigned)strlen(v));
    return *this;
}

XStream& XStream::operator> (char&v)
{
    read(&v, (unsigned)(sizeof v));
    return *this;
}
XStream& XStream::operator> (unsigned char&v)
{
    read(&v, (unsigned)(sizeof v));
    return *this;
}
XStream& XStream::operator> (short& v)
{
    read(&v, (unsigned)(sizeof v));
    return *this;
}
XStream& XStream::operator> (unsigned short&v)
{
    read(&v, (unsigned)(sizeof v));
    return *this;
}
XStream& XStream::operator> (int&v)
{
    read(&v, (unsigned)(sizeof v));
    return *this;
}
XStream& XStream::operator> (unsigned int&v)
{
    read(&v, (unsigned)(sizeof v));
    return *this;
}
XStream& XStream::operator> (long&v)
{
    read(&v, (unsigned)(sizeof v));
    return *this;
}
XStream& XStream::operator> (unsigned long&v)
{
    read(&v, (unsigned)(sizeof v));
    return *this;
}
XStream& XStream::operator> (float&v)
{
    read(&v, (unsigned)(sizeof v));
    return *this;
}
XStream& XStream::operator> (double&v)
{
    read(&v, (unsigned)(sizeof v));
    return *this;
}
XStream& XStream::operator> (long double&v)
{
    read(&v, (unsigned)(sizeof v));
    return *this;
}

XStream& XStream::operator<= (char)
{
    // FIXME
    return *this;
}
XStream& XStream::operator<= (unsigned char)
{
    // FIXME
    return *this;
}
XStream& XStream::operator<= (short)
{
    // FIXME
    return *this;
}
XStream& XStream::operator<= (unsigned short)
{
    // FIXME
    return *this;
}
XStream& XStream::operator<= (int)
{
    // FIXME
    return *this;
}
XStream& XStream::operator<= (unsigned int)
{
    // FIXME
    return *this;
}
XStream& XStream::operator<= (long)
{
    // FIXME
    return *this;
}
XStream& XStream::operator<= (unsigned long)
{
    // FIXME
    return *this;
}
XStream& XStream::operator<= (float)
{
    // FIXME
    return *this;
}
XStream& XStream::operator<= (double)
{
    // FIXME
    return *this;
}
XStream& XStream::operator<= (long double)
{
    // FIXME
    return *this;
}



XStream& XStream::operator>= (char&)
{
    // FIXME
    return *this;
}
XStream& XStream::operator>= (unsigned char&)
{
    // FIXME
    return *this;
}
XStream& XStream::operator>= (short&)
{
    // FIXME
    return *this;
}
XStream& XStream::operator>= (unsigned short&)
{
    // FIXME
    return *this;
}
XStream& XStream::operator>= (int&)
{
    // FIXME
    return *this;
}
XStream& XStream::operator>= (unsigned int&)
{
    // FIXME
    return *this;
}
XStream& XStream::operator>= (long&)
{
    // FIXME
    return *this;
}
XStream& XStream::operator>= (unsigned long&)
{
    // FIXME
    return *this;
}
XStream& XStream::operator>= (float&)
{
    // FIXME
    return *this;
}
XStream& XStream::operator>= (double&)
{
    // FIXME
    return *this;
}
XStream& XStream::operator>= (long double&)
{
    // FIXME
    return *this;
}