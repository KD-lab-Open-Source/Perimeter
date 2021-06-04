#include "StdAfxRD.h"

#include "Scene.h"
#include "ObjNode.h"
#include "ObjMesh.h"
#include "MeshTri.h"
#include "TileMap.h"

#include "AnimChannel.h"
#include "Font.h"

#include "Silicon.h"

inline void xFormPoint(D3DMATRIX &m,const Vect3f &in,Vect4f &out)
{
	out.x=m._11*in.x+m._21*in.y+m._31*in.z+m._41;
	out.y=m._12*in.x+m._22*in.y+m._32*in.z+m._42;
	out.z=m._13*in.x+m._23*in.y+m._33*in.z+m._43;
	out.w=m._14*in.x+m._24*in.y+m._34*in.z+m._44;
	if(out.w!=0) out.w=1/out.w; else out.w=1e-30f;
	out.x*=out.w; out.y*=out.w; out.z*=out.w;
}

void cD3DRender::SetRenderTarget(cTexture* target,LPDIRECT3DSURFACE9 pZBuffer)
{
	for( int nPasses=0; nPasses<nSupportTexture; nPasses++ ) 
	{
		lpD3DDevice->SetTexture( nPasses, CurrentTexture[nPasses]=0 );
	}

	LPDIRECT3DSURFACE9 lpDDSurface=0;
	RDCALL((target->GetDDSurface(0))->GetSurfaceLevel(0,&lpDDSurface));
	RDCALL(lpD3DDevice->SetRenderTarget(0,lpDDSurface));
	RDCALL(lpD3DDevice->SetDepthStencilSurface(pZBuffer));

	if(!pZBuffer)
	{
		SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
		SetRenderState( D3DRS_ZWRITEENABLE, FALSE ); 
	}

	lpDDSurface->Release();
}

void cD3DRender::RestoreRenderTarget()
{
	RDCALL(lpD3DDevice->SetRenderTarget(0,lpBackBuffer));
	RDCALL(lpD3DDevice->SetDepthStencilSurface(lpZBuffer));
	SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );
	SetRenderState( D3DRS_ZWRITEENABLE, TRUE ); 
}

void cD3DRender::SetDrawNode(cCamera *pDrawNode)
{
	if((DrawNode=pDrawNode)==0||lpD3DDevice==0) return;
	if(DrawNode->GetRenderTarget())
	{
		LPDIRECT3DSURFACE9 pZBuffer=DrawNode->GetZBuffer();
		SetRenderTarget(DrawNode->GetRenderTarget(),pZBuffer);
		DWORD color=0;
		if(pDrawNode->GetAttribute(ATTRCAMERA_SHADOW))
		{
			if(Option_ShadowType==SHADOW_MAP_SELF && 
				(dtAdvance->GetID()==DT_RADEON8500 ||
				dtAdvance->GetID()==DT_RADEON9700
				))
			{
				color=D3DCOLOR_RGBA(0,0,0,255);
				kShadow=0.25f;
			}else
			if(CurrentMod4==D3DTOP_MODULATE4X)
			{
				color=D3DCOLOR_RGBA(63,63,63,255);
				kShadow=0.25f;
			}
			else if(CurrentMod4==D3DTOP_MODULATE2X)
			{
				color=D3DCOLOR_RGBA(127,127,127,255);
				kShadow=0.5f;
			}else
			{
				color=D3DCOLOR_RGBA(255,255,255,255);
				kShadow=1.f;
			}
		}else
		{
//			color=D3DCOLOR_RGBA(63,63,63,255);
//			kShadow=0.25f;
			color=D3DCOLOR_RGBA(255,255,255,255);
			kShadow=1.3f;
		}

		if(!pDrawNode->GetAttribute(ATTRCAMERA_NOCLEARTARGET))
		{
			if(!pZBuffer)
			{
				RDCALL(lpD3DDevice->Clear(0,NULL,D3DCLEAR_TARGET,color,1,0));
			}else
			{
				RDCALL(lpD3DDevice->Clear(0,NULL,D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, color, 
					pDrawNode->GetAttribute(ATTRCAMERA_ZINVERT)?0:1, 0));
			}
		}
	}
	else
	{
		RestoreRenderTarget();
	}

	SetDrawTransform(pDrawNode);
}

