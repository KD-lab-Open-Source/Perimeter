#ifndef __XBUFFER_H
#define __XBUFFER_H

#include <string>
#include <cstring>
#include "types.h"

#ifndef NULL
#define NULL	0L
#endif

#define XB_CONV_BUFFER_LEN	63

#define XB_DEFSIZE	256

#define XB_BEG		0
#define XB_CUR		1
#define XB_END		2

#define XB_FORWARD	0
#define XB_BACKWARD	1
#define XB_GLOBAL	2

#define XB_CASEON	0
#define XB_CASEOFF	1

#define XB_DEFDIGITS	8

struct XBuffer
{
	char* buf;
	size_t size;
	size_t offset;
	int digits;
	bool automatic_free;
	bool automatic_realloc;

	explicit XBuffer(size_t sz = XB_DEFSIZE, bool automatic_realloc_ = false);
	XBuffer(void* p, size_t sz);
	~XBuffer() { free(); }
    
    XBuffer(const XBuffer&) = delete;
    XBuffer& operator=(XBuffer const&) = delete;

	void SetDigits(int d) { digits = d; }

	void alloc(size_t sz);
	void free();
	void fill(char fc = '\0');
	void set(size_t off, int mode = XB_BEG);
	void init() { offset = 0; *buf = 0; }
    size_t search(char* what,int mode = XB_FORWARD, int cs = XB_CASEON);
    size_t end() const { return (offset > size); }

	size_t tell() const { return offset; }
	size_t length() const { return size; }
	char* address(){ return buf; }

	size_t read(void* s, size_t len);
	size_t write(const void* s, size_t len, bool bin_flag = true);
	void handleOutOfSize();

    template<typename T> XBuffer& write(const T& v){ while(offset + sizeof(T) > size) handleOutOfSize(); memcpy(&buf[offset], &v, sizeof(T)); offset += sizeof(T); return *this; }
    template<typename T> XBuffer& read(T& v){ memcpy(&v, &buf[offset], sizeof(T)); offset += sizeof(T); return *this; }
	
	XBuffer& operator< (const char* v);
	XBuffer& operator< (char v) { return write(v); }
    XBuffer& operator< (int8_t v) { return write(v); }
	XBuffer& operator< (uint8_t v) { return write(v); }
	XBuffer& operator< (int16_t v) { return write(v); }
	XBuffer& operator< (uint16_t v) { return write(v); }
	XBuffer& operator< (int32_t v ) { return write(v); }
	XBuffer& operator< (uint32_t v) { return write(v); }
	XBuffer& operator< (int64_t v) { return write(v); }
	XBuffer& operator< (uint64_t v) { return write(v); }
	XBuffer& operator< (float v) { return write(v); }
	XBuffer& operator< (double v) { return write(v); }

	XBuffer& operator> (char* v);
	XBuffer& operator> (char& v) { return read(v); }
    XBuffer& operator> (int8_t& v) { return read(v); }
	XBuffer& operator> (uint8_t& v) { return read(v); }
	XBuffer& operator> (int16_t& v) { return read(v); }
	XBuffer& operator> (uint16_t& v) { return read(v); }
	XBuffer& operator> (int32_t& v) { return read(v); }
	XBuffer& operator> (uint32_t& v) { return read(v); }
	XBuffer& operator> (int64_t& v) { return read(v); }
	XBuffer& operator> (uint64_t& v) { return read(v); }
	XBuffer& operator> (float& v) { return read(v); }
	XBuffer& operator> (double& v) { return read(v); }

    template<typename T>
    XBuffer& operator<= (T var) {
        std::string str = std::to_string(var);
        write(str.c_str(), str.length(), false);
        return *this;
    }
    XBuffer& operator<= (const char* var) {
        write(var, strlen(var), false);
        return *this;
    }
    XBuffer& operator<= (const std::string& var) {
        write(var.c_str(), var.length(), false);
        return *this;
    }
	XBuffer& operator<= (float);
	XBuffer& operator<= (double);

    XBuffer& operator>= (char&);
    XBuffer& operator>= (int8_t&);
    XBuffer& operator>= (uint8_t&);
	XBuffer& operator>= (int16_t&);
	XBuffer& operator>= (uint16_t&);
	XBuffer& operator>= (int32_t&);
	XBuffer& operator>= (uint32_t&);
	XBuffer& operator>= (int64_t&);
	XBuffer& operator>= (uint64_t&);
	XBuffer& operator>= (float&);
	XBuffer& operator>= (double&);

#ifdef _WIN32
    //These are required for Windows which doesn't allow "long" to be implicitly casted to std types...
    XBuffer& operator> (long& v) { return this->operator>(checked_reinterpret_cast_ref<long, int32_t>(v)); }
    XBuffer& operator> (unsigned long& v) { return this->operator>(checked_reinterpret_cast_ref<unsigned long, uint32_t>(v)); }
    XBuffer& operator>= (long& v) { return this->operator>=(checked_reinterpret_cast_ref<long, int32_t>(v)); };
    XBuffer& operator>= (unsigned long& v) { return this->operator>=(checked_reinterpret_cast_ref<unsigned long, uint32_t>(v)); };
#endif

	operator const char* () const { return buf; }
	const char* operator ()(int offs){ return buf + offs; }
	XBuffer& operator++(){ offset++; return *this; }
	XBuffer& operator--(){ offset--; return *this; }
	XBuffer& operator+= (int v){ offset+=v; return *this; }
	XBuffer& operator-= (int v){ offset-=v; return *this; }

	const char& operator[](int ind) const { return buf[ind]; }
	const char& operator()() const { return buf[offset]; }

	char& operator[](int ind){ return buf[ind]; }
	char& operator()(){ return buf[offset]; }

private:
	char convBuf[XB_CONV_BUFFER_LEN + 1];
};


#endif /* __XBUFFER_H */

