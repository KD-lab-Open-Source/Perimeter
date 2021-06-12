#include "StdAfxRD.h"
#include "ObjNode.h"
#include "ObjMesh.h"
#include "AnimChannel.h"
#include "MeshBank.h"

cObjMesh::cObjMesh():cObjectNode(KIND_OBJMESH)
{ 
	Tri=0;
	pBank=NULL;
	pOriginalBank=NULL;
}
cObjMesh::~cObjMesh()
{
	Tri=0;
}
//////////////////////////////////////////////////////////////////////////////////////////
// реализация интерфейса cUnkObj
//////////////////////////////////////////////////////////////////////////////////////////

void cObjMesh::PreDraw(cCamera *DrawNode)
{ 
	cObjectNode::PreDraw(DrawNode);

	float alpha=pBank->GetRealAlpha(GetCurrentChannel(),GetPhase(),GetDiffuse());
	if(NodeAttribute.GetAttribute(ATTRNODE_IGNORE)||(alpha<0.004f)) return;

	if(pBank->GetMaterial()->GetAttribute(MAT_IS_BLEND)==0 && alpha>0.99f)
	{
		NodeAttribute.ClearAttribute(ATTRNODE_CURFRAME_ALPHA);
	}else
	{
		DrawNode->Attach(SCENENODE_OBJECTSORT,this);
		NodeAttribute.SetAttribute(ATTRNODE_CURFRAME_ALPHA);
	}

}
void cObjMesh::Draw(cCamera *DrawNode)
{

	if(!DrawNode->GetAttribute(ATTRCAMERA_SHADOW))
	{
		if(NodeAttribute.GetAttribute(ATTRNODE_CURFRAME_ALPHA))
		if(DrawNode->GetAttribute(ATTRCAMERA_REFLECTION) && (GetGlobalMatrix().trans().z<DrawNode->GetHReflection()))
			return;

		sDataRenderMaterial Data;
		int attribute=GetAttribute();
		int nChannel=GetCurrentChannel();
		
		float mesh_phase=GetPhase();
		pBank->GetMaterial(nChannel,mesh_phase,GetRootNode()->GetMaterialAnimTime(),
					attribute,
					GetAmbient(),GetDiffuse(),GetSpecular(),Data);

		gb_RenderDevice3D->dtFixed->SetSimplyMaterial(this,&Data);
		gb_RenderDevice3D->dtFixed->DrawNoMaterial(this,&Data);

		if(Option_DrawMeshBound)
			DrawNode->GetRenderDevice()->DrawBound(GetGlobalMatrix(),
				GlobalBound.min,GlobalBound.max,Option_DrawMeshBoundWire);
	}
}
void cObjMesh::SetColor(const sColor4f *pAmbient,const sColor4f *Diffuse,const sColor4f *Specular)
{
	VISASSERT(0);
}
cIUnkObj* cObjMesh::BuildCopy()
{
	cObjMesh *Mesh=new cObjMesh;
	cObjectNode::SetCopy(Mesh);
	Mesh->Tri=Tri;
	Mesh->pBank=pBank;
	Mesh->pOriginalBank=pOriginalBank;
	return Mesh;
}
void cObjMesh::CalcObj()
{
	cObjectNode::CalcObj();
	if(GetGlobalMatrix().rot().det()<0)
		GetTri()->InvertTri();
}
void cObjMesh::GetBoundingBox(Vect3f &Min,Vect3f &Max)
{ 
	GetTri()->GetBoundingBox(Min,Max); 
	Min=GetGlobalMatrix().xformPoint(Min);
	Max=GetGlobalMatrix().xformPoint(Max);
}

void cObjMesh::CalcBorder(Vect3f& Min,Vect3f& Max)
{
	Vect3f min(1e30f,1e30f,1e30f),max(-1e30f,-1e30f,-1e30f);
	GetTri()->GetBoundingBox(min,max);
	if(fabs(min.x)<1e20f)
	{
		Min=::GetMin(min,Min);
		Min=::GetMin(max,Min);
		Max=::GetMax(min,Max);
		Max=::GetMax(max,Max);
	}

	cObjectNode::CalcBorder(Min,Max);
}

void cObjMesh::SetBank(cMeshBank* _pBank,bool is_original)
{
	pBank=_pBank;
	if(is_original)
		pOriginalBank=_pBank;
}

void cObjMesh::RestoreOriginalMaterial()
{
	RestoreOriginalBank();
	cObjectNode::RestoreOriginalMaterial();
}

