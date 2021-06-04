//Balmer,K-D Lab
#include "StdAfx.h"
#include "Terra.h"
#include "ClusterFind.h"

////////////////////////////////////////////////////////////
//		ClusterFind
////////////////////////////////////////////////////////////

const int size_child8=8;
const int sx8[size_child8]={-1, 0,+1,+1,+1, 0,-1,-1};
const int sy8[size_child8]={-1,-1,-1, 0,+1,+1,+1, 0};

const int size_child=4;
const int sx[size_child]={ 0,+1, 0,-1};
const int sy[size_child]={-1, 0,+1, 0};


//Не в коем случае не менять порядок
/*
const DWORD id_mask=0x00FFFFFF;
const DWORD limits_mask=0xFF000000;

const DWORD next_limits[size_child]={
  ((0xFFu&~(1u<<0))<<24)&id_mask,
  ((0xFFu&~(1u<<1))<<24)&id_mask, 
  ((0xFFu&~(1u<<2))<<24)&id_mask,
  ((0xFFu&~(1u<<3))<<24)&id_mask,
  ((0xFFu&~(1u<<4))<<24)&id_mask,
  ((0xFFu&~(1u<<5))<<24)&id_mask, 
  ((0xFFu&~(1u<<6))<<24)&id_mask,
  ((0xFFu&~(1u<<7))<<24)&id_mask
};
*/

ClusterFind::ClusterFind(int _dx,int _dy,int _max_distance)
{
	dx=_dx;dy=_dy;
	max_distance=_max_distance;
	pmap=new DWORD[dx*dy];
	pone=new Front[max_cell_in_front];
	ptwo=new Front[max_cell_in_front];
	size_one=size_two=0;

	walk_map=new BYTE[dx*dy];

	is_used_size=dx*dy;
	is_used=new BYTE[is_used_size];
	memset(is_used,0,is_used_size);

	is_used_xmin=dx;
	is_used_xmax=-1;
	is_used_ymin=dx;
	is_used_ymax=-1;

	quant_of_build=0;
	cur_quant_build=0;
}

ClusterFind::~ClusterFind()
{
	delete[] pmap;

	delete[] pone;
	delete[] ptwo;

	delete[] is_used;
	delete[] walk_map;
}

void ClusterFind::Set(bool enable_smooting)
{
	if(enable_smooting)
		Smooting();

	memset(pmap,0,dx*dy*sizeof(pmap[0]));

	//Переделать, при превышении предела всё рухнет
	all_cluster.clear();
	all_cluster.resize(1);
	all_cluster.reserve(max_cluster_size);

	Cluster* first_element=&all_cluster[0];
	{
		Cluster& c=all_cluster[0];
		c.x=c.y=0;
		c.xcenter=c.ycenter=0;
		c.walk=0;
		c.self_id=0;
	}

	int cur_num=1;
	for(int y=0;y<dy;y++)
	{
		for(int x=0;x<dx;x++)
		{
			DWORD p=pmap[y*dx+x];
			if(p==0)
			{
				all_cluster.resize(cur_num+1);
				ClusterOne(x,y,cur_num+1,all_cluster[cur_num]);
				cur_num++;
			}
		}
	}

//	xassert(first_element==&all_cluster[0]);

	Relink();

	quant_of_build=0;
	cur_quant_build=0;
}

void ClusterFind::Relink()
{
	vector<Cluster>::iterator it;

	FOR_EACH(all_cluster,it)
	{
		Cluster& c=*it;
		int size=c.index_link.size();
		c.link.resize(size);

		for(int i=0;i<size;i++)
		{
			DWORD il=c.index_link[i];
			xassert(//il>=0 && 
				il<all_cluster.size());
			c.link[i]=&all_cluster[il];
		}
	}
}

