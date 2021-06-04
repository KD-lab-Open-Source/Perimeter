#include "StdAfxRd.h"
#include "Static3dx.h"
#include "Node3dx.h"
#include "NParticle.h"
#include "VisError.h"
#include <algorithm>

enum eTextureMapType
{ /* texture map in 3dSMAX */
	TEXMAP_AM					=	0,   // ambient
	TEXMAP_DI					=	1,   // diffuse
	TEXMAP_SP					=	2,   // specular
	TEXMAP_SH					=	3,   // shininess
	TEXMAP_SS					=	4,   // shininess strength
	TEXMAP_SI					=	5,   // self-illumination
	TEXMAP_OP					=	6,   // opacity
	TEXMAP_FI					=	7,   // filter color
	TEXMAP_BU					=	8,   // bump 
	TEXMAP_RL					=	9,   // reflection
	TEXMAP_RR					=	10,  // refraction 
	TEXMAP_DP					=	11,  // displacement 
};

cStaticVisibilityChainGroup::cStaticVisibilityChainGroup()
{
	ib_polygon=0;
	blend_indices=1;
	bump=false;
	vb_size=0;
}

cStatic3dx::cStatic3dx(bool is_logic_,const char* file_name_)
:file_name(file_name_)
{
	is_logic=is_logic_;
}

cStatic3dx::~cStatic3dx()
{
	vector<cStaticVisibilityChainGroup*>::iterator it;
	FOR_EACH(visibility_groups,it)
	{
		cStaticVisibilityChainGroup* p=*it;
		delete p;
	}
	visibility_groups.clear();

}

void cStatic3dx::Load(CLoadDirectory& rd)
{
	while(CLoadData* ld=rd.next())
	switch(ld->id)
	{
	case C3DX_ANIMATION_HEAD:
		LoadChainData(ld);
		break;
	case C3DX_NODES:
		LoadNodes(ld);
		break;
	case C3DX_MESHES:
		LoadMeshes(ld);
		break;
	case C3DX_MATERIAL_GROUP:
		{
			int mat_num=LoadMaterialsNum(ld);
			LoadMaterials(ld,mat_num);
		}
		break;
	case C3DX_BASEMENT:
		basement.Load(ld);
		break;
	case C3DX_LOGIC_BOUND:
		logic_bound.Load(ld);
		break;
	}

	DummyVisibilityGroup();

	vector<AnimationGroup>::iterator ita;
	FOR_EACH(animation_group,ita)
	{
		AnimationGroup& ag=*ita;
		ag.nodes.clear();
		for(int i=0;i<ag.temp_nodes_name.size();i++)
		{
			string& name=ag.temp_nodes_name[i];
			if(name=="_base_")
			{
				continue;
			}

			int inode;
			for(inode=0;inode<nodes.size();inode++)
			{
				cStaticNode& s=nodes[inode];
				if(name==s.name)
					break;
			}

			if(inode>=nodes.size())
				inode=0;
			
			ag.nodes.push_back(inode);
		}

		ag.temp_nodes_name.clear();
	}

	ParseEffect();
}

void cStatic3dx::LoadChainData(CLoadDirectory rd)
{
	while(CLoadData* ld=rd.next())
	switch(ld->id)
	{
	case C3DX_ANIMATION_GROUPS:
		LoadGroup(ld);
		break;
	case C3DX_ANIMATION_CHAIN:
		LoadChain(ld);
		break;
	case C3DX_ANIMATION_CHAIN_GROUP:
		LoadChainGroup(ld);
		break;
	}
}

void cStatic3dx::LoadGroup(CLoadDirectory rd)
{
	while(CLoadData* ld=rd.next())
	switch(ld->id)
	{
	case C3DX_ANIMATION_GROUP:
		{
			AnimationGroup ag;
			ag.Load(ld);
			animation_group.push_back(ag);
		}
		break;
	}
}

void AnimationGroup::Load(CLoadDirectory rd)
{
	while(CLoadData* ld=rd.next())
	switch(ld->id)
	{
	case C3DX_AG_HEAD:
		{
			CLoadIterator it(ld);
			it>>name;
		}
		break;
	case C3DX_AG_LINK:
		{
			CLoadIterator it(ld);
			it>>temp_nodes_name;
		}
		break;
	}
}

void cStatic3dx::LoadChain(CLoadDirectory rd)
{
	animation_chain.clear();
	while(CLoadData* ld=rd.next())
	switch(ld->id)
	{
	case C3DX_AC_ONE:
		{
			int begin_frame,end_frame;
			CLoadIterator li(ld);
			cAnimationChain ac;
			li>>ac.name;
			li>>begin_frame;
			li>>end_frame;
			li>>ac.time;
			animation_chain.push_back(ac);
		}
		break;
	}
}

