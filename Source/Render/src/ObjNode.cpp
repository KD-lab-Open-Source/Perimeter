#include "StdAfxRD.h"
#include "ObjNode.h"
#include "AnimChannel.h"
#include "MeshBank.h"
#include "ObjMesh.h"
#include "ObjLight.h"
#include "Scene.h"

cObjectNode::cObjectNode(int kind) : cIUnkObj(kind)
{
	AnimChannel=0;
	SetAttribute(ATTRUNKOBJ_COLLISIONTRACE);
	root=NULL;
	RotateMatrix=Mat3f::ZERO;
	RootNode=NULL;
	group=NULL;
	shadow=NULL;
	effect=NULL;
	prev_visible=true;
}

cObjectNode::~cObjectNode()
{ 
	RELEASE(shadow);
	RELEASE(AnimChannel);

	if(effect)
	{
		effect->StopAndReleaseAfterEnd();
		effect=NULL;
	}

	for(cObjectNode *child=GetChild();child;)
	{
		cObjectNode *tmp=child;
		child=child->GetSibling();
		tmp->Release();
	}
	if(GetParentNode()) GetParentNode()->DetachChild(this);

}
//////////////////////////////////////////////////////////////////////////////////////////
// реализация интерфейса cUnkObj
//////////////////////////////////////////////////////////////////////////////////////////
const char* cObjectNode::GetNameObj() const
{
	if(AnimChannel)
		return AnimChannel->ObjectName.c_str();
	return NULL;
}

void cObjectNode::PreDraw(cCamera *DrawNode)
{
	VISASSERT(0<=GetCurrentChannel() && GetCurrentChannel()<AnimChannel->GetNumberChannel());
	cAnimChainNode* anim=AnimChannel->GetChannel(GetCurrentChannel());

	int visible=1;
	anim->GetVisible(GetPhase(),visible);
	NodeAttribute.PutAttribute(ATTRNODE_IGNORE,!visible);

	if(NodeAttribute.GetAttribute(ATTRNODE_EFFECT))
	{
		if(NodeAttribute.GetAttribute(ATTRNODE_EFFECT_CYCLED))
		{
			if(visible && !effect)
			{
				effect=DrawNode->GetScene()->CreateEffect(*AnimChannel->effect_key,RootNode,RootNode->GetScale().x);
				effect->LinkToNode(this);
			}


			if(effect)
				effect->SetParticleRate(visible?1.0f:0);
		}else
		{
			if(visible && !prev_visible)
			{
				cEffect* eff=DrawNode->GetScene()->CreateEffect(*AnimChannel->effect_key,RootNode,RootNode->GetScale().x,true);
				eff->LinkToNode(this);
			}
		}
	}

	prev_visible=visible;

	if(visible)
	for(cObjectNode *child=GetChild();child;child=child->GetSibling())
		child->PreDraw(DrawNode);
}

void cObjectNode::SetPosition(const MatXf& Matrix)
{
	VISASSERT(0);
}
void cObjectNode::SetColor(const sColor4f *ambient,const sColor4f *diffuse,const sColor4f *specular)
{
	VISASSERT(0);
}

float cObjectNode::GetBoundRadius()
{
	return GetRadius();
}

void cObjectNode::GetBoundBox(sBox6f& Box)
{
	Box=GlobalBound;
}

void cObjectNode::CalcObj()
{
	for(cObjectNode *child=GetChild();child;child=child->GetSibling())
		child->CalcObj();
}

void cObjectNode::GetBoundingBox(Vect3f &min,Vect3f &max)
{
	min=Vect3f(1e30f,1e30f,1e30f);
	max=Vect3f(-1e30f,-1e30f,-1e30f);
}

void cObjectNode::CalcBorder()
{ 
	Vect3f Min(1e30f,1e30f,1e30f),Max(-1e30f,-1e30f,-1e30f);
	CalcBorder(Min,Max);

	if(Min.x>Max.x) Min.set(0,0,0),Max.set(0,0,0);
	GlobalBound.min=Min;
	GlobalBound.max=Max;
	GetRadius()=GlobalBound.GetRadius();
}

void cObjectNode::CalcBorder(Vect3f& Min,Vect3f& Max)
{
	for(cObjectNode *child=GetChild();child;child=child->GetSibling())
	{
		cObjectNode *ObjNode=child;
		ObjNode->CalcBorder();
		MatXf ChildOriginalMatrix=ObjNode->GetLocalMatrix();
		if(fabs(ObjNode->GlobalBound.min.x)<1e20f)
		{
			Vect3f min,max;
			ChildOriginalMatrix.xformPoint(ObjNode->GlobalBound.min,min);
			ChildOriginalMatrix.xformPoint(ObjNode->GlobalBound.max,max);
			Min=::GetMin(min,Min);
			Min=::GetMin(max,Min);
			Max=::GetMax(min,Max);
			Max=::GetMax(max,Max);
		}
	}
}

