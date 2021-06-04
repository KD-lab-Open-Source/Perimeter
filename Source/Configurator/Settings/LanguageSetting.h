// LanguageSetting.h: interface for the LanguageSetting class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LanguageSetting_H__E6876981_8AE4_4F1A_8432_E4DA79103B6F__INCLUDED_)
#define AFX_LanguageSetting_H__E6876981_8AE4_4F1A_8432_E4DA79103B6F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SettingBase.h"
#include <list>
#include <string>

class LanguageSetting  : public SettingBase
{
public:
	LanguageSetting(UINT ctrlID);
	~LanguageSetting();

	bool readFromStorage();
	bool saveToStorage() const;

	void DDX_Exchange(CDataExchange* pDX);
	void initControl(CWnd* parent, LocaleMngr const& locMngr) const;
	CString const& getValue() const;
protected:
	CString getAccessibleLanguage() const;
	bool isLanguageAccessible(LPCTSTR language) const;
private:
	CString strValue_;
	CString firstExistingLanguage_;
	std::list<std::basic_string<TCHAR> > dirs_;
};

#endif // !defined(AFX_LanguageSetting_H__E6876981_8AE4_4F1A_8432_E4DA79103B6F__INCLUDED_)
