
#include "StdAfx.h"

#include "InterfaceLogic.h"
#include "Runtime.h"
#include "Config.h"
#include "Terra.h"
#include "UnitAttribute.h"
#include "Universe.h"
#include "GenericControls.h"

#include "GameShell.h"

#include "UniverseInterface.h"
#include "PerimeterShellUI.h"
#include "CameraManager.h"
#include "PerimeterSound.h"
#include "qd_textdb.h"

extern char _bMenuMode;

int _nActiveClusterNum = -1;

bool bNoUnitAction = false;
bool bNoTracking = false;

terUnitSquad* GetSquadByNumber(int n);

//проверка наличия прорабов/бригадиров, создание если нет
void CheckBuildTerrainUnit(terUnitAttributeID nAttrID)
{
	MTG();
	terPlayer* player = universe()->activePlayer();
	terUnitBase* frame = player->frame();
	if(!frame)
		return;

	xassert(nAttrID==UNIT_ATTRIBUTE_TERRAIN_MASTER || nAttrID==UNIT_ATTRIBUTE_BUILD_MASTER);
	int count_unit=0;
	TerrainButtonData* slots = gameShell->getLogicUpdater().getLogicData()->slots;
	for(int k=0;k<5;k++)
	{
		terUnitBase* p = slots[k].unit;
		if(p && p->attr().ID==nAttrID)
		{
			count_unit++;
		}
	}

	if(count_unit==0)
	{
		int k = 0;

		for(k=0; k<5; k++)
			if(slots[k].requestedAttr == nAttrID)
				return;

		k = 0;
		while(k < 5 && 
			(slots[k].unit || slots[k].requestedAttr >= 0)) k++;

		if(k == 5)
			k--;

		player->ClusterPick(nAttrID == UNIT_ATTRIBUTE_TERRAIN_MASTER ? COMMAND_ID_TERRAIN_MASTER_INC : COMMAND_ID_BUILD_MASTER_INC, k);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// Редактирование рабочей области

const float ToolzerCursorMax   = shellCursorAddRegionSize*6;
const float ToolzerCursorMin   = shellCursorAddRegionSize/6;
const float ToolzerCursorDelta = (ToolzerCursorMax - ToolzerCursorMin)/40;
const float RegionPathDeltaDistance = 90.0f;

bool bWasShiftUnpressed = false;

STARFORCE_API_NEW void OnToolzerSizeChange(float y);
STARFORCE_API_NEW void CancelEditWorkarea();

void OnButtonWorkArea(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	if(code == EVENT_PRESSED)
	{
		if(gameShell->BuildingInstaller.inited())
			gameShell->BuildingInstaller.CancelObject();

		CheckBuildTerrainUnit(UNIT_ATTRIBUTE_TERRAIN_MASTER);

		_pShellDispatcher->m_bCanFlip = pWnd->ID != SQSH_WORKAREA4_ID;

		int rg_wanted;
		switch(pWnd->ID)
		{
		case SQSH_WORKAREA2_ID:
			rg_wanted = editRegion2;
			break;
		case SQSH_WORKAREA4_ID:
		case SQSH_WORKAREA3_ID:
			rg_wanted = editRegion1;
			break;

		default:
			xassert(0);
		}

		bool replay = gameShell->CurrentMission.gameType_ == GT_playRellGame;

		CancelEditWorkarea();

		if(_pShellDispatcher->m_nEditRegion != rg_wanted)
		{
			MetaRegionLock lock(_pShellDispatcher->regionMetaDispatcher());
			if(rg_wanted == editRegion1)
			{
				if (!replay) {
					_shellIconManager.AddDynamicHandler(OnMouseMoveRegionEdit, CBCODE_LBDOWN);
					_shellIconManager.AddDynamicHandler(OnMouseMoveRegionEdit, CBCODE_MOUSEMOVE);
				}
				_shellCursorManager.SetActiveCursor(pWnd->ID != SQSH_WORKAREA4_ID ? CShellCursorManager::workarea_in : CShellCursorManager::workarea_out, 1);
			}
			else
			{
				if (!replay) {
					_shellIconManager.AddDynamicHandler(OnMouseMoveRegionEdit2, CBCODE_MOUSEMOVE);
				}
				_shellCursorManager.SetActiveCursor(CShellCursorManager::rov, 1);
			}

			_shellIconManager.AddDynamicHandler(OnLBUpWorkarea, CBCODE_LBUP);
			_shellIconManager.AddDynamicHandler(OnLBDownWorkarea, CBCODE_LBDOWN);
			_shellIconManager.AddDynamicHandler(OnRBDownWorkarea, CBCODE_RBDOWN);
			
			_pShellDispatcher->m_bTolzerFirstClick = true;

			_pShellDispatcher->SetEditRegion(rg_wanted);

			if(rg_wanted == editRegion1)
			{
				int r = _pShellDispatcher->regionMetaDispatcher()->getToolzerRadius()*2;
				_shellCursorManager.SetSize(r);
			}

			((CShellComplexPushButton*)pWnd)->SetCheck(true);

		}
	}
}

bool CShellIconManager::getCurrentEnabledOperation() {
	return (_pShellDispatcher->m_nEditRegion == editRegion2) || !isShiftPressed() || !controls[SQSH_WORKAREA4_ID]->isEnabled();
}

int OnLBUpWorkarea(float,float)
{
	if (gameShell->CurrentMission.gameType_ != GT_playRellGame) {
		MetaRegionLock lock(_pShellDispatcher->regionMetaDispatcher());
		_pShellDispatcher->RegionEndEdit();
	}
	return 1;
}

int OnLBDownWorkarea(float x,float y)
{
	if (gameShell->CurrentMission.gameType_ != GT_playRellGame) {
		bWasShiftUnpressed = false;

		if(_pShellDispatcher->m_nEditRegion != editRegion1)
		{
			Vect3f v;
			if(terCamera->cursorTrace(Vect2f(x - 0.5f, y - 0.5f), v))
			{
				MetaRegionLock lock(_pShellDispatcher->regionMetaDispatcher());
				float radius = 2*_pShellDispatcher->regionMetaDispatcher()->getToolzerRadius();
				v.x = clamp(v.x, radius, vMap.H_SIZE - radius - 1);
				v.y = clamp(v.y, radius, vMap.V_SIZE - radius - 1);

				_pShellDispatcher->regionMetaDispatcher()->setOperation(_shellIconManager.getCurrentEnabledOperation());

				if(_pShellDispatcher->m_bTolzerFirstClick)
				{
					_pShellDispatcher->regionMetaDispatcher()->beginLine(Vect2f(v));
					_pShellDispatcher->m_bTolzerFirstClick = false;
				}
				else
					_pShellDispatcher->regionMetaDispatcher()->lineto(Vect2f(v), 1);
			}
		}
	}
	return 1;
}

int OnRBDownWorkarea(float,float)
{
	CancelEditWorkarea();
	return 0;
}

STARFORCE_API_NEW void CancelEditWorkarea()
{
	if (!universe()) {
		return;
	}
	MetaRegionLock lock(_pShellDispatcher->regionMetaDispatcher());
	bool replay = gameShell->CurrentMission.gameType_ == GT_playRellGame;
	if (!replay) {
		_pShellDispatcher->regionMetaDispatcher()->endLine();

		_shellIconManager.DelDynamicHandler(OnMouseMoveRegionEdit, CBCODE_MOUSEMOVE);
		_shellIconManager.DelDynamicHandler(OnMouseMoveRegionEdit2, CBCODE_MOUSEMOVE);
		_pShellDispatcher->RegionEndEdit();
	}
	_shellIconManager.DelDynamicHandler(OnMouseMoveRegionEdit, CBCODE_LBDOWN);
	_shellIconManager.DelDynamicHandler(OnLBUpWorkarea, CBCODE_LBUP);
	_shellIconManager.DelDynamicHandler(OnLBDownWorkarea, CBCODE_LBDOWN);
	_shellIconManager.DelDynamicHandler(OnRBDownWorkarea, CBCODE_RBDOWN);
	_pShellDispatcher->setArrowCursor();
	_pShellDispatcher->m_nEditRegion = editRegionNone;

	((CShellComplexPushButton*)_shellIconManager.GetWnd(SQSH_WORKAREA2_ID))->SetCheck(0);
	((CShellComplexPushButton*)_shellIconManager.GetWnd(SQSH_WORKAREA3_ID))->SetCheck(0);
	((CShellComplexPushButton*)_shellIconManager.GetWnd(SQSH_WORKAREA4_ID))->SetCheck(0);
}

STARFORCE_API_NEW int OnMouseMoveRegionEdit(float x, float y)
{
	if (_pShellDispatcher->m_bCanFlip) {
		if((_shellIconManager.m_nMouseButtonsState & MK_LBUTTON) && !bWasShiftUnpressed)
		{
			Vect3f v;
			if(terCamera->cursorTrace(Vect2f(x - 0.5f, y - 0.5f), v))
			{
				MetaRegionLock lock(_pShellDispatcher->regionMetaDispatcher());
				float radius = 2*_pShellDispatcher->regionMetaDispatcher()->getToolzerRadius();
				v.x = clamp(v.x, radius, vMap.H_SIZE - radius - 1);
				v.y = clamp(v.y, radius, vMap.V_SIZE - radius - 1);

				_pShellDispatcher->regionMetaDispatcher()->setOperation(_shellIconManager.getCurrentEnabledOperation());
				_pShellDispatcher->regionMetaDispatcher()->activeLayer()->moveToolzer(Vect2f(v));
				_pShellDispatcher->regionMetaDispatcher()->operate();
			}
		}
	} else {
		if( (_shellIconManager.m_nMouseButtonsState & MK_LBUTTON) )
		{
			Vect3f v;
			if(terCamera->cursorTrace(Vect2f(x - 0.5f, y - 0.5f), v))
			{
				MetaRegionLock lock(_pShellDispatcher->regionMetaDispatcher());
				float radius = 2*_pShellDispatcher->regionMetaDispatcher()->getToolzerRadius();
				v.x = clamp(v.x, radius, vMap.H_SIZE - radius - 1);
				v.y = clamp(v.y, radius, vMap.V_SIZE - radius - 1);

				_pShellDispatcher->regionMetaDispatcher()->setOperation(false);
				_pShellDispatcher->regionMetaDispatcher()->setActiveLayer(editRegion2 - 1);
				_pShellDispatcher->regionMetaDispatcher()->activeLayer()->moveToolzer(Vect2f(v));
				_pShellDispatcher->regionMetaDispatcher()->operate();
				_pShellDispatcher->regionMetaDispatcher()->setOperation(false);
				_pShellDispatcher->regionMetaDispatcher()->setActiveLayer(editRegion1 - 1);
				_pShellDispatcher->regionMetaDispatcher()->activeLayer()->moveToolzer(Vect2f(v));
				_pShellDispatcher->regionMetaDispatcher()->operate();
			}
		}
	}

	return 1;
}

STARFORCE_API_NEW int OnMouseMoveRegionEdit2(float x, float y)
{
	if(!_pShellDispatcher->m_bTolzerFirstClick)
	{
		Vect3f v;
		if(terCamera->cursorTrace(Vect2f(x - 0.5f, y - 0.5f), v))
		{
			MetaRegionLock lock(_pShellDispatcher->regionMetaDispatcher());
			float radius = 2*_pShellDispatcher->regionMetaDispatcher()->getToolzerRadius();
			v.x = clamp(v.x, radius, vMap.H_SIZE - radius - 1);
			v.y = clamp(v.y, radius, vMap.V_SIZE - radius - 1);

			_pShellDispatcher->regionMetaDispatcher()->setOperation(_shellIconManager.getCurrentEnabledOperation());
			_pShellDispatcher->regionMetaDispatcher()->lineto(Vect2f(v), 0);
		}
	}

	return 1;
}

STARFORCE_API_NEW void OnToolzerSizeChange(float y)
{
	int s = SIGN(y);
	RegionMetaDispatcher* disp=_pShellDispatcher->regionMetaDispatcher();
	MetaRegionLock lock(disp);
	disp->changeToolzerRadius(s*DeltaToolzerRadius);
	int r = disp->getToolzerRadius()*2;

	_shellCursorManager.SetSize(r);
}

STARFORCE_API_NEW void ToolzerSizeChangeQuant()
{
//	if(_pShellDispatcher->m_nEditRegion == editRegion1 && isAltPressed())
//	{
		gameShell->MousePositionLock = 0;

		if(gameShell->MouseMoveFlag)
		{
			OnToolzerSizeChange(gameShell->mousePositionDelta().y);

			gameShell->MousePositionLock = 1;
			gameShell->setCursorPosition(Vect2f::ZERO);
			gameShell->MouseMoveFlag = 0;
		}
//	}
}
//конец редактирования рабочей области
//////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////
// CShellLogicDispatcher

void CShellLogicDispatcher::OnOverUnit(terUnitBase* p)
{
	if(bNoUnitAction)
		OnOverFriend(p);
	else
	{
		switch(p->attr().ID)
		{
		case UNIT_ATTRIBUTE_FRAME:
			OnOverFrame();
			break;

		default:
			OnOverFriend(p);
		}
	}
}

void CShellLogicDispatcher::OnOverEnemy(terUnitBase* p)
{
//	if(!_shellCursorManager.m_bCursorPermanent || m_nPickAction == SHELL_PICK_UNIT_ATTACK)
	if(m_nPickAction == SHELL_PICK_UNIT_ATTACK)
	{
		terUnitBase* pSelected = GetSelectedUnit();
		if(pSelected && !universe()->select.canSelectionAttackUnit(_pUnitHover()))
			_shellCursorManager.SetActiveCursor(CShellCursorManager::no_attack);
		else
			_shellCursorManager.SetActiveCursor(CShellCursorManager::attack);
	} else if (m_nPickAction == SHELL_PICK_UNIT_BUILD) {
		if (p->Player->isWorld()) {
			_shellCursorManager.SetActiveCursor(unitNeedRepairOrBuild(p) ? CShellCursorManager::sell : CShellCursorManager::sell_no, 1);
		} else {
			_shellCursorManager.SetActiveCursor(CShellCursorManager::sell_no, 1);
		}
	} else if (m_nPickAction == SHELL_PICK_NONE && !_shellCursorManager.m_bCursorPermanent) {
		_shellCursorManager.SetActiveCursor(CShellCursorManager::arrow_enemy);
	}
}
void CShellLogicDispatcher::OnOverFriend(terUnitBase* p)
{
	if(!_shellCursorManager.m_bCursorPermanent)
		if(m_nState != STATE_TRACKING)
		{
			if (m_nPickAction == SHELL_PICK_NONE) {
				_shellCursorManager.SetActiveCursor(CShellCursorManager::arrow);
			}
//			_shellCursorManager.SetActiveCursor(CShellCursorManager::select);
			_shellIconManager.DelDynamicHandler(0, CBCODE_LBDOWN);
		}
}

bool CShellLogicDispatcher::unitNeedRepairOrBuild(terUnitBase* p) {
	return (p->needBuildingRepair() && p->repairRequest());
//	return (p->needBuildingRepair() && (p->repairRequest() || p->repairRequested()));
}

//frame 
int OnAttachFrame(float, float)
{
	universe()->makeCommand(COMMAND_ID_FRAME_ATTACH,0);
	return 0;
}

void CShellLogicDispatcher::OnOverFrame()
{
	if(_bMenuMode)
		return;

	if(!_shellCursorManager.m_bCursorPermanent)
	{
		if (m_nPickAction == SHELL_PICK_NONE) {
			_shellCursorManager.SetActiveCursor(CShellCursorManager::arrow);
		}
/*		int nStatus = _pUnitHover->GetInterfaceStatus();

		if(nStatus & FRAME_INTERFACE_STATUS_ATTACH)
		{
		}
		else
		{
			if(safe_cast<terFrame*>(_pUnitHover)->basementReady())
			{
//				_shellCursorManager.SetActiveCursor(CShellCursorManager::core_install);
				_shellIconManager.AddDynamicHandler(OnAttachFrame, CBCODE_LBDOWN);
			}
//			else
//				_shellCursorManager.SetActiveCursor(CShellCursorManager::core_cant_install);
		}
*/
	}
}

void CShellLogicDispatcher::OnMouseIdle()
{
	if(!_shellCursorManager.m_bCursorPermanent)
	{
		_shellCursorManager.SetActiveCursor(CShellCursorManager::arrow);
		_shellIconManager.DelDynamicHandler(0, CBCODE_LBDOWN);
	}
}

STARFORCE_API_NEW void OnButtonSell(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	if(code == EVENT_PRESSED)
	{
		universe()->makeCommand(COMMAND_ID_UNINSTALL,0); // terPlantUninstall
	}
}

void CheckSelectDefaultSquad(bool bEmptyCheck = false)
{
	CUITabSheet* pSquadSheet = (CUITabSheet*)_shellIconManager.GetWnd(SQSH_TAB_SQUAD_ID);

	terUnitSquad* pSquad = GetSquadByNumber(pSquadSheet->GetActivePage());

	if(pSquad && !(bEmptyCheck && pSquad->Empty()))
		universe()->SelectSquad(pSquad);
}

inline void ChancelUnits(int nAtomID)
{
	universe()->makeCommand(isShiftPressed() ? COMMAND_ID_PRODUCTION_DEC_10 : COMMAND_ID_PRODUCTION_DEC, nAtomID);
}

STARFORCE_API_NEW void OnButtonLegion(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	int nAtomID = pWnd->ID - SQSH_SOLDIER_ID + MUTATION_ATOM_SOLDIER;

	if(code == EVENT_PRESSED)
	{
		CheckSelectDefaultSquad();

		CShellAtomButton* pBtn = (CShellAtomButton*)pWnd;
		if(pBtn->GetPause())
		{
			pBtn->Pause(false);
			universe()->makeCommand(COMMAND_ID_PRODUCTION_PAUSE_OFF, nAtomID);
		}
		else
		{
//			SND2DPlaySound("training");

			universe()->makeCommand((isShiftPressed() || param == 10) ? COMMAND_ID_PRODUCTION_INC_10 : COMMAND_ID_PRODUCTION_INC, nAtomID);
		}
	}
	else if(code == EVENT_RPRESSED)
	{
		CheckSelectDefaultSquad(true);

		CShellAtomButton* pBtn = (CShellAtomButton*)pWnd;
		if (!pBtn->GetPause()) {
			if (pBtn->CanPause()) {
				pBtn->Pause(true);
				universe()->makeCommand(COMMAND_ID_PRODUCTION_PAUSE_ON, nAtomID);
			} else {
				int nReqCount = isShiftPressed() ? 10 : 1;
				for(int i=0; i<nReqCount; i++)
					universe()->makeCommand(COMMAND_ID_BASIC_UNIT_DESTROY, nAtomID);
//				ChancelUnits(nAtomID);
			}
		} else {
			int nReqCount = isShiftPressed() ? 10 : 1;
			for(int i=0; i<nReqCount; i++)
				universe()->makeCommand(COMMAND_ID_PRODUCTION_DEC, nAtomID);
//			ChancelUnits(nAtomID);
		}
	}
}

STARFORCE_API_NEW terUnitAttributeID Button2StructureID(int nBtnID)
{
	terUnitAttributeID n_struct = UNIT_ATTRIBUTE_NONE;

	switch(nBtnID)
	{
	case SQSH_YADRO_ID:
		n_struct = UNIT_ATTRIBUTE_CORE;
		break;
	case SQSH_YADRO_EX_ID:
		n_struct = UNIT_ATTRIBUTE_COLLECTOR;
		break;
	case SQSH_SOLDIER_PLANT_ID:
		n_struct = UNIT_ATTRIBUTE_SOLDIER_PLANT;
		break;
	case SQSH_OFFICER_PLANT_ID:
		n_struct = UNIT_ATTRIBUTE_OFFICER_PLANT;
		break;
	case SQSH_TECHNIC_PLANT_ID:
		n_struct = UNIT_ATTRIBUTE_TECHNIC_PLANT;
		break;
	case SQSH_COMMANDER_ID:
		n_struct = UNIT_ATTRIBUTE_COMMANDER;
		break;
	case SQSH_RELAY_ID:
		n_struct = UNIT_ATTRIBUTE_RELAY;
		break;

	case SQSH_STATION1_ID:
		n_struct = UNIT_ATTRIBUTE_ROCKET_STATION1;
		break;
	case SQSH_STATION2_ID:
		n_struct = UNIT_ATTRIBUTE_LASER_STATION1;
		break;
	case SQSH_STATION3_ID:
		n_struct = UNIT_ATTRIBUTE_BOMB_STATION1;
		break;
	case SQSH_STATION4_ID:
		n_struct = UNIT_ATTRIBUTE_SUBTERRA_STATION1;
		break;
	case SQSH_STATION5_ID:
		n_struct = UNIT_ATTRIBUTE_FLY_STATION1;
		break;
	case SQSH_STATION_EXODUS_LAB_ID:
		n_struct = UNIT_ATTRIBUTE_EXODUS_STATION1;
		break;
	case SQSH_STATION_EMPIRE_LAB_ID:
		n_struct = UNIT_ATTRIBUTE_EMPIRE_STATION1;
		break;
	case SQSH_STATION_HARKBACK_LAB_ID:
		n_struct = UNIT_ATTRIBUTE_HARKBACK_STATION1;
		break;

	case SQSH_GUN_LASER_ID:
		n_struct = UNIT_ATTRIBUTE_LASER_CANNON;
		break;
	case SQSH_GUN_ROCKET_ID:
		n_struct = UNIT_ATTRIBUTE_ROCKET_LAUNCHER;
		break;
	case SQSH_GUN_HOWITZER_ID:
		n_struct = UNIT_ATTRIBUTE_GUN_HOWITZER;
		break;
	case SQSH_GUN_GIMLET_ID:
		n_struct = UNIT_ATTRIBUTE_GUN_SCUM_DISRUPTOR;
		break;
	case SQSH_GUN_FILTH_ID:
		n_struct = UNIT_ATTRIBUTE_GUN_FILTH_NAVIGATOR;
		break;
	case SQSH_GUN_SUBCHASER_ID:
		n_struct = UNIT_ATTRIBUTE_GUN_SUBCHASER;
		break;
	case SQSH_GUN_BALLISTIC_ID:
		n_struct = UNIT_ATTRIBUTE_GUN_BALLISTIC;
		break;

	case SQSH_SELPANEL_UPGRADE_LASER1_ID:
		n_struct = UNIT_ATTRIBUTE_LASER_STATION2;
		break;
	case SQSH_SELPANEL_UPGRADE_LASER2_ID:
		n_struct = UNIT_ATTRIBUTE_LASER_STATION3;
		break;
	case SQSH_SELPANEL_UPGRADE_BOMB1_ID:
		n_struct = UNIT_ATTRIBUTE_BOMB_STATION2;
		break;
	case SQSH_SELPANEL_UPGRADE_BOMB2_ID:
		n_struct = UNIT_ATTRIBUTE_BOMB_STATION3;
		break;
	case SQSH_SELPANEL_UPGRADE_ROCKET1_ID:
		n_struct = UNIT_ATTRIBUTE_ROCKET_STATION2;
		break;
	case SQSH_SELPANEL_UPGRADE_ROCKET2_ID:
		n_struct = UNIT_ATTRIBUTE_ROCKET_STATION3;
		break;
	case SQSH_SELPANEL_UPGRADE_FLY_ID:
		n_struct = UNIT_ATTRIBUTE_FLY_STATION2;
		break;
	case SQSH_SELPANEL_UPGRADE_OMEGA_ID:
		n_struct = UNIT_ATTRIBUTE_CORRIDOR_OMEGA;
		break;
	case SQSH_SELPANEL_UPGRADE_SUBTERRA_ID:
		n_struct = UNIT_ATTRIBUTE_SUBTERRA_STATION2;
		break;

	case SQSH_SELPANEL_UPGRADE_EXODUS1_ID:
		n_struct = UNIT_ATTRIBUTE_EXODUS_STATION2;
		break;
	case SQSH_SELPANEL_UPGRADE_EXODUS2_ID:
		n_struct = UNIT_ATTRIBUTE_EXODUS_STATION3;
		break;
	case SQSH_SELPANEL_UPGRADE_EMPIRE1_ID:
		n_struct = UNIT_ATTRIBUTE_EMPIRE_STATION2;
		break;
	case SQSH_SELPANEL_UPGRADE_EMPIRE2_ID:
		n_struct = UNIT_ATTRIBUTE_EMPIRE_STATION3;
		break;
	case SQSH_SELPANEL_UPGRADE_HARKBACK1_ID:
		n_struct = UNIT_ATTRIBUTE_HARKBACK_STATION2;
		break;
	case SQSH_SELPANEL_UPGRADE_HARKBACK2_ID:
		n_struct = UNIT_ATTRIBUTE_HARKBACK_STATION3;
		break;

	case SQSH_STATIC_BOMB_ID:
		n_struct = UNIT_ATTRIBUTE_STATIC_BOMB;
		break;

	case SQSH_CORRIDOR_ALPHA_ID:
		n_struct = UNIT_ATTRIBUTE_CORRIDOR_ALPHA;
		break;
	case SQSH_CORRIDOR_OMEGA_ID:
		n_struct = UNIT_ATTRIBUTE_CORRIDOR_OMEGA;
		break;
	}

	return n_struct;
}

int Structure2ButtonID(int i)
{
	int id = -1;

	switch(i)
	{
	case UNIT_ATTRIBUTE_CORE:
		id = SQSH_YADRO_ID;
		break;
	case UNIT_ATTRIBUTE_SOLDIER_PLANT:
		id = SQSH_SOLDIER_PLANT_ID;
		break;
	case UNIT_ATTRIBUTE_OFFICER_PLANT:
		id = SQSH_OFFICER_PLANT_ID;
		break;
	case UNIT_ATTRIBUTE_TECHNIC_PLANT:
		id = SQSH_TECHNIC_PLANT_ID;
		break;
	case UNIT_ATTRIBUTE_COLLECTOR:
		id = SQSH_YADRO_EX_ID;
		break;
	case UNIT_ATTRIBUTE_COMMANDER:
		id = SQSH_COMMANDER_ID;
		break;
	case UNIT_ATTRIBUTE_RELAY:
		id = SQSH_RELAY_ID;
		break;

	case UNIT_ATTRIBUTE_ROCKET_STATION1:
	//case UNIT_ATTRIBUTE_ROCKET_STATION2:
	//case UNIT_ATTRIBUTE_ROCKET_STATION3:
		id = SQSH_STATION1_ID;
		break;
	case UNIT_ATTRIBUTE_LASER_STATION1:
//	case UNIT_ATTRIBUTE_LASER_STATION2:
	//case UNIT_ATTRIBUTE_LASER_STATION3:
		id = SQSH_STATION2_ID;
		break;
	case UNIT_ATTRIBUTE_BOMB_STATION1:
	//case UNIT_ATTRIBUTE_BOMB_STATION2:
	//case UNIT_ATTRIBUTE_BOMB_STATION3:
		id = SQSH_STATION3_ID;
		break;

	case UNIT_ATTRIBUTE_EXODUS_STATION1:
//	case UNIT_ATTRIBUTE_EXODUS_STATION2:
//	case UNIT_ATTRIBUTE_EXODUS_STATION3:
		id = SQSH_STATION_EXODUS_LAB_ID;
		break;
	case UNIT_ATTRIBUTE_EMPIRE_STATION1:
//	case UNIT_ATTRIBUTE_EMPIRE_STATION2:
//	case UNIT_ATTRIBUTE_EMPIRE_STATION3:
		id = SQSH_STATION_EMPIRE_LAB_ID;
		break;
	case UNIT_ATTRIBUTE_HARKBACK_STATION1:
//	case UNIT_ATTRIBUTE_HARKBACK_STATION2:
//	case UNIT_ATTRIBUTE_HARKBACK_STATION3:
		id = SQSH_STATION_HARKBACK_LAB_ID;
		break;
	case UNIT_ATTRIBUTE_FLY_STATION1:
		id = SQSH_STATION5_ID;
		break;

	case UNIT_ATTRIBUTE_SUBTERRA_STATION1:
		id = SQSH_STATION4_ID;
		break;
		   
	case UNIT_ATTRIBUTE_LASER_CANNON:
		id = SQSH_GUN_LASER_ID;
		break;
	case UNIT_ATTRIBUTE_ROCKET_LAUNCHER:
		id = SQSH_GUN_ROCKET_ID;
		break;
	case UNIT_ATTRIBUTE_GUN_HOWITZER:
		id = SQSH_GUN_HOWITZER_ID;
		break;
	case UNIT_ATTRIBUTE_GUN_SCUM_DISRUPTOR:
		id = SQSH_GUN_GIMLET_ID;
		break;
	case UNIT_ATTRIBUTE_GUN_FILTH_NAVIGATOR:
		id = SQSH_GUN_FILTH_ID;
		break;
	case UNIT_ATTRIBUTE_GUN_SUBCHASER:
		id = SQSH_GUN_SUBCHASER_ID;
		break;
	case UNIT_ATTRIBUTE_GUN_BALLISTIC:
		id = SQSH_GUN_BALLISTIC_ID;
		break;
	case UNIT_ATTRIBUTE_STATIC_BOMB:
		id = SQSH_STATIC_BOMB_ID;
		break;
	case UNIT_ATTRIBUTE_CORRIDOR_ALPHA:
		id = SQSH_CORRIDOR_ALPHA_ID;
		break;
	case UNIT_ATTRIBUTE_CORRIDOR_OMEGA:
		id = SQSH_CORRIDOR_OMEGA_ID;
		break;
	}

	return id;
}
int Structure2ButtonID_(int i)// только для подсказок по строениям
{
	int id = -1;

	switch(i)
	{
	case UNIT_ATTRIBUTE_CORE:
		id = SQSH_YADRO_ID;
		break;
	case UNIT_ATTRIBUTE_SOLDIER_PLANT:
		id = SQSH_SOLDIER_PLANT_ID;
		break;
	case UNIT_ATTRIBUTE_OFFICER_PLANT:
		id = SQSH_OFFICER_PLANT_ID;
		break;
	case UNIT_ATTRIBUTE_TECHNIC_PLANT:
		id = SQSH_TECHNIC_PLANT_ID;
		break;
	case UNIT_ATTRIBUTE_COLLECTOR:
		id = SQSH_YADRO_EX_ID;
		break;
	case UNIT_ATTRIBUTE_COMMANDER:
		id = SQSH_COMMANDER_ID;
		break;
	case UNIT_ATTRIBUTE_RELAY:
		id = SQSH_RELAY_ID;
		break;

	case UNIT_ATTRIBUTE_ROCKET_STATION1:
	case UNIT_ATTRIBUTE_ROCKET_STATION2:
	case UNIT_ATTRIBUTE_ROCKET_STATION3:
		id = SQSH_STATION1_ID;
		break;
	case UNIT_ATTRIBUTE_LASER_STATION1:
	case UNIT_ATTRIBUTE_LASER_STATION2:
	case UNIT_ATTRIBUTE_LASER_STATION3:
		id = SQSH_STATION2_ID;
		break;
	case UNIT_ATTRIBUTE_BOMB_STATION1:
	case UNIT_ATTRIBUTE_BOMB_STATION2:
	case UNIT_ATTRIBUTE_BOMB_STATION3:
		id = SQSH_STATION3_ID;
		break;
	case UNIT_ATTRIBUTE_FLY_STATION1:
	case UNIT_ATTRIBUTE_FLY_STATION2:
		id = SQSH_STATION5_ID;
		break;

	case UNIT_ATTRIBUTE_SUBTERRA_STATION1:
	case UNIT_ATTRIBUTE_SUBTERRA_STATION2:
		id = SQSH_STATION4_ID;
		break;

	case UNIT_ATTRIBUTE_EXODUS_STATION1:
	case UNIT_ATTRIBUTE_EXODUS_STATION2:
	case UNIT_ATTRIBUTE_EXODUS_STATION3:
		id = SQSH_STATION_EXODUS_LAB_ID;
		break;
	case UNIT_ATTRIBUTE_EMPIRE_STATION1:
	case UNIT_ATTRIBUTE_EMPIRE_STATION2:
	case UNIT_ATTRIBUTE_EMPIRE_STATION3:
		id = SQSH_STATION_EMPIRE_LAB_ID;
		break;
	case UNIT_ATTRIBUTE_HARKBACK_STATION1:
	case UNIT_ATTRIBUTE_HARKBACK_STATION2:
	case UNIT_ATTRIBUTE_HARKBACK_STATION3:
		id = SQSH_STATION_HARKBACK_LAB_ID;
		break;
		   
	case UNIT_ATTRIBUTE_LASER_CANNON:
		id = SQSH_GUN_LASER_ID;
		break;
	case UNIT_ATTRIBUTE_ROCKET_LAUNCHER:
		id = SQSH_GUN_ROCKET_ID;
		break;
	case UNIT_ATTRIBUTE_GUN_HOWITZER:
		id = SQSH_GUN_HOWITZER_ID;
		break;

	case UNIT_ATTRIBUTE_GUN_SCUM_DISRUPTOR:
		id = SQSH_GUN_GIMLET_ID;
		break;
	case UNIT_ATTRIBUTE_GUN_FILTH_NAVIGATOR:
		id = SQSH_GUN_FILTH_ID;
		break;

	case UNIT_ATTRIBUTE_GUN_SUBCHASER:
		id = SQSH_GUN_SUBCHASER_ID;
		break;

	case UNIT_ATTRIBUTE_GUN_BALLISTIC:
		id = SQSH_GUN_BALLISTIC_ID;
		break;

	case UNIT_ATTRIBUTE_STATIC_BOMB:
		id = SQSH_STATIC_BOMB_ID;
		break;

	case UNIT_ATTRIBUTE_CORRIDOR_ALPHA:
		id = SQSH_CORRIDOR_ALPHA_ID;
		break;
	case UNIT_ATTRIBUTE_CORRIDOR_OMEGA:
		id = SQSH_CORRIDOR_OMEGA_ID;
		break;
	}

	return id;
}

STARFORCE_API_NEW terUnitAttributeID Button2LegionID(int id)
{
	terUnitAttributeID n = UNIT_ATTRIBUTE_NONE;
	switch(id)
	{
	case SQSH_SOLDIER_ID:
		n = UNIT_ATTRIBUTE_SOLDIER;
		break;
	case SQSH_OFFICER_ID:
		n = UNIT_ATTRIBUTE_OFFICER;
		break;
	case SQSH_TECHNIC_ID:
		n = UNIT_ATTRIBUTE_TECHNIC;
		break;

	case SQSH_SQUAD_UNIT1:
		n = UNIT_ATTRIBUTE_ROCKER;
		break;
	case SQSH_SQUAD_UNIT2:
		n = UNIT_ATTRIBUTE_SNIPER;
		break;
	case SQSH_SQUAD_UNIT3:
		n = UNIT_ATTRIBUTE_MORTAR;
		break;
	case SQSH_SQUAD_UNIT4:
		n = UNIT_ATTRIBUTE_DIGGER;
		break;
	case SQSH_SQUAD_UNIT5:
		n = UNIT_ATTRIBUTE_LEECH;
		break;
	case SQSH_SQUAD_UNIT6:
		n = UNIT_ATTRIBUTE_R_PROJECTOR;
		break;
	case SQSH_SQUAD_UNIT7:
		n = UNIT_ATTRIBUTE_SCUMER;
		break;
	case SQSH_SQUAD_UNIT8:
		n = UNIT_ATTRIBUTE_LEAMO;
		break;
	case SQSH_SQUAD_UNIT9:
		n = UNIT_ATTRIBUTE_SCUM_SPLITTER;
		break;
	case SQSH_SQUAD_UNIT10:
		n = UNIT_ATTRIBUTE_PIERCER;
		break;
	case SQSH_SQUAD_UNIT11:
		n = UNIT_ATTRIBUTE_CEPTOR;
		break;
	case SQSH_SQUAD_UNIT12:
		n = UNIT_ATTRIBUTE_GYROID;
		break;
	case SQSH_SQUAD_UNIT13:
		n = UNIT_ATTRIBUTE_BOMBER;
		break;
	case SQSH_SQUAD_UNIT14:
		n = UNIT_ATTRIBUTE_STRAFER;
		break;
	case SQSH_SQUAD_UNIT15:
		n = UNIT_ATTRIBUTE_UNSEEN;
		break;
	case SQSH_SQUAD_UNIT16:
		n = UNIT_ATTRIBUTE_EXTIRPATOR;
		break;
	case SQSH_SQUAD_UNIT17:
		n = UNIT_ATTRIBUTE_WARGON;
		break;
	case SQSH_SQUAD_UNIT18:
		n = UNIT_ATTRIBUTE_DISINTEGRATOR;
		break;
	case SQSH_SQUAD_UNIT19:
		n = UNIT_ATTRIBUTE_SCUM_THROWER;
		break;
	case SQSH_SQUAD_UNIT20:
		n = UNIT_ATTRIBUTE_FILTH_SPOT0;
		break;
	case SQSH_SQUAD_UNIT21:
		n = UNIT_ATTRIBUTE_FILTH_SPOT1;
//		n = UNIT_ATTRIBUTE_IMPALER;
		break;
	case SQSH_SQUAD_UNIT22:
		n = UNIT_ATTRIBUTE_FILTH_SPOT2;
//		n = UNIT_ATTRIBUTE_EFLAIR;
		break;
	case SQSH_SQUAD_UNIT23:
		n = UNIT_ATTRIBUTE_FILTH_SPOT3;
//		n = UNIT_ATTRIBUTE_CONDUCTOR;
		break;
	case SQSH_SQUAD_UNIT24:
		n = UNIT_ATTRIBUTE_SCUM_TWISTER;
		break;
	case SQSH_SQUAD_UNIT25:
		n = UNIT_ATTRIBUTE_SCUM_HEATER;
		break;
	}  

	return n;
}
int LegionID2Button(int nAttrID)
{
	int nButtonID = -1;
	switch(nAttrID)
	{
	case UNIT_ATTRIBUTE_ROCKER:
		nButtonID = SQSH_SQUAD_UNIT1;
		break;
	case UNIT_ATTRIBUTE_SNIPER:
		nButtonID = SQSH_SQUAD_UNIT2;
		break;
	case UNIT_ATTRIBUTE_MORTAR:
		nButtonID = SQSH_SQUAD_UNIT3;
		break;
	case UNIT_ATTRIBUTE_DIGGER:
		nButtonID = SQSH_SQUAD_UNIT4;
		break;
	case UNIT_ATTRIBUTE_LEECH:
		nButtonID = SQSH_SQUAD_UNIT5;
		break;
	case UNIT_ATTRIBUTE_R_PROJECTOR:
		nButtonID = SQSH_SQUAD_UNIT6;
		break;
	case UNIT_ATTRIBUTE_SCUMER:
		nButtonID = SQSH_SQUAD_UNIT7;
		break;
	case UNIT_ATTRIBUTE_LEAMO:
		nButtonID = SQSH_SQUAD_UNIT8;
		break;
	case UNIT_ATTRIBUTE_SCUM_SPLITTER:
		nButtonID = SQSH_SQUAD_UNIT9;
		break;
	case UNIT_ATTRIBUTE_PIERCER:
		nButtonID = SQSH_SQUAD_UNIT10;
		break;
	case UNIT_ATTRIBUTE_CEPTOR:
		nButtonID = SQSH_SQUAD_UNIT11;
		break;
	case UNIT_ATTRIBUTE_GYROID:
		nButtonID = SQSH_SQUAD_UNIT12;
		break;
	case UNIT_ATTRIBUTE_BOMBER:
		nButtonID = SQSH_SQUAD_UNIT13;
		break;
	case UNIT_ATTRIBUTE_STRAFER:
		nButtonID = SQSH_SQUAD_UNIT14;
		break;
	case UNIT_ATTRIBUTE_UNSEEN:
		nButtonID = SQSH_SQUAD_UNIT15;
		break;
	case UNIT_ATTRIBUTE_EXTIRPATOR:
		nButtonID = SQSH_SQUAD_UNIT16;
		break;
	case UNIT_ATTRIBUTE_WARGON:
		nButtonID = SQSH_SQUAD_UNIT17;
		break;
	case UNIT_ATTRIBUTE_DISINTEGRATOR:
		nButtonID = SQSH_SQUAD_UNIT18;
		break;
	case UNIT_ATTRIBUTE_SCUM_THROWER:
		nButtonID = SQSH_SQUAD_UNIT19;
		break;
	case UNIT_ATTRIBUTE_FILTH_SPOT0:
		nButtonID = SQSH_SQUAD_UNIT20;
		break;
	case UNIT_ATTRIBUTE_FILTH_SPOT1:
//	case UNIT_ATTRIBUTE_IMPALER:
		nButtonID = SQSH_SQUAD_UNIT21;
		break;
	case UNIT_ATTRIBUTE_FILTH_SPOT2:
//	case UNIT_ATTRIBUTE_EFLAIR:
		nButtonID = SQSH_SQUAD_UNIT22;
		break;
	case UNIT_ATTRIBUTE_FILTH_SPOT3:
//	case UNIT_ATTRIBUTE_CONDUCTOR:
		nButtonID = SQSH_SQUAD_UNIT23;
		break;
	case UNIT_ATTRIBUTE_SCUM_TWISTER:
		nButtonID = SQSH_SQUAD_UNIT24;
		break;
	case UNIT_ATTRIBUTE_SCUM_HEATER:
		nButtonID = SQSH_SQUAD_UNIT25;
		break;
	}

	return nButtonID;
}

void OnButtonStructure(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	if(code == EVENT_PRESSED)
	{			 
		static int logicQuant;
		if (gameShell->isPaused() || logicQuant == universe()->quantCounter()) {
			return;
		}
		logicQuant = universe()->quantCounter();

		CShellComplexPushButton* pBtn = (CShellComplexPushButton*)pWnd;
		if(pBtn->GetStatusNoEnergy())
		{

			CInfoWindow* pWndInfo;
			if(pWndInfo = (CInfoWindow*)_shellIconManager.GetWnd(SQSH_INFOWND_ID))
			{
				pWndInfo->Show(true);
				pWndInfo->SetText(qdTextDB::instance().getText("Interface.Tips.no_energy_for_build")); 
				pWndInfo->SetTime(3000);
//				pWndInfo->Centered();

				//universe()->activePlayer()->soundEvent(SOUND_VOICE_ENERGY_NOT_ENOUGH);
			}
			return;
		}

		terUnitAttributeID n_struct = Button2StructureID(pWnd->ID);
		if(n_struct != UNIT_ATTRIBUTE_NONE)
		{
			universe()->DeselectAll();
			CancelEditWorkarea();

			CheckBuildTerrainUnit(UNIT_ATTRIBUTE_BUILD_MASTER);

			gameShell->BuildingInstaller.InitObject(universe()->activePlayer()->unitAttribute(n_struct));
			gameShell->BuildingInstaller.SetBuildPosition(gameShell->mousePosition(), universe()->activePlayer());
		}
	} else if (code == EVENT_PRESSED_DISABLED) {
		CShellComplexPushButton* pBtn = (CShellComplexPushButton*)pWnd;
		if (pBtn->GetPause()) {
//			pBtn->Pause(false);
			terUnitAttributeID n_struct = Button2StructureID(pWnd->ID);
			if (n_struct != UNIT_ATTRIBUTE_NONE) {
				terBuilding* unit=_shellIconManager.GetProgress(n_struct).unit;
				if(unit)
					unit->commandOutcoming(UnitCommand(COMMAND_ID_CONTINUE_CONSTRUCTION, 0, COMMAND_SELECTED_MODE_NEGATIVE));
			}
		}
	} else if (code == EVENT_RPRESSED) {
		CShellComplexPushButton* pBtn = (CShellComplexPushButton*)pWnd;
		if (!pBtn->GetPause() && !pBtn->isEnabled()) {
//			pBtn->Pause(true);
			terUnitAttributeID n_struct = Button2StructureID(pWnd->ID);
			if (n_struct != UNIT_ATTRIBUTE_NONE) {
				terBuilding* unit=_shellIconManager.GetProgress(n_struct).unit;
				if(unit)
					unit->commandOutcoming(UnitCommand(COMMAND_ID_HOLD_CONSTRUCTION, 0, COMMAND_SELECTED_MODE_NEGATIVE));
			}
		}
	} else if (code == EVENT_DOUBLECLICK) {
		CShellComplexPushButton* pBtn = (CShellComplexPushButton*)pWnd;
		if (!pBtn->isEnabled()) {
			terUnitAttributeID n_struct = Button2StructureID(pWnd->ID);
			if (n_struct != UNIT_ATTRIBUTE_NONE) {
				terBuilding* unit=_shellIconManager.GetProgress(n_struct).unit;
				if(unit){
					terCamera->setCoordinate(CameraCoordinate(unit->position2D(), terCamera->coordinate().psi(), terCamera->coordinate().theta(), terCamera->coordinate().distance()));
				}
			}
		}
	}
}


void OnButtonFieldOn(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	static char _cb[100];

	CShellComplexPushButton* pBtn = (CShellComplexPushButton*)pWnd;

	if(code == EVENT_PRESSED){
		universe()->activePlayer()->ClusterPick(COMMAND_ID_FIELD_START, 0);
	}
//	else if(code == EVENT_PRESSED_DISABLED && !pBtn->GetCheck()){
//		terPlayer* player = universe()->activePlayer();
//		terUnitBase* pFrame = player->frame();
//		if(pFrame)
//		{
//			CInfoWindow* pWndInfo;
//			if(pWndInfo = (CInfoWindow*)_shellIconManager.GetWnd(SQSH_INFOWND_ID))
//			{
//				pWndInfo->Show(true);
//
//				if(!player->countBuildingsConstructed(UNIT_ATTRIBUTE_CORE))
//					pWndInfo->SetText(_shellIconManager.FormatMessageText("<no_field_generators>", _cb));
//
//				pWndInfo->SetTime(3000);
//				pWndInfo->Centered();
//			}
//		}
//	}
//	else if((code == EVENT_ON_WINDOW) && ((pWnd->state & SQSH_ENABLED) || pBtn->GetCheck()))
//	{
//		CInfoWindow* pWndInfo;
//		if(pWndInfo = (CInfoWindow*)_shellIconManager.GetWnd(SQSH_INFOWND_ID))
//		{
//			pWndInfo->Show(param);
//
//			//int nReadyProtectorCount = _pShellDispatcher->GetUnitsReadyCount(UNIT_ATTRIBUTE_PROTECTOR);
//			float f = 0;//terFieldUseEnergy*nReadyProtectorCount;
//			pWndInfo->SetText(_shellIconManager.FormatMessageText("<need_field_energy>", _cb, 0));
//		}
//	}
}
void OnButtonFieldOff(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	if(code == EVENT_PRESSED)
	{
		universe()->activePlayer()->ClusterPick(COMMAND_ID_FIELD_STOP, 0);

//		SND2DPlaySound("shield_off");
	}
}

void OnButtonDisintegrate(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	if(code == EVENT_PRESSED)
	{
		CheckSelectDefaultSquad();
		universe()->makeCommand(COMMAND_ID_UNIT_MORPHING, UNIT_ATTRIBUTE_NONE);
	}
}

void OnButtonTogether(CShellWindow* pWnd, InterfaceEventCode code, int param) {
//	joinSelectedToSquad( GetSquadByNumber(((CUITabSheet*)GetWnd(SQSH_TAB_SQUAD_ID))->GetActivePage()) );
	if (code == EVENT_PRESSED) {
		CUITabSheet* pSquadSheet = (CUITabSheet*)_shellIconManager.GetWnd(SQSH_TAB_SQUAD_ID);
		int nActivePage = pSquadSheet->GetActivePage();
		terUnitSquad* pSquadSelected = GetSquadByNumber(nActivePage);

		CSELECT_AUTOLOCK();
		const UnitList& selList=universe()->select.GetSelectList();
		UnitList::const_iterator selIt;
		for (selIt = selList.begin(); selIt != selList.end(); selIt++) {
			if ((*selIt) != pSquadSelected && !((terUnitSquad*)(*selIt))->Empty() && ((terUnitSquad*)(*selIt))->isBase()) {
				pSquadSelected->commandOutcoming(UnitCommand(COMMAND_ID_ADD_SQUAD, *selIt, COMMAND_SELECTED_MODE_SINGLE));
			}
		}
	}
}

void OnButtonSquadMutate(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	if(code == EVENT_PRESSED)
	{
		CShellLegionButton* pBtn = (CShellLegionButton*)pWnd;
		if(pBtn->GetStatusNoEnergy())
		{
			CInfoWindow* pWndInfo;
			if(pWndInfo = (CInfoWindow*)_shellIconManager.GetWnd(SQSH_INFOWND_ID))
			{
				pWndInfo->Show(true);
				pWndInfo->SetText(qdTextDB::instance().getText("Interface.Tips.no_mutation_energy")); 
				pWndInfo->SetTime(3000);
//				pWndInfo->Centered();
			}

			return;
		}
		else if(!(pBtn->m_bS && pBtn->m_bO && pBtn->m_bT))
		{
			CInfoWindow* pWndInfo;
			if(pWndInfo = (CInfoWindow*)_shellIconManager.GetWnd(SQSH_INFOWND_ID))
			{
				pWndInfo->Show(true);
				pWndInfo->SetText(qdTextDB::instance().getText("Interface.Tips.no_basic_units")); 
				pWndInfo->SetTime(3000);
//				pWndInfo->Centered();
			}

			return;
		}

		CheckSelectDefaultSquad();

		terUnitAttributeID attrID = Button2LegionID(pWnd->ID);

		if(attrID >= 0){
			const AttributeBase* attribute = universe()->activePlayer()->unitAttribute(attrID);
			if(attribute->isLegionary())
				universe()->makeCommand(COMMAND_ID_UNIT_MORPHING, attribute->ID);
		}
	}
}

void OnButtonOffensiveDefensive(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	if(code == EVENT_PRESSED)
	{
		bool off = ((CShellComplexPushButton*)pWnd)->GetCheck();
		universe()->makeCommand(COMMAND_ID_UNIT_OFFENSIVE, off ? 0 : 1);
	}
}

void OnMapWindowClicked(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	if( !_bMenuMode && !gameShell->isScriptReelEnabled() && gameShell->mouseLeftPressed() )
	{
		float x = (gameShell->mousePosition().x + 0.5f)*terRenderDevice->GetSizeX() - pWnd->x;
		float y = (gameShell->mousePosition().y + 0.5f)*terRenderDevice->GetSizeY() - pWnd->y;

		x *= vMap.H_SIZE / pWnd->sx;
		y *= vMap.V_SIZE / pWnd->sy;

		if(x > 0 && y > 0 && x < vMap.H_SIZE && y < vMap.V_SIZE)
			terCamera->setCoordinate(CameraCoordinate(Vect2f(x, y), terCamera->coordinate().psi(), terCamera->coordinate().theta(), terCamera->coordinate().distance()));
	}
	if (code == EVENT_RPRESSED) {
		if (!universe()) {
			return;
		}

		terPlayer* pPlayer = universe()->activePlayer();
		if (!pPlayer) {
			return;
		}
		float x = (gameShell->mousePosition().x + 0.5f)*terRenderDevice->GetSizeX() - pWnd->x;
		float y = (gameShell->mousePosition().y + 0.5f)*terRenderDevice->GetSizeY() - pWnd->y;

		x *= vMap.H_SIZE / pWnd->sx;
		y *= vMap.V_SIZE / pWnd->sy;

		if (x > 0 && y > 0 && x < vMap.H_SIZE && y < vMap.V_SIZE && gameShell->m_ShellDispatcher.GetSelectedUnit() && !gameShell->m_ShellDispatcher.GetSelectedUnit()->isBuilding()) {
			Vect3f v(x, y, 0);
			universe()->makeCommandSubtle(COMMAND_ID_POINT, v, COMMAND_SELECTED_MODE_SINGLE);
		}
	}
}

void OnButtonGotoBase(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	if(code == EVENT_PRESSED)
		universe()->makeCommand(COMMAND_ID_RETURN_TO_BASE, 0, COMMAND_SELECTED_MODE_SINGLE);
}

static CShellWindow* _pBtnLastRequest = 0;
int OnBuildClick(float x, float y)
{
/*	if(_pBtnLastRequest)
	{
		FRAME_TERRAIN_BUILD_DATA* pInfo = &_shellIconManager.m_BrigadiersInfo[_pBtnLastRequest->ID - SQSH_FRAME_TERRAIN_BUILD1_ID];

		if(pInfo->pUnit == 0)
		{
			Vect3f v;
			if(terCamera->cursorTrace(Vect2f(x - 0.5f, y - 0.5f), v))
				universe()->activePlayer()->ClusterActionPoint(v, COMMAND_SELECTED_MODE_NEGATIVE,COMMAND_ID_UNIT_PRODUCTION_MOVE, 0);
		}

		_pBtnLastRequest = 0;
	}
*/
	return 0;
}
void OnButtonTerrainBuild(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	if(code == EVENT_PRESSED)
	{
		int nSlot = pWnd->ID - SQSH_FRAME_TERRAIN_BUILD1_ID;
		TerrainButtonData* slotData = &(gameShell->getLogicUpdater().getLogicData()->slots[nSlot]);

		if(!slotData->unit)
		{
			float x = pWnd->x/(float)terRenderDevice->GetSizeX();
			float sx = pWnd->sx/(float)terRenderDevice->GetSizeX();

			CTerrainBuildButton* btn = (CTerrainBuildButton*)pWnd;
			bool bTerrain = _pShellDispatcher->m_fMouseCurrentX > x && 
				_pShellDispatcher->m_fMouseCurrentX < x + sx/2;
			if ((btn->partDisable == 1 && bTerrain) || (btn->partDisable == 2 && !bTerrain)) {
				bTerrain = !bTerrain;
//				return;
			}
			if(universe()->activePlayer())
				universe()->activePlayer()->ClusterPick(bTerrain ? COMMAND_ID_TERRAIN_MASTER_INC : COMMAND_ID_BUILD_MASTER_INC, nSlot);

			_pBtnLastRequest = pWnd;
//			_shellIconManager.AddDynamicHandler(OnBuildClick, CBCODE_RBDOWN);

//			if(slotData->productionPhase == 0)
//				SND2DPlaySound("development");
		}
		else {
//			HT-SELECT!!!
			if (mt_interface_quant) {
				TerrainButtonData* slotData = &(gameShell->getLogicUpdater().getLogicData()->slots[nSlot]);
				universe()->select.unitToSelection(slotData->unit, isShiftPressed() ? COMMAND_SELECTED_MODE_NEGATIVE : COMMAND_SELECTED_MODE_SINGLE);
			}
		}
	}
	else if(code == EVENT_DOUBLECLICK)
	{
		int nSlot = pWnd->ID - SQSH_FRAME_TERRAIN_BUILD1_ID;
//		HT-SELECT!!!
		if (mt_interface_quant) {
			TerrainButtonData* slotData = &(gameShell->getLogicUpdater().getLogicData()->slots[nSlot]);
			if(slotData->unit)
				terCamera->setCoordinate(CameraCoordinate(slotData->unit->position2D(), terCamera->coordinate().psi(), terCamera->coordinate().theta(), terCamera->coordinate().distance()));
		}
	}
}

void OnButtonSpeed(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	if(code == EVENT_PRESSED)
	{
		if (pWnd->ID == SQSH_SPEED_PAUSE) {
			gameShell->pauseGame();
		} else if (pWnd->ID == SQSH_SPEED_50) {
			gameShell->setSpeed(0.5f);
		} else if (pWnd->ID == SQSH_SPEED_150) {
			gameShell->setSpeed(2.0f);
		} else {
			gameShell->setSpeed(1.0f);
		}
	}
}

terUnitSquad* GetSquadByNumber(int n)
{
	return (0 <= n && n < 5) ? gameShell->getLogicUpdater().getLogicData()->squads[n].squad : 0;
}

void OnSquadTabEvent(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	terUnitSquad* pSquad = GetSquadByNumber(param);
	if(!pSquad)
		return;

	if(code == EVENT_TAB_CHANGED)
	{
		LogicData* logicData = gameShell->getLogicUpdater().getLogicData();
		xassert(param>=0 && param<5);
		SquadPageData& squad_data=logicData->squads[param];
		
		if (squad_data.enabled) {
			universe()->select.unitToSelection(pSquad, isShiftPressed() ? COMMAND_SELECTED_MODE_NEGATIVE : COMMAND_SELECTED_MODE_SINGLE, true);
//			universe()->DeselectAll();
	
//			universe()->SelectSquad(pSquad);
			pSquad->soundEvent(SOUND_VOICE_SQUAD_SELECTED);
		}
	}
	else if(code == EVENT_DOUBLECLICK)
	{
		if(!pSquad->Empty())
			terCamera->setCoordinate(CameraCoordinate(pSquad->position2D(), terCamera->coordinate().psi(), terCamera->coordinate().theta(), terCamera->coordinate().distance()));
	}
	else if(code == EVENT_RPRESSED)
	{
		CSELECT_AUTOLOCK();
		const UnitList& selList=universe()->select.GetSelectList();
		if (selList.empty() || !(selList.front()->attr().ID == UNIT_ATTRIBUTE_SQUAD)) {
			return;
		}
		UnitList::const_iterator selIt;
		for (selIt = selList.begin(); selIt != selList.end(); selIt++) {
			if ((*selIt) != pSquad) {
				(*selIt)->commandOutcoming(UnitCommand(COMMAND_ID_FOLLOW_SQUAD, pSquad, 0, COMMAND_SELECTED_MODE_SINGLE));
			}
		}
	}
}

void OnFrameTabEvent(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	if(code == EVENT_DOUBLECLICK)
	{
		terUnitBase* pFrame = universe()->activePlayer()->frame();
		if(pFrame)
			terCamera->setCoordinate(CameraCoordinate(pFrame->position2D(), terCamera->coordinate().psi(), terCamera->coordinate().theta(), terCamera->coordinate().distance()));
	}
}

void EnterInMissionMenu()
{
	gameShell->prepareForInGameMenu();

	_shellIconManager.GetWnd(SQSH_MM_INMISSION_SAVE_BTN)->Enable(
			gameShell->currentSingleProfile.getLastGameType() != UserSingleProfile::LAN
		&&	gameShell->currentSingleProfile.getLastGameType() != UserSingleProfile::REPLAY );

	_shellIconManager.GetWnd(SQSH_MM_INMISSION_LOAD_BTN)->Enable(
			gameShell->currentSingleProfile.getLastGameType() != UserSingleProfile::LAN );


	_shellIconManager.GetWnd(SQSH_MM_INMISSION_RESTART_BTN)->Enable(
			gameShell->currentSingleProfile.getLastGameType() != UserSingleProfile::LAN );


#ifdef _DEMO_
	_shellIconManager.GetWnd(SQSH_MM_INMISSION_SAVE_BTN)->Enable(false);
	_shellIconManager.GetWnd(SQSH_MM_INMISSION_LOAD_BTN)->Enable(false);
#endif

	_shellIconManager.SwitchMenuScreens(-1, SQSH_MM_INMISSION_SCR);
}
void OnButtonMenu(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	if(code == EVENT_UNPRESSED)
		EnterInMissionMenu();
}
void OnButtonTarget(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	if(code == EVENT_PRESSED)
	{
		_pShellDispatcher->m_nPickAction = SHELL_PICK_UNIT_TARGET;
		_shellCursorManager.SetActiveCursor(CShellCursorManager::move, 1);
	}
}
void OnButtonAttack(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	if(code == EVENT_PRESSED)
	{
		_pShellDispatcher->m_nPickAction = SHELL_PICK_UNIT_ATTACK;
		_pShellDispatcher->m_nPickData = 0;

		terUnitBase* pUnit = _pShellDispatcher->GetSelectedUnit();
		if (pUnit) {
			const AttributeBase* attr = 0;
			if (pUnit->attr().ID == UNIT_ATTRIBUTE_SQUAD) {
				terUnitAttributeID id = safe_cast<terUnitSquad*>(pUnit)->currentMutation();
				if (id != UNIT_ATTRIBUTE_NONE) {
					attr = pUnit->Player->unitAttribute(id);
				}
			} else {
				attr = &(pUnit->attr());
			}

//			if (attr && (attr->AttackClass & UNIT_CLASS_GROUND)) {
//				_pShellDispatcher->m_nPickData = 1;
//			}
		}

		_shellCursorManager.SetActiveCursor(CShellCursorManager::attack, 1);
	}
}

void OnButtonFrameInstall(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	if(code == EVENT_PRESSED)
	{
		terFrame* pFrame = safe_cast<terFrame*>(_pShellDispatcher->GetSelectedUnit());
		if(pFrame)
		{
			if(pFrame->attached())
				universe()->makeCommand(COMMAND_ID_FRAME_DETACH,0);
			else
			{
				if(!pFrame->basementReady())
				{
					CInfoWindow* pWndInfo = (CInfoWindow*)_shellIconManager.GetWnd(SQSH_INFOWND_ID);
					xassert(pWndInfo);
			
					static char _cb[200];
					pWndInfo->Show(true);
					pWndInfo->SetText(_shellIconManager.FormatMessageText("<frame_cant_install>", _cb)); 
					pWndInfo->SetTime(3000);
					pWndInfo->Centered();
				}

				universe()->makeCommand(COMMAND_ID_FRAME_ATTACH,0);
			}
		}
	}
}

void OnButtonFrameTeleportate(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	if(code == EVENT_PRESSED && universe()->activePlayer())
		universe()->activePlayer()->ClusterPick(COMMAND_ID_TELEPORTATE, 0);
}


void OnButtonUpgrade(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	if (code == EVENT_PRESSED) {
		universe()->makeCommand(COMMAND_ID_STRUCTURE_UPGRADE,0);
	} else if (code == EVENT_PRESSED_DISABLED) {
		CShellComplexPushButton* pBtn = (CShellComplexPushButton*)pWnd;
		if (pBtn->GetPause()) {
			universe()->makeCommand(COMMAND_ID_CONTINUE_CONSTRUCTION, 0);
		}
	} else if (code == EVENT_RPRESSED) {
		CShellComplexPushButton* pBtn = (CShellComplexPushButton*)pWnd;
		if (!pBtn->isEnabled()) {
			if (pBtn->GetPause()) {
				universe()->makeCommand(COMMAND_ID_CANCEL_UPGRADE, 0);
			} else {
				universe()->makeCommand(COMMAND_ID_HOLD_CONSTRUCTION, 0);
			}
		}
	}
}

void OnButtonBackToFrame(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	if(code == EVENT_PRESSED){
		terUnitBase* pUnit = _pShellDispatcher->GetSelectedUnit();
		xassert(pUnit);

		if(pUnit){
			pUnit->soundEvent(SOUND_VOICE_MMP_BACK_TO_FRAME);
			universe()->makeCommand(COMMAND_ID_OBJECT, universe()->activePlayer()->frame(), COMMAND_SELECTED_MODE_SINGLE);
		}
	}
}

void OnButtonAlarm(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	if(code == EVENT_PRESSED)
		universe()->activePlayer()->ClusterPick(COMMAND_ID_FRAME_ALARM_ON, 0);
}

void OnButtonStartCharge(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	if(code == EVENT_PRESSED)
	{
		bool stop = ((CShellComplexPushButton*)pWnd)->GetCheck();
		universe()->makeCommand(stop ? COMMAND_ID_STOP_CHARGING : COMMAND_ID_START_CHARGING, 0);
	}
}

void OnButtonStopCharge(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	if(code == EVENT_PRESSED)
		universe()->makeCommand(COMMAND_ID_STOP_CHARGING, 0);
}

void OnButtonFieldSingleOn(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	if(code == EVENT_PRESSED)
	{
		bool stop = ((CShellComplexPushButton*)pWnd)->GetCheck();
		universe()->makeCommand(stop ? COMMAND_ID_FIELD_STOP : COMMAND_ID_FIELD_START,0);
	}
}
void OnButtonFieldSingleOff(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	if(code == EVENT_PRESSED)
	{
		universe()->makeCommand(COMMAND_ID_FIELD_STOP,0);
	}
}
void OnButtonStructuteOn(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	if(code == EVENT_PRESSED)
	{
		bool on = ((CShellComplexPushButton*)pWnd)->GetCheck();
		universe()->makeCommand(on ? COMMAND_ID_POWER_OFF : COMMAND_ID_POWER_ON, 0);
	}
}
void OnButtonStructuteOff(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	if(code == EVENT_PRESSED)
	{
//		terBuilding* building = safe_cast<terBuilding*>(_pShellDispatcher->GetSelectedUnit());
//		if(building->isPluggedIn())
		universe()->makeCommand(COMMAND_ID_POWER_OFF,0);
	}
}
void OnButtonPatrol(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	if(code == EVENT_PRESSED)
	{
		_pShellDispatcher->m_nPickAction = SHELL_PICK_UNIT_PATROL;
		_pShellDispatcher->m_nPickData = 1;
		_shellCursorManager.SetActiveCursor(CShellCursorManager::move, 1);
	}
}

void OnButtonStop(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	if(code == EVENT_PRESSED)
		universe()->makeCommand(COMMAND_ID_STOP,0);
}
void OnButtonBrigadierChange(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	if(code == EVENT_PRESSED){
		terUnitBase* pUnit = _pShellDispatcher->GetSelectedUnit();
		xassert(pUnit);

		if(pUnit){
			for(int i=0; i<5; i++) {
				TerrainButtonData* slotData = &(gameShell->getLogicUpdater().getLogicData()->slots[i]);
				if(slotData->unit == pUnit){
					universe()->activePlayer()->ClusterPick(
						pUnit->attr().ID == UNIT_ATTRIBUTE_TERRAIN_MASTER ? COMMAND_ID_BUILD_MASTER_INC : COMMAND_ID_TERRAIN_MASTER_INC, i);

					universe()->activePlayer()->soundEvent(SOUND_VOICE_MMP_CONVERT_COMMAND);
					break;
				}
			}
		}
	}
}
void OnButtonBrigadierBuild(CShellWindow* pWnd, InterfaceEventCode code, int param)
{
	if(code == EVENT_PRESSED)
	{
		_pShellDispatcher->m_nPickAction = SHELL_PICK_UNIT_BUILD;
		_shellCursorManager.SetActiveCursor(CShellCursorManager::sell, 1);
	}
}

void OnButtonReplayPlayer(CShellWindow* pWnd, InterfaceEventCode code, int param) {
	if (code == EVENT_PRESSED) {
		gameShell->switchActivePlayer(true);
	} else if (code == EVENT_RPRESSED) {
		gameShell->switchActivePlayer(false);
	}
}
