#ifndef __INTERPOLATION_
#define __INTERPOLATION_

#include "SafeMath.h"
#include "..\ht\StreamInterpolation.h"

class terAnimationPhaseIteratorType
{
public:
	terAnimationPhaseIteratorType();

	//period - задаётся в милисекундах
	// в случае period<terLogicTimePeriod через квант перескакивает на конец цепочки.
	void startPeriod(float phase, float period, bool cycling);
	void startPhase(float phase, float delta_phase, bool cycling); 

	void restart() { stopped_ = false; }
	void setPeriod(float period);
	void setPhase(float phase) { phase_ = phase; }
	void setEndPhase(float endPhase, bool cycled); // устанавливает фазу, на которой остановится анимация, ближайшую с учетм/без зацикливания
	
	int Quant();
	bool isEnd() const { return stopped_; };

	float phase() const { return phase_; }
	float deltaPhase() const { return deltaPhase_; }
	float period() const;
	float cycled() const { cycled_; }

private:
	bool cycled_;
	bool stopped_;
	float phase_;
	float endPhase_;
	float deltaPhase_;
	inline void checkDelta();
};

//-------------------------------------
/*
template<class T>
class DefaultInterpolationOp
{
public:
	void operator()(T& x, const T& x0, const T& x1, float t) const { float t_ = 1 - t; x = x0*t_ + x1*t; }
};

class Se3fInterpolationOp
{
public:
	void operator()(Se3f& s, const Se3f& s0, const Se3f& s1, float t) const 
	{ 
		s.trans().interpolate(s0.trans(), s1.trans(), t);
		s.rot().slerp(s0.rot(), s1.rot(), t);
	}
};

class PhaseInterpolationOp
{
public:
	void operator()(float& phase, const float& phase0, const float& phase1, float t) const 
	{ 
		phase = cycle(phase0 + getDist(phase1, phase0, 1 + FLT_EPS)*t, 1 + FLT_EPS);
	}
};

class AngleInterpolationOp
{
public:
	void operator()(float& angle, const float& angle0, const float& angle1, float t) const 
	{ 
		angle = cycle(angle0 + getDist(angle1, angle0, 2*M_PI)*t, 2*M_PI);
	}
};

class Vect3fInterpolationOp
{
public:
	void operator()(Vect3f& p, const Vect3f& p0, const Vect3f& p1, float t) const 
	{ 
		float t_ = 1 - t; 
		p.x = p0.x*t_ + p1.x*t; 
		p.y = p0.y*t_ + p1.y*t; 
		p.z = p0.z*t_ + p1.z*t; 
	}
};

template<class T, class InterpolationOp = DefaultInterpolationOp<T> >
class Interpolator
{
protected:
	T x0_, x1_;
	mutable T xt_;
	bool inited_;
	int quant_;
				 
public:
	Interpolator() { initialize(); }

	void initialize() { inited_ = false; }
	
	void operator=(const T& x) 
	{ 
		if(inited_){ 
			xt_ = x0_ = x1_; 
			x1_ = x; 
		} 
		else{ 
			inited_ = true; 
			xt_ = x0_ = x1_ = x; 
		} 
		quant_ = quantCounter();
	}
	
	const T& operator()() const 
	{ 
		if(quant_ == quantCounter())
			InterpolationOp()(xt_, x0_, x1_, interpolationFactor()); 
		else
			xt_ = x1_;

		return xt_; 
	}
	
	bool fresh() const { return quant_ == quantCounter(); }

	const T& x1() const { return x1_; }
};

typedef Interpolator<Se3f, Se3fInterpolationOp> InterpolatorPose;
typedef Interpolator<float, PhaseInterpolationOp> InterpolatorPhase;
typedef Interpolator<float, AngleInterpolationOp> InterpolatorAngle;
typedef Interpolator<float> InterpolatorFloat;

typedef Interpolator<Vect3f, Vect3fInterpolationOp> InterpolatorVect3f;
*/

