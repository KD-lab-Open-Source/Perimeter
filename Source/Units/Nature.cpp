#include "StdAfx.h"

#include "TerraCleft.h"
#include "Runtime.h"
#include "Universe.h"
#include "Nature.h"
#include "Config.h"

#include "PerimeterSound.h"
#include "GameShell.h"
#include "CameraManager.h"

terNatureObject::terNatureObject(const UnitTemplate& data) : terUnitGeneric(data)
{
	radius_ = attr().boundRadius;
	ModelName = attr().modelData.modelName;
	ChainName = "main";
	NatureFlag = NATURE_FLAG_LIGHT;
	visible_ = true;
	ObjectAnimation.startPhase(0,0,1);

	BodyPoint = 0;
}

terNatureObject::~terNatureObject()
{
	if(BodyPoint){
		delete BodyPoint;
		BodyPoint = NULL;
	}
}

SaveUnitData* terNatureObject::universalSave(SaveUnitData* baseData)
{
	SaveUnitNatureData* data = castOrCreate<SaveUnitNatureData>(baseData);
	terUnitGeneric::universalSave(data);
	
	data->radius = radius();
	data->modelName = ModelName;
	data->chainName = ChainName;
	data->chainPhase = ObjectAnimation.phase();
	data->chainPeriod = ObjectAnimation.period();
	data->natureFlag = NatureFlag;
	data->visible = visible_;

	return data;
}

void terNatureObject::universalLoad(const SaveUnitData* baseData)
{
	const SaveUnitNatureData* data = safe_cast<const SaveUnitNatureData*>(baseData);
	terUnitGeneric::universalLoad(data);

	ModelName = data->modelName;
	ChainName = data->chainName;
	NatureFlag = data->natureFlag;
	visible_ = data->visible;
	setRadius(data->radius); // sets the model name

	ObjectAnimation.startPhase(fmod(data->chainPhase, 1), data->chainPeriod ? terLogicTimePeriod/data->chainPeriod : 0, 1);
	AvatarQuant();
}

void terNatureObject::Quant()
{
	terUnitGeneric::Quant();

	ObjectAnimation.Quant();
	if(BodyPoint)
		BodyPoint->evolve(terGlobalTimePeriodSeconds);
}

void terNatureObject::setPose(const Se3f& poseIn, bool initPose)
{
	terUnitGeneric::setPose(poseIn, false);
	if(BodyPoint)
		BodyPoint->initPose(pose());
	if(avatar())
		avatar()->setPose(pose());
}

void terNatureObject::setRadius(float new_radius)
{
	radius_ = new_radius;

	setModelName(ModelName.c_str()); 
}

const char* terNatureObject::GetModelName()
{
	return ModelName.c_str();
}

void terNatureObject::setModelName(const char* name)
{
	if(avatar()){
		if(name)
			ModelName = name;
		else
			ModelName = attr().modelData.modelName;

		avatar()->SetModel(ModelName.c_str());

		float logic_radius = avatar()->GetModelPoint()->GetBoundRadius();
		sBox6f logicObjectBound;
		avatar()->GetModelPoint()->GetBoundBox(logicObjectBound);

		bool nosize = false;
		if(logic_radius<1e-3)
			nosize = true;
		float scale = radius()/(nosize ? 10 : logic_radius);
		avatar()->SetScale(scale);

		if((NatureFlag & NATURE_FLAG_REAL) && !nosize){
			//xassert(!nosize && "This object no have size");
			setCollisionGroup(collisionGroup() | COLLISION_GROUP_REAL);

			if(!BodyPoint)
				BodyPoint = new RigidBody;
			BodyPoint->build(*attr().rigidBodyPrm, avatar()->GetModelPoint(), logicObjectBound.min*scale, logicObjectBound.max*scale);
		}

		avatar()->setPose(pose());
		avatar()->SetChain(ChainName.c_str());
		realAvatar()->setPhase(ObjectAnimation.phase());

		if(avatar()->GetModelPoint()){
			if(NatureFlag & NATURE_FLAG_LIGHT)
				avatar()->GetModelPoint()->ClearAttr(ATTRUNKOBJ_NOLIGHT);
			else
				avatar()->GetModelPoint()->SetAttr(ATTRUNKOBJ_NOLIGHT);
		}
		avatar()->Start();
	}

	setPose(pose(), true); 
}

