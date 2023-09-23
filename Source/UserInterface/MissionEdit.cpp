#include "StdAfx.h"

#include "MissionEdit.h"

#include "Config.h"
#include "Runtime.h"
#include "terra.h"
#include "GenericControls.h"
#include "Universe.h"
#include "UniverseInterface.h"
#include "GameShell.h"
#include "CameraManager.h"
#include "Universe.h"
#include "GenericFilth.h"
#include "ScanPoly.h"
#include "GeoControl.h"
#include "EditArchive.h"
#include "qd_textdb.h"
#include "codepages/codepages.h"
#include "../HT/ht.h"

//------------------------------------------------
MissionEditor::MissionEditor()
: info_(1000, 1)
{
	copiedID_ = UNIT_ATTRIBUTE_NONE;
	copiedData_ = 0;

	showHardness_ = false;
	editingHardness_ = false;
	hardnessChanged_ = false;
}

MissionEditor::~MissionEditor()
{
    const char* msg = startsWith(getLocale(), "russian") ? "Редактирование неразрушаемости незаписано. Записать?" : "Editing hardness unsaved. Write?";
    if (hardnessChanged_ && MessageBoxQuestion("Mission Editor", msg)) {
        vMap.saveHardness();
    }
}

void MissionEditor::quant()
{
	terUnitBase* unit = universe()->selectedObject();
	if(unit){
		MTAutoSingleThread auto_skip;
		if(isPressed('V')){
			float factor = unit->position().distance(terCamera->GetCamera()->GetPos())*2;
			Vect3f delta = Vect3f::ZERO;
			if(!isShiftPressed()){
				terCamera->GetCamera()->GetMatrix().invXformVect(Vect3f(gameShell->mousePositionDelta().x, -gameShell->mousePositionDelta().y, 0)*factor, delta);
				Vect3f position = unit->position();
				float zOffset = position.z - To3D(position).z;
				delta.z = 0;
				position += delta;
				position.z = To3D(position).z + zOffset;
				unit->setPose(Se3f(unit->orientation(), position), true);
			}
			else{
				delta.z = -gameShell->mousePositionDelta().y*factor;
				unit->setPose(Se3f(unit->orientation(), unit->position() + delta), true);
			}
		}

		if(isPressed('C')){
			if(!isShiftPressed()){
				QuatF delta(-10*gameShell->mousePositionDelta().x, Vect3f::K);
				unit->setOrientation(unit->orientation() % delta);
			}
			else if(!isControlPressed()){
				if(gameShell->mousePositionDelta().norm2() > FLT_EPS){
					Vect3f axis = 10*Vect3f(-gameShell->mousePositionDelta().y, gameShell->mousePositionDelta().x, 0);
					terCamera->matrix().invXformVect(axis);
					QuatF delta(axis.norm(), axis);
					unit->setOrientation(delta % unit->orientation());
				}
			}
			else
				unit->setOrientation(QuatF::ID);
		}

		if(isPressed('X')){
			float radius = unit->radius();
			unit->setRadius(unit->radius()*(1 + gameShell->mousePositionDelta().x - gameShell->mousePositionDelta().y));
		}
	}


	//if(avatar())
	//	avatar()->setPose(pose());

	show();
}

