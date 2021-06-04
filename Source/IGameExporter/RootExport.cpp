#include "StdAfx.h"
#include "RootExport.h"
#include "Interpolate.h"
#include "ExportMesh.h"
#include "DebugDlg.h"
#include "ExportMaterial.h"
#include "ExportBasement.h"
#include "..\render\inc\umath.h"

RootExport::RootExport()
{
	pIgame=NULL;
	node_base_=NULL;
}

RootExport::~RootExport()
{
}

void RootExport::Init(IGameScene * pIgame_)
{
	pIgame=pIgame_;
	time_start=pIgame->GetSceneStartTime();
	time_end=pIgame->GetSceneEndTime();
	time_frame=pIgame->GetSceneTicks();
	xassert(time_start==0);
}

bool RootExport::LoadData(const char* filename)
{
	CLoadDirectoryFile s;
	if(!s.Load(filename))
		return false;
	while(CLoadData* ld=s.next())
	switch(ld->id)
	{
	case C3DX_ANIMATION_HEAD:
		animation_data.Load(ld);
		break;
	}

	ShowConsole(hInstance,NULL);
	ClearConsole();
	return true;
}

int RootExport::GetNumFrames()
{
	return time_end/time_frame+1;
}

void RootExport::Export(const char* filename,bool logic)
{
	materials.clear();
	node_base_=NULL;
	node_map.clear();
	all_nodes.clear();

	export_logic=logic;

	if(export_logic)
	{
		bool is=false;
		for(int loop = 0; loop <pIgame->GetTopLevelNodeCount();loop++)
		{
			IGameNode* pGameNode = pIgame->GetTopLevelNode(loop);
			if(IsIgnore(pGameNode,true))
				continue;
			is=true;
		}
		if(!is)
		{//logic object not found
			DeleteFile(filename);
			return;
		}
	}


	if(!saver.Init(filename))
	{
		Msg("Не могу открыть файл: %s",filename);
		return;
	}

	CalcNodeMap();
	animation_data.Save(saver);
	if(!export_logic)
		SaveMaterials();
	SaveNodes();
	if(!export_logic)
		SaveMeshes();

	SaveBasement();
	if(export_logic)
		SaveLogicBound();
}

bool RootExport::IsIgnore(IGameNode* node,bool root)
{
	if(node->IsTarget())
		return true;
	const char* name=node->GetName();

	if(root)
	{
		if(export_logic)
		{
			if(strstr(name,"logic")==NULL)
				return true;
		}else
		{
			if(strstr(name,"logic"))
				return true;
		}
	}

	if(strcmp(name,"_base_")==0)
		return true;
	return false;
}

void RootExport::CalcNodeMap()
{
	int current_node=0;
	vector<NextLevelNode> cur_level;
	vector<NextLevelNode> next_level;
	for(int loop = 0; loop <pIgame->GetTopLevelNodeCount();loop++)
	{
		IGameNode* pGameNode = pIgame->GetTopLevelNode(loop);
		if(IsIgnore(pGameNode,true))
			continue;

		NextLevelNode n;
		n.node=pGameNode;
		n.current=current_node;
		n.parent=-1;
		n.pParent=NULL;
		cur_level.push_back(n);
		all_nodes.push_back(n);
		current_node++;
	}
	
	while(!cur_level.empty())
	{
		vector<NextLevelNode>::iterator it_node;
		FOR_EACH(cur_level,it_node)
		{
			NextLevelNode& n=*it_node;

			for(int count=0;count<n.node->GetChildCount();count++)
			{
				IGameNode * pChildNode = n.node->GetNodeChild(count);
				//check for selected state - we deal with targets in the light/camera section
				if(strcmp(pChildNode->GetName(),"_base_")==0)
				{
					node_base_=pChildNode;
					continue;
				}

				if(IsIgnore(pChildNode,false))
					continue;


				NextLevelNode next;
				next.node=pChildNode;
				next.current=current_node;
				next.parent=n.current;
				next.pParent=n.node;
				next_level.push_back(next);
				all_nodes.push_back(next);
				current_node++;
			}
		}
		cur_level.swap(next_level);
		next_level.clear();
	}

	{//calc node map
		node_map.clear();
		for(int inode=0;inode<all_nodes.size();inode++)
		{
			NextLevelNode& n=all_nodes[inode];
			xassert(n.current==inode);
			node_map[n.node]=n.current;
		}
	}
}

