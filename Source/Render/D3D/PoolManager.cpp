#include "StdAfxRD.h"
#include "PoolManager.h"

Pool::Pool()
{
	total_pages=free_pages=0;
	free_pages_list=NULL;
	parameter=NULL;
}

Pool::~Pool()
{
	delete[] free_pages_list;
	delete parameter;
}

int Pool::AllocPage()
{
	VISASSERT(free_pages);
	if (!free_pages) return -1;
	free_pages--;
	return free_pages_list[free_pages];
}

void Pool::FreePage(int page)
{
	VISASSERT(page >= 0 && page < total_pages && free_pages < total_pages);
	free_pages_list[free_pages] = page;
	free_pages++;
}

void Pool::SetTotal(int total_pages_)
{
	free_pages = total_pages = total_pages_;
	free_pages_list=new int[total_pages];
	for (int i = 0; i < total_pages; i++)
		free_pages_list[i] = i;
}


/////////////////////////PoolManager///////////////////////

PoolManager::PoolManager()
{
}

PoolManager::~PoolManager()
{
}

void PoolManager::CreatePage(PoolPage& page,const PoolParameter& param)
{
	int i;
	for(i=0;i<pools.size();i++)
	if(pools[i])
	{
		if(param==*pools[i]->GetParam() && pools[i]->IsFreePages())
			break;
	}

	if(i>=pools.size())
	{
		for (i=0;i<pools.size();i++)
		if(pools[i]==NULL)
				break;
		if (i>=pools.size())
		{
			i = pools.size();
			pools.push_back(NULL);
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
	std::vector<Pool*>::iterator it;
	FOR_EACH(pools,it)
		delete *it;

	pools.clear();
}

void PoolManager::Select(const PoolPage& page)
{
	CheckGood(page);
	pools[page.pool]->Select(page.page);
}

void PoolManager::GetUsedMemory(int& total,int& free)
{
	total=free=0;

	std::vector<Pool*>::iterator it;
	FOR_EACH(pools,it)
	{
		int cur_total,cur_free;
		(*it)->GetUsedMemory(cur_total,cur_free);
		total+=cur_total;
		free+=cur_free;
	}
}

////////////////////////////vertex//////////////////////
VertexPool::VertexPool()
{
	vb=NULL;
	page_size=0;
	vertex_format=0;
	vertex_size=0;
}

VertexPool::~VertexPool()
{
	RELEASE(vb);
}

void VertexPool::Create(const PoolParameter* p)
{
	VertexPoolParameter* vp=(VertexPoolParameter*)p;
	parameter=new VertexPoolParameter(*vp);

	page_size = vp->number_vertex;
	vertex_format = vp->vertex_format;
	vertex_size = gb_RenderDevice3D->GetSizeFromFmt(vertex_format);
	SetTotal(VPOOL_MAX_VTX_PER_POOL / page_size);

	RDCALL(gb_RenderDevice3D->lpD3DDevice->CreateVertexBuffer(
		total_pages * page_size * vertex_size,
		//D3DUSAGE_DYNAMIC | 
		D3DUSAGE_WRITEONLY,
		vertex_format,
		D3DPOOL_DEFAULT,
		&vb,NULL ) );
}

void VertexPool::Select(int page)
{
	RDCALL(gb_RenderDevice3D->lpD3DDevice->SetStreamSource(0, vb, 0, vertex_size));
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

void* VertexPool::LockPage(int page)
{
	void *ptr;

	VISASSERT(page >= 0 && page < total_pages);
	RDCALL(vb->Lock(
		vertex_size * page_size * page, // offset, bytes
		vertex_size * page_size, // size, bytes
		&ptr,
		D3DLOCK_NOSYSLOCK
	));
	return ptr;
}

void VertexPool::UnlockPage(int page)
{
	VISASSERT(page >= 0 && page < total_pages);
	RDCALL(vb->Unlock());
}

void VertexPool::GetUsedMemory(int& total,int& free)
{
	total=page_size*total_pages*vertex_size;
	free=page_size*free_pages*vertex_size;
}


int VertexPoolManager::GetPageSize(const VertexPoolPage& page)
{
	CheckGood(page);
	return ((VertexPool*)pools[page.pool])->GetPageSize();
}


//////////////////////////////index///////////////
IndexPool::IndexPool()
{
	ib=NULL;
	page_size=0;
}

IndexPool::~IndexPool()
{
	RELEASE(ib);
}

void IndexPool::Create(const PoolParameter* p)
{
	IndexPoolParameter* vp=(IndexPoolParameter*)p;
	parameter=new IndexPoolParameter(*vp);

	page_size = vp->number_index;
	SetTotal(VPOOL_MAX_VTX_PER_POOL / page_size);

	RDCALL(gb_RenderDevice3D->lpD3DDevice->CreateIndexBuffer(
		total_pages * page_size * sizeof(int16_t),
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_DEFAULT,
		&ib,NULL
	));
}

void IndexPool::Select(int page)
{
	gb_RenderDevice3D->SetIndices(ib);
}

sPolygon* IndexPoolManager::LockPage(const IndexPoolPage& page)
{
	CheckGood(page);
	return (sPolygon*)pools[page.pool]->LockPage(page.page);
}

void IndexPoolManager::UnlockPage(const IndexPoolPage& page)
{
	CheckGood(page);
	pools[page.pool]->UnlockPage(page.page);
}

void* IndexPool::LockPage(int page)
{
	void *ptr;
	VISASSERT(page >= 0 && page < total_pages);

	RDCALL(ib->Lock(
		sizeof(int16_t)* page_size * page, // offset, bytes
		sizeof(int16_t)* page_size, // size, bytes
		&ptr,
		D3DLOCK_NOSYSLOCK
	));
	return ptr;
}

void IndexPool::UnlockPage(int page)
{
	VISASSERT(page >= 0 && page < total_pages);
	RDCALL(ib->Unlock());
}

void IndexPool::GetUsedMemory(int& total,int& free)
{
	total=page_size*total_pages*sizeof(int16_t);
	free=page_size*free_pages*sizeof(int16_t);
}


int IndexPoolManager::GetPageSize(const IndexPoolPage& page)
{
	CheckGood(page);
	return ((IndexPool*)pools[page.pool])->GetPageSize();
}

int IndexPoolManager::GetBaseIndex(const IndexPoolPage& page)
{
	CheckGood(page);
	return ((IndexPool*)pools[page.pool])->GetPageSize()*page.page;
}

