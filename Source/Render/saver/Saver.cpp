//** 1999 Creator - Balmer **//
#include <vector>
#include <string>
#include <cstring>
#include <cstdio>
#include <memory>
#include "xmath.h"

#include "Saver.h"

#include <fcntl.h>
//#include <sys/types.h>
//#include <sys/stat.h>
#include "files/files.h"

#undef WR
#define WR(x) fwrite(&x,sizeof(x),1,f);

CSaver::CSaver(const  char* name)
{
	Init(name);
}

CSaver::CSaver()
{
	f=nullptr;
}

CSaver::~CSaver()
{
	if(f)fclose(f);
}

bool CSaver::Init(const char* name)
{
	p.clear();
	f=fopen(name,"w+b");
	return f!=nullptr;
}

void CSaver::push(uint32_t id)
{
	fwrite(&id,sizeof(id),1,f);
	push();
}

void CSaver::push()
{
	uint32_t w=0;
	WR(w);
    long t = ftell(f);

	p.push_back(t);
}

void CSaver::pop()
{
    long t = ftell(f);

	int n=p.size()-1;
	uint32_t min=p[n];
    long tt=min-4;
    fseek(f,tt,SEEK_SET);
	uint32_t size=uint32_t(t)-min;
	WR(size);

	p.pop_back();


	fseek(f,t,SEEK_SET);
}

/////////////////////////////////////////////////
CLoadDirectory::CLoadDirectory(uint8_t* data,uint32_t _size)
		:begin(data),cur(NULL),size(_size)
{
}

CLoadDirectory::CLoadDirectory(CLoadData* ld)
{
	begin=ld->data;
	size=ld->size;
	cur=0;
}

CLoadData* CLoadDirectory::next()
{
	if(cur>=begin+size)return NULL;

	if(cur==NULL)cur=begin;

	CLoadData* cl=(CLoadData*)cur;
	cur+=cl->size+2*sizeof(uint32_t);
	return cl;
}


CLoadDirectoryFile::CLoadDirectoryFile()
:CLoadDirectory(0,0)
{
}

CLoadDirectoryFile::~CLoadDirectoryFile()
{
	delete[] begin;
}

bool CLoadDirectoryFile::Load(const char* filename)
{
	int file= file_open(filename, _O_RDONLY | _O_BINARY);
	if(file==-1)return false;

	size=_lseek(file,0,SEEK_END);
	if(size<0)return false;
	_lseek(file,0,SEEK_SET);
	begin=new uint8_t[size];
	_read(file,begin,size);
	_close(file);

	cur=0;
	return true;
}



///////////////////
void SaveString(CSaver& s,const char* str,uint32_t ido)
{
	if(str==NULL)return;
	s.push(ido);
	s<<str;
	s.pop();
}
