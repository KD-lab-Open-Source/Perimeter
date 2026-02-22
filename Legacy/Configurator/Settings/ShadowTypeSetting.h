// ShadowTypeSetting.h: interface for the ShadowTypeSetting class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHADOWTYPESETTING_H__2EAAC7EA_2107_4847_B9C5_22D17E8472C1__INCLUDED_)
#define AFX_SHADOWTYPESETTING_H__2EAAC7EA_2107_4847_B9C5_22D17E8472C1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RadioSetting.h"

class ShadowTypeSetting : public RadioSetting  
{
public:
	ShadowTypeSetting(UINT ctrlId, UINT sectionId);
	virtual ~ShadowTypeSetting();
protected:
	virtual int wndToStored(int value) const;
	virtual int storedToWnd(int value) const;
};

#endif // !defined(AFX_SHADOWTYPESETTING_H__2EAAC7EA_2107_4847_B9C5_22D17E8472C1__INCLUDED_)
