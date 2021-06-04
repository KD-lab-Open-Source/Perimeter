#include <float.h>

#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include "StreamBuffer.h"

#define STREAM_BUFFER_RESIZE					1024

#

inline double StringToFloat(char *buf)
{
	int i;
	double a=0;
	if(buf[0]==0) return a;
	double f_sign,f_int=0,f_fract=0,f_power=0;
	for(i=0;buf[i]!='+'&&buf[i]!='-'&&buf[i]<'0'&&buf[i]>'9';i++)
		if(buf[i]) return a;
	// вычислене целой части
	if(buf[i]=='-') { f_sign=-1; i++; } else { if(buf[i]=='+') i++; f_sign=1; }
	for(;buf[i]&&'0'<=buf[i]&&buf[i]<='9';i++)
		f_int=(f_int*10)+(buf[i]-'0');
	if(buf[i]==0) 
	{
		assert(_finite(a));
		return a=f_sign*f_int;
	}
	// вычислене дробной части
	if(buf[i]=='.'||buf[i]==',')
	{ 
		i++;
		for(double f_count=0.1f;buf[i]&&'0'<=buf[i]&&buf[i]<='9';i++,f_count*=0.1f)
			f_fract+=(buf[i]-'0')*f_count;
	}
	if(buf[i]==0) 
	{
		assert(_finite(a));
		return a=f_sign*(f_int+f_fract);
	}
	// вычисление степени числа
	if(buf[i]=='e')
	{
		i++;
		double f_sign_power;
		if(buf[i]==0) 
		{
			assert(_finite(a));
			return a=f_sign*(f_int+f_fract);
		}
		if(buf[i]=='-') { f_sign_power=-1; i++; } else { if(buf[i]=='+') i++; f_sign_power=1; }
		for(;buf[i]&&'0'<=buf[i]&&buf[i]<='9';i++)
			f_power=(f_power*10)+(buf[i]-'0');
		f_power=pow(10.,f_power*f_sign_power);
	}
	assert(_finite(a));
	return a=f_sign*(f_int+f_fract)*f_power;
}