void ClusterFind::CheckAllLink()
{
	Cluster* first=&all_cluster[0];
	vector<Cluster>::iterator it;
	FOR_EACH(all_cluster,it)
	{
		Cluster& c=*it;

		xassert(c.x>=0 && c.x<4096);
		xassert(c.y>=0 && c.y<4096);
		xassert(c.xcenter>=0 && c.xcenter<4096);
		xassert(c.ycenter>=0 && c.ycenter<4096);
		xassert(/*c.self_id>=0 &&*/	c.self_id<8192);

		vector<DWORD>::iterator itd;
		FOR_EACH(c.index_link,itd)
		{
			xassert(/* *itd>=0 && */*itd<all_cluster.size());
		}

		vector<Cluster*>::iterator itc;
		FOR_EACH(c.link,itc)
		{
			Cluster* lc=*itc;
			xassert(lc>=&all_cluster[0]);
			xassert(lc<=&all_cluster[all_cluster.size()-1]);
		}

	}
}

void ClusterFind::ClusterOne(int x,int y,int id,Cluster& c)
{
	Front pnt;
	pnt.x=x;pnt.y=y;
	pone[0]=pnt;
	size_one=1;

	int num_point=1;
	pmap[y*dx+x]=id;
	BYTE weq=walk_map[y*dx+x];

	c.x=x;c.y=y;
	c.xcenter=x;
	c.ycenter=y;
	c.temp_set=false;
	c.walk=weq;
	c.self_id=id;

	vtemp_set.clear();

	for(int i=0;i<=max_distance;i++)
	{
		size_two=0;
		if(i==max_distance)
			size_two=max_cell_in_front;

		for(int j=0;j<size_one;j++)
		{
			Front& pos=pone[j];
			DWORD& p=pmap[pos.y*dx+pos.x];

			for(int k=0;k<size_child;k++)
			{
				DWORD xx=pos.x+sx[k],yy=pos.y+sy[k];
				if(xx>=dx || yy>=dy)continue;

				DWORD addp=yy*dx+xx;
				DWORD& pd=pmap[addp];
				BYTE w=walk_map[addp];

				if(pd!=0)
				{
					Cluster& ct=all_cluster[pd-1];

					if(ct.temp_set)
					{

						xassert(pd<all_cluster.size());
						ct.temp_set=false;
						vtemp_set.push_back(pd-1);
					}
				}else
				{
					if(w==weq && size_two<max_cell_in_front)
					{
						pd=id;
						c.xcenter+=xx;
						c.ycenter+=yy;
						num_point++;

						Front pnt;
						pnt.x=xx;pnt.y=yy;

						ptwo[size_two++]=pnt;
					}
				}
			}
		}

		//swap
		swap(pone,ptwo);
		swap(size_one,size_two);
	}

	c.xcenter/=num_point;
	c.ycenter/=num_point;
	c.temp_set=true;

	vector<DWORD>::iterator it;
	FOR_EACH(vtemp_set,it)
	{
		DWORD d=*it;
		xassert(d<all_cluster.size());
		Cluster& cd=all_cluster[d];
		cd.temp_set=true;
		cd.index_link.push_back(id-1);
		c.index_link.push_back(d);
	}

}

void ClusterFind::SoftPath(vector<Cluster*>& in_path,
				Vect2i from,Vect2i to,
				vector<Vect2i>& out_path)
{
	out_path.clear();
	if(in_path.empty())return;

	out_path.push_back(from);
	int size=in_path.size();

	Vect2i p0,p1,p2;
	p0=from;

	vector<Vect2i> path;
	path.reserve(8);

	for(int i=1;i<size;i++)
	{
		path.clear();
		Cluster *c0,*c1;

		c0=in_path[i];

		if(i==size-1)
		{
			c1=c0;
			p2=p1=to;
		}else
		{
			c1=in_path[i+1];
			p1.x=c0->xcenter;
			p1.y=c0->ycenter;
			p2.x=c1->xcenter;
			p2.y=c1->ycenter;
		}
			


		Vect2i up(c0->x,c0->y),
			up_to(c1->x,c1->y);

		if(IterativeFindPath(p0,
			   p1,p2,
			   up,up_to,
			   path))
			   i++;

		vector<Vect2i>::iterator it;
		FOR_EACH(path,it)
			out_path.push_back(*it);

		p0=path.back();
	}

	Vect2i cp=out_path.back();
	if(cp.x!=to.x || cp.y!=to.y)
		out_path.push_back(to);

}