void terNatureObject::MapUpdateHit(float x0,float y0,float x1,float y1)
{
	if((NatureFlag & NATURE_FLAG_DESTROY) && position().x > x0 && position().x < x1 && position().y > y0 && position().y < y1){
		terNatureFallTree* t = safe_cast<terNatureFallTree*>(Player->buildUnit(UNIT_ATTRIBUTE_FALL_TREE));
		t->setPose(pose(), false);

		Vect3f v = Vect3f(position().x + radius() - 2.0f * terLogicRNDfrand()*radius(), position().y + radius() - 2.0f * terLogicRNDfrand() * radius(), position().z);
		t->setAxis(v);

		t->SetModelName(ModelName.c_str());
		t->setRadius(radius());
		t->Start();

		t->setPose(pose(), true);
		
		Kill();		
	}
}

void terNatureObject::setDamage(const DamageData& damage, terUnitBase* p)
{
	if(NatureFlag & NATURE_FLAG_DESTROY)
		terUnitGeneric::setDamage(damage, p);
}

void terNatureObject::Collision(terUnitBase* p)
{
	if((NatureFlag & NATURE_FLAG_DESTROY) && Player->clan() != p->Player->clan()){
		Vect3f v = p->position();
		float r = p->radius();

		if(v.distance2(position()) < r*r){
			terNatureFallTree* t = safe_cast<terNatureFallTree*>(Player->buildUnit(UNIT_ATTRIBUTE_FALL_TREE));
			t->setPose(pose(), false);
			t->setAxis(v);
			
			t->SetModelName(ModelName.c_str());
			t->setRadius(radius());
			t->Start();

			t->setPose(pose(), false);
			t->setAxis(v);

			Kill();
		}
	}
}

void terNatureObject::AvatarQuant()
{
	terUnitGeneric::AvatarQuant();
	if(avatar()){
		if(BodyPoint)
			avatar()->setPose(BodyPoint->pose());
		else
			avatar()->setPose(pose());

		if(fabs(ObjectAnimation.deltaPhase()) > FLT_EPS)
			realAvatar()->setPhase(ObjectAnimation.phase());

		if(visible_ || gameShell->missionEditor())
			avatar()->Show();
		else
			avatar()->Hide();
	}
}

void terNatureObject::showDebugInfo()
{
	terUnitGeneric::showDebugInfo();
	
	if(BodyPoint)
		BodyPoint->show();
}

void terNatureObject::RefreshAttribute()
{
	if(BodyPoint)
		BodyPoint->reload_parameters();
}

void terNatureObject::ShowInfo()
{
	if(gameShell->missionEditor()){
		if(selected()){
			Vect3f e,w;
			terCamera->GetCamera()->ConvertorWorldToViewPort(&position(),&w,&e);
			terRenderDevice->DrawRectangle(round(e.x) - 8,round(e.y) - 2, 16, 4,sColor4c(255,255,255,255),0);
		}

		if(strlen(label())){
			Vect3f e,w;
			terCamera->GetCamera()->ConvertorWorldToViewPort(&position(),&w,&e);
			terRenderDevice->OutText(round(e.x),round(e.y), label(), sColor4f(RED));
		}
	}
}


//--------------------------------------------------
terNatureTerrain::terNatureTerrain(const UnitTemplate& data) 
: terUnitBase(data) 
{ 
}

terNatureMountain::terNatureMountain(const UnitTemplate& data) : terNatureTerrain(data)
{
	MoutainTool = 0;
}

terNatureMountain::~terNatureMountain()
{
	if(MoutainTool){
		delete MoutainTool;
		MoutainTool = 0;
	}
}

void terNatureMountain::Start()
{
	terNatureTerrain::Start();
	MoutainTool = new CGeoInfluence(position().x,position().y,radius(),radius());
}

void terNatureMountain::Quant()
{
	terNatureTerrain::Quant();

	if(!(MoutainTool->quant()))
		Kill();
}


//--------------------------------------------------

