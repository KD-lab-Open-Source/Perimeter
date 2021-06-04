#include "StdAfx.h"
#include "SelectManager.h"
#include "GameShell.h"
#include "Universe.h"
#include "Config.h"
#include "CameraManager.h"
#include "ExternalShow.h"
#include "InterfaceLogic.h"

extern GameShell* gameShell;

cSelectManager::cSelectManager()
:selectedGroupPriority(0)
{
	player=NULL;
}

cSelectManager::~cSelectManager()
{
}

void cSelectManager::clear(unsigned int group)
{
	UnitList::iterator it;
	FOR_EACH(SelectGroupLists[group],it)
	{
		(*it)->Deselect();
	}

	SelectGroupLists[group].clear();
}

void cSelectManager::removeEnemyAndDead(unsigned int group) {
	for (UnitList::iterator it = SelectGroupLists[group].begin(); it != SelectGroupLists[group].end();) {
		if ( (*it)->playerID() != player->playerID() || !(*it)->alive() ) {
			it = erase(it, group);
		} else {
			it++;
		}
	}
}

void cSelectManager::Quant()
{
	CSELECT_AUTOLOCK();
	if(player!=universe()->activePlayer())
	{
		player=universe()->activePlayer();
	}

	//remove enemy || !alive
	for (unsigned int i = 0; i <= TEMP_SELECTION_GROUP_NUMBER; i++) {
		removeEnemyAndDead(i);
	}
}

int cSelectManager::getUnitSelectionPriority(terUnitBase* p) 
{
	if (p->GetSquadPoint() && (p->attr().MilitaryUnit || p->GetSquadPoint() == p)){
		//сквад
		return 1;
	} else if (p->attr().ID == UNIT_ATTRIBUTE_TERRAIN_MASTER || p->attr().ID == UNIT_ATTRIBUTE_BUILD_MASTER){
		//прорабы, бригадиры
		return 2;
	} else if (p->attr().ID == UNIT_ATTRIBUTE_CORE){
		//ядро
		return 3;
	} else if (p->attr().MilitaryUnit && p->attr().isBuilding()){
		//стац. орудие
		return 4;
	} else {
		//остальные
		return 5;
	}
}

int cSelectManager::calcGroupPriority(UnitList& unit_list) {
	int res = 5;
	UnitList::iterator i_unit1;
	FOR_EACH (unit_list, i_unit1) {
		res = min(getUnitSelectionPriority(*i_unit1), res);
	}
	return res;
}

void cSelectManager::areaToSelection(float x0, float y0, float x1, float y1, int mode) {
	cancelActions();
	CSELECT_AUTOLOCK();
	if (fabs(x0 - x1) < 0.005f && fabs(y0 - y1) < 0.005f) {
		terUnitBase* unit = player->traceUnit(Vect2f(x0, y0));
		if (unit) {
			unitToSelection(unit, mode);
		} else if (mode == COMMAND_SELECTED_MODE_SINGLE) {
			//deselect and clear temp
			clear(TEMP_SELECTION_GROUP_NUMBER);
			clear();
		}
	} else {
		if (SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER].empty()) {
			mode = COMMAND_SELECTED_MODE_SINGLE;
		}
		//deselect and clear temp
		clear(TEMP_SELECTION_GROUP_NUMBER);
		switch (mode) {
			case COMMAND_SELECTED_MODE_SINGLE:
				clear();
				MakeSelectionList(x0 - 0.001f, y0 - 0.001f, x1 + 0.001f, y1 + 0.001f, SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER]);
				filterSelection();
				break;
			case COMMAND_SELECTED_MODE_NEGATIVE:
				UnitList unit_list;
				MakeSelectionList(x0 - 0.001f, y0 - 0.001f, x1 + 0.001f, y1 + 0.001f, unit_list);
				filterSelectionList(unit_list, selectedGroupPriority);
				
				UnitList::iterator unitIter;
				UnitList::iterator unitIter1;
				FOR_EACH (unit_list, unitIter) {
					unitIter1 = findInSelection(*unitIter);
					if (unitIter1 == SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER].end()) {
						SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER].push_back(*unitIter);
					} else {
						erase(unitIter1);
					}
				}
				break;
		}
		selectCurrentSelection();
	}
}

