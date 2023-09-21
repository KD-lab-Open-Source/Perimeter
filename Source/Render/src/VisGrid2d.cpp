#include <climits>
#include "StdAfxRD.h"
#include "VisGrid2d.h"
#include "SafeCast.h"

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
        int sz = cur.obj.size();
        for (int i = 0; i < sz; i++) {
            cIUnkClass* obj = cur.obj[i];
            Vect3f pos = obj->GetCenterObject();
            if ((add_xmin<=pos.x && pos.x <= add_xmax) &&
                (add_ymin<=pos.y && pos.y <= add_ymax))
            {
                add_proc(obj, add_param);
            }
        }

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
	std::vector<OneObj> tlist(sz);
	std::vector<OneObj*> plist(sz);
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

	int xt=horz_cur.size()+ (std::max)(horz_left.size(),horz_right.size());
	int yt=vert_cur.size()+ (std::max)(vert_top.size(),vert_bottom.size());
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

void MTGVector::Release() {
	mtUpdate(INT_MAX);
}

#ifdef MTGVECTOR_USE_HANDLES
void ReleaseSlot(cUnknownHandle* handle) {
    handle->Release();
}
#endif

void MTGVector::DisableChanges(bool disable) {
    disable_changes = disable;
}

#ifdef PERIMETER_DEBUG_ASSERT
void MTGVector::AssertNoObject(cIUnkClass* object) {
    xxassert(std::count(slot.begin(), slot.end(), object) == 0, "AssertNoObject: slot");
    for (auto l : add_list) {
        if (l.processed) continue;
        xxassert(std::count(l.list.begin(), l.list.end(), object) == 0, "AssertNoObject: add_list");
    }
    for (auto l : erase_list) {
        if (l.processed) continue;
        xxassert(std::count(l.list.begin(), l.list.end(), object) == 0, "AssertNoObject: erase_list");
    }
}

size_t MTGVector::PendingChanges() {
    return add_list.size() + erase_list.size();
}
#endif

bool MTGVector::ChangeNow(cIUnkClass *UnkObj) const {
    if (disable_changes) {
        return false;
    }
    
    if (MT_IS_GRAPH() && MT_IS_LOGIC()) {
        return true;//В случае одного потока - удалять спокойно
    }

    return MT_IS_GRAPH() && !UnkObj->GetAttr(ATTRUNKOBJ_CREATED_IN_LOGIC);
}

void MTGVector::AddToList(std::list<sPerQuant>& list, cIUnkClass* UnkObj) {
    MTEnter mtenter(critical);
    int quant = gb_VisGeneric->GetLogicQuant();
#ifdef MTGVECTOR_USE_HANDLES
    auto obj = UnkObj->AcquireHandle();
#else
    auto obj = UnkObj;
#endif

    //Try to add into existing list
    for (auto& per_quant : list) {
        if (per_quant.processed) continue;
        //Check if we are not in past
        if (per_quant.quant > quant) {
            fprintf(stderr, "Whoops we have in AddToList for quant %d but we are in %d!\n", per_quant.quant, quant);
            xassert(0);
            per_quant.processed = true;
        }
        if (per_quant.quant == quant) {
            per_quant.list.push_back(obj);
            return;
        }
    }

    //Not found any list, create new one and add it
    sPerQuant per_quant = {};
    per_quant.processed = false;
    per_quant.quant = quant;
    per_quant.list.push_back(obj);
    list.push_back(per_quant);
}

void MTGVector::RemoveFromList(std::list<sPerQuant>& list, cIUnkClass* UnkObj) {
    MTEnter mtenter(critical);
#ifdef MTGVECTOR_USE_HANDLES
    auto obj = UnkObj->AcquireHandle();
#else
    auto obj = UnkObj;
#endif

    //Remove from any per quant
    for (auto& per_quant : list) {
        auto& pl = per_quant.list;
        if (0 < std::count(pl.begin(), pl.end(), obj)) {
            auto removed = std::remove(pl.begin(), pl.end(), obj);
            pl.erase(removed, pl.end());
        }
    }
}

void MTGVector::Attach(cIUnkClass *UnkObj) {
	if (ChangeNow(UnkObj)) {
#ifdef MTGVECTOR_USE_HANDLES
		slot.push_back(UnkObj->AcquireHandle());
#else
        if (std::count(slot.begin(), slot.end(), UnkObj) == 0) {
            slot.push_back(UnkObj);
        }
#endif
	} else {
        AddToList(add_list, UnkObj);
	}
}