void cStatic3dx::LoadChainGroup(CLoadDirectory rd)
{
	xassert(visibility_groups.empty());
	while(CLoadData* ld=rd.next())
	switch(ld->id)
	{
	case C3DX_ACG_ONE:
		{
			CLoadIterator li(ld);
			cStaticVisibilityChainGroup* p=new cStaticVisibilityChainGroup;
			li>>p->name;
			li>>p->temp_invisible_object;
			visibility_groups.push_back(p);
		}
		break;
	}
}

void cStatic3dx::LoadNodes(CLoadDirectory rd)
{
	while(CLoadData* ld=rd.next())
	switch(ld->id)
	{
	case C3DX_NODE:
		{
			nodes.push_back(cStaticNode());
			cStaticNode& node=nodes.back();
			node.LoadNode(ld);
		}
		break;
	}
}

void cStaticNode::LoadNode(CLoadDirectory rd)
{

	while(CLoadData* ld=rd.next())
	switch(ld->id)
	{
	case C3DX_NODE_HEAD:
		{
			CLoadIterator it(ld);
			it>>name;
			it>>inode;
			it>>iparent;
			xassert(iparent<inode);
		}
		break;
	case C3DX_NODE_CHAIN:
		LoadNodeChain(ld);
		break;
	}
	
}

void cStaticNode::LoadNodeChain(CLoadDirectory rd)
{
	chains.push_back(cStaticNodeChain());
	cStaticNodeChain& chain=chains.back();

	while(CLoadData* ld=rd.next())
	switch(ld->id)
	{
	case C3DX_NODE_CHAIN_HEAD:
		break;
	case C3DX_NODE_POSITION:
		chain.position.Load(ld);
		break;
	case C3DX_NODE_ROTATION:
		chain.rotation.Load(ld);
		break;
	case C3DX_NODE_SCALE:
		chain.scale.Load(ld);
		break;
	case C3DX_NODE_VISIBILITY:
		chain.visibility.Load(ld);
		break;
	}
}

struct SortMyMaterial
{
	cStatic3dx* data;
	SortMyMaterial(cStatic3dx* data_)
	{
		data=data_;
	}

	bool operator()(const cTempMesh3dx* p1,cTempMesh3dx* p2)
	{
		cStaticMaterial& s1=data->materials[p1->imaterial];
		cStaticMaterial& s2=data->materials[p1->imaterial];
		if(s1.pBumpTexture<s2.pBumpTexture)
			return true;
		if(s1.pBumpTexture>s2.pBumpTexture)
			return false;
		return p1->imaterial<p2->imaterial;
	}
};

void cStatic3dx::DummyVisibilityGroup()
{
	if(visibility_groups.empty())
	{
		cStaticVisibilityChainGroup* cg=new cStaticVisibilityChainGroup;
		cg->name="default";
		visibility_groups.push_back(cg);
	}
}

int cTempMesh3dx::CalcMaxBonesPerVertex()
{
	int max_nonzero_bones=0;
	max_nonzero_bones=0;
	for(int i=0;i<bones.size();i++)
	{
		cTempBone& p=bones[i];
		int cur_bones=0;
		for(int ibone=0;ibone<cTempBone::max_bones;ibone++)
		{
			if(p.weight[ibone]>=1/255.0f)
				cur_bones++;
		}
		max_nonzero_bones=max(max_nonzero_bones,cur_bones);
	}

	return max_nonzero_bones;
}

