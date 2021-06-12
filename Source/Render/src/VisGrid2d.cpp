#include "StdAfxRD.h"
#include "VisGrid2d.h"

inline bool IsNowDelete(cIUnkClass *UnkObj)
{
//	if(MT_IS_GRAPH() && UnkObj->GetAttr(ATTRUNKOBJ_CREATED_IN_LOGIC))
//	{
//		int k=0;
//	}

	if(MT_IS_GRAPH() && MT_IS_LOGIC())
		return true;//В случае одного потока - удалять спокойно

	return MT_IS_GRAPH() && !UnkObj->GetAttr(ATTRUNKOBJ_CREATED_IN_LOGIC);
}

QuatTree::QuatTree()
{
	root=NULL;
}

QuatTree::~QuatTree()
{
	clear();	
}

void QuatTree::clear()
{
	delete root;
	root=NULL;
}

void QuatTree::find(Vect2i pos,int radius,find_proc proc,void* param)
{
	find(pos.x-radius,pos.y-radius,pos.x+radius,pos.y+radius,proc,param);
}

void QuatTree::find(int xmin,int ymin,int xmax,int ymax,find_proc proc,void* param)
{
	add_proc=proc;
	add_param=param;

	add_xmin=xmin;
	add_ymin=ymin;
	add_xmax=xmax;
	add_ymax=ymax;
	if(root)
		add(*root);
}

void QuatTree::add(QuatNode& cur)
{
	if((cur.xmin<add_xmax || cur.xmax>add_xmin)&&
	   (cur.ymin<add_ymax || cur.ymax>add_ymin))
	{
		int sz=cur.obj.size();
		for(int i=0;i<sz;i++)
			add_proc(cur.obj[i],add_param);

		if(cur.left)
			add(*cur.left);
		if(cur.right)
			add(*cur.right);
	}
}

void QuatTree::GetBorder(cIUnkClass* obj,int& xmin,int& ymin,int& xmax,int& ymax)
{
	sBox6f b;
	Vect2f mi,ma;
	obj->GetBoundBox(b);
	Vect3f	p[8];
	const MatXf& matrix=obj->GetPosition();
	matrix.xformPoint(Vect3f(b.min.x,b.min.y,b.min.z),p[0]);
	matrix.xformPoint(Vect3f(b.max.x,b.min.y,b.min.z),p[1]);
	matrix.xformPoint(Vect3f(b.min.x,b.max.y,b.min.z),p[2]);
	matrix.xformPoint(Vect3f(b.max.x,b.max.y,b.min.z),p[3]);
	matrix.xformPoint(Vect3f(b.min.x,b.min.y,b.max.z),p[4]);
	matrix.xformPoint(Vect3f(b.max.x,b.min.y,b.max.z),p[5]);
	matrix.xformPoint(Vect3f(b.min.x,b.max.y,b.max.z),p[6]);
	matrix.xformPoint(Vect3f(b.max.x,b.max.y,b.max.z),p[7]);

	mi.set(p[0].x,p[0].y);
	ma=mi;
	for(int k=0;k<8;k++)
	{
		mi.x=min(mi.x,p[k].x);
		mi.y=min(mi.y,p[k].y);
		ma.x=max(ma.x,p[k].x);
		ma.y=max(ma.y,p[k].y);
	}

	xmin=mi.x;
	xmax=ma.x;
	ymin=mi.y;
	ymax=ma.y;
}

void QuatTree::build(int xmin,int ymin,int xmax,int ymax,vect& blist)
{
//	start_timer_auto(BuildTree,1);

	clear();
	int sz=blist.size();
	if(!sz)
		return;
	vector<OneObj> tlist(sz);
	vector<OneObj*> plist(sz);
	for(int i=0;i<sz;i++)
	{
		OneObj& o=tlist[i];
		GetBorder(blist[i],o.xmin,o.ymin,o.xmax,o.ymax);
		o.obj=blist[i];
		plist[i]=&o;
	}

	root=new QuatNode;
	build(xmin,ymin,xmax,ymax,plist,root);
}

