#ifndef __XSTREAM_H
#define __XSTREAM_H

#include <cstring>
#include <cstdint>
#include <string>

#define XS_CONV_BUFFER_LEN	63

#define XS_IN		0x0001
#define XS_OUT		0x0002
#define XS_NOREPLACE	0x0004
#define XS_APPEND	0x0008

#define XS_BEG		0
#define XS_CUR		1
#define XS_END		2

#define XS_DEFDIGITS	8

struct XStream
{
	typedef std::fstream* XSHANDLE;

	XSHANDLE handler;
    bool free_handler;
    int64_t	pos;
	bool	eofFlag;
	int	ErrHUsed;
	std::string fname;
	int64_t	extSize;
	int64_t	extPos;

	XStream(int err = 1);
    XStream(const char* name, unsigned flags,int err = 1);
    XStream(const std::string& name, unsigned flags,int err = 1);
	~XStream();

    XStream(const XStream& xs) = delete;
    XStream& operator=(XStream xs) = delete;

    int	open(const char* name, unsigned f = XS_IN);
    int	open(const std::string& name, unsigned f = XS_IN);
	int	open(XStream* owner,int64_t start,int64_t ext_sz = -1);
	void	close();
	int64_t read(void* buf, int64_t len);
    int64_t write(const void* buf, int64_t len);
    int64_t write_str(const std::string& str);
    int64_t	seek(int64_t offset, int dir);
    int64_t	tell() const { return pos; }
	char*	getline(char* buf, int64_t len);
	bool eof() const { return eofFlag || pos >= size(); }
    int64_t	size() const;
	//int	gethandler(){ return (int)handler; } //TODO seems unused? also not allowed in modern C
	//void	gettime(unsigned& date,unsigned& time);
	void	flush();
	const char*	GetFileName() const { return fname.c_str(); }

    template<typename T> XStream& write(const T& v){ write(&v, sizeof(T)); return *this; }
    template<typename T> XStream& read(T& v){ read(&v, sizeof(T)); return *this; }

    XStream& operator< (const char* v) { write(v, strlen(v)); return *this; }
    XStream& operator< (char v) { return write(v); }
    XStream& operator< (int8_t v) { return write(v); }
    XStream& operator< (uint8_t v) { return write(v); }
    XStream& operator< (int16_t v) { return write(v); }
    XStream& operator< (uint16_t v) { return write(v); }
    XStream& operator< (int32_t v ) { return write(v); }
    XStream& operator< (uint32_t v) { return write(v); }
    XStream& operator< (int64_t v) { return write(v); }
    XStream& operator< (uint64_t v) { return write(v); }
    XStream& operator< (float v) { return write(v); }
    XStream& operator< (double v) { return write(v); }

    XStream& operator> (char* v) { read(v,(unsigned)strlen(v)); return *this; }
    XStream& operator> (char& v) { return read(v); }
    XStream& operator> (int8_t& v) { return read(v); }
    XStream& operator> (uint8_t& v) { return read(v); }
    XStream& operator> (int16_t& v) { return read(v); }
    XStream& operator> (uint16_t& v) { return read(v); }
    XStream& operator> (int32_t& v) { return read(v); }
    XStream& operator> (uint32_t& v) { return read(v); }
    XStream& operator> (int64_t& v) { return read(v); }
    XStream& operator> (uint64_t& v) { return read(v); }
    XStream& operator> (float& v) { return read(v); }
    XStream& operator> (double& v) { return read(v); }

    template<typename T>
    XStream& operator<= (T var) {
        std::string str = std::to_string(var);
        write(str.c_str(), str.length());
        return *this;
    }
	XStream& operator<= (float);
	XStream& operator<= (double);

	XStream& operator>= (char& var);
	XStream& operator>= (int8_t& var);
	XStream& operator>= (uint8_t& var);
	XStream& operator>= (int16_t& var);
	XStream& operator>= (uint16_t& var);
	XStream& operator>= (int32_t& var);
	XStream& operator>= (uint32_t& var);
	XStream& operator>= (int64_t& var);
	XStream& operator>= (uint64_t& var);
    XStream& operator>= (float&);
	XStream& operator>= (double&);

	//Apparently never set but is checked
	bool ioError() const { return false; }

private:
	char convBuf[XS_CONV_BUFFER_LEN + 1];
};


#endif /* __XSTREAM_H */