cStream::cStream()
{
	Attribute=0; lpBuffer=0;
	CurrentPos=CurrentSize=MaxSize=0;
}
cStream::cStream(char *fname,int attribute)
{
	Attribute=0; lpBuffer=0;
	CurrentPos=CurrentSize=MaxSize=0;
	open(fname,attribute);
}
cStream::cStream(int size,int attribute)
{
	Attribute=0; lpBuffer=0;
	CurrentPos=CurrentSize=MaxSize=0;
	open(size,attribute);
}
cStream::~cStream()
{
	close();
}
int cStream::open(char *fname,int attribute)
{
	close();
	return -1;
}
int cStream::open(int size,int attribute)
{
	close();
	Attribute=attribute;
	switch(Attribute&(STREAM_MEM|STREAM_IN|STREAM_OUT))
	{
		case STREAM_MEM|STREAM_IN:
		case STREAM_MEM|STREAM_OUT:
		case STREAM_MEM|STREAM_IN|STREAM_OUT:
			lpBuffer=new char[MaxSize=size];
			break;
		default:
			assert(0);
	}
	return 0;
}
int cStream::open(void *buf,int size,int attribute)
{
	close();
	Attribute=attribute;
	switch(Attribute&(STREAM_MEM|STREAM_IN|STREAM_OUT))
	{
		case STREAM_MEM|STREAM_IN:
		case STREAM_MEM|STREAM_OUT:
		case STREAM_MEM|STREAM_IN|STREAM_OUT:
			if(buf) { lpBuffer=(char*)buf; CurrentSize=MaxSize=size; }
			else lpBuffer=new char[MaxSize=size];
			break;
		default:
			assert(0);
	}
	return 0;
}
void cStream::close()			
{ 
	Attribute=0;
	if(lpBuffer) delete lpBuffer; lpBuffer=0;
	CurrentPos=CurrentSize=MaxSize=0;
}
int cStream::eof()
{ 
	if(lpBuffer&&CurrentPos<CurrentSize) return 0;
	return 1;
}
int cStream::length()
{ 
	if(lpBuffer) return CurrentSize;
	return 0;
}
void cStream::seek(int pos)
{
	CurrentPos=pos;
}
int cStream::write(void *buf,int size)
{
	int NewPos=CurrentPos+size;
	if(NewPos<CurrentSize) 
	{
		memcpy(&lpBuffer[CurrentPos],buf,size);
		CurrentPos=NewPos;
	}
	else if(NewPos<MaxSize) 
	{
		memcpy(&lpBuffer[CurrentPos],buf,size);
		CurrentSize=CurrentPos=NewPos;
	}
	else
	{
		char *tmp=lpBuffer;
		if(size<MaxSize) lpBuffer=new char[MaxSize+=MaxSize];
		else lpBuffer=new char[MaxSize+=size];
		memcpy(lpBuffer,tmp,CurrentSize);
		memcpy(&lpBuffer[CurrentPos],buf,size);
		CurrentSize=CurrentPos=NewPos;
		delete tmp;
	}
	return size;
}
int cStream::read(void *buf,int size)
{
	int d=size-(CurrentSize-CurrentPos);
	if(d<=0) d=size;
	memcpy(buf,&lpBuffer[CurrentPos],d);
	CurrentPos+=d;
	return d;
}
char* cStream::str()
{
	return lpBuffer;
}
int cStream::getline(char *buf,int size)			
{ 
	assert(lpBuffer); 
	buf[0]=0;
	int count=0;
	while(CurrentPos<CurrentSize)
	{
		char ch=lpBuffer[CurrentPos++];
		if(ch!=' '&&ch!='\r'&&ch!='\t'&&ch!='\n') 
		{ buf[count++]=ch; break; }
	}
	while(CurrentPos<CurrentSize)
	{
		char ch=lpBuffer[CurrentPos++];
		if(ch=='\r'||ch=='\n') break;
		buf[count++]=ch;
		if(count>=size) return -1;
	}
	while(CurrentPos<CurrentSize)
	{
		char ch=lpBuffer[CurrentPos];
		if(ch==' '||ch=='\r'||ch=='\t'||ch=='\n') 
		{ ch=lpBuffer[CurrentPos++]; break; }
	}
	buf[count]=0;
	return count; 
}
cStream& cStream::operator << (char a)
{ 
	assert(lpBuffer); 
	char buf[256];
	char *ch=_ltoa(a,buf,10);
	write(&ch,strlen(ch));
	return *this; 
}
cStream& cStream::operator << (unsigned char a)
{ 
	assert(lpBuffer); 
	char buf[256];
	char *ch=_ultoa(a,buf,10);
	write(ch,strlen(ch));
	return *this; 
}
cStream& cStream::operator << (short a)
{ 
	assert(lpBuffer); 
	char buf[256];
	char *ch=_ltoa(a,buf,10);
	write(ch,strlen(ch));
	return *this; 
}
cStream& cStream::operator << (unsigned short a)
{ 
	assert(lpBuffer); 
	char buf[256];
	char *ch=_ultoa(a,buf,10);
	write(ch,strlen(ch));
	return *this; 
}
cStream& cStream::operator << (int a)
{ 
	assert(lpBuffer); 
	char buf[256];
	char *ch=_ltoa(a,buf,10);
	write(ch,strlen(ch));
	return *this; 
}
cStream& cStream::operator << (unsigned int a)
{ 
	assert(lpBuffer); 
	char buf[256];
	char *ch=_ultoa(a,buf,10);
	write(ch,strlen(ch));
	return *this; 
}
cStream& cStream::operator << (long a)
{ 
	assert(lpBuffer); 
	char buf[256];
	char *ch=_ltoa(a,buf,10);
	write(ch,strlen(ch));
	return *this; 
}
cStream& cStream::operator << (unsigned long a)
{ 
	assert(lpBuffer); 
	char buf[256];
	char *ch=_ultoa(a,buf,10);
	write(ch,strlen(ch));
	return *this; 
}
cStream& cStream::operator << (float a)
{ 
	assert(lpBuffer); 
	char buf[256];
	sprintf(buf,"%f",a);
	write(buf,strlen(buf));
	return *this; 
}
cStream& cStream::operator << (double a)
{ 
	assert(lpBuffer); 
	char buf[256];
	sprintf(buf,"%f",a);
	write(buf,strlen(buf));
	return *this; 
}
cStream& cStream::operator << (long double a)
{ 
	assert(lpBuffer); 
	char buf[256];
	sprintf(buf,"%f",a);
	write(buf,strlen(buf));
	return *this; 
}
cStream& cStream::operator << (const char *a)
{ 
	assert(lpBuffer); 
	write((void*)a,strlen(a));
	return *this; 
}
/*
cStream& cStream::operator >> (char &a)
{ 
	assert(lpBuffer); 
	char buf[256];
	(*this)>>buf;
	a=(char)atoi(buf);
	return *this; 
}
cStream& cStream::operator >> (unsigned char &a)
{ 
	assert(lpBuffer); 
	char buf[256];
	(*this)>>buf;
	a=(unsigned char)atoi(buf);
	return *this; 
}
cStream& cStream::operator >> (short &a)
{ 
	assert(lpBuffer); 
	char buf[256];
	(*this)>>buf;
	a=(short)atoi(buf);
	return *this; 
}
cStream& cStream::operator >> (unsigned short &a)
{ 
	assert(lpBuffer); 
	char buf[256];
	(*this)>>buf;
	a=(unsigned short)atoi(buf);
	return *this; 
}
cStream& cStream::operator >> (int &a)
{ 
	assert(lpBuffer); 
	char buf[256];
	(*this)>>buf;
	a=(int)atoi(buf);
	return *this; 
}
cStream& cStream::operator >> (unsigned int &a)
{ 
	assert(lpBuffer); 
	char buf[256];
	(*this)>>buf;
	a=(unsigned int)atoi(buf);
	return *this; 
}
cStream& cStream::operator >> (long &a)
{ 
	assert(lpBuffer); 
	char buf[256];
	(*this)>>buf;
	a=(long)atoi(buf);
	return *this; 
}
cStream& cStream::operator >> (unsigned long &a)
{ 
	assert(lpBuffer); 
	char buf[256];
	(*this)>>buf;
	a=(unsigned long)atoi(buf);
	return *this; 
}
cStream& cStream::operator >> (float &a)
{ 
	assert(lpBuffer); 
	char buf[256];
	(*this)>>buf;
	a=(float)StringToFloat(buf);
	return *this; 
}
cStream& cStream::operator >> (double &a)
{ 
	assert(lpBuffer); 
	char buf[256];
	(*this)>>buf;
	a=(double)StringToFloat(buf);
	return *this; 
}
cStream& cStream::operator >> (long double &a)
{ 
	assert(lpBuffer); 
	char buf[256];
	(*this)>>buf;
	a=(long double)StringToFloat(buf);
	return *this; 
}
cStream& cStream::operator >> (char *buf)
{
	assert(lpBuffer); 
	buf[0]=0;
	int count=0;
	while(CurrentPos<CurrentSize)
	{
		char ch=lpBuffer[CurrentPos++];
		if(ch!=' '&&ch!='\r'&&ch!='\t'&&ch!='\n') { buf[count++]=ch; break; }
	}
	while(CurrentPos<CurrentSize)
	{
		char ch=lpBuffer[CurrentPos++];
		if(ch!=' '&&ch!='\r'&&ch!='\t'&&ch!='\n') buf[count++]=ch;
		else break;
	}
	while(CurrentPos<CurrentSize)
	{
		char ch=lpBuffer[CurrentPos];
		if(ch==' '||ch=='\r'||ch=='\t'||ch=='\n') ch=lpBuffer[CurrentPos++];
		else break;
	}
	buf[count]=0;
	return *this; 
}
*/