void cSelectManager::unitToSelection(terUnitBase* p, int mode, bool passSquad) {
	cancelActions();
	CSELECT_AUTOLOCK();
	if (p->alive() && p->selectAble() && p->playerID() == player->playerID() && (passSquad || p->attr().ID != UNIT_ATTRIBUTE_SQUAD)) {
		if (SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER].empty()) {
			mode = COMMAND_SELECTED_MODE_SINGLE;
		}
		//deselect and clear temp
		clear(TEMP_SELECTION_GROUP_NUMBER);
		switch (mode) {
			case COMMAND_SELECTED_MODE_SINGLE:
				clear();
				selectedGroupPriority = getUnitSelectionPriority(p);
				addUnitOrSquadToSelection(p);
				break;
			case COMMAND_SELECTED_MODE_NEGATIVE:
				int unitPriority = getUnitSelectionPriority(p);
				if (p->GetSquadPoint() && p->attr().MilitaryUnit) {
					p = p->GetSquadPoint();
				}
				UnitList::iterator unitIter = findInSelection(p);
				if (unitIter != SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER].end()) {
					erase(unitIter);
				} else {
					if (unitPriority == selectedGroupPriority) {
						addToSelection(p);
					} else {
						clear();
						selectedGroupPriority = unitPriority;
						addToSelection(p);
					}
				}
				break;
		}
		selectCurrentSelection();
	}
}

void cSelectManager::grade(terUnitBase* from, terUnitBase* to) {
	CSELECT_AUTOLOCK();

	if (from->playerID() == player->playerID()) {
		UnitList::iterator unitIter = findInSelection(from);
		if (unitIter != SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER].end()) {
			erase(unitIter);
		} else {
			return;
		}
	}

	if (
			to->alive()
		&&	to->selectAble()
		&&	to->playerID() == player->playerID()
		&&	to->attr().ID != UNIT_ATTRIBUTE_SQUAD) {

		//deselect and clear temp
		clear(TEMP_SELECTION_GROUP_NUMBER);

		if (SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER].empty()) {
			clear();
			selectedGroupPriority = getUnitSelectionPriority(to);
			addUnitOrSquadToSelection(to);
		} else {
			int unitPriority = getUnitSelectionPriority(to);
			if (to->GetSquadPoint() && to->attr().MilitaryUnit) {
				to = to->GetSquadPoint();
			}
			if (unitPriority == selectedGroupPriority) {
				UnitList::iterator unitIter = findInSelection(to);
				if (unitIter == SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER].end()) {
					addToSelection(to);
				}
			}
		}

		selectCurrentSelection();
	}
}

void cSelectManager::selectInAreaAndCurrentSelection(float x0, float y0, float x1, float y1, int mode) {
	cancelActions();
	CSELECT_AUTOLOCK();
	if (fabs(x0 - x1) < 0.005f && fabs(y0 - y1) < 0.005f) {
		terUnitBase* unit = player->traceUnit(Vect2f(x0, y0));
		if (unit) {
			selectUnitAndCurrentSelection(unit, mode);
		} else if (mode == COMMAND_SELECTED_MODE_NEGATIVE) {
			clear(TEMP_SELECTION_GROUP_NUMBER);
			selectCurrentSelection();
		}
	} else {
		if (SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER].empty()) {
			mode = COMMAND_SELECTED_MODE_SINGLE;
		}
		UnitList unit_list;
		UnitList::iterator unitIter;
		clear(TEMP_SELECTION_GROUP_NUMBER);
		switch (mode) {
			case COMMAND_SELECTED_MODE_SINGLE:
				//!!!
				clear();
				MakeSelectionList(x0 - 0.001f, y0 - 0.001f, x1 + 0.001f, y1 + 0.001f, unit_list);
				filterSelectionList(unit_list);
				
				FOR_EACH (unit_list, unitIter) {
					(*unitIter)->Select();
					SelectGroupLists[TEMP_SELECTION_GROUP_NUMBER].push_back(*unitIter);
				}
				break;
			case COMMAND_SELECTED_MODE_NEGATIVE:
				MakeSelectionList(x0 - 0.001f, y0 - 0.001f, x1 + 0.001f, y1 + 0.001f, unit_list);
				filterSelectionList(unit_list, selectedGroupPriority);
				
				selectCurrentSelection();
				FOR_EACH (unit_list, unitIter) {
					if (findInSelection(*unitIter) == SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER].end()) {
						(*unitIter)->Select();
						SelectGroupLists[TEMP_SELECTION_GROUP_NUMBER].push_back(*unitIter);
					} else {
						(*unitIter)->Deselect();
					}
				}
				break;
		}
	}
}

