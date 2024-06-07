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

	bool inited() const { return ObjectPoint != 0; }
	bool valid() const { return valid_; }
	bool buildingInArea() const { return buildingInArea_; }

private:
    MTSection lock;
	const AttributeBase* Attribute = nullptr;
	Vect3f Position;
	float Angle = 0.0f;
	float Scale = 0.0f;
	bool visible_ = false;
	bool valid_ = false;
	bool buildingInArea_ = false;
	cObjectNodeRoot* ObjectPoint = nullptr;
	
	Vect2f pos_set;
	float angle_set = 0.0f;

    uint8_t* BaseBuff = nullptr;
	int BaseBuffSX = 0;
    int BaseBuffSY = 0;
	int OffsetX = 0;
    int OffsetY = 0;

	bool light_show = false;
	class cTexture* pTexture = nullptr;
	class cPlane* plane = nullptr;

	class terIconBuilding* connection_icon_ = nullptr;

	Vect3f old_build_position;
	float old_build_angle = 0.0f;
	terPlayer* old_build_player;

	void InitTexture();

	bool checkScriptInstructions();
};

#endif //_INSTALLER_H_
