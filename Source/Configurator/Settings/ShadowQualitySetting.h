// ShadowQualitySetting.h: interface for the ShadowQualitySetting class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHADOWQUALITYSETTING_H__A5472206_C256_4FC7_9A94_CA89ABD4F4D4__INCLUDED_)
#define AFX_SHADOWQUALITYSETTING_H__A5472206_C256_4FC7_9A94_CA89ABD4F4D4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ComboSetting.h"

class ShadowQualitySetting : public ComboSetting  
{
public:
	ShadowQualitySetting(UINT ctrlId, UINT sectionId);
	virtual ~ShadowQualitySetting();
	virtual void initControl(CWnd* parent, LocaleMngr const& locMngr) const;
protected:
	virtual int storedToWnd(int value) const;
	virtual int wndToStored(int value) const;
};

#endif // !defined(AFX_SHADOWQUALITYSETTING_H__A5472206_C256_4FC7_9A94_CA89ABD4F4D4__INCLUDED_)
