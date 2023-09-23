#include "StdAfxRD.h"
#ifdef PERIMETER_D3D9
#include "D3DRender.h"
#endif
#include "DrawBuffer.h"
#include "CChaos.h"
#include "VertexFormat.h"

typedef sVertexXYZDT2 VTYPE;

cChaos::cChaos(Vect2f g_size, const char* str_tex0, const char* str_tex1, const char* str_bump, int tile, bool enablebump_)
:cIUnkObj(KIND_NULL)
{
	plane_size=g_size;
	pTex0=NULL;
	time=0;
    db = new DrawBuffer();

#ifdef PERIMETER_D3D9
	enablebump=BUMP_NONE;
	pVS=nullptr;
	pPS=nullptr;

	if(enablebump_ && gb_RenderDevice3D)
	{
		if(gb_RenderDevice3D->DeviceCaps.PixelShaderVersion>= D3DPS_VERSION(2,0))
			enablebump=BUMP_PS14;
		else
		if(gb_RenderDevice3D->DeviceCaps.TextureOpCaps|D3DTEXOPCAPS_BUMPENVMAP)
			enablebump=BUMP_RENDERTARGET;
	}
#endif

	stime_tex0.x=stime_tex0.y=0.005f;
	stime_tex1.x=0;
	stime_tex1.y=-0.03f;
	uvmul=2;

	pTex0=NULL;
	pTexRender=NULL;
	pTexBump=NULL;
#ifdef PERIMETER_D3D9
	if(enablebump==BUMP_PS14)
	{
		pVS=new VSChaos();		pVS->Restore();
		pPS=new PSChaos();
		pPS->Restore();
		pTexBump=GetTexLibrary()->GetElement(str_bump,"NoMipMap NoBlur");
	}

	if(enablebump==BUMP_RENDERTARGET)
	{
		pTex0=gb_VisGeneric->CreateRenderTexture(512,512,TEXTURE_RENDER32,false);
		pTexRender=gb_VisGeneric->CreateRenderTexture(512,512,TEXTURE_RENDER32,false);
		//pTexBump=gb_VisGeneric->CreateTexture(str_bump,"UVBump");
		pTexBump=GetTexLibrary()->GetElement(str_bump,"NoMipMap NoBlur");

		if(pTex0==NULL || pTexRender==NULL)
		{
			enablebump=BUMP_NONE;
			RELEASE(pTex0);
			RELEASE(pTexRender);
		}
	}
#endif

	pTex0_0=gb_VisGeneric->CreateTexture(str_tex0);//"RESOURCE\\EFFECT\\WorldGround.tga");
	pTex0_1=gb_VisGeneric->CreateTexture(str_tex1);//"RESOURCE\\EFFECT\\WorldGround01.tga");


	size=tile*sub_div;
    db->Create((size+1)*(size+1), false, size*size*2*sPolygon::PN, false, VTYPE::fmt, PT_TRIANGLES);
    SetupDB();
}

cChaos::~cChaos()
{
#ifdef PERIMETER_D3D9
	delete pVS;
	delete pPS;
#endif
	db->Destroy();
	RELEASE(pTex0);
	RELEASE(pTexRender);
	RELEASE(pTexBump);

	RELEASE(pTex0_0);
	RELEASE(pTex0_1);
}

void cChaos::Animate(float dt)
{
	time+=dt*1e-3f;
}

void cChaos::RenderAllTexture()
{
#ifdef PERIMETER_D3D9
	if(enablebump==BUMP_RENDERTARGET)
	{
		bool fog=gb_RenderDevice->GetRenderState(RS_FOGENABLE);
        gb_RenderDevice->SetRenderState(RS_FOGENABLE, false);
		int zenable=gb_RenderDevice->GetRenderState(RS_ZENABLE);
		int zwriteenable=gb_RenderDevice->GetRenderState(RS_ZWRITEENABLE); 
		RenderTexture();
		RenderTex0();
		gb_RenderDevice->SetRenderState(RS_FOGENABLE, fog);
		gb_RenderDevice->SetRenderState(RS_ZENABLE, zenable);
		gb_RenderDevice->SetRenderState(RS_ZWRITEENABLE, zwriteenable); 
	}
#endif
}

