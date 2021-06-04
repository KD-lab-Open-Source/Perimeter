#include "stdafx.h"

#include "EditArchive.h"
#include "XPrmArchive.h"
#include "BinaryArchive.h"

#include "RunTime.h"
#include "Save.h"
#include "qd_textdb.h"
#include "UnitAttribute.h"
#include "SoundScript.h"
#include "InterfaceScript.h"

///////////////////////////////////////////////////////////////
class EffectLibraryDispatcher
{
public:
	EffectLibraryDispatcher(){ 
		set_textures_path("RESOURCE\\FX\\TEXTURES");
	}
	~EffectLibraryDispatcher(){ }

	/// добавляет библиотеку эффектов и загружает ее
	const EffectLibrary* register_library(const char* lib_name)
	{
		EffectLibraryContainer::iterator it = libraries_.find(lib_name);
		if(it != libraries_.end())
			return &it -> second;

		EffectLibrary lib;
		libraries_.insert(EffectLibraryContainer::value_type(lib_name,lib));

		it = libraries_.find(lib_name);
		EffectLibrary* lp = &it -> second;

		const char* path = (textures_path_.empty()) ? NULL : textures_path_.c_str();

		if(!lp -> Load(lib_name,path)){
			libraries_.erase(it);
			return NULL;
		}

		return lp;
	}

	/// устанавливает путь к текстурам
	void set_textures_path(const char* path){ textures_path_ = path; }

private:
	typedef map<string,EffectLibrary> EffectLibraryContainer;
	EffectLibraryContainer libraries_;

	/// относительный путь к текстурам
	string textures_path_;
};

Singleton<EffectLibraryDispatcher> effectLibraryDispatcher;

///////////////////////////////////////////////////////////////

RandomGenerator terEffectRND;

float AttributeBase::energyPerElement_ = 0;

SINGLETON_PRM(AttributeLibrary, "AttributeLibrary", "Scripts\\AttributeLibrary") attributeLibrary;

REGISTER_CLASS(AttributeBase, AttributeBase, "Базовые свойства")

AttributeBase::AttributeBase() : 
EffectLib(0)
{ 
	ID = UNIT_ATTRIBUTE_NONE;
	ClassID = UNIT_CLASS_ID_NONE;
	InterpolationType = INTERPOLATION_ID_NONE;
	belligerent = BELLIGERENT_NONE; 

	InterfaceNameTag = "None";

	SelectAble = 0;
	saveAsCommonObject = 0; 
	
	GroundPlaneSpeed = 0;
	GroundAngleSpeed = 0;
	
	MilitaryUnit = 0;
	UnitClass = UNIT_CLASS_IGNORE;
	AttackClass = UNIT_CLASS_IGNORE;
	enemyWorld = 0; // Оъекты, принадлежащие миру, будут атаковаться. Скверна.

	SoundCycled = 1;

	InstallBound = 0;
	lockInitialPosition = 0;

	showPath = 0;

	ExcludeCollision = 0;
	CollisionGroup = 0;

	MakeEnergy = 0; // в секунду
	energyCapacity = 0;

	ConnectionRadius = 0;
	ZeroLayerRadius = 0;
	FieldRadius = 0;

	LifeTime = 0;

	constructionSpeedCoeff = 1.0f;

	SelectionDistance = 1.0f;
	SelectionSize = 1.0f;

	MakingChainNum = -1;
	UpgradeChainName = "making";
	PrevChainName = "main";
	IgnoreTargetTrace = 0;

	ShowCircles = 1;

	enablePathFind = 0;
	enableFieldPathFind = 0;
	considerInFieldPathFind = 1;

	kill_priority = 1;

	isUpgrade = false;

	iconDistanceFactor = 1.1f;
	
	BasementMin.x = BasementMin.y = 0;
	BasementMax.x = BasementMin.y = 0;

	forwardVelocity = 0;
	flyingHeight = 0;
}

void AttributeBase::init()
{
	if(strlen(interfaceNameTag())) {
		string path = string("Interface.Tips.NAMES.") + interfaceNameTag();
		InterfaceName = qdTextDB::instance().getText(path.c_str());
		if(InterfaceName.empty())
			InterfaceName = interfaceNameTag();
	}

	EffectLib = effectsData.effects.size() ? effectLibraryDispatcher().register_library(effectsData.libraryFileName) : 0;

	initGeometryAttribute(modelData, *this);
}

EffectKey* AttributeBase::getEffect(terEffectID effect_id) const
{
	if(EffectLib){
		for(int i = 0; i < effectsData.effects.size(); i++){
			if(effectsData.effects[i].effectID == effect_id)
				return EffectLib->Get(effectsData.effects[i].effectName);
		}
	}

	return NULL;
}

EffectKey* AttributeBase::getEffect(const char* effect_name) const
{
	if(EffectLib)
		return EffectLib->Get(effect_name);

	return NULL;
}

const terUnitEffectData* AttributeBase::getEffectData(terEffectID effect_id) const
{
	if(EffectLib){
		for(int i = 0; i < effectsData.effects.size(); i++){
			if(effectsData.effects[i].effectID == effect_id)
				return &effectsData.effects[i];
		}
	}

	return NULL;
}

GeometryAttribute::GeometryAttribute()
{
	boundRadius = boundRadiusOriginal = 1;
	BasementMin.x = BasementMin.y = 0;
	BasementMax.x = BasementMax.y = 0;
	BasementInscribedRadius = 0;
	BasementPoints.clear();

	modelScale = 1;
	logicObjectBoundOriginal.min = Vect3f::ZERO;
	logicObjectBoundOriginal.max = Vect3f::ZERO;
	logicObjectBound = logicObjectBoundOriginal;
}

