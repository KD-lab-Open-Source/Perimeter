#ifndef _INSTALLER_H_
#define _INSTALLER_H_

#include "UnitAttribute.h"

class terPlayer;

class terBuildingInstaller
{
public:
	terBuildingInstaller(bool light_show = false);
	~terBuildingInstaller();

	void Clear();

	void InitObject(const AttributeBase* attr);
	void ConstructObject(terPlayer* player); 
	void CancelObject();

	//For Interface
	void SetBuildPosition(const Vect2f& mousePos, terPlayer* player);
	void ChangeBuildAngle(float dA, terPlayer* player);
	void setVisible(bool visible) { visible_ = visible; }
	void ShowCircle();
	void UpdateInfo(cCamera *DrawNode);
	//For AI
	void SetBuildPosition(const Vect3f& position, float angle, terPlayer* player);

	terUnitAttributeID attributeID() const { return Attribute ? static_cast<terUnitAttributeID>(Attribute->ID) : UNIT_ATTRIBUTE_NONE; }

	bool inited() const { return ObjectPoint != 0; }
	bool valid() const { return valid_; }
	bool buildingInArea() const { return buildingInArea_; }

private:
	const AttributeBase* Attribute;
	Vect3f Position;
	float Angle;
	float FireRadius,UmbrellaRadius;
	float Scale;
	bool visible_;
	bool valid_;
	bool buildingInArea_;
	cObjectNodeRoot* ObjectPoint;
	
	Vect2f pos_set;
	float angle_set;
	
	char* BaseBuff;
	int BaseBuffSX,BaseBuffSY;
	int OffsetX,OffsetY;

	bool light_show;
	class cTexture* pTexture;
	class cPlane* plane;

	class terIconBuilding* connection_icon_;

	Vect3f old_build_position;
	float old_build_angle;
	terPlayer* old_build_player;

	void InitTexture();
	void checkBuildingConnection();

	bool checkScriptInstructions();
};

#endif //_INSTALLER_H_