#include "StdAfxRD.h"
#include "CChaos.h"

cChaos::cChaos(Vect2f g_size,LPCSTR str_tex0,LPCSTR str_tex1,LPCSTR str_bump,int tile,bool enablebump_)
:cIUnkObj(KIND_NULL)
{
	plane_size=g_size;
	pTex0=NULL;
	time=0;

	enablebump=BUMP_NONE;
	pVS=NULL;
	pPS=NULL;

	if(enablebump_)
	{
		if(gb_RenderDevice3D->DeviceCaps.PixelShaderVersion>= D3DPS_VERSION(2,0))
			enablebump=BUMP_PS14;
		else
		if(gb_RenderDevice3D->DeviceCaps.TextureOpCaps|D3DTEXOPCAPS_BUMPENVMAP)
			enablebump=BUMP_RENDERTARGET;
	}

	stime_tex0.x=stime_tex0.y=0.005f;
	stime_tex1.x=0;
	stime_tex1.y=-0.03f;
	uvmul=2;

	pTex0=NULL;
	pTexRender=NULL;
	pTexBump=NULL;
	if(enablebump==BUMP_PS14)
	{
		pVS=new VSChaos;
		pVS->Restore();
		pPS=new PSChaos;
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


	pTex0_0=gb_VisGeneric->CreateTexture(str_tex0);//"RESOURCE\\EFFECT\\WorldGround.tga");
	pTex0_1=gb_VisGeneric->CreateTexture(str_tex1);//"RESOURCE\\EFFECT\\WorldGround01.tga");


	size=tile*sub_div;
	CreateVB();
	CreateIB();
}

cChaos::~cChaos()
{
	delete pVS;
	delete pPS;
	gb_RenderDevice3D->DeleteIndexBuffer(ib);
	gb_RenderDevice3D->DeleteVertexBuffer(vb);
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
	if(enablebump==BUMP_RENDERTARGET)
	{
		cD3DRender* rd=gb_RenderDevice3D;

		BOOL fog=rd->GetRenderState(D3DRS_FOGENABLE);
		rd->SetRenderState(D3DRS_FOGENABLE, FALSE);
		DWORD zenable=rd->GetRenderState(D3DRS_ZENABLE);
		DWORD zwriteenable=rd->GetRenderState(D3DRS_ZWRITEENABLE); 
		RenderTexture();
		RenderTex0();
		rd->SetRenderState(D3DRS_FOGENABLE, fog);
		rd->SetRenderState( D3DRS_ZENABLE, zenable);
		rd->SetRenderState( D3DRS_ZWRITEENABLE, zwriteenable); 
	}
}

void cChaos::PreDraw(cCamera *DrawNode)
{
	DrawNode->AttachFirst(this);
}

void cChaos::Draw(cCamera *DrawNode)
{
	cD3DRender* rd=DrawNode->GetRenderDevice3D();
	BOOL fog=rd->GetRenderState(D3DRS_FOGENABLE);
//	rd->SetRenderState(D3DRS_FOGENABLE, FALSE);
/*
		rd->SetRenderState(D3DRS_FOGENABLE, FALSE);
		rd->DrawSprite(0,0,512,512,
						0,0,1,1,pTexBump);
		rd->SetRenderState(D3DRS_FOGENABLE, TRUE);
*/
	if(false)
	{
		rd->SetNoMaterial(ALPHA_NONE,0,pTexBump);
	}else
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
		rd->SetTexture(pTexBump,0,2);
		rd->SetTexture(NULL,0,3);
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
		D3DXMATRIX mat;

		umin=sfmod(time*stime_tex0.x*uvmul,1.0f);
		vmin=sfmod(time*stime_tex0.y*uvmul,1.0f);
		memset(mat,0,sizeof(mat));
		mat._11=mat._22=mat._33=mat._44=1;
		mat._31 = umin;
		mat._32 = vmin;
		RDCALL(rd->lpD3DDevice->SetTransform(D3DTS_TEXTURE0,&mat));
		rd->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

		umin=sfmod(time*stime_tex1.x*uvmul,1.0f);
		vmin=sfmod(time*stime_tex1.y*uvmul,1.0f);
		memset(mat,0,sizeof(mat));
		mat._11=mat._22=mat._33=mat._44=1;
		mat._31 = umin;
		mat._32 = vmin;
		RDCALL(rd->lpD3DDevice->SetTransform(D3DTS_TEXTURE1,&mat));
		rd->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);

	}

	float woldsize=max(plane_size.x,plane_size.y);
	DWORD fogcolor=rd->GetRenderState(D3DRS_FOGCOLOR);
	DWORD fogstart=rd->GetRenderState(D3DRS_FOGSTART);
	DWORD fogend=rd->GetRenderState(D3DRS_FOGEND);
	DWORD tablemode=rd->GetRenderState(D3DRS_FOGTABLEMODE);
	DWORD vertexmode=rd->GetRenderState( D3DRS_FOGVERTEXMODE);

