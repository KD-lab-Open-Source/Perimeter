#ifndef __FRAMECORE_H__
#define __FRAMECORE_H__

class terBuildingEnergy : public terBuilding
{
public:
	terBuildingEnergy(const UnitTemplate& data) : terBuilding(data), lighting_(NULL) { }

	bool BuildingEnableRequest();
	void Quant();

	void setRealModel(int modelIndex, float scale);
	
	ChainID chainRequest() const;

	terEffectID explosionID() const;

private:

	/// управление источником света на столбе
	NodeController* lighting_;
};

#endif //__FRAMECORE_H__