void cD3DRender::SetDrawTransform(class cCamera *pDrawNode)
{
	RDCALL(lpD3DDevice->SetTransform(D3DTS_PROJECTION,(D3DXMATRIX*)&pDrawNode->matProj));
	RDCALL(lpD3DDevice->SetTransform(D3DTS_VIEW,(D3DXMATRIX*)&pDrawNode->matView));
	RDCALL(lpD3DDevice->SetViewport((D3DVIEWPORT9*)&pDrawNode->vp));
	if(pDrawNode->GetAttribute(ATTRCAMERA_REFLECTION)==0)
		SetRenderState(D3DRS_CULLMODE,CurrentCullMode=D3DCULL_CW);	// прямое изображение
	else
		SetRenderState(D3DRS_CULLMODE,CurrentCullMode=D3DCULL_CCW);	// отраженное изображение
}

void cD3DRender::DrawNoMaterial(cObjMesh *Mesh,sDataRenderMaterial *Data)
{
	SetMatrix(D3DTS_WORLD,Mesh->GetGlobalMatrix());
	if(Data->mat&MAT_TEXMATRIX_STAGE1)
	{
		D3DXMATRIX mat;
		MatXf &m=Data->TexMatrix;
		memset(mat,0,sizeof(mat));
		mat._11 = m.rot()[0][0],	mat._12 = m.rot()[0][1];
		mat._21 = m.rot()[1][0],	mat._22 = m.rot()[1][1];
		mat._31 = m.trans().x,		mat._32 = m.trans().y;
		RDCALL(lpD3DDevice->SetTransform(D3DTS_TEXTURE0,&mat));
	}

	if(Data->mat&MAT_RENDER_SPHEREMAP)
	{ // сферический мапинг
		D3DXMATRIX mat;
		memset(mat,0,sizeof(mat));
		mat._11=mat._22=mat._41=mat._42=0.5f;
		RDCALL(lpD3DDevice->SetTransform( D3DTS_TEXTURE1, &mat ));
	}

	if(Option_DrawMeshScreen)
		DrawNoMaterialShadowNoWorld(Mesh);

}

void cD3DRender::DrawNoMaterialShadow(cObjMesh *Mesh)
{
	SetMatrix(D3DTS_WORLD,Mesh->GetGlobalMatrix());
	DrawNoMaterialShadowNoWorld(Mesh);
}

void cD3DRender::DrawNoMaterialShadowNoWorld(cObjMesh *Mesh)
{
	cMeshTri *Tri=Mesh->GetTri();
	RDCALL(lpD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
		0,
		Tri->GetOffsetVertex(),Tri->GetNumberVertex(),
		3*Tri->GetOffsetPolygon(),Tri->GetNumberPolygon()));

	NumberPolygon+=Tri->GetNumberPolygon();
	NumDrawObject++;
}

