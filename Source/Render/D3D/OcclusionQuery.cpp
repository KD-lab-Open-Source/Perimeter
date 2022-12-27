#include "StdAfxRD.h"
#include "OcclusionQuery.h"

cOcclusionQuery::cOcclusionQuery()
{
	draw=false;
	pQuery=NULL;
	Init();
    if (gb_RenderDevice3D) {
        gb_RenderDevice3D->occlusion_query.push_back(this);
    }
}

cOcclusionQuery::~cOcclusionQuery()
{
	RELEASE(pQuery);

	if(gb_RenderDevice3D)
	{
		bool erased=false;
		std::vector<cOcclusionQuery*>::iterator it;
		FOR_EACH(gb_RenderDevice3D->occlusion_query,it)
		if(*it==this)
		{
			gb_RenderDevice3D->occlusion_query.erase(it);
			erased=true;
			break;
		}

		VISASSERT(erased);
	}
}

bool cOcclusionQuery::Init()
{
	RELEASE(pQuery);
	if(!gb_RenderDevice3D || !Option_EnableOcclusion)
		return false;
	HRESULT hr;
	hr=gb_RenderDevice3D->lpD3DDevice->CreateQuery(D3DQUERYTYPE_OCCLUSION,&pQuery);
    if (SUCCEEDED(hr)) {
        return true;
    } else {
        printf("Failed cOcclusionQuery::Init %d\n", pQuery != nullptr);
        pQuery = nullptr;
        //RELEASE(pQuery);
        return false;
    }
}

void cOcclusionQuery::Done()
{
	RELEASE(pQuery);
}

void cOcclusionQuery::Test(const Vect3f& pos)
{
    if (gb_RenderDevice3D == nullptr) {
        return;
    }
    
	gb_RenderDevice3D->SetNoMaterial(ALPHA_BLEND,0,NULL);

	gb_RenderDevice3D->SetRenderState(D3DRS_SRCBLEND,D3DBLEND_ZERO);
	gb_RenderDevice3D->SetRenderState(D3DRS_DESTBLEND,D3DBLEND_ONE);
	gb_RenderDevice3D->SetRenderState(D3DRS_BLENDOP,D3DBLENDOP_ADD);

	cVertexBuffer<sVertexXYZ>& buf=gb_RenderDevice3D->BufferXYZOcclusion;
    pQuery->Issue(D3DISSUE_BEGIN);
	sVertexXYZ* v=(sVertexXYZ*)buf.Lock(1);
	v->pos=pos;
	buf.Unlock(1);
	buf.DrawPrimitive(PT_POINTLIST,1,MatXf::ID);
    pQuery->Issue(D3DISSUE_END);
	draw=true;
}

bool cOcclusionQuery::IsVisible()
{
//	return true;
	uint32_t point=0;
	// Stupidly block until we have a query result
//	pQuery->GetData((void *) &point, sizeof(DWORD), D3DGETDATA_FLUSH);
	if(!draw)
		return false;
/*
	while (pQuery->GetData((void *) &point, sizeof(DWORD), D3DGETDATA_FLUSH) == S_FALSE)
	{
		// asynchronous aspect of GetData() so I can do something
		// here while the HW is still working on my DrawPrimitives()
	}
*/

	for(int i=0;i<10;i++)
	{
		if(pQuery->GetData((void *) &point, sizeof(uint32_t), D3DGETDATA_FLUSH) != S_FALSE)
			break;
		Sleep(1);//FOR Radeon 7500, 8500
	}

	return point!=0;
}
