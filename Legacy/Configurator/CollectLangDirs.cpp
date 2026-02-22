// CollectLangDirs.cpp: implementation of the CollectLangDirs class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Configurator.h"
#include "CollectLangDirs.h"

#include "file_enumerator.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CollectLangDirs::CollectLangDirs()
{

}

CollectLangDirs::~CollectLangDirs()
{

}

bool CollectLangDirs::collect(std::list<std::basic_string<TCHAR> > & dirs, 
							  LPCTSTR searchDir)
{
	CString s(searchDir);
	s += _T("\\*");
	file_enumerator fe(s);
	if (!fe)
		return false;

	file_enumerator::iterator b = fe.begin(), e = fe.end();
	for(; b != e; ++b){
		file_find_data const& ffd = *b;
		if (ffd.is_directory()&&!ffd.is_skipable_dir())
		{
			dirs.push_back(*b);
		}
	}
	return true;
}