bool MissionEditor::keyPressed(const sKey& Key)
{
	MTAutoSingleThread mtAutoSkipAssert;

	switch(Key.fullkey)
	{
	case 'Q':
		setPlayer(0);
		return true;
	case VK_TILDE:
	case 'W':
		setPlayer(-1);
		return true;
	case 'E':
		setPlayer(1);
		return true;
    case '0':
        setPlayer(isShiftPressed() ? 19 : 9);
        return true;

	case 'V': 
	case 'X': 
	case 'C': 
		return true;

	case 'C' | KBD_CTRL: 
		copyUnit();
		return true;
	case 'V' | KBD_CTRL: 
		pasteUnit();
		return true;

	case 'F': 
	case VK_INSERT:
		createUnit();
		return true;
		
	case VK_DELETE: {
        bool russian = startsWith(getLocale(), "russian");
        if (editingHardness_) {
            if (MessageBoxQuestion("MissionEditor", russian ? "Стереть всю неразрушаемость?" : "Erase all hardness?")) {
                clearHardness();
            }
        } else if (universe()->selectedObject() && MessageBoxQuestion("MissionEditor", russian ? "Удалить выделенный объект?" : "Delete selected objects?")) {
            universe()->DeleteSelectedObjects();
            _pUnitHover = 0;
        }
        return true;
    }

	case 'D': {
        const char* msg = startsWith(getLocale(), "russian") ? "Удалить выделенный объект?" : "Delete selected objects?";
        if (universe()->selectedObject() && MessageBoxQuestion("MissionEditor", msg)) {
            universe()->DeleteSelectedObjects();
            _pUnitHover = 0;
        }
        return true;
    }

	case 'D' | KBD_SHIFT:
        if (universe()->selectedObject()) {
            universe()->DeleteSelectedObjects();
            _pUnitHover = 0;
        }
		return true;

	case 'H' | KBD_CTRL:
		editingHardness_ = !editingHardness_;
		hardnessPolygon_.clear();
		if(editingHardness_ != showHardness_){
			vMap.toShowHardness(showHardness_ = !showHardness_);
			terMapPoint->UpdateMap(Vect2i::ZERO, Vect2i((int)vMap.H_SIZE, (int)vMap.V_SIZE));
		}
		return true; 

	case VK_RETURN: 
	case VK_RETURN | KBD_SHIFT: 
		if(editingHardness_){
			applyHardness();
			return true; 
		}
		else{
			terUnitBase* unit = universe()->selectedObject();
			if(unit){
				terRenderDevice->Flush(true);
                SDL_ShowCursor(SDL_TRUE);
				ShareHandle<SaveUnitData> data = unit->universalSave(0);
#ifdef _WIN32
                //TODO we should port editArchive to ingame UI and remove win32 window stuff
                HWND hwnd = static_cast<HWND>(hWndVisGeneric);
				EditArchive editArchive(hwnd, TreeControlSetup(0, 0, 500, 400, "editObjectPropsSetup"));
				if(editArchive.edit(data)){
					CSELECT_AUTOLOCK();
					const UnitList& select_list = universe()->select.GetSelectList();
					UnitList::const_iterator ui;
					FOR_EACH(select_list, ui){
						terUnitBase* unit1 = *ui;
						if(unit1->attr()->ID == unit->attr()->ID){
							if(unit1 != unit){
								SaveUnitData* data1 = unit1->universalSave(0);
								*data = *data1; // pos, ori, rad, label
								delete data1;
							}
							unit1->universalLoad(data);
							unit1->Start();
						}												
					}
				}
#endif
				terCamera->setFocus(HardwareCameraFocus);
                SDL_ShowCursor(SDL_FALSE);
				RestoreFocus();
				return true; 
			}
		}
		break;

	case VK_ESCAPE: 
	case VK_ESCAPE | KBD_SHIFT:
		if(editingHardness_) {
            hardnessPolygon_.clear();
            return true;
        }
	}
    
    //Player selector 0 and Tilde are handled in switch above
    if ('1' <= Key.fullkey && Key.fullkey <= '9') {
        setPlayer(Key.fullkey - '1');
        return true;
    } else if (('1' + KBD_SHIFT) <= Key.fullkey && Key.fullkey <= ('9' + KBD_SHIFT)) {
        setPlayer(Key.fullkey - ('1' + KBD_SHIFT) + 10);
        return true;
    }

	return false;
}

bool MissionEditor::keyUnpressed(const sKey& Key)
{
//	switch(Key.key)
//	{
//	}
	
	return false;
}

bool MissionEditor::mouseMove(const Vect2f& pos)
{
	if(gameShell->mouseLeftPressed() && editingHardness_){
		Vect3f v;
		if(terCamera->cursorTrace(pos, v))
			hardnessPolygon_.push_back(v);
		return true;
	}
	return false;
}

