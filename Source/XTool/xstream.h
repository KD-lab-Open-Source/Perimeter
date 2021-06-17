#ifndef __XSTREAM_H
#define __XSTREAM_H

#include <string>

#define XS_CONV_BUFFER_LEN	63

#define XS_IN		0x0001
#define XS_OUT		0x0002
#define XS_NOREPLACE	0x0004
#define XS_APPEND	0x0008
#define XS_NOBUFFERING	0x0010
#define XS_NOSHARING	0x0020

#define XS_BEG		0
#define XS_CUR		1
#define XS_END		2

#define XS_DEFDIGITS	8

struct XStream
{
	typedef void* XSHANDLE;

	XSHANDLE handler;
	long	pos;
	int	eofFlag;
	int	ErrHUsed;
	bool ioError_;
	const char* fname;
	long	extSize;
	long	extPos;

	XStream(int err = 1);
	XStream(const char* name, unsigned flags,int err = 1);
	~XStream();

	int	open(const char* name, unsigned f = XS_IN);
	int	open(XStream* owner,long start,long ext_sz = -1);
	void	close();
	unsigned long read(void* buf, unsigned long len);
	unsigned long write(const void* buf, unsigned long len);
	long	seek(long offset, int dir);
	long	tell() const { return pos; }
	char*	getline(char* buf, unsigned len);
	int	eof(){ return eofFlag || pos >= size(); }
	long	size() const;
	//int	gethandler(){ return (int)handler; } //TODO seems unused? also not allowed in modern C
	//void	gettime(unsigned& date,unsigned& time);
	void	flush();
	const char*	GetFileName() const { return fname; }

	XStream& operator< (const char*);
	XStream& operator< (char);
	XStream& operator< (unsigned char);
	XStream& operator< (short);
	XStream& operator< (unsigned short);
	XStream& operator< (int);
	XStream& operator< (unsigned int);
	XStream& operator< (long);
	XStream& operator< (unsigned long);
	XStream& operator< (float);
	XStream& operator< (double);
	XStream& operator< (long double);

	XStream& operator> (char*);
	XStream& operator> (char&);
	XStream& operator> (unsigned char&);
	XStream& operator> (short&);
	XStream& operator> (unsigned short&);
	XStream& operator> (int&);
	XStream& operator> (unsigned int&);
	XStream& operator> (long&);
	XStream& operator> (unsigned long&);
	XStream& operator> (float&);
	XStream& operator> (double&);
	XStream& operator> (long double&);

    template<typename T>
    XStream& operator<= (T var) {
        std::string str = std::to_string(var);
        write(str.c_str(), str.length());
        return *this;
    }
	XStream& operator<= (float);
	XStream& operator<= (double);
	XStream& operator<= (long double);

	XStream& operator>= (char&);
	XStream& operator>= (unsigned char&);
	XStream& operator>= (short&);
	XStream& operator>= (unsigned short&);
	XStream& operator>= (int&);
	XStream& operator>= (unsigned int&);
	XStream& operator>= (long&);
	XStream& operator>= (unsigned long&);
	XStream& operator>= (float&);
	XStream& operator>= (double&);
	XStream& operator>= (long double&);

	bool isOpen() const { return handler != XSHANDLE(-1); }
	bool ioError() const { return ioError_; }

	int operator! (){ if(handler != XSHANDLE(-1)) return 1; else return 0; } // Obsolete
	operator void* (){ if(handler != XSHANDLE(-1)) return 0; else return this; } // Obsolete

	template<class T> XStream& write(const T& v){ write(&v, sizeof(T)); return *this; }
	template<class T> XStream& read(T& v){ read(&v, sizeof(T)); return *this; }

private:
	char convBuf[XS_CONV_BUFFER_LEN + 1];
};


#endif /* __XSTREAM_H */


