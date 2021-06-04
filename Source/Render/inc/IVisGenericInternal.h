#pragma once
#include "Unknown.h"

#include <xutil.h>
#define VISASSERT(a)											xassert(a)
#include "RenderMT.h"

class cInterfaceRenderDevice;
class cScene;
class cVisGeneric;
class cLogicGeneric;
class cTexture;
class cCamera;

class cIUnkClass : public cUnknownClass, protected sAttribute
{
public:
	// инициализационная часть 
	cIUnkClass(int kind) : cUnknownClass(kind),IParent(0){ }
	virtual ~cIUnkClass(){}
	virtual	int Release();

	// общие функции для работы объектами cIUnkClass
	virtual void PreDraw(cCamera *UCamera)=0;
	virtual void Draw(cCamera *UCamera)=0;
	virtual void Animate(float dt)													{ }

	virtual void SetAttr(int attribute)												{ sAttribute::SetAttribute(attribute); }
	virtual void ClearAttr(int attribute=0xFFFFFFFF)								{ sAttribute::ClearAttribute(attribute); }
	inline int GetAttr(int attribute=0xFFFFFFFF)									{ return sAttribute::GetAttribute(attribute); }
	inline void PutAttr(int attribute,bool on){if(on)SetAttr(attribute);else ClearAttr(attribute);}

	virtual void SetPosition(const MatXf& Matrix);
	virtual const MatXf& GetPosition() const =0;
	virtual Vect3f GetCenterObject() {return GetPosition().trans();}

	virtual void SetColor(const sColor4f *ambient,const sColor4f *diffuse,const sColor4f *specular=0);
	virtual void GetColor(sColor4f *ambient,sColor4f *diffuse,sColor4f *specular=0) const;

	virtual float GetBoundRadius();

	inline class cScene* GetScene()													{ return IParent; }
	inline void SetScene(cScene* pScene)											{ IParent=pScene; }
	//В локальных коорлинатах, для получения Box в глобальных координатах,
	//нужно умножить на GetGlobalMatrix()
	virtual void GetBoundBox(sBox6f& Box);
	virtual void GetLocalBorder(int *nVertex,Vect3f **Vertex,int *nIndex,short **Index);

	virtual void SetAnimKeys(struct sParticleKey *AnimKeys,int size);

	//cIUnkObj
	virtual const MatXf& GetGlobalPosition() const;
	//cUnkObj
	virtual const char* GetName() const											{return NULL; }

	//cObjectNode
	virtual cIUnkClass* NextObject(cIUnkClass *UObj);
	
	virtual void SetRotate(Mat3f* rotate);

	//Пересчитывает местоположение объекта.
	//Нужно вызывать, если хочется узнать положение одного из узлов в cObjectNode.
	virtual void Update();

	// функции для работы с системой частиц (cParticle,cParticleCollision)
	virtual void SetProperties(const Vect3f *Dumping,const Vect3f *Force,const Vect3f *ReflectionImpulse=0)	{ }
protected:
	cScene			*IParent;		// интерфейс породивший данный класс
};

class cEmitter3dObject
{
public:
	virtual void GetAllPoints(vector<Vect3f>& point)=0;
	virtual void GetAllNormals(vector<Vect3f>& point)=0;
	virtual void GetEmitterMaterial(struct cObjMaterial& material)=0;
};

#include "..\src\visgeneric.h"

/*
	Нельзя создать два объекта cVisGeneric или cLogicGeneric.
	При повторном вызове этих функций возвращается тот-же объект.
*/
cVisGeneric* CreateIVisGeneric();
cLogicGeneric* CreateILogicGeneric();

struct sParticleKey
{
	sColor4c	diffuse;	// diffuse of particle
	Vect2f		rotate;		// sin & cos угла поворота * size of particle
	Vect2f		TexPos;		// texture position
	inline void SetRotate(float angle,float size)		{ rotate.x=size*cosf(angle); rotate.y=size*sinf(angle); }
};


void dprintf(char *format, ...);
typedef void (*ExternalObjFunction)();