terNatureRift::terNatureRift(const UnitTemplate& data) : terNatureTerrain(data)
{
	RiftTool = NULL;
	RiftCount = 200;
}

terNatureRift::~terNatureRift()
{
	if(RiftTool){
		delete RiftTool;
		RiftTool = NULL;
	}
}

void terNatureRift::Start()
{
	terNatureTerrain::Start();
	RiftTool = new geoBreak1(position().x, position().y, 100, 7);
}

void terNatureRift::Quant()
{
	terNatureTerrain::Quant();

	RiftCount--;
	if(!(RiftTool->quant()) || RiftCount <= 0)
		Kill();
}

//--------------------------------------------------

terNatureCleft::terNatureCleft(const UnitTemplate& data) : terNatureTerrain(data)
{
	CleftTool = NULL;
}

terNatureCleft::~terNatureCleft()
{
	if(CleftTool){
		delete CleftTool;
		CleftTool = NULL;
	}
}

void terNatureCleft::Start()
{
	terNatureTerrain::Start();

	CleftTool = new CTerraCleftSystem;
    Vect2i p = static_cast<Vect2i>(position2D());
	CleftTool->init(csysRadial, p);
}

void terNatureCleft::Quant()
{
	terNatureTerrain::Quant();

	if(!(CleftTool->quant()))
		Kill();
}

//--------------------------------------------------

terNatureWorm::terNatureWorm(const UnitTemplate& data) : terNatureTerrain(data)
{
	TargetPosition = Vect3f::ZERO;
	TargetAngle = 0;
	pWormImmediately=0;
}

terNatureWorm::~terNatureWorm()
{
	if(pWormImmediately) delete pWormImmediately;
}

void terNatureWorm::Start()
{
	terNatureTerrain::Start();
	TargetPosition = position();
	TargetAngle = terLogicRNDfrand() * M_PI * 2.0f;
	pWormImmediately=new CGeoWorm(round(position().x),round(position().y));
}

const float PERIMETER_NATURE_WORM_MOVE_ANGLE = M_PI * 0.5f;

void terNatureWorm::Quant()
{
	Vect3f v,l;

	terNatureTerrain::Quant();

	v = TargetPosition;
	v -= position();
	float d = v.x * v.x + v.y * v.y;
	float r = sqr(radius());	

	if(d > r){
		//worms(round(position().x),round(position().y));
		pWormImmediately->step(round(position().x),round(position().y));

		v = Vect3f(v.x,v.y,0);
		float s = v.norm();

		float a = getDeltaAngle(cycleAngle(v.psi() - M_PI * 0.5f), angleZ());

		if(s > attr().GroundPlaneSpeed)
			s = attr().GroundPlaneSpeed;

		if(a < 0){
			if(a < -attr().GroundAngleSpeed){
				a = -attr().GroundAngleSpeed;
				s = attr().GroundPlaneSpeed * 0.25f;
			}
		}else{
			if(a > attr().GroundAngleSpeed){
				a = attr().GroundAngleSpeed;
				s = attr().GroundPlaneSpeed * 0.25f;
			}
		}

		setAngleZ(cycleAngle(angleZ() + a));

		l = Vect3f(0,s,0) * Mat3f(angleZ(), Z_AXIS);

		setPosition(position() + l);
		setPositionZ((float)(vMap.GetAlt(round(position().x),round(position().y)) >> VX_FRACTION) + radius());
	}
	else{
		if(terLogicRND(100) < 50)
			TargetAngle += terLogicRNDfrand() * PERIMETER_NATURE_WORM_MOVE_ANGLE;
		else
			TargetAngle -= terLogicRNDfrand() * PERIMETER_NATURE_WORM_MOVE_ANGLE;

		TargetPosition = Vect3f(0, radius()*9.0f,0) * Mat3f(TargetAngle,Z_AXIS);
		TargetPosition += position();

		if(TargetPosition.x < radius()) TargetPosition.x = radius();
		if(TargetPosition.y < radius()) TargetPosition.y = radius();
		if(TargetPosition.x > ((float)vMap.H_SIZE - radius())) TargetPosition.x = (float)vMap.H_SIZE - radius();
		if(TargetPosition.y > ((float)vMap.V_SIZE - radius())) TargetPosition.y = (float)vMap.V_SIZE - radius();
	}
}