void cStatic3dx::LoadMeshes(CLoadDirectory rd)
{
	vector<cTempMesh3dx*> temp_mesh;

	while(CLoadData* ld=rd.next())
	switch(ld->id)
	{
	case C3DX_MESH:
		{
			cTempMesh3dx* mesh=new cTempMesh3dx;
			mesh->Load(ld);

			if(mesh->bones.size()>cStaticIndex::max_index)
			{
				vector<cTempMesh3dx*> split_mesh;
				SplitMesh(mesh,split_mesh);
				for(int i=0;i<split_mesh.size();i++)
				{
					cTempMesh3dx* m=split_mesh[i];
					temp_mesh.push_back(m);
				}
			}else
			{
				temp_mesh.push_back(mesh);
			}
		}
		break;
	}

	sort(temp_mesh.begin(),temp_mesh.end(),SortMyMaterial(this));

	///////////////////////
	DummyVisibilityGroup();
	int i;
	for(i=0;i<visibility_groups.size();i++)
	{
		cStaticVisibilityChainGroup* cur=visibility_groups[i];
		vector<cTempMesh3dx*> cur_mesh;
		vector<cTempMesh3dx*>::iterator itm;

		FOR_EACH(temp_mesh,itm)
		{
			int inode=(*itm)->inode;
			string& name=nodes[inode].name;

			vector<string>::iterator itt;
			bool found=false;
			FOR_EACH(cur->temp_invisible_object,itt)
			{
				if(name==*itt)
				{
					found=true;
					break;
				}
			}

			if(!found)
				cur_mesh.push_back(*itm);
		}

		//temp_mesh - не забыть гененрровать по temp_invisible_object
		BuildChainGroup(cur_mesh,cur);
		cur->temp_invisible_object.clear();
	}

	for(i=0;i<temp_mesh.size();i++)
	{
		delete temp_mesh[i];
	}
}

void cTempMesh3dx::Load(CLoadDirectory rd)
{
	while(CLoadData* ld=rd.next())
	switch(ld->id)
	{
	case C3DX_MESH_HEAD:
		{
			CLoadIterator it(ld);
			inode=-1;
			it>>inode;
			imaterial=-1;
			it>>imaterial;
			xassert(imaterial>=0);
			if(imaterial<0)
				imaterial=0;
		}
		break;
	case C3DX_MESH_VERTEX:
		{
			CLoadIterator it(ld);
			it>>vertex_pos;
		}
		break;
	case C3DX_MESH_NORMAL:
		{
			CLoadIterator it(ld);
			it>>vertex_norm;
		}
		break;
	case C3DX_MESH_UV:
		{
			CLoadIterator it(ld);
			it>>vertex_uv;
		}
		break;
	case C3DX_MESH_FACES:
		{
			CLoadIterator it(ld);
			it>>polygons;
		}
		break;
	case C3DX_MESH_SKIN:
		{
			CLoadIterator it(ld);
			int size=0;
			it>>size;
			typedef map<int,int> BonesInode;
			BonesInode bones_inode;
			bones.resize(size);
			for(int i=0;i<size;i++)
			{
				cTempBone& p=bones[i];
				float sum=0;
				int cur_bones=0;
				for(int ibone=0;ibone<cTempBone::max_bones;ibone++)
				{
					it>>p.weight[ibone];
					it>>p.inode[ibone];
					if(p.weight[ibone]>=1/255.0f)
						cur_bones++;
					sum+=p.weight[ibone];

					if(p.inode[ibone]>=0)
						bones_inode[p.inode[ibone]]=1;
				}
				xassert(sum>=0.999f && sum<1.001f);
/*
				sum=1/sum;
				for(ibone=0;ibone<cTempBone::max_bones;ibone++)
				{
					p.weight[ibone]*=sum;
				}
*/
			}

			inode_array.clear();
			BonesInode::iterator itn;
			FOR_EACH(bones_inode,itn)
			{
				inode_array.push_back(itn->first);
			}
		}
		break;
	}

	xassert(vertex_pos.size()==vertex_norm.size());
	if(!vertex_uv.empty())
	{
		xassert(vertex_pos.size()==vertex_uv.size());
	}
}

