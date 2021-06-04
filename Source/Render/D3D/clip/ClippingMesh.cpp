#include "stdafxrd.h"

#include <set>
#include <vector>
#include "ClippingMesh.h"

/////////////CMesh//////////////////
CMesh::CMesh(AMesh& mesh)
{
	epsilon=1e-4f;
	Set(mesh);
}

CMesh::CMesh()
{
	epsilon=1e-4f;
}

void CMesh::Set(AMesh& mesh)
{
	V.resize(mesh.V.size());
	int i;
	for(i=0;i<V.size();i++)
		V[i].point=mesh.V[i];
	
	E.resize(mesh.E.size());
	for(i=0;i<E.size();i++)
	{
		E[i].vertex[0]=mesh.E[i].vertex[0];
		E[i].vertex[1]=mesh.E[i].vertex[1];

		set<int>::iterator it;
		FOR_EACH(mesh.E[i].face,it)
			E[i].face.insert(*it);
	}

	F.resize(mesh.F.size());
	for(i=0;i<F.size();i++)
	{
		set<int>::iterator it;
		FOR_EACH(mesh.F[i].edge,it)
			F[i].edge.insert(*it);
	}
}

int CMesh::Clip(sPlane4f clipplane)
{
	//vertex processing
	int positive=0,negative=0;
	int i;
	for(i=0;i<V.size();i++)
	{
		CVertex& v=V[i];
		if(v.visible)
		{
			v.distance=clipplane.GetDistance(v.point);
			if(v.distance>=epsilon)
			{
				positive++;
			}else
			if(v.distance<=-epsilon)
			{
				negative++;
				v.visible=false;
			}else
			{
				v.distance=0;
			}
		}
	}

	if(negative==0)
	{//no clipping
		return +1;
	}
	if(positive==0)
	{//all clipped
		return -1;
	}
	
	//Edge processing
	for(i=0;i<E.size();i++)
	{
		CEdge& e=E[i];
		if(e.visible)
		{
			float d0=V[e.vertex[0]].distance,d1=V[e.vertex[1]].distance;
			if(d0<=0 && d1<=0)
			{
				set<int>::iterator it;
				FOR_EACH(e.face,it)
				{
					int j=*it;
					//set<int>::iterator ite=F[j].edge.find(i);
					//F[j].edge.erase(ite);
					F[j].edge.erase(i);
					if(F[j].edge.empty())
						F[j].visible=false;

				}

				e.visible=false;
				continue;
			}

			if(d0>=0 && d1>=0)
			{
				continue;
			}
			
			//edge is split
			float t=d0/(d0-d1);

			CVertex intersect;
			intersect.point=(1-t)*V[e.vertex[0]].point+t*V[e.vertex[1]].point;
			int index=V.size();
			V.push_back(intersect);
			if(d0>0)
				e.vertex[1]=index;
			else
				e.vertex[0]=index;
		}
	}

	//face processing
	CFace closeFace;
//	closeFace.plane=clipplane;
	int findex=F.size();

	for(i=0;i<F.size();i++)
	{
		CFace& f=F[i];
		if(f.visible)
		{
			set<int>::iterator it;
			FOR_EACH(f.edge,it)
			{
				CEdge& e=E[*it];
				V[e.vertex[0]].occurs=0;
				V[e.vertex[1]].occurs=0;
			}

			int start,final;
			if(GetOpenPolyline(f,start,final))
			{
				CEdge closeEdge;
				int eindex=E.size();
				closeEdge.vertex[0]=start;
				closeEdge.vertex[1]=final;
				closeEdge.face.insert(i);
				f.edge.insert(eindex);

				//Code to closing polyhedron
				closeEdge.face.insert(findex);
				closeFace.edge.insert(eindex);
				
				//
				E.push_back(closeEdge);
			}
		}
	}

	F.push_back(closeFace);

	return 0;
}


