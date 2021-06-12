#pragma once
#include <my_STL.h>
#ifdef _WIN32
#include <windows.h>
#endif

#ifndef _DEBUG

#define ASSERT( expr ) ( (void)0 )

#else//_DEBUG
int __cdecl AssertFail(char szErr[], char szFileName[], int nLine, char szMessage[]);
#define MDebugBreak() __asm { int 3 }
#define ASSERT(f) \
		do {\
			if(!(f))\
			{\
				if(AssertFail(#f, __FILE__, __LINE__,NULL))\
				{\
					MDebugBreak();\
				}\
			}\
		} while(0)

#endif//_DEBUG 

void ERRORM(BOOL b,char *format, ...);

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>
#include <vector>

#define MAXSTRLEN 1024
#include "Win2Dos.h"