void GeometryAttribute::initGeometryAttribute(const ModelData& modelData, const AttributeBase& attribute)
{
	float modelScaleOld = modelScale;

    if(!modelData.logicName)
		return;

	if(!(logicTime == FileTime(modelData.logicName))){
		logicTime = FileTime(modelData.logicName);
		cLogicObject* logic = terLogicGeneric->GetElement(modelData.logicName);
		xassert(logic);
		logic->SetPosition(MatXf::ID);
		logic->SetScale(Vect3f(1,1,1));
		logic->Update();
	
		boundRadiusOriginal = logic->GetBoundRadius();
		logic->GetBoundBox(logicObjectBoundOriginal);

		logic->Release();
	}

	modelScale = 1;
	if(attribute.ID == UNIT_ATTRIBUTE_FRAME || attribute.ID == UNIT_ATTRIBUTE_CORRIDOR_ALPHA || attribute.ID == UNIT_ATTRIBUTE_CORRIDOR_OMEGA)
		modelScale *= debuScales.frame;
	else if(attribute.MilitaryUnit)
		modelScale *= debuScales.legion;
	else if(attribute.isBuilding())
		modelScale *= debuScales.buildins;
	else
		modelScale *= debuScales.other;

	if(!modelData.boundRadius){
		modelScale *= modelData.boundScale;
		boundRadius = boundRadiusOriginal*modelScale;
	}
	else{
		boundRadius = modelData.boundRadius*modelScale;
		modelScale = boundRadius/boundRadiusOriginal;
	}

	logicObjectBound = logicObjectBoundOriginal;
	logicObjectBound.min *= modelScale;
	logicObjectBound.max *= modelScale;

	Vect3f deltaBound = logicObjectBound.max - logicObjectBound.min;
	xassert_s(deltaBound.x > FLT_MIN && deltaBound.y > FLT_MIN && deltaBound.z > FLT_MIN && "Zero size bound", modelData.logicName);

	if(attribute.InstallBound && !(modelTime == FileTime(modelData.modelName) || modelScaleOld != modelScale)){
		modelTime = FileTime(modelData.modelName);

		cObjectNodeRoot* model = createObject(modelData.modelName, attribute.belligerent);

		int vertex_num = 0;
		int index_num = 0;
		Vect3f* vertices = 0;
		short* indices = 0;
		
		model->GetLocalBorder(&vertex_num, &vertices, &index_num, &indices);

		if(vertex_num){
			Vect2fVect points;
			points.resize(vertex_num);
			for(int i = 0;i < points.size();i++){
				points[i] = Vect2f(vertices[i]);
				points[i] *= modelScale;
			}

			// 1. Make arbitrary cw-triangle
			xassert(points.size() >= 3);
			Vect2f& p0 = points[0];
			Vect2f& p1 = points[1];
			Vect2f& p2 = points[2];

			if((p1 - p0) % (p2 - p0) > 0)
				swap(p1, p2);

			BasementPoints.clear();
			BasementPoints.push_back(p0);
			BasementPoints.push_back(p1);
			BasementPoints.push_back(p2);

			// 2. Add point lieing outside
			for(Vect2fVect::iterator pi = points.begin() + 3; pi != points.end(); ++pi){
				Vect2f p = *pi;
				Vect2f p0 = BasementPoints.back();
				Vect2fVect::iterator pj;
				FOR_EACH(BasementPoints, pj){
					Vect2f p1 = *pj;
					Vect2f d = p1 - p0;
					Vect2f n(-d.y, d.x); // outer 
					if(dot(p - p0, n) > 0){ // Outside
						BasementPoints.insert(pj, p); // inseert between p0 and p1
						// Check if some corners become concave
						Vect2fVect::iterator pk;
						FOR_EACH(BasementPoints, pk){
							Vect2f p0 = pk != BasementPoints.begin() ? *(pk - 1) : BasementPoints.back();
							Vect2f p1 = *pk;
							Vect2f p2 = pk != BasementPoints.end() - 1 ? *(pk + 1) : BasementPoints.front();
							if((p0 - p1) % (p2 - p1) < 0){
								pk = BasementPoints.erase(pk);
								--pk;
							}
						}
						break;
					}
					p0 = p1;
				}
			}

			xassert(BasementPoints.size() >= 4);

			// Find min/max
			BasementMin = BasementMax = BasementPoints[0];
			Vect2fVect::iterator pj;
			FOR_EACH(BasementPoints, pj){
				Vect2f& v = *pj;
				if(v.x < BasementMin.x)
					BasementMin.x = v.x;
				if(v.x > BasementMax.x)
					BasementMax.x = v.x;
				if(v.y < BasementMin.y)
					BasementMin.y = v.y;
				if(v.y > BasementMax.y)
					BasementMax.y = v.y;
			}

			BasementMin.x = BasementMin.x - 1;
			BasementMax.x = BasementMax.x + 1;
			BasementMin.y = BasementMin.y - 1;
			BasementMax.y = BasementMax.y + 1;
			BasementInscribedRadius = min(min(fabsf(BasementMin.x), fabsf(BasementMin.y)), min(BasementMax.x, BasementMax.y));
		}

		model->Release();
	}
}

