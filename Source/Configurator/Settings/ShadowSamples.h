// ShadowSamples.h: interface for the ShadowSamples class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHADOWSAMPLES_H__D1D8AC15_83E8_4DE7_94FF_F768A5992948__INCLUDED_)
#define AFX_SHADOWSAMPLES_H__D1D8AC15_83E8_4DE7_94FF_F768A5992948__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RadioSetting.h"

class ShadowSamples : public RadioSetting  
{
public:
	ShadowSamples(UINT ctrlId, UINT sectionId);
	virtual ~ShadowSamples();
protected:
	virtual int wndToStored(int value) const;
	virtual int storedToWnd(int value) const;
};

#endif // !defined(AFX_SHADOWSAMPLES_H__D1D8AC15_83E8_4DE7_94FF_F768A5992948__INCLUDED_)
