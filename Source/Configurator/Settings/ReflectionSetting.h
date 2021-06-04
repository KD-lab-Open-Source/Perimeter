// ReflectionSetting.h: interface for the ReflectionSetting class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REFLECTIONSETTING_H__CF6A26DC_C148_431A_880C_551A7BFAAD35__INCLUDED_)
#define AFX_REFLECTIONSETTING_H__CF6A26DC_C148_431A_880C_551A7BFAAD35__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CheckSetting.h"

class ReflectionSetting : public CheckSetting  
{
public:
	ReflectionSetting(UINT ctrlId, UINT sectionId);
	virtual ~ReflectionSetting();

	bool readFromStorage();
	bool saveToStorage() const;

};

#endif // !defined(AFX_REFLECTIONSETTING_H__CF6A26DC_C148_431A_880C_551A7BFAAD35__INCLUDED_)