string GetBelligerentTexturePath(terBelligerent belligerent)
{
	string path = "Resource\\Models\\Main\\";
	switch(belligerent){
	case BELLIGERENT_NONE:
	case BELLIGERENT_EXODUS0:
	case BELLIGERENT_EXODUS1:
	case BELLIGERENT_EXODUS2:
	case BELLIGERENT_EXODUS3:
	case BELLIGERENT_EXODUS4:
		path += "EXODUS\\";
		break;
	case BELLIGERENT_HARKBACKHOOD0:
	case BELLIGERENT_HARKBACKHOOD1:
		path += "HARKBACKHOOD\\";
		break;
	case BELLIGERENT_EMPIRE0:
	case BELLIGERENT_EMPIRE1:
	case BELLIGERENT_EMPIRE2:
	case BELLIGERENT_EMPIRE3:
	case BELLIGERENT_EMPIRE4:
	case BELLIGERENT_EMPIRE_VICE:
		path += "EMPIRE\\";
		break;
	}
	return path;
}

cObjectNodeRoot* createObject(const char* name, terBelligerent belligerent)
{
	string path=GetBelligerentTexturePath(belligerent);
	cObjectNodeRoot* model = terScene->CreateObject(name, path.c_str());
	xassert(model);
	return model;
}

//--------------------------------------------

int DamageMolecula::softAtomCount_[DAMAGE_ELEMENT_TYPE_MAX] = { 2, 3, 1, 4, 0 }; 
#ifdef _DAMAGE_MODEL_DEBUG_
LogStream DamageMolecula::log_("damage.log",XS_OUT);
#endif

DamageMolecula::DamageMolecula() : isAlive_(true)
{
	for(int i = 0; i < DAMAGE_ELEMENT_TYPE_MAX; i++){
		elements_[i] = 0;
		elementsDead_[i] = 0;
	}
}

DamageMolecula::DamageMolecula(int s,int o,int t) : isAlive_(true)
{
	for(int i = 0; i < DAMAGE_ELEMENT_TYPE_MAX; i++){
		elements_[i] = 0;
		elementsDead_[i] = 0;
	}

	elements_[0] = s;
	elements_[1] = o;
	elements_[2] = t;
}

int DamageMolecula::elementCount(int element_filter) const
{
	int count = 0;

	for(int i = 0; i < DAMAGE_ELEMENT_TYPE_MAX; i++){
		if(element_filter & (1 << i))
			count += elements_[i];
	}

	return count;
}

int DamageMolecula::aliveElementCount(int element_filter) const
{
	int count = 0;

	for(int i = 0; i < DAMAGE_ELEMENT_TYPE_MAX; i++){
		if(element_filter & (1 << i))
			count += elements_[i] - elementsDead_[i];
	}

	return count;
}

int DamageMolecula::deadElementCount(int element_filter) const
{
	int count = 0;

	for(int i = 0; i < DAMAGE_ELEMENT_TYPE_MAX; i++){
		if(element_filter & (1 << i))
			count += elementsDead_[i];
	}

	return count;
}

int DamageMolecula::softAtomCount(int element_filter) const 
{
	int count = 0;
	for(int i = 0; i < DAMAGE_ELEMENT_TYPE_MAX; i++){
		if(element_filter & (1 << i))
			count += aliveElementCount(1 << i) * softAtomCount_[i];
	}

	return count;
}

int DamageMolecula::hardAtomCount(int element_filter) const
{
	int count = 0;
	for(int i = 0; i < DAMAGE_ELEMENT_TYPE_MAX; i++){
		if(element_filter & (1 << i))
			count += aliveElementCount(1 << i) * (4 - softAtomCount_[i]);
	}

	return count;
}

int DamageMolecula::repair(int element_count)
{
	int elements_dead = deadElementCount();

	if(!elements_dead) return 0;

	if(element_count > elements_dead)
		element_count = elements_dead;

	for(int i = 0; i < element_count; i++){
		int skip_count = terLogicRND(elements_dead);

		int count = 0;

		int type = 0;
		for(type = 0; type < DAMAGE_ELEMENT_TYPE_MAX; type++){
			count += elementsDead_[type];
			if(count > skip_count) break;
		}

		xassert(elementsDead_[type]);

		elementsDead_[type]--;

		elements_dead--;
	}

	isAlive_ = true;

	return element_count;
}

void DamageMolecula::repair(DamageElementType element_type,int element_count)
{
	if(elementsDead_[element_type] < element_count)
		element_count = elementsDead_[element_type];

	if(element_count){
		elementsDead_[element_type] -= element_count;
		isAlive_ = true;
	}
}