void cObjectNode::SetCopy(cIUnkObj* UObj)
{
	cObjectNode *ObjNode=(cObjectNode*)UObj;
	cIUnkObj::SetCopy(ObjNode);
	ObjNode->GlobalBound=GlobalBound;

	for(cObjectNode *child=GetChild();child;child=child->GetSibling())
	{
		cObjectNode *tmp;
		tmp=(cObjectNode*)child->BuildCopy();
		ObjNode->AttachChild(tmp);
	}
	ObjNode->NodeAttribute=NodeAttribute; 
	if(AnimChannel)AnimChannel->IncRef();
	ObjNode->AnimChannel=AnimChannel;
	if(shadow)shadow->IncRef();
	ObjNode->shadow=shadow;

	ObjNode->root=root;
}

cIUnkObj* cObjectNode::BuildCopy()
{
	cObjectNode *ObjNode=new cObjectNode;
	SetCopy(ObjNode);
	return ObjNode;
}
void cObjectNode::SetPhase(float StartPhase,bool recursive)
{
	VISASSERT(StartPhase>=0 && StartPhase<FRAME_PHASE_RANGE);
	if(StartPhase>FRAME_PHASE_MAX)
		StartPhase=FRAME_PHASE_MAX;
	group->SetPhase(StartPhase,recursive);
	
	RootNode->NodeAttribute.SetAttribute(ATTRNODE_UPDATEMATRIX);
}

void cObjectNode::SetAttr(int attribute)
{
	sAttribute::SetAttribute(attribute); 
	for(cObjectNode *child=GetChild();child;child=child->GetSibling())
		child->SetAttr(attribute);
	if(effect && NodeAttribute.GetAttribute(ATTRNODE_EFFECT_CYCLED))
	{
		effect->StopAndReleaseAfterEnd();
		effect=NULL;
	}
}

void cObjectNode::ClearAttr(int attribute)
{
	sAttribute::ClearAttribute(attribute); 
	for(cObjectNode *child=GetChild();child;child=child->GetSibling())
		child->ClearAttr(attribute);
}

int cObjectNode::SetChannel(int nChannel,bool recursive)
{ 
	if(nChannel<0 || root->GetNumberChannel()<=nChannel)
		return 1;

	RootNode->NodeAttribute.SetAttribute(ATTRNODE_UPDATEMATRIX);

	if(Parent)
	{
		cAnimChainNode* anim=AnimChannel->GetChannel(nChannel);
		anim->GetMatrix(GetPhase(),LocalMatrix);
	}

	group->SetCurrentChannel(nChannel);
	for(cObjectNode *child=GetChild();child;child=child->GetSibling())
		if(!child->IsGroup() || recursive)
			child->SetChannel(nChannel,recursive);
	return 0; 
}
int cObjectNode::SetChannel(const char *NameChain,bool recursive)
{ 
	if(group->lod)
		group->lod->SetChannel(NameChain,recursive);

	int nChannel;
	for(nChannel=0;nChannel<root->GetNumberChannel();nChannel++)
		if(strcmp(root->GetChannel(nChannel).name.c_str(),NameChain)==0)
			break;
	
	//xassert_s(nChannel<root->GetNumberChannel() && "Animation chain not found", NameChain);

	return SetChannel(nChannel,recursive);
}
int cObjectNode::GetChannel(int nChannel,char *NameChain,float *Time)
{ 
	if(nChannel<0||root->GetNumberChannel()<=nChannel) 
	{
		strcpy(NameChain,root->GetChannel(GetCurrentChannel()).name.c_str()); 
		return 1;
	}
	if(NameChain) strcpy(NameChain,root->GetChannel(nChannel).name.c_str());
	if(Time) *Time=root->GetChannel(nChannel).Time;
	return 0; 
}
int cObjectNode::GetCurrentChannel(int *nChannel,char *NameChain,float *Time)
{ 
	if(nChannel) *nChannel=GetCurrentChannel(); 
	if(NameChain) strcpy(NameChain,root->GetChannel(GetCurrentChannel()).name.c_str());
	if(Time) *Time=root->GetChannel(GetCurrentChannel()).Time;
	return 0; 
}
int cObjectNode::GetNumberChannel()
{ 
	return root->GetNumberChannel();
}
void cObjectNode::SetTexture(cTexture *Tex1,cTexture *Tex2)
{
	VISASSERT(0);
}