void QuatTree::build(int xmin,int ymin,int xmax,int ymax,pvect& blist,QuatNode* node)
{
	node->xmin=xmin;
	node->xmax=xmax;
	node->ymin=ymin;
	node->ymax=ymax;

	int divigex=(xmin+xmax)>>1;
	int divigey=(ymin+ymax)>>1;
	if(divigex==xmin || divigey==ymin || blist.size()<4)
	{
		int sz=blist.size();
		node->obj.resize(sz);
		for(int i=0;i<sz;i++)
			node->obj[i]=blist[i]->obj;
		return;
	}

	pvect horz_cur,horz_left,horz_right,
		  vert_cur,vert_top,vert_bottom;

	pvect::iterator it;
	//x
	FOR_EACH(blist,it)
	{
		OneObj* o=*it;
		if(o->xmin>=divigex)
			horz_right.push_back(o);
		else
		if(o->xmax<=divigex)
			horz_left.push_back(o);
		else
			horz_cur.push_back(o);
	}

	//y
	FOR_EACH(blist,it)
	{
		OneObj* o=*it;
		if(o->ymin>=divigey)
			vert_bottom.push_back(o);
		else
		if(o->ymax<=divigey)
			vert_top.push_back(o);
		else
			vert_cur.push_back(o);
	}

	int xt=horz_cur.size()+max(horz_left.size(),horz_right.size());
	int yt=vert_cur.size()+max(vert_top.size(),vert_bottom.size());
	if(xt<yt)
	{
		int sz=horz_cur.size();
		node->obj.resize(sz);
		for(int i=0;i<sz;i++)
			node->obj[i]=horz_cur[i]->obj;

		{
			pvect().swap(vert_cur);
			pvect().swap(vert_top);
			pvect().swap(vert_bottom);
			pvect().swap(horz_cur);
		}

		if(!horz_left.empty())
		{
			node->left=new QuatNode;
			build(xmin,ymin,divigex,ymax,horz_left,node->left);
		}

		if(!horz_right.empty())
		{
			node->right=new QuatNode;
			build(divigex,ymin,xmax,ymax,horz_right,node->right);
		}
	}else
	{
		int sz=vert_cur.size();
		node->obj.resize(sz);
		for(int i=0;i<sz;i++)
			node->obj[i]=vert_cur[i]->obj;

		{
			pvect().swap(horz_cur);
			pvect().swap(horz_left);
			pvect().swap(horz_right);
			pvect().swap(vert_cur);
		}

		if(!vert_top.empty())
		{
			node->left=new QuatNode;
			build(xmin,ymin,xmax,divigey,vert_top,node->left);
		}

		if(!vert_bottom.empty())
		{
			node->right=new QuatNode;
			build(xmin,divigey,xmax,ymax,vert_bottom,node->right);
		}
	}
}

///////////////////////////MTGVector///////////////////////////////
/*
 Для StreamInterpolator необходимо иметь объекты,
 которые являются живыми в конце текущего логического кванта.
 Поэтому - 
   Удаляются только объекты, помеченные как удалённые в
	 текущем и предыдущем логических квантах.
*/
MTGVector::MTGVector()
{
	MTINIT(critical);
}

MTGVector::~MTGVector()
{
	MTDONE(critical);
}

void MTGVector::Release()
{
	mtUpdate(INT_MAX);
/*  //Ужасный кусок кода. Теоретически может удалиться объект, на который есть ссылка
	int cur_size=slot.size();
	for(int i=0;i<slot.size();i++)
	{
		cIUnkClass* p=slot[i];
		p->Release();
		if(slot.size()!=cur_size)
		{
			cur_size=slot.size();
			i--;
		}
	}
*/
}

void MTGVector::Attach(cIUnkClass *UnkObj)
{
	if(MT_IS_GRAPH())
	{
		slot.push_back(UnkObj);
	}else
	{
		MTEnter mtenter(critical);
		add_list.push_back(UnkObj);
	}
}

