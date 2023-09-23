#include "StdAfxRD.h"
#include "PoolManager.h"
#include "SafeCast.h"

Pool::Pool() = default;

Pool::~Pool()
{
	delete[] free_pages_list;
	delete parameter;
}

int Pool::AllocPage()
{
	VISASSERT(free_pages);
	if (!free_pages) {
        return -1;
    }
	free_pages--;
	return free_pages_list[free_pages];
}

void Pool::FreePage(int page)
{
	VISASSERT(page >= 0 && page < total_pages && free_pages < total_pages);
	free_pages_list[free_pages] = page;
	free_pages++;
}

void Pool::SetTotal(size_t total_pages_) {
	free_pages = total_pages = total_pages_;
	free_pages_list=new int[total_pages];
	for (int i = 0; i < total_pages; i++) {
        free_pages_list[i] = i;
    }
}


/////////////////////////PoolManager///////////////////////

PoolManager::PoolManager() = default;

PoolManager::~PoolManager() = default;

void PoolManager::CreatePage(PoolPage& page,const PoolParameter& param) {
	int i;
	for (i=0;i<pools.size();i++)
    //Find pool with free pages
	if(pools[i]) {
		if (param==*pools[i]->GetParam() && pools[i]->IsFreePages()) {
            break;
        }
	}

    if (i>=pools.size()) {
        //Find hole in array
        for (i = 0; i < pools.size(); i++) {
            if (pools[i] == NULL) {
                break;
            }
        }
        //We have to add new at end
        if (i>=pools.size()) {
            i = pools.size();
            pools.push_back(nullptr);
        }
        pools[i] = NewPool();
        pools[i]->Create(&param);
    }

	page.pool = i;
	page.page = pools[i]->AllocPage();
}

void PoolManager::DeletePage(PoolPage& page)
{
	CheckGood(page);
	pools[page.pool]->FreePage(page.page);
/*
	if(pools[vp.pool]->IsEmpty() && pools.size() > 8)
	{
		delete pools[page.pool];
		pools[page.pool] = NULL;
	}
*/
	page.pool=-1;
	page.page=-1;
}

void PoolManager::Clear()
{
	for (auto pool : pools) {
        delete pool;
    }
	pools.clear();
}

size_t PoolManager::GetPoolSize(PoolPage& page) {
    CheckGood(page);
    return pools[page.pool]->total_pages * pools[page.pool]->page_size;
}

void PoolManager::GetUsedMemory(size_t& total, size_t& free) {
	total=free=0;

    for (auto pool : pools) {
		size_t cur_total = 0, cur_free = 0;
		pool->GetUsedMemory(cur_total,cur_free);
		total+=cur_total;
		free+=cur_free;
	}
}

////////////////////////////vertex//////////////////////
VertexPool::VertexPool() = default;
VertexPool::~VertexPool() {
    for (auto vb : buffers) {
        delete vb;
    }
}

void VertexPool::Create(const PoolParameter* p) {
	const VertexPoolParameter* vp = safe_cast<const VertexPoolParameter*>(p);
    xassert(parameter == nullptr);
	parameter = new VertexPoolParameter(*vp);

	page_size = vp->number_vertex;
	vertex_format = vp->vertex_format;
	vertex_size = gb_RenderDevice->GetSizeFromFormat(vertex_format);
    SetTotal(MAX_MEMORY_PER_VTX_POOL / page_size);

    for (int i = 0; i < total_pages; ++i) {
        VertexBuffer* vb = new VertexBuffer();
        gb_RenderDevice->CreateVertexBuffer(*vb, page_size, vertex_format, true);
        buffers.emplace_back(vb);
    }
}

VertexBuffer* VertexPoolManager::GetBuffer(const VertexPoolPage& page) {
    CheckGood(page);
    VertexPool* pool = safe_cast<VertexPool*>(pools[page.pool]);
    return pool->buffers[page.page];
}

void* VertexPoolManager::LockPage(const VertexPoolPage& page)
{
	CheckGood(page);
	return pools[page.pool]->LockPage(page.page);
}

void VertexPoolManager::UnlockPage(const VertexPoolPage& page)
{
	CheckGood(page);
	pools[page.pool]->UnlockPage(page.page);
}

bool VertexPoolParameter::operator==(const PoolParameter& p) const {
    const VertexPoolParameter* vp = safe_cast<const VertexPoolParameter*>(&p);
    return number_vertex==vp->number_vertex && vertex_format==vp->vertex_format;
}

void* VertexPool::LockPage(int page) {
	VISASSERT(page >= 0 && page < total_pages);
    return gb_RenderDevice->LockVertexBuffer(*buffers[page]);
}

void VertexPool::UnlockPage(int page) {
	VISASSERT(page >= 0 && page < total_pages);
	gb_RenderDevice->UnlockVertexBuffer(*buffers[page]);
}

void VertexPool::GetUsedMemory(size_t& total,size_t& free) {
	total=page_size*total_pages*vertex_size;
	free=page_size*free_pages*vertex_size;
}

//////////////////////////////index///////////////
IndexPool::IndexPool() = default;
IndexPool::~IndexPool() = default;

void IndexPool::Create(const PoolParameter* p)
{
	const IndexPoolParameter* vp = safe_cast<const IndexPoolParameter*>(p);
	parameter=new IndexPoolParameter(*vp);

	page_size = vp->number_index;
	SetTotal(MAX_MEMORY_PER_IDX_POOL / page_size);

    gb_RenderDevice->CreateIndexBuffer(ib, total_pages * page_size, true);
}

IndexBuffer* IndexPoolManager::GetBuffer(const IndexPoolPage& page) {
    CheckGood(page);
    IndexPool* pool = safe_cast<IndexPool*>(pools[page.pool]);
    return &pool->ib;
}

indices_t* IndexPoolManager::LockPage(const IndexPoolPage& page)
{
	CheckGood(page);
	return static_cast<indices_t*>(pools[page.pool]->LockPage(page.page));
}

void IndexPoolManager::UnlockPage(const IndexPoolPage& page)
{
	CheckGood(page);
	pools[page.pool]->UnlockPage(page.page);
}

bool IndexPoolParameter::operator==(const PoolParameter& p) const {
    const IndexPoolParameter* vp = safe_cast<const IndexPoolParameter*>(&p);
    return number_index==vp->number_index;
}

void* IndexPool::LockPage(int page) {
	VISASSERT(page >= 0 && page < total_pages);
    return gb_RenderDevice->LockIndexBufferRange(ib, page_size * page, page_size);
}

void IndexPool::UnlockPage(int page) {
	VISASSERT(page >= 0 && page < total_pages);
	gb_RenderDevice->UnlockIndexBuffer(ib);
}

void IndexPool::GetUsedMemory(size_t& total, size_t& free)  {
	total=page_size*total_pages*sizeof(indices_t);
	free=page_size*free_pages*sizeof(indices_t);
}

size_t IndexPoolManager::GetBaseIndex(const IndexPoolPage& page) {
    CheckGood(page);
    return pools[page.pool]->page_size * page.page;
}

size_t IndexPoolManager::GetPoolIndices(const IndexPoolPage& page) {
    CheckGood(page);
    Pool* pool = pools[page.pool];
    return pool->page_size * pool->total_pages;
}