//--------------------------------------------------------
//dy = dx = 100

terNatureTorpedo::terNatureTorpedo(const UnitTemplate& data) : terNatureTerrain(data)
{
	TorpedoImmediately = NULL;
	Direction.x = 1.0f;
	Direction.y = 0;
	Contact = -1;
	Distance = attr().fireRadius()*3;
	ClusterID = 0;
}

terNatureTorpedo::~terNatureTorpedo()
{
	if(TorpedoImmediately){
		delete TorpedoImmediately;
		TorpedoImmediately = NULL;
	}
}

void terNatureTorpedo::setTarget(terUnitBase* p,Vect3f* target)
{ 
	Direction.x = target->x; 
	Direction.y = target->y; 
	Direction.normalize(1); 
}

void terNatureTorpedo::Start()
{
	terNatureTerrain::Start();
	Direction.normalize(1.0f);
	StartPosition = position();
	float z = (float)(vMap.GetAlt(vMap.XCYCL(round(position().x)),vMap.YCYCL(round(position().y))) >> VX_FRACTION);
	ClusterID = FieldCluster::get_cluster_id(field_dispatcher->getIncludingCluster(Vect3f(position().x,position().y,z)));
}

void terNatureTorpedo::SetBulletDistance(float dist)
{
	Distance = dist;
}

void terNatureTorpedo::Quant()
{
	terNatureTerrain::Quant();

	if(TorpedoImmediately){
		float z = (float)(vMap.GetAlt(vMap.XCYCL(TorpedoImmediately->curX),vMap.YCYCL(TorpedoImmediately->curY)) >> VX_FRACTION);
		ClusterID = FieldCluster::get_cluster_id(field_dispatcher -> getIncludingCluster(Vect3f(TorpedoImmediately->curX,TorpedoImmediately->curY,z)));

		float d = (sqr(TorpedoImmediately->curX - StartPosition.x) + sqr(TorpedoImmediately->curY - StartPosition.y));
		if(d > sqr(Distance) || !(TorpedoImmediately->quant()))
			Kill();
	}else{
		setPositionXY(position2D() + Direction*5.0f);
		if(Contact >= 0){
			if(Contact > 0)
				Contact--;
			else
				TorpedoImmediately = new sTorpedo(position().x,position().y,Direction);
		}
		else{
			if(!Player->energyColumn().filled(round(position().x), round(position().y)))
				Contact = 20;
		}
	}
}

//--------------------------------------------------

terNatureFault::terNatureFault(const UnitTemplate& data) : terNatureTerrain(data)
{
	GeoFaultImmediately = NULL;
}

terNatureFault::~terNatureFault()
{
	if(GeoFaultImmediately){
		delete GeoFaultImmediately;
		GeoFaultImmediately = NULL;
	}
}

void terNatureFault::Start()
{
	terNatureTerrain::Start();
	GeoFaultImmediately = new sGeoFault(position2D(), angleZ(), 600 + terLogicRND(4000));
}

void terNatureFault::Quant()
{
	terNatureTerrain::Quant();

	if(GeoFaultImmediately && !(GeoFaultImmediately->quant()))
		Kill();
}

//-------------------------------------------------

int terFaceDemoEnable = 1;

terNatureFace::terNatureFace(const UnitTemplate& data) : terNatureTerrain(data)
{
	TargetPosition = Vect3f::ZERO;
	TargetAngle = 0;
	LifeCount = 300;//Attribute.LegionRelaxTime;
}

terNatureFace::~terNatureFace()
{
	terFaceDemoEnable = 1;
}

void terNatureFace::Start()
{
	terNatureTerrain::Start();

	terFaceDemoEnable = 0;
	TargetPosition = position();
	TargetAngle = terLogicRNDfrand() * M_PI * 2.0f;
	headGeoAction.init(vMap.XCYCL(round(position().x)),vMap.YCYCL(round(position().y)),1);
}

const float PERIMETER_FACE_MOVE_RADIUS = 10;