void cChaos::PreDraw(cCamera *DrawNode)
{
	DrawNode->AttachFirst(this);
}

void cChaos::Draw(cCamera *DrawNode) {
#ifdef PERIMETER_D3D9
    if (gb_RenderDevice->GetRenderSelection() == DEVICE_D3D9) {
        DrawD3D9(DrawNode);
        return;
    }
#endif

    //TODO we need to add bump texture too
    //TODO also implement shader equivalent for shader/Chaos/chaos.vsl to modify UVs at runtime
    gb_RenderDevice->SetNoMaterial(ALPHA_NONE,0,pTex0_0,pTex0_1,COLOR_ADD);
    gb_RenderDevice->SetWorldMatXf(GetGlobalMatrix());
    db->Draw();
}

#ifdef PERIMETER_D3D9
void cChaos::DrawD3D9(cCamera *DrawNode) {
	cD3DRender* rd= dynamic_cast<cD3DRender*>(DrawNode->GetRenderDevice());
	bool fog=rd->GetRenderState(D3DRS_FOGENABLE);
//	rd->SetRenderState(D3DRS_FOGENABLE, FALSE);
/*
		rd->SetRenderState(D3DRS_FOGENABLE, FALSE);
		rd->DrawSprite(0,0,512,512,
						0,0,1,1,pTexBump);
		rd->SetRenderState(D3DRS_FOGENABLE, TRUE);
	if(false)
	{
		rd->SetNoMaterial(ALPHA_NONE,0,pTexBump);
	}else
*/
	if(enablebump==BUMP_PS14)
	{
		for(int ss=0;ss<4;ss++)
		{
			rd->SetSamplerState( ss, D3DSAMP_ADDRESSU , D3DTADDRESS_WRAP);
			rd->SetSamplerState( ss, D3DSAMP_ADDRESSV , D3DTADDRESS_WRAP);

			rd->SetSamplerState( ss, D3DSAMP_MAGFILTER , D3DTEXF_LINEAR);
			rd->SetSamplerState( ss, D3DSAMP_MINFILTER , D3DTEXF_LINEAR);
			rd->SetSamplerState( ss, D3DSAMP_MIPFILTER , D3DTEXF_LINEAR);
		}

		rd->SetNoMaterial(ALPHA_NONE,0,pTex0_0,pTex0_1,COLOR_ADD);
		rd->SetTexture(2, pTexBump,0);
		rd->SetTexture(3, NULL,0);
		float umin,vmin;
		float umin2,vmin2;
		umin=sfmod(time*stime_tex0.x*uvmul*2.0f,1.0f);
		vmin=sfmod(time*stime_tex0.y*uvmul*2.0f,1.0f);
		umin2=sfmod(time*stime_tex1.x*uvmul*0.5f,1.0f);
		vmin2=sfmod(time*stime_tex1.y*uvmul*0.5f,1.0f);

		float umin_b0,vmin_b0,umin_b1,vmin_b1;
		umin_b0=sfmod(time*stime_tex0.x,1.0f);
		vmin_b0=sfmod(time*stime_tex0.y,1.0f);

		umin_b1=sfmod(time*stime_tex1.y*0.2f,1.0f);
		vmin_b1=sfmod(time*stime_tex1.x*0.2f,1.0f);
		pVS->Select(umin,vmin,umin2,vmin2,umin_b0,vmin_b0,umin_b1,vmin_b1);
		pPS->Select();
	}else
	if(enablebump==BUMP_RENDERTARGET)
	{
		//rd->SetDrawTransform(DrawNode);
		//rd->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);

		for(int ss=0;ss<4;ss++)
		{
			rd->SetSamplerState( ss, D3DSAMP_ADDRESSU , D3DTADDRESS_WRAP);
			rd->SetSamplerState( ss, D3DSAMP_ADDRESSV , D3DTADDRESS_WRAP);

			rd->SetSamplerState( ss, D3DSAMP_MAGFILTER , D3DTEXF_LINEAR);
			rd->SetSamplerState( ss, D3DSAMP_MINFILTER , D3DTEXF_LINEAR);
			rd->SetSamplerState( ss, D3DSAMP_MIPFILTER , D3DTEXF_LINEAR);
		}

		if(true)
		{//bump
			rd->SetNoMaterial(ALPHA_NONE,0,pTexRender,pTex0);

			rd->SetTextureStageState( 0, D3DTSS_BUMPENVMAT00, F2DW(0.05f) );
			rd->SetTextureStageState( 0, D3DTSS_BUMPENVMAT01, F2DW(0.00f) );
			rd->SetTextureStageState( 0, D3DTSS_BUMPENVMAT10, F2DW(0.00f) );
			rd->SetTextureStageState( 0, D3DTSS_BUMPENVMAT11, F2DW(0.05f) );

			rd->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);
			rd->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_BUMPENVMAP );
			rd->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			rd->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );
			rd->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

			rd->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1);
			rd->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
			rd->SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			rd->SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );
			rd->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

			rd->SetTextureStageState( 2, D3DTSS_COLOROP,   D3DTOP_DISABLE );
			rd->SetTextureStageState( 2, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
		}else
		{
			rd->SetNoMaterial(ALPHA_NONE,0,pTex0);
//			rd->DrawSprite(0,0,512,512,
//							0,0,1,1,pTexRender);
		}
	}else
	{
		rd->SetNoMaterial(ALPHA_NONE,0,pTex0_0,pTex0_1,COLOR_ADD);

		rd->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);
		rd->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1);
		float umin,vmin;
		Mat4f mat;

		umin=sfmod(time*stime_tex0.x*uvmul,1.0f);
		vmin=sfmod(time*stime_tex0.y*uvmul,1.0f);
		memset(&mat,0,sizeof(mat));
        mat.xx=mat.yy=mat.zz=mat.ww=1;
		mat.zx = umin;
		mat.zy = vmin;
		RDCALL(rd->lpD3DDevice->SetTransform(D3DTS_TEXTURE0, reinterpret_cast<const D3DMATRIX*>(&mat)));
		rd->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

		umin=sfmod(time*stime_tex1.x*uvmul,1.0f);
		vmin=sfmod(time*stime_tex1.y*uvmul,1.0f);
		memset(&mat,0,sizeof(mat));
        mat.xx=mat.yy=mat.zz=mat.ww=1;
		mat.zx = umin;
		mat.zy = vmin;
		RDCALL(rd->lpD3DDevice->SetTransform(D3DTS_TEXTURE1, reinterpret_cast<const D3DMATRIX*>(&mat)));
		rd->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	}

	float woldsize=max(plane_size.x,plane_size.y);
	uint32_t fogcolor=rd->GetRenderState(D3DRS_FOGCOLOR);
	uint32_t fogstart=rd->GetRenderState(D3DRS_FOGSTART);
	uint32_t fogend=rd->GetRenderState(D3DRS_FOGEND);
	uint32_t tablemode=rd->GetRenderState(D3DRS_FOGTABLEMODE);
	uint32_t vertexmode=rd->GetRenderState(D3DRS_FOGVERTEXMODE);