template<class T ,class InterpolationOp = DefaultInterpolationOp<T> >
class Interpolator
{
protected:
	T x_[2];
	bool inited_;
	bool update_;
				 
public:
	Interpolator() { initialize(); }

	void initialize() { inited_ = false; update_=false;}
	
	void operator=(const T& x) 
	{ 
		if(inited_){ 
			x_[0] = x_[1]; 
			x_[1] = x; 
		} 
		else{ 
			inited_ = true; 
			x_[0] = x_[1] = x; 
		} 
		update_=true;
	}
	
	bool fresh() const { return update_; }
	const T& x0() const { return x_[0]; } //Устаревшее роложение
	const T& x1() const { return x_[1]; } //Положение на текущий логический квант

	void operator()(cIUnkObj* cur)
	{ 
		if(update_)
			InterpolationOp()(cur,x_);
		update_=false;
	}
};

class SpriteInterpolationOp
{
public:
	void operator()(cIUnkObj* cur,Vect3f p[2])
	{
		stream_interpolator.set(fSpriteInterpolation,cur)<<p;
	}
};

class Se3fInterpolationOp
{
public:
	void operator()(cIUnkObj* cur,Se3f p[2])
	{
//		xassert(cur->GetAttr(ATTRUNKOBJ_DELETED)==0);
//		cur->SetAttr(ATTRUNKOBJ_TEMP_USED);
		stream_interpolator.set(fSe3fInterpolation,cur)<<p;
	}
};
class PhaseInterpolationOp
{
public:
	void operator()(cIUnkObj* cur,float p[2])
	{
		xassert(0);
	}
};

class AngleInterpolationOp
{
public:
	void operator()(cIUnkObj* cur,float p[2])
	{
		xassert(0);
	}
};

class FloatInterpolationOp
{
public:
	void operator()(cIUnkObj* cur,float p[2])
	{
		stream_interpolator.set(fFloatInterpolation,cur)<<p;
	}
};

class ColorInterpolationOp
{
public:
	void operator()(cIUnkObj* cur,sColorInterpolate p[2])
	{
		stream_interpolator.set(fColorInterpolation,cur)<<p;
	}
};

class ColorDiffuseInterpolationOp
{
public:
	void operator()(cIUnkObj* cur,sColor4f p[2])
	{
		stream_interpolator.set(fColorDiffuseInterpolation,cur)<<p;
	}
};

class ParticleRateInterpolationOp
{
public:
	void operator()(cIUnkObj* cur,float p[2])
	{
		stream_interpolator.set(fParticleRateInterpolation,cur)<<p;
	}
};

typedef Interpolator<Vect3f, SpriteInterpolationOp> InterpolatorSprite;
typedef Interpolator<Se3f, Se3fInterpolationOp> InterpolatorPose;
typedef Interpolator<float, FloatInterpolationOp> InterpolatorFloat;
typedef Interpolator<sColorInterpolate, ColorInterpolationOp> InterpolatorColor;
typedef Interpolator<sColor4f, ColorDiffuseInterpolationOp> InterpolatorColorDiffuse;
typedef Interpolator<float, ParticleRateInterpolationOp> InterpolatorParticleRate;

class InterpolatorAngle:public Interpolator<float, AngleInterpolationOp>
{
public:
	void operator=(const float& x) 
	{
		(*(Interpolator<float, AngleInterpolationOp>*)this)=x;
	}

	void operator()(cIUnkObj* cur,eAxis axis)
	{ 
		if(update_)
		{
			stream_interpolator.set(fAngleInterpolation,cur)<<x_<<(int)axis;
		}
		update_=false;
	}
};

class InterpolatorPhase:public Interpolator<float, PhaseInterpolationOp>
{
public:
	void operator=(const float& x) 
	{
		(*(Interpolator<float, PhaseInterpolationOp>*)this)=x;
	}

	void operator()(cIUnkObj* cur,int recursive)
	{ 
		if(update_)
		{
			stream_interpolator.set(fPhaseInterpolation,cur)<<x_<<recursive;
		}
		update_=false;
	}
};

class InterpolatorLine
{
public:

	InterpolatorLine() { initialize(); set(Vect3f(0,0,0), Vect3f(0,0,0)); }

	void initialize() { update_ = false; }
	bool fresh() const { return update_; }

	void set(const Vect3f& x0, const Vect3f& x1)
	{
		x0_[0] = x0_[1] = x0;
		x1_[0] = x1_[1] = x1;
		update_ = true;
	}

	void changeSource(const Vect3f& x)
	{
		x0_[0] = x0_[1]; 
		x0_[1] = x;

		update_ = true;
	}

	void changeTarget(const Vect3f& x)
	{
		x1_[0] = x1_[1]; 
		x1_[1] = x;

		update_ = true;
	}

	void operator()(cIUnkObj* cur)
	{ 
		if(update_)
			stream_interpolator.set(fLineInterpolation,cur)<<x0_<<x1_;

		update_=false;
	}

private:

	bool update_;

	Vect3f x0_[2];
	Vect3f x1_[2];
};

//-------------------------------

class NodeController
{
public:
	NodeController(const AnimationData* animationData);
	virtual ~NodeController(){};

	virtual void quant();
	virtual void interpolate();

	void setVisualObject(cObjectNode* visualObject);
	cObjectNode* visualObject() const { return visualObject_; }

	virtual void SetChain(const char* name);

	void setChain(ChainID chain);
	void requestChain(ChainID requestedChain, float phase = -1); // Проиграть цепочку от текущей с учетом графа
	void requestChainNode(ChainNodeID requestedNode);
	void requestPhase(float phase); // Проиграть текущую цепочку до phase по кратчайшему, потом остановиться, выставив requestDone()
	bool requestDone() const { return requestDone_; } // В конце запрошенной цепочки для зацикленных и незацикленных цепочек одинаково
	bool isEnd() const {return phaseIterator_.isEnd();}// В конце незацикленной цепочки. Если выставил setChain и хочешь узнать, кончилась ли цепочка.
	ChainID chainID() const { return currentChain() ? currentChain()->chainID : CHAIN_NONE; } // Текущая цепочка
	ChainID requestedChainID() const { return !chainPath_.empty() ? chainPath_.front()->chainID : chainID(); } // Запрошенная цепочка
	const AnimationChain* currentChain() const { return currentChain_; }

	void setAnimationSpeed(float speed); // модуляция скорости анимации (..1..)
	void setPhase(float phase) 
	{ 
		phase_ = phase; 
		phaseIterator_.setPhase(phase);
	}
//	float phase() const { return phase_(); }

	AnimationGroupID animationGroupID() const { return animationData_ ? animationData_->groupID : ANIMATION_GROUP_ROOT; }

	SoundEventID soundEventID() const { return soundEventID_; }
	void setSoundEventID(SoundEventID id){ soundEventID_ = id; }

	void universalSave(struct SaveNodeController& data) const;
	void universalLoad(const SaveNodeController& data);

	void writeDebugInfo(XBuffer& buf);

protected:
	InterpolatorPhase phase_;
	friend class terInterpolationReal;// Должно потом исчезнуть
private:
	cObjectNode* visualObject_;
	
	const AnimationData* animationData_;
	const AnimationChain* currentChain_;
	typedef vector<const AnimationChain*> ChainVect;
	ChainVect chainPath_; // inverse order
	float requestedPhase_;
	bool requestDone_;
	terAnimationPhaseIteratorType phaseIterator_;
	SoundEventID soundEventID_; // sound event needs to be played
	
	bool findChainPath(ChainID requestedChain, ChainVect& chainPath); // from currentChain()
	void setChainInternal(ChainID chain);
};

//-------------------------------

class terInterpolationBase
{
public:
	terInterpolationBase(class terUnitBase* owner);
	virtual ~terInterpolationBase(){}

	virtual void quant();
	virtual void interpolate()=0;

	virtual void setLogicObject(cLogicObject* logicObject);
	virtual cLogicObject* logicObject() const { return NULL; }
	virtual void updateLogicObject();
	