void MTGVector::Detach(cIUnkClass *UnkObj)
{
	if(IsNowDelete(UnkObj))
	{
		vector<cIUnkClass*>::iterator it;
		it=find(slot.begin(),slot.end(),UnkObj);
		if(it!=slot.end())
			slot.erase(it);
		else
		{
			int k=0;
		}
	}else
	{
		MTEnter mtenter(critical);
		UnkObj->IncRef();

		int quant=gb_VisGeneric->GetLogicQuant();
		//MT_IS_GRAPH()?gb_VisGeneric->GetGraphLogicQuant():gb_VisGeneric->GetLogicQuant();
		if(erase_list.empty() || erase_list.back().quant!=quant)
		{
			if(!erase_list.empty())
			{
				int back_quant=erase_list.back().quant;
				VISASSERT(back_quant<quant);
			}

			sErase e;
			e.quant=quant;
			erase_list.push_back(e);
		}

		erase_list.back().erase_list.push_back(UnkObj);
	}
}

void MTGVector::mtUpdate(int cur_quant)
{
	MTEnter mtenter(critical);

	list<sErase>::iterator it_erase;

	FOR_EACH(erase_list,it_erase)
	{
		sErase& e=*it_erase;

		if(e.quant<=cur_quant)
		{
			list<cIUnkClass*>::iterator itl;
			for(itl=e.erase_list.begin();itl!=e.erase_list.end();)
			{
				vector<cIUnkClass*>::iterator it_delete;
				it_delete=find(add_list.begin(),add_list.end(),*itl);
				if(it_delete!=add_list.end())
				{
					(*itl)->Release();
					add_list.erase(it_delete);
					itl=e.erase_list.erase(itl);
				}else
				{
					itl++;
				}
			}
		}
	}

	{
		vector<cIUnkClass*>::iterator itl;
		FOR_EACH(add_list,itl)
			slot.push_back(*itl);
	}

	for(it_erase=erase_list.begin();it_erase!=erase_list.end();)
	{
		sErase& e=*it_erase;
		if(e.quant<=cur_quant)
		{
			list<cIUnkClass*>::iterator itl;
			FOR_EACH(e.erase_list,itl)
			{
				vector<cIUnkClass*>::iterator it_delete;
				cIUnkClass* obj=*itl;
				it_delete=find(slot.begin(),slot.end(),obj);
				if(it_delete!=slot.end())
				{
					obj->Release();
					slot.erase(it_delete);
				}else
				{
					VISASSERT(0);
				}
			}

			it_erase=erase_list.erase(it_erase);
		}else
		{
			it_erase++;
		}
	}

	add_list.clear();

	int erase_lists=0;
	int erase_object=0;
	FOR_EACH(erase_list,it_erase)
	{
		sErase& e=*it_erase;
		erase_lists++;
		erase_object+=e.erase_list.size();
	}

	if(cur_quant==INT_MAX)
		VISASSERT(erase_list.empty());
}


void sGrid2d::Detach(cIUnkClass *UnkObj)
{
	if(IsNowDelete(UnkObj))
	{
		if(detach_now)
		{
			MTG();
			vector<cIUnkClass*>::iterator it;
			it=find(slot.begin(),slot.end(),UnkObj);
			if(it!=slot.end())
				slot.erase(it);
			else
			{
				MTEnter mtenter(critical);
				vector<cIUnkClass*>::iterator it_delete;
				it_delete=find(add_list.begin(),add_list.end(),UnkObj);
				if(it_delete!=add_list.end())
				{
					add_list.erase(it_delete);
				}else
				{
					VISASSERT(0);
				}
			}
		}else
		{
			erase_list.push_back(UnkObj);
		}
	}else
	{
		MTGVector::Detach(UnkObj);
	}
}

void sGrid2d::DetachAndIterate(sGrid2d::iterator& it)
{
	int i=it-slot.begin();

	vector<cIUnkClass*>::iterator itl;
	FOR_EACH(erase_list,itl)
	{
		vector<cIUnkClass*>::iterator it_delete;
		it_delete=find(slot.begin(),slot.end(),*itl);
		if(it_delete!=slot.end())
		{
			if(i>=it_delete-slot.begin())
				i--;
			slot.erase(it_delete);
		}
	}

	erase_list.clear();

	it=slot.begin()+(i+1);
}
