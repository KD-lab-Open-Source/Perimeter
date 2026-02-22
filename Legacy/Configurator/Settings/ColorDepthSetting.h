// ColorDepthSetting.h: interface for the ColorDepthSetting class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLORDEPTHSETTING_H__1AF40E6B_1F37_48D3_AB30_AF9AEE9C0075__INCLUDED_)
#define AFX_COLORDEPTHSETTING_H__1AF40E6B_1F37_48D3_AB30_AF9AEE9C0075__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SettingBase.h"

class ColorDepthSetting : public SettingBase  
{
public:
	ColorDepthSetting(UINT ctrlId);
	virtual ~ColorDepthSetting();

	virtual bool readFromStorage();
	virtual bool saveToStorage() const;
	virtual void DDX_Exchange(CDataExchange* pDX);
private:
	int storageValueToWnd(int value) const;
	int wndValueToStorage(int value) const;
private:
	int value_;
};

#endif // !defined(AFX_COLORDEPTHSETTING_H__1AF40E6B_1F37_48D3_AB30_AF9AEE9C0075__INCLUDED_)