void cSelectManager::selectUnitAndCurrentSelection(terUnitBase* p, int mode) {
	cancelActions();
	CSELECT_AUTOLOCK();
	if (p->alive() && p->selectAble() && p->playerID() == player->playerID() && p->attr().ID != UNIT_ATTRIBUTE_SQUAD) {
		if (SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER].empty()) {
			mode = COMMAND_SELECTED_MODE_SINGLE;
		}
		switch (mode) {
			case COMMAND_SELECTED_MODE_SINGLE:
				p->Select();
				clear(TEMP_SELECTION_GROUP_NUMBER);
				//!!!
//				selectCurrentSelection();
				clear();
				SelectGroupLists[TEMP_SELECTION_GROUP_NUMBER].push_back(p);
				break;
			case COMMAND_SELECTED_MODE_NEGATIVE:
				int unitPriority = getUnitSelectionPriority(p);
				if (p->GetSquadPoint() && p->attr().MilitaryUnit) {
					p = p->GetSquadPoint();
				}
				if (findInSelection(p) != SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER].end()) {
					//можно заменить на выборочный селект
					selectCurrentSelection();
					p->Deselect();
				} else {
					clear(TEMP_SELECTION_GROUP_NUMBER);
					if (unitPriority == selectedGroupPriority) {
						selectCurrentSelection();
					}
					p->Select();
					SelectGroupLists[TEMP_SELECTION_GROUP_NUMBER].push_back(p);
				}
				break;
		}

	}
}

void cSelectManager::filterSelection() {
	xassert(select_lock.is_lock());
	selectedGroupPriority = calcGroupPriority(SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER]);
	filterSelectionList(SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER], selectedGroupPriority);
}

void cSelectManager::allLikeUnitToSelection(terUnitBase* p) {
	cancelActions();
	CSELECT_AUTOLOCK();
	if (p->alive() && p->selectAble() && p->playerID() == player->playerID()) {
		selectedGroupPriority = getUnitSelectionPriority(p);
		clear(TEMP_SELECTION_GROUP_NUMBER);
		clear();
		if ( p->GetSquadPoint() && p->attr().MilitaryUnit ) {
			SquadList::iterator sq_it;
			FOR_EACH (player->squadList(), sq_it) {
				if(*sq_it)
					SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER].push_back(*sq_it);
			}
		} else {
			CUNITS_LOCK(player);
			int unitIDToAdd = p->attr().ID;
			const UnitList& unit_list=player->units();
			UnitList::const_iterator un_it;
			FOR_EACH (unit_list, un_it) {
				if ( (*un_it)->attr().ID == unitIDToAdd ) {
					SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER].push_back(*un_it);
				}
			}

		}
		selectCurrentSelection();
	}
}

void cSelectManager::addUnitOrSquadToSelection(terUnitBase* p) {
	if ( p->GetSquadPoint() && p->attr().MilitaryUnit ) {
		addToSelection( p->GetSquadPoint() );
	} else {
		addToSelection(p);
	}
}

