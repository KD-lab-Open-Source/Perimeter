#ifndef __IRONEXPLOSION_H__
#define __IRONEXPLOSION_H__
 
class terDestructionCraterType : public terUnitBase
{
public:
	terDestructionCraterType(const UnitTemplate& data);

	void Start();
	void Quant();

	void setPose(const Se3f& pose, bool initPose);

private:
	craterToolzerDestroyZP ToolPoint; 
};

class terDebrisCraterType : public terUnitBase
{
public:
	terDebrisCraterType(const UnitTemplate& data);

	void Start();
	void Quant();

	void setPose(const Se3f& pose, bool initPose);

private:
	debrisToolzer ToolPoint; 
};

#endif //__IRONEXPLOSION_H__