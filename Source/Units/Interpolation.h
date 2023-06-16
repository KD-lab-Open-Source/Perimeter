#ifndef __INTERPOLATION_
#define __INTERPOLATION_

#include "SafeMath.h"
#include "../HT/StreamInterpolation.h"

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
	float cycled() const { return cycled_; }

private:
	bool cycled_;
	bool stopped_;
	float phase_;
	float endPhase_;
	float deltaPhase_;
	inline void checkDelta();
};

template<class T>
class DefaultInterpolationOp {
};

template<class T, class InterpolationOp = DefaultInterpolationOp<T> >
class Interpolator
{
protected:
	T x_[2] = {};
	bool inited_ = false;
	bool update_ = false;
				 
public:
	Interpolator() { initialize(); }

	void initialize() { inited_ = false; update_=false; memset(x_, 0, sizeof(T) * 2); }
	
	void set(const T& x) 
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

	void operator()(cUnknownClass* cur)
	{ 
		if (update_) {
            InterpolationOp()(cur, x_);
            update_=false;
        }
	}
};
#define InterpolationOpStruct(NAME, T) \
struct NAME##InterpolationOp { \
	void operator()(cUnknownClass* cur, const T p[2]) { \
        if (stream_interpolator.set(f##NAME##Interpolation, cur)) { stream_interpolator << p[0] << p[1]; }; \
    } \
};

#define InterpolatorWithOp(NAME, T) \
InterpolationOpStruct(NAME,T) \
using Interpolator##NAME = Interpolator<T, NAME##InterpolationOp>;


InterpolatorWithOp(Sprite, Vect3f);
InterpolatorWithOp(Float, float);
InterpolatorWithOp(Color, sColorInterpolate);
InterpolatorWithOp(ColorDiffuse, sColor4f);
InterpolatorWithOp(ParticleRate, float);
InterpolationOpStruct(Se3f, Se3f);
using InterpolatorPose = Interpolator<Se3f, Se3fInterpolationOp>;

struct UnusedAssertOp {
    void operator()(cUnknownClass*, float[2]) { xassert(0); }
};

class InterpolatorAngle: public Interpolator<float, UnusedAssertOp> {
public:
	void operator()(cUnknownClass* cur,eAxis axis) {
		if(update_) {
			if (stream_interpolator.set(fAngleInterpolation,cur)) {
                stream_interpolator << x_ << static_cast<uint8_t>(axis);
            }
		}
		update_=false;
	}
};

class InterpolatorPhase: public Interpolator<float, UnusedAssertOp> {
public:
	void operator()(cUnknownClass* cur,int recursive) {
		if(update_) {
			if (stream_interpolator.set(fPhaseInterpolation,cur)) {
                stream_interpolator << x_ << recursive;
            }
		}
		update_=false;
	}
};

class InterpolatorLine {
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

	void operator()(cUnknownClass* cur)
	{ 
		if(update_) {
            if (stream_interpolator.set(fLineInterpolation, cur)) {
                stream_interpolator << x0_ << x1_;
            }
        }

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
	ChainID chainID() const { return currentChain() ? static_cast<ChainID>(currentChain()->chainID) : CHAIN_NONE; } // ������� �������
	ChainID requestedChainID() const { return !chainPath_.empty() ? static_cast<ChainID>(chainPath_.front()->chainID) : chainID(); } // ����������� �������
	const AnimationChain* currentChain() const { return currentChain_; }

	void setAnimationSpeed(float speed); // модуляция скорости анимации (..1..)
	void setPhase(float phase) 
	{ 
		phase_.set(phase); 
		phaseIterator_.setPhase(phase);
	}
//	float phase() const { return phase_(); }

	AnimationGroupID animationGroupID() const { return animationData_ ? static_cast<AnimationGroupID>(animationData_->groupID) : ANIMATION_GROUP_ROOT; }

	SoundEventID soundEventID() const { return soundEventID_; }
	void setSoundEventID(SoundEventID id){ soundEventID_ = id; }

	void universalSave(struct SaveNodeController& data) const;
	void universalLoad(SaveNodeController& data);

	void writeDebugInfo(XBuffer& buf);

protected:
	InterpolatorPhase phase_;
	friend class terInterpolationReal;// Должно потом исчезнуть
private:
	cObjectNode* visualObject_;
	
	const AnimationData* animationData_;
	const AnimationChain* currentChain_;
	typedef std::vector<const AnimationChain*> ChainVect;
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
	void setPose(const Se3f& pose) { pose_.set(pose); matrix_ = pose; }
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