int DamageMolecula::hit(int attack_width,int attack_power,int attack_filter,int damage_filter,int shield)
{
	if(!attack_width || !attack_power) return 0;

	int hard_atoms = hardAtomCount(attack_filter);
	int soft_atoms = 0;

	if(attack_width > hard_atoms){
		soft_atoms = softAtomCount(attack_filter);
		if(hard_atoms + soft_atoms > attack_width)
			soft_atoms = attack_width - hard_atoms;

		if(shield > soft_atoms)
			shield = soft_atoms;

		soft_atoms -= shield;
		hard_atoms += shield;
	}
	else {
		if(hard_atoms > attack_width)
			hard_atoms = attack_width;
	}

	int hit_count = 0;

	for(int i = 0; i < attack_power; i++){
		int atom_index = terLogicRND(hard_atoms + soft_atoms);

		if(atom_index >= hard_atoms){
			hit_count++;
			soft_atoms--;
		}
		else {
			hard_atoms--;
			soft_atoms++;
		}
	}

#ifdef _DAMAGE_MODEL_DEBUG_
	log_ < "attack_width: " <= attack_width < " attack_power: " <= attack_power < " hit_count " <= hit_count < "\r\n";

	for(i = 0; i < DAMAGE_ELEMENT_TYPE_MAX; i++)
		log_ < " " <= elements_[i] - elementsDead_[i];
	log_ < "\r\n";

	int count = killElements((hit_count + 3) / 4,damage_filter);

	for(i = 0; i < DAMAGE_ELEMENT_TYPE_MAX; i++)
		log_ < " " <= elements_[i] - elementsDead_[i];
	log_ < "\r\n\r\n";

	return count;
#else
	return killElements((hit_count + 3) / 4,damage_filter);
#endif
}

int DamageMolecula::killElements(int element_count,int element_filter)
{
	xassert(element_filter);
	if(!element_count || !element_filter) return 0;

	int elements_alive = aliveElementCount(element_filter); 
	int elements_killed = 0;
	if(element_count >= elements_alive){
		for(int i = 0; i < DAMAGE_ELEMENT_TYPE_MAX; i++){
			if(element_filter & (1 << i))
				elementsDead_[i] = elements_[i];
		}

		elements_killed = elements_alive;
	}
	else {
		for(int i = 0; i < element_count; i++){
			int index = terLogicRND(DAMAGE_ELEMENT_TYPE_MAX);
			while(!(element_filter & (1 << index)) || elements_[index] - elementsDead_[index] <= 0)
				index = terLogicRND(DAMAGE_ELEMENT_TYPE_MAX);

			elementsDead_[index]++;
		}

		elements_killed = element_count;
	}

	if(!aliveElementCount())
		isAlive_ = false;

	return elements_killed;
}

bool DamageMolecula::isStable() const
{
	for(int i = 0; i < DAMAGE_ELEMENT_TYPE_MAX; i++){
		if(elements_[i] && elements_[i] - elementsDead_[i] <= 0)
			return false;
	}

	return true;
}

const SaveDamageMolecula& SaveDamageMolecula::operator=(const DamageMolecula& data)
{
	isAlive = data.isAlive();
	elementsDead.clear();
	elementsDead.reserve(DAMAGE_ELEMENT_TYPE_MAX);
	for(int i = 0; i < DAMAGE_ELEMENT_TYPE_MAX; i++)
		elementsDead.push_back(data.elementsDead((DamageElementType)i));
	
	return *this;
}

const DamageMolecula& DamageMolecula::operator=(const SaveDamageMolecula& data)
{
	if(data.elementsDead.empty())
		return *this;

	isAlive_ = data.isAlive;
	for(int i = 0; i < DAMAGE_ELEMENT_TYPE_MAX; i++)
		elementsDead_[i] = data.elementsDead[i];

	return *this;
}

//---------------------------------
//void initAttributes()
//{ 
//	AttributeLibrary::
//	for(i = 0;i < table.size();i++){
//		AttributeBase& cell = data[table[i].ID];
//		if(cell.ID == UNIT_ATTRIBUTE_NONE || belligerent == table[i].belligerent)
//			cell.init(table[i]); 
//	}
//  
//	AttributeBase::setBuildCost(buildingBlockConsumption.energy*buildingBlockConsumption.time/(100*DamageMolecula(table.BuildingBlock.damageMolecula).elementCount()));
//
//	for (i = 0; i < UNIT_ATTRIBUTE_MAX; i++) {
//		AttributeBase& cell = data[i];
//		if (cell.ID != UNIT_ATTRIBUTE_NONE) {
//			cell.initIntfBalanceData((cell.weaponSetup && cell.weaponSetup->missileID != UNIT_ATTRIBUTE_NONE) ? &data[cell.weaponSetup->missileID] : 0);
//		}
//	}
//}

const char* AttributeBase::internalName() const
{
	return getEnumDescriptor(UNIT_ATTRIBUTE_NONE).nameAlt(ID);
}

bool AttributeBase::enabledByBelligerent(terBelligerent belligerentIn) const
{
	switch(belligerent){
	case BELLIGERENT_NONE:
		return true;
	case BELLIGERENT_EXODUS0:
	case BELLIGERENT_EXODUS1:
	case BELLIGERENT_EXODUS2:
	case BELLIGERENT_EXODUS3:
	case BELLIGERENT_EXODUS4:
	case BELLIGERENT_EMPIRE_VICE:
		return belligerentIn == BELLIGERENT_EXODUS0 || belligerentIn == BELLIGERENT_EXODUS1 || belligerentIn == BELLIGERENT_EMPIRE_VICE ||
			belligerentIn == BELLIGERENT_EXODUS2 || belligerentIn == BELLIGERENT_EXODUS3 || belligerentIn == BELLIGERENT_EXODUS4;
	case BELLIGERENT_HARKBACKHOOD0:
	case BELLIGERENT_HARKBACKHOOD1:
		return belligerentIn == BELLIGERENT_HARKBACKHOOD0 || belligerentIn == BELLIGERENT_HARKBACKHOOD1;
	case BELLIGERENT_EMPIRE0:
	case BELLIGERENT_EMPIRE1:
	case BELLIGERENT_EMPIRE2:
	case BELLIGERENT_EMPIRE3:
	case BELLIGERENT_EMPIRE4:
		return belligerentIn == BELLIGERENT_EMPIRE0 || belligerentIn == BELLIGERENT_EMPIRE1 || belligerentIn == BELLIGERENT_EMPIRE2 ||
			belligerentIn == BELLIGERENT_EMPIRE3 || belligerentIn == BELLIGERENT_EMPIRE4;
	default:
		xassert(0);
		return false;
	}
}

