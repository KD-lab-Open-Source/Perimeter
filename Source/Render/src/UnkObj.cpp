#include "StdAfxRD.h"
#include "Scene.h"

#ifdef C_CHECK_DELETE
cCheckExit cCheckDelete::root;

void SaveKindObjNotFree();
cCheckExit::~cCheckExit()
{
	SaveKindObjNotFree();
}

cCheckDelete::cCheckDelete()
{
	if(root.root)
		root.root->prev=this;
	prev=NULL;
	next=root.root;
	root.root=this;
}

cCheckDelete::~cCheckDelete()
{
	if(next)
		next->prev=prev;

	if(prev)
		prev->next=next;
	else
		root.root=next;
}
#endif //C_CHECK_DELETE

/////////////////////cIUnkClass//////////////
void cIUnkClass::SetAnimKeys(struct sParticleKey *AnimKeys,int size)
{
	VISASSERT(0);
}

cIUnkClass* cIUnkClass::NextObject(cIUnkClass *UObj)
{
	VISASSERT(0);
	return 0;
}

void cIUnkClass::SetRotate(Mat3f* rotate)
{
	VISASSERT(0);
}

void cIUnkClass::Update()
{
	VISASSERT(0);
}

void cIUnkClass::SetPosition(const MatXf& Matrix)
{
	VISASSERT(0);
}

void cIUnkClass::SetColor(const sColor4f *ambient,const sColor4f *diffuse,const sColor4f *specular)
{
	VISASSERT(0);
}
void cIUnkClass::GetColor(sColor4f *ambient,sColor4f *diffuse,sColor4f *specular) const
{
	VISASSERT(0);
}

float cIUnkClass::GetBoundRadius()
{
	VISASSERT(0);
	return 0;
}

void cIUnkClass::GetBoundBox(sBox6f& Box)
{
	VISASSERT(0);
}

void cIUnkClass::GetLocalBorder(int *nVertex,Vect3f **Vertex,int *nIndex,short **Index)
{
	*nVertex=0;
	*Vertex=NULL;
	*nIndex=0;
	*Index=NULL;
	VISASSERT(0);
}
const MatXf& cIUnkClass::GetGlobalPosition() const
{
	VISASSERT(0);
	return MatXf::ID;
}

/////////////////////cIUnkObj////////////////
cIUnkObj::cIUnkObj(int kind):cIUnkClass(kind)
{
	if(MT_IS_LOGIC())
		SetAttr(ATTRUNKOBJ_CREATED_IN_LOGIC);
	Identity(LocalMatrix);
	Identity(GlobalMatrix);
	IParent=0;
	Radius=0;
}

cIUnkObj::~cIUnkObj()
{
}

int cIUnkClass::Release()
{
	if(DecRef()>0) 
	{
		VISASSERT(!GetAttribute(ATTRUNKOBJ_DELETED));
		return GetRef(); 
	}

//	bool is_deleted=GetAttribute(ATTRUNKOBJ_DELETED);
	if(IParent && !GetAttribute(ATTRUNKOBJ_DELETED))
	{
		SetAttribute(ATTRUNKOBJ_DELETED|ATTRUNKOBJ_IGNORE);
		//IParent=NULL;
		IParent->DetachObj(this);
	}

	if(GetRef()<=0)
	{
//		if(IParent && is_deleted)
//		{
//			MTG();
//		}

//		xassert(!GetAttribute(ATTRUNKOBJ_TEMP_USED));
		VISASSERT(GetRef()==0);
		delete this;
		return 0;
	}

	return GetRef();
}


void cIUnkObj::SetCopy(cIUnkObj *UObj)
{ 
	VISASSERT(UObj); 
	UObj->GetRadius()=GetRadius(); 
	UObj->LocalMatrix=GetLocalMatrix(); 
	UObj->GlobalMatrix=GetGlobalMatrix(); 
	UObj->Attribute=GetAttribute();
}
///////////////////cIUnkObjScale////////////////////
cIUnkObjScale::cIUnkObjScale(int kind)
:cIUnkObj(kind)
{
	Scale.set(1,1,1);
}

cIUnkObjScale::~cIUnkObjScale()
{
}

void cIUnkObjScale::SetCopy(cIUnkObj* UObj)
{
	cIUnkObj::SetCopy(UObj);
	cIUnkObjScale* pScale=(cIUnkObjScale*)UObj;
	pScale->Scale=Scale; 
}

///////////////////cUnkObj///////////////////////////
cUnkObj::cUnkObj(int kind):cIUnkObjScale(kind)
{ 
	diffuse.set(1,1,1,1);
	for( int i=0; i<NUMBER_OBJTEXTURE; i++ )
		Texture[i]=0;
}
cUnkObj::~cUnkObj()
{
	for( int i=0; i<NUMBER_OBJTEXTURE; i++ )
		if( Texture[i] )
		{
			Texture[i]->Release();
			Texture[i]=0;
		}
}
void cUnkObj::GetColor(sColor4f *Ambient,sColor4f *Diffuse,sColor4f *Specular) const
{
	if(Ambient) Ambient->set(0,0,0,1);
	if(Diffuse) *Diffuse=GetDiffuse();
	if(Specular) Specular->set(0,0,0,1);
}
void cUnkObj::SetColor(const sColor4f *Ambient,const sColor4f *Diffuse,const sColor4f *Specular)
{
	SetDiffuse(*Diffuse);
}

void cUnkObj::SetCopy(cIUnkObj *UObj)
{
	VISASSERT(UObj);
	cIUnkObjScale::SetCopy(UObj);
	cUnkObj *UnkObj=(cUnkObj*)UObj;
	UnkObj->diffuse=diffuse;
	for( int i=0; i<NUMBER_OBJTEXTURE; i++ )
		if((UnkObj->Texture[i]=Texture[i])!=0) 
			Texture[i]->IncRef();
}

void cUnkObj::SetTexture(int n,cTexture *pTexture,int attr)
{
	VISASSERT(n>=0 && n<NUMBER_OBJTEXTURE);

	RELEASE(Texture[n]);
	Texture[n]=pTexture;
}

void cIUnkObjScale::SetScale(const Vect3f& scale)
{
	if(Scale.x>FLT_EPS)
		GetRadius()*=scale.x/Scale.x;
	else
		GetRadius()=0;
	Scale=scale;
}

void cIUnkObj::SetPosition(const MatXf& Matrix)
{
	CheckMatrix(Matrix);
	GlobalMatrix=LocalMatrix=Matrix;
}