void cObjectNode::RestoreOriginalMaterial()
{
	for(cObjectNode *child=GetChild();child;child=child->GetSibling())
		child->RestoreOriginalMaterial();
}

void cObjectNode::SetAnotherMaterial(cTexture *Tex1,cTexture *Tex2,AllAnotherMaterial* aroot)
{
	for(cObjectNode *child=GetChild();child;child=child->GetSibling())
		child->SetAnotherMaterial(Tex1,Tex2,aroot);
}

cObjectNode::AllAnotherMaterial::AllAnotherMaterial(cTexture *Tex1,cTexture *Tex2)
{
	pTex1=Tex1;
	pTex2=Tex2;
	if(pTex1)pTex1->IncRef();
	if(pTex2)pTex2->IncRef();
}

cObjectNode::AllAnotherMaterial::~AllAnotherMaterial()
{
	vector<AnotherMaterial>::iterator it;
	FOR_EACH(materials,it)
		delete it->pCurMaterial;
	RELEASE(pTex1);
	RELEASE(pTex2);
}

cMeshBank* cObjectNode::AllAnotherMaterial::Find(cMeshBank* pOriginalMaterial)
{
	vector<AnotherMaterial>::iterator it;
	FOR_EACH(materials,it)
		if(it->pOriginalMaterial==pOriginalMaterial)
			return it->pCurMaterial;

	return NULL;
}
		
cMeshBank* cObjectNode::AllAnotherMaterial::Add(cMeshBank* pOriginalMaterial)
{
	cMeshBank* pBank=pOriginalMaterial->BuildStaticCopy();
	AnotherMaterial m;
	m.pOriginalMaterial=pOriginalMaterial;
	m.pCurMaterial=pBank;
	materials.push_back(m);
	return pBank;
}

bool cObjectNode::IntersectSphere(const Vect3f& xp0,const Vect3f& xp1)
{
	MatXf m=GetGlobalMatrix();
	m.Invert();
	Vect3f p0=m*xp0,p1=m*xp1;

	//Vect3f &Center=GetGlobalMatrix().trans();
	Vect3f Center(0,0,0);
	float R2=GetRadius()*GetRadius();

	Vect3f l=p1-p0;
	float A=l.norm2();

	Vect3f d=p0-Center;
	float C=d.x*d.x+d.y*d.y+d.z*d.z-R2;
	float B=d.x*l.x+d.y*l.y+d.z*l.z;
	float det=B*B-A*C;
	if(det<0)return false;
	det=sqrtf(det);
	float t1=(-B+det)/A;
	float t2=(-B-det)/A;
	if((t1>1||t1<0)&&(t2>1||t2<0)) return false;
	return true;
}