//	rd->SetRenderState(D3DRS_FOGENABLE, TRUE);

//	rd->SetRenderState(D3DRS_FOGCOLOR, 0);
//	rd->SetRenderState(D3DRS_FOGSTART, F2DW(woldsize*0.5f));
//	rd->SetRenderState(D3DRS_FOGEND,   F2DW(woldsize*1.5f));
//	rd->SetRenderState( D3DRS_FOGTABLEMODE,  D3DFOG_NONE ),
//	rd->SetRenderState( D3DRS_FOGVERTEXMODE,  D3DFOG_LINEAR );

	rd->SetMatrix(D3DTS_WORLD,GetGlobalMatrix());

	rd->DrawIndexedPrimitive(vb,0,(size+1)*(size+1), ib,0,size*size*2);

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

void cChaos::CreateIB()
{
	gb_RenderDevice3D->CreateIndexBuffer(ib,size*size*2);
	sPolygon* p=gb_RenderDevice3D->LockIndexBuffer(ib);

	int vbwidth=size+1;
	for(int y=0;y<size;y++)
	for(int x=0;x<size;x++)
	{
		int i0=x+y*vbwidth;
		p->p1=i0;
		p->p2=i0+vbwidth;
		p->p3=i0+1;
		p++;
		p->p1=i0+1;
		p->p2=i0+vbwidth;
		p->p3=i0+vbwidth+1;
		p++;
	}
	
	gb_RenderDevice3D->UnlockIndexBuffer(ib);
}

void cChaos::CreateVB()
{
	gb_RenderDevice3D->CreateVertexBuffer(vb,(size+1)*(size+1),VTYPE::fmt,false);

	VTYPE* pVertex=(VTYPE*)gb_RenderDevice3D->LockVertexBuffer(vb);
	xassert(sizeof(VTYPE)==vb.size);

	int smin=-(size/sub_div/2),smax=size/sub_div+smin;
	int xmin=smin*plane_size.x,xmax=smax*plane_size.x,
		ymin=smin*plane_size.y,ymax=smax*plane_size.y;
	float deltax=(xmax-xmin)/(float)size;
	float deltay=(ymax-ymin)/(float)size;
	float du,dv;
	du=dv=1.0f/sub_div;

	Vect3f n(0,0,1),t(1,0,0),b;
	b.cross(n,t);

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
				vout->diffuse.RGBA()=0xFFFFFFFF;
				vout->u1()=ix*du;
				vout->v1()=iy*dv;
				vout->u2()=ix*du*uvmul;
				vout->v2()=iy*dv*uvmul;
			}
		}
	}else
	{
		for(int iy=0;iy<=size;iy++)
		{
			VTYPE* vout=pVertex+iy*(size+1);

			for(int ix=0;ix<=size;ix++,vout++)
			{
				vout->pos.x=ix*deltax+xmin;
				vout->pos.y=iy*deltay+ymin;
				vout->pos.z=0;
				vout->diffuse.RGBA()=0xFFFFFFFF;
				vout->u1()=ix*du*uvmul;
				vout->v1()=iy*dv*uvmul;
				vout->u2()=ix*du*uvmul;
				vout->v2()=iy*dv*uvmul;
			}
		}
	}
	
	gb_RenderDevice3D->UnlockVertexBuffer(vb);
}

void cChaos::InitBumpTexture1(cTexture* pTex)
{
	int Pitch;
	int dx=pTex->GetWidth(),dy=pTex->GetHeight();
	BYTE* pBuffer=(BYTE*)pTex->LockTexture(Pitch);

	float cx=64.0f/dx,cy=64.0f/dy;
	float cx1=60.0f/dx,cy1=60.0f/dy;

	struct VU
	{
		BYTE v,u;
	};

	VISASSERT(sizeof(VU)==2);

	for(int iy=0;iy<dy;iy++)
	{
		VU* p=(VU*)pBuffer;

		for(int ix=0;ix<dx;ix++,p++)
		{
			float du,dv;
			du=0;//cos((ix+iy)*cx*M_PI)+cos((ix-iy)*cx1*M_PI);
			dv=0;//sin((ix+iy)*cy*M_PI)+sin((ix-iy)*cy1*M_PI);
			
			p->u=round(du*16);
			p->v=round(dv*16);
		}

		pBuffer+=Pitch;
	}

	pTex->UnlockTexture();
}