void AttributeBase::initIntfBalanceData(const AttributeBase* missile) 
{
	float basic = damageMolecula[0] + damageMolecula[1] + damageMolecula[2]; 
	if (basic) {
		intfBalanceData.armor = damageMolecula[4] / basic;
	}
	if (unitDamage.mainDamage.power) {
		intfBalanceData.power = unitDamage.mainDamage.power;
		intfBalanceData.width = unitDamage.mainDamage.width;
	} else if (unitDamage.splashDamage.power) {
		intfBalanceData.power = unitDamage.splashDamage.power;
		intfBalanceData.width = unitDamage.splashDamage.width;
	} else if (missile && missile->ID != UNIT_ATTRIBUTE_NONE) {
		if (missile->unitDamage.mainDamage.power) {
			intfBalanceData.power = missile->unitDamage.mainDamage.power;
			intfBalanceData.width = missile->unitDamage.mainDamage.width;
		} else if (missile->unitDamage.splashDamage.power) {
			intfBalanceData.power = missile->unitDamage.splashDamage.power;
			intfBalanceData.width = missile->unitDamage.splashDamage.width;
		}
	}
}

FileTime::FileTime(const char* fname)
{
	FILETIME creation, last_access;
	HANDLE file = CreateFile( fname, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0 );
	if(file==INVALID_HANDLE_VALUE)
	{
		string s="File cannot open to read: ";
		s+=fname;
		xxassert(0,s.c_str());
		dwLowDateTime = dwHighDateTime = 0;
	}

	if(!GetFileTime( file, &creation, &last_access, this))
	{
		string s="File cannot read time: ";
		s+=fname;
		xxassert(0,s.c_str());
	}

	CloseHandle(file);
}


void copyAttributes();
void copyInterfaceAttributes();
void copyRigidBodyTable();
void copyInterfaceAttributesIndispensable();

void debugStartUp()
{
	globalAttr();
	interfaceAttr();

//	soundScriptTable();
//	copyRigidBodyTable();
//	rigidBodyPrmLibrary.edit();
//	copyAttributes();
//	attributeLibrary.edit();
//	copyInterfaceAttributes();
//	interfaceAttr.edit();
//	ErrH.Exit();

	copyInterfaceAttributesIndispensable();

	const AttributeBase* blockAttr = attributeLibrary().find(UNIT_ATTRIBUTE_BUILDING_BLOCK); 
    AttributeBase::setBuildCost(buildingBlockConsumption.energy*buildingBlockConsumption.time/(10*DamageMolecula(blockAttr->damageMolecula).elementCount()));

	AttributeLibrary::Map::iterator i;
	FOR_EACH(attributeLibrary().map(), i){
		AttributeBase* attribute = i->second;
		if(attribute->ID != UNIT_ATTRIBUTE_NONE)
			attribute->initIntfBalanceData((attribute->weaponSetup.missileID != UNIT_ATTRIBUTE_NONE) ? attributeLibrary().find(AttributeIDBelligerent(attribute->weaponSetup.missileID)) : 0);
	}
}

/////////////////////////////////////////
//		Global parameters
/////////////////////////////////////////
ConsumptionData buildingBlockConsumption;
FieldPrm fieldPrm;
DebugScales debuScales; // Отладочное масштабирование
DifficultyPrm difficultyPrmArray[DIFFICULTY_MAX];
TrucksIntrumentParameter trucksIntrumentParameter;
SoundEventsPrm soundEventsPrm;
BelligerentPropertyTable belligerentPropertyTable;
SoundSetup playerSound;
SINGLETON_PRM(GlobalAttributes, "GlobalAttributes", "Scripts\\GlobalAttributes")  globalAttr;

BEGIN_ENUM_DESCRIPTOR_ENCLOSED(RigidBodyPrm, RigidBodyType, "RigidBodyType")
REGISTER_ENUM_ENCLOSED(RigidBodyPrm, UNIT, "UNIT");
REGISTER_ENUM_ENCLOSED(RigidBodyPrm, MISSILE, "MISSILE");
REGISTER_ENUM_ENCLOSED(RigidBodyPrm, ROCKET, "ROCKET");
REGISTER_ENUM_ENCLOSED(RigidBodyPrm, DEBRIS, "DEBRIS");
END_ENUM_DESCRIPTOR_ENCLOSED(RigidBodyPrm, RigidBodyType)

BEGIN_ENUM_DESCRIPTOR(ToolzerActionID, "ToolzerActionID")
REGISTER_ENUM(TOOLZER_NONE, "никакого воздействия");
REGISTER_ENUM(TOOLZER_4ZP, "toolzerAligmentTerrain4ZP");
REGISTER_ENUM(TOOLZER_VH, "toolzerAligmentTerrainVariableH");
REGISTER_ENUM(TOOLZER_H, "toolzerChangeTerHeight");
REGISTER_ENUM(TOOLZER_NZP, "toolzerChangeTerHeightIfNotZP");
REGISTER_ENUM(TOOLZER_VLD, "toolzerPlottingVeryLightDam");
REGISTER_ENUM(TOOLZER_S, "toolzerPlotingSoot");
REGISTER_ENUM(TOOLZER_EZ, "toolzerEraseZP");
END_ENUM_DESCRIPTOR(ToolzerActionID)