	virtual void setPose(const Se3f& pose)=0;
	const MatXf& matrix() const { return matrix_; }
	virtual const Vect3f& pos0() const =0; //Устаревшее роложение
	virtual const Vect3f& pos1() const =0; //Положение на текущий логический квант

	virtual NodeController* animationGroup(AnimationGroupID groupID);

	virtual void Start(){}
	virtual void SetSize(float size){}
	virtual void SetScale(float scale){}

	virtual void SetChain(const char* name);

	virtual void Show(){}
	virtual void Hide(){}

	virtual void SetModel(const char* name,float scale = -1) {}

	virtual cObjectNodeRoot* GetModelPoint() { return 0; }
	virtual void SetModelPoint(cObjectNodeRoot* p) { xassert(0); }

	virtual class terRealPhaseControllerType* AddPhaseController(const char* name){ return NULL; }

	virtual class terSoundController* AddSound(const char* name,int cycled){ return NULL; }
	
	virtual void SetProtectionMode(int mode){}
	virtual void SetProtectionColor(sColor4f& color0,sColor4f& color1){}
	virtual void SetProtectionPosition(Vect3f& pos0,Vect3f& pos1){}
	virtual void SetProtectionDistance(float distance){}

	virtual void SetTrailID(int id){}
	virtual void SetTrailEnable(int enable){}

	virtual void SetGunEnable(int enable){}
	virtual void SetGunDistance(float dist){}

	virtual void UpdateSkinColor(){}

	virtual void showDebugInfo();

protected:
	class terUnitBase* Owner;

	MatXf matrix_;
};

class terInterpolationPose: public terInterpolationBase
{
public:
	terInterpolationPose(class terUnitBase* owner);
	void setPose(const Se3f& pose) { pose_ = pose; matrix_ = pose; }
	void interpolate() { }
	const Vect3f& pos0() const{return pose_.x0().trans();};
	const Vect3f& pos1() const{return pose_.x1().trans();};
protected:
	InterpolatorPose pose_;
};

class terInterpolationCopy : public terInterpolationPose
{
public:
	terInterpolationCopy(terUnitBase* owner);
	~terInterpolationCopy();
	
	void Start();
	void interpolate();

	void SetModelPoint(cObjectNodeRoot* p);
	cObjectNodeRoot* GetModelPoint(){ return ObjectPoint; }
	void UpdateSkinColor();
protected:
	cObjectNodeRoot* ObjectPoint;
};

class terInterpolationUninstall : public terInterpolationCopy
{
public:
	terInterpolationUninstall(terUnitBase* owner);
	~terInterpolationUninstall();

	void Start();
	void interpolate();

	void setSight(float sight){ sight_ = sight; }
	void setHologram(float hologram){ hologram_ = hologram; }
	void setTexture(cTexture* texture) { ObjectPoint->SetTexture(texture, 0); }

private:

	InterpolatorColor color_;
	float sight_;
	float hologram_;

	void updateColor();
};

class terInterpolationMonk
{
public:
	terInterpolationMonk();
	~terInterpolationMonk();
	
	void Start(float scale);

	void Show(){ if(HideFlag)update=true; HideFlag = false; }
	void Hide(){ if(!HideFlag)update=true; HideFlag = true; }
	bool isShow(){return !HideFlag;}

	void ColorUpdate();
	void SetProtectionMode(int mode){ if(ProtectMode!=mode)update=true; ProtectMode = mode; }

	void interpolate();
	void setPos(const Vect3f& pos);
	const Vect3f& pos0() const{return pose_.x0();};
	const Vect3f& pos1() const{return pose_.x1();};
protected:
	InterpolatorSprite pose_;
	InterpolatorSprite pose_protection;

	cSprite* ObjectPoint;
	cSprite* ProtectionPoint;

	bool update;
	bool HideFlag;
	int ProtectMode;
	float Size;
};

//------------------------------------

terInterpolationBase* terCreateUnitInterpolation(enum terInterpolationID id, terUnitBase* owner);

#endif