void cChaos::InitBumpTexture2(cTexture* pTex)
{
	int Pitch;
	int dx=pTex->GetWidth(),dy=pTex->GetHeight();
	BYTE* pBuffer=pTex->LockTexture(Pitch);

	float cx=32.0f/dx,cy=32.0f/dy;

	struct VU
	{
		char v,u;
	};

	VISASSERT(sizeof(VU)==2);

	for(int iy=0;iy<dy;iy++)
	{
		VU* p=(VU*)pBuffer;

		for(int ix=0;ix<dx;ix++,p++)
		{
			float du,dv;
			du=cos(iy*cx*M_PI)*0.9f;
			dv=sin(iy*cy*M_PI)*0.9f;
			
			p->u=round((du+1)*64);
			p->v=round((dv+1)*64);
		}

		pBuffer+=Pitch;
	}

	pTex->UnlockTexture();
}

void cChaos::ConvertToBump(cTexture*& pOut,cTexture* pIn)
{
	int dx=pIn->GetWidth(),dy=pIn->GetHeight();
	pOut=gb_VisGeneric->CreateBumpTexture(dx,dy);

	int PitchIn,PitchOut;
	BYTE* pBufferIn=pIn->LockTexture(PitchIn);
	BYTE* pBufferOut=pOut->LockTexture(PitchOut);

	for(int iy=0;iy<dy;iy++)
	{
		BYTE* pi=pBufferIn;
		BYTE* po=pBufferOut;

		for(int ix=0;ix<dx;ix++)
		{
			po[0]=pi[0]>>1;
			po[1]=pi[1]>>1;
			pi+=4;
			po+=2;
		}

		pBufferIn+=PitchIn;
		pBufferOut+=PitchOut;
	}

	pIn->UnlockTexture();
	pOut->UnlockTexture();
}

void cChaos::RenderTexture()
{
	cD3DRender* rd=gb_RenderDevice3D;
	
	rd->SetRenderTarget(pTexRender,NULL);

	float umin,vmin,umin1,vmin1;
	umin=sfmod(time*stime_tex0.x,1.0f);
	vmin=sfmod(time*stime_tex0.y,1.0f);

	umin1=sfmod(time*stime_tex1.x,1.0f);
	vmin1=sfmod(time*stime_tex1.y,1.0f);

	rd->DrawSprite2(0,0,pTexRender->GetWidth(),pTexRender->GetHeight(),
					umin,vmin,1,1,
					umin1,vmin1,1,1,
					pTexBump,pTexBump,sColor4c(255,255,255,255),0,COLOR_ADD);
	rd->RestoreRenderTarget();
}

void cChaos::RenderTex0()
{
	cD3DRender* rd=gb_RenderDevice3D;

	rd->SetRenderTarget(pTex0,NULL);

	float umin,vmin,umin1,vmin1;
	umin=1-fmodf(time*0.1f,1.0f);
	vmin=1-fmodf(time*0.2f,1.0f);

	umin1=fmodf(time*0.1f,1.0f);
	vmin1=fmodf(time*0.2f,1.0f);
	
	rd->DrawSprite2(0,0,pTexRender->GetWidth(),pTexRender->GetHeight(),
					umin,vmin,1,1,
					umin1,vmin1,1,1,
					pTex0_0,pTex0_1,sColor4c(255,255,255,255),0,COLOR_ADD);

	rd->RestoreRenderTarget();
}


//////////////////////
int CBox::CubeVector::fmt=D3DFVF_XYZ|D3DFVF_TEX1;

CBox::CBox(Vect3f size,LPCSTR str_cube)
:cIUnkObj(KIND_NULL)
{
	sz_rect=size;

	fov90=false;

	box_face=6;
	char* name[]=
	{
		"X0.tga",
		"X1.tga",
		"Y0.tga",
		"Y1.tga",
		"Z0.tga",
		"Z1.tga",
	};

	pBox.resize(box_face);
	int i;
	for(i=0;i<box_face;i++)
	{
		enable_face[i]=true;
		pBox[i]=NULL;
	}
	enable_face[4]=false;

	string path;
	for(i=0;i<box_face;i++)
	if(enable_face[i])
	{
		path=str_cube;path+=name[i];
		cTexture* pTex=gb_VisGeneric->CreateTexture(path.c_str());
		pBox[i]=pTex;
	}

	CreateVB();
}

