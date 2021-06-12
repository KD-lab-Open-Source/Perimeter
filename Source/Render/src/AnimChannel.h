#ifndef __ANIMCHANNEL_H__
#define __ANIMCHANNEL_H__

#include "AnimChannelNode.h"
#include "MeshTri.h"

struct sKeyTexMatrix
{
	MatXf		m;
	float		time;
};
struct sKeyColor
{
	sColor4f	color;
	float		time;
};
struct sKeyFloat
{
	float		a;
	float		time;
};

struct sChannelHeader
{
	string			name;
	float			Time;

	sChannelHeader()										{ Time=0; }
	sChannelHeader(const sChannelHeader& c)
	{
		name=c.name;
		Time=c.Time;
	}
};

class cAnimChainMaterial
{ // одна цепочка анимации
public:
	sChannelHeader* parent;

	vector<sKeyColor>		KeyAmbient;			// анимация материала объекта
	vector<sKeyColor>		KeyDiffuse;			
	vector<sKeyColor>		KeySpecular;		
	vector<sKeyColor>		KeyEmissive;
	vector<sKeyFloat>		KeyPower;
	vector<sKeyFloat>		KeyTransparency;
	vector<sKeyTexMatrix>	KeyTexMatrix;

	cAnimChainMaterial();
	~cAnimChainMaterial();
	// анимационные действия
	void GetDiffuse(float phase,sColor4f &Diffuse);
	void GetMaterial(float phase,cObjMaterial &ObjMaterial);
	void GetTexMatrix(float phase,MatXf &Matrix);

	float GetTransparency(float phase);
	inline bool IsAnimTransparency(){return KeyTransparency.size()>1;}
	inline bool IsAnimMaterial()	{return is_anim_material; }
protected:
	friend class cAnimChannelMaterial;
	bool is_anim_material;
	void EndBuild();
};

class cAnimChannelMaterial : public cUnknownClass, public cAnimChannelBase<cAnimChainMaterial>
{ // список каналов анимации
public:
	~cAnimChannelMaterial();
	void NewChannel(class cAllMeshBank* root);
	inline bool IsAnimMaterial(int nChain)			{ return GetChannel(nChain)->IsAnimMaterial(); }
	inline bool IsAnimTexMatrix(int nChain)			{ return GetChannel(nChain)->KeyTexMatrix.size(); }

	void EndBuild();
};

#endif //__ANIMCHANNEL_H__