void cD3DRender::DrawBound(const MatXf &Matrix,Vect3f &min,Vect3f &max,bool wireframe,sColor4c &Color)
{ 
	VISASSERT(DrawNode);
	int BytePerVertex=8*sizeof(sVertexXYZD);
	int BytePerPolygon=12*sizeof(sPolygon);
	VISASSERT((BytePerVertex+BytePerPolygon)<Buffer.length());
	sVertexXYZD *v=(sVertexXYZD*)&Buffer[0];
	sColor4c diffuse((150*Color.r)>>8,(155*Color.g)>>8,(155*Color.b)>>8,100);
	v[0].pos.set(min.x,min.y,min.z);
	v[1].pos.set(max.x,min.y,min.z);
	v[2].pos.set(min.x,max.y,min.z);
	v[3].pos.set(max.x,max.y,min.z);
	v[4].pos.set(min.x,min.y,max.z);
	v[5].pos.set(max.x,min.y,max.z);
	v[6].pos.set(min.x,max.y,max.z);
	v[7].pos.set(max.x,max.y,max.z);
	v[0].diffuse=diffuse;
	v[1].diffuse=diffuse;
	v[2].diffuse=diffuse;
	v[3].diffuse=diffuse;
	v[4].diffuse=diffuse;
	v[5].diffuse=diffuse;
	v[6].diffuse=diffuse;
	v[7].diffuse=diffuse;
	sPolygon *p=(sPolygon*)&Buffer[BytePerVertex];
	p[0].p1=1, p[0].p2=2, p[0].p3=0;
	p[1].p1=1, p[1].p2=3, p[1].p3=2;
	p[2].p1=4, p[2].p2=6, p[2].p3=5;
	p[3].p1=6, p[3].p2=7, p[3].p3=5;
	p[4].p1=0, p[4].p2=6, p[4].p3=4;
	p[5].p1=0, p[5].p2=2, p[5].p3=6;
	p[6].p1=0, p[6].p2=4, p[6].p3=5;
	p[7].p1=0, p[7].p2=5, p[7].p3=1;
	p[8].p1=1, p[8].p2=7, p[8].p3=3;
	p[9].p1=1, p[9].p2=5, p[9].p3=7;
	p[10].p1=2, p[10].p2=3, p[10].p3=7;
	p[11].p1=2, p[11].p2=7, p[11].p3=6;
	SetFVF(v->fmt);
	SetRenderState( RS_ZWRITEENABLE, FALSE );
	if(wireframe) SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);
	SetMatrix(D3DTS_WORLD,Matrix);
	SetNoMaterial(ALPHA_BLEND);

	RDCALL(lpD3DDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,0,8,12,
		p,D3DFMT_INDEX16,v,sizeof(v[0])));
	
	if(wireframe) SetRenderState(D3DRS_FILLMODE,bWireFrame==0?D3DFILL_SOLID:D3DFILL_WIREFRAME);
	SetRenderState( RS_ZWRITEENABLE, TRUE );
}

#undef FromHex

inline int FromHex(char a)
{
	if(a>='0' && a<='9')
		return a-'0';
	if(a>='A' && a<='F')
		return a-'A'+10;
	if(a>='a' && a<='f')
		return a-'a'+10;
	return -1;
}

/*
	Синтаксис строки 
	string &FEAB89 string && fdsfsdgs
	&FEAB89 - меняет цвет символа
	&& - преобразуется к &
*/
/*
void cD3DRender::ChangeTextColor(const char* &str,sColor4c& diffuse)
{
	if(*str!='&')return;
	if(str[1]=='&')
	{
		str++;
		return;
	}

	DWORD s=0;
	for(int i=1;i<=6;i++)
	{
		int a=FromHex(str[i]);
		if(a<0)return;
		s=(s<<4)+a;
	}

	diffuse.RGBA()&=0xFF000000;
	diffuse.RGBA()|=s;
	str+=i;
}
*/
void cD3DRender::ChangeTextColor(const char* &str,sColor4c& diffuse)
{
	while(*str=='&')
	{
		if(str[1]=='&')
		{
			str++;
			return;
		}

		DWORD s=0;
		int i;
		for(i=1;i<=6;i++)
		{
			int a=FromHex(str[i]);
			if(a<0)return;
			s=(s<<4)+a;
		}

		diffuse.RGBA()&=0xFF000000;
		diffuse.RGBA()|=s;
		str+=i;
	}
}

float cD3DRender::GetFontLength(const char *string)
{ 
	cFontInternal* cf=CurrentFont->GetInternal();
	float xOfs=0;
	float xSize = CurrentFont->GetScale().x*(1<<CurrentFont->GetTexture()->GetX());
	sColor4c diffuse(0,0,0,0);

	for(const char* str=string;*str;str++)
	{
		ChangeTextColor(str,diffuse);
		BYTE c=(unsigned char)*str;
		if(!c || c==10)break;
		if(c<32)continue;
		xOfs+=xSize*cf->Font[c].z-1;
	}

	return xOfs; 
}

float cD3DRender::GetCharLength(const char c)
{
	char str[2];
	str[0]=c;
	str[1]=0;
	return GetFontLength(str);
}

