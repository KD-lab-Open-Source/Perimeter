#include <stdafx.h>
#include "ExportMesh.h"
#include "DebugDlg.h"

ExportMesh::ExportMesh(CSaver& saver_,const char* name_)
:saver(saver_),name(name_)
{
	pobject=NULL;
	inode_current=-1;
}

void ExportMesh::Export(IGameMesh* pobject_,int inode,Matrix3 node_matrix_)
{
	inode_current=inode;
	node_matrix=node_matrix_;
	inv_node_matrix=Inverse(node_matrix);
	inv_node_norm=inv_node_matrix;
	inv_node_norm.NoTrans();

	saver.push(C3DX_MESH);
	pobject=pobject_;
	pobject->InitializeData();

	if(pobject->GetNumberOfVerts()<=0)
	{
		Msg("Error: %s cannot have vertex.\n",name);
		return;
	}

	if(pobject->GetNumberOfFaces()<=0)
	{
		Msg("Error: %s cannot have faces.\n",name);
		return;
	}

	{
		saver.push(C3DX_MESH_HEAD);
		saver<<inode;
		IGameMaterial*  material=pobject->GetMaterialFromFace(0);
		int mat_index=pRootExport->FindMaterialIndex(material);
		if(mat_index<0)
		{
			Msg("Error: %s cannot have material.\n",name);
		}

		saver<<mat_index;
		saver.pop();
	}

	ExportOneMesh();
	saver.pop();
}

struct RePoints
{
	struct ATTR
	{
		int texel;
		int normal;
	};

	struct SKIN
	{
		float weight;
		int   inode;
	};

	vector<ATTR> attr;
	vector<SKIN> skin;
	int new_base_index;
	Matrix3 skin_inv;

	RePoints():new_base_index(0)
	{
		skin_inv.IdentityMatrix();
	}

	void AddUnicalInt(int texel,int normal)
	{
		for(int i=0;i<attr.size();i++)
			if(attr[i].texel==texel && attr[i].normal==normal)
				return;
		ATTR a;
		a.texel=texel;
		a.normal=normal;
		attr.push_back(a);
	}

	int FindNewPoint(int texel,int normal)
	{
		xassert(attr.size());

		for(int i=0;i<attr.size();i++)
		{
			if(attr[i].texel==texel && attr[i].normal==normal)
				return new_base_index+i;
		}

		xassert(0);
		return new_base_index;
	}
};

struct SortByWeight
{
	bool operator ()(const RePoints::SKIN& p0,const RePoints::SKIN& p1)const
	{
		return !(p0.weight<p1.weight);
	}
};

void ExportMesh::DeleteSingularPolygon(vector<FaceEx>& faces)
{
	int deleted=0;
	for(int i=0;i<faces.size();i++)
	{ // удаление вырожденных треугольников
		FaceEx &p=faces[i];
		if(p.vert[0]==p.vert[1] || 
		   p.vert[0]==p.vert[2] || 
		   p.vert[1]==p.vert[2])
		{
			faces.erase(faces.begin()+i);
			i--;
			deleted++;
		}
	}

	if(dbg_show_info_polygon)
		Msg("info: %s deleted=%i\n",name,deleted);
}

