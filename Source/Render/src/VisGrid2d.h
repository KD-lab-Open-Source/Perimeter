#pragma once

//#define MTGVECTOR_USE_HANDLES

class MTGVector
{
public:
#ifdef MTGVECTOR_USE_HANDLES
    using ptr_t = cUnknownHandle*;
#else
    using ptr_t = cIUnkClass*;
#endif
    
protected:
    bool disable_changes = false;
    std::vector<ptr_t> slot = {};
    
    struct sPerQuant {
        bool processed = false;
        int quant = 0;
        std::vector<ptr_t> list = {};
    };
    std::list<sPerQuant> add_list;
    std::list<sPerQuant> erase_list;

#ifdef MTGVECTOR_USE_HANDLES
    //A separate list of handles of objects we are holding alive for rendering
    std::vector<ptr_t> objects_hold;
#endif

    bool ChangeNow(cIUnkClass* UnkObj) const;
    void AddToList(std::list<sPerQuant>& list, cIUnkClass* UnkObj);
    void RemoveFromList(std::list<sPerQuant>& list, cIUnkClass* UnkObj);

	MTDECLARE(critical);
public:
    typedef std::vector<ptr_t>::const_iterator const_iterator;
    inline ptr_t get(size_t i) const { return slot[i]; }
	inline int size() { return slot.size(); }
    const_iterator begin() const noexcept { return slot.cbegin(); }
    const_iterator end() const noexcept { return slot.cend(); }

	MTGVector();
	~MTGVector();

	//Attach, Detach - можно вызывать из другого потока
	void Attach(cIUnkClass *UnkObj);
    void Detach(cIUnkClass *UnkObj);
	void Release();

	//очищает add_list и erase_list в безопасный момент
	//cur_quant == INT_MAX очистить всё
	void mtUpdate(int cur_quant);
    
    void DisableChanges(bool disable);
#ifdef PERIMETER_DEBUG_ASSERT
    void AssertNoObject(cIUnkClass* object);
    size_t PendingChanges();
#endif
};

class QuatTree
{
public:
	typedef std::vector<cIUnkClass*> vect;
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

	typedef std::vector<OneObj*> pvect;
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