CBox::~CBox()
{
	vector<cTexture*>::iterator it;
	FOR_EACH(pBox,it)
	{
		cTexture* p=*it;
		if(p)p->Release();
	}

	gb_RenderDevice3D->DeleteIndexBuffer(ib);
	gb_RenderDevice3D->DeleteVertexBuffer(vb);

}

void CBox::Animate(float dt)
{
}

void CBox::PreDraw(cCamera *DrawNode)
{
	DrawNode->AttachNoRecursive(SCENENODE_OBJECTFIRST,this);
	cCamera* pRef=DrawNode->FindCildCamera(ATTRCAMERA_REFLECTION);
	if(pRef)
		pRef->AttachNoRecursive(SCENENODE_OBJECTSPECIAL2,this);

}

void CBox::Draw(cCamera *DrawNode)
{
	cD3DRender* rd=DrawNode->GetRenderDevice3D();
	bool reflection=DrawNode->GetAttribute(ATTRCAMERA_REFLECTION);
//	rd->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);
//	rd->SetRenderState( RS_CULLMODE, D3DCULL_NONE );
	
    rd->SetSamplerState( 0, D3DSAMP_ADDRESSU,  D3DTADDRESS_MIRROR );
    rd->SetSamplerState( 0, D3DSAMP_ADDRESSV,  D3DTADDRESS_MIRROR );

	D3DXMATRIX matViewSave, matProjSave;
	if(fov90)
	{
		rd->lpD3DDevice->GetTransform( D3DTS_VIEW,       &matViewSave );
		rd->lpD3DDevice->GetTransform( D3DTS_PROJECTION, &matProjSave );
		D3DXMATRIX matView = matViewSave;
		D3DXMATRIX matProj = matViewSave;
		matView._41 = matView._42 = matView._43 = 0.0f;
		D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/2, 1.0f, 0.5f, 10000.0f );
		rd->lpD3DDevice->SetTransform( D3DTS_PROJECTION, &matProj );
		rd->lpD3DDevice->SetTransform( D3DTS_VIEW,       &matView );

		D3DXMATRIX matWorld;
		D3DXMatrixIdentity( &matWorld );
		rd->lpD3DDevice->SetTransform( D3DTS_WORLD, &matWorld );
	}else
	{
		rd->SetMatrix(D3DTS_WORLD,GetGlobalMatrix());
	}

	rd->SetFVF(CubeVector::fmt);

	DWORD zfunc=rd->GetRenderState(D3DRS_ZFUNC);
	if(!reflection)
		rd->SetRenderState(D3DRS_ZFUNC,D3DCMP_ALWAYS);

	for(int i=0;i<box_face;i++)
	if(enable_face[i])
	{
		rd->SetNoMaterial(ALPHA_NONE,0,pBox[i]);
		rd->DrawIndexedPrimitive(vb,0,num_vertex, ib,i*2,2);
	}

	rd->SetRenderState(D3DRS_ZFUNC,zfunc);
	rd->SetSamplerState( 0, D3DSAMP_ADDRESSU , D3DTADDRESS_WRAP);
	rd->SetSamplerState( 0, D3DSAMP_ADDRESSV , D3DTADDRESS_WRAP);

	if(fov90)
	{
		rd->lpD3DDevice->SetTransform( D3DTS_VIEW,       &matViewSave );
		rd->lpD3DDevice->SetTransform( D3DTS_PROJECTION, &matProjSave );
	}
}

