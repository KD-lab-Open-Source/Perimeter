// CollectLangDirs.h: interface for the CollectLangDirs class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLLECTLANGDIRS_H__A2ED127A_4C7F_43F5_928D_646738AEEDBE__INCLUDED_)
#define AFX_COLLECTLANGDIRS_H__A2ED127A_4C7F_43F5_928D_646738AEEDBE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include <string>

class CollectLangDirs  
{
public:
	CollectLangDirs();
	~CollectLangDirs();
	bool collect(std::list<std::basic_string<TCHAR> > & dirs, LPCTSTR searchDir);
};

#endif // !defined(AFX_COLLECTLANGDIRS_H__A2ED127A_4C7F_43F5_928D_646738AEEDBE__INCLUDED_)