BEGIN_ENUM_DESCRIPTOR(ToolzerBuildingDamageMode, "ToolzerBuildingDamageMode")
REGISTER_ENUM(BUILDING_DAMAGE_CIRCLE, "BUILDING_DAMAGE_CIRCLE");
REGISTER_ENUM(BUILDING_DAMAGE_SQUARE, "BUILDING_DAMAGE_SQUARE");
END_ENUM_DESCRIPTOR(ToolzerBuildingDamageMode)

BEGIN_ENUM_DESCRIPTOR(ToolzerPhaseID, "ToolzerPhaseID")
REGISTER_ENUM(TOOLZER_PHASE_NONE, "TOOLZER_PHASE_NONE");
REGISTER_ENUM(TOOLZER_PHASE_DEFAULT, "универсальное имя");
REGISTER_ENUM(TOOLZER_PHASE_START_MOVE, "включается в момент начала движения");
REGISTER_ENUM(TOOLZER_PHASE_MOVE, "включается во время движения");
REGISTER_ENUM(TOOLZER_PHASE_END_MOVE, "включается в момент остановки");
END_ENUM_DESCRIPTOR(ToolzerPhaseID)

BEGIN_ENUM_DESCRIPTOR(PopupFormatGroup, "PopupFormatGroup")
REGISTER_ENUM(POPUP_FORMAT_NONE, "POPUP_FORMAT_NONE");
REGISTER_ENUM(POPUP_FORMAT_BUILDING, "POPUP_FORMAT_BUILDING");
REGISTER_ENUM(POPUP_FORMAT_CORE, "POPUP_FORMAT_CORE");
REGISTER_ENUM(POPUP_FORMAT_AMPLIFIER, "POPUP_FORMAT_AMPLIFIER");
REGISTER_ENUM(POPUP_FORMAT_FRAME, "POPUP_FORMAT_FRAME");
REGISTER_ENUM(POPUP_FORMAT_GUN, "POPUP_FORMAT_GUN");
REGISTER_ENUM(POPUP_FORMAT_SQUAD, "POPUP_FORMAT_SQUAD");
REGISTER_ENUM(POPUP_FORMAT_MMP, "POPUP_FORMAT_MMP");
END_ENUM_DESCRIPTOR(PopupFormatGroup)

BEGIN_ENUM_DESCRIPTOR(ChainNodeID, "ChainNodeID")
REGISTER_ENUM(CHAIN_NODE_NONE, "CHAIN_NODE_NONE");
REGISTER_ENUM(CHAIN_NODE_DEFAULT, "CHAIN_NODE_DEFAULT");
REGISTER_ENUM(CHAIN_NODE_DEFAULT_OTHER, "CHAIN_NODE_DEFAULT_OTHER");
REGISTER_ENUM(CHAIN_NODE_SWITCHED_ON, "CHAIN_NODE_SWITCHED_ON");
REGISTER_ENUM(CHAIN_NODE_SWITCHED_OFF, "CHAIN_NODE_SWITCHED_OFF");
REGISTER_ENUM(CHAIN_NODE_OPENED, "CHAIN_NODE_OPENED");
REGISTER_ENUM(CHAIN_NODE_CLOSED, "CHAIN_NODE_CLOSED");
REGISTER_ENUM(CHAIN_NODE_UPGRADE_START, "CHAIN_NODE_UPGRADE_START");
REGISTER_ENUM(CHAIN_NODE_BUILD0, "CHAIN_NODE_BUILD0");
REGISTER_ENUM(CHAIN_NODE_BUILD1, "CHAIN_NODE_BUILD1");
REGISTER_ENUM(CHAIN_NODE_BUILD2, "CHAIN_NODE_BUILD2");
REGISTER_ENUM(CHAIN_NODE_BUILD3, "CHAIN_NODE_BUILD3");
REGISTER_ENUM(CHAIN_NODE_BUILD4, "CHAIN_NODE_BUILD4");
REGISTER_ENUM(CHAIN_NODE_BUILD5, "CHAIN_NODE_BUILD5");
REGISTER_ENUM(CHAIN_NODE_BUILD6, "CHAIN_NODE_BUILD6");
REGISTER_ENUM(CHAIN_NODE_BUILD7, "CHAIN_NODE_BUILD7");
REGISTER_ENUM(CHAIN_NODE_BUILD8, "CHAIN_NODE_BUILD8");
REGISTER_ENUM(CHAIN_NODE_CHARGED, "CHAIN_NODE_CHARGED");
REGISTER_ENUM(CHAIN_NODE_DISCHARGED, "CHAIN_NODE_DISCHARGED");
REGISTER_ENUM(CHAIN_NODE_FIRE, "CHAIN_NODE_FIRE");
REGISTER_ENUM(CHAIN_NODE_DETACHED, "CHAIN_NODE_DETACHED");
REGISTER_ENUM(CHAIN_NODE_ATTACHED, "CHAIN_NODE_ATTACHED");
REGISTER_ENUM(CHAIN_NODE_DETACHED_OPENED, "CHAIN_NODE_DETACHED_OPENED");
REGISTER_ENUM(CHAIN_NODE_ATTACHED_OPENED, "CHAIN_NODE_ATTACHED_OPENED");
REGISTER_ENUM(CHAIN_NODE_DEAD, "CHAIN_NODE_DEAD");
REGISTER_ENUM(CHAIN_NODE_GROUND, "CHAIN_NODE_GROUND");
REGISTER_ENUM(CHAIN_NODE_FLIGHT, "CHAIN_NODE_FLIGHT");
REGISTER_ENUM(CHAIN_NODE_UNDERGROUND, "CHAIN_NODE_UNDERGROUND");
END_ENUM_DESCRIPTOR(ChainNodeID)

