#include "windows.h"
//TODO
//#include "C:\Balmer\Perimeter\CrtMalloc\MemoryInfo\stackwalker.h"
//#include "C:\Balmer\Perimeter\CrtMalloc\MemoryInfo\stackwalker.cpp"
#include <string>
#include <stdlib.h>


void __stdcall rdProcessStack(DWORD addres,DWORD param)
{
	string& out=*(string*)param;
	const buf_max=256;
	char file_name[buf_max];
	int line;
	char func_name[buf_max];
	char buffer[1024];
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	if(DebugInfoByAddr(addres,file_name,line,func_name,buf_max))
	{
		_splitpath(file_name,drive,dir,fname,ext);
		sprintf(buffer,"%s%s(%i) %s\n",fname,ext,line,func_name);
		out+=buffer;
	}

}

void GetStack(string& out)
{
	ProcessStack(rdProcessStack,(DWORD)&out);
}
