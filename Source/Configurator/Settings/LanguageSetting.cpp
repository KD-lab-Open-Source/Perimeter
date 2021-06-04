// LanguageSetting.cpp: implementation of the LanguageSetting class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "configurator.h"
#include "LanguageSetting.h"

#include <atlbase.h>

#include "CollectLangDirs.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LanguageSetting::LanguageSetting(UINT ctrlID)
: SettingBase(ctrlID)
, strValue_(_T("English"))
{
	CString strIniSection;
	strIniSection.LoadString(IDS_MAIN_INI_MAIN_SECTION);
	CString strLocDir;
	strLocDir.LoadString(IDS_MAIN_INI_VALUE_LOCDIR);

	CString buffer;
	DWORD res = GetPrivateProfileString(strIniSection, strLocDir, 
										_T("Resource\\LocData"), 
										buffer.GetBufferSetLength(INI_READ_BUFFER), 
										INI_READ_BUFFER,
										mainIniFileName);
	buffer.ReleaseBuffer();
	CollectLangDirs collector;
	collector.collect(dirs_, buffer);
//	if (!dirs.empty())
//		strValue_ = dirs_.front().c_str();
	strValue_ = getAccessibleLanguage();
}

LanguageSetting::~LanguageSetting()
{

}

CString LanguageSetting::getAccessibleLanguage() const{
	CString str;
	str = _T("No languages");
	if (!dirs_.empty())
		str = dirs_.front().c_str();

	return str;
}

bool LanguageSetting::isLanguageAccessible(LPCTSTR language) const
{
	std::list<std::basic_string<TCHAR> >::const_iterator res =
		std::find(dirs_.begin(), dirs_.end(), language);
	return (res != dirs_.end());
}

CString const& LanguageSetting::getValue() const{
	return strValue_;
}

bool LanguageSetting::readFromStorage()
{
	DWORD const BUFFER_LEN = 128;

//	strValue_ = getAccessibleLanguage();
	strValue_.LoadString(IDS_LANGUAGE_DEFAULT);

	CRegKey languageKey;
	if (languageKey.Open(HKEY_CURRENT_USER,
		_T("SOFTWARE\\1C\\PerimeterET\\Intf"),KEY_READ) != ERROR_SUCCESS)
		return true;

	CString valueName;
	valueName.LoadString(IDS_VALUE_LANGUAGE);
	CString buffer;
	DWORD dwCount = BUFFER_LEN;
	if (languageKey.QueryValue(buffer.GetBufferSetLength(BUFFER_LEN),
								valueName, &dwCount) != ERROR_SUCCESS)
	{
		buffer.ReleaseBuffer();
		return true;
	}
	buffer.ReleaseBuffer(dwCount);
	if (isLanguageAccessible(buffer))
		strValue_ = buffer;

	return true;
}

bool LanguageSetting::saveToStorage() const{
	DWORD const BUFFER_LEN = 128;

	CRegKey languageKey;
	if (languageKey.Create(HKEY_CURRENT_USER,
		_T("SOFTWARE\\1C\\PerimeterET\\Intf")) != ERROR_SUCCESS)
		return false;

	CString valueName;
	valueName.LoadString(IDS_VALUE_LANGUAGE);
	CString buffer;
	DWORD dwCount = 0;
	if (languageKey.SetValue(strValue_, valueName) != ERROR_SUCCESS)
		return false;
	return true;
}

void LanguageSetting::DDX_Exchange(CDataExchange* pDX)
{
	if (getCtrlId() == (UINT)-1)
		return;

	HWND hwnd = pDX->PrepareCtrl(getCtrlId());
	ASSERT(hwnd);
	CComboBox* box = reinterpret_cast<CComboBox*>(CWnd::FromHandle(hwnd));
	if (pDX->m_bSaveAndValidate)
	{
		int curSel = box->GetCurSel();
		if (curSel != CB_ERR)
			box->GetLBText(curSel, strValue_);
		else
			strValue_ = firstExistingLanguage_;
	}
	else
	{
		if (box->SelectString(-1, strValue_) == CB_ERR)
			box->SetCurSel(-1);
	}
}

void LanguageSetting::initControl(CWnd* parent, LocaleMngr const& locMngr) const
{
	CComboBox* combo = 
		reinterpret_cast<CComboBox*>(parent->GetDlgItem(getCtrlId()));
	ASSERT(combo);

//	firstExistingLanguage_ = LoadString(IDS_LANGUAGE_DEFAULT);

	combo->ResetContent();
	std::list<std::basic_string<TCHAR> >::const_iterator b = dirs_.begin(), 
														e = dirs_.end();
	for(;b != e;++b)
		combo->AddString((*b).c_str());

}