void MTGVector::Detach(cIUnkClass *UnkObj)
{
    RemoveFromList(add_list, UnkObj);
	if (ChangeNow(UnkObj)) {
        auto deleted = std::remove_if(slot.begin(), slot.end(), [UnkObj] ( auto& slot_el ) {
#ifdef MTGVECTOR_USE_HANDLES
            auto ptr = slot_el->Get();
            if (ptr == nullptr || ptr == UnkObj) {
                ReleaseSlot(slot_el);
                return true;
            }
            return false;
#endif
            return slot_el == UnkObj;
        });
        slot.erase(deleted, slot.end());
	} else {
#ifdef MTGVECTOR_USE_HANDLES
        MTEnter mtenter(critical);
        if (std::count_if(objects_hold.begin(), objects_hold.end(), [UnkObj] (auto& el) {
            return el->Get() == UnkObj;
        }) == 0) {
            UnkObj->IncRef()
            objects_hold.push_back(UnkObj->AcquireHandle());
        }
#else
        UnkObj->IncRef();
#endif
        AddToList(erase_list, UnkObj);
	}
}

void MTGVector::mtUpdate(int cur_quant) {
    MTG();
	MTEnter mtenter(critical);

    auto add_list_removed = std::remove_if(
            add_list.begin(),
            add_list.end(),
            [this, cur_quant] (auto& per_quant) {
        if (per_quant.processed) {
            xxassert(0, "mtUpdate: Got processed quant in add_list");
            return true;
        }
        if (per_quant.quant > cur_quant) {
            return false;
        }
        per_quant.processed = true;
        //Transfer to slot list or release
        for (auto pending : per_quant.list) {
            if (std::count(slot.begin(), slot.end(), pending) == 0
#ifdef MTGVECTOR_USE_HANDLES
            && pending->Get()
#endif
            ) {
                slot.push_back(pending);
            } else {
                pending->Release();
            }
        }
        return true;
    });
    add_list.erase(add_list_removed, add_list.end());

    auto erase_list_removed = std::remove_if(
            erase_list.begin(),
            erase_list.end(),
            [this, cur_quant] (auto& per_quant) {
        if (per_quant.processed) {
            xxassert(0, "mtUpdate: Got processed quant in erase_list");
            return true;
        }
        if (per_quant.quant > cur_quant) {
            return false;
        }
        per_quant.processed = true;
        //Remove from slot list and hold list
        for (auto pending : per_quant.list) {
#ifdef MTGVECTOR_USE_HANDLES
            cIUnkClass* obj = safe_cast<cIUnkClass*>(pending->Get());
            auto slot_removed = std::remove_if(slot.begin(), slot.end(), [pending, obj] ( auto& handle ) {
                cUnknownClass* ptr = handle->Get();
                if (handle == pending || ptr == obj || ptr == nullptr) {
                    ReleaseSlot(handle);
                    return true;
                }
                return false;
            });
            slot.erase(slot_removed, slot.end());
            auto objects_hold_removed = std::remove_if(objects_hold.begin(), objects_hold.end(), [pending, obj] ( auto& handle ) {
                cUnknownClass* ptr = handle->Get();
                if (handle == pending || ptr == obj || ptr == nullptr) {
                    if (ptr) {
                        ptr->Release();
                    }
                    handle->Release();
                    return true;
                }
                return false;
            });
            objects_hold.erase(objects_hold_removed, objects_hold.end());
#else
            auto slot_removed = std::remove_if(slot.begin(), slot.end(), [pending] ( auto& obj ) {
                if (obj == pending) {
                    return true;
                }
                return false;
            });
            slot.erase(slot_removed, slot.end());
#endif
            pending->Release();
        }
        return true;
    });
    erase_list.erase(erase_list_removed, erase_list.end());

#ifdef MTGVECTOR_USE_HANDLES
    //Remove stale handles
    auto deleted = std::remove_if(slot.begin(), slot.end(), [] ( cUnknownHandle* handle ) {
        if (handle->Get() == nullptr) {
            ReleaseSlot(handle);
            return true;
        }
        return false;
    });
    slot.erase(deleted, slot.end());
#endif
    
    if (cur_quant == INT_MAX) {
        VISASSERT(add_list.empty());
        VISASSERT(erase_list.empty());
#ifdef MTGVECTOR_USE_HANDLES
        for (auto ptr : slot) {
            ReleaseSlot(ptr);
        }
#endif
    }
}