bool ClusterFind::IterativeFindPath(Vect2i from,
						   Vect2i center,Vect2i to,
						   Vect2i up,Vect2i up_to,
						   vector<Vect2i>& path
						   )
{
	path.clear();
	xassert(from.x>=0 && from.x<dx && from.y>=0 && from.y<dy);
	xassert(up.x>=0 && up.x<dx && up.y>=0 && up.y<dy);
	xassert(up_to.x>=0 && up_to.x<dx && up_to.y>=0 && up_to.y<dy);

	DWORD qfrom=pmap[from.y*dx+from.x];
	DWORD qcenter=pmap[up.y*dx+up.x];
	DWORD qto=pmap[up_to.y*dx+up_to.x];

	Vect2i out;
	LINE_RET ret;

	if(qcenter!=qto)
	if(Line(from.x,from.y,to.x,to.y,
				qfrom,qto,out.x,out.y)==L_GOOD)
	{
		xassert(pmap[out.y*dx+out.x]==qto);
		path.push_back(out);
		return true;
	}

	ret=Line(from.x,from.y,center.x,center.y,
		   qfrom,qcenter,out.x,out.y);
	if(ret==L_GOOD)
	{
		xassert(pmap[out.y*dx+out.x]==qcenter);
		path.push_back(out);
		return false;
	}

	vector<Front> front;
	FindClusterFront(from.x,from.y,qcenter,front);
	if(front.empty())
	{
		out=center;
		path.push_back(out);

		{
			Cluster& c=all_cluster[qfrom-1];
			Cluster::iterator it;
			bool ok=false;
			FOR_EACH(c,it)
			{
				Cluster* c1=*it;
				if(c1->self_id==qcenter)
					ok=true;
			}

			xassert(ok);
		}

		return false;
	}

	float distance;
	Front f,end_point;

	f.x=f.y=-1;
	distance=1e4;

	vector<Front>::iterator it;
	FOR_EACH(front,it)
	{
		Front c=*it;
		float d=sqrtf(sqr(from.x-c.x)+sqr(from.y-c.y));
		d+=sqrtf(sqr(to.x-c.x)+sqr(to.y-c.y));
		if(distance>d)
		{
			f=c;
			distance=d;
		}
	}
        int i;
	for(i=0;i<size_child8;i++)
	{
		DWORD xx=f.x+sx8[i],yy=f.y+sy8[i];
		if(xx<dx && yy<dy)
		{
			if(pmap[yy*dx+xx]==qcenter)
			{
				end_point.x=xx;
				end_point.y=yy;
				break;
			}
		}
	}
	xassert(i<size_child8);

	xassert(pmap[end_point.y*dx+end_point.x]==qcenter);

	{
		vector<Vect2i> p;
		BYTE cur=is_used[f.y*dx+f.x];

		Vect2i cf=f;
		for(BYTE b=cur-1;b>=2;b--)
		{
			for(int i=0;i<size_child8;i++)
			{
				DWORD xx=cf.x+sx8[i],yy=cf.y+sy8[i];
				if(xx<dx && yy<dy)
				{
					if(is_used[yy*dx+xx]==b)
					{
						cf.x=xx;
						cf.y=yy;
						p.push_back(cf);
						break;
					}
				}
			}
			xassert(i<size_child8);
		}

		if(p.size()>3)
		{
			int center=p.size()/2;
			Vect2i p0,p1,p2;
			p0=p.back();
			p1=p[center];
			p2=p.front();

			//Пробуем добавить первую линию
			bool first_line_added=false;
			ret=Line(from.x,from.y,p1.x,p1.y,
				qfrom,qfrom,out.x,out.y);
			if(ret==L_COMPLETE)
			{
				p.erase(p.begin()+center,p.end());
				path.push_back(p1);
				first_line_added=true;
			}

			//Пробуем добавить вторую линию
			ret=Line(p1.x,p1.y,end_point.x,end_point.y,
				qfrom,qcenter,out.x,out.y);
			if(ret==L_GOOD)
			{
				if(first_line_added)
					p.clear();
				else
					p.erase(p.begin(),p.begin()+center);
			}

		}

		/*
			Если идёт подряд несколько точек в 
			вертикальном,горизонтальном или диагональном направлении
			то удалить центральные.
		*/
		if(p.size()>2)
		{
			int size=p.size();
			int dx,dy;
			dx=p[1].x-p[0].x;
			dy=p[1].y-p[0].y;

			int imin=0;
			for(int i=0;i<size-1;i++)
			{
				int px,py;
				px=p[i+1].x-p[i].x;
				py=p[i+1].y-p[i].y;

				if(px!=dx || py!=dy)
				{
					if(imin<i-1)
					{
						p.erase(p.begin()+imin+1,p.begin()+i);
						i=imin+1;
						size=p.size();
					}

					imin=i;
					dx=px;
					dy=py;
				}
			}

			if(imin<i-1)
			{
				p.erase(p.begin()+imin+1,p.begin()+i);
			}
		}

		vector<Vect2i>::reverse_iterator it;
		for(it=p.rbegin();it!=p.rend();it++)
		{
			path.push_back(*it);
		}
	}

	path.push_back(end_point);
	return false;
}

