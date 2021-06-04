#pragma once

class terCorpseDynamic: public terUnitReal
{
public:
	terCorpseDynamic(const UnitTemplate& data);
	~terCorpseDynamic();
	void setParent(terUnitBase* p);

	void Collision(terUnitBase* p);
	void AvatarQuant();
	void Quant();

	void setRealModel(int modelIndex, float scale);
	void setCrater(float crater_radius,int crater_delay,terUnitAttributeID crater_id = UNIT_ATTRIBUTE_DESTRUCTION_CRATER){ craterRadius_ = crater_radius; craterDelay_ = crater_delay; craterID_ = crater_id; }

	void UnitDamageCheck();
	void ShowInfo(){}
	
	bool selectAble() const { return false; }

protected:
	terUnitEffectData effect_data;

	void destruct();
	void explode();
	/// радиус кратера, ноль если кратера нету
	float craterRadius_;
	/// задержка перед созданием кратера
	int craterDelay_;
	/// тип кратера
	terUnitAttributeID craterID_;

	const AttributeBase* OriginalAttribute;
	bool is_kill;
	DurationTimer kill_timer;
	DurationTimer dark_timer;
};
