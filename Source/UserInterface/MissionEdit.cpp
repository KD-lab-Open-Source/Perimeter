
#include "StdAfx.h"

#include "MissionEdit.h"

#include "Config.h"
#include "Runtime.h"
#include "Terra.h"
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
	if(hardnessChanged_ && 
		MessageBox(0, "Редактирование неразрушаемости незаписано. Записать?", "Mission editor", MB_YESNO | MB_ICONQUESTION) 
			== IDYES)
				vMap.saveHardness();
}

void MissionEditor::quant()
{
	terUnitBase* unit = universe()->selectedObject();
	if(unit){
		MTAutoSkipAssert auto_skip;
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
	MTAutoSkipAssert mtAutoSkipAssert;

	switch(Key.fullkey)
	{
	case 'Q':
	case '1':
		setPlayer(0);
		return true;
	case VK_TILDE:
	case 'W':
		setPlayer(-1);
		return true;
	case 'E':
	case '2':
		setPlayer(1);
		return true;
	case '3':
		setPlayer(2);
		return true;
	case '4':
		setPlayer(3);
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
		
	case VK_DELETE:
		if(editingHardness_){
			if(MessageBox(0, "Стереть всю неразрушаемость?", "Mission editor", MB_YESNO | MB_ICONQUESTION) == IDYES)
				clearHardness();
		}
		else if(universe()->selectedObject() && ::MessageBox(0, "Удалить выделенный объект?", "Mission editor", MB_YESNO | MB_ICONQUESTION) == IDYES){
			universe()->DeleteSelectedObjects();
			_pUnitHover = 0;
		}
		return true;

	case 'D':
		if(universe()->selectedObject() && ::MessageBox(0, "Удалить выделенный объект?", "Mission editor", MB_YESNO | MB_ICONQUESTION) == IDYES){
			universe()->DeleteSelectedObjects();
			_pUnitHover = 0;
		}
		return true;

	case 'D' | KBD_SHIFT:
		universe()->DeleteSelectedObjects();
		_pUnitHover = 0;
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
				terRenderDevice->Flush(hWndVisGeneric);
				ShowCursor(1);
				ShareHandle<SaveUnitData> data = unit->universalSave(0);
				EditArchive editArchive(hWndVisGeneric, TreeControlSetup(0, 0, 500, 400, "editObjectPropsSetup"));
				if(editArchive.edit(data)){
					CSELECT_AUTOLOCK();
					const UnitList& select_list = universe()->select.GetSelectList();
					UnitList::const_iterator ui;
					FOR_EACH(select_list, ui){
						terUnitBase* unit1 = *ui;
						if(unit1->attr().ID == unit->attr().ID){
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
				terCamera->setFocus(HardwareCameraFocus);
				ShowCursor(0);				
				RestoreFocus();
				return true; 
			}
		}
		break;

	case VK_ESCAPE: 
	case VK_ESCAPE | KBD_SHIFT:
		if(editingHardness_)
			hardnessPolygon_.clear();
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

	vector<const char*> filth;
	for(int i=0;i<sz;i++)
		filth.push_back(name[i].name);

	const char* choose=popupMenu(filth);

	if(choose)
	for(int i=0;i<sz;i++)
	if(name[i].name==choose)
	{
		return name[i].id;
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

	vector<const char*> filth;
	for(int i=0;i<sz;i++)
		filth.push_back(name[i].name);

	const char* choose=popupMenu(filth);

	if(choose)
	for(int i=0;i<sz;i++)
	if(name[i].name==choose)
	{
		return name[i].id;
	}

	return UNIT_ATTRIBUTE_NONE;
}

void MissionEditor::createUnit()
{
	terUnitAttributeID attributeID = UNIT_ATTRIBUTE_NONE;
	const char* modelDirectory = 0;

	const char* itemBuildings = "Здания";
	const char* itemNature = "Деревья";
	const char* itemFilth = "Скверна";
	const char* itemSensors = "Сенсора";
	const char* itemGeoprocess = "Геопроцессы";
	vector<const char*> items;
	items.push_back(itemBuildings);
	items.push_back(itemNature);
	items.push_back(itemFilth);
	items.push_back(itemSensors);
	items.push_back(itemGeoprocess);
	const char* item = popupMenu(items);
	if(item == itemBuildings){
		terPlayer* player = universe()->activePlayer();
		items.clear();
		items.push_back(player->unitAttribute((terUnitAttributeID)UNIT_ATTRIBUTE_FRAME)->internalName());
		for(int i = 0; i < UNIT_ATTRIBUTE_MAX; i++){
			terUnitAttributeID id = (terUnitAttributeID)i;
			if(isBuilding(id) || isLegionary(id))
				items.push_back(player->unitAttribute(id)->internalName());
		}
		const char* item = popupMenu(items);
		if(item)
			attributeID = (terUnitAttributeID)getEnumDescriptor(UNIT_ATTRIBUTE_NONE).keyByNameAlt(item);
	}
	else if(item == itemNature){
		setPlayer(-1);
		attributeID = UNIT_ATTRIBUTE_STATIC_NATURE;
		modelDirectory = "Resource\\Models\\Environment";
	}
	else if(item == itemFilth){
		setPlayer(-1);
		attributeID = UNIT_ATTRIBUTE_FILTH_SPOT;
	}
	else if(item == itemSensors){
		setPlayer(-1);
		items.clear();
		const char* itemAlphaPotential = "Полюса для Альфы";
		items.push_back(itemAlphaPotential);
		const char* item = popupMenu(items);
		if(item == itemAlphaPotential)
			attributeID = UNIT_ATTRIBUTE_ALPHA_POTENTIAL;
	}else if(item == itemGeoprocess)
	{
		Vect3f v;
		if(universe()->worldPlayer() && terCamera->cursorTrace(gameShell->mousePosition(),v))
		{
			terUnitAttributeID SelectGeo();
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
			string modelName;			
			if(openFileDialog(modelName, modelDirectory, "m3d", "3D Model")){
				strlwr((char*)modelName.c_str());
				size_t pos = modelName.rfind("resource\\");
				if(pos != string::npos)
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
	info_.init();

	string missionName = gameShell->CurrentMission.saveName();
	size_t pos = missionName.rfind("RESOURCE\\");
	if(pos != string::npos)
		missionName.erase(0, pos);
	info_ < "Миссия: " < missionName.c_str() < "\n";

	terPlayer* player = universe()->activePlayer();
	if(player->isWorld())
		info_ < "Игрок \"Мир\"\n";
	else 
		switch(player->playerID()){
		case 0:
			info_ < "Игрок1 \"Я\"\n";
			break;
		case 1:
			info_ < "Игрок2 \"Враг\"\n";
			break;
		default:
			info_ < "Игрок" <= player->playerID() < "\n";
		}

	terUnitBase* unit = universe()->selectedObject();
	if(unit){
		info_ < "Объект: " < unit->attr().internalName() < "\n";
		if(unit->GetModelName())
			info_ < "Модель: " < unit->GetModelName() < "\n";
	}

	if(copiedData_)
		info_ < "Запомнен: " < getEnumDescriptor(UNIT_ATTRIBUTE_NONE).nameAlt(copiedData_->attributeID) < "\n";

	if(editingHardness_)
		info_ < (!isShiftPressed() ? "Редактирование некопаемых областей\n" : "Редактирование копаемых областей\n");

	if(terCamera->pathSize())
		info_ < (terCamera->restricted() ? "Camera: " : "Camera (ограничена): ") <= terCamera->pathSize() < "\n";

	return info_.address();
}

void MissionEditor::copyUnit()
{
	terUnitBase* unit = universe()->selectedObject();
	if(!unit)
		return;

	copiedID_ = unit->attr().ID;
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
		vector<Vect2i> points;
		Vect2i posMin(10000, 10000);
		Vect2i posMax(0, 0);
		Vect3fVect::iterator vi;
		FOR_EACH(hardnessPolygon_, vi){
			Vect2i v = *vi;
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

		scanPolyByLineOp(&points[0], points.size(), ApplyHardnessLineOp(!isShiftPressed()));

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

