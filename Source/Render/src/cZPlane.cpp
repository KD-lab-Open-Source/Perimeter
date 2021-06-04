#include "StdAfxRD.h"
#include "cZPlane.h"

cZPlane::cZPlane()
:cIUnkObjScale(KIND_NULL)
{

	Frame0.Set(1200);
	Frame1.Set(5000);
	Frame0s.Set(4000);
	Frame1s.Set(5000);

	pTex0=pTex1=NULL;
	texturek0=texturek1=1;
	stage2op=D3DTOP_ADDSIGNED;

	speed0x=speed0y=1.0f;
}

void cZPlane::SetTexture(cTexture* _pTex0,cTexture* _pTex1,float k0,float k1,int op)
{
	pTex0=_pTex0;
	pTex1=_pTex1;
	texturek0=k0;
	texturek1=k1;
	stage2op=op;
}

void cZPlane::SetSpeed(int n,float sx,float sy)
{
	if(n==0)
	{
		speed0x=sx;
		speed0y=sy;
	}
	if(n==1)
	{
		speed1x=sx;
		speed1y=sy;
	}
}

cZPlane::~cZPlane()
{
	if(pTex0)pTex0->Release();
	if(pTex1)pTex1->Release();
}

void cZPlane::Animate(float dt)
{
	Frame0.AddPhase(dt);
	Frame1.AddPhase(dt);
	Frame0s.AddPhase(dt);
	Frame1s.AddPhase(dt);
}

void cZPlane::PreDraw(cCamera *DrawNode)
{
	if(DrawNode->GetParent()==NULL)
		DrawNode->AttachFirst(this);
}

void cZPlane::Draw(cCamera *DrawNode)
{
	cD3DRender* rd=DrawNode->GetRenderDevice3D();
	sVertexXYZDT2 *v=(sVertexXYZDT2*)rd->GetStripBuffer();

	const int nvertex=4;
	for(int i=0;i<nvertex;i++)
	{
		v[i].diffuse.set(255,255,255,255);
	}

	int border=1024;
	int xmin=-border,xmax=GetScale().x+border,
		ymin=-border,ymax=GetScale().y+border;
	
	int s0=texturek0*(xmax-xmin)/256,s1=texturek1*(xmax-xmin)/256;

	v[0].pos.set(xmin,ymin,0); v[0].uv[0]=0; v[0].uv[1]=0;  v[0].uv2[0]=0; v[0].uv2[1]=0;
	v[1].pos.set(xmin,ymax,0); v[1].uv[0]=0; v[1].uv[1]=s0; v[1].uv2[0]=0; v[1].uv2[1]=s1;
	v[2].pos.set(xmax,ymin,0); v[2].uv[0]=s0;v[2].uv[1]=0;  v[2].uv2[0]=s1;v[2].uv2[1]=0;
	v[3].pos.set(xmax,ymax,0); v[3].uv[0]=s0;v[3].uv[1]=s0; v[3].uv2[0]=s1;v[3].uv2[1]=s1;

	rd->SetNoMaterial(ALPHA_NONE);

	float phase0=Frame0.GetPhase();
	float phase1=Frame1.GetPhase();
	rd->SetTexture(pTex0,phase0,0);
	rd->SetTexture(pTex1,phase1,1);

	rd->SetRenderState(D3DRS_ALPHATESTENABLE,FALSE);
	rd->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);
	rd->SetRenderState(D3DRS_SPECULARENABLE,FALSE);
	rd->SetRenderState(D3DRS_LIGHTING,FALSE);
	rd->SetRenderState(D3DRS_ZWRITEENABLE,TRUE);
	rd->SetRenderState(D3DRS_NORMALIZENORMALS,FALSE);

	rd->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	rd->SetTextureStageState( 1, D3DTSS_COLOROP, (D3DTEXTUREOP)stage2op);
	rd->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0);
	rd->SetTextureStageState( 1, D3DTSS_TEXCOORDINDEX, 1);

	rd->SetSamplerState( 0, D3DSAMP_ADDRESSU , D3DTADDRESS_MIRROR);
	rd->SetSamplerState( 0, D3DSAMP_ADDRESSV , D3DTADDRESS_MIRROR);
	rd->SetSamplerState( 1, D3DSAMP_ADDRESSU , D3DTADDRESS_WRAP );
	rd->SetSamplerState( 1, D3DSAMP_ADDRESSV , D3DTADDRESS_WRAP );

	{
		float a=Frame0s.GetPhase();

		D3DXMATRIX mat;
		memset(mat,0,sizeof(mat));
		mat._11=mat._22=mat._33=mat._44=1;

		mat._31 = sfmod(speed0x*a,s0);
		mat._32 = sfmod(speed0y*a,s0);
		
		RDCALL(rd->lpD3DDevice->SetTransform(D3DTS_TEXTURE0,&mat));
		rd->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
	}

	{
		float a=Frame1s.GetPhase();

		D3DXMATRIX mat;
		memset(mat,0,sizeof(mat));
		mat._11=mat._22=mat._33=mat._44=1;

		mat._31 = sfmod(speed1x*a,s0);
		mat._32 = sfmod(speed1y*a,s0);
		
		RDCALL(rd->lpD3DDevice->SetTransform(D3DTS_TEXTURE1,&mat));
		rd->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
	}

	rd->SetFVF(v->fmt);
	rd->SetMatrix(D3DTS_WORLD,GetGlobalMatrix());
	rd->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,nvertex-2,rd->GetStripBuffer(),sizeof(v[0]));

	rd->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
	rd->SetTextureStageState( 1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

	rd->SetSamplerState( 0, D3DSAMP_ADDRESSU , D3DTADDRESS_WRAP);
	rd->SetSamplerState( 0, D3DSAMP_ADDRESSV , D3DTADDRESS_WRAP);
}

