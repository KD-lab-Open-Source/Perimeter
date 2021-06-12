#include "StdAfxRD.h"
#include "ShadowVolume.h"
#include "MeshTri.h"
/*
Что надо переделать другим.
1) Выключить тени на ландшафте.
2) Убрать открытые рёбра
*/

ShadowVolume::ShadowVolume()
{
}

ShadowVolume::~ShadowVolume()
{
}

bool ShadowVolume::Add(MatXf& mat,cMeshTri* pTri)
{
	if(pTri->NumVertex==0)
		return false;
	int offset_vertex=vertex.size();
	vertex.resize(offset_vertex+pTri->NumVertex);
	void *pVertex=gb_RenderDevice->LockVertexBuffer(*pTri->vb);
	int i;
	for(i=0;i<pTri->NumVertex;i++)
	{
		vertex[i+offset_vertex]=mat*pTri->GetPos(pVertex,i+pTri->OffsetVertex);
	}

	gb_RenderDevice->UnlockVertexBuffer(*pTri->vb);
	
	int offset_poly=triangle.size();
	triangle.resize(offset_poly+pTri->NumPolygon);
	sPolygon *Polygon=gb_RenderDevice->LockIndexBuffer(*pTri->ib);
	for(i=0;i<pTri->NumPolygon;i++)
	{
		sPolygon &p=Polygon[i+pTri->OffsetPolygon];
		sv_triangle& s=triangle[i+offset_poly];
		s.v[0]=p.p1-pTri->OffsetVertex;
		s.v[1]=p.p2-pTri->OffsetVertex;
		s.v[2]=p.p3-pTri->OffsetVertex;
	}
	gb_RenderDevice->UnlockIndexBuffer(*pTri->ib);

	DeleteRepeatedVertex(offset_vertex,pTri->NumVertex,
						 offset_poly,pTri->NumPolygon);

	return true;
}

void ShadowVolume::EndAdd()
{
	for(int i=0;i<triangle.size();i++)
	{
		sv_triangle& t=triangle[i];
		t.n.Set(vertex[t.v[0]],vertex[t.v[1]],vertex[t.v[2]]);
	}

	ComputeWingedEdges();
}

void ShadowVolume::DeleteRepeatedVertex(int offset_vertex,int size_vertex,
										int offset_poly,int size_poly)
{
	sBox6f bound;
	bound.min=bound.max=vertex[offset_vertex];
	int i;
	for(i=0;i<size_vertex;i++)
		bound.AddBound(vertex[offset_vertex+i]);

	float diameter=bound.min.distance(bound.max);
	float delta=diameter*1e-3f;
	float delta2=delta*delta;

	int vertex_size_new=size_vertex;
	vector<int> repeat(size_vertex);
	//Найти повторяющие
	for(i=0;i<size_vertex;i++)
		repeat[i]=i+offset_vertex;

	for(i=0;i<size_vertex;i++)
	{
		Vect3f& p=vertex[offset_vertex+i];
		for(int j=0;j<offset_vertex+i;j++)
		{
			Vect3f& p1=vertex[j];
			float d=p1.distance2(p);
			if(d<delta2)
			{
				repeat[i]=j;
				vertex_size_new--;
				break;
			}
		}
	}

	//Компактифицировать индексы
	int j=0;
	for(i=0;i<size_vertex;i++)
	{
		if(repeat[i]==i+offset_vertex)
		{
			repeat[i]=j+offset_vertex;
			j++;
		}else
		{
			if(repeat[i]>=offset_vertex)
				repeat[i]=repeat[repeat[i]-offset_vertex];
		}
	}

	//apply
	for(i=0;i<size_vertex;i++)
	if(repeat[i]>=offset_vertex)
		vertex[repeat[i]]=vertex[i+offset_vertex];

	int poly_size_new=0;
	for(i=0;i<size_poly;i++)
	{
		sv_triangle& s=triangle[i+offset_poly];
		for(int j=0;j<3;j++)
		{
			VISASSERT(s.v[j]<size_vertex);
			s.v[j]=repeat[s.v[j]];
			VISASSERT(s.v[j]<vertex_size_new+offset_vertex);
		}
		if(!(s.v[0]==s.v[1] || s.v[0]==s.v[2] || s.v[1]==s.v[2]))
		{
			if(i!=poly_size_new)
				triangle[poly_size_new+offset_poly]=s;
			poly_size_new++;
		}else
		{
			int k=0;	
		}
	}
	
	vertex.resize(offset_vertex+vertex_size_new);
	triangle.resize(offset_poly+poly_size_new);
}

/*
  add_edge will look to see if the current edge is already in the list.
  If it is not, it will add it.  If it is, it will replace the w[1] in
  the existing table with w[0] from the edge being added.
*/

void ShadowVolume::AddEdge(sv_edge& we)
{
	int esize = edge.size();
	for(int i=0; i < esize; i++)
	{
		sv_edge & we0 = edge[i];
		if(we0.e[0] == we.e[0]  && we0.e[1] == we.e[1])
		{
			VISASSERT("facingness different between polys on edge!");
		}
		if(we0.e[0] == we.e[1]  && we0.e[1] == we.e[0])
		{
			if(we0.w[1] != -1)
			{
				VISASSERT("triple edge! bad...");
			}
			we0.w[1] = we.w[0]; // pair the edge and return
			return;
		}
	}
	edge.push_back(we);  // otherwise, add the new edge
}

