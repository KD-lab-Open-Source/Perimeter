// MapLevelLODSetting.h: interface for the MapLevelLODSetting class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MapLevelLODSetting_H__7DB94F4C_95F4_400C_B967_C691BFB28676__INCLUDED_)
#define AFX_MapLevelLODSetting_H__7DB94F4C_95F4_400C_B967_C691BFB28676__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ComboSetting.h"

class MapLevelLODSetting : public ComboSetting  
{
public:
	MapLevelLODSetting(UINT ctrlId, UINT sectionId);
	virtual ~MapLevelLODSetting();
	virtual void initControl(CWnd* parent, LocaleMngr const& locMngr) const;
protected:
	virtual int storedToWnd(int value) const;
	virtual int wndToStored(int value) const;
};

#endif // !defined(AFX_MapLevelLODSetting_H__7DB94F4C_95F4_400C_B967_C691BFB28676__INCLUDED_)
