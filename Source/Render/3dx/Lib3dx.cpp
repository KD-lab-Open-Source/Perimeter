#include "StdAfxRD.h"
#include "Lib3dx.h"

cLib3dx* pLibrary3dx=NULL;

cLib3dx::cLib3dx()
{
	pLibrary3dx=this;
}

cLib3dx::~cLib3dx()
{
	ObjectMap::iterator it;
	FOR_EACH(objects,it)
	{
		delete it->second;
	}

	pLibrary3dx=NULL;
}

cStatic3dx* cLib3dx::GetElement(const char* fname_,const char* TexturePath,bool is_logic)
{
	string fname(fname_);
	strupr((char*)fname.c_str());
	ObjectMap::iterator it=objects.find(fname);
	if(it!=objects.end())
	{
		return it->second;
	}

	cStatic3dx* pStatic=NULL;
	CLoadDirectoryFile rd;
	if(rd.Load(fname.c_str()))
	{
		pStatic=new cStatic3dx(is_logic,fname.c_str());
		pStatic->Load(rd);
	}

	objects[fname]=pStatic;
	return pStatic;
}