/*
void cStatic3dx::BuildSkinGroup(vector<cTempMesh3dx*>& temp_mesh)
{
	vector<cStaticMesh>::iterator it;
	cStaticIndex cur;
	cur.offset_polygon=0;
	cur.num_polygon=0;
	cur.offset_vertex=0;
	cur.num_vertex=0;
	cur.imaterial=0;

	FOR_EACH(meshes,it)
	{
		cStaticMesh& m=*it;

		vector<int>::iterator ifound=find(cur.node_index.begin(),cur.node_index.end(),m.inode);

		bool is_next_material=cur.imaterial!=m.imaterial;

		if(ifound==cur.node_index.end() || is_next_material)
		{
			if(cur.node_index.size()>=cStaticIndex::max_index || is_next_material)
			{
				skin_group.push_back(cur);

				cur.node_index.clear();
				cur.offset_polygon=m.offset_polygon;
				cur.num_polygon=0;
				cur.offset_vertex=m.offset_vertex;
				cur.num_vertex=0;
				cur.imaterial=m.imaterial;
			}

			cur.node_index.push_back(m.inode);
		}

		cur.num_polygon+=m.num_polygon;
		cur.num_vertex+=m.num_vertex;
	}

	if(cur.num_polygon>0)
	{
		skin_group.push_back(cur);
	}
}
/*/
void cStaticVisibilityChainGroup::BuildSkinGroup(vector<cTempMesh3dx*>& temp_mesh)
{
	cStaticIndex cur;
	cur.offset_polygon=0;
	cur.num_polygon=0;
	cur.offset_vertex=0;
	cur.num_vertex=0;
	cur.imaterial=0;

	for(int imesh=0;imesh<meshes.size();imesh++)
	{
		cStaticMesh& m=meshes[imesh];
		cTempMesh3dx& tm=*temp_mesh[imesh];

		vector<int>::iterator ifound=find(cur.node_index.begin(),cur.node_index.end(),m.inode);

		bool is_next=cur.imaterial!=m.imaterial;
		if(cur.node_index.size()+tm.inode_array.size()>cStaticIndex::max_index)
			is_next=true;

		if(ifound==cur.node_index.end() || is_next)
		{
			if(cur.node_index.size()>=cStaticIndex::max_index || is_next)
			{
				xassert(cur.node_index.size()<=cStaticIndex::max_index);
				skin_group.push_back(cur);

				cur.node_index.clear();
				cur.offset_polygon=m.offset_polygon;
				cur.num_polygon=0;
				cur.offset_vertex=m.offset_vertex;
				cur.num_vertex=0;
				cur.imaterial=m.imaterial;
			}

			if(tm.inode_array.size()>1)
			{
				for(int i=0;i<tm.inode_array.size();i++)
					cur.node_index.push_back(tm.inode_array[i]);
			}
			else
				cur.node_index.push_back(m.inode);
		}

		cur.num_polygon+=m.num_polygon;
		cur.num_vertex+=m.num_vertex;
	}

	if(cur.num_polygon>0)
	{
		skin_group.push_back(cur);
	}
}
/**/
int cStatic3dx::LoadMaterialsNum(CLoadDirectory rd)
{
	int num_material=0;
	while(CLoadData* ld=rd.next())
	switch(ld->id)
	{
	case C3DX_MATERIAL:
		num_material++;
		break;
	}

	return num_material;
}

void cStatic3dx::LoadMaterials(CLoadDirectory rd,int num_materials)
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	_splitpath(file_name.c_str(),drive,dir,fname,ext);
	string path_name=drive;
	path_name+=dir;
	path_name+="TEXTURES\\";

	int cur_mat=0;
	materials.resize(num_materials);
	while(CLoadData* ld=rd.next())
	switch(ld->id)
	{
	case C3DX_MATERIAL:
		{
			materials[cur_mat].Load(ld,path_name.c_str());
			cur_mat++;
		}
		break;
	}
}

cStaticMaterial::cStaticMaterial()
{
	ambient=sColor4f(0,0,0);
	diffuse=sColor4f(1,1,1);
	specular=sColor4f(0,0,0);
	opacity=0;
	specular_power=0;

	is_skinned=false;
	pBumpTexture=NULL;
}

cStaticMaterial::~cStaticMaterial()
{
	RELEASE(pBumpTexture);
}

void cStaticMaterial::Load(CLoadDirectory rd,const char* path_name)
{
	string tex_bump;

	while(CLoadData* ld=rd.next())
	switch(ld->id)
	{
	case C3DX_MATERIAL_HEAD:
		{
			CLoadIterator it(ld);
			it>>name;
		}
		break;
	case C3DX_MATERIAL_STATIC:
		{
			CLoadIterator it(ld);
			Vect3f v;
			it>>v;ambient=sColor4f(v.x,v.y,v.z);
			it>>v;diffuse=sColor4f(v.x,v.y,v.z);
			it>>v;specular=sColor4f(v.x,v.y,v.z);
			it>>opacity;
			it>>specular_power;
		}
		break;
	case C3DX_MATERIAL_TEXTUREMAP:
		{
			CLoadIterator it(ld);
			DWORD slot=-1;
			string name;
			it>>slot;
			it>>name;
			if(!name.empty())
			{
				string full_name=path_name;
				full_name+=name;
				switch(slot)
				{
				case TEXMAP_DI:
					tex_diffuse=full_name;
					break;
				case TEXMAP_FI:
					is_skinned=true;
					break;
				case TEXMAP_BU:
					tex_bump=full_name;
					break;
				}
			}
		}
		break;
	}

	gb_RenderDevice3D->SetCurrentConvertDot3Mul(1.0f);//Потом читать из файла
	if(!tex_bump.empty())
	{
		pBumpTexture=GetTexLibrary()->GetElement(tex_bump.c_str(),"Bump");
	}

/*
	if(!tname_diffuse.empty())
	{

		if(tname_filter.empty())
			tex_diffuse=GetTexLibrary()->GetElement(tname_diffuse.c_str());
		else
			tex_diffuse=GetTexLibrary()->GetElementColor(tname_diffuse.c_str(),sColor4c(0,0,255,255));
	}
*/
}

