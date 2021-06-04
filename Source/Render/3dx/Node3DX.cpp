#include "StdAfxRD.h"
#include "node3dx.h"
#include "..\shader\shaders.h"
#include "nparticle.h"
#include "scene.h"
#include "..\..\Util\SafeMath.h"

struct Shader3dx
{
	VSSkin vsSkin;
	PSSkin psSkin;
	VSSkinBump vsSkinBump;
	PSSkinBump psSkinBump;

	Shader3dx()
	{
		vsSkin.Restore();
		psSkin.Restore();
		vsSkinBump.Restore();
		psSkinBump.Restore();
	}
};
Shader3dx* pShader3dx=NULL;

void Done3dxshader()
{
	delete pShader3dx;
	pShader3dx=NULL;
}

void Init3dxshader()
{
	Done3dxshader();
	pShader3dx=new Shader3dx;
}

cNode3dx::cNode3dx()
{
	pos=MatXf::ID;
	phase=0;
	chain=0;
	index_scale=0;
	index_position=0;
	index_rotation=0;
	additional_transform=255;
}

cObject3dx::cObject3dx(cStatic3dx* pStatic_)
:pStatic(pStatic_),cIUnkClass(0)
{
	radius=0;
	ambient.set(1,1,1,0);
	diffuse.set(1,1,1,1);
	specular.set(1,1,1,0);
	skin_color.set(255,255,255,0);
	position=MatXf::ID;
	pGroup=pStatic->visibility_groups[0];
	scale=1;
	nodes.resize(pStatic->nodes.size());
	if(!pStatic->is_logic)
		CalcBoundingBox();
	else
		box=pStatic->logic_bound.bound;
	LoadTexture(false);
	effects.resize(pStatic->effects.size());
}

cObject3dx::~cObject3dx()
{
	int i;
	for(i=0;i<skin_texture.size();i++)
	if(skin_texture[i])
		skin_texture[i]->Release();
	skin_texture.clear();

	for(i=0;i<effects.size();i++)
	if(effects[i].pEffect)
		effects[i].pEffect->StopAndReleaseAfterEnd();
	effects.clear();
}

void cObject3dx::SetScale(float scale_)
{
	scale=scale_;
}

void cObject3dx::SetPosition(const Se3f&  pos)
{
	position=pos;
}

void cObject3dx::SetPosition(const MatXf& pos)
{
	position=pos;
}

const MatXf& cObject3dx::GetPosition() const
{
	return position;
}

void cObject3dx::Update()
{
	UpdateMatrix();
}

void cObject3dx::UpdateMatrix()
{
	xassert(!nodes.empty());
	nodes[0].pos=position;
	Mat3f& r=nodes[0].pos.rot();
/*
	//Mat3f correct;
	//correct.set(M_PI/2,X_AXIS);
	//r=correct*r;
	//r*=scale;
	//fix
	r[0].x=r[0].x*scale;
	r[0].y=r[0].y*scale;
	r[0].z=r[0].z*scale;
	Vect3f ry;
	ry.x=-r[2].x*scale;
	ry.y=-r[2].y*scale;
	ry.z=-r[2].z*scale;
	r[2].x=r[1].x*scale;
	r[2].y=r[1].y*scale;
	r[2].z=r[1].z*scale;

	r[1].x=ry.x;
	r[1].y=ry.y;
	r[1].z=ry.z;
/*/
	r*=scale;
/**/
	int size=nodes.size();
	for(int i=1;i<size;i++)
	{
		cNode3dx& node=nodes[i];
		cStaticNode& sn=pStatic->nodes[i];

		if(sn.iparent<0)//временно
			continue;

		xassert(sn.iparent>=0 && sn.iparent<size);
		cNode3dx& parent=nodes[sn.iparent];
		cStaticNodeChain& chain=sn.chains[node.chain];

		float scale;
		Se3f pos;
		float xyzs[4];
		node.index_scale=chain.scale.FindIndexRelative(node.phase,node.index_scale);
		node.index_position=chain.position.FindIndexRelative(node.phase,node.index_position);
		node.index_rotation=chain.rotation.FindIndexRelative(node.phase,node.index_rotation);

		chain.scale.Interpolate(node.phase,&scale,node.index_scale);
		chain.position.Interpolate(node.phase,(float*)&pos.trans(),node.index_position);
		chain.rotation.Interpolate(node.phase,xyzs,node.index_rotation);
		pos.rot().set(xyzs[3],-xyzs[0],-xyzs[1],-xyzs[2]);


		MatXf cur(pos);
		cur.rot()*=scale;
		if(node.IsAdditionalTransform())
		{
			cur*=additional_transformations[node.additional_transform].mat;
		}

//		node.pos=mat*cur;
		//node.pos=parent.pos*cur;
		node.pos.mult(parent.pos,cur);
		
/*
		if(sn.name=="Mesh01" && false)
		{
			dprintf("%.3f %i %.3f %.3f %.3f %.3f\n",node.phase,node.index_rotation,	xyzs[0],xyzs[1],xyzs[2],xyzs[3]);
		}
*/
		//node.pos=nodes[0].pos*pos;
	}
}