void RootExport::SaveNodes()
{
	// float  INode::GetVisibility(TimeValue t,Interval *valid=NULL)

	saver.push(C3DX_NODES);
	for(int inode=0;inode<all_nodes.size();inode++)
	{
		NextLevelNode& n=all_nodes[inode];
		ExportNode(n.node,n.pParent,n.current,n.parent);
	}
	saver.pop();

}

void RootExport::SaveMeshes()
{
	saver.push(C3DX_MESHES);
	for(int inode=0;inode<all_nodes.size();inode++)
	{
		NextLevelNode& n=all_nodes[inode];
		if(n.node->GetIGameObject())
		{
			if(n.node->GetIGameObject()->GetIGameType()==IGameObject::IGAME_MESH)
			{
				const char* name=n.node->GetName();
				ExportMesh mesh(saver,name);
				Matrix3 m_object=n.node->GetWorldTM(0).ExtractMatrix3();
				Matrix3 m_local=n.node->GetLocalTM(0).ExtractMatrix3();
				
				mesh.Export((IGameMesh*)n.node->GetIGameObject(),n.current,m_object);
			}
		}
	}
	saver.pop();
}

void RootExport::ExportNode(IGameNode* pnode,IGameNode* pParent,int inode,int iparent)
{
	saver.push(C3DX_NODE);
	{
		saver.push(C3DX_NODE_HEAD);
		saver<<pnode->GetName();
		saver<<inode;
		saver<<iparent;
		saver.pop();
	}
//	Msg("Node: %i %i %s\n",inode,iparent,pnode->GetName());

	for(int i=0;i<animation_data.animation_chain.size();i++)
	{
		AnimationChain& ac=animation_data.animation_chain[i];
		saver.push(C3DX_NODE_CHAIN);

		{
			saver.push(C3DX_NODE_CHAIN_HEAD);
			saver<<i;
			saver<<ac.name;
			saver.pop();
		}

		ExportMatrix(pnode,pParent,ac.begin_frame,ac.end_frame-ac.begin_frame,ac.cycled);
		saver.pop();
	}

	saver.pop();
}

