// HTSetting.h: interface for the HTSetting class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HTSETTING_H__3A2A68B1_12D4_4E9E_B347_B5C5C5AF9818__INCLUDED_)
#define AFX_HTSETTING_H__3A2A68B1_12D4_4E9E_B347_B5C5C5AF9818__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RadioSetting.h"

class HTSetting : public RadioSetting
{
public:
	HTSetting(UINT ctrlId);
	virtual ~HTSetting();

//	virtual bool readFromStorage();
//	virtual bool saveToStorage() const;
//	virtual void DDX_Exchange(CDataExchange* pDX);
private:
//	int value_;
};

#endif // !defined(AFX_HTSETTING_H__3A2A68B1_12D4_4E9E_B347_B5C5C5AF9818__INCLUDED_)