void terNatureFace::Quant()
{
	terNatureTerrain::Quant();

	if(LifeCount == 0)
		headGeoAction.finit();
	LifeCount--;

	Vect2f delta = (const Vect2f&)TargetPosition - position2D();
	float dist = delta.norm2();
	if(dist > sqr(PERIMETER_FACE_MOVE_RADIUS)){
		if(!headGeoAction.quant(vMap.XCYCL(round(position().x)),vMap.YCYCL(round(position().y))))
			Kill();

		float speed = clamp(dist, 0, attr().GroundPlaneSpeed);

		float angle = getDeltaAngle(cycleAngle(atan2(delta.y, delta.x) - M_PI * 0.5f), angleZ());
		if(angle < 0){
			if(angle < -attr().GroundAngleSpeed){
				angle = -attr().GroundAngleSpeed;
				speed = attr().GroundPlaneSpeed*0.25f;
			}
		}
		else{
			if(angle > attr().GroundAngleSpeed){
				angle = attr().GroundAngleSpeed;
				speed = attr().GroundPlaneSpeed*0.25f;
			}
		}

		setAngleZ(cycleAngle(angleZ() + angle));

		Vect2f pos = position2D() + Mat2f(angleZ())*Vect2f(0,speed);
		setPosition(to3D(pos, (vMap.GetAlt(pos.xi(), pos.yi()) >> VX_FRACTION) + radius()));
	}
	else{
		if(terLogicRND(100) < 50)
			TargetAngle += terLogicRNDfrand() * PERIMETER_NATURE_WORM_MOVE_ANGLE;
		else
			TargetAngle -= terLogicRNDfrand() * PERIMETER_NATURE_WORM_MOVE_ANGLE;

		TargetPosition = Vect3f(0,PERIMETER_FACE_MOVE_RADIUS*9.0f,0)*Mat3f(TargetAngle,Z_AXIS);
		TargetPosition += position();
		TargetPosition.x = clamp(TargetPosition.x, radius(), (vMap.H_SIZE - 1) - radius());
		TargetPosition.y = clamp(TargetPosition.y, radius(), (vMap.V_SIZE - 1) - radius());
	}
}


//-----------------------------------------------

terNatureFallTree::terNatureFallTree(const UnitTemplate& data) : terUnitBase(data)
{
	Time = round(globalAttr().FallTreeTime*1000./terLogicTimePeriod);
}

void terNatureFallTree::Start()
{
	terUnitBase::Start();

	Angle = 0;
	DeltaAngle = 2.0 * M_PI / (float)((Time + 1)* Time);

	Transparency = 1.0f;

	avatar()->GetModelPoint()->SetAttr(ATTRUNKOBJ_NOLIGHT);
	avatar()->setPose(pose());
	avatar()->SetSize(radius());
	avatar()->Show();
	AvatarQuant();
	avatar()->Start();

	Vect3f p = position();
	SND3DPlaySound("tree_falling",&p);
}

void terNatureFallTree::Quant()
{
	terUnitBase::Quant();

	if(Time > 0){
		Time--;
		QuatF Rotation = orientation();
		Rotation.premult(QuatF(Angle,Axis));
		Angle += DeltaAngle;

		Vect3f v = Vect3f(0,0,radius());
		v *= Mat3f(Rotation);
		
		v += position();
		float z = (float)(vMap.GetAlt(vMap.XCYCL(round(v.x)),vMap.YCYCL(round(v.y))) >> VX_FRACTION);
		if(v.z < z){
			if(fabs(v.z - z) > FLT_EPS)
				Rotation.premult(QuatF(atan2((v.z - z),radius()),Axis));
			Time = 0;
		}
		setPose(Se3f(Rotation, position()), false);
	}
	else{
		Transparency -= 0.01f;
		if(Transparency < FLT_EPS){
			Transparency = 0;
			Kill();
		}
	}
}

void terNatureFallTree::AvatarQuant()
{
	terUnitBase::AvatarQuant();
	avatar()->setPose(pose());
	safe_cast<terInterpolationReal*>(avatar())->setSight(Transparency);
}

void terNatureFallTree::SetModelName(const char* name)
{
	avatar()->SetModel(name);
}

void terNatureFallTree::setAxis(const Vect3f& point)
{
	Axis = point;
	Axis -= position();
	Axis.postcross(Vect3f(0,0,1.0f));
}