void RootExport::ExportMatrix(IGameNode * node,IGameNode* pParent,int interval_begin,int interval_size,bool cycled)
{
	typedef VectTemplate<3> VectPosition;
	typedef VectTemplate<4> VectRotation;
	typedef VectTemplate<1> VectScale;
	typedef InterpolatePosition<VectPosition> IPosition;
	typedef InterpolatePosition<VectRotation> IRotation;
	typedef InterpolatePosition<VectScale> IScale;
	vector<VectPosition> position;
	vector<VectRotation> rotation;
	vector<VectScale> scale;
	IPosition ipos;
	IRotation irot;
	IScale iscale;
	bool scale_error=false;
	bool sign_error=false;

	static char eff[]="effect:";
	bool export_visibility=false;
	const char* node_name=node->GetName();
	if(strncmp(node_name,eff,sizeof(eff)-1)==0)
		export_visibility=true;
	vector<bool> visibility;

	for(int icurrent=0;icurrent<interval_size;icurrent++)
	{
		AffineParts ap;

		int current_max_time=(icurrent+interval_begin)*time_frame;
		Matrix3 m=node->GetLocalTM(current_max_time).ExtractMatrix3();

		decomp_affine(m, &ap);
		Quat q = ap.q;
//		Msg("  q=%f %f %f %f\n",q.x,q.y, q.z, q.w);
//		Msg("  pos=%f %f %f\n",ap.t.x,ap.t.y, ap.t.z);
		VectPosition pos;
		pos[0]=ap.t.x;
		pos[1]=ap.t.y;
		pos[2]=ap.t.z;

		position.push_back(pos);

		VectRotation rot,rot_inv;
		rot[0]=ap.q.x;
		rot[1]=ap.q.y;
		rot[2]=ap.q.z;
		rot[3]=ap.q.w;
		//Матрица вращения может скачком менять знак.
		//Необходимо препятствовать этому явлению.
		if(rotation.empty())
		{
			rotation.push_back(rot);
		}else
		{
			VectRotation rot_prev=rotation.back();
			rot_inv[0]=-ap.q.x;
			rot_inv[1]=-ap.q.y;
			rot_inv[2]=-ap.q.z;
			rot_inv[3]=-ap.q.w;
			float d=rot_prev.distance(rot);
			float d_inv=rot_prev.distance(rot_inv);
			if(d<d_inv)
				rotation.push_back(rot);
			else
				rotation.push_back(rot_inv);
		}

		VectScale s;
		float mids=(ap.k.x+ap.k.y+ap.k.z)/3;
		float deltas=fabsf(mids-ap.k.x)+fabsf(mids-ap.k.y)+fabsf(mids-ap.k.z);
		s[0]=mids;
		scale.push_back(s);

		if(deltas>1e-3f)
			scale_error=true;

		if(ap.f<0)
			sign_error=true;

		if(export_visibility)
		{
			INode *max_node=node->GetMaxNode (); 
			float visible=max_node->GetVisibility(current_max_time);
			visibility.push_back(visible>0.5f);
		}
	}

	if(scale_error)
		Msg("Node %s. Не поддерживается анизотропный scale\n",node->GetName());
	if(sign_error)
		Msg("Node %s. Матрица должна иметь положительный детерминант\n",node->GetName());
/*
	if(strcmp(node->GetName(),"Mesh01")==0)
	{
		for(int i=0;i<rotation.size();i++)
		{
			VectRotation& r=rotation[i];
			Msg("%i %.3f %.3f %.3f %.3f\n",i, r[0],r[1],r[2],r[3]);
		}
	}
*/
	ipos.Interpolate(position,0.01f,cycled);
	saver.push(C3DX_NODE_POSITION);
	if(!ipos.Save(saver,interval_begin,interval_size))
		Msg("Error: Node %s. ITPL_UNKNOWN POSITION\n",node->GetName());
	saver.pop();

	irot.Interpolate(rotation,2e-4f,cycled);
	saver.push(C3DX_NODE_ROTATION);
	if(!irot.Save(saver,interval_begin,interval_size))
		Msg("Error: Node %s. ITPL_UNKNOWN ROTATION\n",node->GetName());
	saver.pop();

	iscale.Interpolate(scale,1e-3f,cycled);
	saver.push(C3DX_NODE_SCALE);
	if(!iscale.Save(saver,interval_begin,interval_size))
		Msg("Error: Node %s. ITPL_UNKNOWN SCALE\n",node->GetName());
	saver.pop();

	if(export_visibility)
		SaveVisibility(visibility,interval_begin,interval_size,cycled);

	if(dbg_show_position)
	{
		for(int i=0;i<ipos.out_data.size();i++)
		{
			IPosition::One& o=ipos.out_data[i];
			if(o.itpl==ITPL_SPLINE || o.itpl==ITPL_LINEAR)
			{
				if(o.itpl==ITPL_SPLINE)
					Msg("SPLINE");
				else
					Msg("LINEAR");
				Msg(" %i %i x=(%f %f %f %f) y=(%f %f %f %f) z=(%f %f %f %f)\n",
					o.interval_begin,o.interval_size,
					o.a0[0],o.a1[0],o.a2[0],o.a3[0],
					o.a0[1],o.a1[1],o.a2[1],o.a3[1],
					o.a0[2],o.a1[2],o.a2[2],o.a3[2]
					);
			}else
			if(o.itpl==ITPL_CONSTANT)
			{
				Msg("CONST %i %i p=%f %f %f\n",o.interval_begin,o.interval_size,
					o.a0[0],o.a0[1],o.a0[2]);
			}else
			{
				Msg("Error: Node %s. UNKN %i %i\n",o.interval_begin,o.interval_size);
			}
		}
	}

	if(dbg_show_scale)
	{
		for(int i=0;i<iscale.out_data.size();i++)
		{
			IScale::One& o=iscale.out_data[i];
			if(o.itpl==ITPL_SPLINE || o.itpl==ITPL_LINEAR)
			{
				if(o.itpl==ITPL_SPLINE)
					Msg("SPLINE");
				else
					Msg("LINEAR");
				Msg(" %i %i s=(%f %f %f %f)\n",
					o.interval_begin,o.interval_size,o.a0[0],o.a1[0],o.a2[0],o.a3[0]);
			}else
			if(o.itpl==ITPL_CONSTANT)
			{
				Msg("CONST %i %i s=%f\n",o.interval_begin,o.interval_size,o.a0[0]);
			}else
			{
				Msg("Error: UNKN %i %i\n",o.interval_begin,o.interval_size);
			}
		}
	}

	if(dbg_show_rotation)
	{
		for(int i=0;i<irot.out_data.size();i++)
		{
			IRotation::One& o=irot.out_data[i];
			if(o.itpl==ITPL_SPLINE || o.itpl==ITPL_LINEAR)
			{
				if(o.itpl==ITPL_SPLINE)
					Msg("SPLINE");
				else
					Msg("LINEAR");
				Msg(" %i %i x=(%f %f %f %f) y=(%f %f %f %f) z=(%f %f %f %f) w=(%f %f %f %f)\n",
					o.interval_begin,o.interval_size,
					o.a0[0],o.a1[0],o.a2[0],o.a3[0],
					o.a0[1],o.a1[1],o.a2[1],o.a3[1],
					o.a0[2],o.a1[2],o.a2[2],o.a3[2],
					o.a0[3],o.a1[3],o.a2[3],o.a3[3]
					);
			}else
			if(o.itpl==ITPL_CONSTANT)
			{
				Msg("CONST %i %i p=%f %f %f %f\n",o.interval_begin,o.interval_size,
					o.a0[0],o.a0[1],o.a0[2],o.a0[3]);
			}else
			{
				Msg("Error: Node %s. UNKN %i %i\n",o.interval_begin,o.interval_size);
			}
		}
	}
}