bool MissionEditor::mouseLeftPressed(const Vect2f& pos)
{
	if(editingHardness_){
		Vect3f v;
		if(terCamera->cursorTrace(pos, v))
			hardnessPolygon_.push_back(v);
		return true;
	}
	return false;
}

bool MissionEditor::mouseRightPressed(const Vect2f& pos)
{
	return false;
}

terFilthSpotID SelectFilth()
{
	struct 
	{
		const char* name;
		terFilthSpotID id;
	} name[]=
	{
		{"дракон",FILTH_SPOT_ID_DRAGON},
		{"муравей",FILTH_SPOT_ID_ANTS},
		{"пчела",FILTH_SPOT_ID_WASP},
		{"приведение",FILTH_SPOT_ID_GHOST},
		{"глаз",FILTH_SPOT_ID_EYE},
		{"ворона",FILTH_SPOT_ID_CROW},
		{"демон",FILTH_SPOT_ID_DAEMON},
		{"крыса",FILTH_SPOT_ID_RAT},
		{"червь",FILTH_SPOT_ID_WORM},
		{"рыба",FILTH_SPOT_ID_SHARK},
		{"вулкан",FILTH_SPOT_ID_VOLCANO},
		{"муравей2",FILTH_SPOT_ID_ANTS2},
		{"змеи",FILTH_SPOT_ID_SNAKE},
		{"дракон2",FILTH_SPOT_ID_DRAGON2},
		{"Вулкан от скамдизраптора",FILTH_SPOT_ID_VOLCANO_SCUM_DISRUPTOR},
		{"Новые муравьи",FILTH_SPOT_ID_A_ANTS}, //Муравьи aant.M3D
		{"Новые вороны",FILTH_SPOT_ID_A_CROW}, //Вороны abird.M3D
		{"Новые демоны",FILTH_SPOT_ID_A_DAEMON}, //Демоны ad_DAEMON.M3D
		{"Новый дракон",FILTH_SPOT_ID_A_DRAGON}, //Дракон adragon_body.M3D adragon_head.M3D adragon_tail.M3D
		{"Новые глаза",FILTH_SPOT_ID_A_EYE},  //Глаза aeye.M3D
		{"Новые крысы",FILTH_SPOT_ID_A_RAT}, //Крысы arat.M3D
		{"Новые пауки",FILTH_SPOT_ID_A_SPIDER}, //Муравьи2 aspider.M3D aspider1.M3D
		{"Новые осы",FILTH_SPOT_ID_A_WASP}, //Осы awasp.M3D
		{"Новый червь",FILTH_SPOT_ID_A_WORM},// Червь aworm.M3D
	};
	int sz=sizeof(name)/sizeof(name[0]);

    bool russian = startsWith(getLocale(), "russian");
	std::vector<const char*> filth;
	for (int i=0;i<sz;i++) {
        if (russian) {
            filth.push_back(name[i].name);
        } else {
            filth.push_back(getEnumDescriptor(FILTH_SPOT_ID_NONE).name(name[i].id));
        }
    }

    int choose=popupMenuIndex(filth);

    if(0 < choose) {
        return name[choose].id;
    }

	return FILTH_SPOT_ID_NONE;
}

terUnitAttributeID SelectGeo()
{
	struct 
	{
		const char* name;
		terUnitAttributeID id;
	} name[]=
	{
		{"гора",UNIT_ATTRIBUTE_GEO_INFLUENCE},
		{"разлом",UNIT_ATTRIBUTE_GEO_BREAK},
		{"деффект",UNIT_ATTRIBUTE_GEO_FAULT},
		{"лицо",UNIT_ATTRIBUTE_GEO_HEAD},
	};
	int sz=sizeof(name)/sizeof(name[0]);

    bool russian = startsWith(getLocale(), "russian");
    std::vector<const char*> filth;
    for (int i=0;i<sz;i++) {
        if (russian) {
            filth.push_back(name[i].name);
        } else {
            filth.push_back(getEnumDescriptor(UNIT_ATTRIBUTE_NONE).name(name[i].id));
        }
    }

	int choose=popupMenuIndex(filth);

	if(0 < choose) {
        return name[choose].id;
    }

	return UNIT_ATTRIBUTE_NONE;
}