void cStatic3dx::SplitMesh(cTempMesh3dx* mesh,vector<cTempMesh3dx*>& split_mesh)
{
	cTempMesh3dx* cur_mesh=new cTempMesh3dx;
	vector<int> node_index;
	vector<int> vertex_realloc(mesh->vertex_pos.size());
	for(int vi=0;vi<vertex_realloc.size();vi++)
		vertex_realloc[vi]=-1;
	typedef map<int,int> BonesInode;
	BonesInode bones_inode;

	for(int ipolygon=0;ipolygon<mesh->polygons.size();ipolygon++)
	{
		sPolygon& sp=mesh->polygons[ipolygon];
retry:
		int ip;
		for(ip=0;ip<3;ip++)
		{
			int ivertex=sp[ip];
			xassert(ivertex>=0 && ivertex<mesh->bones.size());
			cTempBone& p=mesh->bones[ivertex];

			for(int ibone=0;ibone<cTempBone::max_bones;ibone++)
			{
				if(p.inode[ibone]<0)
					continue;
				vector<int>::iterator ifound=find(node_index.begin(),node_index.end(),p.inode[ibone]);

				if(ifound==node_index.end())
				{
					if(node_index.size()>=cStaticIndex::max_index)
					{
						cur_mesh->inode_array.clear();
						BonesInode::iterator itn;
						FOR_EACH(bones_inode,itn)
						{
							int idx=itn->first;
							cur_mesh->inode_array.push_back(idx);
						}

						cur_mesh->inode=mesh->inode;
						cur_mesh->imaterial=mesh->imaterial;

						split_mesh.push_back(cur_mesh);
						bones_inode.clear();
						cur_mesh=new cTempMesh3dx;
						node_index.clear();
						for(int vi=0;vi<vertex_realloc.size();vi++)
							vertex_realloc[vi]=-1;
						goto retry;//Эта точка уже не влезает.
					}

					node_index.push_back(p.inode[ibone]);
				}
			}
		}

		sPolygon new_polygon;
		for(ip=0;ip<3;ip++)
		{
			int ivertex=sp[ip];
			int& idx=vertex_realloc[ivertex];
			if(idx<0)
			{
				idx=cur_mesh->vertex_pos.size();
				cur_mesh->vertex_pos.push_back(mesh->vertex_pos[ivertex]);
				cur_mesh->vertex_norm.push_back(mesh->vertex_norm[ivertex]);
				cur_mesh->vertex_uv.push_back(mesh->vertex_uv[ivertex]);
				cur_mesh->bones.push_back(mesh->bones[ivertex]);

				cTempBone& tb=mesh->bones[ivertex];
				for(int ibone=0;ibone<cTempBone::max_bones;ibone++)
				{
					if(tb.inode[ibone]<0)
						continue;
					bones_inode[tb.inode[ibone]]=1;
				}
			}

			new_polygon[ip]=idx;
		}

		cur_mesh->polygons.push_back(new_polygon);
	}

	if(!cur_mesh->vertex_pos.empty())
	{
		cur_mesh->inode_array.clear();
		BonesInode::iterator itn;
		FOR_EACH(bones_inode,itn)
		{
			cur_mesh->inode_array.push_back(itn->first);
		}
		cur_mesh->inode=mesh->inode;
		cur_mesh->imaterial=mesh->imaterial;
		split_mesh.push_back(cur_mesh);
	}
}