IGameNode* RootExport::Find(const char* name)
{
	for(int loop = 0; loop <pIgame->GetTopLevelNodeCount();loop++)
	{
		IGameNode * pGameNode = pIgame->GetTopLevelNode(loop);
		IGameNode * out=FindRecursive(pGameNode,name);
		if(out)
			return out;
	}

	return NULL;
}

IGameNode* RootExport::FindRecursive(IGameNode* pGameNode,const char* name)
{
	if(strcmp(pGameNode->GetName(),name)==0)
		return pGameNode;
	for(int count=0;count<pGameNode->GetChildCount();count++)
	{
		IGameNode * pChildNode = pGameNode->GetNodeChild(count);
		IGameNode * out=FindRecursive(pChildNode,name);
		if(out)
			return out;
	}

	return NULL;
}

void RootExport::SaveMaterials()
{
	saver.push(C3DX_MATERIAL_GROUP);
	int size=pIgame->GetRootMaterialCount();
	materials.resize(size);
	for(int i=0;i<size;i++)
	{
		IGameMaterial *mat=pIgame->GetRootMaterial(i);
		materials[i]=mat;
		ExportMaterial m(saver);
		m.Export(mat);
	}
	saver.pop();
}

int RootExport::FindMaterialIndex(IGameMaterial* mat)
{
	for(int i=0;i<materials.size();i++)
	if(materials[i]==mat)
		return i;

	return -1;
}

int RootExport::FindNodeIndex(IGameNode* node)
{
	NodeMap::iterator it=node_map.find(node);
	if(it!=node_map.end())
	{
		xassert(it->first==node);
		return it->second;
	}

	return -1;
}

