// LocaleMngr.h: interface for the LocaleMngr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LocaleMngr_H__5F006B1A_30D0_406A_92F1_9F34195D8EF2__INCLUDED_)
#define AFX_LocaleMngr_H__5F006B1A_30D0_406A_92F1_9F34195D8EF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <boost/scoped_ptr.hpp>

class qdTextDB;

class LocaleMngr  
{
	LocaleMngr();
public:
	~LocaleMngr();

	static LocaleMngr& instance();

	bool reloadLocaleData(LPCTSTR language);
	CString const getText(LPCTSTR key) const;

	static CString const makeComboItemsKey(LPCTSTR postfix);
	static CString const makeControlKey(LPCTSTR postfix);
protected:
	CString const getPath(LPCTSTR language) const;
	bool reloadDB(LPCTSTR language);
private:
	boost::scoped_ptr<qdTextDB> textDB_;
};

#endif // !defined(AFX_LocaleMngr_H__5F006B1A_30D0_406A_92F1_9F34195D8EF2__INCLUDED_)