class EmptyHeuristic
{
public:
	inline float operator()(BYTE from,BYTE to){return 0;}
};

ClusterFind::LINE_RET 
	ClusterFind::Line(int xfrom,int yfrom,int xto,int yto,
					   DWORD prev,DWORD eq,int& xeq,int& yeq,
					   bool enable_add_one)
{
	float x,y;
	float lx,ly;

	xassert(xfrom>=0 && xfrom<dx && yfrom>=0 && yfrom<dy);
	xassert(xto>=0 && xto<dx && yto>=0 && yto<dy);

	if(xfrom==xto && yfrom==yto)
	{
//		xassert(0);
		return L_COMPLETE;
	}

	x=xfrom;
	y=yfrom;

	lx=xto-xfrom;
	ly=yto-yfrom;
	int t,maxt;
	if(fabsf(lx)>fabsf(ly))
	{
		maxt=fabsf(lx);
		ly=ly/fabsf(lx);
		lx=(lx>0)?+1:-1;
	}else
	{
		maxt=fabsf(ly);
		lx=lx/fabsf(ly);
		ly=(ly>0)?+1:-1;
	}

	Cluster& cprev=all_cluster[prev-1];
	Cluster& ceq=all_cluster[eq-1];

	BYTE max_walk=max(cprev.walk,ceq.walk);
#ifdef CF_UP_BIT
	//Не считать непроходимым поле, 
	//если юнит направился сквозь него
	BYTE xor_mask=((cprev.walk^ceq.walk)&UP_MASK)?0:UP_MASK;
#endif

	//Надо вынести, так как эвристическая строчка и зависит от level_detail
	if(enable_add_one)
		max_walk++;

	//t<=maxt чтоб захватывало последнюю точку
	for(t=0;t<=maxt;t++)
	{
		//Здесь использовать x,y
		int ix=round(x),iy=round(y);
		DWORD q=pmap[iy*dx+ix];
		Cluster& cur=all_cluster[q-1];

#ifdef CF_UP_BIT
		if((cur.walk^max_walk)&xor_mask)
			return L_BAD;
#endif

		if(cur.walk>max_walk)
			return L_BAD;

		if(q==eq && prev!=eq)
		{
			xeq=ix;
			yeq=iy;

			bool debug_xor;
			HeuristicLine(xfrom,yfrom,xeq,yeq,
				dx,dy,walk_map,EmptyHeuristic(),
					debug_xor);

			if(debug_xor)
			{
				int k=0;
			}

			return debug_xor?L_BAD:L_GOOD;
		}
	
		x+=lx;y+=ly;
	}

	return L_COMPLETE;
}

