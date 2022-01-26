#pragma once

class terGeoControl:public terUnitBase
{
public:
	terGeoControl(const UnitTemplate& data);
	~terGeoControl();

	SaveUnitData* universalSave(SaveUnitData* data);
	void universalLoad(SaveUnitData* data);

	void Start();
	void Quant();
	void DestroyLink();
	void ShowInfo();

	void setActivity(bool activate);
	bool activity() const { return !sleep_; } 

	virtual void Generate(float time)=0;
	virtual void Stop();

protected:
	bool attack_on;
	DurationTimer sleep_timer;
	DurationTimer attack_timer;

	bool sleep_;
	float first_sleep_time;
	float sleep_period;
	float delta_sleep_period;
	float attack_period;
	float delta_attack_periond;

	terUnitAttributeID activatingUnit_;
	float activatingDistance_;

	SND3DSound sound;
};


class terGeoInfluence:public terGeoControl
{
public:
	terGeoInfluence(const UnitTemplate& data);
	~terGeoInfluence();

	SaveUnitData* universalSave(SaveUnitData* data);
	void universalLoad(SaveUnitData* data);

	void Quant();
	void Generate(float time);
	void Stop();
protected:
	CGeoInfluence* mount;
};

class terGeoBreak:public terGeoControl
{
public:
	terGeoBreak(const UnitTemplate& data);
	~terGeoBreak();

	SaveUnitData* universalSave(SaveUnitData* data);
	void universalLoad(SaveUnitData* data);

	void Quant();
	void Generate(float time);
	void Stop();
protected:
	geoBreak1* mount;
	int num_break;
};

class terGeoFault:public terGeoControl
{
public:
	terGeoFault(const UnitTemplate& data);
	~terGeoFault();

	SaveUnitData* universalSave(SaveUnitData* data);
	void universalLoad(SaveUnitData* data);

	void Quant();
	void Generate(float time);
	void Stop();

	void ShowInfo();

protected:
	sGeoFault* mount;
	float length;
	float angle;
};

class terGeoHead:public terGeoControl
{
public:
	terGeoHead(const UnitTemplate& data);
	~terGeoHead();

	SaveUnitData* universalSave(SaveUnitData* data);
	void universalLoad(SaveUnitData* data);

	void Quant();
	void Generate(float time);
	void Stop();
protected:
	s_HeadGeoAction* mount;
};
