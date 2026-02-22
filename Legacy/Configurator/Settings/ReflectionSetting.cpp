// ReflectionSetting.cpp: implementation of the ReflectionSetting class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "configurator.h"
#include "ReflectionSetting.h"
#include "IniReader.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ReflectionSetting::ReflectionSetting(UINT ctrlId, UINT sectionId)
: CheckSetting(ctrlId, sectionId, IDS_KEY_MAP_REFLECTION)
{

}

ReflectionSetting::~ReflectionSetting()
{

}

bool ReflectionSetting::readFromStorage()
{
	return CheckSetting::readFromStorage();
}

bool ReflectionSetting::saveToStorage() const
{
	if (CheckSetting::saveToStorage())
	 return IniReader::writeInt(getSectionId(), 
			IDS_KEY_OBJECT_REFLECTION, 
			getValue(), 
			iniFileName);
	return false;
}