void cD3DRender::OutTextRect(int x,int y,const char *string,int align,Vect2f& bmin,Vect2f& bmax)
{
	bmin.set(x,y);
	bmax.set(x,y);
	if(CurrentFont==0)
	{
		VISASSERT(0 && "Font not set");
		return;
	}

	float xOfs=(float)x, yOfs=(float)y;
	cFontInternal* cf=CurrentFont->GetInternal();
	sColor4c diffuse(0,0,0,0);

	float xSize = CurrentFont->GetScale().x*(1<<cf->GetTexture()->GetX()),
		ySize = CurrentFont->GetScale().y*cf->FontHeight*(1<<cf->GetTexture()->GetY());

	float v_add=cf->FontHeight+1/(double)(1<<cf->GetTexture()->GetY());
	int nSymbol=0;
	for( const char* str=string; *str; str++, yOfs+=ySize)
	{
		xOfs=(float)x;
		if( align>=0 )
		{
			float StringWidth = GetFontLength( str );
			if( align==0 )
				xOfs -= round(StringWidth*0.5f);
			else
				xOfs -= round(StringWidth);
		}
		for( ; *str!=10 ; str++ )
		{
			ChangeTextColor(str,diffuse);
			if(*str==10)
				break;

			int c=(unsigned char)*str;
			if( !c ) goto LABEL_DRAW;
			if(c<32)continue;
			Vect3f& size=cf->Font[c];

			bmin.x=min(xOfs,bmin.x);
			bmin.y=min(yOfs,bmin.y);
			xOfs+=xSize*size.z-1;
			bmax.x=max(xOfs,bmax.x);
			bmax.y=max(yOfs+ySize,bmax.y);

			nSymbol++;
		}
	}
LABEL_DRAW:

	return;
}

void cD3DRender::OutText(int x,int y,const char *string,const sColor4f& color,int align,eBlendMode blend_mode)
{
	if(CurrentFont==0)
	{
		VISASSERT(CurrentFont!=0);
		return;
	}
	
	float xOfs=(float)x, yOfs=(float)y;
	sColor4c diffuse(color);
	cFontInternal* cf=CurrentFont->GetInternal();

/*
	SetNoMaterial(blend_mode,0,CurrentFont->GetTexture());
/*/
	SetTexture(CurrentFont->GetTexture(),0,0);
	SetTexture(NULL,0,1);
	SetTexture(NULL,0,2);
	SetBlendState(blend_mode);

	SetRenderState(D3DRS_SPECULARENABLE,FALSE);
	SetRenderState(D3DRS_LIGHTING,FALSE);
	SetRenderState(D3DRS_NORMALIZENORMALS,FALSE);


	SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );

	SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
	SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
	SetTextureStageState( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	SetTextureStageState( 2, D3DTSS_COLOROP, D3DTOP_DISABLE );

/**/

	if(CurrentFont->GetScale().x>1.01f || CurrentFont->GetScale().y>1.01f)
		SetRenderState(RS_BILINEAR,1);
	else
		SetRenderState(RS_BILINEAR,0);

	float xSize = CurrentFont->GetScale().x*cf->GetTexture()->GetWidth(),
		  ySize = CurrentFont->GetScale().y*cf->FontHeight*cf->GetTexture()->GetHeight();

	float v_add=cf->FontHeight+1/(double)(1<<cf->GetTexture()->GetY());

	QuadBufferXYZWDT1.BeginDraw();

	for( const char* str=string; *str; str++, yOfs+=ySize)
	{
		xOfs=(float)x;
		if( align>=0 )
		{
			float StringWidth = GetFontLength( str );
			if( align==0 )
				xOfs -= round(StringWidth*0.5f);
			else
				xOfs -= round(StringWidth);
		}
		for( ; *str!=10 ; str++ )
		{
			ChangeTextColor(str,diffuse);
			if(*str==10)
				break;

			int c=(unsigned char)*str;
			if( !c ) goto LABEL_DRAW;
			if(c<32)continue;
			Vect3f& size=cf->Font[c];

			sVertexXYZWDT1* v=QuadBufferXYZWDT1.Get();
			sVertexXYZWDT1 &v1=v[1],&v2=v[0],
						   &v3=v[2],&v4=v[3];

			v1.x=v4.x=xOfs;
			v1.y=v2.y=yOfs;
			xOfs+=xSize*size.z-1;
			v3.x=v2.x=xOfs;
			v3.y=v4.y=yOfs+ySize;
			v1.u1()=v4.u1()=size.x;
			v1.v1()=v2.v1()=size.y;
			v3.u1()=v2.u1()=v1.u1()+size.z;
			v3.v1()=v4.v1()=v1.v1()+v_add;
			v1.diffuse=v2.diffuse=v3.diffuse=v4.diffuse=diffuse;
			v1.z=v2.z=v3.z=v4.z=0.001f;
			v1.w=v2.w=v3.w=v4.w=0.001f;
		}
	}
LABEL_DRAW:

	QuadBufferXYZWDT1.EndDraw();
}