void MissionEditor::createUnit()
{
	terUnitAttributeID attributeID = UNIT_ATTRIBUTE_NONE;
	const char* modelDirectory = 0;

    bool russian = startsWith(getLocale(), "russian");
	const char* itemBuildings = russian ? "Здания" : "Buildings";
	const char* itemNature = russian ? "Деревья" : "Nature";
	const char* itemFilth = russian ? "Скверна" : "Scourge";
	const char* itemSensors = russian ? "Сенсора" : "Sensors";
	const char* itemGeoprocess = russian ? "Геопроцессы" : "Geoprocess";
	std::vector<const char*> items;
	items.push_back(itemBuildings);
	items.push_back(itemNature);
	items.push_back(itemFilth);
	items.push_back(itemSensors);
	items.push_back(itemGeoprocess);
	const char* item = popupMenu(items);
	if(item == itemBuildings){
		terPlayer* player = universe()->activePlayer();
		items.clear();
		items.push_back(player->unitAttribute(static_cast<terUnitAttributeID>(UNIT_ATTRIBUTE_FRAME))->internalName(russian));
		for(int i = 0; i < UNIT_ATTRIBUTE_MAX; i++){
			terUnitAttributeID id = (terUnitAttributeID)i;
			if(isBuilding(id) || isLegionary(id)) {
                items.push_back(player->unitAttribute(id)->internalName(russian));
            }
		}
		const char* itemSel = popupMenu(items);
		if(itemSel) {
            if (russian) {
                attributeID = static_cast<terUnitAttributeID>(getEnumDescriptor(UNIT_ATTRIBUTE_NONE).keyByNameAlt(itemSel));
            } else {
                attributeID = static_cast<terUnitAttributeID>(getEnumDescriptor(UNIT_ATTRIBUTE_NONE).keyByName(itemSel));
            }
        }
	}
	else if(item == itemNature){
		setPlayer(-1);
		attributeID = UNIT_ATTRIBUTE_STATIC_NATURE;
		modelDirectory = "RESOURCE\\Models\\Environment";
	}
	else if(item == itemFilth){
		setPlayer(-1);
		attributeID = UNIT_ATTRIBUTE_FILTH_SPOT;
	}
	else if(item == itemSensors){
		setPlayer(-1);
		items.clear();
		const char* itemAlphaPotential = russian ? "Полюса для Альфы" : "Alpha potential";
		items.push_back(itemAlphaPotential);
		const char* item = popupMenu(items);
		if(item == itemAlphaPotential)
			attributeID = UNIT_ATTRIBUTE_ALPHA_POTENTIAL;
	}else if(item == itemGeoprocess)
	{
		Vect3f v;
		if(universe()->worldPlayer() && terCamera->cursorTrace(gameShell->mousePosition(),v))
		{
			terUnitAttributeID id=SelectGeo();
			if(id!=UNIT_ATTRIBUTE_NONE)
			{
				terGeoControl* p = safe_cast<terGeoControl*>(universe()->worldPlayer()->buildUnit(id));
				p->setPose(Se3f(QuatF::ID, v), true);
				p->Start();
				universe()->DeselectAll();
				universe()->SelectUnit(p);
			}
		}
	}

	if(attributeID != UNIT_ATTRIBUTE_NONE){
		Vect3f position;
		terCamera->cursorTrace(gameShell->mousePosition(), position);
		terUnitBase* unit = universe()->activePlayer()->buildUnit(attributeID);
		unit->setPose(Se3f(QuatF::ID, position), true);
		terBuilding* building = dynamic_cast<terBuilding*>(unit);
		if(building){
			building->setBuildingStatus(building->buildingStatus() | BUILDING_STATUS_CONSTRUCTED);
			building->setRealModel(0, 1);
		}
		unit->Start();
		universe()->DeselectAll();
		universe()->SelectUnit(unit);
		
		//_pShellDispatcher->m_nPickAction = SHELL_PICK_MISSIONEDITOR_PLACE_UNIT;
		//_pShellDispatcher->m_nPickData = attribute->ID;

		if(modelDirectory){
			std::string modelName;
			if(openFileDialog(modelName, modelDirectory, "m3d", "3D Model")){
                std::string modelNameLwr = string_to_lower(modelName.c_str());
				size_t pos = modelNameLwr.rfind("resource\\");
				if(pos != std::string::npos)
					modelName.erase(0, pos);
				unit->setModelName(modelName.c_str());
			}
		}

		if(attributeID == UNIT_ATTRIBUTE_FILTH_SPOT){
			terFilthSpotID id=SelectFilth();

			terFilthSpot* filth = safe_cast<terFilthSpot*>(unit);
			if(id!=FILTH_SPOT_ID_NONE)
				filth->SetFilthParamID(id);
		}
	}
}