void CBox::CreateVB()
{
	num_vertex=box_face*4;
	num_index=box_face*6;
	{
		gb_RenderDevice3D->CreateIndexBuffer(ib,num_index);
		sPolygon* p=gb_RenderDevice3D->LockIndexBuffer(ib);

		if(fov90)
		{
			for(int i=0;i<box_face;i++)
			{
				int i0=i*4;
				p->p1=i0+0;
				p->p2=i0+2;
				p->p3=i0+1;
				p++;
				p->p1=i0+0;
				p->p2=i0+3;
				p->p3=i0+2;
				p++;
			}
		}else
		{
			for(int i=0;i<box_face;i++)
			{
				int i0=i*4;
				p->p1=i0+0;
				p->p2=i0+1;
				p->p3=i0+2;
				p++;
				p->p1=i0+0;
				p->p2=i0+2;
				p->p3=i0+3;
				p++;
			}
		}
		
		gb_RenderDevice3D->UnlockIndexBuffer(ib);
	}

	gb_RenderDevice3D->CreateVertexBuffer(vb,num_vertex,CubeVector::fmt,false);
	CubeVector* v=(CubeVector*)gb_RenderDevice3D->LockVertexBuffer(vb);

	float dx=sz_rect.x,dy=sz_rect.y,dz=sz_rect.z;
	if(fov90)
	{
		//X0
		v->pos.set( dx,-dy,-dz);v->t.set( 1, 0); v++;
		v->pos.set( dx,-dy, dz);v->t.set( 1, 1); v++;
		v->pos.set(-dx,-dy, dz);v->t.set( 0, 1); v++;
		v->pos.set(-dx,-dy,-dz);v->t.set( 0, 0); v++;
		//X1
		v->pos.set( dx, dy,-dz);v->t.set( 0, 0); v++;
		v->pos.set(-dx, dy,-dz);v->t.set( 1, 0); v++;
		v->pos.set(-dx, dy, dz);v->t.set( 1, 1); v++;
		v->pos.set( dx, dy, dz);v->t.set( 0, 1); v++;
		//Y0
		v->pos.set(-dx, dy,-dz);v->t.set( 0, 0); v++;
		v->pos.set(-dx,-dy,-dz);v->t.set( 1, 0); v++;
		v->pos.set(-dx,-dy, dz);v->t.set( 1, 1); v++;
		v->pos.set(-dx, dy, dz);v->t.set( 0, 1); v++;
		//Y1
		v->pos.set( dx, dy,-dz);v->t.set( 1, 0); v++;
		v->pos.set( dx, dy, dz);v->t.set( 1, 1); v++;
		v->pos.set( dx,-dy, dz);v->t.set( 0, 1); v++;
		v->pos.set( dx,-dy,-dz);v->t.set( 0, 0); v++;
		//Z0
		v->pos.set( dx, dy, dz);v->t.set( 1, 1); v++;
		v->pos.set(-dx, dy, dz);v->t.set( 0, 1); v++;
		v->pos.set(-dx,-dy, dz);v->t.set( 0, 0); v++;
		v->pos.set( dx,-dy, dz);v->t.set( 1, 0); v++;
		//Z1
		v->pos.set( dx, dy,-dz);v->t.set( 1, 0); v++;
		v->pos.set( dx,-dy,-dz);v->t.set( 1, 1); v++;
		v->pos.set(-dx,-dy,-dz);v->t.set( 0, 1); v++;
		v->pos.set(-dx, dy,-dz);v->t.set( 0, 0); v++;
	}else
	{
/*
		//X0
		v->pos.set( dx,-dy,-dz);v->t.set( 1, 1); v++;
		v->pos.set( dx,-dy, dz);v->t.set( 1, 0); v++;
		v->pos.set(-dx,-dy, dz);v->t.set( 0, 0); v++;
		v->pos.set(-dx,-dy,-dz);v->t.set( 0, 1); v++;
		//X1
		v->pos.set( dx, dy,-dz);v->t.set( 0, 1); v++;
		v->pos.set(-dx, dy,-dz);v->t.set( 1, 1); v++;
		v->pos.set(-dx, dy, dz);v->t.set( 1, 0); v++;
		v->pos.set( dx, dy, dz);v->t.set( 0, 0); v++;
		//Y0
		v->pos.set(-dx, dy,-dz);v->t.set( 0, 1); v++;
		v->pos.set(-dx,-dy,-dz);v->t.set( 1, 1); v++;
		v->pos.set(-dx,-dy, dz);v->t.set( 1, 0); v++;
		v->pos.set(-dx, dy, dz);v->t.set( 0, 0); v++;
		//Y1
		v->pos.set( dx, dy,-dz);v->t.set( 1, 1); v++;
		v->pos.set( dx, dy, dz);v->t.set( 1, 0); v++;
		v->pos.set( dx,-dy, dz);v->t.set( 0, 0); v++;
		v->pos.set( dx,-dy,-dz);v->t.set( 0, 1); v++;
		//Z0
		v->pos.set( dx, dy,-dz);v->t.set( 1, 1); v++;
		v->pos.set( dx,-dy,-dz);v->t.set( 1, 0); v++;
		v->pos.set(-dx,-dy,-dz);v->t.set( 0, 0); v++;
		v->pos.set(-dx, dy,-dz);v->t.set( 0, 1); v++;
		//Z1
		v->pos.set( dx, dy, dz);v->t.set( 1, 0); v++;
		v->pos.set(-dx, dy, dz);v->t.set( 0, 0); v++;
		v->pos.set(-dx,-dy, dz);v->t.set( 0, 1); v++;
		v->pos.set( dx,-dy, dz);v->t.set( 1, 1); v++;
*/
		//X0
		v->pos.set( dx,-dy,  0);v->t.set( 1,0.5f); v++;
		v->pos.set( dx,-dy, dz);v->t.set( 1, 0); v++;
		v->pos.set(-dx,-dy, dz);v->t.set( 0, 0); v++;
		v->pos.set(-dx,-dy,  0);v->t.set( 0,0.5f); v++;
		//X1
		v->pos.set( dx, dy,  0);v->t.set( 0,0.5f); v++;
		v->pos.set(-dx, dy,  0);v->t.set( 1,0.5f); v++;
		v->pos.set(-dx, dy, dz);v->t.set( 1, 0); v++;
		v->pos.set( dx, dy, dz);v->t.set( 0, 0); v++;
		//Y0
		v->pos.set(-dx, dy,  0);v->t.set( 0,0.5f); v++;
		v->pos.set(-dx,-dy,  0);v->t.set( 1,0.5f); v++;
		v->pos.set(-dx,-dy, dz);v->t.set( 1, 0); v++;
		v->pos.set(-dx, dy, dz);v->t.set( 0, 0); v++;
		//Y1
		v->pos.set( dx, dy,  0);v->t.set( 1,0.5f); v++;
		v->pos.set( dx, dy, dz);v->t.set( 1, 0); v++;
		v->pos.set( dx,-dy, dz);v->t.set( 0, 0); v++;
		v->pos.set( dx,-dy,  0);v->t.set( 0,0.5f); v++;
		//Z0
		v->pos.set( dx, dy,-dz);v->t.set( 1, 1); v++;
		v->pos.set( dx,-dy,-dz);v->t.set( 1, 0); v++;
		v->pos.set(-dx,-dy,-dz);v->t.set( 0, 0); v++;
		v->pos.set(-dx, dy,-dz);v->t.set( 0, 1); v++;
		//Z1
		v->pos.set( dx, dy, dz);v->t.set( 1, 0); v++;
		v->pos.set(-dx, dy, dz);v->t.set( 0, 0); v++;
		v->pos.set(-dx,-dy, dz);v->t.set( 0, 1); v++;
		v->pos.set( dx,-dy, dz);v->t.set( 1, 1); v++;
	}

	gb_RenderDevice3D->UnlockVertexBuffer(vb);
}


