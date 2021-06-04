// LocaleMngr.cpp: implementation of the LocaleMngr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "configurator.h"
#include "LocaleMngr.h"
#include "IniReader.h"
#include "qd_textdb.h"
#include <atlbase.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LocaleMngr::LocaleMngr()
: textDB_(new qdTextDB)
{
	
}

LocaleMngr::~LocaleMngr()
{

}

LocaleMngr& LocaleMngr::instance()
{
	static LocaleMngr inst;
	return inst;
}

bool LocaleMngr::reloadLocaleData(LPCTSTR language)
{
	return reloadDB(language);
}

CString const LocaleMngr::getPath(LPCTSTR language) const
{
	CString strIniSection;
	strIniSection.LoadString(IDS_MAIN_INI_MAIN_SECTION);
	CString strLocDir;
	strLocDir.LoadString(IDS_MAIN_INI_VALUE_LOCDIR);

	CString path;
	IniReader::readString(strIniSection, strLocDir, 
							_T("Resource\\LocData"), 
							path, mainIniFileName);

	path += _T("\\");
	path += language;
	path += _T("\\");

	strLocDir.LoadString(IDS_MAIN_INI_VALUE_TEXTDIR);
	CString buffer;
	IniReader::readString(strIniSection, strLocDir, _T("Text"), buffer, mainIniFileName);

	path += buffer;
	strLocDir.LoadString(IDS_MAIN_INI_VALUE_TEXTFILE);
	IniReader::readString(strIniSection, strLocDir, 
					_T("Texts.btdb"), buffer, mainIniFileName);
	path += _T("\\");
	path += buffer;
	return path;
}

bool LocaleMngr::reloadDB(LPCTSTR language)
{
	USES_CONVERSION;
	return textDB_->load(T2CA(getPath(language)));
}

CString const LocaleMngr::getText(LPCTSTR key) const
{
	USES_CONVERSION;
	CString res(textDB_->getText(T2CA(key)));
	if (res.IsEmpty())
		res.LoadString(IDS_NO_DATA);
	return res;
}

CString const LocaleMngr::makeComboItemsKey(LPCTSTR postfix)
{
	CString prefix;
	prefix.LoadString(IDS_COMBO_ITEMS_TDB_PREFIX);
	prefix += postfix;
	return prefix;
}

CString const LocaleMngr::makeControlKey(LPCTSTR postfix)
{
	CString prefix;
	prefix.LoadString(IDS_CONTROLS_TDB_PREFIX);
	prefix += postfix;
	return prefix;
}
