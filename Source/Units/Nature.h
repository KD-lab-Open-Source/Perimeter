#ifndef __NATURE_H__
#define __NATURE_H__

#include "geo.h"

class terNatureObject : public terUnitGeneric
{
public:
	terNatureObject(const UnitTemplate& data);
	virtual ~terNatureObject();

	void Quant();
	void AvatarQuant();

	SaveUnitData* universalSave(SaveUnitData* data);
	void universalLoad(const SaveUnitData* data);

	void setPose(const Se3f& pose, bool initPose);
	void setRadius(float radius);

	void setModelName(const char* name);
    const char* GetModelName();

	void setDamage(const DamageData& damage, terUnitBase* p);

	void Collision(terUnitBase* p);

	RigidBody* GetRigidBodyPoint() const { return BodyPoint; }
	void showDebugInfo();

	void RefreshAttribute();

	void MapUpdateHit(float x0,float y0,float x1,float y1);

	void ShowInfo();

	terInterpolationReal* realAvatar() {return safe_cast<terInterpolationReal*>(avatar()); }

	void setActivity(bool activate) { visible_ = activate;	}
	bool activity() const { return visible_; } 

private:
	bool visible_;
	int NatureFlag;
	string ModelName;
	string ChainName;
	terAnimationPhaseIteratorType ObjectAnimation;

	class RigidBody* BodyPoint;
};

//--------------------------------------------

struct terNatureFallTree : terUnitBase
{
	int Time;
	
	float Transparency;
	float Angle,DeltaAngle;
	Vect3f Axis;
	
	terNatureFallTree(const UnitTemplate& data);
	
	void Start();
	void Quant();
	void AvatarQuant();
	
	void setAxis(const Vect3f& point);
	void SetModelName(const char* name);		
};


//--------------------------------------------------

class terNatureTerrain : public terUnitBase
{
public:
	terNatureTerrain(const UnitTemplate& data);
};

struct terNatureMountain : terNatureTerrain
{
	class CGeoInfluence* MoutainTool;

	terNatureMountain(const UnitTemplate& data);
	virtual ~terNatureMountain();

	void Start();
	void Quant();
	void UnitDamageCheck(){}
};

struct terNatureRift : terNatureTerrain
{
	int RiftCount;
	struct geoBreak1* RiftTool;

	terNatureRift(const UnitTemplate& data);
	virtual ~terNatureRift();

	void Start();
	void Quant();
	void UnitDamageCheck(){}
};

struct terNatureCleft : terNatureTerrain
{
	class CTerraCleftSystem* CleftTool;

	terNatureCleft(const UnitTemplate& data);
	virtual ~terNatureCleft();

	void Start();
	void Quant();
	void UnitDamageCheck(){}
};

struct terNatureWorm : terNatureTerrain
{
	Vect3f TargetPosition;
	float TargetAngle;
	CGeoWorm* pWormImmediately;

	terNatureWorm(const UnitTemplate& data);
	virtual ~terNatureWorm();

	void Start();
	void Quant();
	void UnitDamageCheck(){}
};

struct terNatureFace : terNatureTerrain
{
	Vect3f TargetPosition;
	float TargetAngle;
	int LifeCount;
	s_HeadGeoAction headGeoAction;

	terNatureFace(const UnitTemplate& data);
	virtual ~terNatureFace();

	void Start();
	void Quant();
	void UnitDamageCheck(){}
};

struct terNatureTorpedo : terNatureTerrain
{
	Vect2f Direction;
	struct sTorpedo* TorpedoImmediately;
	int Contact;
	float Distance;
	Vect3f StartPosition;
	int ClusterID;

	terNatureTorpedo(const UnitTemplate& data);
	virtual ~terNatureTorpedo();

	void Start();
	void Quant();
	void setTarget(terUnitBase* p,Vect3f* target);
	void SetBulletDistance(float dist);
};

struct terNatureFault : terNatureTerrain
{
	struct sGeoFault* GeoFaultImmediately;

	terNatureFault(const UnitTemplate& data);
	virtual ~terNatureFault();

	void Start();
	void Quant();
};


#endif //__NATURE_H__