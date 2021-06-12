#pragma once
#include "GenericControls.h"

class cSelectManager
{
public:
	cSelectManager();
	~cSelectManager();
	void Quant();

	MTSection* GetLock(){return &select_lock;}
	const UnitList& GetSelectList()const{xassert(select_lock.is_lock()); return SelectGroupLists[CURRENT_SELECTION_GROUP_NUMBER];}

	void areaToSelection(float x0, float y0, float x1, float y1, int mode);
	void unitToSelection(terUnitBase* p, int mode, bool passSquad = false);
	void grade(terUnitBase* from, terUnitBase* to);
	void allLikeUnitToSelection(terUnitBase* p);
	void selectInAreaAndCurrentSelection(float x0, float y0, float x1, float y1, int mode);
	void selectUnitAndCurrentSelection(terUnitBase* p, int mode);

	void DeleteSelectedObjects();//debug function
	void explodeUnit();//debug function

	void makeCommand(CommandID id, int data);
	void makeCommand2D(CommandID command_id, const Vect3f& position, CommandSelectionMode mode);
	void makeCommand(CommandID command_id, const Vect3f& position, CommandSelectionMode mode);
	void makeCommand(CommandID command_id, terUnitBase* actionObject, CommandSelectionMode mode);
	void makeCommand2DWithCanAttackFilter(const Vect3f& position);
	void makeCommandWithCanAttackFilter(const Vect3f& position);
	void makeCommandWithCanAttackFilter(terUnitBase* actionObject);
	void makeCommandSubtle(CommandID command_id, const Vect3f& position, CommandSelectionMode mode); // object or point
	void toggleHold();

	void ShowCircles();
	int SelectUnit(terUnitBase* p);
	int SelectSquad(terUnitBase* p);
	void DeselectAll();
	terUnitBase* selectedObject();


	void selectGroup(unsigned int group);
	void putCurrentSelectionToGroup(unsigned int group);
	void addCurrentSelectionToGroup(unsigned int group);

	bool canSelectionAttackUnit(terUnitBase* target);

	enum SelectGroups {
		SELECT_GROUP_MAX_COUNT = 10,
		CURRENT_SELECTION_GROUP_NUMBER,
		TEMP_SELECTION_GROUP_NUMBER
	};

protected:

	UnitList::iterator findInSelection(terUnitBase* p, unsigned int group = CURRENT_SELECTION_GROUP_NUMBER) {
		UnitList::iterator ui;
		FOR_EACH (SelectGroupLists[group],ui) {
			if((*ui) == p)
				break;
		}
		return ui;
	}

	void selectSelectionGroup(unsigned int group = CURRENT_SELECTION_GROUP_NUMBER) {
		UnitList::iterator i_unit1;
		FOR_EACH(SelectGroupLists[group],i_unit1)
			(*i_unit1)->Select();
	}

	void selectCurrentSelection() {
		selectSelectionGroup();
	}

	void addToSelection(terUnitBase* p);
	static int getUnitSelectionPriority(terUnitBase* p);

	MTSection select_lock;
	UnitList SelectGroupLists[TEMP_SELECTION_GROUP_NUMBER + 1];	//При удалении из SelectList обязательно делать Deselect()

	UnitList::iterator erase(UnitList::iterator it, unsigned int group = CURRENT_SELECTION_GROUP_NUMBER)
	{
		(*it)->Deselect();
		return SelectGroupLists[group].erase(it);
	}
	void removeEnemyAndDead(unsigned int group = CURRENT_SELECTION_GROUP_NUMBER);
	void clear(unsigned int group = CURRENT_SELECTION_GROUP_NUMBER);
	void copyGroup(unsigned int groupSource, unsigned int groupDestination);

	int selectedGroupPriority;

	void MakeSelectionList(float x0,float y0,float x1,float y1, UnitList& out_list);
	void filterSelectionList(UnitList& unit_list);
	void filterSelection();
	void filterSelectionList(UnitList& unit_list, int priority);
	void addUnitOrSquadToSelection(terUnitBase* p);
	int calcGroupPriority(UnitList& unit_list);

	bool CanAttackUnit(terUnitBase* pUnit, terUnitBase* pTarget);

	void cancelActions();

	terPlayer* player;

};

#define CSELECT_AUTOLOCK() MTAuto select_autolock(universe()->select.GetLock())