void cStatic3dx::BuildChainGroup(vector<cTempMesh3dx*>& temp_mesh,cStaticVisibilityChainGroup* chain_group)
{
	int vertex_size=0;
	int polygon_size=0;
	bool is_blend=false;

	bool first_pass=true;
	vector<cTempMesh3dx*>::iterator itm;
	int max_bones_per_vertex=1;
	FOR_EACH(temp_mesh,itm)
	{
		cTempMesh3dx* m=*itm;
		vertex_size+=m->vertex_pos.size();
		polygon_size+=m->polygons.size();
		int bones_per_vertex=m->CalcMaxBonesPerVertex();
		max_bones_per_vertex=max(max_bones_per_vertex,bones_per_vertex);
		if(!m->bones.empty())
			is_blend=true;

		bool is_bump=materials[m->imaterial].pBumpTexture!=NULL;
		//if(first_pass && is_bump)
		if(is_bump)
			chain_group->bump=true;

//		xassert(chain_group->bump==is_bump);

		first_pass=false;
	}

	xassert(vertex_size<65535);
	xassert(polygon_size<65535);
	chain_group->vb_size=vertex_size;

	vector<sVertexXYZINT1> vertex(vertex_size);
	vector<sPolygon> polygons(polygon_size);

	chain_group->ib_polygon=polygon_size;
	gb_RenderDevice->CreateIndexBuffer(chain_group->ib,chain_group->ib_polygon);
	sPolygon *IndexPolygon=gb_RenderDevice->LockIndexBuffer(chain_group->ib);

//	chain_group->blend_indices=is_blend?4:1;
	chain_group->blend_indices=max_bones_per_vertex;

	cSkinVertex skin_vertex(chain_group->GetBlendWeight(),chain_group->bump);

	gb_RenderDevice->CreateVertexBuffer(chain_group->vb,vertex_size,
		skin_vertex.GetFormat());
	void *pVertex=gb_RenderDevice->LockVertexBuffer(chain_group->vb);

	skin_vertex.SetVB(pVertex,chain_group->vb.size);

	int cur_vertex_offset=0;
	int cur_polygon_offset=0;
	chain_group->meshes.resize(temp_mesh.size());
	int i;
	for(i=0;i<temp_mesh.size();i++)
	{
		cTempMesh3dx& tmesh=*temp_mesh[i];
		cStaticMesh& mesh=chain_group->meshes[i];
		mesh.offset_polygon=cur_polygon_offset;
		mesh.offset_vertex=cur_vertex_offset;
		mesh.num_polygon=tmesh.polygons.size();
		mesh.num_vertex=tmesh.vertex_pos.size();
		mesh.inode=tmesh.inode;
		mesh.imaterial=tmesh.imaterial;

		for(int index=0;index<mesh.num_polygon;index++)
		{
			sPolygon& in=tmesh.polygons[index];
			sPolygon& out=IndexPolygon[cur_polygon_offset+index];
			out.p1=in.p1+cur_vertex_offset;
			out.p2=in.p2+cur_vertex_offset;
			out.p3=in.p3+cur_vertex_offset;
		}

		for(int vertex=0;vertex<mesh.num_vertex;vertex++)
		{
			skin_vertex.Select(cur_vertex_offset+vertex);
			skin_vertex.GetPos()=tmesh.vertex_pos[vertex];
			skin_vertex.GetNorm()=tmesh.vertex_norm[vertex];
			if(tmesh.vertex_uv.empty())
			{
				skin_vertex.GetTexel()=Vect2f(0,0);
			}else
			{
				skin_vertex.GetTexel()=tmesh.vertex_uv[vertex];
			}

			sColor4c& index=skin_vertex.GetIndex();
			index.RGBA()=0;
		}

		cur_vertex_offset+=tmesh.vertex_pos.size();
		cur_polygon_offset+=tmesh.polygons.size();
	}

	gb_RenderDevice->UnlockIndexBuffer(chain_group->ib);

	chain_group->BuildSkinGroup(temp_mesh);

	for(i=0;i<temp_mesh.size();i++)
	{
		cTempMesh3dx& tmesh=*temp_mesh[i];
		cStaticMesh& mesh=chain_group->meshes[i];

		cStaticIndex* cur_index=NULL;
		int j;
		for(j=0;j<chain_group->skin_group.size();j++)
		{
			cStaticIndex& s=chain_group->skin_group[j];
			if(s.offset_polygon<=mesh.offset_polygon && 
				s.offset_polygon+s.num_polygon>mesh.offset_polygon)
			{
				cur_index=&chain_group->skin_group[j];
				break;
			}
		}

		vector<int> node_hash(nodes.size());
		for(j=0;j<node_hash.size();j++)
			node_hash[j]=-1;
		for(j=0;j<cur_index->node_index.size();j++)
		{
			int idx=cur_index->node_index[j];
			xassert(idx>=0 && idx<node_hash.size());
			node_hash[idx]=j;
		}

		if(!tmesh.bones.empty())
		{
			for(int vertex=0;vertex<mesh.num_vertex;vertex++)
			{
				skin_vertex.Select(mesh.offset_vertex+vertex);
				cTempBone& tb=tmesh.bones[vertex];

				sColor4c weight;
				weight.RGBA()=0;
				xassert(chain_group->blend_indices<=cTempBone::max_bones);
				int sum=0;
				int ibone;
				int weights=chain_group->GetBlendWeight();
				for(ibone=0;ibone<weights;ibone++)
				{
					int w=tb.weight[ibone]*255;
					if(ibone==chain_group->blend_indices-1)
						w=255-sum;
					skin_vertex.GetWeight(ibone)=w;
					sum+=w;
				}
				xassert(sum==255 || weights==0);

				sColor4c index;
				index.RGBA()=0;
				for(ibone=0;ibone<chain_group->blend_indices;ibone++)
				{
					int idx=0;
					int cur_inode=tb.inode[ibone];
					if(cur_inode>=0)
						idx=node_hash[cur_inode];
					xassert(idx>=0 && idx<cStaticIndex::max_index);
					index[ibone]=idx;
				}
				skin_vertex.GetIndex()=index;
			}
		}else
		{
			for(int vertex=0;vertex<mesh.num_vertex;vertex++)
			{
				skin_vertex.Select(mesh.offset_vertex+vertex);
				int idx=node_hash[mesh.inode];
				xassert(idx>=0 && idx<cStaticIndex::max_index);
				sColor4c& ix=skin_vertex.GetIndex();
				ix[0]=idx;
				ix[1]=0;
				ix[2]=0;
				ix[3]=0;

				if(chain_group->GetBlendWeight()>0)
				{
					skin_vertex.GetWeight(0)=255;
					skin_vertex.GetWeight(1)=0;
					skin_vertex.GetWeight(2)=0;
					skin_vertex.GetWeight(3)=0;
				}
			}
		}
	}
	gb_RenderDevice->UnlockVertexBuffer(chain_group->vb);

	chain_group->CalcBumpST();
}