void RootExport::SaveBasement()
{
	if(!all_nodes.empty() && node_base_)
	{
		//find root
		NextLevelNode* root=&all_nodes[0];
		const char* root_name=root->node->GetName();
		xassert(root->pParent==NULL);

		//
		if(node_base_->GetIGameObject() && node_base_->GetIGameObject()->GetIGameType()==IGameObject::IGAME_MESH)
		{
			ExportBasement base(saver,node_base_->GetName());
			Matrix3 m_root=root->node->GetWorldTM(0).ExtractMatrix3();
			base.Export((IGameMesh*)node_base_->GetIGameObject(),node_base_,m_root);
		}else
		{
			Msg("Объект с именем _base_ должен состоять из треугольников.\n");
		}
	}
}

void RootExport::SaveLogicBound()
{
	for(int inode=0;inode<all_nodes.size();inode++)
	{
		NextLevelNode& node=all_nodes[inode];
		if(strcmp(node.node->GetName(),"logic bound")==0)
		{
			if(!(node.node->GetIGameObject() && node.node->GetIGameObject()->GetIGameType()==IGameObject::IGAME_MESH))
			{
				Msg("Объект с именем logic bound должен состоять из треугольников.\n");
				break;
			}

			IGameMesh* pobject=(IGameMesh*)node.node->GetIGameObject();
			NextLevelNode* root=&all_nodes[0];

			sBox6f box;
			box.min.set(1e30f,1e30f,1e30f);
			box.max.set(-1e30f,-1e30f,-1e30f);
			Matrix3 m_root=root->node->GetWorldTM(0).ExtractMatrix3();
			Matrix3 inv_root=Inverse(m_root);
			Matrix3 m_node=node.node->GetWorldTM(0).ExtractMatrix3();
			Matrix3 inv_node=Inverse(m_node);
			int num_vertex=pobject->GetNumberOfVerts();
			for(int i=0;i<num_vertex;i++)
			{
				Point3 pos0=pobject->GetVertex(i);
				Point3 pos=pos0*inv_root;
				Vect3f p(pos.x,pos.y,pos.z);
				box.AddBound(p);
			}

			saver.push(C3DX_LOGIC_BOUND);
			saver<<node.current;
			saver<<box.min;
			saver<<box.max;
			saver.pop();
		}
	}
}


struct OneVisibility
{
	int interval_begin;
	int interval_size;
	bool value;
};

void RootExport::SaveVisibility(vector<bool>& visibility,int interval_begin,int interval_size,bool cycled)
{
	if(visibility.empty())
		return;
	vector<OneVisibility> intervals;
	//В принципе аналог кода из InterpolatePosition, но для констант.
	int size=visibility.size();
	bool cur=visibility[0];
	int begin=0;
	for(int i=0;i<size;i++)
	{
		bool b=visibility[i];
		if(b!=cur)
		{
			OneVisibility one;
			one.interval_begin=begin;
			one.interval_size=i-begin;
			xassert(one.interval_size>0);
			one.value=cur;
			intervals.push_back(one);

			begin=i;
			cur=b;
		}
	}

	OneVisibility one;
	one.interval_begin=begin;
	one.interval_size=i-begin;
	xassert(one.interval_size>0);
	one.value=cur;
	intervals.push_back(one);

	int sum=0;
	int cur_begin=0;
	for(i=0;i<intervals.size();i++)
	{
		OneVisibility& o=intervals[i];
		xassert(o.interval_begin==sum);
		sum+=o.interval_size;
	}
	xassert(sum==size);

	saver.push(C3DX_NODE_VISIBILITY);
	saver<<(int)intervals.size();
	for(i=0;i<intervals.size();i++)
	{
		OneVisibility& o=intervals[i];
		saver<<float((o.interval_begin)/(float)interval_size);
		saver<<float(o.interval_size/(float)interval_size);
		saver<<o.value;
	}
	saver.pop();
}
