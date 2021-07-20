//** 1999 Creator - Balmer **//
#include "tweaks.h"
//#include "StdAfx.h"
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <vector>
#include <string>
#include "xmath.h"

#include <cstdio>
#include "Saver.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>

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

void CSaver::push(const unsigned long id)
{
	fwrite(&id,sizeof(id),1,f);
	push();
}

void CSaver::push()
{
	DWORD w=0;
	WR(w);
    long t = ftell(f);

	p.push_back(t);
}

void CSaver::pop()
{
    long t = ftell(f);

	int n=p.size()-1;
	DWORD min=p[n];
    long tt=min-4;
    fseek(f,tt,SEEK_SET);
	DWORD size=DWORD(t)-min;
	WR(size);

	p.pop_back();


	fseek(f,t,SEEK_SET);
}

/////////////////////////////////////////////////
CLoadDirectory::CLoadDirectory(BYTE* data,DWORD _size)
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
	cur+=cl->size+2*sizeof(DWORD);
	return cl;
}


CLoadDirectoryFile::CLoadDirectoryFile()
:CLoadDirectory(0,0)
{
}

CLoadDirectoryFile::~CLoadDirectoryFile()
{
	delete begin;
}

bool CLoadDirectoryFile::Load(LPCSTR filename)
{
	int file=_open(filename,_O_RDONLY|_O_BINARY);
	if(file==-1)return false;

	size=_lseek(file,0,SEEK_END);
	if(size<0)return false;
	_lseek(file,0,SEEK_SET);
	begin=new BYTE[size];
	_read(file,begin,size);
	_close(file);

	cur=0;
	return true;
}



///////////////////
void SaveString(CSaver& s,LPCSTR str,DWORD ido)
{
	if(str==NULL)return;
	s.push(ido);
	s<<str;
	s.pop();
}
