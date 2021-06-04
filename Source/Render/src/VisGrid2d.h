#pragma once

class MTGVector
{
protected:
	vector<cIUnkClass*> slot;
	vector<cIUnkClass*> add_list;

	struct sErase
	{
		int quant;
		list<cIUnkClass*> erase_list;
	};

	list<sErase> erase_list;
	MTDECLARE(critical);
public:
	typedef vector<cIUnkClass*>::iterator iterator;
	inline int size()	{ return slot.size(); }
	cIUnkClass* operator[](int i){return slot[i];}
	iterator begin(){return slot.begin();}
	iterator end(){return slot.end();}

	MTGVector();
	~MTGVector();

	//Attach, Detach - можно вызывать из другого потока
	void Attach(cIUnkClass *UnkObj);
	void Detach(cIUnkClass *UnkObj);
	void Release();

	//очищает add_list и erase_list в безопасный момент
	//cur_quant == INT_MAX очистить всё
	void mtUpdate(int cur_quant);
};

struct sGrid2d:public MTGVector
{
protected:
	bool detach_now;
	vector<cIUnkClass*> erase_list;
public:
	
	sGrid2d():detach_now(true) { }
	~sGrid2d() { }

	void Detach(cIUnkClass *UnkObj);
	void SetDetachNow(bool now){detach_now=now;}
	void DetachAndIterate(sGrid2d::iterator& it);
	bool IsLifeObject(){return erase_list.empty();}
};

class QuatTree
{
public:
	typedef vector<cIUnkClass*> vect;
	typedef void (*find_proc)(cIUnkClass* obj,void* param);
protected:
	struct QuatNode
	{
		int xmin,ymin,xmax,ymax;
		vect obj;
		QuatNode *left,*right;

		QuatNode()
		{
			left=right=NULL;
		}

		~QuatNode()
		{
			delete left;
			delete right;
		}
	};

	QuatNode* root;

	int add_xmin,add_ymin,add_xmax,add_ymax;
	find_proc add_proc;
	void* add_param;
	void add(QuatNode& cur);

	struct OneObj
	{
		int xmin,ymin,xmax,ymax;
		cIUnkClass* obj;
	};

	typedef vector<OneObj*> pvect;
	void build(int xmin,int ymin,int xmax,int ymax,pvect& blist,QuatNode* cur);

public:

	QuatTree();
	~QuatTree();

	void find(Vect2i pos,int radius,find_proc proc,void* param);
	void find(int xmin,int ymin,int xmax,int ymax,find_proc proc,void* param);

	void build(int xmin,int ymin,int xmax,int ymax,vect& blist);
	void clear();
protected:
	void GetBorder(cIUnkClass* p,int& xmin,int& ymin,int& xmax,int& ymax);
};