void cStaticVisibilityChainGroup::CalcBumpST()
{
	if(!bump)
		return;

	cSkinVertex skin_vertex(GetBlendWeight(),bump);
	void *pVertex=gb_RenderDevice->LockVertexBuffer(vb);
	skin_vertex.SetVB(pVertex,vb.size);

	sPolygon *Polygon=gb_RenderDevice->LockIndexBuffer(ib);
	int i;
	for(i=0;i<vb_size;i++)
	{
		skin_vertex.Select(i);
		skin_vertex.GetBumpS().set(0,0,0);
		skin_vertex.GetBumpT().set(0,0,0);
	}
	
	cSkinVertex v0(GetBlendWeight(),bump),v1(GetBlendWeight(),bump),v2(GetBlendWeight(),bump);
	v0.SetVB(pVertex,vb.size);
	v1.SetVB(pVertex,vb.size);
	v2.SetVB(pVertex,vb.size);
	for(i=0;i<ib_polygon;i++)
	{
		Vect3f cp;
		Vect3f edge01,edge02;
		sPolygon &p=Polygon[i];
		v0.Select(p.p1);
		v1.Select(p.p2);
		v2.Select(p.p3);

		// x, s, t
		edge01.set( v1.GetPos().x-v0.GetPos().x, v1.GetTexel().x-v0.GetTexel().x, v1.GetTexel().y-v0.GetTexel().y );
		edge02.set( v2.GetPos().x-v0.GetPos().x, v2.GetTexel().x-v0.GetTexel().x, v2.GetTexel().y-v0.GetTexel().y );

		cp.cross(edge01,edge02);
		if ( fabs(cp.x) > FLT_EPS )
		{
			v0.GetBumpS().x += -cp.y / cp.x;
			v0.GetBumpT().x += -cp.z / cp.x;

			v1.GetBumpS().x += -cp.y / cp.x;
			v1.GetBumpT().x += -cp.z / cp.x;
			
			v2.GetBumpS().x += -cp.y / cp.x;
			v2.GetBumpT().x += -cp.z / cp.x;
		}

		// y, s, t
		edge01.set( v1.GetPos().y-v0.GetPos().y, v1.GetTexel().x-v0.GetTexel().x, v1.GetTexel().y-v0.GetTexel().y );
		edge02.set( v2.GetPos().y-v0.GetPos().y, v2.GetTexel().x-v0.GetTexel().x, v2.GetTexel().y-v0.GetTexel().y );

		cp.cross(edge01, edge02);
		if ( fabs(cp.x) > FLT_EPS )
		{
			v0.GetBumpS().y += -cp.y / cp.x;
			v0.GetBumpT().y += -cp.z / cp.x;

			v1.GetBumpS().y += -cp.y / cp.x;
			v1.GetBumpT().y += -cp.z / cp.x;
			
			v2.GetBumpS().y += -cp.y / cp.x;
			v2.GetBumpT().y += -cp.z / cp.x;
		}

		// z, s, t
		edge01.set( v1.GetPos().z-v0.GetPos().z, v1.GetTexel().x-v0.GetTexel().x, v1.GetTexel().y-v0.GetTexel().y );
		edge02.set( v2.GetPos().z-v0.GetPos().z, v2.GetTexel().x-v0.GetTexel().x, v2.GetTexel().y-v0.GetTexel().y );

		cp.cross(edge01,edge02);
		if ( fabs(cp.x) > FLT_EPS )
		{
			v0.GetBumpS().z += -cp.y / cp.x;
			v0.GetBumpT().z += -cp.z / cp.x;

			v1.GetBumpS().z += -cp.y / cp.x;
			v1.GetBumpT().z += -cp.z / cp.x;
			
			v2.GetBumpS().z += -cp.y / cp.x;
			v2.GetBumpT().z += -cp.z / cp.x;
		}
	}

	for(i=0;i<vb_size;i++)
	{
		skin_vertex.Select(i);
  		skin_vertex.GetBumpS().Normalize();
		skin_vertex.GetBumpT().Normalize();
  		skin_vertex.GetBumpSxT().cross(skin_vertex.GetBumpS(), skin_vertex.GetBumpT());
  		skin_vertex.GetNorm().Normalize();
    		
  		// Get the direction of the SxT vector
  		if (skin_vertex.GetBumpSxT().dot(skin_vertex.GetNorm()) < 0.0f)
  		{
  			skin_vertex.GetBumpSxT() = -skin_vertex.GetBumpSxT();
  		}
	}

	gb_RenderDevice->UnlockVertexBuffer(vb);
	gb_RenderDevice->UnlockIndexBuffer(ib);
}