bool ClusterFind::LineWalk(int xfrom,int yfrom,int xto,int yto,
					   BYTE max_walk)
{
	int dirx=xto-xfrom,diry=yto-yfrom;

	int t,dt;
	int xe=0,ye=0;
	int incx,incy;
	int x=xfrom,y=yfrom;

	incx=dirx>=0?+1:-1;
	incy=diry>=0?+1:-1;

	int deltax,deltay;
	deltax=dirx>=0?dirx:-dirx;
	deltay=diry>=0?diry:-diry;
	dt=deltax>deltay?deltax:deltay;
	int maxt=dt;

	//t<=maxt чтоб захватывало последнюю точку
	for(t=0;t<=maxt;t++)
	{
		//Здесь использовать x,y
		BYTE w=walk_map[y*dx+x];
		if(w>max_walk)
			return false;
	
		xe+=deltax;
		ye+=deltay;

		if(xe>=dt)
		{
			xe-=dt;
			x+=incx;
		}

		if(ye>=dt)
		{
			ye-=dt;
			y+=incy;
		}
	}

	return true;
}


void ClusterFind::FindClusterFront(int x,int y,DWORD to,
		vector<Front>& front)
{
	{
		if(is_used_xmin<=is_used_xmax)
		for(int y=is_used_ymin;y<=is_used_ymax;y++)
		{
			memset(is_used+y*dx+is_used_xmin,0,
					is_used_xmax-is_used_xmin+1);
		}
	}

	//Теоретически здесь is_used пустой
#ifdef _DEBUG
	{
		for(int j=0;j<is_used_size;j++)
			xassert(!is_used[y]);
	}
#endif _DEBUG

	Front pnt;
	pnt.x=x;pnt.y=y;
	pone[0]=pnt;
	size_one=1;

	int num_point=1;
	DWORD id=pmap[y*dx+x];

	is_used[y*dx+x]=1;

	is_used_xmin=x;
	is_used_xmax=x;
	is_used_ymin=y;
	is_used_ymax=y;

	for(int i=0;size_one>0;i++)
	{
		size_two=0;
		for(int j=0;j<size_one;j++)
		{
			Front& pos=pone[j];
			DWORD& p=pmap[pos.y*dx+pos.x];

			bool enable_add=false;

			for(int k=0;k<size_child8;k++)
			{
				DWORD xx=pos.x+sx8[k],yy=pos.y+sy8[k];
				if(xx>=dx || yy>=dy)continue;

				DWORD addp=yy*dx+xx;
				DWORD& pd=pmap[addp];
				BYTE& w=is_used[addp];

				if(pd!=id)
				{
					if(pd==to)
						enable_add=true;
				}else
				{
					if(w==0 && size_two<max_cell_in_front)
					{
						w=i+2;
						num_point++;

						Front pnt;
						pnt.x=xx;pnt.y=yy;

						ptwo[size_two++]=pnt;

						is_used_xmin=min(is_used_xmin,xx);
						is_used_xmax=max(is_used_xmax,xx);
						is_used_ymin=min(is_used_ymin,yy);
						is_used_ymax=max(is_used_ymax,yy);
					}
				}
			}

			if(enable_add)
				front.push_back(pos);
		}

		//swap
		swap(pone,ptwo);
		swap(size_one,size_two);
	}
}

void ClusterFind::Smooting()
{//Убрать мусор
	const int size_child=4;
	const int sx[size_child]={ 0,+1, 0,-1};
	const int sy[size_child]={-1, 0,+1, 0};

	for(int y=1;y<dy-1;y++)
	{
		BYTE* p=walk_map+y*dx;
		for(int x=1;x<dx-1;x++)
		{
			int b=p[x];
			int up,down,center;
			up=down=center=0;
		/*
			for(int k=0;k<size_child;k++)
			{
				int xx=x+sx[k],yy=y+sy[k];
				int cur=walk_map[yy*dx+xx];
				if(cur==b)center++;
				else if(cur<b)down++;
				else up++;
			}
		/*/
				int cur;
				cur=p[x-1];
				if(cur==b)center++;
				else if(cur<b)down++;
				else up++;

				cur=p[x+1];
				if(cur==b)center++;
				else if(cur<b)down++;
				else up++;

				cur=p[x-dx];
				if(cur==b)center++;
				else if(cur<b)down++;
				else up++;

				cur=p[x+dx];
				if(cur==b)center++;
				else if(cur<b)down++;
				else up++;
		/**/

			if(center==0 && (up>0 || down>0))
			{
				if(up>down)
					p[x]=b+1;
				else
					p[x]=b-1;
			}
		}
	}
}