void cD3DRender::OutText(int x,int y,const char *string,const sColor4f& color,int align,eBlendMode blend_mode,
			cTexture* pTexture,eColorMode mode,Vect2f uv,Vect2f duv,float phase,float lerp_factor)
{
	VISASSERT(pTexture);
	if(CurrentFont==0)
	{
		VISASSERT(CurrentFont!=0);
		return;
	}
	
	duv.x*=1024.0f/GetSizeX();
	duv.y*=768.0f/GetSizeY();
	float xOfs=(float)x, yOfs=(float)y;
	sColor4c diffuse(color);
	sColor4c lerp(255*lerp_factor,255*lerp_factor,255*lerp_factor,255*(1-lerp_factor));
	cFontInternal* cf=CurrentFont->GetInternal();

//	SetNoMaterial(blend_mode,phase,CurrentFont->GetTexture(),pTexture,mode);
	SetNoMaterial(blend_mode,phase,pTexture,CurrentFont->GetTexture(),mode);
//	SetNoMaterial(blend_mode,phase,pTexture);

	DWORD index1=GetTextureStageState(1,D3DTSS_TEXCOORDINDEX);
	SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,1);
	SetTextureStageState(1,D3DTSS_TEXCOORDINDEX,0);
	SetRenderState(RS_BILINEAR,0);

	{
		SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATECOLOR_ADDALPHA);
		SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TFACTOR);
		SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_TEXTURE);
		SetRenderState(D3DRS_TEXTUREFACTOR,lerp.RGBA());

		SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
		SetTextureStageState(1,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
		SetTextureStageState(1,D3DTSS_ALPHAARG2,D3DTA_DIFFUSE);

//		SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_MODULATE);
		SetTextureStageState(1,D3DTSS_COLORARG1,D3DTA_DIFFUSE);
		SetTextureStageState(1,D3DTSS_COLORARG2,D3DTA_CURRENT);
//	   DWORD dwNumPasses;
//	   RDCALL(lpD3DDevice->ValidateDevice( &dwNumPasses ));
	}


	float xSize = CurrentFont->GetScale().x*cf->GetTexture()->GetWidth(),
		  ySize = CurrentFont->GetScale().y*cf->FontHeight*cf->GetTexture()->GetHeight();

	float v_add=cf->FontHeight+1/(double)(1<<cf->GetTexture()->GetY());

	QuadBufferXYZWDT2.BeginDraw();

	for( const char* str=string; *str; str++, yOfs+=ySize)
	{
		xOfs=(float)x;
		if( align>=0 )
		{
			float StringWidth = GetFontLength( str );
			if( align==0 )
				xOfs -= round(StringWidth*0.5f);
			else
				xOfs -= round(StringWidth);
		}
		for( ; *str!=10 ; str++ )
		{
			ChangeTextColor(str,diffuse);

			int c=(unsigned char)*str;
			if( !c ) goto LABEL_DRAW;
			if(c==10)break;
			if(c<32)continue;
			Vect3f& size=cf->Font[c];

			sVertexXYZWDT2* v=QuadBufferXYZWDT2.Get();
			sVertexXYZWDT2 &v1=v[1],&v2=v[0],
						   &v3=v[2],&v4=v[3];

			float x0,x1,y0,y1;
			x0=xOfs;
			x1=xOfs+xSize*size.z-1;
			y0=yOfs;
			y1=yOfs+ySize;
			v1.x=v4.x=x0;
			v1.y=v2.y=y0;
			v3.x=v2.x=x1;
			v3.y=v4.y=y1;
			v1.u1()=v4.u1()=size.x;
			v1.v1()=v2.v1()=size.y;
			v3.u1()=v2.u1()=v1.u1()+size.z;
			v3.v1()=v4.v1()=v1.v1()+v_add;
			v1.diffuse=v2.diffuse=v3.diffuse=v4.diffuse=diffuse;
			v1.z=v2.z=v3.z=v4.z=0.001f;
			v1.w=v2.w=v3.w=v4.w=0.001f;

			v1.u2()=v4.u2()=(x0-x)*duv.x+uv.x;
			v1.v2()=v2.v2()=(y0-y)*duv.y+uv.y;
			v3.u2()=v2.u2()=(x1-x)*duv.x+uv.x;
			v3.v2()=v4.v2()=(y1-y)*duv.y+uv.y;

			xOfs=x1;
		}
	}
