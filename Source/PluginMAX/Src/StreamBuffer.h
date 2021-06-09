#ifndef __STREAMBUFFER_H__
#define __STREAMBUFFER_H__

#include "assert.h"

#define STREAM_TEXT_ATTRIBUTE				(STREAM_RETURN|STREAM_NEWLINE|STREAM_HORIZTAB|STREAM_SPACEBAR)
#define STREAM_RETURN						0x00010000	/* '\r' for getline */
#define STREAM_NEWLINE						0x00020000	/* '\n' for getline */
#define STREAM_HORIZTAB						0x00100000	/* '\t' */
#define STREAM_SPACEBAR						0x00200000	/* ' '  */
#define STREAM_STRINGLINE(a)				(0x000F0000&(a))

enum eAttributeStream
{
	STREAM_NULL							=	0,
	STREAM_MEM							=	1<<0,
	STREAM_IN							=	1<<2,
	STREAM_OUT							=	1<<3,
};

class cStream
{
public:
	cStream();
	cStream(int size,int attribute);
	cStream(char *fname,int attribute);
	~cStream();
	
	int open(int size,int attribute);
	int open(char *fname,int attribute);
	int open(void *buf,int size,int attribute);
	int eof();					
	int read(void *buf,int size);
	int write(const void *buf,int size);
	int length();
	int tell();
	void seek(int pos);
	void flush();
	void close();
	char* str();
	int getline(char *buf,int size);

	cStream& operator << (char a);
	cStream& operator << (unsigned char a);
	cStream& operator << (short a);
	cStream& operator << (unsigned short a);
	cStream& operator << (int a);
	cStream& operator << (unsigned int a);
	cStream& operator << (long a);
	cStream& operator << (unsigned long a);
	cStream& operator << (float a);
	cStream& operator << (double a);
	cStream& operator << (long double a);
	cStream& operator << (const char *a);
/*
	cStream& operator >> (char &a);
	cStream& operator >> (unsigned char &a);
	cStream& operator >> (short &a);
	cStream& operator >> (unsigned short &a);
	cStream& operator >> (int &a);
	cStream& operator >> (unsigned int &a);
	cStream& operator >> (long &a);
	cStream& operator >> (unsigned long &a);
	cStream& operator >> (float &a);
	cStream& operator >> (double &a);
	cStream& operator >> (long double &a);
	cStream& operator >> (char *a);
*/
private:
	int		Attribute;
	char	*lpBuffer;
	int		CurrentPos,CurrentSize,MaxSize;
};


#endif // __STREAMBUFFER_H__