bool CMesh::GetOpenPolyline(const CFace& face,int& start,int& final)
{
	set<int>::const_iterator it;
	FOR_EACH(face.edge,it)
	{
		CEdge& e=E[*it];
		V[e.vertex[0]].occurs++;
		V[e.vertex[1]].occurs++;
	}

	start=-1;
	final=-1;
	
	FOR_EACH(face.edge,it)
	{
		CEdge& e=E[*it];
		int i0=e.vertex[0],i1=e.vertex[1];
		if(V[i0].occurs==1)
		{
			if(start==-1)
				start=i0;
			else if(final==-1)
				final=i0;
		}
		if(V[i1].occurs==1)
		{
			if(start==-1)
				start=i1;
			else if(final==-1)
				final=i1;
		}
	}

	return start!=-1;
}


void CMesh::BuildPolygon(APolygons& p)
{//Самый простой метод, некоторые точки могут не использоваться.
	p.points.resize(V.size());
	int i;
	for(i=0;i<V.size();i++)
		p.points[i]=V[i].point;

	int facessize=0;
	for(i=0;i<F.size();i++)
	if(F[i].visible)
	{
		facessize++;
	}

	p.faces.resize(facessize);
	int facenum=0;
	for(i=0;i<F.size();i++)
	if(F[i].visible)
	{
		CFace& face=F[i];
		vector<int>& vertex=p.faces[facenum++];

		set<int>::iterator it;
		set<int> edge=face.edge;

		int begin,old;
		{
			it=edge.begin();
			AEdge& e=E[*it];
			begin=e.vertex[0];
			old=e.vertex[1];
			vertex.push_back(begin);
			vertex.push_back(old);
			edge.erase(it);
		}

		while(!edge.empty())
		{
			if(begin==old)
			{
				VISASSERT(0);
				break;
			}

			bool berase=false;
			set<int>::iterator it;
			FOR_EACH(edge,it)
			{
				AEdge& e=E[*it];
				if(e.vertex[0]==old || e.vertex[1]==old)
				{
					if(e.vertex[0]==old)
						old=e.vertex[1];
					else
						old=e.vertex[0];

					vertex.push_back(old);

					edge.erase(it);
					berase=true;
					break;
				}
			}
			VISASSERT(berase);
		}
	}
}

//////////////////////////////////////////////////////////
void AMesh::CreateABB(Vect3f& vmin,Vect3f& vmax)
{
	V.push_back(Vect3f(vmin.x,vmin.y,vmin.z));
	V.push_back(Vect3f(vmax.x,vmin.y,vmin.z));
	V.push_back(Vect3f(vmin.x,vmax.y,vmin.z));
	V.push_back(Vect3f(vmax.x,vmax.y,vmin.z));
	
	V.push_back(Vect3f(vmin.x,vmin.y,vmax.z));
	V.push_back(Vect3f(vmax.x,vmin.y,vmax.z));
	V.push_back(Vect3f(vmin.x,vmax.y,vmax.z));
	V.push_back(Vect3f(vmax.x,vmax.y,vmax.z));

	E.resize(12);
	
	struct
	{
		int v0,v1;
		int f0,f1;
	} edgedata[]=
	{
		{0,1,	0,1},
		{1,3,	0,2},
		{3,2,	0,3},
		{2,0,	0,4},

		{4,5,	5,1},
		{5,7,	5,2},
		{7,6,	5,3},
		{6,4,	5,4},

		{0,4,	4,1},
		{1,5,	1,2},
		{3,7,	2,3},
		{2,6,	3,4},
	};

	int i;
	for(i=0;i<12;i++)
	{
		AEdge& e=E[i];
		e.vertex[0]=edgedata[i].v0;
		e.vertex[1]=edgedata[i].v1;
		e.face.insert(edgedata[i].f0);
		e.face.insert(edgedata[i].f1);
	}
	
	F.resize(6);
	for(i=0;i<E.size();i++)
	{
		set<int>::iterator it;
		FOR_EACH(E[i].face,it)
			F[*it].edge.insert(i);
	}
}