//////////////////////////cPlane//////////////////////////////

cPlane::cPlane()
:cUnkObj(KIND_NULL)
{
	umin=vmin=0;
	umax=vmax=1;
}

Vect3f cPlane::GetCenterObject()
{
	return GetGlobalMatrix().trans()+GetScale()*0.5f;
}

void cPlane::PreDraw(cCamera *DrawNode)
{
	if(GetAttribute(ATTRUNKOBJ_IGNORE)==0)
	{
		if(GetTexture() && GetTexture()->IsAlpha())
			DrawNode->Attach(SCENENODE_OBJECTSORT,this);
		else
			DrawNode->Attach(SCENENODE_OBJECT,this);
	}
}

void cPlane::Draw(cCamera *DrawNode)
{
	cD3DRender* rd=(cD3DRender*)DrawNode->GetRenderDevice();
	cVertexBuffer<sVertexXYZDT1>* buf=gb_RenderDevice->GetBufferXYZDT1();

	sVertexXYZDT1* vertex=buf->Lock(4);
	vertex[0].pos.set(0,0,0);
	vertex[0].diffuse.RGBA()=0xFFFFFFFF;
	vertex[0].uv[0]=umin;
	vertex[0].uv[1]=vmin;

	vertex[1].pos.set(0,GetScale().y,0);
	vertex[1].diffuse.RGBA()=0xFFFFFFFF;
	vertex[1].uv[0]=umin;
	vertex[1].uv[1]=vmax;

	vertex[2].pos.set(GetScale().x,0,0);
	vertex[2].diffuse.RGBA()=0xFFFFFFFF;
	vertex[2].uv[0]=umax;
	vertex[2].uv[1]=vmin;

	vertex[3].pos.set(GetScale().x,GetScale().y,0);
	vertex[3].diffuse.RGBA()=0xFFFFFFFF;
	vertex[3].uv[0]=umax;
	vertex[3].uv[1]=vmax;
	buf->Unlock(4);

	rd->SetRenderState(RS_BILINEAR,0);
	rd->SetNoMaterial(ALPHA_NONE,0,GetTexture());

	buf->DrawPrimitive(PT_TRIANGLESTRIP,2,GetGlobalMatrix());
	rd->SetRenderState(RS_BILINEAR,1);
}

void cPlane::SetUV(float _umin,float _vmin,float _umax,float _vmax)
{
	umin=_umin;
	vmin=_vmin;
	umax=_umax;
	vmax=_vmax;
}