void cObjMesh::SetAnotherMaterial(cTexture *Tex1,cTexture *Tex2,AllAnotherMaterial* aroot)
{
	cMeshBank* pOriginalBank=GetBank();
	cMeshBank* pAMat=aroot->Find(pOriginalBank);
	if(pAMat==NULL)
	{
		pAMat=aroot->Add(pOriginalBank);
		sAttribute* mat=pAMat->GetMaterial();
		mat->ClearAttribute(0xFFFFFFFF);
		mat->SetAttribute(//MAT_LIGHT|
						MAT_ALPHA_ADDBLENDALPHA);

		pAMat->SetTexture(1,NULL/*Tex1*/);
		pAMat->SetTexture(2,Tex2);
	}

	SetBank(pAMat,false);

	cObjectNode::SetAnotherMaterial(Tex1,Tex2,aroot);
}

bool cObjMesh::Intersect(const Vect3f& p0,const Vect3f& p1)
{
	if(NodeAttribute.GetAttribute(ATTRNODE_IGNORE))
		return false;
	float alpha=pBank->GetRealAlpha(GetCurrentChannel(),GetPhase(),GetDiffuse());
	if(alpha<0.3f)
		return false;

	MatXf m=GetGlobalMatrix();
	m.Invert();
	Vect3f pi0=m*p0,pi1=m*p1;
	if(Tri->Intersect(pi0,pi1))
		return true;
	return cObjectNode::Intersect(p0,p1);
}

void cObjMesh::GetAllPoints(vector<Vect3f>& point)
{
	if(!Tri)return;
	void *pVertex=gb_RenderDevice->LockVertexBuffer(*Tri->vb);
	for(int i=0;i<Tri->NumVertex;i++)
	{
		Vect3f &v=Tri->GetPos(pVertex,i+Tri->OffsetVertex);
		point.push_back(GetGlobalMatrix()*v);
	}
	gb_RenderDevice->UnlockVertexBuffer(*Tri->vb);
}

void cObjMesh::GetAllNormals(vector<Vect3f>& point)
{
	if(!Tri)return;
	void *pVertex=gb_RenderDevice->LockVertexBuffer(*Tri->vb);
	for(int i=0;i<Tri->NumVertex;i++)
	{
		Vect3f &v=Tri->GetNormal(pVertex,i+Tri->OffsetVertex);
		point.push_back(GetGlobalMatrix().rot()*v);
	}
	gb_RenderDevice->UnlockVertexBuffer(*Tri->vb);
}

int cObjMesh::GetAllTriangle(vector<Vect3f>& point,vector<sPolygon>& polygon)
{
	if(!Tri)return 0;
	if(point.size()!=Tri->vb->ptr->NumberVertex)
	{
		return 0;
	}
	VISASSERT(point.size()==Tri->vb->ptr->NumberVertex);

	void *pVertex=gb_RenderDevice->LockVertexBuffer(*Tri->vb);
	int imin=Tri->OffsetVertex,
		imax=imin+Tri->NumVertex;
	for(int i=imin;i<imax;i++)
	{
		Vect3f &v=Tri->GetPos(pVertex,i);
		point[i]=GetGlobalMatrix()*v;
	}
	gb_RenderDevice->UnlockVertexBuffer(*Tri->vb);
	return Tri->NumVertex;
}

void cObjMesh::BuildShadow()
{
	AddShadow(MatXf::ID,Tri);
}

void cObjMesh::ChangeBank(cAllMeshBank* new_root)
{
	cObjectNode::ChangeBank(new_root);
	cMeshBank* new_bank=root->FindUnical(pOriginalBank->GetMaterialName(),pOriginalBank->GetObjectAttribute(),*pOriginalBank->GetMaterial());

	VISASSERT(new_bank);
	SetBank(new_bank,true);
}

void cObjMesh::DrawBadUV(cCamera *DrawNode)
{
	if(!Tri)return;
	void *pVertex=gb_RenderDevice->LockVertexBuffer(*Tri->vb);
	for(int i=0;i<Tri->NumVertex;i++)
	{
		sVertexXYZNT1 &v=Tri->GetVertex(pVertex,i+Tri->OffsetVertex);
		if(!(v.uv[0]>-100 && v.uv[0]<100 && v.uv[1]>-100 && v.uv[1]<100))
		{
			gb_RenderDevice->DrawPoint(GetGlobalMatrix()*v.pos,sColor4c(255,0,0,255));
		}
	}
	gb_RenderDevice->UnlockVertexBuffer(*Tri->vb);
}