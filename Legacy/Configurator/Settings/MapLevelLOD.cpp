// MapLevelLODSetting.cpp: implementation of the MapLevelLODSetting class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "configurator.h"
#include "MapLevelLOD.h"
#include "Localisation/LocaleMngr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const int LOW_LEVEL = 50;
const int MEDIUM_LEVEL = 100;
const int HIGH_LEVEL = 10000;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MapLevelLODSetting::MapLevelLODSetting(UINT ctrlId, UINT sectionId)
:ComboSetting(ctrlId, sectionId, IDS_KEY_MAP_LEVEL_LOD)
{

}

MapLevelLODSetting::~MapLevelLODSetting()
{

}

void MapLevelLODSetting::initControl(CWnd* parent, LocaleMngr const& locMngr) const
{
	if (getCtrlId() == (UINT)-1)
		return;

	TCHAR const* const value[] ={
		_T("Low"),
		_T("Medium"),
		_T("High")
	};
	int count = sizeof(value)/sizeof(TCHAR*);

	CComboBox* combo = reinterpret_cast<CComboBox*>(parent->GetDlgItem(getCtrlId()));
	if (!combo)
		return;

	for(int i = 0; i < count; ++i)
		combo->AddString(locMngr.getText(locMngr.makeComboItemsKey(value[i])));
}

int MapLevelLODSetting::storedToWnd(int value) const{
	switch(value) {
	case LOW_LEVEL: return 0;
	case MEDIUM_LEVEL: return 1;
	case HIGH_LEVEL: return 2;
	}
	ASSERT(0);
	return -1;
}

int MapLevelLODSetting::wndToStored(int value) const{
	switch(value) {
	case 0: return LOW_LEVEL;
	case 1: return MEDIUM_LEVEL;
	case 2: return HIGH_LEVEL;
	}
	ASSERT(0);
	return -1;
}