LABEL_DRAW:

	QuadBufferXYZWDT2.EndDraw();
	SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,0);
	SetTextureStageState(1,D3DTSS_TEXCOORDINDEX,index1);

	SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
	SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

	SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
	SetTextureStageState( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	SetTextureStageState( 1, D3DTSS_COLORARG2, D3DTA_CURRENT );

	SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	SetTextureStageState( 1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	SetTextureStageState( 1, D3DTSS_ALPHAARG2, D3DTA_CURRENT );
}

struct LightByTexture
{
	cTexture* texture;
	vector<cUnkLight*> light;
};

void cD3DRender::Draw(class cScene *Scene)
{ 
	vector<LightByTexture> light;
	for(int i=0;i<Scene->GetNumberLight();i++)
	{
		cUnkLight* ULight=Scene->GetLight(i);
		if(ULight && ULight->GetAttr(ATTRLIGHT_SPHERICAL))
		{
			cTexture* pTexture=ULight->GetTexture();

			LightByTexture* pl=NULL;
			for(int j=0;j<light.size();j++)
			if(light[j].texture==pTexture)
			{
				pl=&light[j];
				break;
			}
			
			if(!pl)
			{
				light.push_back(LightByTexture());
				pl=&light.back();
				pl->texture=pTexture;
			}

			pl->light.push_back(ULight);
		}
	}

	if(light.empty())
		return;

	SetNoMaterial(ALPHA_ADDBLEND);
	cQuadBuffer<sVertexXYZDT1>* quad=GetQuadBufferXYZDT1();

	int i;
	for(i=0;i<light.size();i++)
	{
		LightByTexture& pl=light[i];
		quad->BeginDraw();
		SetTexture(pl.texture,0,0);
		
		for(int j=0;j<pl.light.size();j++)
		{
			cUnkLight* ULight=pl.light[j];
			//sColor4c Diffuse(255,255,255,255);//Непонятно почему была такая строчка
			sColor4c Diffuse(ULight->GetDiffuse()); 
			sVertexXYZDT1 *v=quad->Get();
			Vect3f& p=ULight->GetPos();
			float r=ULight->GetRadius();
			v[0].pos.x=p.x-r; v[0].pos.y=p.y-r; v[0].pos.z=p.z; v[0].u1()=0; v[0].v1()=0;
			v[1].pos.x=p.x-r; v[1].pos.y=p.y+r; v[1].pos.z=p.z; v[1].u1()=0; v[1].v1()=1;
			v[2].pos.x=p.x+r; v[2].pos.y=p.y-r; v[2].pos.z=p.z; v[2].u1()=1; v[2].v1()=0;
			v[3].pos.x=p.x+r; v[3].pos.y=p.y+r; v[3].pos.z=p.z; v[3].u1()=1; v[3].v1()=1;
			
			v[0].diffuse=v[1].diffuse=v[2].diffuse=v[3].diffuse=Diffuse;
		}
		quad->EndDraw();
	}

}

void cD3DRender::Draw(class ElasticSphere *es)
{
	SetMatrix(D3DTS_WORLD,es->GetGlobalMatrix());
	
	sVertexXYZDT2 *VertexFix=(sVertexXYZDT2*)&Buffer[0];
	int is=es->theta_size,js=es->psi_size;
	int BytePerVertex=((is+1)*(js+1)+(is+2)*(js-1)-1)*sizeof(VertexFix[0]);
	VISASSERT(BytePerVertex<Buffer.length());
	Vect3f uv[2];
	Mat3f &mC=DrawNode->GetMatrix().rot();
	uv[0].set(	0.5f*mC[0][0],0.5f*mC[0][1],0.5f*mC[0][2]);
	uv[1].set(	0.5f*mC[1][0],0.5f*mC[1][1],0.5f*mC[1][2]);
	VISASSERT(es->GetTexture(0));
	SetNoMaterial(es->blendMode,es->GetFrame()->GetPhase(),es->GetTexture(0),es->GetTexture(1),COLOR_MOD);
	SetFVF(VertexFix->fmt);

	int cull=GetRenderState(D3DRS_CULLMODE);
	SetRenderState(D3DRS_CULLMODE,D3DCULL_CW);

	sColor4c Diffuse(es->GetDiffuse());
	float dv=0.5f-es->GetFrame()->GetPhase();
	int i;
	for(i=0;i<=is;i++)
	{
		sVertexXYZDT2 &v=VertexFix[2*i];
		const Vect3f &n=es->normal(0,i);
		v.pos=es->point(0,i);
		v.GetTexel().set(0/(float)es->psi_size,i/(float)es->theta_size);
		v.GetTexel2().set(n.y*0.5f+0.5f,n.z*0.5f+dv);
		v.diffuse=Diffuse;
	}
	int j;
	for(j=0;j<js;j++)
		if(j&1)
			for(i=0;i<=is;i++)
			{
				sVertexXYZDT2 &v=VertexFix[(2*is+3)*j+2*i+1];
				const Vect3f &n=es->normal(j+1,is-i);
				v.pos=es->point(j+1,is-i);
				v.GetTexel().set((j+1)/(float)es->psi_size,(is-i)/(float)es->theta_size);
				v.GetTexel2().set(n.y*0.5f+0.5f,n.z*0.5f+dv);
				v.diffuse=Diffuse;
			}
		else
			for(i=0;i<=is;i++)
			{
				sVertexXYZDT2 &v=VertexFix[(2*is+3)*j+2*i+1];
				const Vect3f &n=es->normal(j+1,i);
				v.pos=es->point(j+1,i);
				v.GetTexel().set((j+1)/(float)es->psi_size,i/(float)es->theta_size);
				v.GetTexel2().set(n.y*0.5f+0.5f,n.z*0.5f+dv);
				v.diffuse=Diffuse;
			}
	for(j=0;j<js-1;j++)
		if(j&1)
		{
			for(i=0;i<=is;i++)
				VertexFix[(2*is+3)*(j+1)+2*(is-i)+0]=VertexFix[(2*is+3)*j+2*i+1];
			VertexFix[(2*is+3)*j+2*is+2]=VertexFix[(2*is+3)*j+2*is+1];
		}
		else
		{
			for(i=0;i<=is;i++)
				VertexFix[(2*is+3)*(j+1)+2*(is-i)+0]=VertexFix[(2*is+3)*j+2*i+1];
			VertexFix[(2*is+3)*j+2*is+2]=VertexFix[(2*is+3)*j+2*is+1];
		}
	DrawPrimitiveUP(D3DPT_TRIANGLESTRIP,(is+1)*(js+1)+(is+2)*(js-1)-1-2,VertexFix,sizeof(VertexFix[0]));
	NumberPolygon+=(is+1)*(js+1)+(is+2)*(js-1)-1-2;
	NumDrawObject++;

	SetRenderState(D3DRS_CULLMODE,cull);
}