const char* MissionEditor::info()
{
    const std::string& locale = getLocale();
    bool russian = startsWith(locale, "russian");
    info_.clear();
    
    //TODO use qdTextDB instead of hardcoding texts here

    //Mission name
	std::string missionName = gameShell->CurrentMission.savePathKey();
	size_t pos = missionName.rfind(std::string("resource") + PATH_SEP);
	if(pos != std::string::npos) {
        missionName.erase(0, pos);
    }
    if (russian) {
        info_ += convertToCodepage("Миссия: ", locale);
    } else {
        info_ += "Mission: ";
    }
    info_ += missionName + "\n";

    //Active Player
	terPlayer* player = universe()->activePlayer();
    info_ += qdTextDB::instance().getText("Interface.Menu.ComboItems.Player");
    if (russian) {
        if(player->isWorld()) {
            info_ += convertToCodepage(" \"Мир\"", locale);
        } else if (player->playerID() == 0) {
            info_ += convertToCodepage(" \"Я\"", locale);
        } else if (player->playerID() == 1) {
            info_ += convertToCodepage(" \"Враг\"", locale);
        }
    } else {
        if(player->isWorld()) {
            info_ += " \"World\"";
        } else if (player->playerID() == 0) {
            info_ += " \"Me\"";
        } else if (player->playerID() == 1) {
            info_ += " \"Enemy\"";
        }
    }
    info_ += " " + std::to_string(player->playerID()) + "\n";

    //Selected object
	terUnitBase* unit = universe()->selectedObject();
	if (unit) {
        if (russian) {
            info_ += convertToCodepage("Объект: ", locale);
        } else {
            info_ += "Object: ";
        }
        info_ += convertToCodepage(unit->attr()->internalName(false), locale);
        if (russian) {
            //Most alt names are in russian anyway
            info_ += " - ";
            info_ += convertToCodepage(unit->attr()->internalName(true), locale);
        }
        info_ += "\n";
		if (unit->GetModelName()) {
            if (russian) {
                info_ += convertToCodepage("Модель: ", locale);
            } else {
                info_ += "Model: ";
            }
            info_ += convertToCodepage(unit->GetModelName(), locale);
            info_ += "\n";
        }
	}

    
    //Copied item
	if (copiedData_) {
        if (russian) {
            info_ += convertToCodepage("Запомнен: ", locale);
        } else {
            info_ += "Copied: ";
        }
        info_ += convertToCodepage(getEnumDescriptor(UNIT_ATTRIBUTE_NONE).nameAlt(copiedData_->attributeID), locale);
        info_ += "\n";
    }

    //Hardness edit
	if (editingHardness_) {
        if (russian) {
            info_ += convertToCodepage(
                    !isShiftPressed() ? "Редактирование некопаемых областей\n"
                                      : "Редактирование копаемых областей\n",
                    locale
            );
        } else {
            info_ += !isShiftPressed() ? "Editing non-digging regions\n" : "Editing regions to dig\n";
        }
    }

    //Camera
	if (terCamera->pathSize()) {
        if (terCamera->restricted()) {
            info_ += "Camera: ";
        } else if (russian) {
            info_ += convertToCodepage("Camera (ограничена): ", locale);
        } else {
            info_ += "Camera (restricted): ";
        }
        info_ += std::to_string(terCamera->pathSize()) + "\n";
    }
    
	return info_.c_str();
}

