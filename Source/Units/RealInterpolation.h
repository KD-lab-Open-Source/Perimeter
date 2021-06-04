#ifndef _REAL_INTERPOLATION_
#define _REAL_INTERPOLATION_

#include "Interpolation.h"

class terRealPhaseControllerType
{
	cObjectNode* ObjectPoint;

	InterpolatorPhase phase_;
	InterpolatorAngle angle_x_;
	InterpolatorAngle angle_z_;

public:
	terRealPhaseControllerType(cObjectNode* object);
	void setPhase(float phase) { phase_ = phase; }
	void setAngleX(float angle_x) { angle_x_ = angle_x; }
	void setAngleZ(float angle_z) { angle_z_ = angle_z; }
	void SetChain(const char* name);
	void Update();
};

struct SoundControllerSetup;

class terSoundController
{
public:
	terSoundController(const char* name,int cycled);
	terSoundController(const SoundControllerSetup& setup);
	terSoundController(const terSoundController& data) { *this = data; const_cast<terSoundController&>(data).sound_ = 0; }
	~terSoundController();

	bool operator == (SoundID id) const { return (ID_ == id); }

	void play(){ needStart_ = true; } 
	void stop();

	bool isPlaying() const { return (isPlaying_ || needStart_); }

	void setVolume(float volume) { volume_ = volume; }
	void setFrequency(float frequency) { frequency_ = frequency; }
	void setPosition(const Vect3f& position); // не интерполируется!

	void update();

private:

	SoundID ID_;
	class SND3DSound* sound_;

	InterpolatorFloat volume_;
	InterpolatorFloat frequency_;
	bool cycled_;
	
	bool isPlaying_;
	bool needStart_;
};

class terInterpolationReal : public terInterpolationPose
{
protected:
	NodeController node;

	cLogicObject* logicObject_;
	typedef list<NodeController> NodeControllerList;
	NodeControllerList nodeControllers_;

	typedef list<terRealPhaseControllerType> PhaseListType;
	typedef list<terSoundController> SoundControllerList;

	PhaseListType PhaseControlList;
	SoundControllerList soundControllers_;

	//ObjectPoint должен меняться атомарно,
	//то есть после инициализации ObjectPoint должен всегда указывать на валидный объект.
	cObjectNodeRoot* ObjectPoint;

	int HideFlag;

	int SpecialEffectID;

	InterpolatorColor color_;
	float freeze_;
	float heal_;
	float hot_;
	float sight_;
	float build_;
	float life_;
	float hologram_;

public:
	terInterpolationReal(terUnitBase* owner);
	~terInterpolationReal();
	
	virtual void quant();
	virtual void updateLogicObject();
	virtual void setLogicObject(cLogicObject* logicObject);

	void Start();
	void interpolate();

	void SetModel(const char* name,float scale);

	void SetChain(const char* name);

	void SetSize(float size);
	void SetScale(float scale);

	terRealPhaseControllerType* AddPhaseController(const char* name);
	NodeController* animationGroup(AnimationGroupID groupID);
	
	terSoundController* AddSound(const char* name,int cycled);

	terSoundController* findSound(SoundID id);
	bool playSound(SoundID id);
	bool stopSound(SoundID id);

	cObjectNodeRoot* GetModelPoint(){ return ObjectPoint; }

	void UpdateSkinColor();

	void Show(){ 
		if(ObjectPoint)
			ObjectPoint->ClearAttr(ATTRUNKOBJ_IGNORE); 
	}
	void Hide(){ 
		if(ObjectPoint)
			ObjectPoint->SetAttr(ATTRUNKOBJ_IGNORE); 
	}

	void setTexture(cTexture* texture) { ObjectPoint->SetTexture(texture, 0); }

	void setFreeze(float freeze) { freeze_ = freeze; }
	void setHeal(float heal){ heal_ = heal; }
	void setHot(float hot) { hot_ = hot; }
	void setSight(float sight) { sight_ = sight; }
	void setBuild(float build) { build_ = build; }
	void setLife(float life) { life_ = life; }
	void setHologram(float hologram) { hologram_ = hologram; }

	cLogicObject* logicObject() const { return logicObject_; }
	void showDebugInfo();

	const AnimationChain* currentChain() const { return node.currentChain(); }
	float phase() const { return node.phase_.x1(); }

	void requestChain(ChainID requestedChain, float phase = -1){node.requestChain(requestedChain,phase);}
	void requestChainNode(ChainNodeID requestedNode){node.requestChainNode(requestedNode);}
	void requestPhase(float phase){node.requestPhase(phase);}
	bool requestDone() const { return node.requestDone(); }
	void setAnimationSpeed(float speed){ node.setAnimationSpeed(speed); }
	bool isEnd() const {return node.isEnd();}
	ChainID chainID() const { return node.chainID(); }
	ChainID requestedChainID() const { return node.requestedChainID(); }
	void setChain(ChainID chain){node.setChain(chain);}

