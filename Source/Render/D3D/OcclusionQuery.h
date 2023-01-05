#pragma once

#ifdef PERIMETER_D3D9_OCCLUSION
class cOcclusionQuery
{
	IDirect3DQuery9* pQuery;
	bool draw;
public:
	cOcclusionQuery();
	~cOcclusionQuery();

	bool Init();
	void Done();

	bool IsInit(){return pQuery!=NULL;}
	void Test(const Vect3f& pos);
	bool IsVisible();
};
#endif