bool cObjectNode::Intersect(const Vect3f& p0,const Vect3f& p1)
{
	if(NodeAttribute.GetAttribute(ATTRNODE_IGNORE))
		return false;

	if(IntersectSphere(p0,p1))
	for(cObjectNode *child=GetChild();child;child=child->GetSibling())
		if(child->Intersect(p0,p1))
			return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
// реализация cObjectNode
//////////////////////////////////////////////////////////////////////////////////////////
void cObjectNode::AttachChild(cObjectNode *child)
{
	child->SetParent(this);
	cBaseNode <cObjectNode>::AttachChild(child);
}
void cObjectNode::DetachChild(cObjectNode *child)
{
	child->SetParent(0);
	cBaseNode <cObjectNode>::DetachChild(child);
}
void cObjectNode::CalcMatrix()
{
	if(Parent)
	{
		if(AnimChannel)
		{
			cAnimChainNode* anim=AnimChannel->GetChannel(0);
			anim->GetMatrix(0,LocalMatrix);
		}

		GlobalMatrix=Parent->GetGlobalMatrix()*GetLocalMatrix();
	}else
		GlobalMatrix=GetLocalMatrix();
	for(cObjectNode *child=GetChild();child;child=child->GetSibling())
		child->CalcMatrix();
}

cObjectNode* cObjectNode::FindSubObject(const char *name)
{ 
	return FindObject(name);
}

cObjectNode* cObjectNode::FindObject(const char *name)
{
	if(name==NULL && GetNameObj()==NULL)
		return this;
	if(GetNameObj()&&stricmp(GetNameObj(),name)==0) 
		return this;
	for(cObjectNode *child=GetChild();child;child=child->GetSibling())
		if(cObjectNode *UnkNode=child->FindObject(name)) 
			return UnkNode;
	return 0;
}

const char* cObjectNode::GetName() const
{
	return GetNameObj();
}

cIUnkClass* cObjectNode::NextObject(cIUnkClass *UObj)
{
	if(UObj==0) return GetChild();
	cObjectNode* pNode=(cObjectNode*)UObj;
	return pNode->GetSibling();
}

void cObjectNode::SetRotate(Mat3f* rotate)
{
	if(group->lod)
		group->lod->SetRotate(rotate);

	RootNode->NodeAttribute.SetAttribute(ATTRNODE_UPDATEMATRIX);
	
	if(rotate)
	{
		NodeAttribute.SetAttribute(ATTRNODE_ENABLEROTATEMATRIX);
		RotateMatrix=*rotate;
	}else
	{
		NodeAttribute.ClearAttribute(ATTRNODE_ENABLEROTATEMATRIX);
	}
}

void cObjectNode::SetGroup(bool is_group)
{
	if(is_group)
		NodeAttribute.SetAttribute(ATTRNODE_ISGROUP);
	else
		NodeAttribute.ClearAttribute(ATTRNODE_ISGROUP);
}

void cObjectNode::AddChild(vector<cObjectNode*>& all_child)
{
	for(cObjectNode *child=GetChild();child;child=child->GetSibling())
	{
		all_child.push_back(child);
		child->AddChild(all_child);
	}
}

int cObjectNode::GetNumGroup()
{
	int num=0;
	if(IsGroup())
		num++;
	for(cObjectNode *child=GetChild();child;child=child->GetSibling())
		num+=child->GetNumGroup();

	return num;
}

void cObjectNode::SetGroup(int& cur_num,vector<cObjectGroup>& groups,cObjectGroup* cur_group)
{
	if(IsGroup())
	{
		VISASSERT(cur_num<groups.size());
		cObjectGroup& g=groups[cur_num++];
		group=&g;

		if(cur_group)
			cur_group->AttachChild(group);
		
		cur_group=group;
	}else
		group=cur_group;
	VISASSERT(group);

	for(cObjectNode *child=GetChild();child;child=child->GetSibling())
		child->SetGroup(cur_num,groups,group);
}

void cObjectNode::UpdateMatrix()
{
	cAnimChainNode* anim=AnimChannel->GetChannel(GetCurrentChannel());

	if(anim->IsAnimMatrix())
		anim->GetMatrix(GetPhase(),LocalMatrix);
	GlobalMatrix.mult(GetParentNode()->GetGlobalMatrix(),GetLocalMatrix());
	if(NodeAttribute.GetAttribute(ATTRNODE_ENABLEROTATEMATRIX))
		GlobalMatrix.rot()*=RotateMatrix;
}	

void cObjectNode::AddShadow(MatXf mat,cMeshTri* Tri)
{
	if(!(gb_RenderDevice->GetRenderMode()&RENDERDEVICE_MODE_STRENCIL))
		return;
	bool anim=false;
	if(AnimChannel)
	for(int i=0;i<AnimChannel->GetNumberChannel();i++)
		anim=anim || AnimChannel->IsAnimMatrix(i);

	if(anim || !Parent)
	{
		if(!shadow)
			shadow=new ShadowVolume;
		shadow->Add(mat,Tri);
	}else
		Parent->AddShadow(GetLocalMatrix()*mat,Tri);
}

void cObjectNode::BuildShadow()
{
}

void cObjectNode::DrawShadow(cCamera *DrawNode)
{
	if(shadow)
	{
		Vect3f LightDirection;
		DrawNode->GetScene()->GetLighting( &LightDirection);
		
		shadow->Draw(DrawNode,GetGlobalMatrix(),LightDirection,Option_DebugShowShadowVolume==1);
	}
}

void cObjectNode::ChangeBank(cAllMeshBank* new_root)
{
	root=new_root;
}

/////////////////cObjectNodeRoot///////////////
cObjectNodeRoot::cObjectNodeRoot()
:cObjectNode(KIND_OBJ_NODE_ROOT)
{
	AnnoterRoot=NULL;
	Base=0;
	Scale.set(1,1,1);
	RootNode=this;
	MaterialAnimTime=0;
	NodeAttribute.SetAttribute(ATTRNODE_ISGROUP);

	Diffuse.set(1,1,1,1);
	Specular.set(1,1,1,0);
	Ambient.set(0,0,0,0);
	LightIntensity.set(0,0,0,0);
	SkinColor.set(1,1,1,1);

	RootLod=NULL;
}

cObjectNodeRoot::~cObjectNodeRoot()
{
	ClearDrawSort();
	vector<cObjectGroup>::iterator it_group;
	FOR_EACH(groups,it_group)
	{
		it_group->Parent=NULL;
		it_group->Child=NULL;
		it_group->Sibling=NULL;
	}

	if(AnnoterRoot)
		delete AnnoterRoot;
	if(NodeAttribute.GetAttribute(ATTRNODE_COPYBASE)==0)
		if(Base) delete Base;
	Base=0;

	RELEASE(RootLod);
}


void cObjectNodeRoot::SetTexture(cTexture *Tex1,cTexture *Tex2)
{
	MTEnter enter(IParent->GetLockDraw());
	if(!AnnoterRoot && !Tex1 && !Tex2)
		return;

	if(AnnoterRoot && AnnoterRoot->pTex1==Tex1 && AnnoterRoot->pTex2==Tex2)
		return;

	ClearDrawSort();

	if(AnnoterRoot)
		delete AnnoterRoot;
	AnnoterRoot=NULL;

	RestoreOriginalMaterial();
	if(Tex1==NULL && Tex2==NULL)
		return;

	AnnoterRoot=new AllAnotherMaterial(Tex1,Tex2);
	SetAnotherMaterial(Tex1,Tex2,AnnoterRoot);
}

void cObjectNodeRoot::SetFileName(const char *fName)
{
	fname=fName;
}

cIUnkObj* cObjectNodeRoot::BuildCopy()
{
	cObjectNodeRoot *ObjNode=new cObjectNodeRoot;
	SetCopy(ObjNode);
	return ObjNode;
}

void cObjectNodeRoot::SetCopy(cIUnkObj* UObj)
{
	cObjectNodeRoot* ObjNode=(cObjectNodeRoot*)UObj;
	cObjectNode::SetCopy(UObj);
	ObjNode->SetFileName(GetFileName());
	ObjNode->Base=Base;
	ObjNode->NodeAttribute.SetAttribute(ATTRNODE_COPYBASE);

	ObjNode->Diffuse=Diffuse;
	ObjNode->Specular=Specular;
	ObjNode->Ambient=Ambient;

	ObjNode->Scale=Scale;
	ObjNode->BuildChild();
	ObjNode->BuildGroup();///->CopyGroup

	if(RootLod)
	{
		ObjNode->RootLod=(cObjectNodeRoot*)RootLod->BuildCopy();
		ObjNode->SynchronizeGroupLod();
	}
}

void cObjectNodeRoot::ChangeBank(cAllMeshBank* new_root)
{
	cObjectNode::ChangeBank(new_root);
	vector<cObjectNode*>::iterator it;
	FOR_EACH(all_child,it)
		(*it)->ChangeBank(new_root);
}


void cObjectNodeRoot::BuildShadow()
{
	vector<cObjectNode*>::iterator it;
	FOR_EACH(all_child,it)
		(*it)->BuildShadow();

	FOR_EACH(all_child,it)
	if((*it)->shadow)
		(*it)->shadow->EndAdd();

	if(shadow)
		shadow->EndAdd();
}

void cObjectNodeRoot::GetLocalBorder(int *nVertex,Vect3f **Vertex,int *nIndex,short **Index)
{
	if(Base)
	{
		if(nVertex) *nVertex = Base->Vertex.size();
		if(Vertex)	*Vertex = &Base->Vertex[0];
		if(nIndex) *nIndex = Base->Poly.size();
		if(Index) *Index = (short*)&Base->Poly[0];
	}
	else
	{
		if(nVertex) *nVertex = 0;
		if(Vertex)	*Vertex = 0;
		if(nIndex) *nIndex = 0;
		if(Index) *Index = 0;
	}
}

void cObjectNodeRoot::SetBase(sBound *pBase)
{
	VISASSERT(Base==NULL);
	Base=pBase;
}

void cObjectNodeRoot::Animate(float dt)
{
	MaterialAnimTime+=dt;
	LightIntensity.set(0,0,0,0);
	point_light.clear();
}

void cObjectNodeRoot::PreDraw(cCamera *DrawNode)
{
	if(!observer.empty())
	{
		Update();
		observer.UpdateLink();
	}

	if(!DrawNode->TestVisible(GetGlobalMatrix(),GlobalBound.min,GlobalBound.max) )
		return;

	DrawNode->AttachTestShadow(this);
	Update();

	//////calc lod
	if(RootLod)
	{
		float d=DrawNode->GetPos().distance(GetGlobalMatrix().trans());
		float fLOD=GetScale().x*DrawNode->GetFocusViewPort().x/d;
		if(fLOD<Option_FarDistanceLOD)
		{
			RootLod->PreDraw(DrawNode);
			return;
		}
	}

	for(cObjectNode *child=GetChild();child;child=child->GetSibling())
		child->PreDraw(DrawNode);

	PreDrawSort(DrawNode);
	
	if(false)
	{
		Vect3f v1=GetGlobalMatrix()*GlobalBound.min;
		Vect3f v2=GetGlobalMatrix()*GlobalBound.max;
		gb_RenderDevice->DrawLine(v1,v2,sColor4c(255,255,255,255));
	}
}

void cObjectNodeRoot::DrawShadow(cCamera *DrawNode)
{
	Update();
	cObjectNode::DrawShadow(DrawNode);
	vector<cObjectNode*>::iterator it;
	FOR_EACH(all_child,it)
		(*it)->DrawShadow(DrawNode);
}

//Для объектов, в которых есть спецэффеты не 
//SetScale может давать неправильные результаты.
//Правильно вызывать сразу после создания.
void cObjectNodeRoot::SetScale(const Vect3f& scale)
{
	Vect3f s=scale;
	if(s.x<FLT_EPS) s.x=FLT_EPS;
	if(s.y<FLT_EPS) s.y=FLT_EPS; 
	if(s.z<FLT_EPS) s.z=FLT_EPS;
	Scale=s;
	if(RootLod)
		RootLod->SetScale(scale);

	//Фикс (кривой) для постоянно генерирующихся объектов
	vector<cObjectNode*>::iterator it;
	FOR_EACH(all_child,it)
	{
		cObjectNode* node=*it;
		RELEASE(node->effect);
	}
}

void cObjectNodeRoot::BuildChild()
{
	all_child.clear();
	mesh_child.clear();
	light_child.clear();
	AddChild(all_child);
	vector<cObjectNode*>::iterator it;
	FOR_EACH(all_child,it)
	{
		if((*it)->GetKind()==KIND_OBJMESH)
			mesh_child.push_back((cObjMesh*)*it);
	}
	FOR_EACH(all_child,it)
	{
		if((*it)->GetKind()==KIND_LIGHT)
			light_child.push_back((cObjLight*)*it);
	}

	FOR_EACH(all_child,it)
		(*it)->SetRootNode(this);
}

void cObjectNodeRoot::SetPosition(const MatXf& Matrix)
{
	CheckMatrix(Matrix);
	LocalMatrix=Matrix;
	GlobalMatrix=GetLocalMatrix();
	GlobalMatrix.rot().scale(GetScale());
//	CalcMatrix();//Debug
	if(RootLod)
		RootLod->SetPosition(Matrix);

	NodeAttribute.SetAttribute(ATTRNODE_UPDATEMATRIX);
}

void cObjectNodeRoot::BuildGroup()
{
	int num_group=GetNumGroup();
	groups.resize(num_group);

	int cur_num=0;
	SetGroup(cur_num,groups,NULL);
	VISASSERT(cur_num==num_group);
}

void cObjectNodeRoot::SetColor(const sColor4f *ambient,const sColor4f *diffuse,
							const sColor4f *specular)
{
	if(diffuse)
		Diffuse=*diffuse;
	if(specular)
		Specular=*specular;
	if(ambient)
		Ambient=*ambient;

//	VISASSERT(Diffuse.r>=0 && Diffuse.r<=1.0f);
//	VISASSERT(Diffuse.g>=0 && Diffuse.g<=1.0f);
//	VISASSERT(Diffuse.b>=0 && Diffuse.b<=1.0f);

	if(RootLod)
		RootLod->SetColor(ambient,diffuse,specular);
}

void cObjectNodeRoot::SetSkinColor(const sColor4f& color)
{
	SkinColor = color;
	if(RootLod)
		RootLod->SetSkinColor(color);
}

inline void cMeshSortingPhase::Attach(cObjMesh* p)
{
	p->pSortingNext=pMeshFirst;
	pMeshFirst=p;
}

void cObjectNodeRoot::PreDrawSort(cCamera *DrawNode)
{
	vector<cMeshSortingBank*>::iterator it_sort;
	FOR_EACH(DrawSort,it_sort)
		(*it_sort)->phase.clear();
	
	vector<cObjMesh*>::iterator it_mesh;
	FOR_EACH(mesh_child,it_mesh)
	{
		cMeshSortingBank* pSortBank=NULL;
		cObjMesh* pMesh=*it_mesh;
		if(pMesh->NodeAttribute.GetAttribute(ATTRNODE_IGNORE|ATTRNODE_CURFRAME_ALPHA))
			continue;

		FOR_EACH(DrawSort,it_sort)
		if((*it_sort)->pBank==pMesh->GetBank())
		{
			pSortBank=*it_sort;
			break;
		}
		
		if(pSortBank==NULL)
		{
			pSortBank=new cMeshSortingBank;
			pSortBank->pBank=pMesh->GetBank();
			DrawSort.push_back(pSortBank);
		}

		if(pSortBank->pBank->GetMaterial()->GetAttribute(MAT_IS_BLEND))
			continue;

		float alpha=pSortBank->pBank->GetRealAlpha(GetCurrentChannel(),GetPhase(),GetDiffuse());
		if(alpha<0.99f)
			continue;

		cMeshSortingPhase* pPhase=NULL;
		float phase;
		int channel=pMesh->GetCurrentChannel();
		
		if(pMesh->GetBank()->GetAnimChannelMat()->IsAnimMaterial(channel))
			phase=pMesh->GetPhase();
		else
			phase=0;

		int attribute=pMesh->GetAttribute(ATTRUNKOBJ_NOLIGHT);

		vector<cMeshSortingPhase>::iterator it_phase;
		FOR_EACH(pSortBank->phase,it_phase)
		if(it_phase->phase==phase && it_phase->channel==channel && 
			it_phase->attribute==attribute)
		{
			pPhase=&*it_phase;
			break;
		}
		if(pPhase==NULL)
		{
			pSortBank->phase.push_back(cMeshSortingPhase());
			pPhase=&pSortBank->phase.back();
			pPhase->phase=phase;
			pPhase->channel=channel;
			pPhase->attribute=attribute;
			pPhase->root=this;
			pPhase->pBank=pMesh->GetBank();
		}

		pPhase->Attach(pMesh);
	}

	FOR_EACH(DrawSort,it_sort)
	{
		cMeshSortingBank* pSortBank=*it_sort;
		vector<cMeshSortingPhase>::iterator it_phase;
		FOR_EACH(pSortBank->phase,it_phase)
		{
			cMeshSortingPhase& s=*it_phase;
			if(s.pBank->GetObjectAttribute().GetAttribute(ATTRUNKOBJ_COLLISIONTRACE))
			{
				s.diffuse=SkinColor.GetRGB();
			}else
			{
				s.diffuse=Diffuse.RGBA();
			}
			
			s.ambient=(Ambient+LightIntensity).RGBA();

			DrawNode->Attach(&s);
		}
	}
}

void cObjectNodeRoot::ClearDrawSort()
{
	vector<cMeshSortingBank*>::iterator it;
	FOR_EACH(DrawSort,it)
		delete *it;
	DrawSort.clear();
}

void cMeshSortingPhase::GetMaterial(sDataRenderMaterial* Data)
{
	cObjMaterial  Material;
	if(pBank->GetObjectAttribute().GetAttribute(ATTRUNKOBJ_COLLISIONTRACE))
	{
		sColor4f c=root->GetSkinColor();
		sColor4f zero(0,0,0,1);
		sColor4f half(c.r*0.5f,c.g*0.5f,c.b*0.5f,1);
		pBank->GetMaterial(channel,
			phase,attribute,
			half,c,zero,
			Material);
	}else
	{
		pBank->GetMaterial(channel,
			phase,attribute,
			root->GetAmbient(),root->GetDiffuse(),root->GetSpecular(),
			Material);
	}

	Material.Ambient+=root->GetLightIntensity();
	if(Material.Ambient.r>1)
		Material.Ambient.r=1;
	if(Material.Ambient.g>1)
		Material.Ambient.g=1;
	if(Material.Ambient.b>1)
		Material.Ambient.b=1;

	pBank->GetMaterial(channel,
		phase,
		GetFront()->GetRootNode()->GetMaterialAnimTime(),
		Material,*Data);
	
}

int cObjectNodeRoot::GetMaterialNum()
{
	return root->GetNumberMeshBank();
}

int cObjectNodeRoot::GetObjectNum()
{
	return all_child.size();
}

void cObjectNodeRoot::ClearAttr(int attribute)
{
	if(attribute==ATTRUNKOBJ_IGNORE && !GetAttr(ATTRUNKOBJ_IGNORE))
		return;
	if(RootLod)
		RootLod->ClearAttr(attribute);
	cObjectNode::ClearAttr(attribute);
}

void cObjectNodeRoot::SetAttr(int attribute)
{
	if(attribute==ATTRUNKOBJ_IGNORE && GetAttr(ATTRUNKOBJ_IGNORE))
		return;

	if(RootLod)
		RootLod->SetAttr(attribute);
	cObjectNode::SetAttr(attribute);
}

void cObjectNodeRoot::Update()
{
	if(!NodeAttribute.GetAttribute(ATTRNODE_UPDATEMATRIX))
		return;
	vector<cObjectNode*>::iterator it;
	FOR_EACH(all_child,it)
	{
		(*it)->UpdateMatrix();
	}

	NodeAttribute.ClearAttribute(ATTRNODE_UPDATEMATRIX);
}

void cObjectNodeRoot::GetAllPoints(vector<Vect3f>& point)
{
	MatXf save=GetLocalMatrix();
	SetPosition(MatXf::ID);
	Update();
	vector<cObjMesh*>::iterator it;
	FOR_EACH(mesh_child,it)
	{
		(*it)->GetAllPoints(point);
	}
	SetPosition(save);
	Update();
}

void cObjectNodeRoot::GetAllNormals(vector<Vect3f>& point)
{
	MatXf save=GetLocalMatrix();
	SetPosition(MatXf::ID);
	Update();
	vector<cObjMesh*>::iterator it;
	FOR_EACH(mesh_child,it)
	{
		(*it)->GetAllNormals(point);
	}
	SetPosition(save);
	Update();
}

void cObjectNodeRoot::GetAllTriangle(vector<Vect3f>& point,vector<sPolygon>& polygon)
{
	point.clear();
	polygon.clear();
	if(root->GetNumberMeshBank()==0)
		return;

	MatXf save=GetLocalMatrix();
	SetPosition(MatXf::ID);
	Update();

	//Довольно криво, предполагается, что у одного cObjectNodeRoot один cMeshStatic 
	cMeshStatic* bank=root->GetMeshBank(0)->GetBank();
	point.resize(bank->vb.ptr->NumberVertex);

	polygon.resize(bank->ib_polygon);
	sPolygon *IndexPolygon=gb_RenderDevice->LockIndexBuffer(bank->ib);
	for(int i=0;i<polygon.size();i++)
		polygon[i]=IndexPolygon[i];
	gb_RenderDevice->UnlockIndexBuffer(bank->ib);

	int filled_point=0;
	vector<cObjMesh*>::iterator it;
	FOR_EACH(mesh_child,it)
	{
		filled_point+=(*it)->GetAllTriangle(point,polygon);
	}

	VISASSERT(filled_point==point.size());
	SetPosition(save);
	Update();
}

void cObjectNodeRoot::OcclusionTest()
{
	vector<cObjLight*>::iterator it;
	FOR_EACH(light_child,it)
		(*it)->OcclusionTest();
}

////////////////////////////cObjectGroup//////////////////////
cObjectGroup::cObjectGroup()
{
	ChannelNumber=0;
	phase=0;
	lod=NULL;
}

void cObjectGroup::SetPhase(float StartPhase,bool recursive)
{
	phase=StartPhase;
	if(lod)
		lod->SetPhase(StartPhase,false);

	if(recursive)
		for(cObjectGroup *c=Child;c;c=c->Sibling)
			c->SetPhase(phase,recursive);
}

void cObjectGroup::SetCurrentChannel(int nChannel)
{
	ChannelNumber=nChannel;
}

const char* cObjectNode::GetCurrentChannelName() const
{
	return root->GetChannel(GetCurrentChannel()).name.c_str();
}

void cObjectNodeRoot::SynchronizeGroupLod()
{
	VISASSERT(RootLod);

	group->lod=RootLod;
	vector<cObjectNode*>::iterator it;
	FOR_EACH(all_child,it)
	{
		cObjectNode* cur=*it;
		if(cur->IsGroup())
		{
			const char* name=cur->GetNameObj();
			cObjectNode* lod=RootLod->FindObject(name);
			if(lod)
			{
				VISASSERT(lod->IsGroup());
				if(lod->IsGroup())
				{
					cur->group->lod=lod;
				}
			}
		}
	}
}

void cObjectNodeRoot::DrawBadUV(cCamera *DrawNode)
{
	vector<cObjMesh*>::iterator it;
	FOR_EACH(mesh_child,it)
	{
		(*it)->DrawBadUV(DrawNode);
	}
}

void cObjectNodeRoot::AddLight(cUnkLight* light)
{
	point_light.push_back(light);
}

void cObjectNodeRoot::GetEmitterMaterial(cObjMaterial& material)
{
	vector<cObjMesh*>& child=GetMeshChild();
	vector<cObjMesh*>::iterator it;
	FOR_EACH(child,it)
	{
		cMeshBank* mb=(*it)->GetBank();
		mb->GetMaterial(0,0,0,
			sColor4f(1,1,1,0),sColor4f(1,1,1,1),sColor4f(1,1,1,0),
			material);
		return;
	}
	xassert(0);
}
