#pragma once
#include "Static3dx.h"

class cLib3dx
{
public:
	cLib3dx();
	~cLib3dx();

	cStatic3dx* GetElement(const char* fname,const char* TexturePath,bool is_logic);
protected:
	typedef map<string,cStatic3dx*> ObjectMap;
	ObjectMap objects;
};

extern cLib3dx* pLibrary3dx;