void MissionEditor::copyUnit()
{
	terUnitBase* unit = universe()->selectedObject();
	if(!unit)
		return;

	copiedID_ = unit->attr()->ID;
	if(copiedData_){
		delete copiedData_;
		copiedData_ = 0;
	}
	copiedData_ = unit->universalSave(copiedData_);
}

void MissionEditor::pasteUnit()
{
	if(copiedData_){
		Vect3f position;
		terCamera->cursorTrace(gameShell->mousePosition(), position);
		terUnitBase* unit = universe()->activePlayer()->buildUnit(copiedID_);
		copiedData_->position = position;
		unit->universalLoad(copiedData_);
		unit->Start();
		universe()->DeselectAll();
		universe()->SelectUnit(unit);
	}
}

void MissionEditor::setPlayer(int playerID)
{
	if(playerID < 0 || playerID >= universe()->Players.size())
		playerID = universe()->Players.size() - 1;
	if(universe()->activePlayer()->playerID() != playerID)
		universe()->SetActivePlayer(playerID);
}

void MissionEditor::show()
{
	if(!hardnessPolygon_.empty()){
		Vect3f posPrev = hardnessPolygon_.back();
		Vect3fVect::iterator vi;
		FOR_EACH(hardnessPolygon_, vi){
			Vect3f posNext = vi + 1 != hardnessPolygon_.end() ? *(vi + 1) : hardnessPolygon_.front();
			if(((posPrev - *vi) % (posNext - *vi)).z <= 0){
				terRenderDevice->DrawLine(posPrev, *vi, !isShiftPressed() ? RED : WHITE);
				posPrev = *vi;
			}
			else{
				vi = hardnessPolygon_.erase(vi);
				--vi;
			}
		}
	}
}

class ApplyHardnessLineOp
{
	bool makeHard_;
	
public:
	ApplyHardnessLineOp(bool makeHard) : makeHard_(makeHard) {}
	void operator()(int x1,int x2,int y)
	{
		while(x1 <= x2){
			vMap.setHardness(x1, y, makeHard_ ? 0 : 3);
			x1++;
		}
	}
};

void MissionEditor::applyHardness()
{
	hardnessChanged_ = true;

	if(hardnessPolygon_.size() > 2){
		std::vector<Vect2i> points;
		Vect2i posMin(10000, 10000);
		Vect2i posMax(0, 0);
		Vect3fVect::iterator vi;
		FOR_EACH(hardnessPolygon_, vi){
            //3f -> 2f -> 2i
			Vect2i v =  static_cast<Vect2i>(static_cast<Vect2f>(*vi));
			if(posMin.x > v.x)
				posMin.x = v.x;
			if(posMin.y > v.y)
				posMin.y = v.y;
			if(posMax.x < v.x)
				posMax.x = v.x;
			if(posMax.y < v.y)
				posMax.y = v.y;
			points.push_back(vMap.w2m(Vect2f(v)));
		}

        ApplyHardnessLineOp op = ApplyHardnessLineOp(!isShiftPressed());
		scanPolyByLineOp(&points[0], points.size(), op);

		terMapPoint->UpdateMap(posMin, posMax);

		hardnessPolygon_.clear();
	}
}

void MissionEditor::clearHardness()
{
	hardnessChanged_ = true;

	for(int y = 0; y < vMap.GV_SIZE; y++)
		for(int x = 0; x < vMap.GV_SIZE; x++)
			vMap.setHardness(x, y, 3);

	terMapPoint->UpdateMap(Vect2i::ZERO, Vect2i((int)vMap.H_SIZE, (int)vMap.V_SIZE));
}