void cSelectManager::makeCommand2D(CommandID command_id, const Vect3f& position, CommandSelectionMode mode) {
	CSELECT_AUTOLOCK();
	UnitList::iterator ui;
	FOR_EACH(SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER],ui)
		(*ui)->commandOutcoming(UnitCommand(command_id, To3D(position), 0, mode));
}

void cSelectManager::makeCommandSubtle(CommandID command_id, const Vect3f& position, CommandSelectionMode mode)
{

	if(!SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER].empty()){
		int size = SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER].size();
		if(size > 1){
			Vect2f posAvr = Vect2f::ZERO;
			UnitList::iterator ui;
			FOR_EACH(SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER], ui)
				posAvr += (*ui)->position2D();
			posAvr /= size;

			Vect2f dir = Vect2f(position) - posAvr;
			Vect2f delta(dir.y, -dir.x);
			delta /= delta.norm() + FLT_EPS;

			ui = SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER].begin();
			Vect2f left = position;
			Vect2f right = position;
			if(size & 1){
				(*ui)->commandOutcoming(UnitCommand(command_id, To3D(position), 0, mode));
				left -= delta*(*ui)->radius();
				right += delta*(*ui)->radius();
				++ui;
			}

			while(ui != SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER].end()){ // Остаток четный
				(*ui)->commandOutcoming(UnitCommand(command_id, To3D(left -= delta*(*ui)->radius()), 0, mode));
				left -= delta*(*ui)->radius();
				++ui;

				(*ui)->commandOutcoming(UnitCommand(command_id, To3D(right += delta*(*ui)->radius()), 0, mode));
				right += delta*(*ui)->radius();
				++ui;
			}
		}
		else
			SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER].front()->commandOutcoming(UnitCommand(command_id, To3D(position), 0, mode));
	}

}

void cSelectManager::makeCommand(CommandID command_id, const Vect3f& position, CommandSelectionMode mode)
{
	CSELECT_AUTOLOCK();
	UnitList::iterator ui;
	FOR_EACH(SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER],ui)
		(*ui)->commandOutcoming(UnitCommand(command_id, position, 0, mode));
}

void cSelectManager::makeCommand(CommandID id, int data)
{
	CSELECT_AUTOLOCK();
	UnitList::iterator ui;
	FOR_EACH(SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER],ui)
		(*ui)->commandOutcoming(UnitCommand(id, data, COMMAND_SELECTED_MODE_NEGATIVE));
}

void cSelectManager::makeCommand(CommandID command_id, terUnitBase* actionObject, CommandSelectionMode mode)
{
	CSELECT_AUTOLOCK();
	UnitList::iterator ui;
	FOR_EACH(SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER],ui)
		(*ui)->commandOutcoming(UnitCommand(command_id, actionObject, 0, mode));
}

void cSelectManager::makeCommand2DWithCanAttackFilter(const Vect3f& position) {
	CSELECT_AUTOLOCK();
	UnitList::iterator ui;
	FOR_EACH (SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER], ui) {
		if (CanAttackUnit(*ui, 0)) {
			(*ui)->commandOutcoming(UnitCommand(COMMAND_ID_POINT, To3D(position), 0, COMMAND_SELECTED_MODE_SINGLE));
		}
	}
}
void cSelectManager::makeCommandWithCanAttackFilter(const Vect3f& position) {
	CSELECT_AUTOLOCK();
	UnitList::iterator ui;
	FOR_EACH(SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER], ui) {
		if (CanAttackUnit(*ui, 0)) {
			(*ui)->commandOutcoming(UnitCommand(COMMAND_ID_ATTACK, position, 0, COMMAND_SELECTED_MODE_SINGLE));
		}
	}
}
void cSelectManager::makeCommandWithCanAttackFilter(terUnitBase* actionObject) {
	CSELECT_AUTOLOCK();
	UnitList::iterator ui;
	FOR_EACH(SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER], ui) {
		if (CanAttackUnit(*ui, actionObject)) {
			(*ui)->commandOutcoming(UnitCommand(COMMAND_ID_OBJECT, actionObject, 0, COMMAND_SELECTED_MODE_SINGLE));
		}
	}
}