void cObject3dx::SetPhase(float phase)
{
	int size=nodes.size();
	for(int i=1;i<size;i++)
	{
		cNode3dx& node=nodes[i];
		node.phase=phase;
	}
}

void cObject3dx::DrawLine(cCamera* pCamera)
{
	UpdateMatrix();
	sColor4c color(255,255,255);
	sColor4c c0(255,0,0);
	int size=nodes.size();
	for(int i=1;i<size;i++)
	{
		cNode3dx& node=nodes[i];
		cStaticNode& sn=pStatic->nodes[i];
		cNode3dx& parent=nodes[sn.iparent];

		gb_RenderDevice->DrawLine(parent.pos.trans(),node.pos.trans(),sn.iparent==0?c0:color);
	}
}

void cObject3dx::Draw(cCamera* pCamera)
{
//	DrawLine(pCamera);
	UpdateMatrix();

//	psSkin.Select();
	sDataRenderMaterial material;
	material.Ambient=sColor4f(0.2f,0.2f,0.2f);
	material.Diffuse=sColor4f(1,1,1);
	material.Specular=sColor4f(0.5f,0.5f,0.5f);
	material.Emissive=sColor4f(0,0,0);
	material.Power=16;
	material.Phase=0;
	material.mat=MAT_LIGHT;
	material.Tex[0]=material.Tex[1]=NULL;
	material.TexMatrix=MatXf::ID;
	material.MaterialAnimPhase=0;

	gb_RenderDevice3D->SetRenderState(D3DRS_ZFUNC,D3DCMP_LESSEQUAL);
	gb_RenderDevice3D->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);
	gb_RenderDevice3D->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
	gb_RenderDevice3D->SetTexture(material.Tex[0],material.Phase,0);
	gb_RenderDevice3D->SetBlendState(ALPHA_NONE);

	gb_RenderDevice3D->SetRenderState(D3DRS_FOGENABLE,FALSE);
	for(int i=0;i<2;i++)
	{
		gb_RenderDevice3D->SetSamplerState(i,D3DSAMP_ADDRESSU,D3DTADDRESS_WRAP);
		gb_RenderDevice3D->SetSamplerState(i,D3DSAMP_ADDRESSV,D3DTADDRESS_WRAP);
		gb_RenderDevice3D->SetSamplerState(i,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
		gb_RenderDevice3D->SetSamplerState(i,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
		gb_RenderDevice3D->SetSamplerState(i,D3DSAMP_MIPFILTER,D3DTEXF_LINEAR);
	}
/*

//	vsSkin.SetMaterial(&material);
	gb_RenderDevice3D->SetFVF(pStatic->vb);
	int size=pStatic->meshes.size();
	for(int i=0;i<size;i++)
	{
		cStaticMesh& mesh=pStatic->meshes[i];
		cNode3dx& node=nodes[mesh.inode];

//		vector<MatXf> world;
//		world.push_back(node.pos);
//		vsSkin.Select(gb_RenderDevice3D->GetDrawNode()->matViewProj,1,world,NULL);

		gb_RenderDevice3D->SetMatrix(D3DTS_WORLD,node.pos);
		gb_RenderDevice3D->DrawIndexedPrimitive(
			pStatic->vb,mesh.offset_vertex,mesh.num_vertex,
			pStatic->ib,mesh.offset_polygon,mesh.num_polygon);
	}

		//gb_RenderDevice3D->SetFVF(pStatic->vb);
		//gb_RenderDevice3D->SetMatrix(D3DTS_WORLD,nodes[0].pos);
		//gb_RenderDevice3D->DrawIndexedPrimitive(
		//	pStatic->vb,0,pStatic->vb.ptr->NumberVertex,
		//	pStatic->ib,0,pStatic->ib_polygon);
/*/
	//Из плюсов - выводит стабильное количество полигонов вне зависимости
	//от количества подобъектов. Из минусов - для больших объектов не слишком
	//эффективен. indexed 14 mtrtis, nonindexed 26 mtris - пиковые значения на FX 5950.

	//!!! Не забыть сортировку по материалам.

	int size=pGroup->skin_group.size();
	vector<MatXf> world;
	for(int i=0;i<size;i++)
	{
		cStaticIndex& s=pGroup->skin_group[i];
		cStaticMaterial& mat=pStatic->materials[s.imaterial];
		cTexture* diffuse_texture=skin_texture[s.imaterial];
		material.Ambient.interpolate3(mat.ambient,ambient,ambient.a);
		material.Ambient.a=0;
		material.Diffuse=mat.diffuse*diffuse;
		material.Specular.interpolate3(mat.specular,specular,specular.a);
		material.Emissive=sColor4f(0,0,0);
		material.Power=mat.specular_power;
		material.Phase=0;
		material.mat=MAT_LIGHT;
		material.Tex[0]=diffuse_texture;
		material.Tex[1]=mat.pBumpTexture;
		material.TexMatrix=MatXf::ID;
		material.MaterialAnimPhase=0;
		gb_RenderDevice3D->SetTexture(material.Tex[0],material.Phase,0);
		gb_RenderDevice3D->SetTexture(material.Tex[1],material.Phase,1);

		world.resize(s.node_index.size());//статический массив должен быть
		for(int j=0;j<s.node_index.size();j++)
		{
			int inode=s.node_index[j];
			cNode3dx& node=nodes[inode];
			world[j]=node.pos;
		}

		if(mat.pBumpTexture)
		{
			pShader3dx->vsSkinBump.SetMaterial(&material);
			pShader3dx->vsSkinBump.Select(gb_RenderDevice3D->GetDrawNode()->matViewProj,1,world,NULL,pGroup->blend_indices);
			pShader3dx->psSkinBump.SetMaterial(&material);
			pShader3dx->psSkinBump.Select();
		}else
		{
			pShader3dx->vsSkin.SetMaterial(&material);
			pShader3dx->vsSkin.Select(gb_RenderDevice3D->GetDrawNode()->matViewProj,1,world,NULL,pGroup->blend_indices);
			pShader3dx->psSkin.Select();
		}

		gb_RenderDevice3D->SetFVF(pGroup->vb);
//		gb_RenderDevice3D->SetMatrix(D3DTS_WORLD,node.pos);
		gb_RenderDevice3D->DrawIndexedPrimitive(
			pGroup->vb,s.offset_vertex,s.num_vertex,
			pGroup->ib,s.offset_polygon,s.num_polygon);
	}
	
/**/
	gb_RenderDevice3D->SetVertexShader(NULL);
	gb_RenderDevice3D->SetPixelShader(NULL);
}

int cObject3dx::GetAnimationGroup(const char* name)
{
	vector<AnimationGroup>& animation_group=pStatic->animation_group;
	int size=animation_group.size();
	for(int i=0;i<size;i++)
	{
		AnimationGroup& ac=animation_group[i];
		if(ac.name==name)
			return i;
	}
	xassert(0);
	return -1;
}

int cObject3dx::GetAnimationGroupNumber()
{
	return pStatic->animation_group.size();
}

const char* cObject3dx::GetAnimationGroupName(int igroup)
{
	return pStatic->animation_group[igroup].name.c_str();
}

void cObject3dx::SetAnimationGroupPhase(int igroup,float phase)
{
	if(!(igroup>=0 && igroup<pStatic->animation_group.size()))
	{
		xassert(0 && "Bad group index");
		return;
	}
	AnimationGroup& ag=pStatic->animation_group[igroup];

	int size=ag.nodes.size();
	for(int iag=0;iag<size;iag++)
	{
		int inode=ag.nodes[iag];
		cNode3dx& node=nodes[inode];
		node.phase=phase;
	}
}

bool cObject3dx::SetVisibilityGroup(const char* name)
{
	vector<cStaticVisibilityChainGroup*>::iterator it;
	FOR_EACH(pStatic->visibility_groups,it)
	{
		string& cname=(*it)->name;
		if(cname==name)
		{
			pGroup=*it;
			return true;
		}
	}

	xassert(0);
	return false;
}

int cObject3dx::GetChainNumber()
{
	return pStatic->animation_chain.size();
}

cAnimationChain* cObject3dx::GetChain(int i)
{
	xassert(i>=0 && i<pStatic->animation_chain.size());
	return &pStatic->animation_chain[i];
}

int cObject3dx::FindChain(const char* chain_name)
{
	int size=pStatic->animation_chain.size();
	for(int i=0;i<size;i++)
	{
		cAnimationChain& ac=pStatic->animation_chain[i];
		if(ac.name==chain_name)
			return i;
	}

	return -1;
}

bool cObject3dx::SetChain(const char* chain_name)
{
	int chain=FindChain(chain_name);
	if(chain<0)
	{
		xassert(0);
		return false;
	}

	vector<cNode3dx>::iterator it;
	FOR_EACH(nodes,it)
	{
		cNode3dx& node=*it;
		node.chain=chain;
		node.index_scale=
		node.index_position=
		node.index_rotation=0;
	}

	return true;
}

void cObject3dx::SetAnimationGroupChain(int igroup,const char* chain_name)
{
	int chain_index=FindChain(chain_name);
	if(chain_index<0)
	{
		xassert(0);
		return;
	}

	SetAnimationGroupChain(igroup,chain_index);
}

void cObject3dx::SetAnimationGroupChain(int igroup,int chain_index)
{
	if(!(igroup>=0 && igroup<pStatic->animation_group.size()))
	{
		xassert(0 && "Bad group index");
		return;
	}
	AnimationGroup& ag=pStatic->animation_group[igroup];

	int size=ag.nodes.size();
	for(int iag=0;iag<size;iag++)
	{
		int inode=ag.nodes[iag];
		cNode3dx& node=nodes[inode];
		node.chain=chain_index;
		node.index_scale=
		node.index_position=
		node.index_rotation=0;
	}
}


int cObject3dx::GetAnimationGroupChain(int igroup)
{
	if(!(igroup>=0 && igroup<pStatic->animation_group.size()))
	{
		xassert(0 && "Bad group index");
		return 0;
	}
	AnimationGroup& ag=pStatic->animation_group[igroup];
	int size=ag.nodes.size();
	if(size==0)
		return 0;
	int inode=ag.nodes[0];
	cNode3dx& node=nodes[inode];
	return node.chain;
}

void cObject3dx::PreDraw(cCamera *pCamera)
{
	ProcessEffect(pCamera);
	pCamera->Attach(SCENENODE_OBJECT,this);
}

void cObject3dx::Animate(float dt)
{
}

float cObject3dx::GetBoundRadius()
{
	sBox6f box;
	GetBoundBox(box);
	float r=box.max.distance(box.min)*0.5f;
	return radius;
}

void cObject3dx::GetBoundBox(sBox6f& box_)
{
	box_=box;
}

void cObject3dx::CalcBoundingBox()
{
	box.min.set(1e30f,1e30f,1e30f);
	box.max.set(-1e30f,-1e30f,-1e30f);

	vector<Vect3f> points;
	GetAllPoints(points);
	vector<Vect3f>::iterator it;
	FOR_EACH(points,it)
	{
		Vect3f& pos=*it;
		box.AddBound(pos);
		float r=pos.norm();
		radius=max(radius,r);
	}
}

void cObject3dx::GetAllPoints(vector<Vect3f>& point)
{
	GetAllPointsParam(point,true);
}

void cObject3dx::GetAllPointsParam(vector<Vect3f>& point,bool zero_pos)
{
	MatXf old_mat;
	float old_scale;
	if(zero_pos)
	{
		old_mat=GetPosition();
		old_scale=GetScale();
		SetScale(1);
		SetPosition(MatXf::ID);
		UpdateMatrix();
	}

	int num_points=0;
	int size=pGroup->skin_group.size();
	int i;
	for(i=0;i<size;i++)
	{
		cStaticIndex& s=pGroup->skin_group[i];
		num_points+=s.num_vertex;
	}

	point.resize(num_points);

	int pointindex=0;
	cSkinVertex skin_vertex(pGroup->GetBlendWeight(),pGroup->bump);
	void *pVertex=gb_RenderDevice->LockVertexBuffer(pGroup->vb);
	skin_vertex.SetVB(pVertex,pGroup->vb.size);

	vector<MatXf> world;
	for(i=0;i<size;i++)
	{
		cStaticIndex& s=pGroup->skin_group[i];

		world.resize(s.node_index.size());
		for(int j=0;j<s.node_index.size();j++)
		{
			int inode=s.node_index[j];
			cNode3dx& node=nodes[inode];
			world[j]=node.pos;
		}

		int max_vertex=s.offset_vertex+s.num_vertex;
		int blend_weight=pGroup->GetBlendWeight();
		float int2float=1/255.0f;
		for(int vertex=s.offset_vertex;vertex<max_vertex;vertex++)
		{
			skin_vertex.Select(vertex);
			Vect3f global_pos;
			if(pGroup->blend_indices==1)
			{
				int idx=skin_vertex.GetIndex()[0];
				Vect3f pos=skin_vertex.GetPos();
				global_pos=world[idx]*pos;
			}else
			{
				global_pos=Vect3f::ZERO;
				Vect3f pos=skin_vertex.GetPos();
				for(int ibone=0;ibone<blend_weight;ibone++)
				{
					int idx=skin_vertex.GetIndex()[ibone];
					float weight=skin_vertex.GetWeight(ibone)*int2float;
					global_pos+=(world[idx]*pos)*weight;
				}
			}

			point[pointindex++]=global_pos;
		}
	}

	xassert(point.size()==pointindex);

	gb_RenderDevice->UnlockVertexBuffer(pGroup->vb);

	if(zero_pos)
	{
		SetScale(old_scale);
		SetPosition(old_mat);
		UpdateMatrix();
	}
}

void cObject3dx::GetAllNormals(vector<Vect3f>& point)
{
	MatXf old_mat=GetPosition();
	float old_scale=GetScale();
	SetScale(1);
	SetPosition(MatXf::ID);
	UpdateMatrix();

	int num_points=0;
	int size=pGroup->skin_group.size();
	int i;
	for(i=0;i<size;i++)
	{
		cStaticIndex& s=pGroup->skin_group[i];
		num_points+=s.num_vertex;
	}

	point.resize(num_points);

	int pointindex=0;
	cSkinVertex skin_vertex(pGroup->GetBlendWeight(),pGroup->bump);
	void *pVertex=gb_RenderDevice->LockVertexBuffer(pGroup->vb);
	skin_vertex.SetVB(pVertex,pGroup->vb.size);

	float int2float=1/255.0f;
	vector<MatXf> world;
	for(i=0;i<size;i++)
	{
		cStaticIndex& s=pGroup->skin_group[i];

		world.resize(s.node_index.size());
		for(int j=0;j<s.node_index.size();j++)
		{
			int inode=s.node_index[j];
			cNode3dx& node=nodes[inode];
			world[j]=node.pos;
		}

		int max_vertex=s.offset_vertex+s.num_vertex;
		int blend_weight=pGroup->GetBlendWeight();
		for(int vertex=s.offset_vertex;vertex<max_vertex;vertex++)
		{
			skin_vertex.Select(vertex);
			Vect3f global_pos;
			if(pGroup->blend_indices==1)
			{
				int idx=skin_vertex.GetIndex()[0];
				Vect3f pos=skin_vertex.GetNorm();
				global_pos=world[idx].rot()*pos;
			}else
			{
				global_pos=Vect3f::ZERO;
				Vect3f pos=skin_vertex.GetNorm();
				for(int ibone=0;ibone<blend_weight;ibone++)
				{
					int idx=skin_vertex.GetIndex()[ibone];
					float weight=skin_vertex.GetWeight(ibone)*int2float;
					global_pos+=(world[idx].rot()*pos)*weight;
				}
			}

			point[pointindex++]=global_pos;
		}
	}

	xassert(point.size()==pointindex);

	gb_RenderDevice->UnlockVertexBuffer(pGroup->vb);

	SetScale(old_scale);
	SetPosition(old_mat);
	UpdateMatrix();
}

void cObject3dx::GetEmitterMaterial(struct cObjMaterial& material)
{
	xassert(0);
}

const char* cObject3dx::GetFileName()
{
	return pStatic->file_name.c_str();
}

int cObject3dx::FindNode(const char* node_name)
{
	int n_size=pStatic->nodes.size();
	for(int i=0;i<n_size;i++)
	{
		cStaticNode& node=pStatic->nodes[i];
		if(node.name==node_name)
			return i;
	}

	return -1;
}

void cObject3dx::SetSkinColor(sColor4f skin_color_)
{
	skin_color=skin_color_;
	LoadTexture(true);
}

void cObject3dx::LoadTexture(bool skinned)
{
	int m_size=pStatic->materials.size();
	if(skin_texture.empty())
	{
		skin_texture.resize(m_size);
	}else
	{
		xassert(skin_texture.size()==m_size);
	}

	for(int i=0;i<m_size;i++)
	{
		cStaticMaterial& mat=pStatic->materials[i];
		if(mat.is_skinned && skinned)
		{
			RELEASE(skin_texture[i]);
			skin_texture[i]=GetTexLibrary()->GetElementColor(mat.tex_diffuse.c_str(),skin_color);
		}

		if(!mat.is_skinned && !skinned)
		{
			RELEASE(skin_texture[i]);
			skin_texture[i]=GetTexLibrary()->GetElement(mat.tex_diffuse.c_str());
		}
	}
}

void cObject3dx::GetLocalBorder(int *nVertex,Vect3f **Vertex,int *nIndex,short **Index)
{
	cStaticBasement& b=pStatic->basement;
	*nVertex=b.vertex.size();
	*Vertex=&b.vertex[0];
	*nIndex=b.polygons.size();
	*Index=(short*)&b.polygons[0];

}

void cObject3dx::DrawLogicBound()
{
	sBox6f& box=pStatic->logic_bound.bound;
	MatXf pos=GetPosition();
	pos.rot()*=scale;
	gb_RenderDevice->DrawBound(pos,box.min,box.max,0,sColor4c(255,0,0,255));
}

void DrawPointer(MatXf m,sColor4c color,float len,int xyz);
static void DrawLogicNode(MatXf& pos,float logic_radius, bool all)
{
	sColor4c color;
	if(all)
	{
		DrawPointer(pos,sColor4c(255,0,0),logic_radius*0.1f,0);
		DrawPointer(pos,sColor4c(0,255,0),logic_radius*0.1f,1);
		DrawPointer(pos,sColor4c(0,0,255),logic_radius*0.1f,2);
	}else
	{
		DrawPointer(pos,sColor4c(255,255,255),logic_radius*0.1f,1);
	}
}

void cObject3dx::DrawLogic()
{
	UpdateMatrix();

	cCamera *pCamera=gb_RenderDevice->GetDrawNode();
	Vect3f pos=pCamera->GetMatrix()*position.trans();
	float min_size=100000/pos.z;
	float radius=max(scale*GetBoundRadius(),min_size);
	int size=nodes.size();
	for(int i=0;i<size;i++)
	{
		cNode3dx& node=nodes[i];
		cStaticNode& sn=pStatic->nodes[i];

		DrawLogicNode(node.pos,radius,true);
	}
}

const MatXf& cObject3dx::GetNodePosition(int nodeindex)
{
	xassert(nodeindex>=0 && nodeindex<nodes.size());
	cNode3dx& s=nodes[nodeindex];
	return s.pos;
}

void cObject3dx::SetUserTransform(int nodeindex,const MatXf& pos)
{
	xassert(nodeindex>=0 && nodeindex<nodes.size());
	cNode3dx& s=nodes[nodeindex];
	if(s.IsAdditionalTransform())
	{
		BYTE add_index=s.additional_transform;
		xassert(add_index<additional_transformations.size());
		additional_transformations[add_index].mat=pos;
	}else
	{
		int found_index=-1;
		int size=additional_transformations.size();
		xassert(size<254);
		for(int i=0;i<size;i++)
		if(additional_transformations[i].nodeindex==-1)
		{
			found_index=i;
			break;
		}
		if(found_index==-1)
		{
			s.additional_transform=size;
			AdditionalTransformation n;
			n.nodeindex=nodeindex;
			n.mat=pos;
			additional_transformations.push_back(n);
		}else
		{
			s.additional_transform=found_index;
			additional_transformations[found_index].nodeindex=nodeindex;
			additional_transformations[found_index].mat=pos;
		}

	}
}

void cObject3dx::RestoreUserTransform(int nodeindex)
{
	xassert(nodeindex>=0 && nodeindex<nodes.size());
	cNode3dx& s=nodes[nodeindex];
	if(!s.IsAdditionalTransform())
		return;

	xassert(s.additional_transform<additional_transformations.size());
	additional_transformations[s.additional_transform].nodeindex=-1;
	s.additional_transform=255;
	xassert(!s.IsAdditionalTransform());
}

void cObject3dx::ProcessEffect(cCamera *pCamera)
{
	if(!observer.empty())
	{
		Update();
		observer.UpdateLink();
	}

	int effect_size=effects.size();
	for(int ieffect=0;ieffect<effect_size;ieffect++)
	{
		EffectData& e=effects[ieffect];
		cStaticEffect& se=pStatic->effects[ieffect];
		cStaticNode& snode=pStatic->nodes[se.node];
		cNode3dx& node=nodes[se.node];
		cStaticNodeChain& chain=snode.chains[node.chain];

		xassert(!chain.visibility.data.empty());

		if(e.index_visibility==255)
		{
			e.index_visibility=chain.visibility.FindIndex(node.phase);
			e.prev_phase=node.phase;
		}

		if(se.is_cycled)
		{
			xassert(0 && "Непроверенно");
			e.index_visibility=chain.visibility.FindIndexRelative(node.phase,e.index_visibility);
			bool visible=false;
			chain.visibility.Interpolate(node.phase,&visible,e.index_visibility);
			if(visible && !e.pEffect)
			{
				e.pEffect=pCamera->GetScene()->CreateEffect(*se.effect_key,this,scale);
				e.pEffect->LinkToNode(this,se.node);
			}

			if(e.pEffect)
				e.pEffect->SetParticleRate(visible?1.0f:0);
		}else
		{
			///Ищем 
			float delta_plus,delta_minus;
			if(node.phase>e.prev_phase)
			{
				delta_plus=node.phase-e.prev_phase;
				delta_minus=1-delta_plus;
			}else
			{
				delta_minus=e.prev_phase-node.phase;
				delta_plus=1-delta_minus;
			}

			bool is_up=false;
			Interpolator3dxBool::darray& data=chain.visibility.data;
			int dsize=data.size();
			if(delta_plus<delta_minus)
			{
				int cur=e.index_visibility;
				bool prev_visibly=data[cur].a;
				
				do
				{
					sInerpolate3dxBool& p=data[cur];

					if(p.a && !prev_visibly)
						is_up=true;
					
					float t=(node.phase-p.tbegin)*p.inv_tsize;
					if(t>=0 && t<=1)
					{
						break;
					}

					prev_visibly=p.a;

					cur++;
					if(cur>=dsize)
						cur=0;
				}while(cur!=e.index_visibility);

				e.index_visibility=cur;
				
			}else
			{
				int cur=e.index_visibility;
				bool prev_visibly=data[cur].a;
				
				do
				{
					sInerpolate3dxBool& p=data[cur];

					if(p.a && !prev_visibly)
						is_up=true;
					
					float t=(node.phase-p.tbegin)*p.inv_tsize;
					if(t>=0 && t<=1)
					{
						break;
					}

					prev_visibly=p.a;

					cur--;
					if(cur<0)
						cur=dsize-1;
				}while(cur!=e.index_visibility);

				e.index_visibility=cur;
			}

			if(is_up)
			{
				cEffect* eff=pCamera->GetScene()->CreateEffect(*se.effect_key,this,scale,true);
				eff->LinkToNode(this,se.node);
			}
		}

		e.prev_phase=node.phase;
	}

}

void cObject3dx::SetColor(const sColor4f *ambient_,const sColor4f *diffuse_,const sColor4f *specular_)
{
	if(ambient_)
		ambient=*ambient_;
	if(diffuse_)
		diffuse=*diffuse_;
	if(specular_)
		specular=*specular_;
}

void cObject3dx::GetColor(sColor4f *ambient_,sColor4f *diffuse_,sColor4f *specular_) const
{
	if(ambient_)
		*ambient_=ambient;
	if(diffuse_)
		*diffuse_=diffuse;
	if(specular_)
		*specular_=specular;
}
/*
bool IntersectSphere(const MatXf& pos,float radius,const Vect3f& xp0,const Vect3f& xp1)
{
	MatXf m=pos;
	m.Invert();
	Vect3f p0=m*xp0,p1=m*xp1;

	Vect3f Center(0,0,0);
	float R2=radius*radius;

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
/*/
bool IntersectSphere(const MatXf& pos,float radius,const Vect3f& p0,const Vect3f& p1)
{
	Vect3f Center=pos.trans();
	float R2=radius*radius;

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
/**/

bool Intersection(const Vect3f& a,const Vect3f& b,const Vect3f& c,
		const Vect3f& p0,const Vect3f& p1,const Vect3f& pn,float& t);

bool cObject3dx::Intersect(const Vect3f& p0,const Vect3f& p1)
{
	if(!IntersectSphere(GetPosition(),GetBoundRadius()*scale,p0,p1))
		return false;
	return IntersectTriangle(p0,p1);
}

bool cObject3dx::IntersectTriangle(const Vect3f& p0,const Vect3f& p1)
{
	vector<Vect3f> point;
	GetAllPointsParam(point,false);

	Vect3f pn=p1-p0;
	bool intersect=false;
	sPolygon* pPolygon=gb_RenderDevice->LockIndexBuffer(pGroup->ib);

	for(int i=0;i<pGroup->ib_polygon;i++)
	{
		sPolygon& p=pPolygon[i];
		float t;
		if(Intersection(point[p[0]],point[p[1]],point[p[2]],
			p0,p1,pn,t))
		{
			intersect=true;
			break;
		}
	}

	gb_RenderDevice->UnlockIndexBuffer(pGroup->ib);
	return intersect;
}
