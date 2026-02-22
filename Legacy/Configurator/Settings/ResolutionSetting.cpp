// ResolutionSetting.cpp: implementation of the ResolutionSetting class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "configurator.h"
#include "ResolutionSetting.h"

#include <boost/tokenizer.hpp>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

TCHAR const DEFAULT_SCR_X_SIZE [] = _T("800");
TCHAR const DEFAULT_SCR_Y_SIZE [] = _T("600");
TCHAR const GENERAL_DELIMETER[] = _T(",");

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ResolutionSetting::ResolutionSetting(UINT ctrlId)
: SettingBase(ctrlId)
{

}

ResolutionSetting::~ResolutionSetting()
{

}

//! Считать значение из его(значения) хранилища
bool ResolutionSetting::readFromStorage()
{
	TCHAR buffer[32];
	CString section;
	section.LoadString(IDS_SECTION_GRAPHICS);
	CString key;
	key.LoadString(IDS_KEY_SCR_SIZE_X);
	GetPrivateProfileString(section, key, DEFAULT_SCR_X_SIZE, buffer, 32, iniFileName);
	CString resolution(buffer);
	resolution += _T("x");
	key.LoadString(IDS_KEY_SCR_SIZE_Y);
	GetPrivateProfileString(section, key, DEFAULT_SCR_Y_SIZE, buffer, 32, iniFileName);
	resolution += buffer;
	value_ = resolution;
	return true;
}

//! Записать значение в хранилище
bool ResolutionSetting::saveToStorage() const{
	CString resolution = value_;

	int delimPos = resolution.Find(_T("x"));
	CString sizeX = resolution.Left(delimPos);
	CString sizeY = resolution.Mid(delimPos + 1);

	CString section;
	section.LoadString(IDS_SECTION_GRAPHICS);

	CString key;
	key.LoadString(IDS_KEY_SCR_SIZE_X);
	if (WritePrivateProfileString(section, key, sizeX, iniFileName) != TRUE)
		return false;

	key.LoadString(IDS_KEY_SCR_SIZE_Y);
	return (WritePrivateProfileString(section, key, sizeY, iniFileName)==TRUE);
}

//! Организуем обмент между контролом в диалоге и значением
void ResolutionSetting::DDX_Exchange(CDataExchange* pDX)
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
			box->GetLBText(curSel, value_);
		else
			value_ = _T("");
	}
	else
	{
		if (box->SelectString(-1, value_) == CB_ERR)
			box->SetCurSel(-1);
	}
}

//! Инициализация котрола относящегося к настройке(например заполнить комбобокс)
void ResolutionSetting::initControl(CWnd* parent, LocaleMngr const& locMngr) const
{
	CString strDefault;
	strDefault.LoadString(IDS_RESOLUTIONS);

	CString strIniSection;
	strIniSection.LoadString(IDS_MAIN_INI_MAIN_SECTION);
	CString strIniLanguages;
	strIniLanguages.LoadString(IDS_MAIN_INI_VALUE_RESOLUTIONS);

	CString buffer;
	DWORD res = GetPrivateProfileString(strIniSection, strIniLanguages, 
		strDefault, buffer.GetBufferSetLength(INI_READ_BUFFER), INI_READ_BUFFER,
		mainIniFileName);
	buffer.ReleaseBuffer();

	boost::char_separator<TCHAR> sep(GENERAL_DELIMETER);
	typedef boost::tokenizer<boost::char_separator<TCHAR>, 
							LPCTSTR,
							std::basic_string<TCHAR> > tokenizer;
	LPCTSTR const begin(buffer);
	LPCTSTR const end(begin + buffer.GetLength());

	CComboBox* combo = reinterpret_cast<CComboBox*>(parent->GetDlgItem(getCtrlId()));
	ASSERT(combo);
	combo->ResetContent();

	tokenizer toks(begin, end, sep);
	tokenizer::const_iterator b = toks.begin(), e = toks.end();
	for(;b != e;++b)
		combo->AddString((*b).c_str());
}