void cSelectManager::toggleHold()
{
	CSELECT_AUTOLOCK();
	UnitList::iterator ui;
	FOR_EACH(SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER],ui) {
		terBuilding* building = dynamic_cast<terBuilding*>(*ui);
		if (building) {
			if (building->buildingStatus() & BUILDING_STATUS_HOLD_CONSTRUCTION) {
				building->commandOutcoming(UnitCommand(COMMAND_ID_CONTINUE_CONSTRUCTION, 0, COMMAND_SELECTED_MODE_NEGATIVE));
			} else if (building->isUpgrading() || !building->isConstructed()) {
				building->commandOutcoming(UnitCommand(COMMAND_ID_HOLD_CONSTRUCTION, 0, COMMAND_SELECTED_MODE_NEGATIVE));
			} 
		} else if (dynamic_cast<terUnitSquad*>(*ui)) {
//			DamageMolecula a_req, a_progr, a_enabled, a_pause;
//			pSquad->GetAtomPaused(a_pause);
			(*ui)->commandOutcoming(UnitCommand(COMMAND_ID_PRODUCTION_PAUSE_ON, MUTATION_ATOM_SOLDIER, COMMAND_SELECTED_MODE_NEGATIVE));
			(*ui)->commandOutcoming(UnitCommand(COMMAND_ID_PRODUCTION_PAUSE_ON, MUTATION_ATOM_OFFICER, COMMAND_SELECTED_MODE_NEGATIVE));
			(*ui)->commandOutcoming(UnitCommand(COMMAND_ID_PRODUCTION_PAUSE_ON, MUTATION_ATOM_TECHNIC, COMMAND_SELECTED_MODE_NEGATIVE));
		}
	}
}

void cSelectManager::addToSelection(terUnitBase* p)
{
	CSELECT_AUTOLOCK();
	if(findInSelection(p) == SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER].end())
		SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER].push_back(p);
}

void cSelectManager::DeleteSelectedObjects()
{
	CSELECT_AUTOLOCK();
	UnitList::iterator ui;
	FOR_EACH(SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER],ui)
		(*ui)->Kill();
}

void cSelectManager::explodeUnit()
{
	CSELECT_AUTOLOCK();
	UnitList::iterator ui;
	FOR_EACH(SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER], ui)
		(*ui)->commandOutcoming(UnitCommand(COMMAND_ID_EXPLODE_UNIT, 0));
}

void cSelectManager::MakeSelectionList(float x0, float y0, float x1, float y1, UnitList& out_list)
{
	xassert(select_lock.is_lock());
	if (x0 > x1) {
		swap(x0, x1);
	}
	if (y0 > y1) {
		swap(y0, y1);
	}
	sPlane4f PlaneClip[5];
	terCamera->GetCamera()->GetPlaneClip(PlaneClip,&sRectangle4f(x0,y0,x1,y1));

	CUNITS_LOCK(player);
	const UnitList& unit_list=player->units();
	UnitList::const_iterator ui;
	FOR_EACH(unit_list,ui){
		terUnitBase* p = *ui;
		if(p->alive() && p->selectAble() && p->attr().ID != UNIT_ATTRIBUTE_SQUAD){
//		if(p->alive() && p->selectAble()){
			if(terObjectBoxTest(p->position(),PlaneClip))
				out_list.push_back(p);
		}
	}
}

void cSelectManager::filterSelectionList(UnitList& unit_list) {
	filterSelectionList(unit_list, calcGroupPriority(unit_list));
}