int ShadowVolume::ComputeWingedEdges()
{
	unsigned int i;
	// for each triangle, try to add each edge to the winged_edge vector,
	// but check first to see if it's already there
	unsigned int tsize = triangle.size();
	for(i=0; i < tsize; i++)
	{
		sv_triangle & t = triangle[i];
		for(int j=0; j < 3; j++)
		{
			sv_edge we;
			we.e[0] = t.v[   j   ];
			we.e[1] = t.v[(j+1)%3];
			we.w[0] = i;
			we.w[1] = -1;  // subsequent attempt to add this edge will replace w[1] 
			AddEdge(we);
		}
	}
	int open_edge = 0;
	for(i=0; i < edge.size(); i++)
	{
		if(edge[i].w[1] == -1)
			open_edge++;
	}
	//fprintf(stderr, "out of % edges, there were %d open edges\n", edge.size(), open_edge);
	return open_edge;
}

void ShadowVolume::Draw(cCamera *camera,const MatXf& mat,Vect3f light,bool line)
{
	//DrawEdge(camera,mat,light);
	DrawVolume(camera,mat,light,line);
}

void ShadowVolume::DrawEdge(cCamera *camera,const MatXf& mat,Vect3f light)
{
	for(int i=0;i<edge.size();i++)
	{
		sv_edge& e=edge[i];
		Vect3f p1=mat*vertex[e.e[0]];
		Vect3f p2=mat*vertex[e.e[1]];
		gb_RenderDevice->DrawLine(p1,p2,sColor4c(255,255,255,255));
	}
}

// This routine draws the extruded "possible silhouette" edge.  The
// edge is extruded to infinity.

// The paper describes identifying silhouette edge loops.  The approach
// in this demo is to visit each edge, determine if it's a "possible silhouette"
// or not, and if it is, draw the extruded edge.   This approach is not
// as efficient, but it has the benefit of being extremely simple.

// This routine also doubles as the routine for drawing the local and ininite
// silhouette edges (when prim == GL_LINES).

void ShadowVolume::DrawVolume(cCamera *camera,const MatXf& mat,Vect3f light_dir,bool line)
{
	Mat3f mat_inv;
	mat_inv.invert(mat.rot());
	Vect3f olight=mat_inv*light_dir;

	cVertexBuffer<sVertexXYZDT1>* pBuf=NULL;
	sVertexXYZDT1 *Vertex=NULL;
	int nVertex=0;
	sColor4c color(0,0,0,128);

	if(!line)
	{
		pBuf=camera->GetRenderDevice()->GetBufferXYZDT1();
		Vertex=pBuf->Lock();
	}

	for(unsigned int i=0; i < edge.size(); i++)
	{
		sv_edge & we = edge[i];
//		if(we.w[0] == -1 || we.w[1] == -1)
//		   continue;

		sv_triangle& p0 = triangle[we.w[0]];
		float f0 = //p0.n.GetDistance(olight);
					p0.n.A*olight.x+p0.n.B*olight.y+p0.n.C*olight.z;
			
		float f1 = -f0;
		if(we.w[1] != -1)
		{
			sv_triangle& p1 = triangle[we.w[1]];
			f1 = p1.n.A*olight.x+p1.n.B*olight.y+p1.n.C*olight.z;
				//p1.n.GetDistance(olight);
		}


		int edge[2];

		if(f0 >= 0 && f1 < 0)
		{
			edge[0] = we.e[1];
			edge[1] = we.e[0];
		}
		else if(f1 >= 0 && f0 < 0)
		{
			edge[0] = we.e[0];
			edge[1] = we.e[1];
		}
		else
		{
			if(line)
			{
				edge[0] = we.e[0];
				edge[1] = we.e[1];
			}else
			continue;
		}
		
		Vect3f& pn0 = vertex[edge[0]];
		Vect3f& pn1 = vertex[edge[1]];

		if(line)
		{
			if(we.w[1] == -1)
				gb_RenderDevice->DrawLine(mat*pn0,mat*pn1,sColor4c(255,0,0,255));
			else
				gb_RenderDevice->DrawLine(mat*pn0,mat*pn1,sColor4c(255,255,255,255));
		}else
		{
			sVertexXYZDT1 *v=&Vertex[nVertex];
			Vect3f v2=pn0+olight*1000;
			Vect3f v3=pn1+olight*1000;
			v[0].pos=pn0; v[0].diffuse=color;
			v[1].pos=pn1; v[1].diffuse=color;
			v[2].pos=v2; v[2].diffuse=color;

			v[3].pos=pn1; v[3].diffuse=color;
			v[4].pos=v3; v[4].diffuse=color;
			v[5].pos=v2; v[5].diffuse=color;

			nVertex+=6;

			if(nVertex+6>=pBuf->GetSize())
			{
				pBuf->Unlock(nVertex);
				pBuf->DrawPrimitive(PT_TRIANGLELIST,nVertex/3,mat);
				Vertex=pBuf->Lock();
				nVertex=0;
			}

		}
/*
		// local segment
		glVertex4f(pn0.x, pn0.y, pn0.z, 1);
		glVertex4f(pn1.x, pn1.y, pn1.z, 1);

		// segment projected to infinity
		glVertex4f(pn1.x*olight[3] - olight[0],
			pn1.y*olight[3] - olight[1],
			pn1.z*olight[3] - olight[2],
			0);
		glVertex4f(pn0.x*olight[3] - olight[0],
			pn0.y*olight[3] - olight[1],
			pn0.z*olight[3] - olight[2],
			0);
*/
	}

	if(!line)
	{
		pBuf->Unlock(nVertex);
		if(nVertex>0)
			pBuf->DrawPrimitive(PT_TRIANGLELIST,nVertex/3,mat);
	}
}