struct ClusterFindNormal
{
	int dx,dy;
	float cs,sn;
	int cur_len;
};

void ClusterFind::BuildSidePath(vector<Vect2i>& in_path,
					vector<Vect2i>& out_path,
					int max_distance,bool left)
{
/*
Сдвинуть in_path максимально вбок.
Путь не должен пролегать по слишком кривой местности.
Местность, не более чем на 1 кривее той, по которой 
проходит центральный путь.

max_distance - желаемый сдвиг (равен расстоянию между юнитами)

Процесс итеративный:
Дигаем точку на max_distance,(max_distance+1)/2,3,2,1 пиксел и смотрим, 
не заехали на плохую местность.
Если заехали, уменьшаем шаг. 
И так для каждой точки.
После этого процедуру повторяем.
*/

	int size=in_path.size();
	out_path=in_path;

	if(size<2)
		return;

	vector<ClusterFindNormal> normal;
	normal.resize(size);

	for(int i=0;i<size;i++)
	{
		const float min_len=6;
		float len_to,len_from;
		int imin,imax;
		imin=0;
		imax=min(i+1,size-1);

		int parity=0;
		while(imin>0 || imax<size-1)
		{
			len_from=len_to=0;
			Vect2i cur,c;
			int j;

			cur=in_path[i];
			for(j=0;j>=imin;j--)
			{
				c=in_path[j];
				len_from+=sqrtf(sqr(c.x-cur.x)+sqr(c.y-cur.y));
				cur=c;
			}

			cur=in_path[i];
			for(j=0;j<=imax;j++)
			{
				c=in_path[j];
				len_to+=sqrtf(sqr(c.x-cur.x)+sqr(c.y-cur.y));
				cur=c;
			}

			if(len_to+len_from>=min_len)
				break;

			if(parity&1)
				imax=min(imax+1,size-1);
			else
				imin=max(imin-1,0);

			parity++;
		}

		xassert(imin<imax);
		Vect2i c0,c1;
		c0=in_path[imin];
		c1=in_path[imax];

		ClusterFindNormal& cf=normal[i];
		cf.dx=c1.x-c0.x;
		cf.dy=c1.y-c0.y;

		float len=sqrtf(sqr(cf.dx)+sqr(cf.dy));
		float mul=(left)?1:-1;
		cf.cs= (cf.dy/len)*mul;
		cf.sn=-(cf.dx/len)*mul;

		cf.cur_len=0;
	}

	bool first=true;
	int iteration=0;
	bool no_all_escape;

	const bool badd=true;
	const BYTE cmax_walk=4;

	do
	{
		no_all_escape=false;//Ни одна из точек не перемещается
		
		int maxd=first?(max_distance+1)/2:max_distance;
		first=false;

		for(int i=0;i<size;i++)
		{
			ClusterFindNormal& cf=normal[i];

			int curd=maxd;

			while(1)
			{
				//Проверка корректности
				bool is_ok=true;

				Vect2i cur;//Рассчитать
				cur=in_path[i];
				cur.x+=round(cf.cs*curd);
				cur.y+=round(cf.sn*curd);


				if(cur.x<0)cur.x=0;
				if(cur.x>=dx)cur.x=dx-1;
				if(cur.y<0)cur.y=0;
				if(cur.y>=dy)cur.y=dy-1;

				Vect2i p1=in_path[i];

				if(i>0)
				{
					Vect2i& prev=out_path[i-1];

					Vect2i p0=in_path[i-1];

					BYTE wfrom=walk_map[p0.y*dx+p0.x],
						 wto  =walk_map[p1.y*dx+p1.x];
					BYTE max_walk=max(wfrom,wto);
					if(badd)max_walk=max(max_walk,cmax_walk);

					if(!LineWalk(prev.x,prev.y,cur.x,cur.y,max_walk))
					{
						is_ok=false;
					}
				}

				if(i<size-1 && is_ok)
				{
					Vect2i& next=out_path[i+1];

					Vect2i p2=in_path[i+1];
					BYTE wfrom=walk_map[p1.y*dx+p1.x],
						 wto  =walk_map[p2.y*dx+p2.x];
					BYTE max_walk=max(wfrom,wto);
					if(badd)max_walk=max(max_walk,cmax_walk);

					if(!LineWalk(cur.x,cur.y,next.x,next.y,max_walk))
					{
						is_ok=false;
					}
				}

				if(is_ok)
				{
					no_all_escape=true;
					cf.cur_len=curd;
					out_path[i]=cur;
					break;
				}

				if(curd<=1)break;
				curd=(curd+1)>>1;
			}
			
		}

		iteration++;
	}
	//while(false);
	while(/*no_all_escape &&*/ iteration<4);


}