bool ExportMesh::ExportOneMesh()
{
	vector<RePoints> pnt(pobject->GetNumberOfVerts());
	vector<FaceEx> faces;

	bool is_uv=false;
	if(pobject->GetNumberOfTexVerts()>0)
		is_uv=true;

	{
		int num_faces=pobject->GetNumberOfFaces();
		faces.resize(num_faces);
		for(int i=0;i<num_faces;i++)
		{
			faces[i]=*pobject->GetFace(i);
		}
	}

	DeleteSingularPolygon(faces);//Потом включить

	for(int i=0;i<faces.size();i++)
	{
		FaceEx& f=faces[i];
		for(int j=0;j<3;j++)
		{
			xassert(f.vert[j]<pnt.size());
			pnt[f.vert[j]].AddUnicalInt(f.texCoord[j],f.norm[j]);
		}
	}

	int num_bones_vertex[max_bones+2];//Количество вертексов с таким количеством bones
	for(i=0;i<max_bones+2;i++)
		num_bones_vertex[i]=0;

	int n_vertex=0;
	for(i=0;i<pnt.size();i++)
	{
		n_vertex+=max(pnt[i].attr.size(),1);
	}

	bool is_skin=false;
	if(pobject->IsObjectSkinned())
	{
		is_skin=true;
		IGameSkin* pskin=pobject->GetIGameSkin();
		int num_skin_verts=pskin->GetNumOfSkinnedVerts();
		int num_verts=pnt.size();
		xassert(num_verts<=num_skin_verts);
		for(i=0;i<pnt.size();i++)
		{
			RePoints& p=pnt[i];
			int nbones=pskin->GetNumberOfBones(i);

			IGameSkin::VertexType vt=pskin->GetVertexType(i);
			if(nbones==0)
			{
				p.skin.resize(1);
				RePoints::SKIN& s=p.skin[0];
				s.weight=1;
				s.inode=inode_current;
				p.skin_inv=inv_node_matrix;
//				GMatrix intMat;
//				pskin->GetInitSkinTM(intMat);
//				Matrix3 matw=intMat.ExtractMatrix3();
//				p.skin_inv=Inverse(matw);
			}else
			{
				p.skin.resize(nbones);
				Matrix3 sum;
				sum.Zero();
				float sum_weight=0;
				for(int ibone=0;ibone<nbones;ibone++)
				{
					RePoints::SKIN& s=p.skin[ibone];
					s.weight=pskin->GetWeight(i,ibone);
					IGameNode* pnode=pskin->GetIGameBone(i,ibone);
					s.inode=pRootExport->FindNodeIndex(pnode);
					xassert(s.inode>=0);

					GMatrix intMat;
					verify(pskin->GetInitBoneTM(pnode,intMat));
					Matrix3 matw=intMat.ExtractMatrix3();
					matw*=s.weight;
					sum_weight+=s.weight;
					sum+=matw;
				}

				if(sum_weight<0.999f)
				{
					nbones++;
					p.skin.resize(nbones);

					RePoints::SKIN& s=p.skin[nbones-1];
					s.weight=1-sum_weight;
					s.inode=inode_current;

					GMatrix intMat;
					pskin->GetInitSkinTM(intMat);
					Matrix3 matw=intMat.ExtractMatrix3();
					matw*=s.weight;
					sum_weight+=s.weight;
					sum+=matw;
				}

				xassert(fabsf(sum_weight-1)<1e-3f);

				GMatrix intMat;
				pskin->GetInitSkinTM(intMat);
				Matrix3 matw=intMat.ExtractMatrix3();
				p.skin_inv=inv_node_matrix*matw*Inverse(sum);
			}

			{
				int mbones=min(nbones,max_bones+1);
				num_bones_vertex[mbones]++;
			}

			sort(p.skin.begin(),p.skin.end(),SortByWeight());
		}

	}


	int n_polygon=faces.size();
	vector<sVertex> new_vertex(n_vertex);
	vector<sPolygon> new_polygon(n_polygon);

	{
		int cur_vertex=0;
		for(i=0;i<pnt.size();i++)
		{
			RePoints& p=pnt[i];
			p.new_base_index=cur_vertex;
			for(int j=0;j<p.attr.size();j++,cur_vertex++)
			{
				sVertex& v=new_vertex[cur_vertex];

				Point3 pos0=pobject->GetVertex(i);
				Point3 pos;
				int norm_index=p.attr[j].normal;
				xassert(norm_index<pobject->GetNumberOfNormals());
				Point3 norm0=pobject->GetNormal(norm_index);
				Point3 norm;

				if(is_skin)
				{
					pos=pos0*p.skin_inv;
					Matrix3 inv_norm=p.skin_inv;
					inv_norm.NoTrans();
					norm=norm0*inv_norm;
				}else
				{
					pos=pos0*inv_node_matrix;
					norm=norm0*inv_node_norm;
				}
				v.pos.x=pos.x;
				v.pos.y=pos.y;
				v.pos.z=pos.z;

				v.norm.x=norm.x;
				v.norm.y=norm.y;
				v.norm.z=norm.z;
				
				if(is_uv)
				{
					int tex_index=p.attr[j].texel;
					xassert(tex_index<pobject->GetNumberOfTexVerts());
					Point2 tex=pobject->GetTexVertex(tex_index);
					v.uv.x=tex.x;
					v.uv.y=tex.y;
				}else
				{
					v.uv.x=v.uv.y=0;
				}

				for(int ibone=0;ibone<max_bones;ibone++)
				{
					v.bones[ibone]=0;
					v.bones_inode[ibone]=-1;
				}

				int nbones=min(max_bones,p.skin.size());
				for(ibone=0;ibone<nbones;ibone++)
				{
					v.bones[ibone]=p.skin[ibone].weight;
					v.bones_inode[ibone]=p.skin[ibone].inode;
				}
			}
		}
	}
	
	for(i=0;i<n_polygon;i++)
	{
		FaceEx& f=faces[i];

		for(int j=0;j<3;j++)
		{
			new_polygon[i].p[j]=pnt[f.vert[j]].FindNewPoint(f.texCoord[j],f.norm[j]);
		}
	}

	SortPolygon(&new_polygon[0],n_polygon);

	if(new_vertex.size()>65535)
	{
		Msg("Error: %s %i vertex. Количество вершин в объекте слишком велико.\n",name,new_vertex.size());
		return false;
	}

	saver.push(C3DX_MESH_VERTEX);
	saver<<new_vertex.size();
	for(i=0;i<new_vertex.size();i++)
	{
		saver<<new_vertex[i].pos;
	}
	saver.pop();

	saver.push(C3DX_MESH_NORMAL);
	saver<<new_vertex.size();
	for(i=0;i<new_vertex.size();i++)
	{
		saver<<new_vertex[i].norm;
	}
	saver.pop();

	if(is_uv)
	{
		saver.push(C3DX_MESH_UV);
		saver<<new_vertex.size();
		for(i=0;i<new_vertex.size();i++)
		{
			saver<<new_vertex[i].uv;
		}
		saver.pop();
	}

	if(is_skin)
	{
		saver.push(C3DX_MESH_SKIN);
		saver<<new_vertex.size();
		for(i=0;i<new_vertex.size();i++)
		{
			sVertex& v=new_vertex[i];
			for(int ibone=0;ibone<max_bones;ibone++)
			{
				saver<<v.bones[ibone];
				saver<<v.bones_inode[ibone];
			}
		}
		saver.pop();
	}

	saver.push(C3DX_MESH_FACES);
	saver<<(int)new_polygon.size();
	for(i=0;i<new_polygon.size();i++)
	{
		sPolygon p=new_polygon[i];
//		swap(p.p[0],p.p[1]);
		for(int j=0;j<3;j++)
		{
			xassert(p.p[j]>=0 && p.p[j]<65535);
			WORD pp=p.p[j];
			saver<<pp;
		}
	}
	saver.pop();

	if(dbg_show_info_polygon)
	{
		Msg("%s polygon=%i vertex=%i\n",name,n_polygon,n_vertex);
		if(is_skin)
		{
			Msg("%s bones",name);
			for(i=0;i<max_bones+2;i++)
			{
				Msg(" %i=%i",i,num_bones_vertex[i]);
			}
			Msg("\n");
		}
	}
	return true;
}