//	rd->SetRenderState(D3DRS_FOGENABLE, TRUE);

//	rd->SetRenderState(D3DRS_FOGCOLOR, 0);
//	rd->SetRenderState(D3DRS_FOGSTART, F2DW(woldsize*0.5f));
//	rd->SetRenderState(D3DRS_FOGEND,   F2DW(woldsize*1.5f));
//	rd->SetRenderState( D3DRS_FOGTABLEMODE,  D3DFOG_NONE ),
//	rd->SetRenderState( D3DRS_FOGVERTEXMODE,  D3DFOG_LINEAR );

    rd->SetWorldMatXf(GetGlobalMatrix());

    db->Draw();

	rd->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
	rd->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
	rd->SetTextureStageState( 3, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	rd->SetTextureStageState( 3, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	rd->SetRenderState(D3DRS_FOGENABLE,fog);
	rd->SetRenderState(D3DRS_FOGCOLOR, fogcolor);
	rd->SetRenderState(D3DRS_FOGSTART, fogstart);
	rd->SetRenderState(D3DRS_FOGEND,   fogend);
	rd->SetRenderState(D3DRS_FOGTABLEMODE,tablemode);
	rd->SetRenderState(D3DRS_FOGVERTEXMODE,vertexmode);

	rd->SetTexture(NULL,0,2);
	rd->SetTexture(NULL,0,3);
	rd->SetVertexShader(NULL);
	rd->SetPixelShader(NULL);
}
#endif

void cChaos::SetupDB() {
    VTYPE* pVertex = nullptr;
    indices_t* p;
    db->LockAll(pVertex, p);

	int smin=-(size/sub_div/2),smax=size/sub_div+smin;
	int xmin=smin*plane_size.x,xmax=smax*plane_size.x,
		ymin=smin*plane_size.y,ymax=smax*plane_size.y;
	float deltax=(xmax-xmin)/(float)size;
	float deltay=(ymax-ymin)/(float)size;
	float du,dv;
	du=dv=1.0f/sub_div;

#ifdef PERIMETER_D3D9
	if(enablebump==BUMP_RENDERTARGET)
	{
		for(int iy=0;iy<=size;iy++)
		{
			VTYPE* vout=pVertex+iy*(size+1);

			for(int ix=0;ix<=size;ix++,vout++)
			{
				vout->pos.x=ix*deltax+xmin;
				vout->pos.y=iy*deltay+ymin;
				vout->pos.z=0;
                vout->diffuse=0xFFFFFFFF;
				vout->u1()=ix*du;
				vout->v1()=iy*dv;
				vout->u2()=ix*du*uvmul;
				vout->v2()=iy*dv*uvmul;
			}
		}
	} else
#endif
	{
		for(int iy=0;iy<=size;iy++)
		{
			VTYPE* vout=pVertex+iy*(size+1);

			for(int ix=0;ix<=size;ix++,vout++)
			{
				vout->pos.x=ix*deltax+xmin;
				vout->pos.y=iy*deltay+ymin;
				vout->pos.z=0;
                vout->diffuse=0xFFFFFFFF;
				vout->u1()=ix*du*uvmul;
				vout->v1()=iy*dv*uvmul;
				vout->u2()=ix*du*uvmul;
				vout->v2()=iy*dv*uvmul;
			}
		}
	}

    int vbwidth=size+1;
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            int i0 = x + y * vbwidth;
            p[0] = i0;
            p[1] = p[4] = i0 + vbwidth;
            p[2] = p[3] = i0 + 1;
            p[5] = i0 + vbwidth + 1;
            p += 6;
        };
    }

    db->Unlock();
}

