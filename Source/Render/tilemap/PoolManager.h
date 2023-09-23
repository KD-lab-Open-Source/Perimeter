#pragma once

const size_t MAX_MEMORY_PER_VTX_POOL = 64 * 1024;
const size_t MAX_MEMORY_PER_IDX_POOL = 32 * 1024;

struct PoolParameter
{
	virtual bool operator==(const PoolParameter& p) const=0;
	virtual ~PoolParameter() = default;
};

struct PoolPage
{
	int pool;
	int page;

	PoolPage(){pool=page=-1;}

	inline int IsInit() { return pool >= 0; }
};

class Pool
{
protected:
    size_t total_pages = 0;
    size_t free_pages = 0;
    size_t page_size = 0;
	int *free_pages_list = nullptr;
	PoolParameter* parameter = nullptr;
    friend class PoolManager;
    friend class IndexPoolManager;
public:
	Pool();
	virtual ~Pool();

	int AllocPage();
	void FreePage(int page);
    bool IsEmpty() const { return total_pages==free_pages; };
	bool IsFreePages() const { return free_pages>0; }

	PoolParameter* GetParam() const { return parameter; }

	virtual void Create(const PoolParameter* p)=0;
	virtual void* LockPage(int page)=0;
	virtual void UnlockPage(int page)=0;

	virtual void GetUsedMemory(size_t& total, size_t& free)=0;
protected:
	void SetTotal(size_t total_pages);
};

class PoolManager
{
public:
	PoolManager();
	virtual ~PoolManager();

	void CreatePage(PoolPage& new_page,const PoolParameter& param);
	void DeletePage(PoolPage& page);
	void Clear();
    size_t GetPoolSize(PoolPage& page);

	void GetUsedMemory(size_t& total, size_t& free);
protected:
	std::vector<Pool*> pools;
	virtual Pool* NewPool()=0;

#ifdef PERIMETER_DEBUG_ASSERT
	inline void CheckGood(const PoolPage& page)
	{
		VISASSERT(page.pool >=0 && page.pool < pools.size());
		VISASSERT(pools[page.pool]);
		VISASSERT(page.page >= 0 && page.page < pools[page.pool]->total_pages);
	}
#endif
};

#ifndef PERIMETER_DEBUG_ASSERT
#define CheckGood(X)
#endif

/////////////////////////vertex/////////////////////
struct VertexPoolParameter:public PoolParameter
{
	int number_vertex = 0;
    vertex_fmt_t vertex_format = 0;

	VertexPoolParameter(const VertexPoolParameter& p)
		:number_vertex(p.number_vertex), vertex_format(p.vertex_format) {}

	VertexPoolParameter(int number_vertex_, vertex_fmt_t vertex_format_)
		:number_vertex(number_vertex_), vertex_format(vertex_format_) {}

	bool operator==(const PoolParameter& p) const override;
};

struct VertexPoolPage:public PoolPage
{
};

class VertexPool:public Pool
{
protected:
    std::vector<VertexBuffer*> buffers;
    vertex_fmt_t vertex_format = 0;
    size_t vertex_size = 0;
    friend class VertexPoolManager;
public:
	VertexPool();
	~VertexPool() override;

	void Create(const PoolParameter* p) override;

	void* LockPage(int page) override;
	void UnlockPage(int page) override;

	void GetUsedMemory(size_t& total,size_t& free) override;
};

class VertexPoolManager:public PoolManager
{
protected:
    Pool* NewPool() override { return new VertexPool(); }
public:
    VertexBuffer* GetBuffer(const VertexPoolPage& page);
	void* LockPage(const VertexPoolPage& page);
	void UnlockPage(const VertexPoolPage& page);
};


////////////////////////////index/////////////////////////////////
struct IndexPoolParameter:public PoolParameter
{
	int number_index;

	IndexPoolParameter(const IndexPoolParameter& p)
		:number_index(p.number_index) {}

	explicit IndexPoolParameter(int number_index_)
		:number_index(number_index_) {}

	bool operator==(const PoolParameter& p) const override;
};

struct IndexPoolPage:public PoolPage
{
};

class IndexPool:public Pool
{
protected:
    IndexBuffer ib;
    friend class IndexPoolManager;
public:
	IndexPool();
	~IndexPool() override;

	void Create(const PoolParameter* p) override;

	void* LockPage(int page) override;
	void UnlockPage(int page) override;

	void GetUsedMemory(size_t& total, size_t& free) override;
};

class IndexPoolManager:public PoolManager
{
protected:
    Pool* NewPool() override { return new IndexPool(); }
public:
	void CreatePage(PoolPage& new_page,int num_index)
	{
		PoolManager::CreatePage(new_page,IndexPoolParameter(num_index));
	}

    IndexBuffer* GetBuffer(const IndexPoolPage& page);
    indices_t* LockPage(const IndexPoolPage& page);
	void UnlockPage(const IndexPoolPage& page);

    size_t GetBaseIndex(const IndexPoolPage& page);
    size_t GetPoolIndices(const IndexPoolPage& page);
};
