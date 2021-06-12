#ifndef __GENERICUNIT_H__
#define __GENERICUNIT_H__

#include "GenericControls.h"
#include "ToolzerController.h"

class terUnitGeneric : public terUnitBase, public GridElementType
{
public:
	terUnitGeneric(const UnitTemplate& data);
	~terUnitGeneric();

	void Kill();

	void setPose(const Se3f& pose, bool initPose);

	void Start();
	void Quant();

	virtual void setTarget(terUnitBase* p,Vect3f* target){}

	virtual void explode();
	/// идентификатор эффекта взрыва
	virtual terEffectID explosionID() const { return EFFECT_ID_EXPLOSION; }

	virtual	void UnitDamageCheck();

	virtual void SoundExplosion(){};

	void Save(XBuffer& out);
	void Load(XBuffer& in);

protected:
	/// создает "труп" юнита/здания, вызывается из explode()
	virtual bool createCorpse();

private:
};

/// труп юнита
class terUnitCorpse : public terUnitBase
{
public:
	terUnitCorpse(const UnitTemplate& data);

	void Quant();
	void Start();

	virtual void setParent(terUnitBase* p);

	void setLifeTime(int life_time){ lifeTime_ = life_time; }
	void setCrater(float crater_radius,int crater_delay,terUnitAttributeID crater_id = UNIT_ATTRIBUTE_DESTRUCTION_CRATER){ craterRadius_ = crater_radius; craterDelay_ = crater_delay; craterID_ = crater_id; }

protected:

	int time() const { return time_; }

private:

	/// текущее время существования трупа
	int time_;

	/// общее время существования трупа
	int lifeTime_;

	/// радиус кратера, ноль если кратера нету
	float craterRadius_;
	/// задержка перед созданием кратера
	int craterDelay_;
	/// тип кратера
	terUnitAttributeID craterID_;
};

/// кратер от юнита
class terCrater : public terUnitBase
{
public:
	terCrater(const UnitTemplate& data);
	~terCrater();

	void Start();
	void Quant();

	float toolzerScale() const { return toolzer_.scale(); }
	void setToolzerScale(float scale){ radius_ = toolzer_.radius() * scale; toolzer_.setScale(scale); }

	void setPose(const Se3f& pose, bool initPose);

	SaveUnitData* universalSave(SaveUnitData* data);
	void universalLoad(const SaveUnitData* data);

private:

	/// эффект над кратером (дым, огонь, etc)
	cEffect* effect_;

	DurationTimer lifeTimer_;

	/// тулзер кратера
	ToolzerController toolzer_;
};

//---------------------------------------------

void drawMark2d( cInterfaceRenderDevice *IRender, float x, float y, float phase, float health, float radiusFactor );

#endif //__GENERICUNIT_H__