///////////////////////////////////////////////
#include "ObjLibrary.h"
#include "ObjMesh.h"
CSkySpere::CSkySpere(cObjLibrary* pObj,LPCSTR str_name,LPCSTR str_texture,int h_size)
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
	cD3DRender* rd=DrawNode->GetRenderDevice3D();
	DWORD zfunc=rd->GetRenderState(D3DRS_ZFUNC);
	DWORD zwriteenable=rd->GetRenderState(D3DRS_ZWRITEENABLE);
	BOOL fog=rd->GetRenderState(D3DRS_FOGENABLE);
	rd->SetRenderState(D3DRS_FOGENABLE, FALSE);
	rd->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
	rd->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	Vect2f zplane=DrawNode->GetZPlane();
	DrawNode->SetZPlaneTemp(Vect2f(zplane.x,zplane.y*10));
	rd->SetDrawNode(DrawNode);

	vector<cObjMesh*>& meshes=pSkySphere->GetMeshChild();
	vector<cObjMesh*>::iterator it;

	FOR_EACH(meshes,it)
		(*it)->Draw(DrawNode);

	rd->SetRenderState(D3DRS_FOGENABLE, fog);
	rd->SetRenderState(D3DRS_ZFUNC, zfunc);
	rd->SetRenderState(D3DRS_ZWRITEENABLE, zwriteenable);
	DrawNode->SetZPlaneTemp(zplane);
	rd->SetDrawNode(DrawNode);

}