BEGIN_ENUM_DESCRIPTOR(AnimationGroupID, "AnimationGroupID")
REGISTER_ENUM(ANIMATION_GROUP_ROOT, "ANIMATION_GROUP_ROOT");
REGISTER_ENUM(ANIMATION_GROUP_CITY, "ANIMATION_GROUP_CITY");
REGISTER_ENUM(ANIMATION_GROUP_PILLAR, "ANIMATION_GROUP_PILLAR");
REGISTER_ENUM(ANIMATION_GROUP_ANIMATE, "ANIMATION_GROUP_ANIMATE");
REGISTER_ENUM(ANIMATION_GROUP_LIGHTING, "ANIMATION_GROUP_LIGHTING");
REGISTER_ENUM(ANIMATION_GROUP_CHARGING, "ANIMATION_GROUP_CHARGING");
REGISTER_ENUM(ANIMATION_GROUP_FIRE, "ANIMATION_GROUP_FIRE");
END_ENUM_DESCRIPTOR(AnimationGroupID)

BEGIN_ENUM_DESCRIPTOR(SoundEventID, "SoundEventID")
REGISTER_ENUM(SOUND_EVENT_NONE, "SOUND_EVENT_NONE");
REGISTER_ENUM(SOUND_EVENT_EXPLOSION, "SOUND_EVENT_EXPLOSION");
REGISTER_ENUM(SOUND_EVENT_SELECT, "SOUND_EVENT_SELECT");
REGISTER_ENUM(SOUND_EVENT_SELECT_ALTERNATE, "SOUND_EVENT_SELECT_ALTERNATE");
REGISTER_ENUM(SOUND_EVENT_OPEN, "SOUND_EVENT_OPEN");
REGISTER_ENUM(SOUND_EVENT_BUILDING_READY, "SOUND_EVENT_BUILDING_READY");
REGISTER_ENUM(SOUND_EVENT_BUILDING_HOLD_CONSTRUCTION, "SOUND_EVENT_BUILDING_HOLD_CONSTRUCTION");
REGISTER_ENUM(SOUND_EVENT_BUILDING_CONNECTED, "SOUND_EVENT_BUILDING_CONNECTED");
REGISTER_ENUM(SOUND_EVENT_BUILDING_DISCONNECTED, "SOUND_EVENT_BUILDING_DISCONNECTED");
REGISTER_ENUM(SOUND_VOICE_SQUAD_SELECTED, "SOUND_VOICE_SQUAD_SELECTED");
REGISTER_ENUM(SOUND_VOICE_SQUAD_MOVES, "SOUND_VOICE_SQUAD_MOVES");
REGISTER_ENUM(SOUND_VOICE_SQUAD_ATTACKS, "SOUND_VOICE_SQUAD_ATTACKS");
REGISTER_ENUM(SOUND_VOICE_SQUAD_UNDER_ATTACK, "SOUND_VOICE_SQUAD_UNDER_ATTACK");
REGISTER_ENUM(SOUND_VOICE_SQUAD_TRANSFORM_STARTED, "SOUND_VOICE_SQUAD_TRANSFORM_STARTED");
REGISTER_ENUM(SOUND_VOICE_SQUAD_TRANSFORM_FINISHED, "SOUND_VOICE_SQUAD_TRANSFORM_FINISHED");
REGISTER_ENUM(SOUND_VOICE_MMP_CONVERT_COMMAND, "SOUND_VOICE_MMP_CONVERT_COMMAND");
REGISTER_ENUM(SOUND_VOICE_TERRAFORMING_STARTED, "SOUND_VOICE_TERRAFORMING_STARTED");
REGISTER_ENUM(SOUND_VOICE_TERRAFORMING_FINISHED, "SOUND_VOICE_TERRAFORMING_FINISHED");
REGISTER_ENUM(SOUND_VOICE_ENERGY_COLLECTOR_EMPTY, "SOUND_VOICE_ENERGY_COLLECTOR_EMPTY");
REGISTER_ENUM(SOUND_VOICE_ENERGY_COLLECTOR_FULL, "SOUND_VOICE_ENERGY_COLLECTOR_FULL");
REGISTER_ENUM(SOUND_VOICE_ENERGY_NOT_ENOUGH, "SOUND_VOICE_ENERGY_NOT_ENOUGH");
REGISTER_ENUM(SOUND_VOICE_ENERGY_LOSING, "SOUND_VOICE_ENERGY_LOSING");
REGISTER_ENUM(SOUND_VOICE_ENERGY_RESUPPLY, "SOUND_VOICE_ENERGY_RESUPPLY");
REGISTER_ENUM(SOUND_VOICE_PSHIELD_GLOBAL_ACTIVE, "SOUND_VOICE_PSHIELD_GLOBAL_ACTIVE");
REGISTER_ENUM(SOUND_VOICE_PSHIELD_GLOBAL_OFF, "SOUND_VOICE_PSHIELD_GLOBAL_OFF");
REGISTER_ENUM(SOUND_VOICE_PSHIELD_LOCAL_ACTIVE, "SOUND_VOICE_PSHIELD_LOCAL_ACTIVE");
REGISTER_ENUM(SOUND_VOICE_PSHIELD_LOCAL_OFF, "SOUND_VOICE_PSHIELD_LOCAL_OFF");
REGISTER_ENUM(SOUND_VOICE_FRAME_ALARM, "SOUND_VOICE_FRAME_ALARM");
REGISTER_ENUM(SOUND_VOICE_FRAME_INSTALLED, "SOUND_VOICE_FRAME_INSTALLED");
REGISTER_ENUM(SOUND_VOICE_FRAME_DEINSTALLED, "SOUND_VOICE_FRAME_DEINSTALLED");
REGISTER_ENUM(SOUND_VOICE_FRAME_TELEPORTATION, "SOUND_VOICE_FRAME_TELEPORTATION");
REGISTER_ENUM(SOUND_VOICE_PORTAL_OUT_OF_RANGE, "SOUND_VOICE_PORTAL_OUT_OF_RANGE");
REGISTER_ENUM(SOUND_VOICE_SPIRAL_CHARGED, "SOUND_VOICE_SPIRAL_CHARGED");
REGISTER_ENUM(SOUND_VOICE_SPIRAL_CHARGING, "SOUND_VOICE_SPIRAL_CHARGING");
REGISTER_ENUM(SOUND_VOICE_SPIRAL_NOT_FULLY_CHARGED, "SOUND_VOICE_SPIRAL_NOT_FULLY_CHARGED");
REGISTER_ENUM(SOUND_VOICE_FILTH_DETECTED, "SOUND_VOICE_FILTH_DETECTED");
REGISTER_ENUM(SOUND_VOICE_GEOACTIVITY_DETECTED, "SOUND_VOICE_GEOACTIVITY_DETECTED");
REGISTER_ENUM(SOUND_VOICE_BUILDING_DISASSEMBLED, "SOUND_VOICE_BUILDING_DISASSEMBLED");
REGISTER_ENUM(SOUND_VOICE_BUILDING_UPGRADE_FINISHED, "SOUND_VOICE_BUILDING_UPGRADE_FINISHED");
REGISTER_ENUM(SOUND_VOICE_UNIT_MOVE, "SOUND_VOICE_UNIT_MOVE");
REGISTER_ENUM(SOUND_VOICE_UNIT_SELECTED, "SOUND_VOICE_UNIT_SELECTED");
REGISTER_ENUM(SOUND_VOICE_UNIT_PRODUCTION_STARTED, "SOUND_VOICE_UNIT_PRODUCTION_STARTED");
REGISTER_ENUM(SOUND_VOICE_UNIT_UNDER_ATTACK, "SOUND_VOICE_UNIT_UNDER_ATTACK");
REGISTER_ENUM(SOUND_VOICE_BUILDING_UNDER_ATTACK, "SOUND_VOICE_BUILDING_UNDER_ATTACK");
REGISTER_ENUM(SOUND_VOICE_UNIT_READY, "SOUND_VOICE_UNIT_READY");
REGISTER_ENUM(SOUND_VOICE_BUILDING_READY, "SOUND_VOICE_BUILDING_READY");
REGISTER_ENUM(SOUND_VOICE_UNIT_NEEDS_REPAIR, "SOUND_VOICE_UNIT_NEEDS_REPAIR");
REGISTER_ENUM(SOUND_VOICE_UNIT_DESTROYED, "SOUND_VOICE_UNIT_DESTROYED");
REGISTER_ENUM(SOUND_VOICE_BUILDING_DESTROYED, "SOUND_VOICE_BUILDING_DESTROYED");
REGISTER_ENUM(SOUND_VOICE_MMP_BACK_TO_FRAME, "SOUND_VOICE_MMP_BACK_TO_FRAME");
REGISTER_ENUM(SOUND_VOICE_WEAPON_CHARGED, "SOUND_VOICE_WEAPON_CHARGED");
REGISTER_ENUM(SOUND_VOICE_OBJECTIVES_UPDATED, "SOUND_VOICE_OBJECTIVES_UPDATED");
REGISTER_ENUM(SOUND_VOICE_OBJECTIVE_COMPLETE, "SOUND_VOICE_OBJECTIVE_COMPLETE");
END_ENUM_DESCRIPTOR(SoundEventID)

BEGIN_ENUM_DESCRIPTOR(SoundID, "SoundID")
REGISTER_ENUM(SOUND_NONE, "SOUND_NONE");
REGISTER_ENUM(SOUND_SHOT, "SOUND_SHOT");
REGISTER_ENUM(SOUND_MOVE, "SOUND_MOVE");
REGISTER_ENUM(SOUND_PRODUCTION, "SOUND_PRODUCTION");
END_ENUM_DESCRIPTOR(SoundID)

BEGIN_ENUM_DESCRIPTOR(CollisionGroupID, "CollisionGroupID")
REGISTER_ENUM(COLLISION_GROUP_ENEMY, "COLLISION_GROUP_ENEMY");
REGISTER_ENUM(COLLISION_GROUP_REAL, "COLLISION_GROUP_REAL");
REGISTER_ENUM(COLLISION_GROUP_ENEMY_ONLY, "COLLISION_GROUP_ENEMY_ONLY");
END_ENUM_DESCRIPTOR(CollisionGroupID)