void cChaos::RenderTexture()
{
#ifdef PERIMETER_D3D9
	cD3DRender* rd=gb_RenderDevice3D;
    if (!rd) return;
	
	rd->SetRenderTarget(pTexRender,NULL);

	float umin,vmin,umin1,vmin1;
	umin=sfmod(time*stime_tex0.x,1.0f);
	vmin=sfmod(time*stime_tex0.y,1.0f);

	umin1=sfmod(time*stime_tex1.x,1.0f);
	vmin1=sfmod(time*stime_tex1.y,1.0f);

	gb_RenderDevice->DrawSprite2(0,0,pTexRender->GetWidth(),pTexRender->GetHeight(),
					umin,vmin,1,1,
					umin1,vmin1,1,1,
					pTexBump,pTexBump,sColor4c(255,255,255,255),0,COLOR_ADD);
	rd->RestoreRenderTarget();
#endif
}

void cChaos::RenderTex0()
{
#ifdef PERIMETER_D3D9
	cD3DRender* rd=gb_RenderDevice3D;
    if (!rd) return;

	rd->SetRenderTarget(pTex0,NULL);

	float umin,vmin,umin1,vmin1;
	umin=1- xm::fmod(time * 0.1f, 1.0f);
	vmin=1- xm::fmod(time * 0.2f, 1.0f);

	umin1= xm::fmod(time * 0.1f, 1.0f);
	vmin1= xm::fmod(time * 0.2f, 1.0f);

    gb_RenderDevice->DrawSprite2(0,0,pTexRender->GetWidth(),pTexRender->GetHeight(),
					umin,vmin,1,1,
					umin1,vmin1,1,1,
					pTex0_0,pTex0_1,sColor4c(255,255,255,255),0,COLOR_ADD);

	rd->RestoreRenderTarget();
#endif
}