int ExportMesh::NumCashed(sPolygon* polygon,int n_polygon)
{
	const max_cache_size=8;
	int cache_size=0;
	list<int> cache;

	int kgood=0;
	int kmiss=0;
	for(int i=0;i<n_polygon;i++)
	{
		sPolygon& cur=polygon[i];
		for(int k=0;k<3;k++)
		{
			int poly=cur.p[k];
			list<int>::iterator it;
			bool in=false;
			FOR_EACH(cache,it)
			{
				if(*it==poly)
				{
					in=true;
					break;
				}
			}

			if(in)
				kgood++;
			else
				kmiss++;
		}

		for(k=0;k<3;k++)
		{
			int poly=cur.p[k];
			list<int>::iterator it;
			bool in=false;
			FOR_EACH(cache,it)
			{
				if(*it==poly)
				{
					in=true;
					break;
				}
			}

			if(in)
			{
				swap(*it,cache.back());
			}else
			{
				cache.push_back(poly);
				cache_size++;
				if(cache_size>max_cache_size)
					cache.pop_front();
			}
		}
	}

	xassert(kgood+kmiss==n_polygon*3);
	return kmiss;
}
//*
void ExportMesh::SortPolygon(sPolygon* polygon,int n_polygon)
{
	//Не забыть попробовать сортировку по большему количесту точек.
	int prev_cashed=NumCashed(polygon,n_polygon);
	int sorted=0;

	for(int i=1;i<n_polygon;i++)
	{
		int joptimal=-1;
		int koptimal=-1;
		sPolygon& prev=polygon[i-1];
		for(int j=i;j<n_polygon;j++)
		{
			sPolygon& cur=polygon[j];
			int k=0;
			if(cur.p[0]==prev.p[0])k++;
			if(cur.p[1]==prev.p[0])k++;
			if(cur.p[2]==prev.p[0])k++;
			if(cur.p[1]==prev.p[1])k++;
			if(cur.p[2]==prev.p[1])k++;
			if(cur.p[2]==prev.p[2])k++;
			if(k>koptimal)
			{
				joptimal=j;
				koptimal=k;
				if(k>1)
					break;
			}
		}

		if(joptimal!=i)
		{
			sPolygon tmp=polygon[i];
			polygon[i]=polygon[joptimal];
			polygon[joptimal]=tmp;
			sorted++;
		}
	}

	int cur_cashed=NumCashed(polygon,n_polygon);

	if(dbg_show_info_polygon)
		Msg("info: sorted=%i prev=%i cur=%i\n",sorted,prev_cashed,cur_cashed);
}
/*/
void ExportMesh::SortPolygon(sPolygon* polygon,int n_polygon)
{
	//Не забыть попробовать сортировку по большему количесту точек.
	int prev_cashed=NumCashed(polygon,n_polygon);
	int sorted=0;

	const max_cache_size=8;
	int cache_size=0;
	list<int> cache;

	for(int i=0;i<n_polygon;i++)
	{
		int joptimal=-1;
		int koptimal=-1;

		int jmax=n_polygon;//min(n_polygon,i+500);
		for(int j=i;j<jmax;j++)
		{
			sPolygon& cur=polygon[j];

			int in_cache=0;

			for(int k=0;k<3;k++)
			{
				int poly=cur.p[k];
				list<int>::iterator it;
				bool in=false;
				FOR_EACH(cache,it)
				{
					if(*it==poly)
					{
						in_cache++;
						break;
					}
				}
			}

			if(in_cache>koptimal)
			{
				joptimal=j;
				koptimal=in_cache;
				if(in_cache>2)
					break;
			}
		}

		if(joptimal!=i)
		{
			sPolygon tmp=polygon[i];
			polygon[i]=polygon[joptimal];
			polygon[joptimal]=tmp;
			sorted++;
		}

		sPolygon cur=polygon[i];

		for(int k=0;k<3;k++)
		{
			int poly=cur.p[k];
			list<int>::iterator it;
			bool in=false;
			FOR_EACH(cache,it)
			{
				if(*it==poly)
				{
					in=true;
					break;
				}
			}

			if(in)
			{
				swap(*it,cache.back());
			}else
			{
				cache.push_back(poly);
				cache_size++;
				if(cache_size>max_cache_size)
				{
					cache_size--;
					cache.pop_front();
				}
			}
		}
	}

	int cur_cashed=NumCashed(polygon,n_polygon);

	if(dbg_show_info_polygon)
		Msg("info: sorted=%i prev=%i cur=%i\n",sorted,prev_cashed,cur_cashed);
}
/**/