void cSelectManager::filterSelectionList(UnitList& unit_list, int priority) {
	if (priority == 1) {
		UnitList squad_list;

		UnitList::iterator i_unit1 = unit_list.begin();
		while (i_unit1 != unit_list.end()) {
			terUnitBase* p = (*i_unit1)->GetSquadPoint();
			if (p && (*i_unit1)->attr().MilitaryUnit ) {
				UnitList::iterator i_unit2;
				FOR_EACH (squad_list,i_unit2) {
					if (*i_unit2 == p) {
						break;
					}
				}
				if (i_unit2 == squad_list.end()) {
					squad_list.push_back(p);
				}
			}

			(*i_unit1)->Deselect();
			i_unit1 = unit_list.erase(i_unit1);
		}

		FOR_EACH (squad_list, i_unit1) {
			unit_list.push_back(*i_unit1);
		}
	} else {
		UnitList::iterator i_unit1 = unit_list.begin();
		while (i_unit1 != unit_list.end()) {
			if (getUnitSelectionPriority(*i_unit1) != priority) {
				(*i_unit1)->Deselect();
				i_unit1 = unit_list.erase(i_unit1);
			} else {
				++i_unit1;
			}
		}
	}

}

void cSelectManager::ShowCircles()
{
	MTL();
	CSELECT_AUTOLOCK();
	gbCircleShow->Lock();
	gbCircleShow->Clear();

	UnitList::iterator i_unit;
	FOR_EACH(SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER],i_unit){
		if((*i_unit)->alive() && (*i_unit)->selected()){
			(*i_unit)->ShowCircles();
		}
	}
	//show temp circles
	FOR_EACH(SelectGroupLists[TEMP_SELECTION_GROUP_NUMBER],i_unit){
		if((*i_unit)->alive() && (*i_unit)->selected()){
			(*i_unit)->ShowCircles();
		}
	}

	gbCircleShow->Unlock();
}

int cSelectManager::SelectUnit(terUnitBase* p)
{
	cancelActions();
	CSELECT_AUTOLOCK();
	UnitList::iterator i_unit;
	if(p->alive() && p->selectAble() && p->playerID() == player->playerID()){
		if(p->isSingleSelection()){
			clear();
			SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER].push_back(p);
			return 1;
		}
		else{
			FOR_EACH(SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER],i_unit){
				if((*i_unit)->isSingleSelection()){
					clear();
					SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER].push_back(p);
					return 1;
				}
			}
			addToSelection(p);
			return 1;
		}
		clear(TEMP_SELECTION_GROUP_NUMBER);
		selectCurrentSelection();
	}
	return 0;
}

int cSelectManager::SelectSquad(terUnitBase* p)
{
	cancelActions();
	CSELECT_AUTOLOCK();
	if(p->alive() && p->selectAble() && p->playerID() == player->playerID()){
		clear();
		addToSelection(p);
		selectedGroupPriority = getUnitSelectionPriority(p);
		clear(TEMP_SELECTION_GROUP_NUMBER);
		selectCurrentSelection();
		return 1;
	}
	return 0;
}

void cSelectManager::DeselectAll()
{
	CSELECT_AUTOLOCK();
	clear(TEMP_SELECTION_GROUP_NUMBER);
	clear();
}

terUnitBase* cSelectManager::selectedObject() 
{ 
	CSELECT_AUTOLOCK();
	return !SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER].empty() ? SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER].front() : 0; 
}

void cSelectManager::selectGroup(unsigned int group) {
	if (group < CURRENT_SELECTION_GROUP_NUMBER) {
		CSELECT_AUTOLOCK();
		copyGroup(group, CURRENT_SELECTION_GROUP_NUMBER);
		clear(TEMP_SELECTION_GROUP_NUMBER);
		selectCurrentSelection();
	}
}

void cSelectManager::putCurrentSelectionToGroup(unsigned int group) {
	if (group < CURRENT_SELECTION_GROUP_NUMBER) {
		CSELECT_AUTOLOCK();
		copyGroup(CURRENT_SELECTION_GROUP_NUMBER, group);
		selectCurrentSelection();
	}
}