	void setPhase(float phase){node.setPhase(phase);}
	void SetModelPoint(cObjectNodeRoot* p);

	void universalSave(struct SaveInterpolationReal& data) const;
	void universalLoad(const SaveInterpolationReal& data);
};

const int PERIMETER_PORT_LIGHT_POINT_MAX = 4;
const int PERIMETER_PORT_LIGHT_EFFECT_SCALE = 1;
const int PERIMETER_PORT_LIGHT_EFFECT_MAX = PERIMETER_PORT_LIGHT_POINT_MAX * PERIMETER_PORT_LIGHT_EFFECT_SCALE;

struct terInterpolationCorridor : terInterpolationReal
{
	int ProtectionMode;

	cObjectNode* LightSource[PERIMETER_PORT_LIGHT_POINT_MAX];
	cObjectNode* LightDirection[PERIMETER_PORT_LIGHT_POINT_MAX];
	cLine3d* LightPoint[PERIMETER_PORT_LIGHT_EFFECT_MAX];

	terInterpolationCorridor(terUnitBase* owner);
	~terInterpolationCorridor();

	void Start();
	void interpolate();
	void SetModel(const char* name,float scale);

	void GetLightPoint();
	void UpdateLightPoint();
	void SetProtectionMode(int mode){ ProtectionMode = mode; }

	void UpdateSkinColor();
};

//-----------------------------------------------

class terInterpolationLaser : public terInterpolationReal
{
public:
	terInterpolationLaser(terUnitBase* owner);
	~terInterpolationLaser();

	void interpolate();
	void quant();

	void UpdateSkinColor();

	void SetGunEnable(int enable);

	void CreateGunSource();

	void SetModel(const char* name,float scale);

	void setLaserLine(const Vect3f& target_pos);

	void CreateLaserPoint();

private:

	bool isEnabled_;
	cLine3d* line_;

	bool disableFlag_;
	int disableCounter_;

//	cObjectNode* laserSource_;
	cLogicTile* laserSource_;
	InterpolatorLine laserLine_;

	Vect3f sourcePosition_;
	Vect3f targetPosition_;
};

struct terInterpolationGun : terInterpolationReal
{
	int GunEnable,GunStatus;
	float GunDistance;
	cObjectNode* GunSource;
	cObjectNode* GunDirection;
	struct CBaseEffect* EffectPoint;

	terInterpolationGun(terUnitBase* owner);
	~terInterpolationGun();

	void Start();
	void interpolate();

	void SetGunEnable(int enable){ GunEnable = enable; }
	void SetGunDistance(float dist){ GunDistance = dist; }

	void CreateGunSource();
	void UpdateGun();

	void SetModel(const char* name,float scale);
};

class terInterpolationLeech : public terInterpolationReal
{
public:
	terInterpolationLeech(terUnitBase* owner) : terInterpolationReal(owner), connectionPoint_(NULL) { }
	void SetModel(const char* name,float scale);

	Vect3f getConnectionPosition() const;

private:

	cObjectNode* connectionPoint_;	
};

//---------------------------------

struct terInterpolationStructure : terInterpolationReal
{	
	float GunDistance;

	terInterpolationStructure(terUnitBase* owner);
	~terInterpolationStructure();

	void SetGunDistance(float dist){ GunDistance = dist; }
};

struct terInterpolationConnection : terInterpolationStructure
{
	typedef vector<cObjectNode*> connections_container_t;
	connections_container_t ConnectionPoints;

	terInterpolationConnection(terUnitBase* owner) : terInterpolationStructure(owner) {}

	void SetModel(const char* name,float scale);

	Vect3f GetConnectionPosition(const Vect3f& target);
};

struct terInterpolationCore : terInterpolationConnection
{
	cObjectNodeRoot* ProtectionPoint;
	cObjectNode* ProtectionSource;
	int ProtectionMode;
	InterpolatorColorDiffuse protection_color_;
	float protection_parameter_;

	terInterpolationCore(terUnitBase* owner);
	~terInterpolationCore();
	
	void Start();
	void interpolate();

	void setProtectionParameter(float protection_parameter);
	void setProtectionParameterImmediately(float protection_parameter);
	void SetProtectionMode(int mode) { ProtectionMode = mode; }

	void UpdateSkinColor();

	void SetModel(const char* name,float scale);
};

#endif
//-------------------------------