///////////////////////////////////////////////
#include "ObjLibrary.h"
#include "ObjMesh.h"
CSkySpere::CSkySpere(cObjLibrary* pObj, const char* str_name, const char* str_texture, int h_size)
:cIUnkObj(KIND_NULL)
{
	pSkySphere=pObj->GetElement(str_name,str_texture);
	pSkySphere->SetAttr(ATTRUNKOBJ_NOLIGHT|ATTRUNKOBJ_REFLECTION);

	sBox6f Box;
	pSkySphere->GetBoundBox(Box);

	MatXf pos=MatXf::ID;
	pos.trans().x=0.5f*(float)h_size;
	pos.trans().y=0.5f*(float)h_size;
	float s=4.0f*h_size/2048;
	if(h_size>2048)
		s*=2048.0f/(Box.max.x*2.2f);
	else
		s*=2048.0f/(Box.max.x*1.8f);

	pSkySphere->SetScale(Vect3f(s,s,s));
	pSkySphere->SetPosition(pos);
	pSkySphere->Update();

}

CSkySpere::~CSkySpere()
{
	RELEASE(pSkySphere);
}

void CSkySpere::Animate(float dt)
{
}

void CSkySpere::PreDraw(cCamera *DrawNode)
{
	DrawNode->AttachNoRecursive(SCENENODE_OBJECTFIRST,this);

	cCamera* pRef=DrawNode->FindCildCamera(ATTRCAMERA_REFLECTION);
	if(pRef)
		//child->AttachNoRecursive(SCENENODE_OBJECTSPECIAL2,this);
		pRef->AttachNoRecursive(SCENENODE_OBJECTFIRST,this);
}

void CSkySpere::Draw(cCamera *DrawNode)
{
	int zfunc=gb_RenderDevice->GetRenderState(RS_ZFUNC);
	int zwriteenable=gb_RenderDevice->GetRenderState(RS_ZWRITEENABLE);
    bool fog=gb_RenderDevice->GetRenderState(RS_FOGENABLE);
	gb_RenderDevice->SetRenderState(RS_FOGENABLE, false);
	gb_RenderDevice->SetRenderState(RS_ZFUNC, CMP_ALWAYS);
	gb_RenderDevice->SetRenderState(RS_ZWRITEENABLE, false);

	Vect2f zplane=DrawNode->GetZPlane();
	DrawNode->SetZPlaneTemp(Vect2f(zplane.x,zplane.y*10));
    gb_RenderDevice->SetDrawNode(DrawNode);

	std::vector<cObjMesh*>& meshes=pSkySphere->GetMeshChild();
	std::vector<cObjMesh*>::iterator it;

	FOR_EACH(meshes,it)
		(*it)->Draw(DrawNode);

	gb_RenderDevice->SetRenderState(RS_FOGENABLE, fog);
	gb_RenderDevice->SetRenderState(RS_ZFUNC, zfunc);
	gb_RenderDevice->SetRenderState(RS_ZWRITEENABLE, zwriteenable);
	DrawNode->SetZPlaneTemp(zplane);
    gb_RenderDevice->SetDrawNode(DrawNode);
}