void cStaticBasement::Load(CLoadDirectory rd)
{
	while(CLoadData* ld=rd.next())
	switch(ld->id)
	{
	case C3DX_BASEMENT_VERTEX:
		{
			CLoadIterator it(ld);
			it>>vertex;
		}
		break;
	case C3DX_BASEMENT_FACES:
		{
			CLoadIterator it(ld);
			it>>polygons;
		}
		break;
	}
}

void cStaticLogicBound::Load(CLoadIterator it)
{
	it>>inode;
	it>>bound.min;
	it>>bound.max;
}

cVisError& cStatic3dx::errlog()
{
	return VisError<<"Error load file: "<<file_name.c_str()<<"\r\n";
}

static bool is_space(char c)
{
	return c==' ' || c==8;
}

static bool is_name_char(char c)
{
	return (c>='0' && c<='9') || 
		   (c>='a' && c<='z') ||
		   (c>='A' && c<='Z') || c=='_';
}

void cStatic3dx::ParseEffect()
{
	for(int inode=0;inode<nodes.size();inode++)
	{
		cStaticNode& node=nodes[inode];
		static char eff[]="effect:";
		const char* cur=node.name.c_str();
		const char* end;
		if(strncmp(cur,eff,sizeof(eff)-1)!=0)
			continue;
		cur+=sizeof(eff)-1;
		while(is_space(*cur))
			cur++;
		end=cur;
		while(is_name_char(*end))
			end++;

		string file_name(cur,end-cur);
		EffectLibrary* lib=gb_VisGeneric->GetEffectLibrary(file_name.c_str(),true);

		if(!lib)
		{
			errlog()<<"Object: \""<<node.name.c_str()<<"\"\r\n"<<
						"Effect library path: "<<gb_VisGeneric->GetEffectPath()<<"\"\r\n"<<
						"Library not found: \""<<file_name.c_str()<<"\""<<VERR_END;
			return;
		}

		cur=end;
		while(is_space(*cur))
			cur++;

		end=cur;
		while(is_name_char(*end))
			end++;

		cStaticEffect effect;
		effect.node=inode;
		string effect_name(cur,end-cur);
		effect.effect_key=lib->Get(effect_name.c_str());
		if(!effect.effect_key)
		{
			errlog()<<"Object: \""<<node.name.c_str()<<"\"\r\n"<<
						"Effect library path: "<<gb_VisGeneric->GetEffectPath()<<"\r\n"
						"Effect not found: \""<<effect_name.c_str()<<"\""<<VERR_END;
			return;
		}
		effect.is_cycled=effect.effect_key->IsCycled();

		effects.push_back(effect);
	}
}
