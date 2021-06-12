#include "xglobal.h"
//#include <ostream.h>

void xtSysFinit(){}
void xtClearMessageQueue(){}

void win32_break(char* error,char* msg)
{
//	cerr << "--------------------------------\n";
//	cerr << error << "\n";
//	cerr << msg << "\n";
//	cerr  << "--------------------------------" << endl;
	_ASSERT(FALSE) ;
}