void cSelectManager::addCurrentSelectionToGroup(unsigned int group) {
	if (group < CURRENT_SELECTION_GROUP_NUMBER) {
		CSELECT_AUTOLOCK();
		if (SelectGroupLists[group].empty()) {
			copyGroup(CURRENT_SELECTION_GROUP_NUMBER, group);
			return;
		}
		int groupPriority = calcGroupPriority(SelectGroupLists[group]);
		if (groupPriority == selectedGroupPriority) {
			cancelActions();
			UnitList::iterator unitIter;
			UnitList::iterator unitIter1;
			FOR_EACH (SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER], unitIter) {
				unitIter1 = findInSelection(*unitIter, group);
				if (unitIter1 == SelectGroupLists[group].end()) {
					SelectGroupLists[group].push_back(*unitIter);
				}
			}
		}
	}
}

void cSelectManager::copyGroup(unsigned int groupSource, unsigned int groupDestination) {
	if (!SelectGroupLists[groupSource].empty()) {
		cancelActions();
		if (!SelectGroupLists[groupDestination].empty()) {
			clear(groupDestination);
		}
		UnitList::iterator it;
		FOR_EACH (SelectGroupLists[groupSource], it) {
			SelectGroupLists[groupDestination].push_back(*it);
		}
	}
}

bool cSelectManager::CanAttackUnit(terUnitBase* pUnit, terUnitBase* pTarget) {
	bool res = false;

	if (pUnit->attr().ID == UNIT_ATTRIBUTE_SQUAD) {
		terUnitSquad* squad = safe_cast<terUnitSquad*>(pUnit);
		terPlayer* player = pUnit->Player;
		int attackClass = UNIT_CLASS_IGNORE;
		if (!squad->Empty()) {
			if ( !squad->isBase() ) {
				attackClass = player->unitAttribute(squad->currentMutation())->AttackClass;
			} else {
				attackClass = squad->attackEnemyClass();
/*
				if ( squad->countUnits(UNIT_ATTRIBUTE_SOLDIER) ) {
					attackClass |= player->unitAttribute(UNIT_ATTRIBUTE_SOLDIER)->AttackClass;
				}
				if ( squad->countUnits(UNIT_ATTRIBUTE_TECHNIC) ) {
					attackClass |= player->unitAttribute(UNIT_ATTRIBUTE_TECHNIC)->AttackClass;
				}
				if ( squad->countUnits(UNIT_ATTRIBUTE_OFFICER) ) {
					attackClass |= player->unitAttribute(UNIT_ATTRIBUTE_OFFICER)->AttackClass;
				}
*/
			}
		}
		if (pTarget) {
			res = attackClass & pTarget->unitClass();
		} else if (attackClass & UNIT_CLASS_GROUND) {
			res = true;
		}
	} else if (pUnit->isBuildingPowerOn()) {
		const AttributeBase* attr = &(pUnit->attr());
		if (pTarget) {
			if (attr->AttackClass & pTarget->unitClass()) {
				float d2 = pUnit->position().distance2(pTarget->position());
				res = d2 < sqr(attr->fireRadius()) && d2 > sqr(attr->fireRadiusMin());
			}
		} else {
			if (attr->AttackClass & UNIT_CLASS_GROUND) {
				Vect3f v;
				if (terCamera->cursorTrace(Vect2f(_pShellDispatcher->m_fMouseCurrentX - 0.5f, _pShellDispatcher->m_fMouseCurrentY - 0.5f), v)) {
					float d2 = pUnit->position().distance2(v);
					res = d2 < sqr(attr->fireRadius()) && d2 > sqr(attr->fireRadiusMin());
				}
			}
		}
	}

	return res;
}

bool cSelectManager::canSelectionAttackUnit(terUnitBase* target) {
	CSELECT_AUTOLOCK();
	UnitList::iterator i_unit;
	FOR_EACH(SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER], i_unit) {
		if (CanAttackUnit(*i_unit, target)) {
			return true;
		}
	}
	return false;
}

void cSelectManager::cancelActions() {
	if (gameShell->BuildingInstaller.inited()) {
		gameShell->BuildingInstaller.CancelObject();
	}
	if (_pShellDispatcher->m_nPickAction != SHELL_PICK_NONE) {
		_pShellDispatcher->m_nPickAction = SHELL_PICK_NONE;
		_pShellDispatcher->m_nPickData = 0;
		_pShellDispatcher->setArrowCursor();
	}
}