void ClusterFind::SetLater(bool enable_smooting,int _quant_of_build)
{
	quant_of_build=_quant_of_build;

	cur_quant_build=0;

	if(enable_smooting)
		Smooting();

	memset(pmap,0,dx*dy*sizeof(pmap[0]));

	all_cluster.clear();
	all_cluster.reserve(max_cluster_size);
	all_cluster.resize(1);
	Cluster* first_element=&all_cluster[0];
	{
		Cluster& c=all_cluster[0];
		c.x=c.y=0;
		c.xcenter=c.ycenter=0;
		c.walk=0;
		c.self_id=0;
	}

	set_later_cur_num=1;
}

bool ClusterFind::SetLaterQuant()
{
	if(ready())
		return true;

	bool end = (cur_quant_build+1) >= quant_of_build;

	int ymin = (dy*cur_quant_build)/quant_of_build;
	int ymax = (dy*(cur_quant_build+1))/quant_of_build;

	xassert(!end || ymax==dy);

	for(int y = ymin;y<ymax;y++)
	{
		for(int x = 0;x<dx;x++)
		{
			DWORD p = pmap[y*dx+x];
			if(p==0)
			{
				all_cluster.resize(set_later_cur_num+1);
				ClusterOne(x,y,set_later_cur_num+1,all_cluster[set_later_cur_num]);
				set_later_cur_num++;
			}
		}
	}

	if(end)
		Relink();

	cur_quant_build++;
	return end;
}

/*
Как проложить путь сбоку, не расстоянии примерно X.

Тупо перенести путь вбок.
Смотреть вокруг (примерно на Y,преимущественно вперёд) и 
объезжать в реальном времени маленькие горы и других юнитов.

Если впереди есть гора, которую не видно как объехать, 
то постепенно приближать путь к базовому.
Причём приближение должно происходить по возможности гладко.


Как смотреть вокруг.
Если по направлению движения есть область 
худшей проходимости, и она достаточно велика,
то постепенно рулить к базовому пути, 
иначе найти обходной путь по A*.

Достаточно большой областью считается такая, в которая простирается 
более чем на Y точек вперёд.

Как производить спрямление пути:


*/

/*
1 - определить нормали
2 - смещаться вправо и влево итеративно:
	а) пока расстояние меньше максиума
	б) пока проходимость не хуже базовой + (0.1 .. 0.2)
	   (подсчитывать среднее по линии)
	с) если дорога слишком узка, то снизить тебования к проходимости.

3 - постобработка
	где слишком быстро расширяется - сузить, 
	спрямить (даже за счёт некоторого сужения).

4 - посчитать среднюю ширину и разбить на соответствующее 
	количество линий (пропорционально ширине техники).
	
*/
